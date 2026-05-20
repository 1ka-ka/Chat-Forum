# ChatForum - 聊天论坛系统

基于 B/S 架构的聊天论坛系统，模仿百度贴吧的基础功能，支持用户注册登录、发帖（文字+图片）、浏览帖子、评论、点赞、实时私聊、修改个人信息等核心功能。

## 技术栈

### 后端
- **Python 3.10+** — 核心开发语言
- **FastAPI** — 高性能异步 Web 框架
- **Uvicorn** — ASGI 服务器
- **MySQL 8.x** — 关系型数据库
- **PyJWT** — JWT Token 认证
- **PyMySQL** — MySQL 数据库驱动
- **bcrypt** — 密码加密
- **WebSocket** — 实时私聊通信

### 前端
- **Vue 3** — 前端框架 (Composition API)
- **TypeScript** — 类型安全
- **Vite** — 构建工具
- **Vue Router** — 路由管理
- **Pinia** — 状态管理
- **Element Plus** — UI 组件库
- **Axios** — HTTP 请求

## 功能特性

| 模块 | 功能 |
|------|------|
| 用户模块 | 注册、登录、登出、个人信息编辑（头像、昵称） |
| 帖子模块 | 发帖（支持文字+图片上传）、帖子列表查询（分页）、帖子详情查看 |
| 评论模块 | 对帖子发表评论、查看帖子评论列表 |
| 点赞模块 | 对帖子点赞/取消点赞（切换）、显示帖子点赞数 |
| 私聊模块 | 一对一实时私聊（WebSocket）、聊天历史记录查询、在线用户状态、未读消息计数 |
| 用户主页 | 查看其他用户信息、发帖数统计、快捷发私信 |

## 项目结构

```
ChatForum/
├── python_backend/               # Python FastAPI 后端
│   ├── main.py                  # 程序入口，中间件配置，WebSocket挂载
│   ├── database.py              # 数据库连接配置
│   ├── requirements.txt         # Python 依赖列表
│   ├── routers/                 # API 路由
│   │   ├── auth.py              # 认证接口（注册/登录）
│   │   ├── user.py              # 用户接口（个人信息/修改）
│   │   ├── post.py              # 帖子接口（发帖/列表/详情）
│   │   ├── comment.py           # 评论接口（发表/列表）
│   │   ├── like.py              # 点赞接口（切换点赞）
│   │   └── chat.py              # 私聊接口 + WebSocket + 消息发送
│   ├── utils/                   # 工具类
│   │   ├── auth.py              # JWT 认证工具
│   │   ├── middleware.py        # 认证中间件辅助
│   │   └── file_util.py         # 文件上传工具
├── frontend/                    # Vue 3 前端
│   ├── src/
│   │   ├── api/                 # API 接口封装
│   │   │   ├── request.ts       # Axios 实例与拦截器
│   │   │   ├── user.ts          # 用户 API
│   │   │   ├── post.ts          # 帖子 API
│   │   │   ├── comment.ts       # 评论 API
│   │   │   ├── like.ts          # 点赞 API
│   │   │   └── chat.ts          # 私聊 API
│   │   ├── components/          # 公共组件
│   │   │   ├── NavBar.vue       # 导航栏
│   │   │   ├── PostCard.vue     # 帖子卡片
│   │   │   ├── LikeButton.vue   # 点赞按钮
│   │   │   ├── CommentItem.vue  # 评论项
│   │   │   ├── ChatMessage.vue  # 聊天消息
│   │   │   ├── ImageUpload.vue  # 图片上传
│   │   │   └── UserAvatar.vue   # 用户头像
│   │   ├── views/               # 页面视图
│   │   │   ├── HomeView.vue     # 首页（帖子列表）
│   │   │   ├── LoginView.vue    # 登录页
│   │   │   ├── RegisterView.vue # 注册页
│   │   │   ├── PostDetailView.vue # 帖子详情
│   │   │   ├── CreatePostView.vue # 发帖页
│   │   │   ├── ProfileView.vue  # 个人中心
│   │   │   ├── UserView.vue     # 用户主页
│   │   │   └── ChatView.vue     # 私聊页
│   │   ├── stores/              # Pinia 状态管理
│   │   │   ├── user.ts          # 用户状态
│   │   │   └── chat.ts          # 聊天状态
│   │   ├── router/              # 路由配置
│   │   ├── types/               # TypeScript 类型定义
│   │   ├── utils/               # 工具函数
│   │   │   ├── auth.ts          # Token 管理
│   │   │   └── websocket.ts     # WebSocket 管理器
│   │   └── styles/              # 全局样式
│   ├── package.json
│   └── vite.config.ts           # Vite 配置（含开发代理）
├── sql/
│   ├── schema.sql               # 数据库建表语句
│   └── init_data.sql            # 初始测试数据
├── uploads/                     # 用户上传文件存储
│   ├── images/                  # 帖子图片
│   └── avatars/                 # 用户头像
├── logs/                        # 日志目录
├── start.bat                    # 一键启动脚本（开发环境）
├── build.bat                    # 构建脚本（生产环境）
└── README.md
```

## 环境要求

| 环境 | 版本要求 |
|------|---------|
| Python | 3.10+ |
| MySQL | 8.0+ |
| Node.js | 18+ |
| npm | 9+ |

## 快速开始

### 1. 数据库配置

确保 MySQL 已启动，然后创建数据库并导入初始化脚本：

```bash
# 登录 MySQL
mysql -u root -p

# 在 MySQL 中执行
source sql/schema.sql;
source sql/init_data.sql;
```

或者直接命令行导入：

```bash
mysql -u root -p < sql/schema.sql
mysql -u root -p < sql/init_data.sql
```

> 默认数据库配置：主机 `127.0.0.1`，端口 `3306`，用户 `root`，密码 `123456`，数据库 `chatforum`。
> 如需修改，请编辑 `python_backend/database.py` 中的 `DB_CONFIG`。

### 2. 安装后端依赖

```bash
cd python_backend

# 创建虚拟环境（推荐）
python -m venv venv

# Windows 激活虚拟环境
venv\Scripts\activate

# 安装依赖
pip install -r requirements.txt
```

### 3. 安装前端依赖

```bash
cd frontend
npm install
```

### 4. 启动项目

**方式一：一键启动（推荐）**

双击 `start.bat` 或在项目根目录执行：

```bash
.\start.bat
```

**方式二：手动启动**

终端1 — 启动后端：

```bash
cd python_backend
venv\Scripts\python.exe -m uvicorn main:app --host 0.0.0.0 --port 8080
```

终端2 — 启动前端开发服务器：

```bash
cd frontend
npm run dev
```

### 5. 访问应用

- 前端页面：http://localhost:5173
- 后端 API：http://localhost:8080
- API 文档：http://localhost:8080/docs（Swagger UI）

## 生产环境部署

```bash
# 1. 构建前端
cd frontend
npm run build

# 2. 将 dist 目录复制到后端
xcopy /E /Y /I "frontend\dist" "python_backend\frontend\dist"

# 3. 启动后端（自动托管前端静态文件）
cd python_backend
venv\Scripts\python.exe -m uvicorn main:app --host 0.0.0.0 --port 8080
```

或直接运行构建脚本：

```bash
.\build.bat
```

然后访问 http://localhost:8080 即可。

## 测试账号

| 用户名 | 密码 | 昵称 |
|--------|------|------|
| alice | 123456 | Alice |
| bob | 123456 | Bob |
| charlie | 123456 | Charlie |

## API 接口

基础 URL: `http://localhost:8080/api`

### 认证接口

| 方法 | 路径 | 说明 |
|------|------|------|
| POST | /api/auth/register | 用户注册 |
| POST | /api/auth/login | 用户登录 |

### 用户接口

| 方法 | 路径 | 说明 | 需认证 |
|------|------|------|--------|
| GET | /api/user/profile | 获取当前用户信息 | ✓ |
| PUT | /api/user/profile | 更新个人信息（昵称/头像） | ✓ |
| GET | /api/user/{id} | 获取指定用户信息 | |

### 帖子接口

| 方法 | 路径 | 说明 | 需认证 |
|------|------|------|--------|
| POST | /api/posts | 发帖（支持图片上传） | ✓ |
| GET | /api/posts | 获取帖子列表（分页） | |
| GET | /api/posts/{id} | 获取帖子详情 | |

### 评论接口

| 方法 | 路径 | 说明 | 需认证 |
|------|------|------|--------|
| POST | /api/posts/{id}/comments | 发表评论 | ✓ |
| GET | /api/posts/{id}/comments | 获取评论列表（分页） | |

### 点赞接口

| 方法 | 路径 | 说明 | 需认证 |
|------|------|------|--------|
| POST | /api/posts/{id}/like | 点赞/取消点赞（切换） | ✓ |

### 私聊接口

| 方法 | 路径 | 说明 | 需认证 |
|------|------|------|--------|
| GET | /api/chat/conversations | 获取会话列表 | ✓ |
| GET | /api/chat/messages/{userId} | 获取聊天记录（分页） | ✓ |
| PUT | /api/chat/read/{userId} | 标记消息已读 | ✓ |
| POST | /api/chat/send | 发送消息（REST备用） | ✓ |

### WebSocket

连接地址: `ws://localhost:8080/ws/chat?token=<jwt_token>`

**发送消息格式：**

```json
{
  "type": "message",
  "receiver_id": 2,
  "content": "你好！"
}
```

**接收消息格式：**

```json
{
  "type": "message",
  "message_id": 1,
  "sender_id": 1,
  "receiver_id": 2,
  "content": "你好！",
  "created_at": "2025-01-01 12:00:00"
}
```

**心跳机制：**

```json
// 发送
{ "type": "ping" }
// 响应
{ "type": "pong" }
```

**在线状态通知：**

```json
{
  "type": "online_status",
  "user_id": 1,
  "online": true
}
```

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
| 409 | 资源冲突（如用户名已存在） |
| 500 | 服务器内部错误 |

## 开发说明

### 前端开发代理

开发环境下，Vite 配置了以下代理规则（`vite.config.ts`）：

- `/api` → `http://localhost:8080` — API 请求代理
- `/ws` → `ws://localhost:8080` — WebSocket 代理
- `/uploads` → `http://localhost:8080` — 上传文件访问代理

### 认证机制

- 使用 JWT Token 进行身份认证
- Token 有效期 24 小时
- Token 存储在浏览器 `localStorage` 中
- 登录/注册接口无需认证，其余需认证接口通过 `Authorization: Bearer <token>` 传递

### 添加新功能

1. **后端**：在 `python_backend/routers/` 目录下添加路由文件，在 `main.py` 中注册路由
2. **前端**：在 `frontend/src/api/` 添加 API 封装，在 `views/` 添加页面，在 `router/index.ts` 注册路由

### 代码规范

- 后端使用 Python 3.10+ 标准，遵循 FastAPI 框架规范
- 前端使用 Vue 3 Composition API + TypeScript
- 所有 API 接口需实现参数校验和错误处理
- 数据库操作使用 PyMySQL，每个请求创建独立连接并在 finally 中关闭

## License

MIT License
