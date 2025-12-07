# Legacy Code Analyse: Benötigte Dateien

## Zusammenfassung

Nach detaillierter Analyse der Abhängigkeiten und Funktionsaufrufe:

## ✅ Kern-Dateien (ESSENTIELL)

### 1. Zentrale Header & Strukturen
- **`astrouni.h`** (40KB) - **HAUPTHEADER**
  - Alle Datenstrukturen (`RADIX`, `ORTE`, `AU_INIT`)
  - Deutsche Textkonstanten (Planeten, Zeichen, Häuser)
  - Funktionsdeklarationen für alle Module
  - Windows-spezifische Definitionen

### 2. Hauptprogramm & GUI
- **`astrouni.c`** (23KB) - **HAUPTPROGRAMM**
  - `WinMain()` - Windows Einstiegspunkt
  - GUI Initialisierung
  - Event Handler
  - Globale Variablen: `hInstance`, `hWndMain`

### 3. Astrologie-Berechnungen (KERN)
- **`auwurzel.h`** (4KB) - **BERECHNUNGS-HEADER**
  - Konstanten für Berechnungen
  - Externe Variablen-Deklarationen
  
- **`auwurzel.c`** (53KB) - **HAUPTBERECHNUNGS-ENGINE** ⭐
  - `sCalcChart()` - Hauptberechnungsfunktion
  - `sCalcHauser()` - Häusersystem-Berechnung
  - `vCalcPlanets()` - Planetenpositionen
  - `vKoch()`, `vHausPlacidus()`, `vEqual()` - Häusersysteme
  - `vRegiomontanus()`, `vCampanus()`, `vTopocentric()` - weitere Systeme
  - `vPorphyry()`, `vMorinus()`, `vAlcabitius()`, `vWhole()` - moderne Systeme
  - `sHaus()` - Häuser zeichnen
  - `sPlanet()` - Planeten zeichnen
  - `sRadix()` - Radix-Chart zeichnen
  - `vCalcWinkel()` - Winkelberechnungen
  - `vCalcAsp()` - Aspekt-Berechnung

- **`aucalc.h`** (12KB) - **MATHEMATIK-HEADER**
  - Konstanten (PI, DEG2RAD, etc.)
  - Externe Funktionsdeklarationen
  
- **`aucalc.c`** (24KB) - **MATHEMATISCHE BASIS**
  - `dJulTag()` - Julianisches Datum
  - `dMod()`, `dModRad()` - Modulo-Operationen
  - `dWinkel()` - Winkelberechnungen
  - `vGrad2GMS()` - Grad zu Grad/Minuten/Sekunden
  - `vPolToRec()`, `vRecToPol()` - Koordinaten-Konvertierung

### 4. Ephemeriden-Interface
- **`aupla.c`** (30KB) - **PLACALC INTEGRATION**
  - Ephemeriden-Datei-Zugriff
  - `lrz_file_posit()` - Datei-Positionierung
  - `outer_hel()` - Äußere Planeten
  - Interface zu Ephemeriden-Datenbank

- **`aupla1.c`** (33KB) - **ERWEITERTE PLACALC**
  - Genauere Berechnungen
  - Copyright Astrodienst AG
  - Mercury-Fix für -2100 bis -3100

### 5. Hilfsfunktionen & Utilities
- **`auhelper.c`** (68KB) - **HILFSFUNKTIONEN**
  - String-Operationen
  - Farb-Management
  - Datum/Zeit-Formatierung
  - `vSetHausOrb()` - Häuser-Orbis setzen
  - `sFormatTime()`, `sFormatDatum()` - Zeit/Datum formatieren
  - GUI-Hilfsfunktionen

### 6. Ausgabe & Rendering
- **`auprint.c`** (29KB) - **AUSGABE-FORMATIERUNG** ⭐
  - Text-Generierung für Charts
  - Druckfunktionen
  - Globale Arrays: `szHausTyp[][]`, `szHoroTyp[][]`
  - `sPColor[COLORS]` - Druck-Farben

- **`aupr1int.c`** (22KB) - **CHART-RENDERING**
  - Detaillierte Chart-Darstellung
  - Planeten-Positionen ausgeben
  - Häuser-Tabelle zeichnen
  - Aspekt-Tabelle zeichnen

### 7. Daten-I/O
- **`io.c`** (23KB) - **DATEI-I/O**
  - .dat Files laden/speichern
  - Ephemeriden-Zugriff
  - Kommentar: "planetary calculation routines from Swiss Ephemeris"

- **`astrofil.c`** (83KB) - **DATENBANK-VERWALTUNG**
  - Radix-Daten speichern/laden
  - Personen-Datenbank
  - Orte-Datenbank

### 8. GUI-Komponenten
- **`aulistbo.c`** (13KB) - **LISTENBOX-HANDLING**
  - Custom Listbox für Radix-Daten
  - Orte-Listbox
  - Globale Variable: `lbcolor`

### 9. Transit-Berechnungen
- **`autransi.c`** (15KB) - **TRANSIT-BERECHNUNGEN**
  - Transit-Planeten berechnen
  - `sRucklauf()` - Rückläufigkeit
  - `sIncDate()` - Datum inkrementieren

## ⚠️ Duplikate & Kopien (IGNORIEREN)

- **`auwurze1.c`** (50KB) - Kopie/Backup von `auwurzel.c`
  - Unterscheidet sich von `auwurzel.c`
  - Wahrscheinlich ältere Version
  - **NICHT BENÖTIGT**

## 📦 Daten-Dateien (KRITISCH)

### Ephemeriden & Datenbanken
- **`astroeph.dat`** (450KB) - **PLANETEN-EPHEMERIDEN** ⭐⭐⭐
  - Binäre Planetendaten
  - 80-Tage Schritte
  - Struktur: `EPHEM` mit Offsets für Planeten/Asteroiden/Chiron

- **`astroger.dat`** (271KB) - **DEUTSCHE TEXTE**
  - Lokalisierung
  - Interpretations-Texte

- **`default.dat`** (40KB) - **STANDARD-EINSTELLUNGEN**
  - Farben (Screen/Print/Mono)
  - Orbis-Werte
  - Default-Konfiguration

- **`astroini.dat`** (44KB) - **INITIALISIERUNG**
  - Benutzer-Einstellungen
  - Gespeicherte Konfiguration

- **`astronam.dat`** (1.8KB) - **NAMEN-DATENBANK**
  - Personen-Namen

- **`astroorg.dat`** (179KB) - **ORTE-DATENBANK**
  - Geografische Koordinaten
  - Zeitzonen

- **`europa.dat`** (213KB) - **EUROPA-ORTE**
  - Erweiterte Orte-Datenbank

## 🚫 Nicht benötigte Dateien

### Legacy Astrolog-Dateien
- **`astro.h`** / **`astrolog.h`** (46KB) - Astrolog32 Headers
  - Nicht von AstroUni verwendet
  - Nur für Referenz

- **`extern.h`** (23KB) - Externe Deklarationen
  - Möglicherweise für Astrolog32
  - Nicht direkt in AstroUni inkludiert

### Build & Ressourcen
- **`astrouni.def`** (273 Bytes) - Windows DEF File
  - Nur für 16-bit Windows Build
  
- **`astrouni.rc`** (48KB) - Windows Ressourcen
  - Dialoge, Menüs, Strings
  - Nur für GUI-Rekonstruktion interessant

- **`astrouni.prj`** (9KB) - Borland Projekt-File
  - Build-Konfiguration
  - Nicht für Portierung benötigt

- **`astroun1.def`** / **`astroun1.rc`** - Alternative Ressourcen
  - Varianten/Backups

### Utility-Programme
- **`reorder.c`** (3.4KB) - Daten-Konverter
  - Erstellt `astroeph.dat` aus Einzel-Files
  - Nur für Daten-Vorbereitung

- **`decrypt.c`** (667 Bytes) - Entschlüsselung
  - Wahrscheinlich ungenutzt

- **`func.c`** (1.9KB) - Mini-Funktionen
  - Sehr klein, möglicherweise Test-Code

### Ressourcen
- **`asu_____.ttf`** (51KB) - Astrologie-Font
  - Glyphen für Planeten/Zeichen
  - Optional für GUI

- **`main.ico`** / **`radix.ico`** - Icons
  - Nur für Windows GUI

- **`datenb.rc`** / **`dateng.rc`** - Ressourcen-Varianten

### Weitere Header
- **`astrounr.h`** (10KB) - Ressourcen-Header
  - Nur für Windows-Build

- **`audef.h`** (327 Bytes) - Zusätz-Definitionen
  - Sehr klein, möglicherweise leer

## 📋 Abhängigkeits-Hierarchie

```
astrouni.c (WinMain)
  └─ astrouni.h (Hauptheader)
       ├─ auwurzel.h
       │    └─ auwurzel.c ⭐ (Kern-Berechnungen)
       │         ├─ aucalc.h
       │         │    └─ aucalc.c (Mathematik)
       │         └─ aupla.c (Ephemeriden)
       │              └─ aupla1.c (Erweitert)
       ├─ auhelper.c (Utilities)
       ├─ auprint.c (Ausgabe)
       ├─ aupr1int.c (Rendering)
       ├─ io.c (Datei-I/O)
       ├─ astrofil.c (Datenbank)
       ├─ aulistbo.c (GUI)
       └─ autransi.c (Transite)
```

## 🎯 Minimaler Satz für Core Library

**Für reine Berechnungs-Engine (ohne GUI):**

```
Core Berechnungen:
├── astrouni.h          # Strukturen & Konstanten
├── auwurzel.h/c        # Hauptberechnungen ⭐
├── aucalc.h/c          # Mathematik
├── aupla.c             # Ephemeriden-Interface
└── aupla1.c            # Erweiterte Ephemeriden

Daten:
└── astroeph.dat        # Planeten-Datenbank ⭐⭐⭐
```

**Für vollständige Portierung (mit GUI):**

```
Alle oben + :
├── astrouni.c          # Hauptprogramm
├── auhelper.c          # Hilfsfunktionen
├── auprint.c           # Ausgabe
├── aupr1int.c          # Rendering
├── io.c                # Datei-I/O
├── astrofil.c          # Datenbank
├── aulistbo.c          # GUI-Komponenten
├── autransi.c          # Transite

Daten:
├── astroeph.dat        # Ephemeriden
├── astroger.dat        # Deutsche Texte
├── default.dat         # Einstellungen
├── astroini.dat        # Konfiguration
├── astronam.dat        # Namen
└── astroorg.dat        # Orte
```

## ✅ Bestätigung

**JA, Sie hatten Recht!**

Die Hauptdateien sind tatsächlich:
- **`auwurzel.c/h`** - Kern der Astrologie-Berechnungen
- **`auprint.c`** - Ausgabe und Formatierung
- **`astrouni.c/h`** - Hauptprogramm und Strukturen

Diese bilden das **Herz der Anwendung**. Alle anderen Dateien sind entweder:
- Unterstützend (Helper, I/O)
- GUI-spezifisch (Listboxen, Dialoge)
- Duplikate (`auwurze1.c`)
- Legacy/Ungenutzt (`astro.h`, `extern.h`)

---

*Stand: November 2025*  
*Analyse basierend auf Include-Hierarchie und Funktionsaufrufen*
