@echo off
cd /d "%~dp0"
chcp 65001 >nul
title ChatForum 数据库初始化

echo ========================================
echo   ChatForum 数据库初始化工具
echo ========================================

set MYSQL_PATH=
for /f "delims=" %%i in ('where mysql 2^>nul') do set MYSQL_PATH=%%i

if "%MYSQL_PATH%"=="" (
  if exist "D:\MySQL\MySQL Server 8.0\bin\mysql.exe" (
    set MYSQL_PATH=D:\MySQL\MySQL Server 8.0\bin\mysql.exe
  ) else if exist "C:\Program Files\MySQL\MySQL Server 8.0\bin\mysql.exe" (
    set MYSQL_PATH=C:\Program Files\MySQL\MySQL Server 8.0\bin\mysql.exe
  ) else (
    echo [错误] 未找到 MySQL
    echo 请将 MySQL 的 bin 目录添加到 PATH 环境变量
    pause
    exit /b 1
  )
)

echo.
echo 请输入 MySQL 密码:
set /p MYSQL_PWD=

echo.
echo 正在创建数据库...
"%MYSQL_PATH%" -u root -p%MYSQL_PWD% < sql\schema.sql
if %ERRORLEVEL% NEQ 0 (
  echo [错误] 数据库创建失败
  pause
  exit /b 1
)

echo 正在导入初始数据...
"%MYSQL_PATH%" -u root -p%MYSQL_PWD% chatforum < sql\init_data.sql
if %ERRORLEVEL% NEQ 0 (
  echo [错误] 数据导入失败
  pause
  exit /b 1
)

echo.
echo ========================================
echo   数据库初始化完成!
echo ========================================
echo.
pause
