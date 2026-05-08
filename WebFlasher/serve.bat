@echo off
echo Starting local HTTP server for WebFlasher...
echo Open Chrome and go to: http://localhost:8080
echo.
echo Press Ctrl+C to stop.
echo.
cd /d "%~dp0"
python -m http.server 8080
pause
