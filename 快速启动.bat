@echo off
cd /d "%~dp0"
chcp 65001 >nul
title ChatForum 快速启动

echo ========================================
echo   ChatForum 快速启动
echo   (需要先运行 setup.bat 完成初始化)
echo ========================================

REM 创建必要目录
if not exist "uploads\images" mkdir uploads\images 2>nul
if not exist "uploads\avatars" mkdir uploads\avatars 2>nul
if not exist "logs" mkdir logs 2>nul

REM 检查后端是否存在
if not exist "build\Release\ChatForum.exe" (
  echo.
  echo [错误] 后端未编译!
  echo 请先运行 setup.bat 完成初始化
  echo.
  pause
  exit /b 1
)

REM 检查前端依赖
if not exist "frontend\node_modules" (
  echo.
  echo [错误] 前端依赖未安装!
  echo 请先运行 setup.bat 完成初始化
  echo.
  pause
  exit /b 1
)

echo.
echo [1/2] 启动后端服务...
start "ChatForum 后端" cmd /k "cd build\Release && ChatForum.exe ..\..\config\config.json"

timeout /t 2 /nobreak >nul

echo [2/2] 启动前端服务...
start "ChatForum 前端" cmd /k "cd frontend && npm run dev"

echo.
echo ========================================
echo   启动完成!
echo   前端: http://localhost:5173
echo   后端: http://localhost:8080
echo.
echo   提示: 关闭这两个窗口即可停止服务
echo ========================================
echo.
pause
