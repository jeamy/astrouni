# Textanalyse-Feature für AstroUniverse

## Übersicht

Automatische Interpretation von Radix-Horoskopen mit detaillierten deutschen Texten.

## Features

### ✅ Implementiert

1. **Button "📄 Textanalyse"**
   - Rechts in der Daten-Liste im Radix-Fenster
   - Blau hervorgehoben
   - Öffnet Dialog mit vollständiger Analyse

2. **Analysierte Elemente**
   - 🌟 **Sonnenzeichen** (Hauptthema der Persönlichkeit)
   - 🌅 **Aszendent** (äußere Erscheinung, erste Wirkung)
   - 🌙 **Mondzeichen** (emotionale Bedürfnisse)
   - ✨ **Aspekte** (bis zu 10 wichtigste Planetenbeziehungen)

3. **Textqualität**
   - Professionelle astrologische Interpretationen
   - Alle 12 Sternzeichen vollständig beschrieben
   - Spezifische Aspekt-Deutungen (z.B. Sonne-Mond-Konjunktion)
   - Generische Aspekt-Beschreibungen als Fallback

4. **Darstellung**
   - HTML-formatiert mit Styling
   - Astrologische Symbole (♈ Widder, ☉ Sonne, ☌ Konjunktion, etc.)
   - Hierarchische Gliederung (H1, H2, H3)
   - Lesbare Typografie mit Arial, 11pt

5. **Export-Funktion**
   - "Als HTML speichern" Button
   - Automatischer Dateiname: `Vorname_20251214_Analyse.html`
   - UTF-8 Encoding
   - Standalone HTML-Datei

## Verwendung

### Im Programm

1. Radix-Horoskop öffnen oder erstellen
2. Button **"📄 Textanalyse"** klicken (rechts, unter den Daten)
3. Dialog mit vollständiger Analyse öffnet sich
4. Optional: "Als HTML speichern" für Export

### Technisch

```cpp
// Textanalyse erstellen
#include "../core/astro_text_analyzer.h"

AstroTextAnalyzer analyzer;
QString html = analyzer.analyzeRadix(radix);

// Einzelne Elemente
QString sunText = analyzer.analyzeSunSign(signIndex);
QString ascText = analyzer.analyzeAscendant(signIndex);
QString aspectText = analyzer.analyzeAspect(planet1, planet2, aspect);
```

## Code-Struktur

### Neue Dateien

```
src/core/
├── astro_text_analyzer.h     # Header (Interface)
└── astro_text_analyzer.cpp   # Implementation (Texte)
```

### Geänderte Dateien

```
src/gui/
├── radix_window.h             # Button + Slot deklariert
└── radix_window.cpp           # UI + Dialog implementiert

src/core/
└── CMakeLists.txt             # Build-Konfiguration
```

## Textbeispiel

```html
<h2>🌟 Sonnenzeichen: Widder</h2>
<p class='sign-symbol'>♈</p>
<p>Als <b>Widder</b> sind Sie ein geborener Pionier. 
Ihre Energie ist direkt, spontan und mutig...</p>

<h3>☉ Sonne ☌ Konjunktion ☽ Mond</h3>
<p>Die Verbindung von Sonne und Mond zeigt Einheit 
von Wille und Gefühl. Sie sind im Einklang mit sich selbst.</p>
```

## Erweiterungsmöglichkeiten

### Nächste Schritte (TODO)

- [ ] Transit-Interpretationen
- [ ] Synastrie-Texte (Partnerschaftsanalyse)
- [ ] Progressionen
- [ ] Häuser-Deutungen (Planet in Haus)
- [ ] Rückläufige Planeten
- [ ] Mehrsprachigkeit (EN, FR, IT)
- [ ] PDF-Export
- [ ] Druckfunktion
- [ ] Personalisierte Texte (Alter, Geschlecht berücksichtigen)
- [ ] Erweiterte Aspekt-Bibliothek
- [ ] KI-gestützte Textgenerierung

### Textbibliothek erweitern

```cpp
// In initializeAspectTexts()
m_aspectTexts[makeAspectKey(P_JUPITER, P_SATURN, KONJUNKTION)] =
    "<p>Große Zyklen und soziale Verantwortung...</p>";

m_aspectTexts[makeAspectKey(P_URANUS, P_NEPTUN, SEXTIL)] =
    "<p>Innovative Spiritualität...</p>";
```

### Neue Analysemethoden

```cpp
// Planet in Haus
QString analyzePlanetInHouse(int planet, int house);

// Planet rückläufig
QString analyzeRetrograde(int planet);

// Stellium (3+ Planeten in einem Zeichen)
QString analyzeStellium(const QVector<int>& planets, int sign);
```

## Test-Branch

Branch: `test`
Commit: `ab6439c`
GitHub: https://github.com/jeamy/astrouni/tree/test

## Lizenz

Wie AstroUniverse: GPL v2
