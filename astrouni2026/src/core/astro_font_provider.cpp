/**
 * @file astro_font_provider.cpp
 * @brief Implementierung des Astrologie-Symbol-Providers
 */

#include "astro_font_provider.h"
#include <QFontDatabase>
#include <QDir>
#include <QCoreApplication>
#include <QDebug>

namespace astro {

AstroFontProvider& AstroFontProvider::instance() {
    static AstroFontProvider instance;
    return instance;
}

AstroFontProvider::AstroFontProvider()
    : m_hasAstroFont(false)
    , m_fontName("Sans Serif")
{
    initSymbols();
    detectFont();
}

void AstroFontProvider::detectFont() {
    // Versuche den Font aus dem Ressourcen-Verzeichnis zu laden
    QString appPath = QCoreApplication::applicationDirPath();
    QString fontPath = appPath + "/resources/fonts/asu_____.ttf";
    
    // Fallback-Pfade
    QStringList fontPaths = {
        fontPath,
        appPath + "/../resources/fonts/asu_____.ttf",
        ":/fonts/asu_____.ttf"  // Qt Resource
    };
    
    for (const QString& path : fontPaths) {
        if (QDir().exists(path)) {
            int fontId = QFontDatabase::addApplicationFont(path);
            if (fontId != -1) {
                QStringList families = QFontDatabase::applicationFontFamilies(fontId);
                if (!families.isEmpty()) {
                    m_fontName = families.first();
                    m_hasAstroFont = true;
                    qInfo() << "AstroFontProvider: AstroUniverse-Font geladen aus:" << path;
                    return;
                }
            }
        }
    }
    
    // Prüfe ob der Font bereits im System installiert ist
    if (QFontDatabase::families().contains("AstroUniverse")) {
        m_fontName = "AstroUniverse";
        m_hasAstroFont = true;
        qInfo() << "AstroFontProvider: AstroUniverse-Font im System gefunden";
        return;
    }
    
    qInfo() << "AstroFontProvider: AstroUniverse-Font nicht gefunden, verwende Unicode-Symbole";
}

void AstroFontProvider::initSymbols() {
    // Legacy-Font Zeichen aus auwurzel.h szLNANamen[]
    // Diese Zeichen sind im AstroUniverse-Font auf ASCII-Positionen gemappt
    // Original: "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", "<", "L", "Ch", "C", "P", "J", "V"
    // Die tatsächlichen Glyphen im Font sind auf diese Positionen:
    m_legacyPlanetSymbols = {
        "Q",   // Sonne
        "R",   // Mond
        "S",   // Merkur
        "T",   // Venus
        "U",   // Mars
        "V",   // Jupiter
        "W",   // Saturn
        "X",   // Uranus
        "Y",   // Neptun
        "Z",   // Pluto
        "<",   // Nordknoten
        "L",   // Lilith
        "K",   // Chiron (angepasst)
        "C",   // Ceres
        "P",   // Pallas
        "J",   // Juno
        "v"    // Vesta
    };
    
    // Legacy Sternzeichen (szBNamen aus auwurzel.h)
    // Auf ASCII-Positionen A-L gemappt
    m_legacySternzeichenSymbols = {
        "A",   // Widder
        "B",   // Stier
        "C",   // Zwillinge
        "D",   // Krebs
        "E",   // Löwe
        "F",   // Jungfrau
        "G",   // Waage
        "H",   // Skorpion
        "I",   // Schütze
        "J",   // Steinbock
        "K",   // Wassermann
        "L"    // Fische
    };
    
    // Legacy Aspekt-Symbole (cAspektGly aus auwurzel.h)
    m_legacyAspektSymbols = {
        "q",   // Konjunktion
        "y",   // Halbsextil
        "x",   // Sextil
        "r",   // Quadratur
        "e",   // Trigon
        "i",   // Quincunx
        "w"    // Opposition
    };
    
    m_legacyRetrograde = "f";  // Retrograde-Symbol im Legacy-Font
    m_legacyGrad = "`";        // Grad-Symbol
    
    // Unicode-Fallback Symbole (aus constants.h)
    m_unicodePlanetSymbols = {
        "☉",   // Sonne (U+2609)
        "☽",   // Mond (U+263D)
        "☿",   // Merkur (U+263F)
        "♀",   // Venus (U+2640)
        "♂",   // Mars (U+2642)
        "♃",   // Jupiter (U+2643)
        "♄",   // Saturn (U+2644)
        "♅",   // Uranus (U+2645)
        "♆",   // Neptun (U+2646)
        "♇",   // Pluto (U+2647)
        "☊",   // Nordknoten (U+260A)
        "⚸",   // Lilith (U+26B8)
        "⚷",   // Chiron (U+26B7)
        "⚳",   // Ceres (U+26B3)
        "⚴",   // Pallas (U+26B4)
        "⚵",   // Juno (U+26B5)
        "⚶"    // Vesta (U+26B6)
    };
    
    m_unicodeSternzeichenSymbols = {
        "♈",   // Widder (U+2648)
        "♉",   // Stier (U+2649)
        "♊",   // Zwillinge (U+264A)
        "♋",   // Krebs (U+264B)
        "♌",   // Löwe (U+264C)
        "♍",   // Jungfrau (U+264D)
        "♎",   // Waage (U+264E)
        "♏",   // Skorpion (U+264F)
        "♐",   // Schütze (U+2650)
        "♑",   // Steinbock (U+2651)
        "♒",   // Wassermann (U+2652)
        "♓"    // Fische (U+2653)
    };
    
    m_unicodeAspektSymbols = {
        "☌",   // Konjunktion (U+260C)
        "⚺",   // Halbsextil (U+26BA)
        "⚹",   // Sextil (U+26B9)
        "□",   // Quadratur (U+25A1)
        "△",   // Trigon (U+25B3)
        "⚻",   // Quincunx (U+26BB)
        "☍"    // Opposition (U+260D)
    };
    
    m_unicodeRetrograde = "℞";  // Rx-Symbol (U+211E)
    m_unicodeGrad = "°";        // Grad-Symbol (U+00B0)
}

QFont AstroFontProvider::getSymbolFont(int pointSize) const {
    QFont font(m_fontName, pointSize);
    return font;
}

QString AstroFontProvider::planetSymbol(int planet) const {
    if (planet < 0) return "";
    
    if (m_hasAstroFont) {
        if (planet < m_legacyPlanetSymbols.size()) {
            return m_legacyPlanetSymbols[planet];
        }
    } else {
        if (planet < m_unicodePlanetSymbols.size()) {
            return m_unicodePlanetSymbols[planet];
        }
    }
    return "";
}

QString AstroFontProvider::sternzeichenSymbol(int zeichen) const {
    if (zeichen < 0 || zeichen >= 12) return "";
    
    if (m_hasAstroFont) {
        return m_legacySternzeichenSymbols[zeichen];
    } else {
        return m_unicodeSternzeichenSymbols[zeichen];
    }
}

QString AstroFontProvider::aspektSymbol(int aspekt) const {
    if (aspekt < 0) return "";
    
    // Aspekt-Index aus Grad-Wert ermitteln
    int idx = -1;
    switch (aspekt) {
        case 0:   idx = 0; break;  // Konjunktion
        case 30:  idx = 1; break;  // Halbsextil
        case 60:  idx = 2; break;  // Sextil
        case 90:  idx = 3; break;  // Quadratur
        case 120: idx = 4; break;  // Trigon
        case 150: idx = 5; break;  // Quincunx
        case 180: idx = 6; break;  // Opposition
        default:
            // Direkter Index wenn < 7
            if (aspekt < 7) idx = aspekt;
            break;
    }
    
    if (idx < 0) return "";
    
    if (m_hasAstroFont) {
        if (idx < m_legacyAspektSymbols.size()) {
            return m_legacyAspektSymbols[idx];
        }
    } else {
        if (idx < m_unicodeAspektSymbols.size()) {
            return m_unicodeAspektSymbols[idx];
        }
    }
    return "";
}

QString AstroFontProvider::retrogradeSymbol() const {
    return m_hasAstroFont ? m_legacyRetrograde : m_unicodeRetrograde;
}

QString AstroFontProvider::gradSymbol() const {
    return m_hasAstroFont ? m_legacyGrad : m_unicodeGrad;
}

} // namespace astro
