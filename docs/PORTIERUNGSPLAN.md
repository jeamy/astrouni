# Portierungsplan: AstroUniverse 1:1 Legacy-Portierung

**Ziel:** Strikte 1:1-Portierung der 16-bit Borland C++ Anwendung "AstroUniverse v0.04Beta" nach modernem C++20 mit wxWidgets. Es werden **keine neuen Ideen oder Architekturen** eingeführt. Funktionalität, UI-Verhalten und Datenverarbeitung werden exakt nachgebildet.

**Herausforderungen:**
- 16-bit → 32/64-bit Typen-Anpassung.
- Borland C++ → Moderner C++ Compiler (GCC/Clang/MSVC).
- Windows API → Cross-Platform wxWidgets.
- Entfernung von Hardcodierten Pfaden.

## Portierungs-Strategie: 1:1 Mapping

Jede C-Datei und Funktion aus dem `legacy/` Verzeichnis wird einer entsprechenden C++-Datei und Klasse/Funktion zugeordnet.

### Projektstruktur (1:1-Mapping)

```
astrouni/
├── CMakeLists.txt       # Haupt-Build-Konfiguration
│
├── core/                # Portierung von Logik & Daten (astro.h, audef.h)
│   ├── include/astro/
│   │   ├── data_types.h   # RADIX, ORTE, etc. als C++ structs
│   │   └── constants.h    # CM_U_*, etc. als enums
│   └── src/
│
├── gui/                 # Portierung von UI-Logik (astrouni.c, astrofil.c)
│   ├── main.cpp         # Port von WinMain
│   ├── main_frame.h/cpp # Port von MainWndProc
│   ├── dialogs/         # Port der Dlg*Proc-Funktionen
│   └── rendering/       # Port von auwurzel.c (Zeichenlogik)
│
├── database/            # Port von Daten-Handling (astrofil.c, auhelper.c)
│   └── legacy_data_manager.h/cpp # sReadDat, sWriteDat, sReadIni etc.
│
└── tests/               # Unit-Tests für portierte Logik
```

### Funktions- & Datei-Mapping

| Legacy-Datei | Zweck | Moderne Entsprechung |
|:---|:---|:---|
| `astrouni.c` | App-Struktur, Event-Handler | `gui/main.cpp`, `gui/main_frame.cpp` |
| `astrofil.c` | Dialog-Logik, Datei-I/O | `gui/dialogs/*`, `database/*` |
| `auwurzel.c` | Radix-Zeichnung | `gui/rendering/radix_renderer.cpp` |
| `auhelper.c` | Einstellungs- & Farb-Handling | `database/legacy_data_manager.cpp` |
| `aucalc.c` | Astronomische Berechnungen | `core/src/calculations.cpp` |

### Daten-Handling (`.dat` Dateien)

Ein zentraler `LegacyDataManager` wird für das Lesen und Schreiben der folgenden Binärdateien verantwortlich sein, wobei die ursprüngliche Datenstruktur exakt beibehalten wird:

- **astroini.dat**: Persistente Einstellungen (Farben, Orben, Pfade).
- **astronam.dat**: Personendatenbank.
- **astroorg.dat**: Ortsdatenbank.
- **default.dat**: Standard-Einstellungen.
- **europa.dat**: Zusätzliche Ortsdaten.

## Nächste Schritte

1.  **Grundstruktur anlegen**: Erstellen der oben definierten Verzeichnis- und Dateistruktur.
2.  **Header-Portierung**: Portieren der zentralen Header (`astro.h`, `astrouni.h`) nach `core/include/astro/`, um Datentypen und Konstanten zu definieren.
3.  **Implementierung**: Beginn der 1:1-Implementierung, startend mit `gui/main.cpp` (`WinMain`).
4.  **Dokumentation**: Jeder Schritt wird in `docs/STATUS.md` festgehalten.
