@echo off
rem AstroUniverse 2026 - Windows Release Build
rem Prueft/zeigt Abhaengigkeiten, baut Release, erstellt Minimal-Dist

setlocal ENABLEDELAYEDEXPANSION

set SCRIPT_DIR=%~dp0
set PROJECT_DIR=%SCRIPT_DIR%astrouni2026
set BUILD_DIR=%PROJECT_DIR%\build-windows-release
set DIST_DIR=%PROJECT_DIR%\dist\windows
set BUILD_TYPE=Release

rem Optionale Default-Konfiguration fuer Qt6 + MinGW nur setzen,
rem wenn noch nichts gesetzt ist (kann in der aufrufenden Shell ueberschrieben werden)
rem Qt6_DIR   -> Ordner mit Qt6Config.cmake
rem CMAKE_PREFIX_PATH -> Qt-Prefix (..\mingw_64)
if not defined Qt6_DIR (
  set "Qt6_DIR=C:\Qt\6.10.1\mingw_64\lib\cmake\Qt6"
)
if not defined CMAKE_PREFIX_PATH (
  set "CMAKE_PREFIX_PATH=C:\Qt\6.10.1\mingw_64"
)
set "QT_MINGW_BIN=C:\Qt\Tools\mingw1310_64\bin"
echo %PATH% | findstr /I /C:"%QT_MINGW_BIN%" >NUL
if errorlevel 1 (
  set "PATH=%QT_MINGW_BIN%;%PATH%"
)

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
    echo         set PATH=C:\Qt\Tools\mingw1310_64\bin;%%PATH%%
  )
  
  echo !MISSING_DEPS! | findstr /C:"qt6" >NUL && (
    echo   - Qt6: https://www.qt.io/download-qt-installer
    echo     Bei Installation "MinGW" Komponente auswaehlen ^(inkl. Compiler^)
    echo     Nach Installation Qt6_DIR setzen:
    echo       set Qt6_DIR=C:\Qt\6.10.1\mingw_64
    echo       set PATH=C:\Qt\Tools\mingw1310_64\bin;%%PATH%%
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

rem Swiss Ephemeris Dateien (PFLICHT fuer Planetenberechnung)
rem   - sepl_18.se1  (Planeten 1800-2400)
rem   - semo_18.se1  (Mond 1800-2400)
rem   - seas_18.se1  (Asteroiden 1800-2400)
mkdir "%DIST_DIR%\swisseph\ephe" 2>NUL
if exist "%PROJECT_DIR%\swisseph\ephe" (
  echo Kopiere Swiss Ephemeris Dateien...
  xcopy "%PROJECT_DIR%\swisseph\ephe\*.se1" "%DIST_DIR%\swisseph\ephe" /Y >NUL
) else (
  echo WARNUNG: Swiss Ephemeris Dateien nicht gefunden unter %PROJECT_DIR%\swisseph\ephe
)

rem Ressourcen (Icons etc.) - optional aber empfohlen
if exist "%PROJECT_DIR%\resources" (
  xcopy "%PROJECT_DIR%\resources" "%DIST_DIR%\resources" /E /I /Y >NUL
)

rem Qt-Runtime (DLLs) und Plugins fuer MinGW deployen
rem CMAKE_PREFIX_PATH = Qt-Root (z.B. C:\Qt\6.10.1\mingw_64)
set "QT_PREFIX=%CMAKE_PREFIX_PATH%"
set "QT_BIN=%QT_PREFIX%\bin"
if exist "%QT_BIN%\Qt6Core.dll" (
  echo Kopiere Qt6 Runtime-DLLs...
  rem Nur die wirklich benoetigten Qt-DLLs kopieren
  for %%D in (Qt6Core.dll Qt6Gui.dll Qt6Widgets.dll Qt6PrintSupport.dll) do (
    if exist "%QT_BIN%\%%D" copy /Y "%QT_BIN%\%%D" "%DIST_DIR%" >NUL
  )
  rem MinGW Runtime-DLLs (libgcc, libstdc++, libwinpthread)
  for %%D in (libgcc_s_seh-1.dll libstdc++-6.dll libwinpthread-1.dll) do (
    if exist "%QT_BIN%\%%D" copy /Y "%QT_BIN%\%%D" "%DIST_DIR%" >NUL
  )
  rem Platform-Plugin (qwindows.dll) - MUSS vorhanden sein
  mkdir "%DIST_DIR%\platforms" 2>NUL
  if exist "%QT_PREFIX%\plugins\platforms\qwindows.dll" (
    copy /Y "%QT_PREFIX%\plugins\platforms\qwindows.dll" "%DIST_DIR%\platforms" >NUL
  ) else (
    echo WARNUNG: qwindows.dll nicht gefunden unter %QT_PREFIX%\plugins\platforms
  )
  rem Imageformats (optional, fuer PNG/JPG etc.)
  if exist "%QT_PREFIX%\plugins\imageformats" (
    mkdir "%DIST_DIR%\imageformats" 2>NUL
    xcopy "%QT_PREFIX%\plugins\imageformats\*.dll" "%DIST_DIR%\imageformats" /Y >NUL
  )
  rem Styles (optional, fuer native Windows-Look)
  if exist "%QT_PREFIX%\plugins\styles" (
    mkdir "%DIST_DIR%\styles" 2>NUL
    xcopy "%QT_PREFIX%\plugins\styles\*.dll" "%DIST_DIR%\styles" /Y >NUL
  )
) else (
  echo WARNUNG: Qt6Core.dll nicht unter %QT_BIN% gefunden. Bitte Pfad pruefen.
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
