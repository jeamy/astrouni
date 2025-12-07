# Swiss Ephemeris Setup - Cross-Platform

## Schnellstart

### 1. Ephemeriden-Dateien herunterladen

```bash
cd /media/data/programming/v004beta/swisseph
./download_ephe.sh
```

**Oder manuell**:
```bash
cd swisseph/ephe
curl -O https://www.astro.com/ftp/swisseph/ephe/seas_18.se1
curl -O https://www.astro.com/ftp/swisseph/ephe/semo_18.se1
curl -O https://www.astro.com/ftp/swisseph/ephe/sepl_18.se1
```

### 2. Swiss Ephemeris Library installieren

## Linux (Ubuntu/Debian)

### Option A: System-Package (empfohlen)
```bash
sudo apt-get update
sudo apt-get install libswe-dev
```

### Option B: Aus Source kompilieren
```bash
# Dependencies
sudo apt-get install build-essential

# Download und Build
cd /tmp
wget https://www.astro.com/ftp/swisseph/swe_unix_src_2.10.03.tar.gz
tar xzf swe_unix_src_2.10.03.tar.gz
cd swisseph/src

# Kompilieren
make
sudo make install
sudo ldconfig

# Verifizieren
ldconfig -p | grep libswe
```

## macOS

### Option A: Homebrew
```bash
# Falls nicht im Standard-Repo, manuell installieren
brew tap homebrew/science  # Falls verfügbar
brew install swisseph
```

### Option B: Aus Source
```bash
# Xcode Command Line Tools benötigt
xcode-select --install

# Download und Build
cd /tmp
curl -O https://www.astro.com/ftp/swisseph/swe_unix_src_2.10.03.tar.gz
tar xzf swe_unix_src_2.10.03.tar.gz
cd swisseph/src

# Kompilieren
make
sudo make install

# Library-Pfad aktualisieren
sudo update_dyld_shared_cache
```

## Windows

### Option A: vcpkg (empfohlen)
```powershell
# vcpkg installieren (falls noch nicht vorhanden)
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat

# Swiss Ephemeris installieren
.\vcpkg install swisseph:x64-windows
.\vcpkg integrate install
```

### Option B: Manuell
```powershell
# 1. Download
# https://www.astro.com/ftp/swisseph/swe_win32_2.10.03.zip

# 2. Extrahieren nach C:\swisseph

# 3. In CMakeLists.txt:
# set(SWISSEPH_ROOT "C:/swisseph")
```

## Verifikation

### Test-Programm erstellen

```cpp
// test_swisseph.cpp
#include <iostream>
#include <swephexp.h>

int main() {
    // Ephemeriden-Pfad setzen
    swe_set_ephe_path("./swisseph/ephe");
    
    // Sonne am 1.1.2000, 12:00 UT
    double jd = 2451545.0;
    double xx[6];
    char serr[256];
    
    int ret = swe_calc_ut(jd, SE_SUN, SEFLG_SWIEPH | SEFLG_SPEED, xx, serr);
    
    if (ret < 0) {
        std::cerr << "Fehler: " << serr << std::endl;
        return 1;
    }
    
    std::cout << "Swiss Ephemeris funktioniert!" << std::endl;
    std::cout << "Sonne am 1.1.2000:" << std::endl;
    std::cout << "  Länge: " << xx[0] << "°" << std::endl;
    std::cout << "  Breite: " << xx[1] << "°" << std::endl;
    std::cout << "  Distanz: " << xx[2] << " AU" << std::endl;
    std::cout << "  Geschwindigkeit: " << xx[3] << "°/Tag" << std::endl;
    
    swe_close();
    return 0;
}
```

### Kompilieren und Testen

**Linux/macOS**:
```bash
g++ -o test_swisseph test_swisseph.cpp -lswe
./test_swisseph
```

**Windows (mit vcpkg)**:
```powershell
cl test_swisseph.cpp /I"C:\vcpkg\installed\x64-windows\include" ^
   /link "C:\vcpkg\installed\x64-windows\lib\swe.lib"
test_swisseph.exe
```

**Erwartete Ausgabe**:
```
Swiss Ephemeris funktioniert!
Sonne am 1.1.2000:
  Länge: 280.xxx°
  Breite: 0.xxx°
  Distanz: 0.983xxx AU
  Geschwindigkeit: 1.019xxx°/Tag
```

## CMake Integration (Cross-Platform)

```cmake
# CMakeLists.txt
cmake_minimum_required(VERSION 3.20)
project(AstroUniverse)

set(CMAKE_CXX_STANDARD 20)

# Swiss Ephemeris finden (Cross-Platform)
if(WIN32)
    # Windows: vcpkg oder manuell
    find_package(swisseph CONFIG QUIET)
    if(NOT swisseph_FOUND)
        set(SWISSEPH_ROOT "C:/swisseph" CACHE PATH "Swiss Ephemeris root")
        find_library(SWISSEPH_LIB NAMES swe libswe 
                     PATHS ${SWISSEPH_ROOT}/lib 
                           ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/lib)
        find_path(SWISSEPH_INCLUDE swephexp.h 
                  PATHS ${SWISSEPH_ROOT}/include
                        ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include)
    endif()
elseif(APPLE)
    # macOS: Homebrew oder /usr/local
    find_library(SWISSEPH_LIB swe 
                 PATHS /usr/local/lib 
                       /opt/homebrew/lib
                       /usr/lib)
    find_path(SWISSEPH_INCLUDE swephexp.h 
              PATHS /usr/local/include 
                    /opt/homebrew/include
                    /usr/include)
else()
    # Linux: System-Installation
    find_library(SWISSEPH_LIB swe 
                 PATHS /usr/lib 
                       /usr/local/lib
                       /usr/lib/x86_64-linux-gnu)
    find_path(SWISSEPH_INCLUDE swephexp.h 
              PATHS /usr/include 
                    /usr/local/include)
endif()

if(NOT SWISSEPH_LIB OR NOT SWISSEPH_INCLUDE)
    message(FATAL_ERROR 
        "Swiss Ephemeris nicht gefunden!\n"
        "Linux: sudo apt-get install libswe-dev\n"
        "macOS: brew install swisseph\n"
        "Windows: vcpkg install swisseph:x64-windows")
endif()

message(STATUS "Swiss Ephemeris Library: ${SWISSEPH_LIB}")
message(STATUS "Swiss Ephemeris Include: ${SWISSEPH_INCLUDE}")

# Ephemeriden-Pfad
set(EPHE_PATH "${CMAKE_SOURCE_DIR}/swisseph/ephe" CACHE PATH "Ephemeris path")
add_definitions(-DEPHE_PATH="${EPHE_PATH}")

# Executable
add_executable(test_swisseph test_swisseph.cpp)
target_include_directories(test_swisseph PRIVATE ${SWISSEPH_INCLUDE})
target_link_libraries(test_swisseph PRIVATE ${SWISSEPH_LIB})

# Windows: DLL kopieren
if(WIN32 AND EXISTS "${SWISSEPH_ROOT}/bin/swe.dll")
    add_custom_command(TARGET test_swisseph POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${SWISSEPH_ROOT}/bin/swe.dll"
        $<TARGET_FILE_DIR:test_swisseph>)
endif()
```

### Build-Prozess

```bash
mkdir build && cd build

# Linux/macOS
cmake ..
make

# Windows
cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Release

# Test
./test_swisseph  # Linux/macOS
.\Release\test_swisseph.exe  # Windows
```

## Troubleshooting

### Linux: Library nicht gefunden
```bash
# Prüfen ob installiert
dpkg -L libswe-dev

# Library-Cache aktualisieren
sudo ldconfig

# Manuell Pfad setzen
export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH
```

### macOS: dyld Library not loaded
```bash
# Library-Pfad prüfen
otool -L test_swisseph

# Pfad korrigieren
install_name_tool -change libswe.dylib /usr/local/lib/libswe.dylib test_swisseph
```

### Windows: DLL nicht gefunden
```powershell
# DLL neben .exe kopieren
copy C:\vcpkg\installed\x64-windows\bin\swe.dll .

# Oder System-PATH erweitern
$env:PATH += ";C:\vcpkg\installed\x64-windows\bin"
```

### Ephemeriden-Dateien nicht gefunden
```cpp
// Im Code: Absoluten Pfad verwenden
#ifdef _WIN32
    swe_set_ephe_path("C:/Users/YourName/astro/swisseph/ephe");
#else
    swe_set_ephe_path("/home/yourname/astro/swisseph/ephe");
#endif
```

## Nächste Schritte

1. ✅ Swiss Ephemeris installiert
2. ✅ Ephemeriden-Dateien heruntergeladen
3. ✅ Test-Programm funktioniert
4. → Integration in AstroUniverse Core Library
5. → Häusersysteme implementieren
6. → GUI anbinden

Siehe: `SWISS_EPHEMERIS_INTEGRATION.md` für Details zur Code-Integration.
