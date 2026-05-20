@echo off
chcp 65001 >nul
echo ========================================
echo    畅谈 项目构建脚本
echo ========================================
echo.

cd /d "%~dp0"

echo [1/2] 构建前端...
cd frontend
if not exist "node_modules" (
    echo 安装前端依赖...
    npm install
)
echo 正在构建前端...
call npm run build
if %errorlevel% neq 0 (
    echo 前端构建失败！
    pause
    exit /b 1
)
cd ..

echo.
echo [2/2] 复制前端构建产物...
if not exist "python_backend\frontend\dist" (
    mkdir "python_backend\frontend\dist"
)
xcopy /E /Y /I "frontend\dist" "python_backend\frontend\dist"

echo.
echo ========================================
echo    构建完成！
echo ========================================
echo.
echo   生产环境启动方式：
echo   cd python_backend
echo   venv\Scripts\python.exe -m uvicorn main:app --host 0.0.0.0 --port 8080
echo.
echo   访问 http://localhost:8080 即可使用
echo.
pause
