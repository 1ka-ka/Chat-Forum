# ChatForum - 聊天论坛系统

一个基于 C++ Drogon + Vue 3 的现代化聊天论坛系统，支持发帖、评论、点赞和实时私聊功能。

## 技术栈

### 后端
- C++17
- Drogon Web 框架
- MySQL 8.0+
- jwt-cpp (JWT 认证)
- libbcrypt (密码加密)

### 前端
- Vue 3 + TypeScript
- Vite
- Element Plus
- Pinia (状态管理)
- Vue Router
- Axios

## 功能特性

- [x] 用户注册/登录
- [x] 个人信息管理（头像、昵称）
- [x] 发布帖子（支持文字+图片）
- [x] 帖子列表/详情查看
- [x] 评论功能
- [x] 点赞功能
- [x] 实时私聊（WebSocket）
- [x] 会话列表和聊天历史
- [x] 用户在线状态
- [x] 消息已读/未读

## 项目结构

```
ChatForum/
├── backend/              # 后端代码
│   ├── controllers/      # 控制器
│   ├── models/           # 数据模型
│   ├── services/         # 业务逻辑
│   ├── utils/            # 工具类
│   ├── websocket/        # WebSocket
│   ├── main.cpp          # 入口文件
│   └── CMakeLists.txt
├── frontend/             # 前端代码
│   ├── src/
│   │   ├── components/   # 组件
│   │   ├── views/        # 页面
│   │   ├── api/          # API
│   │   ├── stores/       # 状态管理
│   │   └── router/       # 路由
│   ├── package.json
│   └── vite.config.ts
├── config/               # 配置文件
├── sql/                  # 数据库脚本
├── uploads/              # 上传文件
├── logs/                 # 日志
└── CMakeLists.txt
```

## 快速开始

### 环境要求

- Windows 10/11
- Visual Studio 2022 (包含 CMake)
- MySQL 8.0+
- Node.js 16+
- Git

### 1. 克隆项目

```bash
git clone https://github.com/1ka-ka/Chat-Forum.git
cd Chat-Forum
```

### 2. 安装 vcpkg 和依赖

如果你还没有安装 vcpkg：

```bash
git clone https://github.com/microsoft/vcpkg.git C:\vcpkg
cd C:\vcpkg
.\bootstrap-vcpkg.bat
```

安装项目依赖：

```bash
C:\vcpkg\vcpkg install drogon:x64-windows jwt-cpp:x64-windows libmysql:x64-windows
```

### 3. 配置数据库

创建 MySQL 数据库并执行初始化脚本：

```bash
# 使用 MySQL 客户端
mysql -u root -p < sql/schema.sql
mysql -u root -p chatforum < sql/init_data.sql
```

### 4. 配置后端

编辑 `config/config.json`，修改数据库连接信息：

```json
{
  "db_clients": [
    {
      "name": "default",
      "rdbms": "mysql",
      "host": "127.0.0.1",
      "port": 3306,
      "dbname": "chatforum",
      "user": "root",
      "passwd": "your_password"
    }
  ],
  "jwt": {
    "secret": "change_this_to_your_secure_secret"
  }
}
```

### 5. 构建后端

```bash
mkdir build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=C:\vcpkg\scripts\buildsystems\vcpkg.cmake
cmake --build . --config Release
```

### 6. 启动后端

```bash
cd build\Release
ChatForum.exe ..\..\config\config.json
```

后端服务将在 `http://localhost:8080` 启动。

### 7. 安装前端依赖

```bash
cd frontend
npm install
```

### 8. 启动前端

```bash
# 开发模式
npm run dev

# 生产构建
npm run build
```

前端服务将在 `http://localhost:5173` 启动。

## 一键启动

项目提供了便捷的启动脚本：

```bash
# Windows 一键启动（需要先构建）
start.bat
```

## 默认账号

测试用户（密码均为 `123456`）：
- `alice` / 爱丽丝
- `bob` / 鲍勃
- `charlie` / 查理

## API 文档

### 认证接口
- `POST /api/auth/register` - 用户注册
- `POST /api/auth/login` - 用户登录

### 用户接口
- `GET /api/user/profile` - 获取当前用户信息
- `PUT /api/user/profile` - 更新用户信息
- `GET /api/user/{id}` - 获取指定用户信息

### 帖子接口
- `POST /api/posts` - 发布帖子
- `GET /api/posts` - 获取帖子列表
- `GET /api/posts/{id}` - 获取帖子详情

### 评论接口
- `POST /api/posts/{id}/comments` - 发表评论
- `GET /api/posts/{id}/comments` - 获取评论列表

### 点赞接口
- `POST /api/posts/{id}/like` - 点赞/取消点赞

### 私聊接口
- `GET /api/chat/conversations` - 获取会话列表
- `GET /api/chat/messages/{userId}` - 获取聊天记录
- `PUT /api/chat/read/{userId}` - 标记为已读

### WebSocket
- `ws://localhost:8080/ws/chat?token=xxx` - 私聊连接

## 开发说明

### 后端开发
- 使用 Drogon 框架的 Controller 和 Service 分层
- 异步编程模式
- JWT 认证 + 中间件

### 前端开发
- Vue 3 Composition API
- Pinia 状态管理
- WebSocket 封装

## 许可证

MIT

## 联系方式

如有问题，欢迎提交 Issue。
