@echo off
REM GUI Start Script für Windows
REM AstroUniverse - Modern Astrology Software

setlocal enabledelayedexpansion

echo ===============================================
echo   AstroUniverse GUI (Windows)
echo ===============================================
echo.

REM Finde GUI Binary
set GUI_EXE=
if exist "build\gui\Release\astro_gui.exe" set GUI_EXE=build\gui\Release\astro_gui.exe
if exist "build\gui\Debug\astro_gui.exe" set GUI_EXE=build\gui\Debug\astro_gui.exe
if exist "build\gui\astro_gui.exe" set GUI_EXE=build\gui\astro_gui.exe

if "%GUI_EXE%"=="" (
    echo [FEHLER] GUI nicht gefunden!
    echo Bitte zuerst build.bat ausfuehren
    exit /b 1
)

echo Starte wxWidgets GUI...
echo.
%GUI_EXE%

endlocal
