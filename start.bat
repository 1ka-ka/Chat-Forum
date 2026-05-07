@echo off
chcp 65001 >nul
echo ========================================
echo    ChatForum 项目启动脚本
echo ========================================
echo.

cd /d "%~dp0"

echo [1/3] 检查数据库连接...
mysql -u root -p123456 -e "USE chatforum;" 2>nul
if %errorlevel% neq 0 (
    echo 错误：无法连接数据库，请确保：
    echo   1. MySQL 已启动
    echo   2. 数据库 chatforum 已创建
    echo   3. 已执行 sql/schema.sql 和 sql/init_data.sql
    echo.
    echo 可以使用以下命令初始化数据库：
    echo   mysql -u root -p ^< sql\schema.sql
    echo   mysql -u root -p ^< sql\init_data.sql
    pause
    exit /b 1
)
echo 数据库连接成功！

echo.
echo [2/3] 启动后端服务器...
if not exist "build\ChatForum.exe" (
    echo 警告：未找到后端可执行文件，正在构建...
    call :build_backend
)

start "ChatForum Backend" cmd /c "build\ChatForum.exe config\config.json"
echo 后端服务器启动中（端口 8080）...

echo.
echo [3/3] 启动前端开发服务器...
cd frontend
if not exist "node_modules" (
    echo 安装前端依赖...
    npm install
)
start "ChatForum Frontend" cmd /c "npm run dev"

echo.
echo ========================================
echo    启动完成！
echo ========================================
echo.
echo   前端地址: http://localhost:5173
echo   后端地址: http://localhost:8080
echo.
echo   测试账号:
echo     alice / 123456
echo     bob   / 123456
echo     charlie / 123456
echo.
echo 按任意键退出此窗口（后端和前端服务将继续运行）...
pause >nul
exit /b

:build_backend
    if not exist "build" mkdir build
    cd build
    cmake .. -DCMAKE_TOOLCHAIN_FILE=C:\vcpkg\scripts\buildsystems\vcpkg.cmake
    cmake --build . --config Release
    cd ..
    exit /b
