@echo off
echo ==========================
echo   Git Auto Push Script
echo ==========================

cd /d "%~dp0"

echo.
git add .

set /p msg=Enter commit message: 

git commit -m "%msg%"

echo.
echo Pulling latest changes...
git pull origin main --rebase

echo.
echo Pushing to origin main...
git push origin main

echo.
echo Done!
pause