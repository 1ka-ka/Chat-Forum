# Git 版本管理与 GitHub 上传指南

同一个本地项目可以推送到多个不同的 GitHub 仓库。

---

## 一、首次初始化（每个项目只需做一次）

### 1. 初始化 + 首次提交

```powershell
cd e:\trea\聊天论坛\ChatForum
git init
git add .
git commit -m "初始提交"
```

### 2. 添加多个远程仓库

一个本地项目可以关联多个远程仓库，给每个仓库起不同的名字：

```powershell
git remote add origin https://github.com/你的用户名/ChatForum.git
git remote add school https://github.com/你的用户名/ChatForum-School.git
git remote add backup https://github.com/你的用户名/ChatForum-Backup.git
```

- `origin` — 主仓库（日常开发用）
- `school` — 学校提交用的仓库（同一个账号下另建一个）
- `backup` — 备份仓库（同一个账号下再建一个）

名字随便起，自己能认出来就行。

### 3. 首次推送到各仓库

```powershell
git branch -M main
git push -u origin main
git push -u school main
git push -u backup main
```

---

## 二、日常使用：提交 + 推送到指定仓库

### 提交（每次更新后必做）

```powershell
cd e:\trea\聊天论坛\ChatForum
git add .
git commit -m "更新说明"
```

### 推送到指定仓库

```powershell
git push origin       # 推到 GitHub 个人仓库
git push school       # 推到学校仓库
git push backup       # 推到备份仓库
```

### 一次推到所有仓库

```powershell
git push origin && git push school && git push backup
```

---

## 三、管理远程仓库

### 查看当前关联了哪些仓库

```powershell
git remote -v
```

输出示例：
```
origin   https://github.com/你的用户名/ChatForum.git (fetch)
origin   https://github.com/你的用户名/ChatForum.git (push)
school   https://github.com/你的用户名/ChatForum-School.git (fetch)
school   https://github.com/你的用户名/ChatForum-School.git (push)
```

### 添加新的远程仓库

```powershell
git remote add 仓库名 https://github.com/用户名/仓库.git
```

### 删除远程仓库关联

```powershell
git remote remove 仓库名
```

### 修改远程仓库地址

```powershell
git remote set-url 仓库名 https://github.com/新地址.git
```

---

## 四、三个项目的初始化命令

### 平面弹幕游戏

```powershell
cd e:\trea\平面弹幕游戏\BulletStorm
git init
git add .
git commit -m "初始提交"
git remote add origin https://github.com/你的用户名/BulletStorm.git
git branch -M main
git push -u origin main
```

### 聊天论坛

```powershell
cd e:\trea\聊天论坛\ChatForum
git init
git add .
git commit -m "初始提交"
git remote add origin https://github.com/你的用户名/ChatForum.git
git branch -M main
git push -u origin main
```

### 我的个人网站

```powershell
cd e:\trea\我的个人网站
git init
git add .
git commit -m "初始提交"
git remote add origin https://github.com/你的用户名/MyWebsite.git
git branch -M main
git push -u origin main
```

之后想加新仓库，随时 `git remote add 新名字 地址` 即可。

---

## 五、.gitignore 已排除的内容

以下内容不会被提交到仓库（已在 .gitignore 中配置）：

| 排除项 | 原因 |
|--------|------|
| `build/` | C++ 编译产物，本地重新编译即可 |
| `node_modules/` | 前端依赖，npm install 重新安装 |
| `dist/` | 前端构建产物，npm run build 重新生成 |
| `config/config.json` | 含数据库密码和 JWT 密钥，不能上传 |
| `uploads/` 中的文件 | 用户上传的图片，不属于代码 |
| `*.exe` / `*.o` | 编译生成的二进制文件 |
| `.vs/` / `.vscode/` | IDE 配置 |

**注意：** `config/config.json.example`（不含密码的模板）会被提交，`config/config.json`（含真实密码）不会。

---

## 六、遇到问题

### push 时提示"rejected"（远程有更新）

```powershell
git pull 仓库名 --rebase
git push 仓库名
```

### 换电脑后克隆项目

```powershell
git clone https://github.com/你的用户名/ChatForum.git
cd ChatForum
npm install
pip install -r python_backend/requirements.txt
copy config\config.json.example config\config.json
```
