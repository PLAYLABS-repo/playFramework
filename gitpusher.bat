@echo off
echo ==========================
echo   Git Auto Push Script
echo ==========================

cd /d "%~dp0"

echo.
echo Adding files...
git add .

echo.
set /p msg=Enter commit message: 

echo.
echo Committing...
git commit -m "%msg%"

echo.
echo Pushing to origin main...
git push origin main

echo.
echo Done!
pause