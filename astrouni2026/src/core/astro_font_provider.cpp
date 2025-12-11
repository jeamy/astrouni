/**
 * @file astro_font_provider.cpp
 * @brief Implementierung des Astrologie-Symbol-Providers
 */

#include "astro_font_provider.h"
#include <QFontDatabase>
#include <QFontInfo>
#include <QFontMetrics>
#include <QDir>
#include <QFileInfo>
#include <QFile>
#include <QTextStream>
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
    , m_hasSymbolFont(false)
    , m_symbolFontName("Sans Serif")
    , m_hasDejaVuFont(false)
    , m_dejaVuFontName("Sans Serif")
{
    initSymbols();
    detectFont();
}

void AstroFontProvider::detectFont() {
    QString appPath = QCoreApplication::applicationDirPath();
    QStringList families = QFontDatabase::families();
    
    // ========== 1. AstroUniverse-Font für Sternzeichen laden ==========
    
    // Prüfe ob der Font bereits im System installiert ist
    for (const QString& family : families) {
        if (family.compare("AstroUniverse", Qt::CaseInsensitive) == 0 ||
            family.contains("AstroUniverse", Qt::CaseInsensitive)) {
            m_fontName = family;
            m_hasAstroFont = true;
            qInfo() << "AstroFontProvider: AstroUniverse-Font im System gefunden:" << family;
            break;
        }
    }
    
    // Falls nicht im System, versuche aus Ressourcen zu laden
    if (!m_hasAstroFont) {
        QStringList fontPaths = {
            appPath + "/resources/fonts/asu_____.ttf",
            appPath + "/fonts/asu_____.ttf",
            appPath + "/../resources/fonts/asu_____.ttf",
            appPath + "/../Resources/fonts/asu_____.ttf",  // macOS Bundle: Contents/Resources/fonts
            ":/fonts/asu_____.ttf"
        };
        
        for (const QString& path : fontPaths) {
            QFileInfo fi(path);
            if (fi.exists() && fi.isFile()) {
                int fontId = QFontDatabase::addApplicationFont(path);
                if (fontId != -1) {
                    QStringList loadedFamilies = QFontDatabase::applicationFontFamilies(fontId);
                    if (!loadedFamilies.isEmpty()) {
                        m_fontName = loadedFamilies.first();
                        m_hasAstroFont = true;
                        qInfo() << "AstroFontProvider: AstroUniverse-Font geladen aus:" << path;
                        break;
                    }
                }
            }
        }
    }
    
    if (!m_hasAstroFont) {
        qInfo() << "AstroFontProvider: AstroUniverse-Font nicht gefunden, verwende Unicode-Symbole für Sternzeichen";
    }
    // ========== 2. DejaVu Sans laden (enthält Asteroiden-Glyphen U+26B3-U+26B8) ==========
    QStringList dejaVuFontPaths = {
        appPath + "/resources/fonts/DejaVuSans.ttf",
        appPath + "/fonts/DejaVuSans.ttf",
        appPath + "/../resources/fonts/DejaVuSans.ttf",
        appPath + "/../Resources/fonts/DejaVuSans.ttf",  // macOS Bundle: Contents/Resources/fonts
        ":/fonts/DejaVuSans.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/Library/Fonts/DejaVuSans.ttf",
        "/System/Library/Fonts/DejaVuSans.ttf"
    };
    for (const QString& path : dejaVuFontPaths) {
        QFileInfo fi(path);
        if (fi.exists() && fi.isFile()) {
            int fontId = QFontDatabase::addApplicationFont(path);
            if (fontId != -1) {
                QStringList loadedFamilies = QFontDatabase::applicationFontFamilies(fontId);
                if (!loadedFamilies.isEmpty()) {
                    m_dejaVuFontName = loadedFamilies.first();
                    m_hasDejaVuFont = true;
                    qInfo() << "AstroFontProvider: DejaVu Sans geladen aus:" << path;
                    break;
                }
            }
        }
    }
    if (!m_hasDejaVuFont) {
        for (const QString& family : families) {
            if (family.contains("DejaVu Sans", Qt::CaseInsensitive)) {
                m_dejaVuFontName = family;
                m_hasDejaVuFont = true;
                qInfo() << "AstroFontProvider: DejaVu Sans im System gefunden:" << family;
                break;
            }
        }
    }
    if (!m_hasDejaVuFont) {
        qInfo() << "AstroFontProvider: DejaVu Sans nicht gefunden";
    }
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
        QString(QChar(0xC5)),   // Widder
        QString(QChar(0xC6)),   // Stier
        QString(QChar(0xC7)),   // Zwillinge
        QString(QChar(0xC8)),   // Krebs
        QString(QChar(0xC9)),   // Löwe
        QString(QChar(0xCA)),   // Jungfrau
        QString(QChar(0xCB)),   // Waage
        QString(QChar(0xCC)),   // Skorpion
        QString(QChar(0xCD)),   // Schütze
        QString(QChar(0xCE)),   // Steinbock
        QString(QChar(0xCF)),   // Wassermann
        QString(QChar(0xD0))    // Fische
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
    
    // Aspekt-Symbole: Verwende weit verbreitete Unicode-Zeichen die in den meisten Fonts vorhanden sind
    m_unicodeAspektSymbols = {
        "☌",   // Konjunktion (U+260C) - in vielen Fonts
        "∠",   // Halbsextil (U+2220 ANGLE) - Ersatz für U+26BA, weit verbreitet
        "✱",   // Sextil (U+2731 HEAVY ASTERISK) - Ersatz für U+26B9, weit verbreitet  
        "□",   // Quadratur (U+25A1) - in vielen Fonts
        "△",   // Trigon (U+25B3) - in vielen Fonts
        "⌐",   // Quincunx (U+2310 REVERSED NOT SIGN) - Ersatz für U+26BB, weit verbreitet
        "☍"    // Opposition (U+260D) - in vielen Fonts
    };
    
    m_unicodeRetrograde = "℞";  // Rx-Symbol (U+211E)
    m_unicodeGrad = "°";        // Grad-Symbol (U+00B0)
}

QFont AstroFontProvider::getSymbolFont(int pointSize) const {
    // AstroUniverse-Font 4pt größer darstellen für bessere Lesbarkeit
    int adjustedSize = m_hasAstroFont ? pointSize + 3 : pointSize;
    QFont font(m_fontName, adjustedSize);
    return font;
}

QFont AstroFontProvider::getPlanetSymbolFont(int pointSize) const {
    // Prüfe Fonts nach Glyph-Abdeckung für Asteroiden (U+26B3 bis U+26B8)
    // DejaVu Sans hat auch die Aspekt-Glyphen (U+26B9, U+26BA, U+26BB, U+260C, U+260D)
    const QVector<uint> asteroidGlyphs = {
        0x26B3, 0x26B4, 0x26B5, 0x26B6, 0x26B7, 0x26B8  // Asteroiden: Ceres, Pallas, Juno, Vesta, Chiron, Lilith
    };

    struct Candidate {
        QString name;
        bool available;
    };
    QList<Candidate> candidates;
    candidates.append({m_dejaVuFontName, m_hasDejaVuFont});
    candidates.append({m_symbolFontName, m_hasSymbolFont});
    candidates.append({"DejaVu Sans", true});
    candidates.append({"Apple Symbols", true});
    candidates.append({"Segoe UI Symbol", true});

    for (const Candidate& c : candidates) {
        if (!c.available || c.name.isEmpty()) {
            continue;
        }
        QFont font(c.name, pointSize);
        font.setStyleStrategy(QFont::PreferMatch);
        QFontMetrics fm(font);
        bool ok = true;
        for (uint cp : asteroidGlyphs) {
            if (!fm.inFontUcs4(cp)) {
                ok = false;
                break;
            }
        }
        if (ok) {
            return font;
        }
    }

    return QFont("Sans Serif", pointSize);
}

QString AstroFontProvider::planetSymbol(int planet) const {
    if (planet < 0) return "";

    // Immer Unicode-Symbole für Planeten (System-Font),
    // nie Glyphen aus dem AstroUniverse-Font
    if (planet < m_unicodePlanetSymbols.size()) {
        return m_unicodePlanetSymbols[planet];
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

QString AstroFontProvider::sternzeichenSymbolUnicode(int zeichen) const {
    if (zeichen < 0 || zeichen >= 12) return "";
    return m_unicodeSternzeichenSymbols[zeichen];
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
    
    // Immer Unicode-Symbole für Aspekte verwenden
    // Die Legacy-Symbole (0x91-0x97) sind Steuerzeichen und werden nicht korrekt angezeigt
    if (idx < m_unicodeAspektSymbols.size()) {
        return m_unicodeAspektSymbols[idx];
    }
    return "";
}

QFont AstroFontProvider::getAspektSymbolFont(int pointSize) const {
    // Aspekt-Symbole verwenden Unicode, daher gleicher Font wie Planeten (DejaVu Sans)
    return getPlanetSymbolFont(pointSize);
}

QString AstroFontProvider::retrogradeSymbol() const {
    // Immer Unicode für Retrograde (System-Font)
    return m_unicodeRetrograde;
}

QString AstroFontProvider::gradSymbol() const {
    // Immer Unicode für Grad (System-Font)
    return m_unicodeGrad;
}

} // namespace astro
