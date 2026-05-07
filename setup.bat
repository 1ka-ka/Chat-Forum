@echo off
cd /d "%~dp0"
chcp 65001 >nul
title ChatForum 完整初始化向导

echo ========================================
echo   ChatForum 完整初始化向导
echo ========================================

REM 检查管理员权限
net session >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
  echo 警告: 建议以管理员身份运行
)

echo.
echo [1/6] 检查环境...

REM 查找 CMake
set CMAKE_PATH=
for /f "delims=" %%i in ('where cmake 2^>nul') do set CMAKE_PATH=%%i
if "%CMAKE_PATH%"=="" (
  if exist "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe" (
    set PATH=C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin;%PATH%
  ) else (
    echo [错误] 未找到 CMake，请安装 Visual Studio 或 CMake
    pause
    exit /b 1
  )
)
echo [OK] CMake 已找到

REM 检查 Node.js
where node >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
  echo [错误] 未找到 Node.js，请从 nodejs.org 下载安装
  pause
  exit /b 1
)
echo [OK] Node.js 已找到

REM 检查 vcpkg
if not exist "C:\vcpkg\vcpkg.exe" (
  echo.
  echo [2/6] 正在安装 vcpkg...
  if not exist "C:\vcpkg" (
    git clone https://github.com/microsoft/vcpkg.git C:\vcpkg
  )
  cd C:\vcpkg
  call bootstrap-vcpkg.bat
  cd /d "%~dp0"
)
echo [OK] vcpkg 已就绪

echo.
echo [3/6] 正在安装依赖包 (这可能需要很长时间)...
if not exist "C:\vcpkg\installed\x64-windows\include\drogon" (
  C:\vcpkg\vcpkg install drogon:x64-windows jwt-cpp:x64-windows libmysql:x64-windows
) else (
  echo [跳过] 依赖已安装
)

echo.
echo [4/6] 初始化数据库...
echo 请手动执行以下步骤:
echo   1. 打开 MySQL 客户端
echo   2. 执行: source %CD%\sql\schema.sql
echo   3. 执行: source %CD%\sql\init_data.sql
echo.
echo 按任意键继续...
pause >nul

echo.
echo [5/6] 正在编译后端...
if not exist "build" mkdir build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=C:\vcpkg\scripts\buildsystems\vcpkg.cmake
if %ERRORLEVEL% NEQ 0 (
  echo [错误] CMake 配置失败
  cd ..
  pause
  exit /b 1
)
cmake --build . --config Release
if %ERRORLEVEL% NEQ 0 (
  echo [错误] 编译失败
  cd ..
  pause
  exit /b 1
)
cd ..
echo [OK] 后端编译完成

echo.
echo [6/6] 正在安装前端依赖...
cd frontend
call npm install
if %ERRORLEVEL% NEQ 0 (
  echo [错误] npm install 失败
  cd ..
  pause
  exit /b 1
)
cd ..
echo [OK] 前端依赖安装完成

echo.
echo ========================================
echo   初始化完成!
echo ========================================
echo.
echo 现在你可以:
echo   1. 双击 "快速启动.bat" 启动服务
echo   2. 或运行 "start.bat" 启动
echo.
echo 默认测试账号: alice/bob/charlie
echo 密码: 123456
echo.
pause
