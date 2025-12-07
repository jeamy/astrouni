---
trigger: always_on
---

Du hast nichts zu tun, als den legacy code 1:1 zu übersetzen. keine eigenen ideen, keine änderungen am design. kein zusätzlicher code oder dialoge, menüs, etc.
PORTIERUNGSPLAN in docs ist die ausgangslage
das ist deine aufgabe
*Herausforderungen:**
- 16-bit → 32/64-bit Typen-Anpassung.
- Borland C++ → Moderner C++ Compiler (GCC/Clang/MSVC).
- Windows API → Cross-Platform wxWidgets.
- Entfernung von Hardcodierten Pfaden.

## Portierungs-Strategie: 1:1 Mapping

Jede C-Datei und Funktion aus dem `legacy/` Verzeichnis wird einer entsprechenden C++-Datei und Klasse/Funktion zugeordnet.