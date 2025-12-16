## AstroUniverse 2026 – Binary Releases

Existing `.dat` files from the legacy installation (AstroUnivers v0.04Beta) can be reused by copying them into the `astrouni2026/data/` directory.

[Deutsch / German Release Notes](RELEASE_NOTES.md)

## Downloads

### 🪟 Windows (64-bit)

**File:** `astrouni2026-windows-release.zip`  

**Contents:**

- `astrouni2026.exe`
- required Qt6 DLLs (Core, Gui, Widgets, PrintSupport)
- MinGW runtime DLLs
- minimal data files (`data/…`)
- Swiss Ephemeris files (`swisseph/ephe/*.se1`)
- resources (icons etc.)

**Start:**

1. Unzip the ZIP into any folder (e.g. `C:\Program Files\astrouni2026`).
2. If desired: copy existing `.dat` files from the legacy installation into `data/`.
3. Start `astrouni2026.exe` directly from that folder.

No separate Qt installation is required.  
If SmartScreen appears: choose “More info → Run anyway”.

---

### 🐧 Linux (64-bit)

**File:** `astrouni2026-linux-release.zip`  

**Contents:**

- `astrouni2026`
- `run_astrouni2026.sh` (launcher)
- minimal data files (`data/…`)
- Swiss Ephemeris files (`swisseph/ephe/*.se1`)
- resources (icons etc.)
- if available at build time: Qt6 libraries and plugins in the subdirectory  
  `lib/` and `plugins/` (or bundled system-dependently via `qtpaths6`/`patchelf`)

**Start:**

```bash
unzip astrouni2026-linux-release.zip
cd <unpacked_folder>/dist/linux
chmod +x astrouni2026
chmod +x run_astrouni2026.sh
./run_astrouni2026.sh
```

`run_astrouni2026.sh` is recommended because it starts independently of the current working directory and sets the paths for included Qt libraries and Qt plugins (`lib/`, `plugins/`).

If the included Qt libs are not sufficient or were not bundled, Qt6 must be provided by the system, e.g. on Debian/Ubuntu:

```bash
sudo apt install qt6-base-dev qt6-tools-dev
```

Optional: copy existing `.dat` files from the legacy installation into `data/`.

---

### 🍎 macOS (64-bit, Intel & Apple Silicon)

**File:** `AstroUniverse2026-macos.dmg`  

**Contents (inside the DMG volume):**

- `AstroUniverse2026.app`
- `data/` (minimal data files)
- `swisseph/ephe/*.se1` (Swiss Ephemeris files)
- `resources/` (icons etc.)
- if available at build time: embedded Qt6 frameworks and plugins in `AstroUniverse2026.app`  
  (via `macdeployqt` or fallback bundling via `qtpaths6`)

**Installation & start:**

1. Double-click `AstroUniverse2026-macos.dmg`, volume “AstroUniverse2026” will be mounted.
2. Copy the full contents (at least `AstroUniverse2026.app`, `data/`, `swisseph/`, `resources/`) into any folder, e.g. `~/Applications/astro_universe2026/`.
3. Optional: copy existing `.dat` files from the legacy installation into `data/`.
4. Start `AstroUniverse2026.app` via double-click in Finder.

No separate Qt installation is required, as long as `macdeployqt` or `qtpaths6` was available at build time and the frameworks were bundled into the app.  
If Gatekeeper blocks the app: use “right click → Open” and confirm the start once.
