@echo off
REM Test Script für Windows
REM AstroUniverse - Astrology Software

setlocal enabledelayedexpansion

echo ===============================================
echo   AstroUniverse Test Suite (Windows)
echo ===============================================
echo.

REM Prüfe ob Build existiert
if not exist build (
    echo [FEHLER] Build-Verzeichnis nicht gefunden!
    echo Bitte zuerst build.bat ausfuehren
    exit /b 1
)

cd build

REM Finde Test-Binary
set TEST_EXE=
if exist "tests\Release\astro_tests.exe" set TEST_EXE=tests\Release\astro_tests.exe
if exist "tests\Debug\astro_tests.exe" set TEST_EXE=tests\Debug\astro_tests.exe
if exist "tests\astro_tests.exe" set TEST_EXE=tests\astro_tests.exe

if "%TEST_EXE%"=="" (
    echo [FEHLER] Test-Binary nicht gefunden!
    echo Bitte zuerst build.bat ausfuehren
    exit /b 1
)

REM Tests ausführen
echo Fuehre Unit Tests aus...
echo.

%TEST_EXE%

set TEST_RESULT=%ERRORLEVEL%

echo.
if %TEST_RESULT% EQU 0 (
    echo ===============================================
    echo   [OK] Alle Tests bestanden!
    echo ===============================================
) else (
    echo ===============================================
    echo   [FEHLER] Tests fehlgeschlagen!
    echo ===============================================
    exit /b 1
)

REM Optional: CTest
where ctest >nul 2>nul
if %ERRORLEVEL% EQU 0 (
    echo.
    echo Fuehre CTest aus...
    ctest --output-on-failure -C Release
)

echo.
echo Test-Suite erfolgreich abgeschlossen!

endlocal
