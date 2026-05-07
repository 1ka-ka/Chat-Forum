@echo off
chcp 65001 >nul
echo ========================================
echo    ChatForum 项目重新构建脚本
echo ========================================
echo.

cd /d "%~dp0"

echo [1/2] 清理旧构建...
if exist "build" rmdir /s /q build
if exist "frontend\dist" rmdir /s /q frontend\dist
echo 清理完成！

echo.
echo [2/2] 重新构建后端...
mkdir build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=C:\vcpkg\scripts\buildsystems\vcpkg.cmake -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
if %errorlevel% neq 0 (
    echo 构建失败！
    pause
    exit /b 1
)

cd ..

echo.
echo 构建完成！使用 start.bat 启动项目。
pause
