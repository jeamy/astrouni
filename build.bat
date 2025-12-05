@echo off
REM
REM AstroUniverse 2026 - Build Script für Windows
REM

setlocal enabledelayedexpansion

set SCRIPT_DIR=%~dp0
set PROJECT_DIR=%SCRIPT_DIR%astrouni2026
set BUILD_TYPE=%1
if "%BUILD_TYPE%"=="" set BUILD_TYPE=Release

echo ========================================
echo   AstroUniverse 2026 Build
echo ========================================
echo Build Type: %BUILD_TYPE%
echo.

REM Prüfen ob CMake installiert ist
where cmake >nul 2>nul
if %ERRORLEVEL% neq 0 (
    echo FEHLER: CMake nicht gefunden!
    echo.
    echo Installation:
    echo   1. Visual Studio 2022 mit "Desktop development with C++"
    echo   2. Oder: https://cmake.org/download/
    exit /b 1
)

REM Build-Verzeichnis erstellen
set BUILD_DIR=%PROJECT_DIR%\build
if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"
cd /d "%BUILD_DIR%"

REM CMake konfigurieren
echo Konfiguriere CMake...
cmake -DCMAKE_BUILD_TYPE=%BUILD_TYPE% ..
if %ERRORLEVEL% neq 0 (
    echo FEHLER: CMake Konfiguration fehlgeschlagen!
    exit /b 1
)

REM Bauen
echo.
echo Baue Projekt...
cmake --build . --config %BUILD_TYPE%
if %ERRORLEVEL% neq 0 (
    echo FEHLER: Build fehlgeschlagen!
    exit /b 1
)

echo.
echo ========================================
echo   Build erfolgreich!
echo ========================================
echo.
echo Ausfuehren:
echo   cd %BUILD_DIR%\%BUILD_TYPE%
echo   astrouni2026.exe
echo.
echo Tests:
echo   cd %BUILD_DIR%
echo   ctest -C %BUILD_TYPE% --output-on-failure
echo.

endlocal
