/**
 * @file print_manager.cpp
 * @brief Implementierung der Druck-Funktionen
 * 
 * 1:1 Port von sDlgPrint, sPrintAsp, sPrintText, sPrintKoord aus legacy/auprint.c
 */

#include "print_manager.h"
#include "chart_widget.h"
#include "../core/calculations.h"
#include "../core/constants.h"
#include "../core/transit_calc.h"
#include "../core/astro_font_provider.h"

#include <QPrintDialog>
#include <QPrintPreviewDialog>
#include <QPageSetupDialog>
#include <QFont>
#include <QFontMetrics>
#include <cmath>

namespace astro {

//==============================================================================
// Konstruktor / Destruktor
//==============================================================================

PrintManager::PrintManager(QObject* parent)
    : QObject(parent)
    , m_printer(nullptr)
    , m_pageWidth(0)
    , m_pageHeight(0)
    , m_factor(1.0)
    , m_centerX(0)
    , m_centerY(0)
    , m_radius(0) {
}

PrintManager::~PrintManager() {
    delete m_printer;
}

//==============================================================================
// Öffentliche Methoden
//==============================================================================

bool PrintManager::printRadix(const Radix& radix, const AuInit& auinit, QWidget* parent) {
    // Port von: sDlgPrint(Radix, PRN_NORMAL)
    
    if (!m_printer) {
        m_printer = new QPrinter(QPrinter::HighResolution);
    }
    
    // Drucker-Dialog anzeigen
    QPrintDialog dialog(m_printer, parent);
    dialog.setWindowTitle(tr("Horoskop drucken"));
    
    if (dialog.exec() != QDialog::Accepted) {
        return false;
    }
    
    // Seiten-Parameter berechnen (Port von sSetViewport)
    QRect pageRect = m_printer->pageRect(QPrinter::DevicePixel).toRect();
    m_pageWidth = pageRect.width();
    m_pageHeight = pageRect.height();
    
    // Faktor für Skalierung (Port von Sys.PrnFactor)
    m_factor = m_pageWidth / 800.0;  // Basis: 800 Pixel Referenzbreite
    
    // Mittelpunkt und Radius (Port von sMpX, sMpY, sRadius)
    m_centerX = m_pageWidth / 2;
    m_centerY = static_cast<int>(m_pageHeight / 2.5);
    int druber = m_centerX / 8;  // DRUBERFACTOR = 8
    m_radius = m_centerX - 5 * druber;
    
    QPainter painter;
    if (!painter.begin(m_printer)) {
        return false;
    }
    
    painter.setRenderHint(QPainter::Antialiasing);
    
    // Seite 1: Grafik + Text + Aspekte-Tabelle
    // Port von: sPrintText(), sRadix(), sHaus(), sPlanet(), sDrawAspekte(), sPrintAsp()
    
    // 1. Personen-Daten und Positionen drucken
    printText(painter, radix, m_factor);
    
    // 2. Radix-Grafik zeichnen
    drawRadixForPrint(painter, radix, auinit, m_centerX, m_centerY, m_radius, m_factor);
    
    // 3. Aspekte-Tabelle drucken
    printAspects(painter, radix, m_factor);
    
    // Neue Seite für Koordinaten (Port von: NEWFRAME + sPrintKoord)
    m_printer->newPage();
    
    // 4. Koordinaten-Seite drucken
    printCoordinates(painter, radix, m_factor);
    
    painter.end();
    
    return true;
}

bool PrintManager::printTransits(const Radix& basisRadix,
                                  const QVector<Radix>& transits,
                                  const QVector<TransitAspekt>& aspekte,
                                  const AuInit& auinit,
                                  QWidget* parent) {
    // Port von: sDlgPrint(Radix, PRN_MULTITRANS) -> sPrintMTransit()
    
    if (!m_printer) {
        m_printer = new QPrinter(QPrinter::HighResolution);
    }
    
    QPrintDialog dialog(m_printer, parent);
    dialog.setWindowTitle(tr("Transite drucken"));
    
    if (dialog.exec() != QDialog::Accepted) {
        return false;
    }
    
    QRect pageRect = m_printer->pageRect(QPrinter::DevicePixel).toRect();
    m_pageWidth = pageRect.width();
    m_pageHeight = pageRect.height();
    m_factor = m_pageWidth / 800.0;
    
    QPainter painter;
    if (!painter.begin(m_printer)) {
        return false;
    }
    
    painter.setRenderHint(QPainter::Antialiasing);
    
    // Font für Text
    QFont textFont("Arial", static_cast<int>(10 * m_factor));
    painter.setFont(textFont);
    QFontMetrics fm(textFont);
    int lineHeight = fm.height() + 4;
    
    int xText = static_cast<int>(20 * m_factor);
    int yText = static_cast<int>(40 * m_factor);
    int pageBottom = m_pageHeight - static_cast<int>(40 * m_factor);
    
    // Header: Basis-Person
    QString header = QString("Transite für %1 %2")
        .arg(basisRadix.rFix.vorname, basisRadix.rFix.name);
    painter.drawText(xText, yText, header);
    yText += lineHeight * 2;
    
    // Spalten-Header
    painter.drawText(xText, yText, tr("Transit-Planet"));
    painter.drawText(xText + static_cast<int>(150 * m_factor), yText, tr("Aspekt"));
    painter.drawText(xText + static_cast<int>(220 * m_factor), yText, tr("Radix-Objekt"));
    painter.drawText(xText + static_cast<int>(350 * m_factor), yText, tr("Von"));
    painter.drawText(xText + static_cast<int>(450 * m_factor), yText, tr("Bis"));
    yText += lineHeight;
    
    // Trennlinie
    painter.drawLine(xText, yText, m_pageWidth - xText, yText);
    yText += lineHeight / 2;
    
    // Transit-Aspekte auflisten
    for (const TransitAspekt& ta : aspekte) {
        if (ta.startIndex < 0 || ta.startIndex >= transits.size()) continue;
        
        // Seitenumbruch prüfen
        if (yText > pageBottom) {
            m_printer->newPage();
            yText = static_cast<int>(40 * m_factor);
        }
        
        const Radix& trStart = transits.at(ta.startIndex);
        int endIdx = qMin(ta.endIndex >= 0 ? ta.endIndex : ta.startIndex, transits.size() - 1);
        const Radix& trEnd = transits.at(endIdx);
        
        // Transit-Planet
        QString transitPlanet = astroFont().planetSymbol(ta.transitPlanet);
        if (trStart.planetTyp.size() > ta.transitPlanet && 
            (trStart.planetTyp[ta.transitPlanet] & P_TYP_RUCK)) {
            transitPlanet += " R";
        }
        painter.drawText(xText, yText, transitPlanet);
        
        // Aspekt-Symbol
        int aspIdx = 0;
        switch (ta.aspekt) {
            case KONJUNKTION: aspIdx = 0; break;
            case HALBSEX:     aspIdx = 1; break;
            case SEXTIL:      aspIdx = 2; break;
            case QUADRATUR:   aspIdx = 3; break;
            case TRIGON:      aspIdx = 4; break;
            case QUINCUNX:    aspIdx = 5; break;
            case OPOSITION:   aspIdx = 6; break;
        }
        painter.drawText(xText + static_cast<int>(150 * m_factor), yText, 
                         astroFont().aspektSymbol(aspIdx));
        
        // Radix-Objekt
        QString radixObj;
        if (ta.isHaus) {
            radixObj = QString("H%1").arg(ta.radixPlanet + 1);
        } else {
            radixObj = astroFont().planetSymbol(ta.radixPlanet);
        }
        painter.drawText(xText + static_cast<int>(220 * m_factor), yText, radixObj);
        
        // Datum von/bis
        QString vonDatum = QString("%1.%2.%3")
            .arg(trStart.rFix.tag, 2, 10, QChar('0'))
            .arg(trStart.rFix.monat, 2, 10, QChar('0'))
            .arg(trStart.rFix.jahr);
        painter.drawText(xText + static_cast<int>(350 * m_factor), yText, vonDatum);
        
        QString bisDatum = QString("%1.%2.%3")
            .arg(trEnd.rFix.tag, 2, 10, QChar('0'))
            .arg(trEnd.rFix.monat, 2, 10, QChar('0'))
            .arg(trEnd.rFix.jahr);
        painter.drawText(xText + static_cast<int>(450 * m_factor), yText, bisDatum);
        
        yText += lineHeight;
    }
    
    painter.end();
    
    return true;
}

bool PrintManager::showPrinterSetup(QWidget* parent) {
    // Port von: hdcPrinterIni(PRN_SETUP, ...)
    if (!m_printer) {
        m_printer = new QPrinter(QPrinter::HighResolution);
    }
    
    QPageSetupDialog dialog(m_printer, parent);
    return dialog.exec() == QDialog::Accepted;
}

//==============================================================================
// Private Methoden
//==============================================================================

void PrintManager::printText(QPainter& painter, const Radix& radix, double factor) {
    // Port von: sPrintText()
    
    QFont textFont("Arial", static_cast<int>(10 * factor));
    // Apple Symbols primär für macOS (enthält alle Planeten- und Asteroiden-Symbole U+2600-U+26FF)
    QFont planetFont;
    {
        int size = static_cast<int>(12 * factor);
        QStringList symbolFonts = {"Apple Symbols", "Segoe UI Symbol", "Noto Sans Symbols2", "DejaVu Sans"};
        for (const QString& fontName : symbolFonts) {
            planetFont = QFont(fontName, size);
            if (QFontInfo(planetFont).family().contains(fontName.left(5), Qt::CaseInsensitive)) {
                break;
            }
        }
        planetFont.setPointSize(size);
    }
    // Astro-Font für Sternzeichen (Fallback: gleicher Systemfont)
    QFont zodiacFont = planetFont;
    if (astroFont().hasAstroFont()) {
        zodiacFont = astroFont().getSymbolFont(static_cast<int>(12 * factor));
    }
    painter.setFont(textFont);
    QFontMetrics fm(textFont);
    
    int lineHeight = static_cast<int>(fm.height() * 1.3);
    int xText = static_cast<int>(20 * factor);
    int yText = static_cast<int>(30 * factor);
    int xAdd = static_cast<int>(30 * factor);
    
    // Horoskop-Typ
    static const char* horoTypen[] = {"Radix", "Transit", "Synastrie", "Composit", "Combine", "Direkt"};
    QString horoTyp = (radix.horoTyp >= 0 && radix.horoTyp < 6) ? horoTypen[radix.horoTyp] : "Radix";
    painter.drawText(xText, yText, horoTyp);
    yText += lineHeight;
    
    // Häusersystem
    static const char* hausTypen[] = {
        "Koch", "Placidus", "Equal", "Equal (MC)", "Whole Sign",
        "Topozentrisch", "Campanus", "Meridian", "Regiomontanus",
        "Porphyry", "Porphyry New", "Morinus", "Alcabitius", "Null"
    };
    QString hausTyp = (radix.hausSys >= 0 && radix.hausSys < 14) ? hausTypen[radix.hausSys] : "Placidus";
    painter.drawText(xText, yText, QString("Häuser: %1").arg(hausTyp));
    yText += lineHeight;
    
    // Name
    QString name = QString("%1 %2").arg(radix.rFix.vorname, radix.rFix.name);
    painter.drawText(xText, yText, name);
    yText += lineHeight;
    
    // Geburtsdatum und -zeit
    int16_t stunden, minuten;
    Calculations::decimalToTime(radix.rFix.zeit, stunden, minuten);
    QString datum = QString("%1.%2.%3  %4:%5")
        .arg(radix.rFix.tag, 2, 10, QChar('0'))
        .arg(radix.rFix.monat, 2, 10, QChar('0'))
        .arg(radix.rFix.jahr)
        .arg(stunden, 2, 10, QChar('0'))
        .arg(minuten, 2, 10, QChar('0'));
    painter.drawText(xText, yText, datum);
    yText += lineHeight;
    
    // Geburtsort
    painter.drawText(xText, yText, QString("%1, %2").arg(radix.rFix.ort, radix.rFix.land));
    yText += lineHeight * 2;
    
    // Planeten-Positionen
    painter.drawText(xText, yText, tr("Planetenpositionen"));
    yText += lineHeight / 2;
    
    painter.setFont(planetFont);
    for (int i = 0; i < radix.anzahlPlanet && i < radix.planet.size(); ++i) {
        yText += lineHeight;
        
        // Rückläufigkeit
        if (radix.planetTyp.size() > i && (radix.planetTyp[i] & P_TYP_RUCK)) {
            painter.drawText(xText - xAdd / 2, yText, "R");
        }
        
        // Planet-Symbol
        QString symbol = astroFont().planetSymbol(i);
        painter.drawText(xText, yText, symbol);
        
        // Position
        QString pos = formatPosition(radix.planet[i]);
        painter.drawText(xText + xAdd, yText, pos);
        
        // Sternzeichen (immer zuerst AstroUniverse-Font, Fallback Systemfont)
        int stz = static_cast<int>(radix.planet[i] / 30.0) % 12;
        QString stzSymbol = astroFont().sternzeichenSymbol(stz);
        painter.setFont(zodiacFont);
        painter.drawText(xText + xAdd * 4, yText, stzSymbol);
        painter.setFont(planetFont);
        
        // In Haus
        if (radix.inHaus.size() > i) {
            painter.drawText(xText + xAdd * 5, yText, QString("in H%1").arg(radix.inHaus[i]));
        }
    }
    
    // Häuser-Positionen (rechts daneben)
    int xHaus = xText + static_cast<int>(200 * factor);
    yText = static_cast<int>(30 * factor) + lineHeight * 5;
    
    painter.setFont(textFont);
    painter.drawText(xHaus, yText, tr("Häuserpositionen"));
    yText += lineHeight / 2;
    
    for (int i = 0; i < MAX_HAUS && i < radix.haus.size(); ++i) {
        yText += lineHeight;
        
        // Haus-Nummer und Position im normalen Text-Font
        painter.setFont(textFont);
        painter.drawText(xHaus, yText, QString("H%1").arg(i + 1, 2, 10, QChar('0')));
        QString pos = formatPosition(radix.haus[i]);
        painter.drawText(xHaus + xAdd, yText, pos);
        
        // Sternzeichen (immer zuerst AstroUniverse-Font, Fallback Systemfont)
        int stz = static_cast<int>(radix.haus[i] / 30.0) % 12;
        QString stzSymbol = astroFont().sternzeichenSymbol(stz);
        painter.setFont(zodiacFont);
        painter.drawText(xHaus + xAdd * 4, yText, stzSymbol);
    }
}

void PrintManager::printAspects(QPainter& painter, const Radix& radix, double factor) {
    // Port von: sPrintAsp()
    
    // Apple Symbols primär für macOS
    QFont symbolFont;
    {
        int size = static_cast<int>(10 * factor);
        QStringList symbolFonts = {"Apple Symbols", "Segoe UI Symbol", "Noto Sans Symbols2", "DejaVu Sans"};
        for (const QString& fontName : symbolFonts) {
            symbolFont = QFont(fontName, size);
            if (QFontInfo(symbolFont).family().contains(fontName.left(5), Qt::CaseInsensitive)) {
                break;
            }
        }
        symbolFont.setPointSize(size);
    }
    painter.setFont(symbolFont);
    QFontMetrics fm(symbolFont);
    
    int xAdd = static_cast<int>(fm.horizontalAdvance("☉") * 1.6);
    int yAdd = static_cast<int>(fm.height() * 1.5);
    int xBAdd = xAdd / 2;
    int yBAdd = yAdd / 4;
    
    // Position unterhalb der Grafik
    int yAspStart = m_centerY + m_radius + static_cast<int>(60 * factor);
    int xText = static_cast<int>(20 * factor);
    int yText = yAspStart;
    
    int numPlanets = qMin(static_cast<int>(radix.anzahlPlanet), P_NKNOTEN);
    
    // Aspekte-Matrix zeichnen
    int xBox = xText - xBAdd;
    int yBox = yText - yBAdd;
    int xBoxE = xBox + xAdd * (numPlanets + 1);
    int yBoxE = yBox + yAdd * (numPlanets + 1);
    
    // Horizontale Linien und Planeten-Symbole (Zeilen)
    painter.drawLine(xBox, yBox, xBoxE, yBox);
    for (int i = 0; i < numPlanets; ++i) {
        // Planet-Symbol links
        QString symbol = astroFont().planetSymbol(i);
        painter.drawText(xText, yText, symbol);
        
        yBox += yAdd;
        painter.drawLine(xBox, yBox, xBoxE, yBox);
        
        // Aspekte in dieser Zeile
        int sAddX = xText + xAdd;
        for (int j = i + 1; j < numPlanets; ++j) {
            int idx = i * radix.anzahlPlanet + j;
            if (idx < radix.aspPlanet.size()) {
                int asp = static_cast<int>(radix.aspPlanet[idx]);
                // int8_t overflow: 150 -> -106, 180 -> -76
                if (asp < 0 && asp != KEIN_ASP) asp += 256;
                if (asp != KEIN_ASP) {
                    int aspIdx = 0;
                    switch (asp) {
                        case KONJUNKTION: aspIdx = 0; break;
                        case HALBSEX:     aspIdx = 1; break;
                        case SEXTIL:      aspIdx = 2; break;
                        case QUADRATUR:   aspIdx = 3; break;
                        case TRIGON:      aspIdx = 4; break;
                        case QUINCUNX:    aspIdx = 5; break;
                        case OPOSITION:   aspIdx = 6; break;
                    }
                    painter.drawText(sAddX + (j - i - 1) * xAdd, yText, 
                                     astroFont().aspektSymbol(aspIdx));
                }
            }
        }
        
        yText += yAdd;
    }
    yBox += yAdd;
    painter.drawLine(xBox, yBox, xBoxE, yBox);
    
    // Vertikale Linien und Planeten-Symbole (Spalten)
    int sAddX = xText + xAdd;
    yBox = yAspStart - yBAdd;
    painter.drawLine(xBox, yBox, xBox, yBoxE);
    for (int i = 1; i < numPlanets; ++i) {
        // Planet-Symbol unten
        QString symbol = astroFont().planetSymbol(i);
        painter.drawText(sAddX, yText, symbol);
        
        sAddX += xAdd;
        xBox += xAdd;
        painter.drawLine(xBox, yBox, xBox, yBoxE);
    }
    xBox += xAdd;
    painter.drawLine(xBox, yBox, xBox, yBoxE);
}

void PrintManager::printHouseAspects(QPainter& painter, const Radix& radix, int yStart, double factor) {
    // Port von: sPrintHausAsp()
    // Häuser-Aspekte werden ähnlich wie Planeten-Aspekte gedruckt
    
    if (radix.aspHaus.isEmpty()) return;
    
    // Apple Symbols primär für macOS
    QFont symbolFont("Apple Symbols", static_cast<int>(10 * factor));
    painter.setFont(symbolFont);
    QFontMetrics fm(symbolFont);
    
    int lineHeight = static_cast<int>(fm.height() * 1.3);
    int xText = static_cast<int>(20 * factor);
    int yText = yStart;
    
    painter.drawText(xText, yText, tr("Häuser-Aspekte"));
    yText += lineHeight;
    
    int numPlanets = radix.anzahlPlanet;
    for (int i = 0; i < numPlanets; ++i) {
        for (int h = 0; h < MAX_HAUS; ++h) {
            int idx = i * MAX_HAUS + h;
            if (idx >= radix.aspHaus.size()) continue;
            
            int asp = static_cast<int>(radix.aspHaus[idx]);
            // int8_t overflow: 150 -> -106, 180 -> -76
            if (asp < 0 && asp != KEIN_ASP) asp += 256;
            if (asp == KEIN_ASP) continue;
            
            QString planetSymbol = astroFont().planetSymbol(i);
            int aspIdx = 0;
            switch (asp) {
                case KONJUNKTION: aspIdx = 0; break;
                case HALBSEX:     aspIdx = 1; break;
                case SEXTIL:      aspIdx = 2; break;
                case QUADRATUR:   aspIdx = 3; break;
                case TRIGON:      aspIdx = 4; break;
                case QUINCUNX:    aspIdx = 5; break;
                case OPOSITION:   aspIdx = 6; break;
            }
            QString aspSymbol = astroFont().aspektSymbol(aspIdx);
            
            QString line = QString("%1 %2 H%3")
                .arg(planetSymbol)
                .arg(aspSymbol)
                .arg(h + 1, 2, 10, QChar('0'));
            painter.drawText(xText, yText, line);
            yText += lineHeight;
        }
    }
}

void PrintManager::printCoordinates(QPainter& painter, const Radix& radix, double factor) {
    // Port von: sPrintKoord()
    
    QFont textFont("Arial", static_cast<int>(10 * factor));
    painter.setFont(textFont);
    QFontMetrics fm(textFont);
    
    int lineHeight = static_cast<int>(fm.height() * 1.3);
    int xText = static_cast<int>(40 * factor);
    int yText = static_cast<int>(40 * factor);
    
    // Header
    painter.drawText(xText, yText, tr("Koordinaten"));
    yText += lineHeight * 2;
    
    // Personen-Daten
    painter.drawText(xText, yText, QString("Name: %1 %2").arg(radix.rFix.vorname, radix.rFix.name));
    yText += lineHeight;
    
    painter.drawText(xText, yText, QString("Geburtsdatum: %1.%2.%3")
        .arg(radix.rFix.tag, 2, 10, QChar('0'))
        .arg(radix.rFix.monat, 2, 10, QChar('0'))
        .arg(radix.rFix.jahr));
    yText += lineHeight;
    
    int16_t stunden, minuten;
    Calculations::decimalToTime(radix.rFix.zeit, stunden, minuten);
    painter.drawText(xText, yText, QString("Geburtszeit: %1:%2")
        .arg(stunden, 2, 10, QChar('0'))
        .arg(minuten, 2, 10, QChar('0')));
    yText += lineHeight;
    
    painter.drawText(xText, yText, QString("Geburtsort: %1, %2").arg(radix.rFix.ort, radix.rFix.land));
    yText += lineHeight;
    
    painter.drawText(xText, yText, QString("Länge: %1°").arg(radix.rFix.laenge, 0, 'f', 4));
    yText += lineHeight;
    
    painter.drawText(xText, yText, QString("Breite: %1°").arg(radix.rFix.breite, 0, 'f', 4));
    yText += lineHeight;
    
    painter.drawText(xText, yText, QString("Zeitzone: %1 h").arg(radix.rFix.zone, 0, 'f', 1));
    yText += lineHeight * 2;
    
    // Planeten-Koordinaten
    painter.drawText(xText, yText, tr("Planeten-Koordinaten (ekliptisch)"));
    yText += lineHeight;
    
    // Spalten-Header
    int col1 = xText;
    int col2 = xText + static_cast<int>(80 * factor);
    int col3 = xText + static_cast<int>(200 * factor);
    int col4 = xText + static_cast<int>(320 * factor);
    
    painter.drawText(col1, yText, tr("Planet"));
    painter.drawText(col2, yText, tr("Länge"));
    painter.drawText(col3, yText, tr("Breite"));
    painter.drawText(col4, yText, tr("Geschw."));
    yText += lineHeight;
    
    // Trennlinie
    painter.drawLine(col1, yText - lineHeight / 4, col4 + static_cast<int>(80 * factor), yText - lineHeight / 4);
    
    for (int i = 0; i < radix.anzahlPlanet && i < radix.planet.size(); ++i) {
        QString planetName;
        switch (i) {
            case 0: planetName = "Sonne"; break;
            case 1: planetName = "Mond"; break;
            case 2: planetName = "Merkur"; break;
            case 3: planetName = "Venus"; break;
            case 4: planetName = "Mars"; break;
            case 5: planetName = "Jupiter"; break;
            case 6: planetName = "Saturn"; break;
            case 7: planetName = "Uranus"; break;
            case 8: planetName = "Neptun"; break;
            case 9: planetName = "Pluto"; break;
            case 10: planetName = "N.Mndknt."; break;
            case 11: planetName = "Chiron"; break;
            default: planetName = QString("Planet %1").arg(i); break;
        }
        
        painter.drawText(col1, yText, planetName);
        painter.drawText(col2, yText, formatDegree(radix.planet[i]));
        
        // Rückläufigkeit anzeigen
        if (radix.planetTyp.size() > i && (radix.planetTyp[i] & P_TYP_RUCK)) {
            painter.drawText(col4, yText, "R");
        }
        
        yText += lineHeight;
    }
    
    yText += lineHeight;
    
    // Häuser-Koordinaten
    painter.drawText(xText, yText, tr("Häuser-Spitzen"));
    yText += lineHeight;
    
    for (int i = 0; i < MAX_HAUS && i < radix.haus.size(); ++i) {
        QString hausLabel;
        switch (i) {
            case 0: hausLabel = "ASC (H1)"; break;
            case 3: hausLabel = "IC (H4)"; break;
            case 6: hausLabel = "DSC (H7)"; break;
            case 9: hausLabel = "MC (H10)"; break;
            default: hausLabel = QString("H%1").arg(i + 1); break;
        }
        
        painter.drawText(col1, yText, hausLabel);
        painter.drawText(col2, yText, formatDegree(radix.haus[i]));
        yText += lineHeight;
    }
}

void PrintManager::drawRadixForPrint(QPainter& painter, const Radix& radix, const AuInit& auinit,
                                      int centerX, int centerY, int radius, double factor) {
    // Port von: sRadix(), sHaus(), sPlanet(), sDrawAspekte() für Druck
    // Verwendet die gleiche Zeichenlogik wie ChartWidget, aber mit Druck-Farben (schwarz)
    
    painter.save();
    
    // Schwarz für Druck
    QPen blackPen(Qt::black, static_cast<int>(factor));
    painter.setPen(blackPen);
    painter.setBrush(Qt::NoBrush);
    
    // Radien berechnen (wie in ChartWidget::calculateRadii)
    bool show3 = (auinit.sTeilung & S_3er) != 0;
    bool show9 = (auinit.sTeilung & S_9er) != 0;
    
    double radiusStz, radius10, radius3, radius5, radiusPlanet, radiusAsp;
    
    if (show3 && show9) {
        radiusStz = radius * KREIS_STZ;
        radius10 = radius * KREIS_10_GRAD;
        radius3 = radius * KREIS_3_GRAD;
        radius5 = radius * KREIS_5_GRAD;
        radiusPlanet = radius * KREIS_PLANET;
    } else if (show3 || show9) {
        radiusStz = radius * KREIS3_STZ;
        radius10 = radius * KREIS3_10_GRAD;
        radius3 = radius * KREIS3_3_GRAD;
        radius5 = radius * KREIS3_5_GRAD;
        radiusPlanet = radius * KREIS3_PLANET;
    } else {
        radiusStz = radius * KREIS9_STZ;
        radius10 = radius * KREIS9_10_GRAD;
        radius3 = radius * KREIS9_3_GRAD;
        radius5 = radius * KREIS9_5_GRAD;
        radiusPlanet = radius * KREIS9_PLANET;
    }
    radiusAsp = radius * KREIS_ASP;
    
    // Kreise zeichnen
    painter.drawEllipse(QPointF(centerX, centerY), radius, radius);
    painter.drawEllipse(QPointF(centerX, centerY), radiusStz, radiusStz);
    if (show3) {
        painter.drawEllipse(QPointF(centerX, centerY), radius10, radius10);
    }
    if (show9) {
        painter.drawEllipse(QPointF(centerX, centerY), radius3, radius3);
    }
    painter.drawEllipse(QPointF(centerX, centerY), radius5, radius5);
    painter.drawEllipse(QPointF(centerX, centerY), radiusAsp, radiusAsp);
    
    // Rotation: ASC links
    double rotation = -radix.asc;
    
    auto degreeToPoint = [&](double degree, double r) -> QPointF {
        double rotated = degree + rotation;
        double rad = (180.0 + rotated) * PI / 180.0;
        return QPointF(centerX + r * std::cos(rad), centerY - r * std::sin(rad));
    };
    
    // 30-Grad-Linien (Sternzeichen-Grenzen)
    for (int i = 0; i < 12; ++i) {
        double degree = i * 30.0;
        QPointF outer = degreeToPoint(degree, radius);
        QPointF inner = degreeToPoint(degree, radius10);
        painter.drawLine(outer, inner);
    }
    
    // Sternzeichen-Symbole: immer zuerst AstroUniverse-Font, Fallback System-/Symbolfont
    QFont zodiacFont;
    if (astroFont().hasAstroFont()) {
        zodiacFont = astroFont().getSymbolFont(static_cast<int>(14 * factor));
    } else {
        // Apple Symbols primär für macOS
        int size = static_cast<int>(14 * factor);
        QStringList symbolFonts = {"Apple Symbols", "Segoe UI Symbol", "Noto Sans Symbols2", "DejaVu Sans"};
        for (const QString& fontName : symbolFonts) {
            zodiacFont = QFont(fontName, size);
            if (QFontInfo(zodiacFont).family().contains(fontName.left(5), Qt::CaseInsensitive)) {
                break;
            }
        }
        zodiacFont.setPointSize(size);
    }
    painter.setFont(zodiacFont);
    for (int i = 0; i < 12; ++i) {
        double degree = i * 30.0 + 15.0;
        double symbolRadius = (radius + radiusStz) / 2.0;
        QPointF pos = degreeToPoint(degree, symbolRadius);
        QString symbol = astroFont().sternzeichenSymbol(i);
        QRectF rect(pos.x() - 10, pos.y() - 10, 20, 20);
        painter.drawText(rect, Qt::AlignCenter, symbol);
    }
    
    // Häuser zeichnen
    for (int i = 0; i < MAX_HAUS && i < radix.haus.size(); ++i) {
        double degree = radix.haus[i];
        bool isMainAxis = (i == 0 || i == 3 || i == 6 || i == 9);
        
        if (isMainAxis) {
            QPen thickPen(Qt::black, static_cast<int>(2 * factor));
            painter.setPen(thickPen);
            QPointF outer = degreeToPoint(degree, radius);
            QPointF inner = degreeToPoint(degree, radiusAsp);
            painter.drawLine(outer, inner);
            painter.setPen(blackPen);
        } else {
            QPointF outer = degreeToPoint(degree, radiusStz);
            QPointF inner = degreeToPoint(degree, radiusAsp);
            painter.drawLine(outer, inner);
        }
    }
    
    // Planeten zeichnen (immer mit System-/Symbolfont, nie AstroUniverse-Font)
    // Apple Symbols primär für macOS (enthält alle Planeten- und Asteroiden-Symbole U+2600-U+26FF)
    QFont planetFont;
    {
        int size = static_cast<int>(12 * factor);
        QStringList symbolFonts = {"Apple Symbols", "Segoe UI Symbol", "Noto Sans Symbols2", "DejaVu Sans"};
        for (const QString& fontName : symbolFonts) {
            planetFont = QFont(fontName, size);
            if (QFontInfo(planetFont).family().contains(fontName.left(5), Qt::CaseInsensitive)) {
                break;
            }
        }
        planetFont.setPointSize(size);
    }
    painter.setFont(planetFont);
    
    // Kollisions-Versatz berechnen
    QVector<int> offsetLevel(radix.anzahlPlanet, 0);
    for (int i = 0; i < radix.anzahlPlanet; ++i) {
        for (int j = 0; j < i; ++j) {
            double diff = std::fabs(radix.planet[i] - radix.planet[j]);
            if (diff > 180.0) diff = 360.0 - diff;
            if (diff < 6.0) {
                offsetLevel[i] = qMax(offsetLevel[i], offsetLevel[j] + 1);
            }
        }
    }
    
    for (int i = 0; i < radix.anzahlPlanet && i < radix.planet.size(); ++i) {
        double degree = radix.planet[i];
        double offset = offsetLevel[i] * 20.0 * factor;
        double symbolRadius = radiusPlanet - 20 * factor - offset;
        
        // Tic-Linie
        QPointF outer = degreeToPoint(degree, radiusStz);
        QPointF inner = degreeToPoint(degree, symbolRadius + 15 * factor);
        painter.drawLine(outer, inner);
        
        // Symbol
        QPointF pos = degreeToPoint(degree, symbolRadius);
        QString symbol = astroFont().planetSymbol(i);
        QRectF rect(pos.x() - 10, pos.y() - 10, 20, 20);
        painter.drawText(rect, Qt::AlignCenter, symbol);
    }
    
    // Aspekte zeichnen
    int numPlanets = radix.anzahlPlanet;
    for (int i = 0; i < numPlanets; ++i) {
        for (int j = i + 1; j < numPlanets; ++j) {
            int idx = i * numPlanets + j;
            if (idx >= radix.aspPlanet.size()) continue;
            
            int asp = static_cast<int>(radix.aspPlanet[idx]);
            // int8_t overflow: 150 -> -106, 180 -> -76
            if (asp < 0 && asp != KEIN_ASP) asp += 256;
            if (asp == KEIN_ASP) continue;
            
            // Linienstil je nach Aspekt
            Qt::PenStyle style = Qt::SolidLine;
            switch (asp) {
                case SEXTIL:    style = Qt::DashLine; break;
                case TRIGON:    style = Qt::DashDotLine; break;
                case HALBSEX:   style = Qt::DotLine; break;
                case QUINCUNX:  style = Qt::DashDotDotLine; break;
            }
            painter.setPen(QPen(Qt::black, static_cast<int>(factor), style));
            
            QPointF p1 = degreeToPoint(radix.planet[i], radiusAsp);
            QPointF p2 = degreeToPoint(radix.planet[j], radiusAsp);
            painter.drawLine(p1, p2);
        }
    }
    
    painter.restore();
}

//==============================================================================
// Hilfsfunktionen
//==============================================================================

QString PrintManager::formatDegree(double degree) const {
    // Format: 123°45'67"
    int grad = static_cast<int>(degree);
    double rest = (degree - grad) * 60.0;
    int minuten = static_cast<int>(rest);
    int sekunden = static_cast<int>((rest - minuten) * 60.0);
    
    return QString("%1°%2'%3\"")
        .arg(grad, 3, 10, QChar('0'))
        .arg(minuten, 2, 10, QChar('0'))
        .arg(sekunden, 2, 10, QChar('0'));
}

QString PrintManager::formatPosition(double degree) const {
    // Format: 12°34'56" im Zeichen
    double inSign = std::fmod(degree, 30.0);
    int grad = static_cast<int>(inSign);
    double rest = (inSign - grad) * 60.0;
    int minuten = static_cast<int>(rest);
    int sekunden = static_cast<int>((rest - minuten) * 60.0);
    
    return QString("%1°%2'%3\"")
        .arg(grad, 2, 10, QChar('0'))
        .arg(minuten, 2, 10, QChar('0'))
        .arg(sekunden, 2, 10, QChar('0'));
}

} // namespace astro
