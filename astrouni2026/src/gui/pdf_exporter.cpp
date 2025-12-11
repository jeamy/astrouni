/**
 * @file pdf_exporter.cpp
 * @brief Implementierung des PDF-Exports für Horoskope
 */

#include "pdf_exporter.h"
#include "../core/astro_font_provider.h"
#include "../core/constants.h"
#include "chart_widget.h"

#include <QDate>
#include <QFileDialog>
#include <QPainter>
#include <QPixmap>
#include <QPrintDialog>
#include <QPrinter>
#include <cmath>

namespace astro {

// Statische Daten für Namen (global für diese Datei)
static const char *planetNames[] = {
    "Sonne",  "Mond",   "Merkur", "Venus", "Mars",    "Jupiter",
    "Saturn", "Uranus", "Neptun", "Pluto", "Mondkn.", "Lilith",
    "Chiron", "Ceres",  "Pallas", "Juno",  "Vesta"};

static const char *zeichenNames[] = {
    "Widder", "Stier",    "Zwillinge", "Krebs",     "Löwe",       "Jungfrau",
    "Waage",  "Skorpion", "Schütze",   "Steinbock", "Wassermann", "Fische"};

static const char *hausNames[] = {"AC", "2", "3", "IC", "5",  "6",
                                  "DC", "8", "9", "MC", "11", "12"};

struct AspLegende {
  int id;
  const char *name;
};

static const AspLegende aspekte[] = {
    {KONJUNKTION, "Konjunktion"}, {SEXTIL, "Sextil"},
    {QUADRATUR, "Quadrat"},       {TRIGON, "Trigon"},
    {OPOSITION, "Opposition"},    {QUINCUNX, "Quincunx"},
    {HALBSEX, "Halbsextil"}};

PdfExporter::PdfExporter(QObject *parent) : QObject(parent) {}

bool PdfExporter::exportRadix(const Radix &radix, const AuInit &auinit,
                              ChartWidget *chartWidget, QWidget *parent) {
  // Datei-Dialog für PDF-Speicherort
  QString fileName = QFileDialog::getSaveFileName(
      parent, tr("Horoskop als PDF exportieren"),
      QString("%1_%2.pdf").arg(radix.rFix.vorname, radix.rFix.name),
      tr("PDF-Dateien (*.pdf)"));

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

bool PdfExporter::printRadix(const Radix &radix, const AuInit &auinit,
                             ChartWidget *chartWidget, QWidget *parent) {
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

bool PdfExporter::exportSynastrie(const Radix &radix1, const Radix &radix2,
                                  const AuInit &auinit,
                                  ChartWidget *chartWidget, QWidget *parent) {
  QString fileName = QFileDialog::getSaveFileName(
      parent, tr("Synastrie als PDF exportieren"),
      QString("%1_%2_Synastrie.pdf")
          .arg(radix1.rFix.vorname, radix2.rFix.vorname),
      tr("PDF-Dateien (*.pdf)"));

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

bool PdfExporter::printSynastrie(const Radix &radix1, const Radix &radix2,
                                 const AuInit &auinit, ChartWidget *chartWidget,
                                 QWidget *parent) {
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

bool PdfExporter::exportTransit(const Radix &radix, const Radix &transit,
                                const AuInit &auinit, ChartWidget *chartWidget,
                                QWidget *parent) {
  QString fileName = QFileDialog::getSaveFileName(
      parent, tr("Transit als PDF exportieren"),
      QString("%1_%2_Transit.pdf").arg(radix.rFix.vorname, radix.rFix.name),
      tr("PDF-Dateien (*.pdf)"));

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

bool PdfExporter::printTransit(const Radix &radix, const Radix &transit,
                               const AuInit &auinit, ChartWidget *chartWidget,
                               QWidget *parent) {
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

void PdfExporter::renderPage(QPainter &painter, QPrinter &printer,
                             const Radix &radix, const AuInit &auinit,
                             ChartWidget *chartWidget) {
  QRect pageRect = printer.pageRect(QPrinter::DevicePixel).toRect();
  int pageWidth = pageRect.width();
  int pageHeight = pageRect.height();

  double dpiX = printer.logicalDpiX();
  double dpiY = printer.logicalDpiY();
  double mmToPixelX = dpiX / 25.4;
  double mmToPixelY = dpiY / 25.4;

  painter.setRenderHint(QPainter::Antialiasing);
  painter.setRenderHint(QPainter::TextAntialiasing);

  // Fonts
  QFont titleFont("DejaVu Sans", 14);
  titleFont.setBold(true);
  QFont headerFont("DejaVu Sans", 10);
  headerFont.setBold(true);
  QFont normalFont("DejaVu Sans", 9);
  QFont smallFont("DejaVu Sans", 7);
  // Planeten/Asteroiden: getPlanetSymbolFont (DejaVu Sans für Glyphen)
  QFont symbolFont = astroFont().getPlanetSymbolFont(10);
  // Sternzeichen: getSymbolFont (AstroUniverse) - Explizit vom User gefordert
  QFont zodiacFont = astroFont().getSymbolFont(10);

  int marginX = static_cast<int>(10 * mmToPixelX);
  int marginY = static_cast<int>(10 * mmToPixelY);
  int y = marginY;

  // Helper: Farbe anpassen (weiß -> schwarz)
  auto adjustColor = [](QColor c) -> QColor {
    if (c.lightness() > 200)
      return Qt::black;
    return c;
  };

  // === HEADER ===
  painter.setFont(titleFont);
  painter.setPen(Qt::black);
  QString fullName = QString("%1 %2").arg(radix.rFix.vorname, radix.rFix.name);
  painter.drawText(marginX, y + painter.fontMetrics().ascent(), fullName);
  y += painter.fontMetrics().height() + static_cast<int>(2 * mmToPixelY);

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
  painter.drawText(marginX, y + painter.fontMetrics().ascent(), gebInfo);
  y += painter.fontMetrics().height() + static_cast<int>(3 * mmToPixelY);

  // === RADIX-GRAFIK (quadratisch!) ===
  // === RADIX-GRAFIK (quadratisch!) ===
  if (chartWidget) {
    int chartWidth =
        //        static_cast<int>((pageWidth - 2 * marginX) * 1); // 100% der
        //        Breite
        static_cast<int>((pageWidth * 0.95)); // 95% der Breite
    int chartHeight = chartWidth;             // Quadratisch

    // Falls zu hoch für die Seite (mit Header), skalieren
    int availableHeight = pageHeight - y - marginY;
    if (chartHeight > availableHeight) {
      chartHeight = availableHeight;
      chartWidth = chartHeight;
    }

    int chartX = (pageWidth - chartWidth) / 2;

    // Vertikal zentrieren im verbleibenden Platz
    int chartY = y + (availableHeight - chartHeight) / 3;

    // Bild ist bereits quadratisch (1200x1200) aus renderForPrint
    QImage chartImage = chartWidget->renderForPrint(1200);

    // Zeichne in ein Ziel-Rechteck mit korrekten physischen Dimensionen
    QRect targetRect(chartX, chartY, chartWidth, chartHeight);
    painter.drawImage(targetRect, chartImage);

    // y += chartHeight + static_cast<int>(3 * mmToPixelY);
  }

  // === SEITE 2: TABELLEN ===
  printer.newPage();
  int pageNum =
      2; // Start counting pages here? Aspect loop resets this or continues?
  // Code uses pageNum later for Aspects "Page 2" text.
  // We probably want Page 1, Page 2, Page 3 etc.
  // Let's rely on manual page count if needed or just handle header.

  y = marginY;

  // Header auf Seite 2 (optional, aber sinnvoll zur Identifikation)
  painter.setFont(titleFont);
  painter.setPen(Qt::black);
  painter.drawText(
      marginX, y + painter.fontMetrics().ascent(),
      QString("%1 %2 - Daten").arg(radix.rFix.vorname, radix.rFix.name));
  y += painter.fontMetrics().height() + static_cast<int>(5 * mmToPixelY);

  // === DREI-SPALTEN-LAYOUT: Positionen | Häuser | Legende ===
  int colWidth = (pageWidth - 4 * marginX) / 3;
  int col1X = marginX;
  int col2X = marginX + colWidth + marginX;
  int col3X = marginX + 2 * (colWidth + marginX);
  int tableY = y;

  // === POSITIONEN (linke Spalte) ===
  painter.setFont(headerFont);
  painter.setPen(Qt::black);
  painter.drawText(col1X, tableY + painter.fontMetrics().ascent(),
                   "Positionen");

  // === HÄUSER (mittlere Spalte) ===
  painter.drawText(col2X, tableY + painter.fontMetrics().ascent(), "Häuser");

  // === LEGENDE (rechte Spalte) ===
  painter.drawText(col3X, tableY + painter.fontMetrics().ascent(), "Legende");

  tableY += painter.fontMetrics().height() + static_cast<int>(1 * mmToPixelY);

  int lineHeight = static_cast<int>(5.5 * mmToPixelY);
  int numPlanets = qMin(static_cast<int>(radix.anzahlPlanet), 17);
  int posY = tableY;

  // Planeten-Namen für Legende (jetzt global static)
  // static const char *planetNames[] = ... entfernt

  for (int i = 0; i < numPlanets && i < radix.planet.size(); ++i) {
    // Planet-Symbol
    painter.setFont(symbolFont);
    QString symbol = astroFont().planetSymbol(i);
    painter.setPen(adjustColor(getPlanetColor(i, auinit)));
    painter.drawText(col1X, posY + painter.fontMetrics().ascent(), symbol);

    // Rückläufig?
    if (i < radix.planetTyp.size() && (radix.planetTyp[i] & P_TYP_RUCK)) {
      painter.drawText(col1X + static_cast<int>(4 * mmToPixelX),
                       posY + painter.fontMetrics().ascent(),
                       astroFont().retrogradeSymbol());
    }

    // Position (Grad/Minuten)
    painter.setFont(normalFont);
    painter.setPen(Qt::black);
    painter.drawText(col1X + static_cast<int>(8 * mmToPixelX),
                     posY + painter.fontMetrics().ascent(),
                     formatDegree(radix.planet[i], false)); // ohne Sternzeichen

    // Sternzeichen-Symbol separat mit zodiacFont und Element-Farbe
    painter.setFont(zodiacFont);
    int zeichen = getZeichen(radix.planet[i]);
    painter.setPen(adjustColor(getZeichenColor(zeichen)));
    painter.drawText(col1X + static_cast<int>(20 * mmToPixelX),
                     posY + painter.fontMetrics().ascent(),
                     astroFont().sternzeichenSymbol(zeichen));

    // Legende: Symbol = Name
    painter.setPen(adjustColor(getPlanetColor(i, auinit)));
    painter.drawText(col3X, posY + painter.fontMetrics().ascent(), symbol);
    painter.setFont(smallFont);
    painter.setPen(Qt::black);
    painter.drawText(col3X + static_cast<int>(5 * mmToPixelX),
                     posY + painter.fontMetrics().ascent(),
                     (i < 17) ? planetNames[i] : QString("Planet %1").arg(i));

    posY += lineHeight;
  }

  // === HÄUSER (mittlere Spalte) ===
  int hausY = tableY;
  // static const char *hausNames[] = ... entfernt (global)

  for (int i = 0; i < MAX_HAUS && i < radix.haus.size(); ++i) {
    painter.setFont(normalFont);
    painter.setPen(Qt::black);
    painter.drawText(col2X, hausY + painter.fontMetrics().ascent(),
                     hausNames[i]);
    painter.drawText(col2X + static_cast<int>(8 * mmToPixelX),
                     hausY + painter.fontMetrics().ascent(),
                     formatDegree(radix.haus[i], false)); // ohne Sternzeichen
    // Sternzeichen-Symbol separat mit Element-Farbe
    painter.setFont(zodiacFont);
    int zeichen = getZeichen(radix.haus[i]);
    painter.setPen(adjustColor(getZeichenColor(zeichen)));
    painter.drawText(col2X + static_cast<int>(20 * mmToPixelX),
                     hausY + painter.fontMetrics().ascent(),
                     astroFont().sternzeichenSymbol(zeichen));
    hausY += lineHeight;
  }

  // Sternzeichen-Legende (nach Planeten)
  posY += static_cast<int>(2 * mmToPixelY);
  painter.setFont(headerFont);
  painter.setPen(Qt::black);
  painter.drawText(col3X, posY + painter.fontMetrics().ascent(), "Zeichen");
  posY += painter.fontMetrics().height() + static_cast<int>(1 * mmToPixelY);

  // static const char *zeichenNames[] = ... entfernt (global)

  for (int i = 0; i < 12; ++i) {
    painter.setFont(zodiacFont);
    painter.setPen(adjustColor(getZeichenColor(i)));
    painter.drawText(col3X, posY + painter.fontMetrics().ascent(),
                     astroFont().sternzeichenSymbol(i));
    painter.setFont(smallFont);
    painter.drawText(col3X + static_cast<int>(5 * mmToPixelX),
                     posY + painter.fontMetrics().ascent(), zeichenNames[i]);
    posY += lineHeight;
  }

  // --- ASPEKTE LEGENDE ---
  posY += static_cast<int>(2 * mmToPixelY);
  painter.setFont(headerFont);
  painter.setPen(Qt::black);
  painter.drawText(col3X, posY + painter.fontMetrics().ascent(), "Aspekte");
  posY += lineHeight + static_cast<int>(1 * mmToPixelY);

  for (const auto &asp : aspekte) {
    painter.setFont(symbolFont);
    painter.setPen(adjustColor(getAspektColor(asp.id)));
    painter.drawText(col3X, posY + painter.fontMetrics().ascent(),
                     astroFont().aspektSymbol(asp.id));

    painter.setFont(smallFont);
    painter.setPen(Qt::black);
    painter.drawText(col3X + static_cast<int>(5 * mmToPixelX),
                     posY + painter.fontMetrics().ascent(), asp.name);
    posY += lineHeight;
  }

  // === FOOTER Seite 2 (Tabellen) ===
  painter.setFont(smallFont);
  painter.setPen(Qt::gray);
  painter.drawText(marginX, pageHeight - marginY,
                   QString("Erstellt mit AstroUniverse 2026 am %1 - Seite 2")
                       .arg(QDate::currentDate().toString("dd.MM.yyyy")));

  // === SEITE 3+: ASPEKTE ===
  // printer.newPage(); // Wir sind schon auf Seite 2, Aspekte sollen danach
  // kommen (vielleicht auf Seite 2 unten oder Seite 3) Überprüfen ob noch Platz
  // auf Seite 2 ist? Die Tabellen nehmen ca. (17 Planeten * 3.5mm) ~ 60mm +
  // Header ein. Das sollte auf den Rest von Seite 2 passen, wenn wir keinen
  // PageBreak erzwingen. ABER: Der Code unten macht explizit printer.newPage().
  // User wollte "Aspekte anschließend".
  // Wenn wir printer.newPage() entfernen, werden Aspekte unter die Tabellen
  // gezeichnet. Wenn wir es lassen, kommen sie auf Seite 3. User sagte "der
  // rest bleibt gleich, nur eine seite nach hinten verschben". Das heißt,
  // Aspekte starteten vorher auf Seite 2. Jetzt auf Seite 3? "den text für die
  // position/häuser/legende auf die zweite seite verschieben... es wird also
  // eine seite eingefügt."

  printer.newPage();
  pageNum = 3;
  y = marginY;

  painter.setFont(titleFont);
  painter.setPen(Qt::black);
  painter.drawText(
      marginX, y + painter.fontMetrics().ascent(),
      QString("Aspekte - %1 %2").arg(radix.rFix.vorname, radix.rFix.name));
  y += painter.fontMetrics().height() + static_cast<int>(5 * mmToPixelY);

  int aspStartY = y;
  // lineHeight use existing (4.5mm)
  int aspColWidth = (pageWidth - 2 * marginX) / 3; // 3 Spalten für mehr Platz
  int aspCol = 0;
  int aspX = marginX;
  int aspY = y;

  for (int i = 0; i < numPlanets && i < radix.planet.size(); ++i) {
    for (int j = i + 1; j < numPlanets; ++j) {
      int idx = i * radix.anzahlPlanet + j;
      if (idx >= radix.aspPlanet.size())
        continue;

      int asp = static_cast<int>(radix.aspPlanet[idx]);
      if (asp < 0 && asp != KEIN_ASP)
        asp += 256;
      if (asp == KEIN_ASP)
        continue;

      // Orb berechnen
      double pos1 = radix.planet[i];
      double pos2 = radix.planet[j];
      double diff = std::abs(pos1 - pos2);
      if (diff > 180)
        diff = 360 - diff;
      double orb = std::abs(diff - asp);

      // Sternzeichen der Planeten
      int zeichen1 = getZeichen(pos1);
      int zeichen2 = getZeichen(pos2);

      // Symbole: Planet1 in Zeichen1 Aspekt Planet2 in Zeichen2 (Orb)
      QString sym1 = astroFont().planetSymbol(i);
      QString aspSym = astroFont().aspektSymbol(asp);
      QString sym2 = astroFont().planetSymbol(j);
      QString sign1 = astroFont().sternzeichenSymbol(zeichen1);
      QString sign2 = astroFont().sternzeichenSymbol(zeichen2);

      // Planet1
      painter.setFont(symbolFont);
      painter.setPen(adjustColor(getPlanetColor(i, auinit)));
      painter.drawText(aspX, aspY + painter.fontMetrics().ascent(), sym1);

      // Zeichen1
      painter.setFont(zodiacFont);
      painter.setPen(adjustColor(getZeichenColor(zeichen1)));
      painter.drawText(aspX + static_cast<int>(6 * mmToPixelX),
                       aspY + painter.fontMetrics().ascent(), sign1);

      // Aspekt
      painter.setFont(symbolFont);
      painter.setPen(adjustColor(getAspektColor(asp)));
      painter.drawText(aspX + static_cast<int>(12 * mmToPixelX),
                       aspY + painter.fontMetrics().ascent(), aspSym);

      // Planet2
      painter.setPen(adjustColor(getPlanetColor(j, auinit)));
      painter.drawText(aspX + static_cast<int>(18 * mmToPixelX),
                       aspY + painter.fontMetrics().ascent(), sym2);

      // Zeichen2
      painter.setFont(zodiacFont);
      painter.setPen(adjustColor(getZeichenColor(zeichen2)));
      painter.drawText(aspX + static_cast<int>(24 * mmToPixelX),
                       aspY + painter.fontMetrics().ascent(), sign2);

      // Orb
      painter.setFont(smallFont);
      painter.setPen(Qt::black);
      painter.drawText(aspX + static_cast<int>(30 * mmToPixelX),
                       aspY + painter.fontMetrics().ascent(),
                       QString("%1°").arg(orb, 0, 'f', 1));

      aspY += lineHeight;

      // Neue Spalte wenn zu weit unten (Radix)
      if (aspY > pageHeight - marginY - lineHeight * 2) {
        aspCol++;
        if (aspCol >= 3) {
          // Neue Seite
          painter.setFont(smallFont);
          painter.setPen(Qt::gray);
          painter.drawText(marginX, pageHeight - marginY,
                           QString("Erstellt mit AstroUniverse 2026 - Seite %1")
                               .arg(pageNum));

          printer.newPage();
          pageNum++;
          aspCol = 0;
          y = marginY;

          painter.setFont(titleFont);
          painter.setPen(Qt::black);
          painter.drawText(marginX, y + painter.fontMetrics().ascent(),
                           QString("Aspekte - %1 %2 (Fortsetzung)")
                               .arg(radix.rFix.vorname, radix.rFix.name));
          y +=
              painter.fontMetrics().height() + static_cast<int>(5 * mmToPixelY);
          aspStartY = y;
        }
        aspX = marginX + aspCol * aspColWidth;
        aspY = aspStartY;
      }
    }
  }

  // === FOOTER letzte Seite ===
  painter.setFont(smallFont);
  painter.setPen(Qt::gray);
  painter.drawText(marginX, pageHeight - marginY,
                   QString("Erstellt mit AstroUniverse 2026 am %1 - Seite %2")
                       .arg(QDate::currentDate().toString("dd.MM.yyyy"))
                       .arg(pageNum));
}

QString PdfExporter::formatDegree(double degree, bool withSign) {
  while (degree < 0)
    degree += 360;
  while (degree >= 360)
    degree -= 360;

  int sign = static_cast<int>(degree / 30.0);
  double inSign = degree - sign * 30.0;

  int deg = static_cast<int>(inSign);
  int min = static_cast<int>((inSign - deg) * 60);

  if (withSign) {
    QString signSym = astroFont().sternzeichenSymbol(sign);
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
  while (degree < 0)
    degree += 360;
  while (degree >= 360)
    degree -= 360;
  return static_cast<int>(degree / 30.0);
}

QColor PdfExporter::getPlanetColor(int planet, const AuInit &auinit) {
  // Farben aus zentraler AuInit::initColors() verwenden
  if (planet < auinit.planetColor.size() &&
      auinit.planetColor[planet].isValid()) {
    return auinit.planetColor[planet];
  }
  return sColor[COL_PLAN];
}

QColor PdfExporter::getAspektColor(int aspekt) {
  switch (aspekt) {
  case KONJUNKTION:
    return sColor[CKONJUNKTION];
  case HALBSEX:
    return sColor[CHALBSEX];
  case SEXTIL:
    return sColor[CSEXTIL];
  case QUADRATUR:
    return sColor[CQUADRATUR];
  case TRIGON:
    return sColor[CTRIGON];
  case QUINCUNX:
    return sColor[CQUINCUNX];
  case OPOSITION:
    return sColor[COPOSITION];
  default:
    return Qt::black;
  }
}

QColor PdfExporter::getZeichenColor(int zeichen) {
  // Element-Farben wie im Chart: Feuer=0,4,8; Erde=1,5,9; Luft=2,6,10;
  // Wasser=3,7,11
  int element = zeichen % 4;
  switch (element) {
  case 0:
    return sColor[COL_FEUER]; // Feuer: Widder, Löwe, Schütze
  case 1:
    return sColor[COL_ERDE]; // Erde: Stier, Jungfrau, Steinbock
  case 2:
    return sColor[COL_LUFT]; // Luft: Zwillinge, Waage, Wassermann
  case 3:
    return sColor[COL_WASSER]; // Wasser: Krebs, Skorpion, Fische
  default:
    return Qt::black;
  }
}

void PdfExporter::renderSynastriePage(QPainter &painter, QPrinter &printer,
                                      const Radix &radix1, const Radix &radix2,
                                      const AuInit &auinit,
                                      ChartWidget *chartWidget) {
  QRect pageRect = printer.pageRect(QPrinter::DevicePixel).toRect();
  int pageWidth = pageRect.width();
  int pageHeight = pageRect.height();

  double dpi = printer.resolution();
  double mmToPixel = dpi / 25.4;

  painter.setRenderHint(QPainter::Antialiasing);
  painter.setRenderHint(QPainter::TextAntialiasing);

  QFont titleFont("DejaVu Sans", 14);
  titleFont.setBold(true);
  QFont headerFont("DejaVu Sans", 10);
  headerFont.setBold(true);
  QFont normalFont("DejaVu Sans", 9);
  QFont smallFont("DejaVu Sans", 7);
  QFont symbolFont = astroFont().getPlanetSymbolFont(10);
  QFont zodiacFont =
      astroFont().getSymbolFont(10); // Changed from getPlanetSymbolFont

  int margin = static_cast<int>(10 * mmToPixel);
  int y = margin;

  // Helper: Farbe anpassen (weiß -> schwarz)
  auto adjustColor = [](QColor c) -> QColor {
    if (c.lightness() > 200)
      return Qt::black;
    return c;
  };

  // Helper: Datum formatieren
  auto formatDate = [](const RadixFix &r) {
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

  // static const char *hausNames[] = ... entfernt (global)

  // ========== SEITE 1: Beide Personen, Grafik, Positionen Person 1 ==========

  // Helper variables for this scope
  double dpiX = printer.logicalDpiX();
  double dpiY = printer.logicalDpiY();
  double mmToPixelX = dpiX / 25.4;
  double mmToPixelY = dpiY / 25.4;
  int marginX = static_cast<int>(10 * mmToPixelX);
  int marginY = static_cast<int>(10 * mmToPixelY);

  // Header
  painter.setFont(titleFont);
  painter.setPen(Qt::black);
  QString title = QString("Synastrie: %1 %2  ↔  %3 %4")
                      .arg(radix1.rFix.vorname, radix1.rFix.name,
                           radix2.rFix.vorname, radix2.rFix.name);
  painter.drawText(marginX, y + painter.fontMetrics().ascent(), title);
  y += painter.fontMetrics().height() + static_cast<int>(2 * mmToPixelY);

  // Geburtsdaten beider Personen
  painter.setFont(normalFont);
  painter.drawText(
      marginX, y + painter.fontMetrics().ascent(),
      QString("%1 %2: %3")
          .arg(radix1.rFix.vorname, radix1.rFix.name, formatDate(radix1.rFix)));
  y += painter.fontMetrics().height();
  painter.drawText(
      marginX, y + painter.fontMetrics().ascent(),
      QString("%1 %2: %3")
          .arg(radix2.rFix.vorname, radix2.rFix.name, formatDate(radix2.rFix)));
  y += painter.fontMetrics().height() + static_cast<int>(5 * mmToPixelY);

  // Synastrie-Grafik (quadratisch!)
  if (chartWidget) {
    int chartWidth = static_cast<int>((pageWidth - 2 * marginX) * 0.95);
    // User change in renderPage:
    // static_cast<int>((pageWidth * 0.95));
    // Replicating exactly what user did:
    chartWidth = static_cast<int>((pageWidth * 0.95));

    int chartHeight = chartWidth;

    if (chartHeight > (pageHeight - y - marginY)) {
      chartHeight = pageHeight - y - marginY;
      chartWidth = chartHeight;
    }

    int chartX = (pageWidth - chartWidth) / 2;

    // Bild ist bereits quadratisch (1200x1200) aus renderForPrint
    QImage chartImage = chartWidget->renderForPrint(1200);

    // Vertikal zentrieren (User: / 3)
    int availableHeight = pageHeight - y - marginY;
    int chartY = y + (availableHeight - chartHeight) / 3;

    // Zeichne in ein Ziel-Rechteck mit korrekten physischen Dimensionen
    QRect targetRect(chartX, chartY, chartWidth, chartHeight);
    painter.drawImage(targetRect, chartImage);

    // y += chartHeight + static_cast<int>(3 * mmToPixelY);
  }

  // Footer Seite 1
  painter.setFont(smallFont);
  painter.setPen(Qt::gray);
  painter.drawText(marginX, pageHeight - marginY,
                   QString("Erstellt mit AstroUniverse 2026 am %1 - Seite 1")
                       .arg(QDate::currentDate().toString("dd.MM.yyyy")));

  // ========== SEITE 2: Positionen Person 1 + Häuser + Legende ==========
  printer.newPage();
  y = marginY;

  // Header Seite 2
  painter.setFont(titleFont);
  painter.setPen(Qt::black);
  painter.drawText(marginX, y + painter.fontMetrics().ascent(),
                   QString("Daten (Fortsetzung)"));
  y += painter.fontMetrics().height() + static_cast<int>(5 * mmToPixelY);

  // Positionen Person 1 + Häuser + Legende (3 Spalten)
  int colWidth = (pageWidth - 4 * marginX) / 3;
  int col1X = marginX;
  int col2X = marginX + colWidth + marginX;
  int col3X = marginX + 2 * (colWidth + marginX);
  int tableY = y;

  // Line Height 4.5mm (User Request)
  // Line Height 5.5mm (User Request)
  int lineHeight = static_cast<int>(5.5 * mmToPixelY);

  painter.setFont(headerFont);
  painter.setPen(Qt::black);
  painter.drawText(col1X, tableY + painter.fontMetrics().ascent(),
                   QString("Positionen %1").arg(radix1.rFix.vorname));
  painter.drawText(col2X, tableY + painter.fontMetrics().ascent(), "Häuser");
  painter.drawText(col3X, tableY + painter.fontMetrics().ascent(), "Legende");
  tableY += lineHeight + static_cast<int>(2 * mmToPixelY);

  // int lineHeight = static_cast<int>(4 * mmToPixelY); // OLD
  int numPlanets1 = qMin(static_cast<int>(radix1.anzahlPlanet), 17);
  int posY = tableY;

  // static const char *planetNames[] ... entfernt
  // static const char *zeichenNames[] ... entfernt

  for (int i = 0; i < numPlanets1 && i < radix1.planet.size(); ++i) {
    // Symbol
    painter.setFont(symbolFont);
    QString symbol = astroFont().planetSymbol(i);
    painter.setPen(adjustColor(getPlanetColor(i, auinit)));
    painter.drawText(col1X, posY + painter.fontMetrics().ascent(), symbol);

    // Rückläufig?
    if (i < radix1.planetTyp.size() && (radix1.planetTyp[i] & P_TYP_RUCK)) {
      painter.drawText(col1X + static_cast<int>(4 * mmToPixelX),
                       posY + painter.fontMetrics().ascent(),
                       astroFont().retrogradeSymbol());
    }

    // Position (ohne Sternzeichen)
    painter.setFont(normalFont);
    painter.setPen(Qt::black);
    painter.drawText(col1X + static_cast<int>(8 * mmToPixelX),
                     posY + painter.fontMetrics().ascent(),
                     formatDegree(radix1.planet[i], false));
    // Sternzeichen separat mit Element-Farbe
    // Sternzeichen separat mit Element-Farbe
    painter.setFont(zodiacFont);
    int zeichen = getZeichen(radix1.planet[i]);
    painter.setPen(adjustColor(getZeichenColor(zeichen)));
    painter.drawText(col1X + static_cast<int>(20 * mmToPixel),
                     posY + painter.fontMetrics().ascent(),
                     astroFont().sternzeichenSymbol(zeichen));

    // Legende: Symbol = Name
    painter.setFont(symbolFont);
    painter.setPen(adjustColor(getPlanetColor(i, auinit)));
    painter.drawText(col3X, posY + painter.fontMetrics().ascent(), symbol);
    painter.setFont(smallFont);
    painter.setPen(Qt::black);
    painter.drawText(col3X + static_cast<int>(5 * mmToPixelX),
                     posY + painter.fontMetrics().ascent(),
                     (i < 17) ? planetNames[i] : QString("Planet %1").arg(i));

    posY += lineHeight;
  }

  int hausY = tableY;
  for (int i = 0; i < MAX_HAUS && i < radix1.haus.size(); ++i) {
    painter.setFont(normalFont);
    painter.setPen(Qt::black);
    painter.drawText(col2X, hausY + painter.fontMetrics().ascent(),
                     hausNames[i]);
    painter.drawText(col2X + static_cast<int>(8 * mmToPixelX),
                     hausY + painter.fontMetrics().ascent(),
                     formatDegree(radix1.haus[i], false));
    // Sternzeichen separat mit Element-Farbe
    // Sternzeichen separat mit Element-Farbe
    painter.setFont(zodiacFont);
    int zeichen = getZeichen(radix1.haus[i]);
    painter.setPen(adjustColor(getZeichenColor(zeichen)));
    painter.drawText(col2X + static_cast<int>(20 * mmToPixel),
                     hausY + painter.fontMetrics().ascent(),
                     astroFont().sternzeichenSymbol(zeichen));
    hausY += lineHeight;
  }

  // Sternzeichen-Legende (nach Planeten)
  posY += static_cast<int>(2 * mmToPixelY);
  painter.setFont(headerFont);
  painter.setPen(Qt::black);
  painter.drawText(col3X, posY + painter.fontMetrics().ascent(), "Zeichen");
  posY += lineHeight + static_cast<int>(1 * mmToPixel);

  for (int i = 0; i < 12; ++i) {
    painter.setFont(zodiacFont);
    painter.setPen(adjustColor(getZeichenColor(i)));
    painter.drawText(col3X, posY + painter.fontMetrics().ascent(),
                     astroFont().sternzeichenSymbol(i));
    painter.setFont(smallFont);
    painter.drawText(col3X + static_cast<int>(5 * mmToPixel),
                     posY + painter.fontMetrics().ascent(), zeichenNames[i]);
    posY += lineHeight;
  }

  // --- ASPEKTE LEGENDE ---
  posY += static_cast<int>(2 * mmToPixelY);
  painter.setFont(headerFont);
  painter.setPen(Qt::black);
  painter.drawText(col3X, posY + painter.fontMetrics().ascent(), "Aspekte");
  posY += lineHeight + static_cast<int>(1 * mmToPixelY);

  for (const auto &asp : aspekte) {
    painter.setFont(symbolFont);
    painter.setPen(adjustColor(getAspektColor(asp.id)));
    painter.drawText(col3X, posY + painter.fontMetrics().ascent(),
                     astroFont().aspektSymbol(asp.id));

    painter.setFont(smallFont);
    painter.setPen(Qt::black);
    painter.drawText(col3X + static_cast<int>(5 * mmToPixel),
                     posY + painter.fontMetrics().ascent(), asp.name);
    posY += lineHeight;
  }

  // Footer Seite 2
  painter.setFont(smallFont);
  painter.setPen(Qt::gray);
  painter.drawText(marginX, pageHeight - marginY,
                   QString("Erstellt mit AstroUniverse 2026 am %1 - Seite 2")
                       .arg(QDate::currentDate().toString("dd.MM.yyyy")));

  // ========== SEITE 3: Positionen Person 2 + Aspekte ==========
  printer.newPage();
  // pageNum starts at 3 now
  int pageNum = 3;
  y = marginY;

  // Header Seite 2
  painter.setFont(titleFont);
  painter.setPen(Qt::black);
  painter.drawText(marginX, y + painter.fontMetrics().ascent(),
                   QString("Synastrie: %1 %2  ↔  %3 %4 (Fortsetzung)")
                       .arg(radix1.rFix.vorname, radix1.rFix.name,
                            radix2.rFix.vorname, radix2.rFix.name));
  y += painter.fontMetrics().height() + static_cast<int>(5 * mmToPixelY);

  // Positionen Person 2
  painter.setFont(headerFont);
  painter.setPen(Qt::black);
  painter.drawText(
      marginX, y + painter.fontMetrics().ascent(),
      QString("Positionen %1 %2").arg(radix2.rFix.vorname, radix2.rFix.name));
  y += lineHeight + static_cast<int>(2 * mmToPixelY);

  int numPlanets2 = qMin(static_cast<int>(radix2.anzahlPlanet), 17);
  for (int i = 0; i < numPlanets2 && i < radix2.planet.size(); ++i) {
    // Symbol
    painter.setFont(symbolFont);
    QString symbol = astroFont().planetSymbol(i);
    painter.setPen(adjustColor(getPlanetColor(i, auinit)));
    painter.drawText(marginX, y + painter.fontMetrics().ascent(), symbol);

    // Rückläufig?
    if (i < radix2.planetTyp.size() && (radix2.planetTyp[i] & P_TYP_RUCK)) {
      painter.drawText(marginX + static_cast<int>(4 * mmToPixelX),
                       y + painter.fontMetrics().ascent(),
                       astroFont().retrogradeSymbol());
    }

    // Position (ohne Sternzeichen)
    painter.setFont(normalFont);
    painter.setPen(Qt::black);
    painter.drawText(marginX + static_cast<int>(8 * mmToPixelX),
                     y + painter.fontMetrics().ascent(),
                     formatDegree(radix2.planet[i], false));
    painter.setFont(zodiacFont);
    int zeichen = getZeichen(radix2.planet[i]);
    painter.setPen(adjustColor(getZeichenColor(zeichen)));
    painter.drawText(margin + static_cast<int>(20 * mmToPixel),
                     y + painter.fontMetrics().ascent(),
                     astroFont().sternzeichenSymbol(zeichen));
    y += lineHeight;
  }

  y += static_cast<int>(5 * mmToPixelY);

  // Synastrie-Aspekte
  painter.setFont(headerFont);
  painter.setPen(Qt::black);
  painter.drawText(marginX, y + painter.fontMetrics().ascent(),
                   "Synastrie-Aspekte");
  y += lineHeight + static_cast<int>(2 * mmToPixelY);

  int aspStartY = y;
  // lineHeight 1.2x already set
  int aspColWidth = (pageWidth - 2 * marginX) / 3; // 3 Spalten für mehr Platz
  int aspCol = 0;
  int aspX = marginX;
  int aspY = y;

  for (int i = 0; i < numPlanets1 && i < radix1.planet.size(); ++i) {
    for (int j = 0; j < numPlanets2 && j < radix2.planet.size(); ++j) {
      double pos1 = radix1.planet[i];
      double pos2 = radix2.planet[j];
      double diff = std::abs(pos1 - pos2);
      if (diff > 180)
        diff = 360 - diff;

      int asp = -1;
      double orb = 0;
      if (diff < 8) {
        asp = KONJUNKTION;
        orb = diff;
      } else if (std::abs(diff - 30) < 3) {
        asp = HALBSEX;
        orb = std::abs(diff - 30);
      } else if (std::abs(diff - 60) < 6) {
        asp = SEXTIL;
        orb = std::abs(diff - 60);
      } else if (std::abs(diff - 90) < 8) {
        asp = QUADRATUR;
        orb = std::abs(diff - 90);
      } else if (std::abs(diff - 120) < 8) {
        asp = TRIGON;
        orb = std::abs(diff - 120);
      } else if (std::abs(diff - 150) < 3) {
        asp = QUINCUNX;
        orb = std::abs(diff - 150);
      } else if (std::abs(diff - 180) < 8) {
        asp = OPOSITION;
        orb = std::abs(diff - 180);
      }

      if (asp < 0)
        continue;

      // Sternzeichen der Planeten
      int zeichen1 = getZeichen(pos1);
      int zeichen2 = getZeichen(pos2);

      // Symbole: Planet1 Zeichen1 Aspekt Planet2 Zeichen2 (Orb)
      QString sym1 = astroFont().planetSymbol(i);
      QString aspSym = astroFont().aspektSymbol(asp);
      QString sym2 = astroFont().planetSymbol(j);
      QString sign1 = astroFont().sternzeichenSymbol(zeichen1);
      QString sign2 = astroFont().sternzeichenSymbol(zeichen2);

      // Planet1
      painter.setFont(symbolFont);
      painter.setPen(adjustColor(getPlanetColor(i, auinit)));
      painter.drawText(aspX, aspY + painter.fontMetrics().ascent(), sym1);

      // Zeichen1
      painter.setFont(zodiacFont);
      painter.setPen(adjustColor(getZeichenColor(zeichen1)));
      painter.drawText(aspX + static_cast<int>(6 * mmToPixelX),
                       aspY + painter.fontMetrics().ascent(), sign1);

      // Aspekt
      painter.setFont(symbolFont);
      painter.setPen(adjustColor(getAspektColor(asp)));
      painter.drawText(aspX + static_cast<int>(12 * mmToPixelX),
                       aspY + painter.fontMetrics().ascent(), aspSym);

      // Planet2
      painter.setPen(adjustColor(getPlanetColor(j, auinit)));
      painter.drawText(aspX + static_cast<int>(18 * mmToPixelX),
                       aspY + painter.fontMetrics().ascent(), sym2);

      // Zeichen2
      painter.setFont(zodiacFont);
      painter.setPen(adjustColor(getZeichenColor(zeichen2)));
      painter.drawText(aspX + static_cast<int>(24 * mmToPixelX),
                       aspY + painter.fontMetrics().ascent(), sign2);

      // Orb
      painter.setFont(smallFont);
      painter.setPen(Qt::black);
      painter.drawText(aspX + static_cast<int>(30 * mmToPixelX),
                       aspY + painter.fontMetrics().ascent(),
                       QString("%1°").arg(orb, 0, 'f', 1));

      aspY += lineHeight;

      // Neue Spalte wenn zu weit unten (Synastrie)
      if (aspY > pageHeight - marginY - lineHeight * 2) {
        aspCol++;
        if (aspCol >= 3) { // 3 Spalten wegen mehr Platz für Zeichen
          // Neue Seite
          painter.setFont(smallFont);
          painter.setPen(Qt::gray);
          painter.drawText(marginX, pageHeight - marginY,
                           QString("Erstellt mit AstroUniverse 2026 - Seite %1")
                               .arg(pageNum));

          printer.newPage();
          pageNum++;
          aspCol = 0;
          y = marginY;

          painter.setFont(titleFont);
          painter.setPen(Qt::black);
          painter.drawText(marginX, y + painter.fontMetrics().ascent(),
                           QString("Synastrie-Aspekte (Fortsetzung)"));
          y +=
              painter.fontMetrics().height() + static_cast<int>(5 * mmToPixelY);
          aspStartY = y;
        }
        aspX = marginX + aspCol * aspColWidth;
        aspY = aspStartY;
      }
    }
  }

  // Footer letzte Seite
  painter.setFont(smallFont);
  painter.setPen(Qt::gray);
  painter.drawText(marginX, pageHeight - marginY,
                   QString("Erstellt mit AstroUniverse 2026 am %1 - Seite %2")
                       .arg(QDate::currentDate().toString("dd.MM.yyyy"))
                       .arg(pageNum));
}

void PdfExporter::renderTransitPage(QPainter &painter, QPrinter &printer,
                                    const Radix &radix, const Radix &transit,
                                    const AuInit &auinit,
                                    ChartWidget *chartWidget) {
  QRect pageRect = printer.pageRect(QPrinter::DevicePixel).toRect();
  int pageWidth = pageRect.width();
  int pageHeight = pageRect.height();

  double dpiX = printer.logicalDpiX();
  double dpiY = printer.logicalDpiY();
  double mmToPixelX = dpiX / 25.4;
  double mmToPixelY = dpiY / 25.4;

  painter.setRenderHint(QPainter::Antialiasing);
  painter.setRenderHint(QPainter::TextAntialiasing);

  QFont titleFont("DejaVu Sans", 14);
  titleFont.setBold(true);
  QFont headerFont("DejaVu Sans", 10);
  headerFont.setBold(true);
  QFont normalFont("DejaVu Sans", 9);
  QFont smallFont("DejaVu Sans", 7);
  QFont symbolFont = astroFont().getPlanetSymbolFont(10);
  QFont zodiacFont = astroFont().getSymbolFont(10);

  int marginX = static_cast<int>(10 * mmToPixelX);
  int marginY = static_cast<int>(10 * mmToPixelY);
  int y = marginY;

  // Helper: Farbe anpassen (weiß -> schwarz)
  auto adjustColor = [](QColor c) -> QColor {
    if (c.lightness() > 200)
      return Qt::black;
    return c;
  };

  // Helper: Datum formatieren
  auto formatDate = [](const RadixFix &r) {
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

  // ========== SEITE 1: Person-Daten, Grafik, Positionen Person ==========

  // Header
  painter.setFont(titleFont);
  painter.setPen(Qt::black);
  QString title =
      QString("Transit für %1 %2").arg(radix.rFix.vorname, radix.rFix.name);
  painter.drawText(marginX, y + painter.fontMetrics().ascent(), title);
  y += painter.fontMetrics().height() + static_cast<int>(2 * mmToPixelY);

  // Geburtsdaten Person
  painter.setFont(normalFont);
  painter.drawText(marginX, y + painter.fontMetrics().ascent(),
                   QString("Geburt: %1").arg(formatDate(radix.rFix)));
  y += painter.fontMetrics().height();

  // Transit-Datum
  painter.drawText(marginX, y + painter.fontMetrics().ascent(),
                   QString("Transit: %1").arg(formatDate(transit.rFix)));
  y += painter.fontMetrics().height() + static_cast<int>(5 * mmToPixelY);

  // Transit-Grafik (quadratisch!)
  if (chartWidget) {
    int chartWidth = static_cast<int>((pageWidth - 2 * marginX) * 0.95);
    // User change in renderPage:
    // static_cast<int>((pageWidth * 0.95));
    // Replicating exactly what user did:
    chartWidth = static_cast<int>((pageWidth * 0.95));

    int chartHeight = chartWidth;

    if (chartHeight > (pageHeight - y - marginY)) {
      chartHeight = pageHeight - y - marginY;
      chartWidth = chartHeight;
    }

    int chartX = (pageWidth - chartWidth) / 2;

    // Bild ist bereits quadratisch (1200x1200) aus renderForPrint
    QImage chartImage = chartWidget->renderForPrint(1200);

    // Vertikal zentrieren (User: / 3)
    int availableHeight = pageHeight - y - marginY;
    int chartY = y + (availableHeight - chartHeight) / 3;

    // Zeichne in ein Ziel-Rechteck mit korrekten physischen Dimensionen
    QRect targetRect(chartX, chartY, chartWidth, chartHeight);
    painter.drawImage(targetRect, chartImage);

    // y += chartHeight + static_cast<int>(3 * mmToPixelY);
  }

  // Footer Seite 1
  painter.setFont(smallFont);
  painter.setPen(Qt::gray);
  painter.drawText(marginX, pageHeight - marginY,
                   QString("Erstellt mit AstroUniverse 2026 am %1 - Seite 1")
                       .arg(QDate::currentDate().toString("dd.MM.yyyy")));

  // ========== SEITE 2: Radix-Positionen + Häuser + Legende ==========
  printer.newPage();
  y = marginY;

  // Header Seite 2
  painter.setFont(titleFont);
  painter.setPen(Qt::black);
  painter.drawText(marginX, y + painter.fontMetrics().ascent(),
                   QString("Daten (Fortsetzung)"));
  y += painter.fontMetrics().height() + static_cast<int>(5 * mmToPixelY);

  // Positionen Person + Häuser + Legende (3 Spalten)
  int colWidth = (pageWidth - 4 * marginX) / 3;
  int col1X = marginX;
  int col2X = marginX + colWidth + marginX;
  int col3X = marginX + 2 * (colWidth + marginX);
  int tableY = y;

  // Line Height 5.5mm (User Request)
  int lineHeight = static_cast<int>(5.5 * mmToPixelY);

  painter.setFont(headerFont);
  painter.setPen(Qt::black);
  painter.drawText(col1X, tableY + painter.fontMetrics().ascent(),
                   QString("Radix-Positionen %1").arg(radix.rFix.vorname));
  painter.drawText(col2X, tableY + painter.fontMetrics().ascent(), "Häuser");
  painter.drawText(col3X, tableY + painter.fontMetrics().ascent(), "Legende");
  tableY += lineHeight + static_cast<int>(2 * mmToPixelY);

  // int lineHeight = static_cast<int>(4 * mmToPixelY); // OLD
  int numPlanetsRadix = qMin(static_cast<int>(radix.anzahlPlanet), 17);
  int posY = tableY;

  for (int i = 0; i < numPlanetsRadix && i < radix.planet.size(); ++i) {
    // Symbol
    painter.setFont(symbolFont);
    QString symbol = astroFont().planetSymbol(i);
    painter.setPen(adjustColor(getPlanetColor(i, auinit)));
    painter.drawText(col1X, posY + painter.fontMetrics().ascent(), symbol);

    // Rückläufig?
    if (i < radix.planetTyp.size() && (radix.planetTyp[i] & P_TYP_RUCK)) {
      painter.drawText(col1X + static_cast<int>(4 * mmToPixelX),
                       posY + painter.fontMetrics().ascent(),
                       astroFont().retrogradeSymbol());
    }

    // Position (ohne Sternzeichen)
    painter.setFont(normalFont);
    painter.setPen(Qt::black);
    painter.drawText(col1X + static_cast<int>(8 * mmToPixelX),
                     posY + painter.fontMetrics().ascent(),
                     formatDegree(radix.planet[i], false));
    // Sternzeichen separat mit Element-Farbe
    painter.setFont(zodiacFont);
    int zeichen = getZeichen(radix.planet[i]);
    painter.setPen(adjustColor(getZeichenColor(zeichen)));
    painter.drawText(col1X + static_cast<int>(20 * mmToPixelX),
                     posY + painter.fontMetrics().ascent(),
                     astroFont().sternzeichenSymbol(zeichen));

    // Legende: Symbol = Name
    painter.setFont(symbolFont);
    painter.setPen(adjustColor(getPlanetColor(i, auinit)));
    painter.drawText(col3X, posY + painter.fontMetrics().ascent(), symbol);
    painter.setFont(smallFont);
    painter.setPen(Qt::black);
    painter.drawText(col3X + static_cast<int>(5 * mmToPixelX),
                     posY + painter.fontMetrics().ascent(),
                     (i < 17) ? planetNames[i] : QString("Planet %1").arg(i));

    posY += lineHeight;
  }

  int hausY = tableY;
  for (int i = 0; i < MAX_HAUS && i < radix.haus.size(); ++i) {
    painter.setFont(normalFont);
    painter.setPen(Qt::black);
    painter.drawText(col2X, hausY + painter.fontMetrics().ascent(),
                     hausNames[i]);
    painter.drawText(col2X + static_cast<int>(8 * mmToPixelX),
                     hausY + painter.fontMetrics().ascent(),
                     formatDegree(radix.haus[i], false));
    // Sternzeichen separat mit Element-Farbe
    painter.setFont(zodiacFont);
    int zeichen = getZeichen(radix.haus[i]);
    painter.setPen(adjustColor(getZeichenColor(zeichen)));
    painter.drawText(col2X + static_cast<int>(20 * mmToPixelX),
                     hausY + painter.fontMetrics().ascent(),
                     astroFont().sternzeichenSymbol(zeichen));
    hausY += lineHeight;
  }

  // Sternzeichen-Legende (nach Planeten)
  posY += static_cast<int>(2 * mmToPixelY);
  painter.setFont(headerFont);
  painter.setPen(Qt::black);
  painter.drawText(col3X, posY + painter.fontMetrics().ascent(), "Zeichen");
  posY += lineHeight + static_cast<int>(1 * mmToPixelY);

  for (int i = 0; i < 12; ++i) {
    painter.setFont(zodiacFont);
    painter.setPen(adjustColor(getZeichenColor(i)));
    painter.drawText(col3X, posY + painter.fontMetrics().ascent(),
                     astroFont().sternzeichenSymbol(i));
    painter.setFont(smallFont);
    painter.drawText(col3X + static_cast<int>(5 * mmToPixelX),
                     posY + painter.fontMetrics().ascent(), zeichenNames[i]);
    posY += lineHeight;
  }

  // --- ASPEKTE LEGENDE ---
  posY += static_cast<int>(2 * mmToPixelY);
  painter.setFont(headerFont);
  painter.setPen(Qt::black);
  painter.drawText(col3X, posY + painter.fontMetrics().ascent(), "Aspekte");
  posY += lineHeight + static_cast<int>(1 * mmToPixelY);

  for (const auto &asp : aspekte) {
    painter.setFont(symbolFont);
    painter.setPen(adjustColor(getAspektColor(asp.id)));
    painter.drawText(col3X, posY + painter.fontMetrics().ascent(),
                     astroFont().aspektSymbol(asp.id));

    painter.setFont(smallFont);
    painter.setPen(Qt::black);
    painter.drawText(col3X + static_cast<int>(5 * mmToPixelX),
                     posY + painter.fontMetrics().ascent(), asp.name);
    posY += lineHeight;
  }

  // Footer Seite 2
  painter.setFont(smallFont);
  painter.setPen(Qt::gray);
  painter.drawText(marginX, pageHeight - marginY,
                   QString("Erstellt mit AstroUniverse 2026 am %1 - Seite 2")
                       .arg(QDate::currentDate().toString("dd.MM.yyyy")));

  // ========== SEITE 3: Transit-Positionen + Aspekte ==========
  printer.newPage();
  // pageNum starts at 3 now
  int pageNum = 3;
  y = marginY;

  // Header Seite 3 (Fortsetzung)
  painter.setFont(titleFont);
  painter.setPen(Qt::black);
  painter.drawText(marginX, y + painter.fontMetrics().ascent(),
                   QString("Transit für %1 %2 (Fortsetzung)")
                       .arg(radix.rFix.vorname, radix.rFix.name));
  y += painter.fontMetrics().height() + static_cast<int>(5 * mmToPixelY);

  // Transit-Positionen
  painter.setFont(headerFont);
  painter.setPen(Qt::black);
  painter.drawText(marginX, y + painter.fontMetrics().ascent(),
                   QString("Transit-Positionen (%1.%2.%3)")
                       .arg(transit.rFix.tag, 2, 10, QChar('0'))
                       .arg(transit.rFix.monat, 2, 10, QChar('0'))
                       .arg(transit.rFix.jahr));
  y += lineHeight + static_cast<int>(2 * mmToPixelY);

  int numPlanetsTransit = qMin(static_cast<int>(transit.anzahlPlanet), 17);
  for (int i = 0; i < numPlanetsTransit && i < transit.planet.size(); ++i) {
    // Symbol mit T- Prefix
    painter.setFont(normalFont);
    painter.setPen(Qt::black);
    painter.drawText(marginX, y + painter.fontMetrics().ascent(), "T-");

    painter.setFont(symbolFont);
    QString symbol = astroFont().planetSymbol(i);
    painter.setPen(adjustColor(getPlanetColor(i, auinit)));
    painter.drawText(marginX + static_cast<int>(4 * mmToPixelX),
                     y + painter.fontMetrics().ascent(), symbol);

    // Rückläufig?
    if (i < transit.planetTyp.size() && (transit.planetTyp[i] & P_TYP_RUCK)) {
      painter.drawText(marginX + static_cast<int>(8 * mmToPixelX),
                       y + painter.fontMetrics().ascent(),
                       astroFont().retrogradeSymbol());
    }

    // Position (ohne Sternzeichen)
    painter.setFont(normalFont);
    painter.setPen(Qt::black);
    painter.drawText(marginX + static_cast<int>(12 * mmToPixelX),
                     y + painter.fontMetrics().ascent(),
                     formatDegree(transit.planet[i], false));
    // Sternzeichen separat mit Element-Farbe
    painter.setFont(zodiacFont);
    int zeichen = getZeichen(transit.planet[i]);
    painter.setPen(adjustColor(getZeichenColor(zeichen)));
    painter.drawText(marginX + static_cast<int>(24 * mmToPixelX),
                     y + painter.fontMetrics().ascent(),
                     astroFont().sternzeichenSymbol(zeichen));
    y += lineHeight;
  }

  y += static_cast<int>(5 * mmToPixelY);

  // Transit-Aspekte
  // Aspekte (auf Seite 3)
  // Aspekte (auf Seite 3)
  painter.setFont(headerFont);
  painter.setPen(Qt::black);
  painter.drawText(marginX, y + painter.fontMetrics().ascent(),
                   "Aspekte (T=Transit, R=Radix)");
  y += lineHeight + static_cast<int>(2 * mmToPixelY);

  int aspStartY = y;
  // lineHeight already set to 1.2x
  int aspColWidth = (pageWidth - 2 * marginX) / 3; // 3 Spalten für mehr Platz
  int aspCol = 0;
  int aspX = marginX;
  int aspY = y;

  // Transit-Aspekte: Transit-Planeten zu Radix-Planeten
  for (int i = 0; i < numPlanetsTransit && i < transit.planet.size(); ++i) {
    for (int j = 0; j < numPlanetsRadix && j < radix.planet.size(); ++j) {
      double pos1 = transit.planet[i];
      double pos2 = radix.planet[j];
      double diff = std::abs(pos1 - pos2);
      if (diff > 180)
        diff = 360 - diff;

      int asp = -1;
      double orb = 0;
      if (diff < 8) {
        asp = KONJUNKTION;
        orb = diff;
      } else if (std::abs(diff - 30) < 3) {
        asp = HALBSEX;
        orb = std::abs(diff - 30);
      } else if (std::abs(diff - 60) < 6) {
        asp = SEXTIL;
        orb = std::abs(diff - 60);
      } else if (std::abs(diff - 90) < 8) {
        asp = QUADRATUR;
        orb = std::abs(diff - 90);
      } else if (std::abs(diff - 120) < 8) {
        asp = TRIGON;
        orb = std::abs(diff - 120);
      } else if (std::abs(diff - 150) < 3) {
        asp = QUINCUNX;
        orb = std::abs(diff - 150);
      } else if (std::abs(diff - 180) < 8) {
        asp = OPOSITION;
        orb = std::abs(diff - 180);
      }

      if (asp < 0)
        continue;

      // Sternzeichen der Planeten
      int zeichen1 = getZeichen(pos1);
      int zeichen2 = getZeichen(pos2);

      // Symbole: T-Planet Zeichen1 Aspekt R-Planet Zeichen2 (Orb)
      QString sym1 = astroFont().planetSymbol(i);
      QString aspSym = astroFont().aspektSymbol(asp);
      QString sym2 = astroFont().planetSymbol(j);
      QString sign1 = astroFont().sternzeichenSymbol(zeichen1);
      QString sign2 = astroFont().sternzeichenSymbol(zeichen2);

      // T- prefix
      painter.setFont(smallFont);
      painter.setPen(Qt::black);
      painter.drawText(aspX, aspY + painter.fontMetrics().ascent(), "T");

      // Planet1
      painter.setFont(symbolFont);
      painter.setPen(adjustColor(getPlanetColor(i, auinit)));
      painter.drawText(aspX + static_cast<int>(2 * mmToPixelX),
                       aspY + painter.fontMetrics().ascent(), sym1);

      // Zeichen1
      painter.setFont(zodiacFont);
      painter.setPen(adjustColor(getZeichenColor(zeichen1)));
      painter.drawText(aspX + static_cast<int>(7 * mmToPixelX),
                       aspY + painter.fontMetrics().ascent(), sign1);

      // Aspekt
      painter.setFont(symbolFont);
      painter.setPen(adjustColor(getAspektColor(asp)));
      painter.drawText(aspX + static_cast<int>(13 * mmToPixelX),
                       aspY + painter.fontMetrics().ascent(), aspSym);

      // R- prefix
      painter.setFont(smallFont);
      painter.setPen(Qt::black);
      painter.drawText(aspX + static_cast<int>(19 * mmToPixelX),
                       aspY + painter.fontMetrics().ascent(), "R");

      // Planet2
      painter.setFont(symbolFont);
      painter.setPen(adjustColor(getPlanetColor(j, auinit)));
      painter.drawText(aspX + static_cast<int>(21 * mmToPixelX),
                       aspY + painter.fontMetrics().ascent(), sym2);

      // Zeichen2
      painter.setFont(zodiacFont);
      painter.setPen(adjustColor(getZeichenColor(zeichen2)));
      painter.drawText(aspX + static_cast<int>(27 * mmToPixelX),
                       aspY + painter.fontMetrics().ascent(), sign2);

      // Orb
      painter.setFont(smallFont);
      painter.setPen(Qt::black);
      painter.drawText(aspX + static_cast<int>(33 * mmToPixelX),
                       aspY + painter.fontMetrics().ascent(),
                       QString("%1°").arg(orb, 0, 'f', 1));

      aspY += lineHeight;

      // Neue Spalte wenn zu weit unten
      if (aspY > pageHeight - marginY - lineHeight * 2) {
        aspCol++;
        if (aspCol >= 3) {
          // Neue Seite
          painter.setFont(smallFont);
          painter.setPen(Qt::gray);
          painter.drawText(marginX, pageHeight - marginY,
                           QString("Erstellt mit AstroUniverse 2026 - Seite %1")
                               .arg(pageNum));

          printer.newPage();
          pageNum++;
          aspCol = 0;
          y = marginY;

          // Header auf neuer Seite
          painter.setFont(titleFont);
          painter.setPen(Qt::black);
          painter.drawText(marginX, y + painter.fontMetrics().ascent(),
                           QString("Transit-Aspekte (Fortsetzung)"));
          y +=
              painter.fontMetrics().height() + static_cast<int>(5 * mmToPixelY);
          aspStartY = y;
        }
        aspX = marginX + aspCol * aspColWidth;
        aspY = aspStartY;
      }
    }
  }

  // Footer letzte Seite
  painter.setFont(smallFont);
  painter.setPen(Qt::gray);
  painter.drawText(marginX, pageHeight - marginY,
                   QString("Erstellt mit AstroUniverse 2026 am %1 - Seite %2")
                       .arg(QDate::currentDate().toString("dd.MM.yyyy"))
                       .arg(pageNum));
}

} // namespace astro
