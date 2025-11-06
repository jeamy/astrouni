@echo off
REM Clean Script für Windows
REM Entfernt alle Build-Artefakte

echo Entferne Build-Artefakte...

if exist build rmdir /s /q build
if exist cmake-build-debug rmdir /s /q cmake-build-debug
if exist cmake-build-release rmdir /s /q cmake-build-release
del /q *.obj 2>nul
del /q *.lib 2>nul

echo [OK] Bereinigung abgeschlossen
