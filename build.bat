@echo off
REM Build Script für Windows
REM AstroUniverse - Modern Astrology Software

setlocal enabledelayedexpansion

echo ===============================================
echo   AstroUniverse Build Script (Windows)
echo ===============================================
echo.

REM Prüfe CMake
where cmake >nul 2>nul
if %ERRORLEVEL% NEQ 0 (
    echo [FEHLER] CMake nicht gefunden!
    echo.
    echo Installation:
    echo   - Download: https://cmake.org/download/
    echo   - Oder: winget install Kitware.CMake
    echo.
    exit /b 1
)

echo [OK] CMake gefunden
cmake --version | findstr /C:"version"
echo.

REM Prüfe Compiler
where cl >nul 2>nul
if %ERRORLEVEL% NEQ 0 (
    where g++ >nul 2>nul
    if %ERRORLEVEL% NEQ 0 (
        echo [FEHLER] Kein C++ Compiler gefunden!
        echo.
        echo Installation:
        echo   - Visual Studio 2019+ mit C++ Desktop Development
        echo   - Oder: MinGW-w64
        echo.
        exit /b 1
    ) else (
        echo [OK] MinGW GCC gefunden
        set GENERATOR=-G "MinGW Makefiles"
    )
) else (
    echo [OK] MSVC gefunden
    set GENERATOR=-G "Visual Studio 17 2022"
)
echo.

REM Build-Typ
set BUILD_TYPE=%1
if "%BUILD_TYPE%"=="" set BUILD_TYPE=Release
echo Build-Typ: %BUILD_TYPE%
echo.

REM Build-Verzeichnis erstellen
echo Erstelle Build-Verzeichnis...
if not exist build mkdir build
cd build

REM CMake konfigurieren
echo Konfiguriere CMake...
cmake %GENERATOR% -DCMAKE_BUILD_TYPE=%BUILD_TYPE% ..

if %ERRORLEVEL% NEQ 0 (
    echo [FEHLER] CMake-Konfiguration fehlgeschlagen!
    exit /b 1
)
echo [OK] CMake-Konfiguration erfolgreich
echo.

REM Kompilieren
echo Kompiliere Projekt...
cmake --build . --config %BUILD_TYPE% -j

if %ERRORLEVEL% NEQ 0 (
    echo [FEHLER] Kompilierung fehlgeschlagen!
    exit /b 1
)
echo [OK] Kompilierung erfolgreich
echo.

REM Zusammenfassung
echo ===============================================
echo   Build erfolgreich abgeschlossen!
echo ===============================================
echo.
echo Ausfuehrbare Dateien:
echo   - %BUILD_TYPE%\example_chart.exe     (Beispielprogramm)
echo   - tests\%BUILD_TYPE%\astro_tests.exe (Unit Tests)
echo.
echo Naechste Schritte:
echo   test.bat                (Tests ausfuehren)
echo   %BUILD_TYPE%\example_chart.exe  (Demo starten)
echo.

endlocal
