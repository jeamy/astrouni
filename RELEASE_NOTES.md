## AstroUniverse 2026 – Binär‑Releases

Vorhandene `.dat`‑Dateien aus der Legacy‑Installation (AstroUnivers v0.04Beta) können weiterverwendet werden, indem sie in das Verzeichnis `astrouni2026/data/` kopiert werden.

[English Release Notes](RELEASE_NOTES_EN.md)

## Downloads

### 🪟 Windows (64‑bit)

**Datei:** `astrouni2026-windows-release.zip`  

**Inhalt:**

- `astrouni2026.exe`
- benötigte Qt6‑DLLs (Core, Gui, Widgets, PrintSupport)
- MinGW‑Runtime‑DLLs
- minimale Datendateien (`data/…`)
- Swiss‑Ephemeris‑Dateien (`swisseph/ephe/*.se1`)
- Ressourcen (Icons etc.)

**Start:**

1. ZIP in einen beliebigen Ordner entpacken (z.B. `C:\Programme\astrouni2026`).
2. Falls gewünscht: vorhandene `.dat`‑Dateien aus der Legacy‑Installation nach `data/` kopieren.
3. `astrouni2026.exe` direkt aus diesem Ordner starten.

Es ist **keine separate Qt‑Installation** nötig.  
Bei SmartScreen ggf. „Weitere Informationen → Trotzdem ausführen“ wählen.

---

### 🐧 Linux (64‑bit)

Hinweis: Um GLIBC‑Kompatibilitätsprobleme zu vermeiden, sollte der Linux‑Release idealerweise in einer Ubuntu 20.04 Umgebung (glibc 2.31) gebaut werden (Docker‑Build: `build_linux_release_docker_ubuntu2004.sh`).

**Datei:** `astrouni2026-linux-release.zip`  

**Inhalt:**

- `astrouni2026`
- `run_astrouni2026.sh` (Launcher)
- minimale Datendateien (`data/…`)
- Swiss‑Ephemeris‑Dateien (`swisseph/ephe/*.se1`)
- Ressourcen (Icons etc.)
- falls beim Build verfügbar: Qt6‑Bibliotheken und Plugins im Unterverzeichnis  
  `lib/` und `plugins/` (bzw. systemabhängig über `qtpaths6`/`patchelf` gebündelt)

**Start:**

```bash
unzip astrouni2026-linux-release.zip
cd <entpackter_ordner>/dist/linux
chmod +x astrouni2026
chmod +x run_astrouni2026.sh
./run_astrouni2026.sh
```

`run_astrouni2026.sh` wird empfohlen, da es unabhängig vom aktuellen Working Directory startet und die Pfade für mitgelieferte Qt‑Bibliotheken und Qt‑Plugins (`lib/`, `plugins/`) setzt.

Falls die mitgelieferten Qt‑Libs nicht ausreichen oder nicht gebündelt wurden, muß Qt6 vom System bereitgestellt werden, z.B. unter Debian/Ubuntu:

```bash
sudo apt install qt6-base-dev qt6-tools-dev
```

Optional: vorhandene `.dat`‑Dateien aus der Legacy‑Installation nach `data/` kopieren.

---

### 🍎 macOS (64‑bit, Intel & Apple Silicon)

**Datei:** `AstroUniverse2026-macos.dmg`  

**Inhalt (im DMG‑Volume):**

- `AstroUniverse2026.app`
- `data/` (minimale Datendateien)
- `swisseph/ephe/*.se1` (Swiss‑Ephemeris‑Dateien)
- `resources/` (Icons etc.)
- sofern beim Build verfügbar: eingebettete Qt6‑Frameworks und Plugins in `AstroUniverse2026.app`  
  (per `macdeployqt` oder Fallback über `qtpaths6` gebündelt)

**Installation & Start:**

1. `AstroUniverse2026-macos.dmg` doppelt anklicken, Volume „AstroUniverse2026“ wird gemountet.
2. Den gesamten Inhalt (mindestens `AstroUniverse2026.app`, `data/`, `swisseph/`, `resources/`) in einen beliebigen Ordner kopieren, z.B. `~/Programme/astro_universe2026/`.
3. Optional: vorhandene `.dat`‑Dateien aus der Legacy‑Installation nach `data/` kopieren.
4. `AstroUniverse2026.app` per Doppelklick im Finder starten.

Es ist **keine separate Qt‑Installation** nötig, sofern beim Build `macdeployqt` oder `qtpaths6` verfügbar war und die Frameworks in die App gebündelt wurden.  
Falls Gatekeeper anschlägt, im Kontextmenü „Rechtsklick → Öffnen“ wählen und den Start einmalig bestätigen.
