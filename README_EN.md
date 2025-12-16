# AstroUniverse 2026 - Astrology Software

Port of the legacy astrology software **AstroUnivers v0.04Beta (1996)** from the
`legacy/` directory (16-bit Borland C++ / Windows) to a C++20/Qt6
cross-platform architecture.

[Deutsch / German README](README.md)

## Features

- **Precise calculations** - Swiss Ephemeris (embedded, cross-platform)
- **Cross-platform** - Linux, macOS, Windows
- **Legacy compatible** - Reads AstroUnivers .dat files
- **Open source** - GPL v2

## Requirements

**Ubuntu/Debian:**
```bash
sudo apt-get install build-essential cmake git qt6-base-dev qt6-tools-dev
```

**macOS:**
```bash
xcode-select --install   # Xcode Command Line Tools (clang++)
brew install cmake qt@6
```

**Windows:**
- Visual Studio 2022 with C++ Desktop Development (MSVC) **or** Qt 6 with MinGW
- CMake (e.g. `winget install Kitware.CMake`)
- Qt6 installer: https://www.qt.io/download-qt-installer (example configuration with MinGW: `build_windows_release.bat`)

## Build

### Development (manual CMake build)

```bash
cd astrouni2026
mkdir build && cd build
cmake ..
cmake --build . -j$(nproc)
```

### Platform-specific release builds via scripts

For ready-to-run releases there are three scripts in the project root directory.

#### Linux

```bash
cd astrouni
bash build_linux_release.sh
```

For maximum compatibility (glibc 2.31, Ubuntu 20.04), the Linux release can also be built via Docker:

```bash
cd astrouni
bash build_linux_release_docker_ubuntu2004.sh
```

(optional, if the Docker image has already been built: `bash build_linux_release_docker_ubuntu2004.sh --skip-build`)

Linux compatibility (binary release):

- **Architecture:** x86_64
- **glibc:** at least **2.31** (release build baseline: Ubuntu 20.04)
- **Note:** The release bundles Qt libs and Qt plugins (incl. `xcb`) into `lib/` and `plugins/`.

Typical distributions/versions with glibc ≥ 2.31:

- **Ubuntu:** 20.04, 22.04, 24.04
- **Debian:** 11 (bullseye), 12 (bookworm)
- **Linux Mint:** 20.x, 21.x
- **Pop!_OS:** 20.04, 22.04
- **Fedora:** 34+
- **Arch/Manjaro:** rolling releases
- **RHEL/Rocky/Alma:** 9+

The script:

- checks/installs dependencies (via `apt`, if available)
- builds `astrouni2026` in `astrouni2026/build-linux-release`
- creates a minimal release in `astrouni2026/dist/linux` with:
  - `astrouni2026` (binary)
  - `data/astroorg.dat`, `data/astroger.dat`, `data/europa.dat` (locations)
  - `data/default.dat` (default orbs)
  - `resources/` (icons etc., if present)
- additionally creates (if `zip` is installed) an archive
  `astrouni2026/dist/astrouni2026-linux-release.zip`

Starting the release:

```bash
cd astrouni2026/dist/linux
./run_astrouni2026.sh
```

Why `run_astrouni2026.sh`:

- **Start independent of the current working directory** (path resolution always relative to the program directory)
- **Sets** `LD_LIBRARY_PATH` and Qt plugin paths to the included `lib/` and `plugins/` directories

Alternative (if the system already provides matching Qt6 libs or RPATH is sufficient):

```bash
cd astrouni2026/dist/linux
./astrouni2026
```

#### macOS

```bash
cd astrouni
bash build_macos_release.sh
```

The script:

- checks dependencies (Xcode Command Line Tools, CMake, Qt6 via Homebrew)
- builds `astrouni2026` in `astrouni2026/build-macos-release`
- creates a minimal release in `astrouni2026/dist/macos` with the same
  minimal data as on Linux (`astroorg.dat`, `astroger.dat`, `europa.dat`,
  `default.dat` and `resources/`)

Starting the release (terminal):

```bash
cd astrouni2026/dist/macos
./astrouni2026
```

#### Windows

> **System requirement:** Windows 10 or newer required.  

```bat
cd astrouni
build_windows_release.bat
```

The script:

- checks dependencies (CMake, C++ compiler, Qt6) and prints download links and
  example configurations if needed
- builds `astrouni2026.exe` in `astrouni2026/build-windows-release`
- creates a minimal release in `astrouni2026/dist/windows` with:
  - `astrouni2026.exe`
  - `data/astroorg.dat`, `data/astroger.dat`, `data/europa.dat`, `data/default.dat`
  - `resources\` (if present)
- additionally creates (if PowerShell is available) an archive
  `astrouni2026\dist\astrouni2026-windows-release.zip`

**Important (Windows / Qt + MinGW):**

- Before running the script, the Qt environment variables must be set
  (either in the shell or via the default logic inside the script):

  ```bat
  rem Example (adjust to your installation):
  set Qt6_DIR=C:\Qt\6.10.1\mingw_64
  set PATH=C:\Qt\Tools\mingw1310_64\bin;%PATH%
  ```

- The batch script includes a corresponding commented snippet; adjust version/path
  to your local Qt installation if required.

Starting the release:

```bat
astrouni2026\dist\windows\astrouni2026.exe
```

## Data files (minimal installation)

- `astroorg.dat`, `astroger.dat`, `europa.dat`  
  Location databases. At least one of these files should be present,
  recommended are all three.
- `default.dat`  
  Default orbs and default aspect settings.

Without these files the program still starts, but location data and sensible
orb defaults will be missing.

Other `.dat` files (e.g. `astronam.dat`, `astroini.dat`, `astrorad.dat`,
`astronot.dat`) are **optional** and will be created if needed.

**Note on reusing existing data:**

- Existing `.dat` files from the legacy installation (AstroUnivers v0.04Beta)
  can be reused by copying them into `astrouni2026/data/`.

### Astrology font (optional, recommended)

For an original-like display of astrology symbols (planets, zodiac signs,
aspects) the **AstroUniverse font** can be installed:

1. The font file `asu_____.ttf` is located under `resources/fonts/`
2. **Installation:**
   - **Windows:** right-click the file → "Install" or "Install for all users"
   - **Linux:** copy to `~/.local/share/fonts/` and run `fc-cache -fv`
   - **macOS:** double-click the file → "Install font"

If the font is **not installed**, the program automatically uses **Unicode symbols**
as a fallback (☉☽☿♀♂♃♄ etc.), which are available on all modern systems.

## Tests

```bash
cd astrouni2026/build
ctest --output-on-failure
```

## Run

```bash
./astrouni2026       # Linux/macOS
astrouni2026.exe     # Windows
```

## Project structure

```
astrouni2026/
├── CMakeLists.txt           # CMake build configuration
├── src/
│   ├── main.cpp             # Entry point
│   ├── core/                # Calculation engine
│   │   ├── constants.h      # Constants
│   │   ├── data_types.h     # RADIX, ORTE, AU_INIT
│   │   ├── calculations.h   # Math
│   │   ├── chart_calc.h     # Chart calculation
│   │   ├── transit_calc.h   # Transit calculation
│   │   └── swiss_eph.h      # Swiss Ephemeris wrapper
│   ├── data/                # Data I/O
│   │   ├── legacy_io.h      # .dat files
│   │   ├── orte_db.h        # Locations database
│   │   └── person_db.h      # Person database
│   └── gui/                 # Qt6 GUI
│       ├── main_window.h
│       ├── radix_window.h
│       ├── chart_widget.h
│       └── dialogs/         # Dialogs
├── swisseph/                # Swiss Ephemeris (embedded)
│   ├── src/                 # Source code (cross-platform)
│   └── ephe/                # Ephemeris files
├── data/                    # Legacy .dat files
├── tests/                   # Unit tests
└── docs/                    # Documentation
```

## API example (Qt6 version)

```cpp
#include "core/swiss_eph.h"
#include "core/chart_calc.h"
#include "core/calculations.h"

using namespace astro;

// Initialize Swiss Ephemeris
swissEph().setEphePath("swisseph/ephe");

// Prepare radix data
Radix radix;
radix.rFix.vorname = "Max";
radix.rFix.name = "Mustermann";
radix.rFix.tag = 15;
radix.rFix.monat = 6;
radix.rFix.jahr = 1990;
radix.rFix.zeit = 14.5;  // 14:30
radix.rFix.breite = 52.52;  // Berlin
radix.rFix.laenge = 13.40;
radix.rFix.zone = 1.0;  // CET
radix.hausSys = TYP_PLACIDUS;

// Calculate chart
ChartCalc::calculate(radix);

// Print planets
for (int i = 0; i < radix.anzahlPlanet; ++i) {
    QString pos = Calculations::degToString(radix.planet[i], true, true);
    qDebug() << "Planet" << i << ":" << pos;
}
```

## Technology stack

- **C++20** - Modern C++ standard
- **Qt6 ≥ 6.2** - GUI framework (cross-platform)
- **CMake 3.16+** - Build system
- **Swiss Ephemeris 2.10** - Embedded (no external dependency)

## License

**GNU General Public License v2.0**

AstroUniverse is free software under the GPL v2 license.

- **License:** [LICENSE](LICENSE) (GPL v2)
- **Copyright:** [COPYRIGHT](COPYRIGHT)
- **SPDX:** `GPL-2.0-or-later`

### Third-party components

- **Swiss Ephemeris** - GPL v2 / LGPL (Astrodienst AG)
- **Qt6** - LGPL v3 (The Qt Company)

### Swiss Ephemeris
- **GPL v2** or **LGPL** for open source
- **Commercial license** available (~CHF 750)
- See: https://www.astro.com/swisseph/

### Requirements
- GCC 11+ or Clang 15+ or MSVC 2022 (C++20)
- CMake 3.16+
- Qt6 ≥ 6.2

### Build options

```bash
cd astrouni2026

# Debug build
cmake -DCMAKE_BUILD_TYPE=Debug -B build-debug
cmake --build build-debug

# Release build (optimized)
cmake -DCMAKE_BUILD_TYPE=Release -B build
cmake --build build

# Without tests
cmake -DBUILD_TESTS=OFF -B build
```

## Contributing

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add amazing feature'`)
4. Push the branch (`git push origin feature/amazing-feature`)
5. Open a pull request

## Contact

- **Project**: AstroUniverse
- **Status**: Qt6 GUI in development
- **Version**: 2026.1.0

## Credits

- **Original AstroUnivers v0.04Beta**: legacy Windows software (16-bit Borland C++)
- **Swiss Ephemeris**: Astrodienst Zürich (https://www.astro.com/swisseph/)
- **Port**: Modern C++20 with Qt6

---

*As of: December 2025*
