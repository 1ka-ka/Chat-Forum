# ChatForum - 聊天论坛系统

基于 B/S 架构的聊天论坛系统，支持用户注册登录、发帖（文字+图片）、浏览帖子、评论、点赞、私聊等核心功能。

## 技术栈

### 后端
- **C++17** - 核心开发语言
- **Drogon** - 高性能 HTTP/WebSocket Web 框架
- **MySQL 8.x** - 关系型数据库
- **jwt-cpp** - JWT Token 认证
- **libmysql** - MySQL 客户端驱动

### 前端
- **Vue 3** - 前端框架 (Composition API)
- **TypeScript** - 类型安全
- **Vite** - 构建工具
- **Vue Router** - 路由管理
- **Pinia** - 状态管理
- **Element Plus** - UI 组件库

## 功能特性

| 模块 | 功能 |
|------|------|
| 用户模块 | 注册、登录、登出、个人信息编辑（头像、昵称） |
| 帖子模块 | 发帖（支持文字+图片上传）、帖子列表查询（分页）、帖子详情查看 |
| 评论模块 | 对帖子发表评论、查看帖子评论列表 |
| 点赞模块 | 对帖子点赞/取消点赞、显示帖子点赞数 |
| 私聊模块 | 一对一实时私聊（WebSocket）、聊天历史记录查询、在线用户状态 |

## 项目结构

```
ChatForum/
├── CMakeLists.txt              # 根 CMake 配置
├── config/
│   └── config.json             # Drogon 配置文件
├── sql/
│   ├── schema.sql              # 数据库建表语句
│   └── init_data.sql           # 初始测试数据
├── backend/
│   ├── main.cpp                # 程序入口
│   ├── controllers/            # HTTP 控制器
│   ├── services/               # 业务逻辑层
│   ├── models/                 # 数据模型
│   ├── utils/                  # 工具类
│   └── websocket/              # WebSocket 控制器
├── frontend/                   # Vue 3 前端项目
│   ├── src/
│   │   ├── api/               # API 接口封装
│   │   ├── components/        # 公共组件
│   │   ├── views/             # 页面视图
│   │   ├── stores/            # Pinia 状态管理
│   │   ├── router/            # 路由配置
│   │   └── utils/             # 工具函数
│   └── ...
├── uploads/                    # 用户上传文件存储
│   ├── images/                # 帖子图片
│   └── avatars/               # 用户头像
└── logs/                       # 日志目录
```

## 环境要求

### 后端
- C++17 兼容编译器 (Visual Studio 2022 / GCC 11+ / Clang 15+)
- CMake 3.16+
- MySQL 8.0+
- vcpkg (用于依赖管理)

### 前端
- Node.js 18+
- npm 9+

## 快速开始

### 1. 数据库配置

创建数据库并导入初始化脚本：

```bash
mysql -u root -p < sql/schema.sql
mysql -u root -p < sql/init_data.sql
```

修改 `config/config.json` 中的数据库连接信息：

```json
{
  "db_clients": [{
    "host": "127.0.0.1",
    "port": 3306,
    "dbname": "chatforum",
    "user": "root",
    "passwd": "your_password"
  }]
}
```

### 2. 安装后端依赖

```bash
# 安装 vcpkg (如果尚未安装)
git clone https://github.com/microsoft/vcpkg.git C:\vcpkg
cd C:\vcpkg
.\bootstrap-vcpkg.bat

# 安装依赖
vcpkg install drogon:x64-windows jwt-cpp:x64-windows libmysql:x64-windows
```

### 3. 构建后端

```bash
cd ChatForum
mkdir build && cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=C:\vcpkg\scripts\buildsystems\vcpkg.cmake
cmake --build . --config Release
```

### 4. 安装前端依赖

```bash
cd frontend
npm install
```

### 5. 启动项目

**启动后端：**
```bash
# 在 build 目录
./ChatForum ../config/config.json
```

**启动前端开发服务器：**
```bash
cd frontend
npm run dev
```

或使用运行脚本：
```bash
.\start.bat
```

访问 http://localhost:5173 即可使用。

## 测试账号

| 用户名 | 密码 | 昵称 |
|--------|------|------|
| alice | 123456 | 爱丽丝 |
| bob | 123456 | 鲍勃 |
| charlie | 123456 | 查理 |

## API 接口

基础 URL: `http://localhost:8080/api`

### 认证接口

| 方法 | 路径 | 说明 |
|------|------|------|
| POST | /api/auth/register | 用户注册 |
| POST | /api/auth/login | 用户登录 |

### 用户接口

| 方法 | 路径 | 说明 | 认证 |
|------|------|------|------|
| GET | /api/user/profile | 获取当前用户信息 | ✓ |
| PUT | /api/user/profile | 更新个人信息 | ✓ |
| GET | /api/user/{id} | 获取指定用户信息 | |

### 帖子接口

| 方法 | 路径 | 说明 | 认证 |
|------|------|------|------|
| POST | /api/posts | 发帖 | ✓ |
| GET | /api/posts | 获取帖子列表 | |
| GET | /api/posts/{id} | 获取帖子详情 | |

### 评论接口

| 方法 | 路径 | 说明 | 认证 |
|------|------|------|------|
| POST | /api/posts/{id}/comments | 发表评论 | ✓ |
| GET | /api/posts/{id}/comments | 获取评论列表 | |

### 点赞接口

| 方法 | 路径 | 说明 | 认证 |
|------|------|------|------|
| POST | /api/posts/{id}/like | 点赞/取消点赞 | ✓ |

### 私聊接口

| 方法 | 路径 | 说明 | 认证 |
|------|------|------|------|
| GET | /api/chat/conversations | 获取会话列表 | ✓ |
| GET | /api/chat/messages/{userId} | 获取聊天记录 | ✓ |
| PUT | /api/chat/read/{userId} | 标记已读 | ✓ |

### WebSocket

连接地址: `ws://localhost:8080/ws/chat?token=<jwt_token>`

## 统一响应格式

```json
{
  "code": 200,
  "message": "success",
  "data": {}
}
```

| code | 含义 |
|------|------|
| 200 | 成功 |
| 400 | 请求参数错误 |
| 401 | 未认证/Token失效 |
| 404 | 资源不存在 |
| 409 | 资源冲突 |
| 500 | 服务器内部错误 |

## 开发说明

### 添加新功能

1. **后端**：在 `backend/` 目录下添加相应的 Controller、Service、Model
2. **前端**：在 `frontend/src/` 目录下添加相应的 View、Component、API

### 代码规范

- 后端使用 C++17 标准，遵循 Drogon 框架规范
- 前端使用 Vue 3 Composition API + TypeScript
- 所有 API 接口需实现参数校验和错误处理

## License

MIT License
