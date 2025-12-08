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
    // Case-insensitive Suche, da Windows den Font-Namen anders registrieren kann
    QStringList families = QFontDatabase::families();
    for (const QString& family : families) {
        if (family.compare("AstroUniverse", Qt::CaseInsensitive) == 0) {
            m_fontName = family;  // Verwende den exakten Namen aus der DB
            m_hasAstroFont = true;
            qInfo() << "AstroFontProvider: AstroUniverse-Font im System gefunden:" << family;
            return;
        }
    }
    
    qInfo() << "AstroFontProvider: AstroUniverse-Font nicht gefunden, verwende Unicode-Symbole";
    qDebug() << "AstroFontProvider: Verfügbare Fonts:" << families.filter("Astro", Qt::CaseInsensitive);
}

void AstroFontProvider::initSymbols() {
    // Legacy-Font Zeichen aus auwurzel.h szLNANamen[]
    // Hex-Werte aus dem Original: 0xAA-0xBA
    // Diese Zeichen sind im AstroUniverse-Font auf Latin1 Extended Positionen gemappt
    m_legacyPlanetSymbols = {
        QString(QChar(0xAA)),   // Sonne
        QString(QChar(0xAB)),   // Mond
        QString(QChar(0xAC)),   // Merkur
        QString(QChar(0xAD)),   // Venus
        QString(QChar(0xAE)),   // Mars
        QString(QChar(0xAF)),   // Jupiter
        QString(QChar(0xB0)),   // Saturn
        QString(QChar(0xB1)),   // Uranus
        QString(QChar(0xB2)),   // Neptun
        QString(QChar(0xB3)),   // Pluto
        QString(QChar(0xB4)),   // Nordknoten
        QString(QChar(0xB5)),   // Lilith
        QString(QChar(0xB6)),   // Chiron
        QString(QChar(0xB7)),   // Ceres
        QString(QChar(0xB8)),   // Pallas
        QString(QChar(0xB9)),   // Juno
        QString(QChar(0xBA))    // Vesta
    };
    
    // Legacy Sternzeichen (szBNamen aus auwurzel.h)
    // Hex-Werte aus dem Original: 0xE5-0xF0
    m_legacySternzeichenSymbols = {
        QString(QChar(0xE5)),   // Widder
        QString(QChar(0xE6)),   // Stier
        QString(QChar(0xE7)),   // Zwillinge
        QString(QChar(0xE8)),   // Krebs
        QString(QChar(0xE9)),   // Löwe
        QString(QChar(0xEA)),   // Jungfrau
        QString(QChar(0xEB)),   // Waage
        QString(QChar(0xEC)),   // Skorpion
        QString(QChar(0xED)),   // Schütze
        QString(QChar(0xEE)),   // Steinbock
        QString(QChar(0xEF)),   // Wassermann
        QString(QChar(0xF0))    // Fische
    };
    
    // Legacy Aspekt-Symbole (cAspektGly aus auwurzel.h)
    // Hex-Werte: 0x91, 0x97, 0x93, 0x94, 0x92, 0x95, 0x96
    m_legacyAspektSymbols = {
        QString(QChar(0x91)),   // Konjunktion
        QString(QChar(0x97)),   // Halbsextil
        QString(QChar(0x93)),   // Sextil
        QString(QChar(0x94)),   // Quadratur
        QString(QChar(0x92)),   // Trigon
        QString(QChar(0x95)),   // Quincunx
        QString(QChar(0x96))    // Opposition
    };
    
    m_legacyRetrograde = QString(QChar(0x9A));  // Retrograde-Symbol im Legacy-Font
    m_legacyGrad = QString(QChar(0xB0));        // Grad-Symbol (gleich wie Saturn, prüfen!)
    
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
    // AstroUniverse-Font 4pt größer darstellen für bessere Lesbarkeit
    int adjustedSize = m_hasAstroFont ? pointSize + 5 : pointSize;
    QFont font(m_fontName, adjustedSize);
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
