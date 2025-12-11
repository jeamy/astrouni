/**
 * @file pdf_exporter.cpp
 * @brief Implementierung des PDF-Exports für Horoskope
 */

#include "pdf_exporter.h"
#include "chart_widget.h"
#include "../core/constants.h"
#include "../core/astro_font_provider.h"

#include <QFileDialog>
#include <QPrinter>
#include <QPrintDialog>
#include <QPainter>
#include <QDate>
#include <QPixmap>
#include <cmath>

namespace astro {

PdfExporter::PdfExporter(QObject* parent)
    : QObject(parent) {
}

bool PdfExporter::exportRadix(const Radix& radix, const AuInit& auinit,
                               ChartWidget* chartWidget, QWidget* parent) {
    // Datei-Dialog für PDF-Speicherort
    QString fileName = QFileDialog::getSaveFileName(
        parent,
        tr("Horoskop als PDF exportieren"),
        QString("%1_%2.pdf").arg(radix.rFix.vorname, radix.rFix.name),
        tr("PDF-Dateien (*.pdf)")
    );
    
    if (fileName.isEmpty()) {
        return false;
    }
    
    // PDF erstellen
    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(fileName);
    printer.setPageSize(QPageSize::A4);
    printer.setPageMargins(QMarginsF(15, 15, 15, 15), QPageLayout::Millimeter);
    
    QPainter painter;
    if (!painter.begin(&printer)) {
        return false;
    }
    
    renderPage(painter, printer, radix, auinit, chartWidget);
    
    painter.end();
    return true;
}

bool PdfExporter::printRadix(const Radix& radix, const AuInit& auinit,
                              ChartWidget* chartWidget, QWidget* parent) {
    QPrinter printer(QPrinter::HighResolution);
    printer.setPageSize(QPageSize::A4);
    printer.setPageMargins(QMarginsF(15, 15, 15, 15), QPageLayout::Millimeter);
    
    QPrintDialog dialog(&printer, parent);
    dialog.setWindowTitle(tr("Horoskop drucken"));
    
    if (dialog.exec() != QDialog::Accepted) {
        return false;
    }
    
    QPainter painter;
    if (!painter.begin(&printer)) {
        return false;
    }
    
    renderPage(painter, printer, radix, auinit, chartWidget);
    
    painter.end();
    return true;
}

bool PdfExporter::exportSynastrie(const Radix& radix1, const Radix& radix2,
                                   const AuInit& auinit, ChartWidget* chartWidget, QWidget* parent) {
    QString fileName = QFileDialog::getSaveFileName(
        parent,
        tr("Synastrie als PDF exportieren"),
        QString("%1_%2_Synastrie.pdf").arg(radix1.rFix.vorname, radix2.rFix.vorname),
        tr("PDF-Dateien (*.pdf)")
    );
    
    if (fileName.isEmpty()) {
        return false;
    }
    
    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(fileName);
    printer.setPageSize(QPageSize::A4);
    printer.setPageMargins(QMarginsF(10, 10, 10, 10), QPageLayout::Millimeter);
    
    QPainter painter;
    if (!painter.begin(&printer)) {
        return false;
    }
    
    renderSynastriePage(painter, printer, radix1, radix2, auinit, chartWidget);
    
    painter.end();
    return true;
}

bool PdfExporter::printSynastrie(const Radix& radix1, const Radix& radix2,
                                  const AuInit& auinit, ChartWidget* chartWidget, QWidget* parent) {
    QPrinter printer(QPrinter::HighResolution);
    printer.setPageSize(QPageSize::A4);
    printer.setPageMargins(QMarginsF(10, 10, 10, 10), QPageLayout::Millimeter);
    
    QPrintDialog dialog(&printer, parent);
    dialog.setWindowTitle(tr("Synastrie drucken"));
    
    if (dialog.exec() != QDialog::Accepted) {
        return false;
    }
    
    QPainter painter;
    if (!painter.begin(&printer)) {
        return false;
    }
    
    renderSynastriePage(painter, printer, radix1, radix2, auinit, chartWidget);
    
    painter.end();
    return true;
}

bool PdfExporter::exportTransit(const Radix& radix, const Radix& transit,
                                 const AuInit& auinit, ChartWidget* chartWidget, QWidget* parent) {
    QString fileName = QFileDialog::getSaveFileName(
        parent,
        tr("Transit als PDF exportieren"),
        QString("%1_%2_Transit.pdf").arg(radix.rFix.vorname, radix.rFix.name),
        tr("PDF-Dateien (*.pdf)")
    );
    
    if (fileName.isEmpty()) {
        return false;
    }
    
    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(fileName);
    printer.setPageSize(QPageSize::A4);
    printer.setPageMargins(QMarginsF(10, 10, 10, 10), QPageLayout::Millimeter);
    
    QPainter painter;
    if (!painter.begin(&printer)) {
        return false;
    }
    
    renderTransitPage(painter, printer, radix, transit, auinit, chartWidget);
    
    painter.end();
    return true;
}

bool PdfExporter::printTransit(const Radix& radix, const Radix& transit,
                                const AuInit& auinit, ChartWidget* chartWidget, QWidget* parent) {
    QPrinter printer(QPrinter::HighResolution);
    printer.setPageSize(QPageSize::A4);
    printer.setPageMargins(QMarginsF(10, 10, 10, 10), QPageLayout::Millimeter);
    
    QPrintDialog dialog(&printer, parent);
    dialog.setWindowTitle(tr("Transit drucken"));
    
    if (dialog.exec() != QDialog::Accepted) {
        return false;
    }
    
    QPainter painter;
    if (!painter.begin(&printer)) {
        return false;
    }
    
    renderTransitPage(painter, printer, radix, transit, auinit, chartWidget);
    
    painter.end();
    return true;
}

void PdfExporter::renderPage(QPainter& painter, QPrinter& printer,
                              const Radix& radix, const AuInit& auinit,
                              ChartWidget* chartWidget) {
    QRect pageRect = printer.pageRect(QPrinter::DevicePixel).toRect();
    int pageWidth = pageRect.width();
    int pageHeight = pageRect.height();
    
    double dpi = printer.resolution();
    double mmToPixel = dpi / 25.4;
    
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::TextAntialiasing);
    
    // Fonts
    QFont titleFont("Arial", 14);
    titleFont.setBold(true);
    QFont headerFont("Arial", 10);
    headerFont.setBold(true);
    QFont normalFont("Arial", 9);
    QFont smallFont("Arial", 7);
    // Planeten/Asteroiden: getPlanetSymbolFont (DejaVu Sans für Glyphen)
    QFont symbolFont = astroFont().getPlanetSymbolFont(10);
    // Sternzeichen: getPlanetSymbolFont für Unicode-Symbole (DejaVu Sans hat ♈♉♊ etc.)
    QFont zodiacFont = astroFont().getPlanetSymbolFont(10);
    
    int margin = static_cast<int>(10 * mmToPixel);
    int y = margin;
    
    // Helper: Farbe anpassen (weiß -> schwarz)
    auto adjustColor = [](QColor c) -> QColor {
        if (c.lightness() > 200) return Qt::black;
        return c;
    };
    
    // === HEADER ===
    painter.setFont(titleFont);
    painter.setPen(Qt::black);
    QString fullName = QString("%1 %2").arg(radix.rFix.vorname, radix.rFix.name);
    painter.drawText(margin, y + painter.fontMetrics().ascent(), fullName);
    y += painter.fontMetrics().height() + static_cast<int>(2 * mmToPixel);
    
    // Geburtsdaten
    painter.setFont(normalFont);
    int stunde = static_cast<int>(radix.rFix.zeit);
    int minute = static_cast<int>((radix.rFix.zeit - stunde) * 60);
    QString gebInfo = QString("%1.%2.%3 um %4:%5 Uhr, %6")
        .arg(radix.rFix.tag, 2, 10, QChar('0'))
        .arg(radix.rFix.monat, 2, 10, QChar('0'))
        .arg(radix.rFix.jahr)
        .arg(stunde, 2, 10, QChar('0'))
        .arg(minute, 2, 10, QChar('0'))
        .arg(radix.rFix.ort);
    painter.drawText(margin, y + painter.fontMetrics().ascent(), gebInfo);
    y += painter.fontMetrics().height() + static_cast<int>(3 * mmToPixel);
    
    // === RADIX-GRAFIK (quadratisch!) ===
    if (chartWidget) {
        int chartSizeMm = 120;
        int chartSize = static_cast<int>(chartSizeMm * mmToPixel);
        int chartX = (pageWidth - chartSize) / 2;
        
        // Bild ist bereits quadratisch (1200x1200) aus renderForPrint
        QImage chartImage = chartWidget->renderForPrint(1200);
        
        // Zeichne in ein quadratisches Ziel-Rechteck (erzwingt korrektes Seitenverhältnis)
        QRect targetRect(chartX, y, chartSize, chartSize);
        painter.drawImage(targetRect, chartImage);
        
        y += chartSize + static_cast<int>(3 * mmToPixel);
    }
    
    // === DREI-SPALTEN-LAYOUT: Positionen | Häuser | Legende ===
    int colWidth = (pageWidth - 4 * margin) / 3;
    int col1X = margin;
    int col2X = margin + colWidth + margin;
    int col3X = margin + 2 * (colWidth + margin);
    int tableY = y;
    
    // === POSITIONEN (linke Spalte) ===
    painter.setFont(headerFont);
    painter.setPen(Qt::black);
    painter.drawText(col1X, tableY + painter.fontMetrics().ascent(), "Positionen");
    
    // === HÄUSER (mittlere Spalte) ===
    painter.drawText(col2X, tableY + painter.fontMetrics().ascent(), "Häuser");
    
    // === LEGENDE (rechte Spalte) ===
    painter.drawText(col3X, tableY + painter.fontMetrics().ascent(), "Legende");
    
    tableY += painter.fontMetrics().height() + static_cast<int>(1 * mmToPixel);
    
    int lineHeight = static_cast<int>(3.5 * mmToPixel);
    int numPlanets = qMin(static_cast<int>(radix.anzahlPlanet), 17);
    int posY = tableY;
    
    // Planeten-Namen für Legende
    static const char* planetNames[] = {
        "Sonne", "Mond", "Merkur", "Venus", "Mars",
        "Jupiter", "Saturn", "Uranus", "Neptun", "Pluto",
        "Mondkn.", "Lilith", "Chiron", "Ceres", "Pallas", "Juno", "Vesta"
    };
    
    for (int i = 0; i < numPlanets && i < radix.planet.size(); ++i) {
        // Planet-Symbol
        painter.setFont(symbolFont);
        QString symbol = astroFont().planetSymbol(i);
        painter.setPen(adjustColor(getPlanetColor(i, auinit)));
        painter.drawText(col1X, posY + painter.fontMetrics().ascent(), symbol);
        
        // Rückläufig?
        if (i < radix.planetTyp.size() && (radix.planetTyp[i] & P_TYP_RUCK)) {
            painter.drawText(col1X + static_cast<int>(4 * mmToPixel), posY + painter.fontMetrics().ascent(), 
                             astroFont().retrogradeSymbol());
        }
        
        // Position (Grad/Minuten)
        painter.setFont(normalFont);
        painter.setPen(Qt::black);
        painter.drawText(col1X + static_cast<int>(8 * mmToPixel), posY + painter.fontMetrics().ascent(), 
                         formatDegree(radix.planet[i], false));  // ohne Sternzeichen
        
        // Sternzeichen-Symbol separat mit zodiacFont und Element-Farbe
        painter.setFont(zodiacFont);
        int zeichen = getZeichen(radix.planet[i]);
        painter.setPen(adjustColor(getZeichenColor(zeichen)));
        painter.drawText(col1X + static_cast<int>(20 * mmToPixel), posY + painter.fontMetrics().ascent(), 
                         astroFont().sternzeichenSymbolUnicode(zeichen));
        
        // Legende: Symbol = Name
        painter.setPen(adjustColor(getPlanetColor(i, auinit)));
        painter.drawText(col3X, posY + painter.fontMetrics().ascent(), symbol);
        painter.setFont(smallFont);
        painter.setPen(Qt::black);
        painter.drawText(col3X + static_cast<int>(5 * mmToPixel), posY + painter.fontMetrics().ascent(), 
                         (i < 17) ? planetNames[i] : QString("Planet %1").arg(i));
        
        posY += lineHeight;
    }
    
    // === HÄUSER (mittlere Spalte) ===
    int hausY = tableY;
    static const char* hausNames[] = {"AC", "2", "3", "IC", "5", "6", "DC", "8", "9", "MC", "11", "12"};
    
    for (int i = 0; i < MAX_HAUS && i < radix.haus.size(); ++i) {
        painter.setFont(normalFont);
        painter.setPen(Qt::black);
        painter.drawText(col2X, hausY + painter.fontMetrics().ascent(), hausNames[i]);
        painter.drawText(col2X + static_cast<int>(8 * mmToPixel), hausY + painter.fontMetrics().ascent(), 
                         formatDegree(radix.haus[i], false));  // ohne Sternzeichen
        // Sternzeichen-Symbol separat mit Element-Farbe
        painter.setFont(zodiacFont);
        int zeichen = getZeichen(radix.haus[i]);
        painter.setPen(adjustColor(getZeichenColor(zeichen)));
        painter.drawText(col2X + static_cast<int>(20 * mmToPixel), hausY + painter.fontMetrics().ascent(), 
                         astroFont().sternzeichenSymbolUnicode(zeichen));
        hausY += lineHeight;
    }
    
    // Sternzeichen-Legende (nach Planeten)
    posY += static_cast<int>(2 * mmToPixel);
    painter.setFont(headerFont);
    painter.setPen(Qt::black);
    painter.drawText(col3X, posY + painter.fontMetrics().ascent(), "Zeichen");
    posY += painter.fontMetrics().height() + static_cast<int>(1 * mmToPixel);
    
    static const char* zeichenNames[] = {
        "Widder", "Stier", "Zwillinge", "Krebs", "Löwe", "Jungfrau",
        "Waage", "Skorpion", "Schütze", "Steinbock", "Wassermann", "Fische"
    };
    
    for (int i = 0; i < 12; ++i) {
        painter.setFont(zodiacFont);
        painter.setPen(adjustColor(getZeichenColor(i)));
        painter.drawText(col3X, posY + painter.fontMetrics().ascent(), astroFont().sternzeichenSymbolUnicode(i));
        painter.setFont(smallFont);
        painter.drawText(col3X + static_cast<int>(5 * mmToPixel), posY + painter.fontMetrics().ascent(), zeichenNames[i]);
        posY += lineHeight;
    }
    
    // === FOOTER Seite 1 ===
    painter.setFont(smallFont);
    painter.setPen(Qt::gray);
    painter.drawText(margin, pageHeight - margin, 
                     QString("Erstellt mit AstroUniverse 2026 am %1 - Seite 1")
                     .arg(QDate::currentDate().toString("dd.MM.yyyy")));
    
    // === SEITE 2+: ASPEKTE ===
    printer.newPage();
    int pageNum = 2;
    y = margin;
    
    painter.setFont(titleFont);
    painter.setPen(Qt::black);
    painter.drawText(margin, y + painter.fontMetrics().ascent(), 
                     QString("Aspekte - %1 %2").arg(radix.rFix.vorname, radix.rFix.name));
    y += painter.fontMetrics().height() + static_cast<int>(5 * mmToPixel);
    
    int aspStartY = y;
    lineHeight = static_cast<int>(4 * mmToPixel);
    int aspColWidth = (pageWidth - 2 * margin) / 3;  // 3 Spalten für mehr Platz
    int aspCol = 0;
    int aspX = margin;
    int aspY = y;
    
    for (int i = 0; i < numPlanets; ++i) {
        for (int j = i + 1; j < numPlanets; ++j) {
            int idx = i * radix.anzahlPlanet + j;
            if (idx >= radix.aspPlanet.size()) continue;
            
            int asp = static_cast<int>(radix.aspPlanet[idx]);
            if (asp < 0 && asp != KEIN_ASP) asp += 256;
            if (asp == KEIN_ASP) continue;
            
            // Orb berechnen
            double pos1 = radix.planet[i];
            double pos2 = radix.planet[j];
            double diff = std::abs(pos1 - pos2);
            if (diff > 180) diff = 360 - diff;
            double orb = std::abs(diff - asp);
            
            // Sternzeichen der Planeten
            int zeichen1 = getZeichen(pos1);
            int zeichen2 = getZeichen(pos2);
            
            // Symbole: Planet1 in Zeichen1 Aspekt Planet2 in Zeichen2 (Orb)
            QString sym1 = astroFont().planetSymbol(i);
            QString aspSym = astroFont().aspektSymbol(asp);
            QString sym2 = astroFont().planetSymbol(j);
            QString sign1 = astroFont().sternzeichenSymbolUnicode(zeichen1);
            QString sign2 = astroFont().sternzeichenSymbolUnicode(zeichen2);
            
            // Planet1
            painter.setFont(symbolFont);
            painter.setPen(adjustColor(getPlanetColor(i, auinit)));
            painter.drawText(aspX, aspY + painter.fontMetrics().ascent(), sym1);
            
            // Zeichen1
            painter.setFont(zodiacFont);
            painter.setPen(adjustColor(getZeichenColor(zeichen1)));
            painter.drawText(aspX + static_cast<int>(4 * mmToPixel), aspY + painter.fontMetrics().ascent(), sign1);
            
            // Aspekt
            painter.setFont(symbolFont);
            painter.setPen(adjustColor(getAspektColor(asp)));
            painter.drawText(aspX + static_cast<int>(8 * mmToPixel), aspY + painter.fontMetrics().ascent(), aspSym);
            
            // Planet2
            painter.setPen(adjustColor(getPlanetColor(j, auinit)));
            painter.drawText(aspX + static_cast<int>(12 * mmToPixel), aspY + painter.fontMetrics().ascent(), sym2);
            
            // Zeichen2
            painter.setFont(zodiacFont);
            painter.setPen(adjustColor(getZeichenColor(zeichen2)));
            painter.drawText(aspX + static_cast<int>(16 * mmToPixel), aspY + painter.fontMetrics().ascent(), sign2);
            
            // Orb
            painter.setFont(smallFont);
            painter.setPen(Qt::black);
            painter.drawText(aspX + static_cast<int>(20 * mmToPixel), aspY + painter.fontMetrics().ascent(), 
                             QString("%1°").arg(orb, 0, 'f', 1));
            
            aspY += lineHeight;
            
            // Neue Spalte wenn zu weit unten (Radix)
            if (aspY > pageHeight - margin - lineHeight * 2) {
                aspCol++;
                if (aspCol >= 3) {
                    // Neue Seite
                    painter.setFont(smallFont);
                    painter.setPen(Qt::gray);
                    painter.drawText(margin, pageHeight - margin, 
                                     QString("Erstellt mit AstroUniverse 2026 - Seite %1").arg(pageNum));
                    
                    printer.newPage();
                    pageNum++;
                    aspCol = 0;
                    y = margin;
                    
                    painter.setFont(titleFont);
                    painter.setPen(Qt::black);
                    painter.drawText(margin, y + painter.fontMetrics().ascent(), 
                                     QString("Aspekte - %1 %2 (Fortsetzung)").arg(radix.rFix.vorname, radix.rFix.name));
                    y += painter.fontMetrics().height() + static_cast<int>(5 * mmToPixel);
                    aspStartY = y;
                }
                aspX = margin + aspCol * aspColWidth;
                aspY = aspStartY;
            }
        }
    }
    
    // === FOOTER letzte Seite ===
    painter.setFont(smallFont);
    painter.setPen(Qt::gray);
    painter.drawText(margin, pageHeight - margin, 
                     QString("Erstellt mit AstroUniverse 2026 am %1 - Seite %2")
                     .arg(QDate::currentDate().toString("dd.MM.yyyy")).arg(pageNum));
}

QString PdfExporter::formatDegree(double degree, bool withSign) {
    while (degree < 0) degree += 360;
    while (degree >= 360) degree -= 360;
    
    int sign = static_cast<int>(degree / 30.0);
    double inSign = degree - sign * 30.0;
    
    int deg = static_cast<int>(inSign);
    int min = static_cast<int>((inSign - deg) * 60);
    
    if (withSign) {
        QString signSym = astroFont().sternzeichenSymbolUnicode(sign);
        return QString("%1°%2' %3")
            .arg(deg, 2, 10, QChar('0'))
            .arg(min, 2, 10, QChar('0'))
            .arg(signSym);
    } else {
        return QString("%1°%2'")
            .arg(deg, 2, 10, QChar('0'))
            .arg(min, 2, 10, QChar('0'));
    }
}

int PdfExporter::getZeichen(double degree) {
    while (degree < 0) degree += 360;
    while (degree >= 360) degree -= 360;
    return static_cast<int>(degree / 30.0);
}

QColor PdfExporter::getPlanetColor(int planet, const AuInit& auinit) {
    // Farben aus zentraler AuInit::initColors() verwenden
    if (planet < auinit.planetColor.size() && auinit.planetColor[planet].isValid()) {
        return auinit.planetColor[planet];
    }
    return sColor[COL_PLAN];
}

QColor PdfExporter::getAspektColor(int aspekt) {
    switch (aspekt) {
        case KONJUNKTION: return sColor[CKONJUNKTION];
        case HALBSEX:     return sColor[CHALBSEX];
        case SEXTIL:      return sColor[CSEXTIL];
        case QUADRATUR:   return sColor[CQUADRATUR];
        case TRIGON:      return sColor[CTRIGON];
        case QUINCUNX:    return sColor[CQUINCUNX];
        case OPOSITION:   return sColor[COPOSITION];
        default:          return Qt::black;
    }
}

QColor PdfExporter::getZeichenColor(int zeichen) {
    // Element-Farben wie im Chart: Feuer=0,4,8; Erde=1,5,9; Luft=2,6,10; Wasser=3,7,11
    int element = zeichen % 4;
    switch (element) {
        case 0: return sColor[COL_FEUER];   // Feuer: Widder, Löwe, Schütze
        case 1: return sColor[COL_ERDE];    // Erde: Stier, Jungfrau, Steinbock
        case 2: return sColor[COL_LUFT];    // Luft: Zwillinge, Waage, Wassermann
        case 3: return sColor[COL_WASSER];  // Wasser: Krebs, Skorpion, Fische
        default: return Qt::black;
    }
}

void PdfExporter::renderSynastriePage(QPainter& painter, QPrinter& printer,
                                       const Radix& radix1, const Radix& radix2,
                                       const AuInit& auinit, ChartWidget* chartWidget) {
    QRect pageRect = printer.pageRect(QPrinter::DevicePixel).toRect();
    int pageWidth = pageRect.width();
    int pageHeight = pageRect.height();
    
    double dpi = printer.resolution();
    double mmToPixel = dpi / 25.4;
    
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::TextAntialiasing);
    
    QFont titleFont("Arial", 14);
    titleFont.setBold(true);
    QFont headerFont("Arial", 10);
    headerFont.setBold(true);
    QFont normalFont("Arial", 9);
    QFont smallFont("Arial", 7);
    QFont symbolFont = astroFont().getPlanetSymbolFont(10);
    QFont zodiacFont = astroFont().getPlanetSymbolFont(10);
    
    int margin = static_cast<int>(10 * mmToPixel);
    int y = margin;
    
    // Helper: Farbe anpassen (weiß -> schwarz)
    auto adjustColor = [](QColor c) -> QColor {
        if (c.lightness() > 200) return Qt::black;
        return c;
    };
    
    // Helper: Datum formatieren
    auto formatDate = [](const RadixFix& r) {
        int stunde = static_cast<int>(r.zeit);
        int minute = static_cast<int>((r.zeit - stunde) * 60);
        return QString("%1.%2.%3 um %4:%5 Uhr, %6")
            .arg(r.tag, 2, 10, QChar('0'))
            .arg(r.monat, 2, 10, QChar('0'))
            .arg(r.jahr)
            .arg(stunde, 2, 10, QChar('0'))
            .arg(minute, 2, 10, QChar('0'))
            .arg(r.ort);
    };
    
    static const char* hausNames[] = {"AC", "2", "3", "IC", "5", "6", "DC", "8", "9", "MC", "11", "12"};
    
    // ========== SEITE 1: Beide Personen, Grafik, Positionen Person 1 ==========
    
    // Header: Beide Personen
    painter.setFont(titleFont);
    painter.setPen(Qt::black);
    QString title = QString("Synastrie: %1 %2  ↔  %3 %4")
        .arg(radix1.rFix.vorname, radix1.rFix.name, radix2.rFix.vorname, radix2.rFix.name);
    painter.drawText(margin, y + painter.fontMetrics().ascent(), title);
    y += painter.fontMetrics().height() + static_cast<int>(2 * mmToPixel);
    
    // Geburtsdaten beider Personen
    painter.setFont(normalFont);
    painter.drawText(margin, y + painter.fontMetrics().ascent(), 
                     QString("%1 %2: %3").arg(radix1.rFix.vorname, radix1.rFix.name, formatDate(radix1.rFix)));
    y += painter.fontMetrics().height();
    painter.drawText(margin, y + painter.fontMetrics().ascent(), 
                     QString("%1 %2: %3").arg(radix2.rFix.vorname, radix2.rFix.name, formatDate(radix2.rFix)));
    y += painter.fontMetrics().height() + static_cast<int>(5 * mmToPixel);
    
    // Synastrie-Grafik (quadratisch!)
    if (chartWidget) {
        int chartSizeMm = 110;
        int chartSize = static_cast<int>(chartSizeMm * mmToPixel);
        int chartX = (pageWidth - chartSize) / 2;
        
        // Bild ist bereits quadratisch (1200x1200) aus renderForPrint
        QImage chartImage = chartWidget->renderForPrint(1200);
        
        // Zeichne in ein quadratisches Ziel-Rechteck (erzwingt korrektes Seitenverhältnis)
        QRect targetRect(chartX, y, chartSize, chartSize);
        painter.drawImage(targetRect, chartImage);
        
        y += chartSize + static_cast<int>(3 * mmToPixel);
    }
    
    // Positionen Person 1 + Häuser + Legende (3 Spalten)
    int colWidth = (pageWidth - 4 * margin) / 3;
    int col1X = margin;
    int col2X = margin + colWidth + margin;
    int col3X = margin + 2 * (colWidth + margin);
    int tableY = y;
    
    painter.setFont(headerFont);
    painter.setPen(Qt::black);
    painter.drawText(col1X, tableY + painter.fontMetrics().ascent(), 
                     QString("Positionen %1").arg(radix1.rFix.vorname));
    painter.drawText(col2X, tableY + painter.fontMetrics().ascent(), "Häuser");
    painter.drawText(col3X, tableY + painter.fontMetrics().ascent(), "Legende");
    tableY += painter.fontMetrics().height() + static_cast<int>(2 * mmToPixel);
    
    int lineHeight = static_cast<int>(4 * mmToPixel);
    int numPlanets1 = qMin(static_cast<int>(radix1.anzahlPlanet), 17);
    int posY = tableY;
    
    static const char* planetNames[] = {
        "Sonne", "Mond", "Merkur", "Venus", "Mars", "Jupiter", "Saturn",
        "Uranus", "Neptun", "Pluto", "Mondkn.", "Lilith", "Chiron",
        "Ceres", "Pallas", "Juno", "Vesta"
    };
    static const char* zeichenNames[] = {
        "Widder", "Stier", "Zwillinge", "Krebs", "Löwe", "Jungfrau",
        "Waage", "Skorpion", "Schütze", "Steinbock", "Wassermann", "Fische"
    };
    
    for (int i = 0; i < numPlanets1 && i < radix1.planet.size(); ++i) {
        // Symbol
        painter.setFont(symbolFont);
        QString symbol = astroFont().planetSymbol(i);
        painter.setPen(adjustColor(getPlanetColor(i, auinit)));
        painter.drawText(col1X, posY + painter.fontMetrics().ascent(), symbol);
        
        // Rückläufig?
        if (i < radix1.planetTyp.size() && (radix1.planetTyp[i] & P_TYP_RUCK)) {
            painter.drawText(col1X + static_cast<int>(4 * mmToPixel), posY + painter.fontMetrics().ascent(), 
                             astroFont().retrogradeSymbol());
        }
        
        // Position (ohne Sternzeichen)
        painter.setFont(normalFont);
        painter.setPen(Qt::black);
        painter.drawText(col1X + static_cast<int>(8 * mmToPixel), posY + painter.fontMetrics().ascent(), 
                         formatDegree(radix1.planet[i], false));
        // Sternzeichen separat mit Element-Farbe
        painter.setFont(zodiacFont);
        int zeichen = getZeichen(radix1.planet[i]);
        painter.setPen(adjustColor(getZeichenColor(zeichen)));
        painter.drawText(col1X + static_cast<int>(20 * mmToPixel), posY + painter.fontMetrics().ascent(), 
                         astroFont().sternzeichenSymbolUnicode(zeichen));
        
        // Legende: Symbol = Name
        painter.setFont(symbolFont);
        painter.setPen(adjustColor(getPlanetColor(i, auinit)));
        painter.drawText(col3X, posY + painter.fontMetrics().ascent(), symbol);
        painter.setFont(smallFont);
        painter.setPen(Qt::black);
        painter.drawText(col3X + static_cast<int>(5 * mmToPixel), posY + painter.fontMetrics().ascent(), 
                         (i < 17) ? planetNames[i] : QString("Planet %1").arg(i));
        
        posY += lineHeight;
    }
    
    int hausY = tableY;
    for (int i = 0; i < MAX_HAUS && i < radix1.haus.size(); ++i) {
        painter.setFont(normalFont);
        painter.setPen(Qt::black);
        painter.drawText(col2X, hausY + painter.fontMetrics().ascent(), hausNames[i]);
        painter.drawText(col2X + static_cast<int>(8 * mmToPixel), hausY + painter.fontMetrics().ascent(), 
                         formatDegree(radix1.haus[i], false));
        // Sternzeichen separat mit Element-Farbe
        painter.setFont(zodiacFont);
        int zeichen = getZeichen(radix1.haus[i]);
        painter.setPen(adjustColor(getZeichenColor(zeichen)));
        painter.drawText(col2X + static_cast<int>(20 * mmToPixel), hausY + painter.fontMetrics().ascent(), 
                         astroFont().sternzeichenSymbolUnicode(zeichen));
        hausY += lineHeight;
    }
    
    // Sternzeichen-Legende (nach Planeten)
    posY += static_cast<int>(2 * mmToPixel);
    painter.setFont(headerFont);
    painter.setPen(Qt::black);
    painter.drawText(col3X, posY + painter.fontMetrics().ascent(), "Zeichen");
    posY += painter.fontMetrics().height() + static_cast<int>(1 * mmToPixel);
    
    for (int i = 0; i < 12; ++i) {
        painter.setFont(zodiacFont);
        painter.setPen(adjustColor(getZeichenColor(i)));
        painter.drawText(col3X, posY + painter.fontMetrics().ascent(), astroFont().sternzeichenSymbolUnicode(i));
        painter.setFont(smallFont);
        painter.drawText(col3X + static_cast<int>(5 * mmToPixel), posY + painter.fontMetrics().ascent(), zeichenNames[i]);
        posY += lineHeight;
    }
    
    // Footer Seite 1
    painter.setFont(smallFont);
    painter.setPen(Qt::gray);
    painter.drawText(margin, pageHeight - margin, 
                     QString("Erstellt mit AstroUniverse 2026 am %1 - Seite 1/2")
                     .arg(QDate::currentDate().toString("dd.MM.yyyy")));
    
    // ========== SEITE 2: Positionen Person 2 + Aspekte ==========
    printer.newPage();
    y = margin;
    
    // Header Seite 2
    painter.setFont(titleFont);
    painter.setPen(Qt::black);
    painter.drawText(margin, y + painter.fontMetrics().ascent(), 
                     QString("Synastrie: %1 %2  ↔  %3 %4 (Fortsetzung)")
                     .arg(radix1.rFix.vorname, radix1.rFix.name, radix2.rFix.vorname, radix2.rFix.name));
    y += painter.fontMetrics().height() + static_cast<int>(5 * mmToPixel);
    
    // Positionen Person 2
    painter.setFont(headerFont);
    painter.setPen(Qt::black);
    painter.drawText(margin, y + painter.fontMetrics().ascent(), 
                     QString("Positionen %1 %2").arg(radix2.rFix.vorname, radix2.rFix.name));
    y += painter.fontMetrics().height() + static_cast<int>(2 * mmToPixel);
    
    int numPlanets2 = qMin(static_cast<int>(radix2.anzahlPlanet), 17);
    for (int i = 0; i < numPlanets2 && i < radix2.planet.size(); ++i) {
        // Symbol
        painter.setFont(symbolFont);
        QString symbol = astroFont().planetSymbol(i);
        painter.setPen(adjustColor(getPlanetColor(i, auinit)));
        painter.drawText(margin, y + painter.fontMetrics().ascent(), symbol);
        
        // Rückläufig?
        if (i < radix2.planetTyp.size() && (radix2.planetTyp[i] & P_TYP_RUCK)) {
            painter.drawText(margin + static_cast<int>(4 * mmToPixel), y + painter.fontMetrics().ascent(), 
                             astroFont().retrogradeSymbol());
        }
        
        // Position (ohne Sternzeichen)
        painter.setFont(normalFont);
        painter.setPen(Qt::black);
        painter.drawText(margin + static_cast<int>(8 * mmToPixel), y + painter.fontMetrics().ascent(), 
                         formatDegree(radix2.planet[i], false));
        // Sternzeichen separat mit Element-Farbe
        painter.setFont(zodiacFont);
        int zeichen = getZeichen(radix2.planet[i]);
        painter.setPen(adjustColor(getZeichenColor(zeichen)));
        painter.drawText(margin + static_cast<int>(20 * mmToPixel), y + painter.fontMetrics().ascent(), 
                         astroFont().sternzeichenSymbolUnicode(zeichen));
        y += lineHeight;
    }
    
    y += static_cast<int>(5 * mmToPixel);
    
    // Synastrie-Aspekte
    painter.setFont(headerFont);
    painter.setPen(Qt::black);
    painter.drawText(margin, y + painter.fontMetrics().ascent(), "Synastrie-Aspekte");
    y += painter.fontMetrics().height() + static_cast<int>(2 * mmToPixel);
    
    int aspStartY = y;
    lineHeight = static_cast<int>(4 * mmToPixel);
    int aspColWidth = (pageWidth - 2 * margin) / 3;  // 3 Spalten für mehr Platz
    int aspCol = 0;
    int aspX = margin;
    int aspY = y;
    int pageNum = 2;
    
    for (int i = 0; i < numPlanets1 && i < radix1.planet.size(); ++i) {
        for (int j = 0; j < numPlanets2 && j < radix2.planet.size(); ++j) {
            double pos1 = radix1.planet[i];
            double pos2 = radix2.planet[j];
            double diff = std::abs(pos1 - pos2);
            if (diff > 180) diff = 360 - diff;
            
            int asp = -1;
            double orb = 0;
            if (diff < 8) { asp = KONJUNKTION; orb = diff; }
            else if (std::abs(diff - 30) < 3) { asp = HALBSEX; orb = std::abs(diff - 30); }
            else if (std::abs(diff - 60) < 6) { asp = SEXTIL; orb = std::abs(diff - 60); }
            else if (std::abs(diff - 90) < 8) { asp = QUADRATUR; orb = std::abs(diff - 90); }
            else if (std::abs(diff - 120) < 8) { asp = TRIGON; orb = std::abs(diff - 120); }
            else if (std::abs(diff - 150) < 3) { asp = QUINCUNX; orb = std::abs(diff - 150); }
            else if (std::abs(diff - 180) < 8) { asp = OPOSITION; orb = std::abs(diff - 180); }
            
            if (asp < 0) continue;
            
            // Sternzeichen der Planeten
            int zeichen1 = getZeichen(pos1);
            int zeichen2 = getZeichen(pos2);
            
            // Symbole: Planet1 Zeichen1 Aspekt Planet2 Zeichen2 (Orb)
            QString sym1 = astroFont().planetSymbol(i);
            QString aspSym = astroFont().aspektSymbol(asp);
            QString sym2 = astroFont().planetSymbol(j);
            QString sign1 = astroFont().sternzeichenSymbolUnicode(zeichen1);
            QString sign2 = astroFont().sternzeichenSymbolUnicode(zeichen2);
            
            // Planet1
            painter.setFont(symbolFont);
            painter.setPen(adjustColor(getPlanetColor(i, auinit)));
            painter.drawText(aspX, aspY + painter.fontMetrics().ascent(), sym1);
            
            // Zeichen1
            painter.setFont(zodiacFont);
            painter.setPen(adjustColor(getZeichenColor(zeichen1)));
            painter.drawText(aspX + static_cast<int>(4 * mmToPixel), aspY + painter.fontMetrics().ascent(), sign1);
            
            // Aspekt
            painter.setFont(symbolFont);
            painter.setPen(adjustColor(getAspektColor(asp)));
            painter.drawText(aspX + static_cast<int>(8 * mmToPixel), aspY + painter.fontMetrics().ascent(), aspSym);
            
            // Planet2
            painter.setPen(adjustColor(getPlanetColor(j, auinit)));
            painter.drawText(aspX + static_cast<int>(12 * mmToPixel), aspY + painter.fontMetrics().ascent(), sym2);
            
            // Zeichen2
            painter.setFont(zodiacFont);
            painter.setPen(adjustColor(getZeichenColor(zeichen2)));
            painter.drawText(aspX + static_cast<int>(16 * mmToPixel), aspY + painter.fontMetrics().ascent(), sign2);
            
            // Orb
            painter.setFont(smallFont);
            painter.setPen(Qt::black);
            painter.drawText(aspX + static_cast<int>(20 * mmToPixel), aspY + painter.fontMetrics().ascent(), 
                             QString("%1°").arg(orb, 0, 'f', 1));
            
            aspY += lineHeight;
            
            // Neue Spalte wenn zu weit unten (Synastrie)
            if (aspY > pageHeight - margin - lineHeight * 2) {
                aspCol++;
                if (aspCol >= 3) {  // 3 Spalten wegen mehr Platz für Zeichen
                    // Neue Seite
                    painter.setFont(smallFont);
                    painter.setPen(Qt::gray);
                    painter.drawText(margin, pageHeight - margin, 
                                     QString("Erstellt mit AstroUniverse 2026 - Seite %1").arg(pageNum));
                    
                    printer.newPage();
                    pageNum++;
                    aspCol = 0;
                    y = margin;
                    
                    painter.setFont(titleFont);
                    painter.setPen(Qt::black);
                    painter.drawText(margin, y + painter.fontMetrics().ascent(), 
                                     QString("Synastrie-Aspekte (Fortsetzung)"));
                    y += painter.fontMetrics().height() + static_cast<int>(5 * mmToPixel);
                    aspStartY = y;
                }
                aspX = margin + aspCol * aspColWidth;
                aspY = aspStartY;
            }
        }
    }
    
    // Footer letzte Seite
    painter.setFont(smallFont);
    painter.setPen(Qt::gray);
    painter.drawText(margin, pageHeight - margin, 
                     QString("Erstellt mit AstroUniverse 2026 am %1 - Seite %2")
                     .arg(QDate::currentDate().toString("dd.MM.yyyy")).arg(pageNum));
}

void PdfExporter::renderTransitPage(QPainter& painter, QPrinter& printer,
                                     const Radix& radix, const Radix& transit,
                                     const AuInit& auinit, ChartWidget* chartWidget) {
    QRect pageRect = printer.pageRect(QPrinter::DevicePixel).toRect();
    int pageWidth = pageRect.width();
    int pageHeight = pageRect.height();
    
    double dpi = printer.resolution();
    double mmToPixel = dpi / 25.4;
    
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::TextAntialiasing);
    
    QFont titleFont("Arial", 14);
    titleFont.setBold(true);
    QFont headerFont("Arial", 10);
    headerFont.setBold(true);
    QFont normalFont("Arial", 9);
    QFont smallFont("Arial", 7);
    QFont symbolFont = astroFont().getPlanetSymbolFont(10);
    QFont zodiacFont = astroFont().getPlanetSymbolFont(10);
    
    int margin = static_cast<int>(10 * mmToPixel);
    int y = margin;
    
    // Helper: Farbe anpassen (weiß -> schwarz)
    auto adjustColor = [](QColor c) -> QColor {
        if (c.lightness() > 200) return Qt::black;
        return c;
    };
    
    // Helper: Datum formatieren
    auto formatDate = [](const RadixFix& r) {
        int stunde = static_cast<int>(r.zeit);
        int minute = static_cast<int>((r.zeit - stunde) * 60);
        return QString("%1.%2.%3 um %4:%5 Uhr, %6")
            .arg(r.tag, 2, 10, QChar('0'))
            .arg(r.monat, 2, 10, QChar('0'))
            .arg(r.jahr)
            .arg(stunde, 2, 10, QChar('0'))
            .arg(minute, 2, 10, QChar('0'))
            .arg(r.ort);
    };
    
    static const char* hausNames[] = {"AC", "2", "3", "IC", "5", "6", "DC", "8", "9", "MC", "11", "12"};
    
    // ========== SEITE 1: Person-Daten, Grafik, Positionen Person ==========
    
    // Header
    painter.setFont(titleFont);
    painter.setPen(Qt::black);
    QString title = QString("Transit für %1 %2").arg(radix.rFix.vorname, radix.rFix.name);
    painter.drawText(margin, y + painter.fontMetrics().ascent(), title);
    y += painter.fontMetrics().height() + static_cast<int>(2 * mmToPixel);
    
    // Geburtsdaten Person
    painter.setFont(normalFont);
    painter.drawText(margin, y + painter.fontMetrics().ascent(), 
                     QString("Geburt: %1").arg(formatDate(radix.rFix)));
    y += painter.fontMetrics().height();
    
    // Transit-Datum
    painter.drawText(margin, y + painter.fontMetrics().ascent(), 
                     QString("Transit: %1").arg(formatDate(transit.rFix)));
    y += painter.fontMetrics().height() + static_cast<int>(5 * mmToPixel);
    
    // Transit-Grafik (quadratisch!)
    if (chartWidget) {
        int chartSizeMm = 110;
        int chartSize = static_cast<int>(chartSizeMm * mmToPixel);
        int chartX = (pageWidth - chartSize) / 2;
        
        // Bild ist bereits quadratisch (1200x1200) aus renderForPrint
        QImage chartImage = chartWidget->renderForPrint(1200);
        
        // Zeichne in ein quadratisches Ziel-Rechteck (erzwingt korrektes Seitenverhältnis)
        QRect targetRect(chartX, y, chartSize, chartSize);
        painter.drawImage(targetRect, chartImage);
        
        y += chartSize + static_cast<int>(3 * mmToPixel);
    }
    
    // Positionen Person + Häuser + Legende (3 Spalten)
    int colWidth = (pageWidth - 4 * margin) / 3;
    int col1X = margin;
    int col2X = margin + colWidth + margin;
    int col3X = margin + 2 * (colWidth + margin);
    int tableY = y;
    
    painter.setFont(headerFont);
    painter.setPen(Qt::black);
    painter.drawText(col1X, tableY + painter.fontMetrics().ascent(), 
                     QString("Radix-Positionen %1").arg(radix.rFix.vorname));
    painter.drawText(col2X, tableY + painter.fontMetrics().ascent(), "Häuser");
    painter.drawText(col3X, tableY + painter.fontMetrics().ascent(), "Legende");
    tableY += painter.fontMetrics().height() + static_cast<int>(2 * mmToPixel);
    
    int lineHeight = static_cast<int>(4 * mmToPixel);
    int numPlanetsRadix = qMin(static_cast<int>(radix.anzahlPlanet), 17);
    int posY = tableY;
    
    static const char* planetNames[] = {
        "Sonne", "Mond", "Merkur", "Venus", "Mars", "Jupiter", "Saturn",
        "Uranus", "Neptun", "Pluto", "Mondkn.", "Lilith", "Chiron",
        "Ceres", "Pallas", "Juno", "Vesta"
    };
    static const char* zeichenNames[] = {
        "Widder", "Stier", "Zwillinge", "Krebs", "Löwe", "Jungfrau",
        "Waage", "Skorpion", "Schütze", "Steinbock", "Wassermann", "Fische"
    };
    
    for (int i = 0; i < numPlanetsRadix && i < radix.planet.size(); ++i) {
        // Symbol
        painter.setFont(symbolFont);
        QString symbol = astroFont().planetSymbol(i);
        painter.setPen(adjustColor(getPlanetColor(i, auinit)));
        painter.drawText(col1X, posY + painter.fontMetrics().ascent(), symbol);
        
        // Rückläufig?
        if (i < radix.planetTyp.size() && (radix.planetTyp[i] & P_TYP_RUCK)) {
            painter.drawText(col1X + static_cast<int>(4 * mmToPixel), posY + painter.fontMetrics().ascent(), 
                             astroFont().retrogradeSymbol());
        }
        
        // Position (ohne Sternzeichen)
        painter.setFont(normalFont);
        painter.setPen(Qt::black);
        painter.drawText(col1X + static_cast<int>(8 * mmToPixel), posY + painter.fontMetrics().ascent(), 
                         formatDegree(radix.planet[i], false));
        // Sternzeichen separat mit Element-Farbe
        painter.setFont(zodiacFont);
        int zeichen = getZeichen(radix.planet[i]);
        painter.setPen(adjustColor(getZeichenColor(zeichen)));
        painter.drawText(col1X + static_cast<int>(20 * mmToPixel), posY + painter.fontMetrics().ascent(), 
                         astroFont().sternzeichenSymbolUnicode(zeichen));
        
        // Legende: Symbol = Name
        painter.setFont(symbolFont);
        painter.setPen(adjustColor(getPlanetColor(i, auinit)));
        painter.drawText(col3X, posY + painter.fontMetrics().ascent(), symbol);
        painter.setFont(smallFont);
        painter.setPen(Qt::black);
        painter.drawText(col3X + static_cast<int>(5 * mmToPixel), posY + painter.fontMetrics().ascent(), 
                         (i < 17) ? planetNames[i] : QString("Planet %1").arg(i));
        
        posY += lineHeight;
    }
    
    int hausY = tableY;
    for (int i = 0; i < MAX_HAUS && i < radix.haus.size(); ++i) {
        painter.setFont(normalFont);
        painter.setPen(Qt::black);
        painter.drawText(col2X, hausY + painter.fontMetrics().ascent(), hausNames[i]);
        painter.drawText(col2X + static_cast<int>(8 * mmToPixel), hausY + painter.fontMetrics().ascent(), 
                         formatDegree(radix.haus[i], false));
        // Sternzeichen separat mit Element-Farbe
        painter.setFont(zodiacFont);
        int zeichen = getZeichen(radix.haus[i]);
        painter.setPen(adjustColor(getZeichenColor(zeichen)));
        painter.drawText(col2X + static_cast<int>(20 * mmToPixel), hausY + painter.fontMetrics().ascent(), 
                         astroFont().sternzeichenSymbolUnicode(zeichen));
        hausY += lineHeight;
    }
    
    // Sternzeichen-Legende (nach Planeten)
    posY += static_cast<int>(2 * mmToPixel);
    painter.setFont(headerFont);
    painter.setPen(Qt::black);
    painter.drawText(col3X, posY + painter.fontMetrics().ascent(), "Zeichen");
    posY += painter.fontMetrics().height() + static_cast<int>(1 * mmToPixel);
    
    for (int i = 0; i < 12; ++i) {
        painter.setFont(zodiacFont);
        painter.setPen(adjustColor(getZeichenColor(i)));
        painter.drawText(col3X, posY + painter.fontMetrics().ascent(), astroFont().sternzeichenSymbolUnicode(i));
        painter.setFont(smallFont);
        painter.drawText(col3X + static_cast<int>(5 * mmToPixel), posY + painter.fontMetrics().ascent(), zeichenNames[i]);
        posY += lineHeight;
    }
    
    // Footer Seite 1
    painter.setFont(smallFont);
    painter.setPen(Qt::gray);
    painter.drawText(margin, pageHeight - margin, 
                     QString("Erstellt mit AstroUniverse 2026 am %1 - Seite 1")
                     .arg(QDate::currentDate().toString("dd.MM.yyyy")));
    
    // ========== SEITE 2: Transit-Positionen + Aspekte ==========
    printer.newPage();
    int pageNum = 2;
    y = margin;
    
    // Header Seite 2
    painter.setFont(titleFont);
    painter.setPen(Qt::black);
    painter.drawText(margin, y + painter.fontMetrics().ascent(), 
                     QString("Transit für %1 %2 (Fortsetzung)").arg(radix.rFix.vorname, radix.rFix.name));
    y += painter.fontMetrics().height() + static_cast<int>(5 * mmToPixel);
    
    // Transit-Positionen
    painter.setFont(headerFont);
    painter.setPen(Qt::black);
    painter.drawText(margin, y + painter.fontMetrics().ascent(), 
                     QString("Transit-Positionen (%1.%2.%3)")
                     .arg(transit.rFix.tag, 2, 10, QChar('0'))
                     .arg(transit.rFix.monat, 2, 10, QChar('0'))
                     .arg(transit.rFix.jahr));
    y += painter.fontMetrics().height() + static_cast<int>(2 * mmToPixel);
    
    int numPlanetsTransit = qMin(static_cast<int>(transit.anzahlPlanet), 17);
    for (int i = 0; i < numPlanetsTransit && i < transit.planet.size(); ++i) {
        // Symbol mit T- Prefix
        painter.setFont(normalFont);
        painter.setPen(Qt::black);
        painter.drawText(margin, y + painter.fontMetrics().ascent(), "T-");
        
        painter.setFont(symbolFont);
        QString symbol = astroFont().planetSymbol(i);
        painter.setPen(adjustColor(getPlanetColor(i, auinit)));
        painter.drawText(margin + static_cast<int>(4 * mmToPixel), y + painter.fontMetrics().ascent(), symbol);
        
        // Rückläufig?
        if (i < transit.planetTyp.size() && (transit.planetTyp[i] & P_TYP_RUCK)) {
            painter.drawText(margin + static_cast<int>(8 * mmToPixel), y + painter.fontMetrics().ascent(), 
                             astroFont().retrogradeSymbol());
        }
        
        // Position (ohne Sternzeichen)
        painter.setFont(normalFont);
        painter.setPen(Qt::black);
        painter.drawText(margin + static_cast<int>(12 * mmToPixel), y + painter.fontMetrics().ascent(), 
                         formatDegree(transit.planet[i], false));
        // Sternzeichen separat mit Element-Farbe
        painter.setFont(zodiacFont);
        int zeichen = getZeichen(transit.planet[i]);
        painter.setPen(adjustColor(getZeichenColor(zeichen)));
        painter.drawText(margin + static_cast<int>(24 * mmToPixel), y + painter.fontMetrics().ascent(), 
                         astroFont().sternzeichenSymbolUnicode(zeichen));
        y += lineHeight;
    }
    
    y += static_cast<int>(5 * mmToPixel);
    
    // Transit-Aspekte
    painter.setFont(headerFont);
    painter.setPen(Qt::black);
    painter.drawText(margin, y + painter.fontMetrics().ascent(), "Transit-Aspekte");
    y += painter.fontMetrics().height() + static_cast<int>(2 * mmToPixel);
    
    int aspStartY = y;
    lineHeight = static_cast<int>(4 * mmToPixel);
    int aspColWidth = (pageWidth - 2 * margin) / 3;  // 3 Spalten für mehr Platz
    int aspCol = 0;
    int aspX = margin;
    int aspY = y;
    
    // Transit-Aspekte: Transit-Planeten zu Radix-Planeten
    for (int i = 0; i < numPlanetsTransit && i < transit.planet.size(); ++i) {
        for (int j = 0; j < numPlanetsRadix && j < radix.planet.size(); ++j) {
            double pos1 = transit.planet[i];
            double pos2 = radix.planet[j];
            double diff = std::abs(pos1 - pos2);
            if (diff > 180) diff = 360 - diff;
            
            int asp = -1;
            double orb = 0;
            if (diff < 8) { asp = KONJUNKTION; orb = diff; }
            else if (std::abs(diff - 30) < 3) { asp = HALBSEX; orb = std::abs(diff - 30); }
            else if (std::abs(diff - 60) < 6) { asp = SEXTIL; orb = std::abs(diff - 60); }
            else if (std::abs(diff - 90) < 8) { asp = QUADRATUR; orb = std::abs(diff - 90); }
            else if (std::abs(diff - 120) < 8) { asp = TRIGON; orb = std::abs(diff - 120); }
            else if (std::abs(diff - 150) < 3) { asp = QUINCUNX; orb = std::abs(diff - 150); }
            else if (std::abs(diff - 180) < 8) { asp = OPOSITION; orb = std::abs(diff - 180); }
            
            if (asp < 0) continue;
            
            // Sternzeichen der Planeten
            int zeichen1 = getZeichen(pos1);
            int zeichen2 = getZeichen(pos2);
            
            // Symbole: T-Planet Zeichen1 Aspekt R-Planet Zeichen2 (Orb)
            QString sym1 = astroFont().planetSymbol(i);
            QString aspSym = astroFont().aspektSymbol(asp);
            QString sym2 = astroFont().planetSymbol(j);
            QString sign1 = astroFont().sternzeichenSymbolUnicode(zeichen1);
            QString sign2 = astroFont().sternzeichenSymbolUnicode(zeichen2);
            
            // T- prefix
            painter.setFont(smallFont);
            painter.setPen(Qt::black);
            painter.drawText(aspX, aspY + painter.fontMetrics().ascent(), "T");
            
            // Planet1
            painter.setFont(symbolFont);
            painter.setPen(adjustColor(getPlanetColor(i, auinit)));
            painter.drawText(aspX + static_cast<int>(2 * mmToPixel), aspY + painter.fontMetrics().ascent(), sym1);
            
            // Zeichen1
            painter.setFont(zodiacFont);
            painter.setPen(adjustColor(getZeichenColor(zeichen1)));
            painter.drawText(aspX + static_cast<int>(6 * mmToPixel), aspY + painter.fontMetrics().ascent(), sign1);
            
            // Aspekt
            painter.setFont(symbolFont);
            painter.setPen(adjustColor(getAspektColor(asp)));
            painter.drawText(aspX + static_cast<int>(10 * mmToPixel), aspY + painter.fontMetrics().ascent(), aspSym);
            
            // R- prefix
            painter.setFont(smallFont);
            painter.setPen(Qt::black);
            painter.drawText(aspX + static_cast<int>(14 * mmToPixel), aspY + painter.fontMetrics().ascent(), "R");
            
            // Planet2
            painter.setFont(symbolFont);
            painter.setPen(adjustColor(getPlanetColor(j, auinit)));
            painter.drawText(aspX + static_cast<int>(16 * mmToPixel), aspY + painter.fontMetrics().ascent(), sym2);
            
            // Zeichen2
            painter.setFont(zodiacFont);
            painter.setPen(adjustColor(getZeichenColor(zeichen2)));
            painter.drawText(aspX + static_cast<int>(20 * mmToPixel), aspY + painter.fontMetrics().ascent(), sign2);
            
            // Orb
            painter.setFont(smallFont);
            painter.setPen(Qt::black);
            painter.drawText(aspX + static_cast<int>(24 * mmToPixel), aspY + painter.fontMetrics().ascent(), 
                             QString("%1°").arg(orb, 0, 'f', 1));
            
            aspY += lineHeight;
            
            // Neue Spalte wenn zu weit unten
            if (aspY > pageHeight - margin - lineHeight * 2) {
                aspCol++;
                if (aspCol >= 3) {
                    // Neue Seite
                    painter.setFont(smallFont);
                    painter.setPen(Qt::gray);
                    painter.drawText(margin, pageHeight - margin, 
                                     QString("Erstellt mit AstroUniverse 2026 - Seite %1").arg(pageNum));
                    
                    printer.newPage();
                    pageNum++;
                    aspCol = 0;
                    y = margin;
                    
                    painter.setFont(titleFont);
                    painter.setPen(Qt::black);
                    painter.drawText(margin, y + painter.fontMetrics().ascent(), 
                                     QString("Transit-Aspekte (Fortsetzung)"));
                    y += painter.fontMetrics().height() + static_cast<int>(5 * mmToPixel);
                    aspStartY = y;
                }
                aspX = margin + aspCol * aspColWidth;
                aspY = aspStartY;
            }
        }
    }
    
    // Footer letzte Seite
    painter.setFont(smallFont);
    painter.setPen(Qt::gray);
    painter.drawText(margin, pageHeight - margin, 
                     QString("Erstellt mit AstroUniverse 2026 am %1 - Seite %2")
                     .arg(QDate::currentDate().toString("dd.MM.yyyy")).arg(pageNum));
}

} // namespace astro
