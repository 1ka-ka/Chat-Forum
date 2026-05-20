@echo off
chcp 65001 >nul
echo ========================================
echo    畅谈 项目启动脚本
echo ========================================
echo.

cd /d "%~dp0"

echo 请选择启动模式：
echo   [1] 开发模式（双服务器：Vite 5173 + API 8080，支持热更新）
echo   [2] 单服务器模式（仅 8080，需先构建前端）
echo.
set /p MODE="请输入选择 (1/2，默认1): "
if "%MODE%"=="" set MODE=1

echo.
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
echo [2/3] 准备后端...
cd python_backend
if not exist "venv" (
    echo 创建 Python 虚拟环境...
    python -m venv venv
)
if not exist "venv\Lib\site-packages\fastapi" (
    echo 安装 Python 依赖...
    call venv\Scripts\activate.bat && pip install -r requirements.txt
)
cd ..

if "%MODE%"=="2" (
    echo.
    echo [3/3] 单服务器模式 - 检查前端构建产物...
    if not exist "python_backend\frontend\dist\index.html" (
        echo 未找到前端构建产物，正在构建...
        cd frontend
        if not exist "node_modules" (
            echo 安装前端依赖...
            npm install
        )
        call npm run build
        if %errorlevel% neq 0 (
            echo 前端构建失败！
            pause
            exit /b 1
        )
        cd ..
        if not exist "python_backend\frontend\dist" (
            mkdir "python_backend\frontend\dist"
        )
        xcopy /E /Y /I "frontend\dist" "python_backend\frontend\dist" >nul
    )
    echo 前端构建产物就绪！
    echo.
    echo 启动服务器...
    cd python_backend
    start "畅谈 Server" cmd /c "cd /d %~dp0python_backend && venv\Scripts\python.exe -m uvicorn main:app --host 0.0.0.0 --port 8080"
    cd ..
    echo.
    echo ========================================
    echo    启动完成！（单服务器模式）
    echo ========================================
    echo.
    echo   访问地址: http://localhost:8080
    echo   API文档:  http://localhost:8080/docs
    echo.
    echo   测试账号:
    echo     alice / 123456
    echo     bob   / 123456
    echo     charlie / 123456
) else (
    echo.
    echo [3/3] 开发模式 - 启动双服务器...
    cd python_backend
    start "畅谈 Backend" cmd /c "cd /d %~dp0python_backend && venv\Scripts\python.exe -m uvicorn main:app --host 0.0.0.0 --port 8080"
    cd ..
    echo 后端服务器启动中（端口 8080）...

    echo.
    cd frontend
    if not exist "node_modules" (
        echo 安装前端依赖...
        npm install
    )
    start "畅谈 Frontend" cmd /c "npm run dev"
    cd ..
    echo 前端开发服务器启动中（端口 5173）...

    echo.
    echo ========================================
    echo    启动完成！（开发模式）
    echo ========================================
    echo.
    echo   前端地址: http://localhost:5173
    echo   后端地址: http://localhost:8080
    echo   API文档:  http://localhost:8080/docs
    echo.
    echo   测试账号:
    echo     alice / 123456
    echo     bob   / 123456
    echo     charlie / 123456
)

echo.
echo 按任意键退出此窗口（服务将继续运行）...
pause >nul
exit /b
