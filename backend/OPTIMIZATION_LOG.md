# Backend 代码优化记录

本次优化目标：让 `backend/` 中的 C++ Drogon 后端代码真正体现教学文档和项目介绍中声称的技术栈使用和设计思想。

---

## 优化1 [严重]：AuthMiddleware 挂载到路由

### 问题

`main.cpp` 中注册了中间件 `app.registerMiddleware<AuthMiddleware>("auth")`，但所有 Controller 的 `ADD_METHOD_TO` 宏都没有引用 `"auth"`，导致中间件从未生效。所有需要认证的接口（getProfile、createPost、toggleLike 等）的 `req->getAttribute<int64_t>("userId")` 调用实际上拿不到有效值。

### 修复

在需要认证的路由的 `ADD_METHOD_TO` 宏中添加 `"auth"` 参数，不需要认证的路由保持不变：

| Controller | 路由 | 修改 |
|------------|------|------|
| UserController | `/api/user/profile` GET | 添加 `"auth"` |
| UserController | `/api/user/profile` PUT | 添加 `"auth"` |
| PostController | `/api/posts` POST | 添加 `"auth"` |
| CommentController | `/api/posts/{1}/comments` POST | 添加 `"auth"` |
| LikeController | `/api/posts/{1}/like` POST | 添加 `"auth"` |
| ChatController | 全部4个路由 | 添加 `"auth"` |
| NotificationController | 全部4个路由 | 添加 `"auth"` |

**不需要认证的路由**（保持不变）：
- `/api/auth/register` POST — 注册
- `/api/auth/login` POST — 登录
- `/api/user/{1}` GET — 查看他人资料
- `/api/posts` GET — 浏览帖子列表
- `/api/posts/{1}` GET — 查看帖子详情
- `/api/posts/{1}/comments` GET — 查看评论
- `/api/posts/{1}/like/users` GET — 查看点赞用户

### 涉及文件

- `backend/controllers/UserController.h`
- `backend/controllers/PostController.h`
- `backend/controllers/CommentController.h`
- `backend/controllers/LikeController.h`
- `backend/controllers/ChatController.h`
- `backend/controllers/NotificationController.h`

### 教学意义

修复后，项目真正体现了 Drogon 中间件的 AOP（面向切面编程）设计：
- `registerMiddleware` 注册中间件到框架
- `ADD_METHOD_TO` 的第5个参数指定中间件名，实现路由级别的精细控制
- 这和 Python FastAPI 的全局中间件 `@app.middleware("http")` 形成对比——Drogon 是显式声明式，FastAPI 是全局拦截+白名单放行

---

## 优化2 [严重]：CommentService callback 所有权 bug

### 问题

`CommentService::createComment` 中，INSERT 成功后并行发起 UPDATE 和 SELECT 两个异步操作。原代码中 SELECT 的成功回调和错误回调都通过 `callback = std::move(callback)` 移动捕获了同一个 callback 变量。由于 `std::function` 只能被移动一次，两个 lambda 中只有一个能获得有效的 callback，另一个获得的是 moved-from 的空对象——这是未定义行为。

### 修复

- UPDATE 回调改为空 lambda，不捕获 callback（它是"发射后不管"的操作）
- SELECT 的成功回调通过移动捕获持有 callback 的唯一所有权
- SELECT 的错误回调改为空 lambda（callback 已被移动到成功回调中）
- 外层 errorCallback 也改为空 lambda（callback 已被 insertCallback 移走）

### 涉及文件

- `backend/services/CommentService.cpp`

### 教学意义

修复后，代码正确体现了 C++ 异步编程中 `std::function` 移动语义的关键约束：
- callback 只能被移动一次，所有权必须明确
- "发射后不管"的操作不应捕获 callback
- 面试中可以讨论用 `shared_ptr` 包装 callback 让多个回调共享所有权的改进方案

---

## 优化3 [中等]：FileService 添加文件大小限制

### 问题

项目介绍中声称"限制文件大小并校验扩展名白名单"，但 `FileService.cpp` 中只有扩展名校验和空文件检查，没有文件大小限制。

### 修复

在 `FileService::saveUploadFile` 中添加 5MB 文件大小限制：

```cpp
const size_t MAX_FILE_SIZE = 5 * 1024 * 1024;
if (fileContent.size() > MAX_FILE_SIZE) {
    callback(ResponseUtil::badRequest("文件大小不能超过5MB"));
    return;
}
```

### 涉及文件

- `backend/services/FileService.cpp`

---

## 文档更新

以下教学文档中的过时/错误描述已同步更新：

| 文档 | 修改内容 |
|------|---------|
| `04-JWT-Authentication.md` | 补充3：从"getJwtSecret 代码重复"改为"ConfigUtil 统一配置管理"，反映代码已通过 ConfigUtil 消除重复 |
| `04-JWT-Authentication.md` | 第7节密钥管理：更新代码示例，展示 AuthMiddleware 委托给 ConfigUtil |
| `04-JWT-Authentication.md` | 第6节 WebSocket 认证：`getJwtSecret()` 改为 `ConfigUtil::getJwtSecret()` |
| `03-WebSocket-Thread-Safety.md` | 补充4：从"并行异步巧妙设计"改为"callback 所有权"，修正对 CommentService 的错误解释 |
