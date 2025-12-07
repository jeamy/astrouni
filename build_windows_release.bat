@echo off
rem AstroUniverse 2026 - Windows Release Build
rem Prueft/zeigt Abhaengigkeiten, baut Release, erstellt Minimal-Dist

setlocal ENABLEDELAYEDEXPANSION

set SCRIPT_DIR=%~dp0
set PROJECT_DIR=%SCRIPT_DIR%astrouni2026
set BUILD_DIR=%PROJECT_DIR%\build-windows-release
set DIST_DIR=%PROJECT_DIR%\dist\windows
set BUILD_TYPE=Release

echo === AstroUniverse 2026 - Windows Release Build ===
echo.
rem Beispiel-Konfiguration fuer Qt mit MinGW:
rem   Qt-Installation unter G:\Qt (Qt Online Installer, MinGW-Kit)
rem   Vor dem Aufruf dieses Scripts z.B.:
rem     set Qt6_DIR=G:\Qt\6.7.0\mingw_64
rem     set PATH=G:\Qt\Tools\mingw1120_64\bin;%%PATH%%
rem Diese Werte ggf. an die eigene Qt-Version und den MinGW-Ordner anpassen.

rem ===========================================================================
rem [0] Abhaengigkeiten pruefen
rem ===========================================================================

set MISSING_DEPS=

rem CMake pruefen
where cmake >NUL 2>&1
if errorlevel 1 set MISSING_DEPS=!MISSING_DEPS! cmake

rem C++ Compiler pruefen (cl.exe fuer MSVC oder g++ fuer MinGW)
where cl >NUL 2>&1
if errorlevel 1 (
  where g++ >NUL 2>&1
  if errorlevel 1 set MISSING_DEPS=!MISSING_DEPS! compiler
)

rem Qt6 pruefen (qmake6 oder Qt6_DIR Umgebungsvariable)
where qmake6 >NUL 2>&1
if errorlevel 1 (
  if not defined Qt6_DIR (
    if not defined CMAKE_PREFIX_PATH (
      set MISSING_DEPS=!MISSING_DEPS! qt6
    )
  )
)

rem Falls Abhaengigkeiten fehlen: Hinweise ausgeben
if defined MISSING_DEPS (
  echo Fehlende Abhaengigkeiten:!MISSING_DEPS!
  echo.
  echo Automatische Installation unter Windows nicht moeglich.
  echo Bitte installiere die folgenden Komponenten manuell:
  echo.
  
  echo !MISSING_DEPS! | findstr /C:"cmake" >NUL && (
    echo   - CMake: https://cmake.org/download/
    echo     oder: winget install Kitware.CMake
  )
  
  echo !MISSING_DEPS! | findstr /C:"compiler" >NUL && (
    echo   - C++ Compiler ^(EINE der folgenden Optionen^):
    echo.
    echo     Option A: Visual Studio ^(MSVC^)
    echo       https://visualstudio.microsoft.com/
    echo       Workload "Desktopentwicklung mit C++" waehlen
    echo       Qt6 fuer MSVC installieren
    echo.
    echo     Option B: MinGW ^(empfohlen mit Qt^)
    echo       Qt6 Installer installiert MinGW automatisch mit!
    echo       Nur PATH setzen:
    echo         set PATH=G:\Qt\Tools\mingw1120_64\bin;%%PATH%%
  )
  
  echo !MISSING_DEPS! | findstr /C:"qt6" >NUL && (
    echo   - Qt6: https://www.qt.io/download-qt-installer
    echo     Bei Installation "MinGW" Komponente auswaehlen ^(inkl. Compiler^)
    echo     Nach Installation Qt6_DIR setzen:
    echo       set Qt6_DIR=G:\Qt\6.7.0\mingw_64
    echo       set PATH=G:\Qt\Tools\mingw1120_64\bin;%%PATH%%
  )
  
  echo.
  echo Nach Installation dieses Skript erneut ausfuehren.
  exit /B 1
)

echo Alle Abhaengigkeiten vorhanden.
echo.

rem ===========================================================================
rem [1] CMake konfigurieren
rem ===========================================================================
echo [1/3] CMake konfigurieren...

rem Generator automatisch erkennen: MinGW oder MSVC
set CMAKE_GENERATOR=
where g++ >NUL 2>&1
if not errorlevel 1 (
    echo Erkannt: MinGW ^(g++^) - verwende "MinGW Makefiles"
    set CMAKE_GENERATOR=-G "MinGW Makefiles"
) else (
    where cl >NUL 2>&1
    if not errorlevel 1 (
        echo Erkannt: MSVC ^(cl^) - verwende Visual Studio Generator
        rem Kein expliziter Generator noetig, CMake waehlt automatisch
    ) else (
        echo WARNUNG: Kein Compiler gefunden!
    )
)

cmake -S "%PROJECT_DIR%" -B "%BUILD_DIR%" %CMAKE_GENERATOR% -DCMAKE_BUILD_TYPE=%BUILD_TYPE%

if errorlevel 1 goto :error

rem ===========================================================================
rem [2] Bauen
rem ===========================================================================
echo.
echo [2/3] Bauen...
cmake --build "%BUILD_DIR%" --config %BUILD_TYPE%

if errorlevel 1 goto :error

rem ===========================================================================
rem [3] Dist-Verzeichnis erstellen (Minimalversion)
rem ===========================================================================
echo.
echo [3/3] Dist-Verzeichnis erstellen...
if exist "%DIST_DIR%" rmdir /S /Q "%DIST_DIR%"
mkdir "%DIST_DIR%"

rem Binaerdatei kopieren (Single-Config: astrouni2026.exe im Build-Dir)
if exist "%BUILD_DIR%\astrouni2026.exe" (
  copy /Y "%BUILD_DIR%\astrouni2026.exe" "%DIST_DIR%" >NUL
) else (
  rem Multi-Config (z.B. Visual Studio): Release\astrouni2026.exe
  if exist "%BUILD_DIR%\%BUILD_TYPE%\astrouni2026.exe" (
    copy /Y "%BUILD_DIR%\%BUILD_TYPE%\astrouni2026.exe" "%DIST_DIR%" >NUL
  ) else (
    echo FEHLER: Binaerdatei astrouni2026.exe wurde nicht gefunden.
    goto :error
  )
)

rem MINIMAL erforderliche Daten-Dateien:
rem   - astroorg.dat, astroger.dat, europa.dat  (Orte - mind. eine muss existieren)
rem   - default.dat                             (Standard-Orben)
rem NICHT noetig fuer Start: astronam.dat, astroini.dat, astrorad.dat, astronot.dat
mkdir "%DIST_DIR%\data" 2>NUL
for %%F in (astroorg.dat astroger.dat europa.dat default.dat) do (
  if exist "%PROJECT_DIR%\data\%%F" copy /Y "%PROJECT_DIR%\data\%%F" "%DIST_DIR%\data" >NUL
)

rem Ressourcen (Icons etc.) - optional aber empfohlen
if exist "%PROJECT_DIR%\resources" (
  xcopy "%PROJECT_DIR%\resources" "%DIST_DIR%\resources" /E /I /Y >NUL
)

rem Release-Zip im dist-Verzeichnis erstellen (optional, via PowerShell)
set ZIP_NAME=astrouni2026-windows-release.zip
where powershell >NUL 2>&1
if errorlevel 1 (
  echo Hinweis: PowerShell nicht gefunden, Release-Zip wird nicht erstellt.
) else (
  echo Erzeuge Release-Zip: %ZIP_NAME%
  pushd "%PROJECT_DIR%\dist"
  if exist "%ZIP_NAME%" del /F /Q "%ZIP_NAME%"
  powershell -NoLogo -NoProfile -Command "Compress-Archive -Path 'windows\*' -DestinationPath '%ZIP_NAME%' -Force"
  popd
  echo Release-Zip erstellt: %PROJECT_DIR%\dist\%ZIP_NAME%
)

echo.
echo ========================================
echo   Release-Build fertig!
echo ========================================
echo.
echo Start:
echo   %DIST_DIR%\astrouni2026.exe
echo.

goto :eof

:error
echo.
echo Build fehlgeschlagen.
exit /B 1

endlocal
