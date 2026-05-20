# ChatForum 功能与代码一一对应关系

> 本文档列出面试文档和项目介绍中提到的所有功能模块、技术点，以及对应的具体代码文件和位置。

---

## 一、技术栈对照

| 面试文档声称的技术栈 | C++ 后端代码 | Python 后端代码 |
|---|---|---|
| C++17 | `backend/` 全部代码，CMakeLists.txt 设置 CMAKE_CXX_STANDARD 17 | — |
| Drogon 框架 | `backend/main.cpp`, 所有 Controller/WebSocket 继承 Drogon 基类 | — |
| FastAPI 框架 | — | `python_backend/main.py` |
| jwt-cpp | `backend/utils/JwtUtil.h` + `JwtUtil.cpp` | — |
| PyJWT | — | `python_backend/utils/auth.py` |
| BCrypt (C++) | `backend/services/UserService.cpp` hashPassword/verifyPassword | — |
| bcrypt (Python) | — | `python_backend/routers/auth.py` |
| JsonCpp | `backend/models/*.cpp` toJson(), `backend/websocket/ChatWebSocketController.cpp` | — |
| PyMySQL | — | `python_backend/database.py` |
| MySQL 8.x | `backend/services/*.cpp` execSqlAsync, `config/config.json.example` | `python_backend/database.py` |
| CMake | `backend/CMakeLists.txt`, 根目录 `CMakeLists.txt` | — |
| WebSocket | `backend/websocket/ChatWebSocketController.h/cpp` | `python_backend/routers/chat.py` |
| Vue 3 + TypeScript | `frontend/src/` | `frontend/src/` |

---

## 二、功能模块对照

### 2.1 用户认证模块

| 功能点 | 面试文档描述 | C++ 代码位置 | Python 代码位置 |
|---|---|---|---|
| 用户注册 | POST /api/auth/register，校验→查重→插入→签发Token | `backend/controllers/UserController.cpp` registerUser → `backend/services/UserService.cpp` registerUser | `python_backend/routers/auth.py` register |
| 用户登录 | POST /api/auth/login，查用户→验密码→签发Token，失败统一返回"用户名或密码错误" | `backend/services/UserService.cpp` login | `python_backend/routers/auth.py` login |
| JWT签发 | Builder模式 jwt::create().set_issuer().set_payload_claim().sign(hs256) | `backend/utils/JwtUtil.cpp` generateToken | `python_backend/utils/auth.py` create_token |
| JWT验证 | Verifier模式 jwt::verify().allow_algorithm(hs256).with_issuer() | `backend/utils/JwtUtil.cpp` verifyToken | `python_backend/utils/auth.py` decode_token |
| 从Token提取userId | 先验证再提取，保证签名有效 | `backend/utils/JwtUtil.cpp` getUserIdFromToken | `python_backend/utils/auth.py` decode_token |
| BCrypt密码哈希 | 自动管理盐，输出$2b$格式 | `backend/services/UserService.cpp` hashPassword/verifyPassword | `python_backend/routers/auth.py` bcrypt.hashpw/checkpw |
| 认证中间件 | CRTP HttpMiddleware，提取Bearer Token→验证→setAttribute注入userId→放行 | `backend/utils/AuthMiddleware.h` + `AuthMiddleware.cpp` | `python_backend/main.py` auth_middleware |
| WebSocket认证 | URL参数?token=兼容Authorization Header | `backend/websocket/ChatWebSocketController.cpp` authenticateConnection | `python_backend/routers/chat.py` websocket_chat |
| JWT密钥管理 | 从config.json读取，不硬编码 | `backend/utils/ConfigUtil.cpp` getJwtSecret | `python_backend/utils/auth.py` JWT_SECRET（硬编码） |

### 2.2 帖子模块

| 功能点 | 面试文档描述 | C++ 代码位置 | Python 代码位置 |
|---|---|---|---|
| 发帖 | POST /api/posts，multipart表单，支持多图上传 | `backend/controllers/PostController.cpp` createPost → `backend/services/PostService.cpp` createPost | `python_backend/routers/post.py` create_post |
| 帖子列表 | GET /api/posts，两步异步：先查总数再查列表，EXISTS子查询is_liked，content超100字截断 | `backend/services/PostService.cpp` getPostList | `python_backend/routers/post.py` get_post_list |
| 帖子详情 | GET /api/posts/{id}，EXISTS子查询is_liked | `backend/services/PostService.cpp` getPostById | `python_backend/routers/post.py` get_post_by_id |
| 图片上传 | 扩展名白名单校验，唯一文件名，大小限制 | `backend/services/FileService.cpp` saveUploadFile | `python_backend/utils/file_util.py` |

### 2.3 评论模块

| 功能点 | 面试文档描述 | C++ 代码位置 | Python 代码位置 |
|---|---|---|---|
| 发表评论 | POST /api/posts/{id}/comments，INSERT→并行：UPDATE计数+SELECT详情 | `backend/controllers/CommentController.cpp` createComment → `backend/services/CommentService.cpp` createComment | `python_backend/routers/comment.py` create_comment |
| 评论列表 | GET /api/posts/{id}/comments，分页 | `backend/services/CommentService.cpp` getComments | `python_backend/routers/comment.py` get_comments |
| 嵌套评论 | parent_comment_id支持回复 | `backend/models/Comment.h` _parentCommentId (optional) | `python_backend/routers/comment.py` CommentReq.parent_comment_id |

### 2.4 点赞模块

| 功能点 | 面试文档描述 | C++ 代码位置 | Python 代码位置 |
|---|---|---|---|
| 点赞切换 | POST /api/posts/{id}/like，4层回调：SELECT→DELETE/INSERT→UPDATE计数→SELECT最新计数 | `backend/controllers/LikeController.cpp` toggleLike → `backend/services/LikeService.cpp` toggleLike | `python_backend/routers/like.py` toggle_like |
| 防负数 | GREATEST(like_count-1, 0) | `backend/services/LikeService.cpp` | `python_backend/routers/like.py` |
| 点赞用户列表 | GET /api/posts/{id}/like/users | — | `python_backend/routers/like.py` get_like_users |

### 2.5 私聊模块

| 功能点 | 面试文档描述 | C++ 代码位置 | Python 代码位置 |
|---|---|---|---|
| WebSocket连接 | /ws/chat?token=xxx，认证→setContext绑定userId→加入映射→广播在线 | `backend/websocket/ChatWebSocketController.cpp` handleNewConnection | `python_backend/routers/chat.py` websocket_chat |
| 消息处理 | 解析JSON→ping/pong→message类型→入库→查发送者信息→推送双方 | `backend/websocket/ChatWebSocketController.cpp` handleNewMessage | `python_backend/routers/chat.py` websocket_chat |
| 连接关闭 | getContext→unique_lock擦除→广播离线 | `backend/websocket/ChatWebSocketController.cpp` handleConnectionClosed | `python_backend/routers/chat.py` WebSocketDisconnect |
| 在线状态广播 | shared_lock遍历_connections，跳过自己 | `backend/websocket/ChatWebSocketController.cpp` broadcastOnlineStatus | `python_backend/routers/chat.py` broadcast_online_status |
| 定向推送 | shared_lock查找userId→send | `backend/websocket/ChatWebSocketController.cpp` sendMessageToUser | `python_backend/routers/chat.py` send_to_user |
| 会话列表 | GET /api/chat/conversations，子查询聚合最近消息+未读计数 | `backend/controllers/ChatController.cpp` → `backend/services/ChatService.cpp` getConversations | `python_backend/routers/chat.py` get_conversations |
| 聊天记录 | GET /api/chat/messages/{userId}，分页 | `backend/services/ChatService.cpp` getMessages | `python_backend/routers/chat.py` get_messages |
| 标记已读 | PUT /api/chat/read/{userId} | `backend/services/ChatService.cpp` markAsRead | `python_backend/routers/chat.py` mark_as_read |
| REST发送 | POST /api/chat/send | `backend/controllers/ChatController.cpp` sendMessage | `python_backend/routers/chat.py` send_message |

### 2.6 通知模块

| 功能点 | 面试文档描述 | C++ 代码位置 | Python 代码位置 |
|---|---|---|---|
| 通知列表 | GET /api/notifications | — | `python_backend/routers/notification.py` get_notifications |
| 未读数 | GET /api/notifications/unread_count | — | `python_backend/routers/notification.py` get_unread_count |
| 标记已读 | PUT /api/notifications/read/{id} | — | `python_backend/routers/notification.py` mark_notification_read |
| 全部已读 | PUT /api/notifications/read_all | — | `python_backend/routers/notification.py` mark_all_read |
| 创建通知 | 评论/点赞/私信时自动创建 | — | `python_backend/utils/notification.py` create_notification |

---

## 三、C++ 核心技术点对照

### 3.1 Drogon 框架（面试教学1）

| 技术点 | 面试文档描述 | 代码位置 |
|---|---|---|
| CRTP控制器 | HttpController<T> 编译期确定路由表 | `backend/controllers/UserController.h` 等5个Controller |
| METHOD_LIST宏 | ADD_METHOD_TO 声明路由映射 | `backend/controllers/UserController.h` L12-L18 |
| 路径参数{1} | 自动绑定到函数参数 | `backend/controllers/UserController.h` getUserById, `backend/controllers/LikeController.h` toggleLike |
| 认证中间件 | HttpMiddleware<AuthMiddleware> CRTP，invoke中拦截/放行 | `backend/utils/AuthMiddleware.h` + `AuthMiddleware.cpp` |
| setAttribute/getAttribute | 中间件注入userId，Controller读取 | `backend/utils/AuthMiddleware.cpp` setAttribute, `backend/controllers/UserController.cpp` getAttribute |
| CORS全局处理 | registerPreRoutingAdvice拦截OPTIONS返回204，registerPostHandlingAdvice注入CORS头 | `backend/main.cpp` L38-L67 |
| Controller→Service分层 | Controller只提取参数调用Service | 所有Controller的每个方法 |

### 3.2 异步ORM与回调（面试教学2）

| 技术点 | 面试文档描述 | 代码位置 |
|---|---|---|
| execSqlAsync | 全部DB操作异步执行 | `backend/services/*.cpp` 所有方法 |
| 参数化查询 | ?占位防SQL注入 | `backend/services/UserService.cpp` registerUser等 |
| 两层异步回调 | 注册：SELECT查重→INSERT写入 | `backend/services/UserService.cpp` registerUser |
| 4层回调嵌套 | 点赞切换：SELECT→DELETE/INSERT→UPDATE→SELECT | `backend/services/LikeService.cpp` toggleLike |
| 移动捕获 | callback = std::move(callback) | 所有Service中lambda捕获列表 |
| mutable关键字 | 允许修改捕获的callback | 所有Service中lambda声明 |
| 并行异步 | 评论创建后并行UPDATE计数+SELECT详情 | `backend/services/CommentService.cpp` createComment |
| 两步异步分页 | 先查总数再查列表 | `backend/services/PostService.cpp` getPostList |
| EXISTS子查询 | 列表查询同时返回is_liked | `backend/services/PostService.cpp` getPostList |

### 3.3 WebSocket与线程安全（面试教学3）

| 技术点 | 面试文档描述 | 代码位置 |
|---|---|---|
| WebSocketController CRTP | WebSocketController<T> 三个生命周期回调 | `backend/websocket/ChatWebSocketController.h` |
| static连接映射 | unordered_map<int64_t, WebSocketConnectionPtr> | `backend/websocket/ChatWebSocketController.h` _connections |
| shared_mutex读写锁 | 读多写少场景优化 | `backend/websocket/ChatWebSocketController.h` _mutex |
| shared_lock读操作 | 广播在线状态、定向推送 | `backend/websocket/ChatWebSocketController.cpp` broadcastOnlineStatus, sendMessageToUser |
| unique_lock写操作 | 连接建立、连接关闭 | `backend/websocket/ChatWebSocketController.cpp` handleNewConnection, handleConnectionClosed |
| setContext绑定身份 | make_shared<int64_t>(userId) | `backend/websocket/ChatWebSocketController.cpp` handleNewConnection |
| getContext取身份 | getContext<int64_t>() | `backend/websocket/ChatWebSocketController.cpp` handleNewMessage |
| RAII锁管理 | 构造加锁，析构解锁 | 所有shared_lock/unique_lock使用处 |
| static成员类外初始化 | .cpp文件中定义 | `backend/websocket/ChatWebSocketController.cpp` L9-L10 |

### 3.4 JWT认证（面试教学4）

| 技术点 | 面试文档描述 | 代码位置 |
|---|---|---|
| Builder模式签发 | jwt::create().set_issuer().set_payload_claim().sign() | `backend/utils/JwtUtil.cpp` generateToken |
| Verifier模式验证 | jwt::verify().allow_algorithm().with_issuer() | `backend/utils/JwtUtil.cpp` verifyToken |
| allow_algorithm白名单 | 防算法混淆攻击 | `backend/utils/JwtUtil.cpp` verifyToken L26 |
| HS256对称加密 | 单体应用场景 | `backend/utils/JwtUtil.cpp` L18, L26 |
| BCrypt密码存储 | generateHash/validatePassword | `backend/services/UserService.cpp` hashPassword/verifyPassword |
| WebSocket双通道认证 | URL参数+Header兼容 | `backend/websocket/ChatWebSocketController.cpp` authenticateConnection |
| 密钥从配置读取 | getCustomConfig()["jwt"]["secret"] | `backend/utils/ConfigUtil.cpp` getJwtSecret |

---

## 四、工程设计对照

| 设计点 | 面试文档描述 | C++ 代码位置 | Python 代码位置 |
|---|---|---|---|
| MVC分层 | Controller→Service→Model | controllers/ → services/ → models/ | routers/ → (内联在router中) → database.py |
| 统一响应 | {code, message, data} | `backend/utils/ResponseUtil.h/cpp` | 每个router直接返回dict |
| 输入校验前置 | ValidationUtil统一校验 | `backend/utils/ValidationUtil.h/cpp` | 每个router内联校验 |
| 配置外置 | config.json读取JWT密钥 | `backend/utils/ConfigUtil.h/cpp`, `config/config.json.example` | `python_backend/utils/auth.py`（硬编码） |
| 文件上传安全 | 扩展名白名单+大小限制+唯一文件名 | `backend/services/FileService.h/cpp` | `python_backend/utils/file_util.py` |

---

## 五、项目文件完整清单

### C++ Drogon 后端（`backend/`）

```
backend/
├── CMakeLists.txt
├── main.cpp
├── controllers/
│   ├── UserController.h          # 用户控制器（注册/登录/资料）
│   ├── UserController.cpp
│   ├── PostController.h          # 帖子控制器（发帖/列表/详情）
│   ├── PostController.cpp
│   ├── CommentController.h       # 评论控制器（发表/列表）
│   ├── CommentController.cpp
│   ├── LikeController.h          # 点赞控制器（切换点赞）
│   ├── LikeController.cpp
│   ├── ChatController.h          # 聊天控制器（会话/消息/已读/发送）
│   └── ChatController.cpp
├── models/
│   ├── User.h                    # 用户ORM模型
│   ├── User.cpp
│   ├── Post.h                    # 帖子ORM模型
│   ├── Post.cpp
│   ├── Comment.h                 # 评论ORM模型
│   ├── Comment.cpp
│   ├── Like.h                    # 点赞ORM模型
│   ├── Like.cpp
│   ├── Message.h                 # 消息ORM模型
│   └── Message.cpp
├── services/
│   ├── UserService.h             # 用户业务逻辑
│   ├── UserService.cpp
│   ├── PostService.h             # 帖子业务逻辑
│   ├── PostService.cpp
│   ├── CommentService.h          # 评论业务逻辑
│   ├── CommentService.cpp
│   ├── LikeService.h             # 点赞业务逻辑
│   ├── LikeService.cpp
│   ├── ChatService.h             # 聊天业务逻辑
│   ├── ChatService.cpp
│   ├── FileService.h             # 文件上传服务
│   └── FileService.cpp
├── utils/
│   ├── JwtUtil.h                 # JWT工具（Builder/Verifier）
│   ├── JwtUtil.cpp
│   ├── AuthMiddleware.h          # 认证中间件（CRTP）
│   ├── AuthMiddleware.cpp
│   ├── ResponseUtil.h            # 统一响应工具
│   ├── ResponseUtil.cpp
│   ├── ValidationUtil.h          # 输入校验工具
│   ├── ValidationUtil.cpp
│   ├── ConfigUtil.h              # 配置读取工具
│   └── ConfigUtil.cpp
└── websocket/
    ├── ChatWebSocketController.h # WebSocket控制器（CRTP+读写锁）
    └── ChatWebSocketController.cpp
```

### Python FastAPI 后端（`python_backend/`）

```
python_backend/
├── main.py                       # 程序入口，中间件，路由注册
├── database.py                   # 数据库连接
├── requirements.txt              # Python依赖
├── routers/
│   ├── auth.py                   # 认证接口
│   ├── user.py                   # 用户接口
│   ├── post.py                   # 帖子接口
│   ├── comment.py                # 评论接口
│   ├── like.py                   # 点赞接口
│   ├── chat.py                   # 聊天接口+WebSocket
│   └── notification.py           # 通知接口
└── utils/
    ├── auth.py                   # JWT工具
    ├── middleware.py              # 认证中间件辅助
    ├── file_util.py              # 文件上传工具
    └── notification.py           # 通知创建工具
```

---

## 六、两套后端功能覆盖对比

| 功能 | C++ Drogon | Python FastAPI |
|---|:---:|:---:|
| 用户注册/登录 | ✅ | ✅ |
| JWT认证中间件 | ✅ | ✅ |
| 用户资料查看/修改 | ✅ | ✅ |
| 查看他人信息 | ✅ | ✅ |
| 发帖（文字+图片） | ✅ | ✅ |
| 帖子列表（分页+搜索） | ✅ | ✅ |
| 帖子详情 | ✅ | ✅ |
| 发表评论 | ✅ | ✅ |
| 嵌套评论（回复） | ✅ | ✅ |
| 评论列表 | ✅ | ✅ |
| 点赞/取消点赞 | ✅ | ✅ |
| 点赞用户列表 | ❌ | ✅ |
| WebSocket私聊 | ✅ | ✅ |
| 在线状态广播 | ✅ | ✅ |
| 心跳机制 | ✅ | ✅ |
| 会话列表 | ✅ | ✅ |
| 聊天记录 | ✅ | ✅ |
| 标记已读 | ✅ | ✅ |
| REST消息发送 | ✅ | ✅ |
| 通知系统 | ❌ | ✅ |
| 文件上传 | ✅ | ✅ |
| CORS处理 | ✅ | ✅ |
| 统一响应格式 | ✅ | ✅ |
| 输入校验 | ✅ | ✅ |
| 配置外置 | ✅ | ✅（部分硬编码） |
| MVC分层 | ✅（Controller/Service/Model） | ✅（Router/内联/Database） |
| 异步DB操作 | ✅（execSqlAsync） | ✅（同步PyMySQL） |
| 读写锁线程安全 | ✅（shared_mutex） | ❌（单线程asyncio） |
| 连接池 | ✅（Drogon内置） | ❌（每次新建连接） |
