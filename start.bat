@echo off
cd /d "%~dp0"

echo ========================================
echo   ChatForum 一键启动脚本
echo ========================================

REM 检查依赖
echo [1/3] 检查环境...
where cmake >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
  echo 错误: 未找到 CMake
  pause
  exit /b 1
)

where node >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
  echo 错误: 未找到 Node.js
  pause
  exit /b 1
)

where mysql >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
  echo 警告: 未找到 MySQL, 请确保 MySQL 已启动
)

echo 环境检查通过
echo.

REM 构建后端（如果需要）
if not exist "build\Release\ChatForum.exe" (
  echo [2/3] 首次运行, 正在构建后端...
  if not exist "build" mkdir build
  cd build
  cmake .. -DCMAKE_TOOLCHAIN_FILE=C:\vcpkg\scripts\buildsystems\vcpkg.cmake
  if %ERRORLEVEL% NEQ 0 (
    echo 构建失败
    pause
    exit /b 1
  )
  cmake --build . --config Release
  if %ERRORLEVEL% NEQ 0 (
    echo 构建失败
    pause
    exit /b 1
  )
  cd ..
  echo 后端构建完成
)

REM 安装前端依赖（如果需要）
if not exist "frontend\node_modules" (
  echo [3/3] 安装前端依赖...
  cd frontend
  call npm install
  cd ..
  echo 前端依赖安装完成
)

echo.
echo ========================================
echo   启动服务中...
echo ========================================

REM 创建必要的目录
if not exist "uploads\images" mkdir uploads\images 2>nul
if not exist "uploads\avatars" mkdir uploads\avatars 2>nul
if not exist "logs" mkdir logs 2>nul

REM 启动后端（后台）
echo 正在启动后端服务...
start "ChatForum Backend" cmd /k "cd build\Release && ChatForum.exe ..\..\config\config.json"

REM 等待一下
timeout /t 2 /nobreak >nul

REM 启动前端（新窗口）
echo 正在启动前端服务...
start "ChatForum Frontend" cmd /k "cd frontend && npm run dev"

echo.
echo ========================================
echo   启动成功!
echo   后端: http://localhost:8080
echo   前端: http://localhost:5173
echo.
echo   按任意键关闭本窗口 (服务仍在运行)
echo ========================================
pause >nul
