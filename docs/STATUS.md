# Projekt-Status: AstroUniverse 1:1 Portierung

**Stand:** 8. November 2025, 11:30 UTC+1

## Phase 1: Projekt-Setup & Core-Struktur

- [x] **Strategie festgelegt**: Strikte 1:1-Portierung ohne neue Architekturen.
- [x] **Portierungsplan aktualisiert**: `PORTIERUNGSPLAN.md` spiegelt die 1:1-Strategie wider.
- [x] **Verzeichnisstruktur angelegt**: `core`, `gui`, `database`, `tests` sind erstellt.
- [x] **Build-System konfiguriert**: Funktionierende `CMakeLists.txt` für das gesamte Projekt und die Sub-Module.
- [x] **Core-Datentypen portiert**: `constants.h` und `data_types.h` aus `astrouni.h` und `astro.h` erstellt.
- [x] **GUI-Einstiegspunkt portiert**: `WinMain` wurde als `wxApp` in `gui/main.cpp` implementiert.
- [x] **Test-Framework integriert**: Google Test wird via `FetchContent` automatisch geladen.
- [x] **Build verifiziert**: Das Projekt kompiliert erfolgreich und die Tests werden ausgeführt.

## Phase 2: Daten-Logik (LegacyDataManager)

- [x] **`LegacyDataManager` implementiert**: Basis-Implementierung für das Lesen/Schreiben von Binärdaten erstellt.
- [x] **`astroini.dat` Handling**: `ReadIni` und `WriteIni` portiert und getestet.
- [x] **`astronam.dat` Handling**: `ReadPerson` und `WritePerson` portiert und getestet.
- [x] **`astroorg.dat` Handling**: `ReadOrte` und `WriteOrte` portiert und getestet.
- [ ] **Unit-Tests erweitern**: Tests für die verbleibenden `.dat`-Dateien hinzufügen.

## Phase 3: Dialog-Portierung

- [x] **Workaround für Menüleiste**: Temporäre Buttons im Hauptfenster ermöglichen das Aufrufen von Dialogen.
- [x] **Personen-Dialog**: Vollständiges UI-Layout portiert. Die Suchfunktionen für Personen (`PersonSearchDialog`) und Orte (`OrtSearchDialog`) sind implementiert und laden die Daten korrekt in den Hauptdialog. Diverse Sizer-Layout-Fehler wurden behoben.
- [x] **Orte-Dialog**: Vollständiges UI-Layout 1:1 aus Legacy-Code portiert.

## Blockierende Probleme

- **GUI `assert` Fehler**: Ein hartnäckiger, nicht reproduzierbarer `assert` in wxWidgets bei der Menü-Erstellung verhindert die Weiterentwicklung der GUI.
  - **Workaround**: Die Menüleiste wurde temporär im Code deaktiviert, um die Anwendung startfähig zu halten und die Portierung der Dialoge zu ermöglichen.
  - **Nächster Schritt**: Die Dialoge werden über temporäre Buttons im Hauptfenster aufgerufen.
