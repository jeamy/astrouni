/**
 * @file chart_widget.cpp
 * @brief Implementierung der Radix-Zeichnung
 * 
 * 1:1 Port der Zeichenfunktionen aus legacy/auwurzel.c
 */

#include "chart_widget.h"
#include "../core/calculations.h"
#include <QPainterPath>
#include <QMouseEvent>
#include <QImage>
#include <cmath>
#include <limits>

namespace astro {

//==============================================================================
// Konstruktor / Destruktor
//==============================================================================

ChartWidget::ChartWidget(QWidget* parent)
    : QWidget(parent)
    , m_transit(nullptr)
    , m_radius(0)
    , m_rotation(0)
    , m_showAspects(true)
    , m_show3Degree(true)
    , m_show9Degree(true)
    , m_highlightPlanet(-1)
    , m_highlightAspect1(-1)
    , m_highlightAspect2(-1)
    , m_showSynastrieAspects(false)
    , m_highlightTransitPlanet(-1)
    , m_highlightRadixPlanet(-1) {
    
    // Hintergrund nicht automatisch füllen
    // Der eigentliche Radix-Kreis wird in drawRadix() grau hinterlegt
    setAutoFillBackground(false);
    
    // Fonts - serifenlose Systemfonts für Plattformunabhängigkeit
    m_mainFont = QFont(QFont().defaultFamily(), 10);  // System-Standardfont
    // Symbol-Fonts werden direkt in den Zeichenfunktionen mit Fallback-Kette gesetzt
    
    // Minimum-Größe
    setMinimumSize(400, 400);
}

ChartWidget::~ChartWidget() {
}

//==============================================================================
// Setter
//==============================================================================

void ChartWidget::setRadix(const Radix& radix) {
    m_radix = radix;
    
    // STRICT LEGACY: ASC immer links/waagrecht!
    // m_rotation = -ASC, damit in degreeToPoint der ASC bei 0° relativ liegt
    // und dann auf 180° (links) im Bildschirm-Koordinatensystem gemappt wird
    m_rotation = -m_radix.asc;
    
    update();
}

void ChartWidget::setTransitRadix(const Radix* transit) {
    m_transit = transit;
    update();
}

void ChartWidget::setSettings(const AuInit& auinit) {
    m_auinit = auinit;
    
    // Teilung (3er/9er Grad-Markierungen)
    // Wenn Flag gesetzt ist, wird die Teilung angezeigt
    m_show3Degree = (m_auinit.sTeilung & S_3er) != 0;
    m_show9Degree = (m_auinit.sTeilung & S_9er) != 0;
    
    update();
}

void ChartWidget::updateChart() {
    calculateRadii();
    update();
}

void ChartWidget::highlightPlanet(int planetIndex) {
    m_highlightPlanet = planetIndex;
    m_highlightAspect1 = -1;
    m_highlightAspect2 = -1;
    update();
}

void ChartWidget::highlightAspect(int planet1, int planet2) {
    m_highlightPlanet = -1;
    m_highlightAspect1 = planet1;
    m_highlightAspect2 = planet2;
    m_highlightTransitPlanet = -1;
    m_highlightRadixPlanet = -1;
    update();
}

void ChartWidget::highlightTransitAspect(int transitPlanet, int radixPlanet) {
    m_highlightPlanet = -1;
    m_highlightAspect1 = -1;
    m_highlightAspect2 = -1;
    m_highlightTransitPlanet = transitPlanet;
    m_highlightRadixPlanet = radixPlanet;
    update();
}

void ChartWidget::setShowSynastrieAspects(bool showSynastrieAspects) {
    if (m_showSynastrieAspects != showSynastrieAspects) {
        m_showSynastrieAspects = showSynastrieAspects;
        update();
    }
}

//==============================================================================
// Größen
//==============================================================================

QSize ChartWidget::minimumSizeHint() const {
    return QSize(400, 400);
}

QSize ChartWidget::sizeHint() const {
    return QSize(600, 600);
}

//==============================================================================
// Events
//==============================================================================

void ChartWidget::paintEvent(QPaintEvent* /*event*/) {
    // Offscreen zeichnen (wie Legacy: Basis-Buffer, später Zoom-Ausschnitt)
    QImage buffer(size(), QImage::Format_ARGB32_Premultiplied);
    buffer.fill(Qt::transparent);
    QPainter buf(&buffer);
    buf.setRenderHint(QPainter::Antialiasing);
    
    // Radien berechnen (bezogen auf Widget-Größe)
    calculateRadii();
    
    // Zeichnen
    drawRadix(buf);
    drawZodiacSigns(buf);
    drawDegreeMarks(buf);
    drawHouses(buf);
    drawPlanets(buf);
    
    if (m_showAspects) {
        drawAspects(buf);
    }
    
    // Zusätzlicher Aspekt-Kreis wenn aktiviert (MAUS_ASPEKT)
    if (m_aspectCircleActive) {
        // Legacy nutzte keinen separaten Aspekt-Kreis-Farbindex; wir verwenden den Radix-Farbton
        buf.setPen(QPen(sColor[COL_RADIX], 2, Qt::DashLine));
        buf.setBrush(Qt::NoBrush);
        buf.drawEllipse(m_center, m_radiusAsp, m_radiusAsp);
    }
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    if (m_zoomActive && m_zoomFactor > 1.0) {
        painter.translate(m_zoomCenter);
        painter.scale(m_zoomFactor, m_zoomFactor);
        painter.translate(-m_zoomCenter);
    }
    painter.drawImage(QPoint(0, 0), buffer);
}

void ChartWidget::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::RightButton) {
        // Rechtsklick: wenn Planet/Haus/Aspekt getroffen -> Signal, sonst Zoom-Toggle
        int planetIdx = -1;
        bool isTransit = false;
        if (findPlanetAtPoint(event->pos(), planetIdx, isTransit)) {
            emit planetClicked(planetIdx, isTransit);
            return;
        }
        int hausIdx = findHouseAtPoint(event->pos());
        if (hausIdx >= 0) {
            emit houseClicked(hausIdx);
            return;
        }
        int a1 = -1, a2 = -1;
        bool aspTransit = false;
        if (findAspectAtPoint(event->pos(), a1, a2, aspTransit)) {
            emit aspectClicked(a1, a2, aspTransit);
            return;
        }
        toggleZoom(event->pos());
        return;
    }
    
    if (event->button() == Qt::LeftButton) {
        // Klick auf Planet?
        int planetIdx = -1;
        bool isTransit = false;
        if (findPlanetAtPoint(event->pos(), planetIdx, isTransit)) {
            emit planetClicked(planetIdx, isTransit);
            return;
        }
        
        // Klick auf Haus?
        int hausIdx = findHouseAtPoint(event->pos());
        if (hausIdx >= 0) {
            emit houseClicked(hausIdx);
            return;
        }
        
        // Klick auf Aspekt-Linie?
        int a1 = -1, a2 = -1;
        bool aspTransit = false;
        if (findAspectAtPoint(event->pos(), a1, a2, aspTransit)) {
            emit aspectClicked(a1, a2, aspTransit);
            return;
        }
        
        // Sonst Aspekt-Kreis toggeln (MAUS_ASPEKT)
        toggleAspectCircle();
        return;
    }
    
    QWidget::mousePressEvent(event);
}

void ChartWidget::mouseReleaseEvent(QMouseEvent* event) {
    QWidget::mouseReleaseEvent(event);
}

void ChartWidget::mouseMoveEvent(QMouseEvent* event) {
    QWidget::mouseMoveEvent(event);
}

void ChartWidget::mouseDoubleClickEvent(QMouseEvent* event) {
    // Doppelklick: Aspekt-Kreis toggeln
    Q_UNUSED(event);
    toggleAspectCircle();
}

void ChartWidget::resizeEvent(QResizeEvent* event) {
    calculateRadii();
    QWidget::resizeEvent(event);
}

//==============================================================================
// Maus-/Interaktions-Helper
//==============================================================================

double ChartWidget::pointToDegree(const QPointF& p) const {
    // Umkehrung von degreeToPoint: 0° = links, CCW positiv
    double dx = p.x() - m_center.x();
    double dy = m_center.y() - p.y();  // invertiertes Y
    double rad = std::atan2(dy, dx);   // 0 rechts, CCW
    double degScreen = rad * 180.0 / PI;
    double rotated = degScreen - 180.0;      // inverse zu (180 + rotatedDegree)
    double degree = rotated - m_rotation;    // inverse zur Rotation
    return Calculations::mod360(degree);
}

bool ChartWidget::findPlanetAtPoint(const QPointF& p, int& planetIdx, bool& isTransit) const {
    planetIdx = -1;
    isTransit = false;
    double bestDist = std::numeric_limits<double>::max();
    
    // Kollisions-Versatz wie in drawPlanets (Radix)
    QVector<int> offsetLevel(m_radix.anzahlPlanet, 0);
    for (int i = 0; i < m_radix.anzahlPlanet; ++i) {
        for (int j = 0; j < i; ++j) {
            double diff = std::fabs(m_radix.planet[i] - m_radix.planet[j]);
            if (diff > 180.0) diff = 360.0 - diff;
            if (diff < 6.0) {
                offsetLevel[i] = std::max(offsetLevel[i], offsetLevel[j] + 1);
            }
        }
    }
    
    for (int i = 0; i < m_radix.anzahlPlanet; ++i) {
        double offset = offsetLevel[i] * 25.0;
        double symbolRadius = m_radiusPlanet - 30 - offset;
        QPointF pos = degreeToPoint(m_radix.planet[i], symbolRadius);
        double dist = std::hypot(p.x() - pos.x(), p.y() - pos.y());
        if (dist <= 16.0 && dist < bestDist) {  // Nähe zum Symbol
            bestDist = dist;
            planetIdx = i;
            isTransit = false;
        }
    }
    
    // Transit-/Synastrie-Planeten (wenn vorhanden)
    if (m_transit != nullptr) {
        QVector<int> transitOffsetLevel(m_transit->anzahlPlanet, 0);
        for (int i = 0; i < m_transit->anzahlPlanet; ++i) {
            for (int j = 0; j < i; ++j) {
                double diff = std::fabs(m_transit->planet[i] - m_transit->planet[j]);
                if (diff > 180.0) diff = 360.0 - diff;
                if (diff < 6.0) {
                    transitOffsetLevel[i] = std::max(transitOffsetLevel[i], transitOffsetLevel[j] + 1);
                }
            }
        }
        for (int i = 0; i < m_transit->anzahlPlanet; ++i) {
            double offset = transitOffsetLevel[i] * 25.0;
            double symbolRadius = m_radiusPlanet - 30 - offset;
            QPointF pos = degreeToPoint(m_transit->planet[i], symbolRadius);
            double dist = std::hypot(p.x() - pos.x(), p.y() - pos.y());
            if (dist <= 16.0 && dist < bestDist) {
                bestDist = dist;
                planetIdx = i;
                isTransit = true;
            }
        }
    }
    
    return planetIdx >= 0;
}

int ChartWidget::findHouseAtPoint(const QPointF& p) const {
    if (m_radix.haus.isEmpty()) return -1;
    
    double degree = pointToDegree(p);
    double r = std::hypot(p.x() - m_center.x(), p.y() - m_center.y());
    
    // Nur Klicks im Bereich der Häuser-Linien berücksichtigen
    if (r < m_radiusAsp * 0.7 || r > m_radius + 12) {
        return -1;
    }
    
    int bestIdx = -1;
    double bestDiff = 999.0;
    for (int i = 0; i < MAX_HAUS; ++i) {
        double diff = std::fabs(Calculations::minDist(degree, m_radix.haus[i]));
        if (diff < bestDiff) {
            bestDiff = diff;
            bestIdx = i;
        }
    }
    return (bestDiff <= 6.0) ? bestIdx : -1;
}

void ChartWidget::toggleZoom(const QPoint& pos) {
    if (!m_zoomActive) {
        m_zoomActive = true;
        m_zoomCenter = pos;
        m_zoomFactor = 2.0;
    } else {
        m_zoomActive = false;
        m_zoomFactor = 1.0;
    }
    update();
}

void ChartWidget::toggleAspectCircle() {
    m_aspectCircleActive = !m_aspectCircleActive;
    update();
}

bool ChartWidget::findAspectAtPoint(const QPointF& p, int& idx1, int& idx2, bool& isTransit) const {
    idx1 = idx2 = -1;
    isTransit = false;
    double bestDist = std::numeric_limits<double>::max();
    
    auto distToSegment = [](const QPointF& a, const QPointF& b, const QPointF& p) {
        double vx = b.x() - a.x();
        double vy = b.y() - a.y();
        double wx = p.x() - a.x();
        double wy = p.y() - a.y();
        double c1 = vx * wx + vy * wy;
        if (c1 <= 0) return std::hypot(p.x() - a.x(), p.y() - a.y());
        double c2 = vx * vx + vy * vy;
        if (c2 <= c1) return std::hypot(p.x() - b.x(), p.y() - b.y());
        double t = c1 / c2;
        double projx = a.x() + t * vx;
        double projy = a.y() + t * vy;
        return std::hypot(p.x() - projx, p.y() - projy);
    };
    
    // Radix-Aspekte
    if (!m_showSynastrieAspects || m_transit == nullptr) {
        int numPlanets = m_radix.anzahlPlanet;
        for (int i = 0; i < numPlanets; ++i) {
            for (int j = i + 1; j < numPlanets; ++j) {
                int idx = i * numPlanets + j;
                int8_t asp = m_radix.aspPlanet[idx];
                if (asp == KEIN_ASP) continue;
                QPointF p1 = degreeToPoint(m_radix.planet[i], m_radiusAsp);
                QPointF p2 = degreeToPoint(m_radix.planet[j], m_radiusAsp);
                double d = distToSegment(p1, p2, p);
                if (d < 8.0 && d < bestDist) {
                    bestDist = d;
                    idx1 = i;
                    idx2 = j;
                    isTransit = false;
                }
            }
        }
    } else {
        // Transit/Synastrie-Aspekte: Transit-Planet zu Radix-Planet
        int numTransit = m_transit->anzahlPlanet;
        int numPlanets = m_radix.anzahlPlanet;
        for (int i = 0; i < numTransit; ++i) {
            for (int j = 0; j < numPlanets; ++j) {
                double transitPos = m_transit->planet[i];
                double radixPos = m_radix.planet[j];
                double diff = std::abs(transitPos - radixPos);
                if (diff > 180.0) diff = 360.0 - diff;
                
                int8_t asp = KEIN_ASP;
                if (diff <= 10.0) asp = KONJUNKTION;
                else if (std::abs(diff - 30.0) <= 3.0) asp = HALBSEX;
                else if (std::abs(diff - 60.0) <= 6.0) asp = SEXTIL;
                else if (std::abs(diff - 90.0) <= 8.0) asp = QUADRATUR;
                else if (std::abs(diff - 120.0) <= 8.0) asp = TRIGON;
                else if (std::abs(diff - 150.0) <= 3.0) asp = QUINCUNX;
                else if (std::abs(diff - 180.0) <= 10.0) asp = OPOSITION;
                
                if (asp == KEIN_ASP) continue;
                
                QPointF p1 = degreeToPoint(transitPos, m_radiusAsp);
                QPointF p2 = degreeToPoint(radixPos, m_radiusAsp);
                double d = distToSegment(p1, p2, p);
                if (d < 8.0 && d < bestDist) {
                    bestDist = d;
                    idx1 = i;
                    idx2 = j;
                    isTransit = true;
                }
            }
        }
    }
    return idx1 >= 0 && idx2 >= 0;
}

//==============================================================================
// Radien berechnen
//==============================================================================

void ChartWidget::calculateRadii() {
    // Mittelpunkt
    m_center = QPointF(width() / 2.0, height() / 2.0);
    
    // Basis-Radius (kleinere Dimension minus Rand)
    m_radius = qMin(width(), height()) / 2.0 - 20.0;
    
    // Radien für verschiedene Kreise (aus constants.h)
    if (m_show3Degree && m_show9Degree) {
        // Normal
        m_radiusStz = m_radius * KREIS_STZ;
        m_radius10 = m_radius * KREIS_10_GRAD;
        m_radius3 = m_radius * KREIS_3_GRAD;
        m_radius5 = m_radius * KREIS_5_GRAD;
        m_radiusPlanet = m_radius * KREIS_PLANET;
    } else if (!m_show3Degree && m_show9Degree) {
        // Ohne 3er
        m_radiusStz = m_radius * KREIS3_STZ;
        m_radius10 = m_radius * KREIS3_10_GRAD;
        m_radius3 = m_radius * KREIS3_3_GRAD;
        m_radius5 = m_radius * KREIS3_5_GRAD;
        m_radiusPlanet = m_radius * KREIS3_PLANET;
    } else {
        // Ohne 3er und 9er
        m_radiusStz = m_radius * KREIS9_STZ;
        m_radius10 = m_radius * KREIS9_10_GRAD;
        m_radius3 = m_radius * KREIS9_3_GRAD;
        m_radius5 = m_radius * KREIS9_5_GRAD;
        m_radiusPlanet = m_radius * KREIS9_PLANET;
    }
    
    m_radiusAsp = m_radius * KREIS_ASP;
}

//==============================================================================
// Koordinaten-Konvertierung
//==============================================================================

QPointF ChartWidget::degreeToPoint(double degree, double radius) const {
    // STRICT LEGACY: ASC links, GEGEN den Uhrzeigersinn (mathematisch positiv)
    // Rotation anwenden: degree + m_rotation gibt den Winkel relativ zum ASC
    // m_rotation = -ASC, damit ASC bei 0° relativ liegt
    double rotatedDegree = degree + m_rotation;
    
    // Umrechnung: 0° relativ = links (180° im Bildschirm-Koordinatensystem)
    // GEGEN den Uhrzeigersinn: Winkel im Bildschirm nimmt mit zunehmendem rotatedDegree ZU
    double rad = (180.0 + rotatedDegree) * PI / 180.0;
    
    return QPointF(
        m_center.x() + radius * std::cos(rad),
        m_center.y() - radius * std::sin(rad)
    );
}

//==============================================================================
// Radix-Kreis zeichnen
// Port von: sRadix(HDC, RADIX*)
//==============================================================================

void ChartWidget::drawRadix(QPainter& painter) {
    // Hintergrund-Kreis des Radix (grau hinterlegt wie im Legacy)
    painter.setPen(Qt::NoPen);
    painter.setBrush(sColor[COL_HINTER]);
    painter.drawEllipse(m_center, m_radius, m_radius);
    
    // Äußerer Kreis
    painter.setPen(QPen(sColor[COL_RADIX], 2));
    painter.setBrush(Qt::NoBrush);
    painter.drawEllipse(m_center, m_radius, m_radius);
    
    // Sternzeichen-Kreis
    painter.setPen(QPen(sColor[COL_RADIX], 1));
    painter.drawEllipse(m_center, m_radiusStz, m_radiusStz);
    
    // 10-Grad-Kreis
    painter.drawEllipse(m_center, m_radius10, m_radius10);
    
    // 5-Grad-Kreis (wenn 3er/9er angezeigt)
    if (m_show3Degree || m_show9Degree) {
        painter.drawEllipse(m_center, m_radius5, m_radius5);
    }
    
    // Planeten-Kreis
    painter.drawEllipse(m_center, m_radiusPlanet, m_radiusPlanet);
    
    // Aspekt-Kreis (innerer Kreis)
    painter.drawEllipse(m_center, m_radiusAsp, m_radiusAsp);
}

//==============================================================================
// Sternzeichen zeichnen
//==============================================================================

void ChartWidget::drawZodiacSigns(QPainter& painter) {
    // Sternzeichen-Trennlinien (alle 30°)
    painter.setPen(QPen(sColor[COL_RADIX], 1));
    
    for (int i = 0; i < 12; ++i) {
        double degree = i * 30.0;
        QPointF outer = degreeToPoint(degree, m_radius);
        QPointF inner = degreeToPoint(degree, m_radiusStz);
        painter.drawLine(outer, inner);
    }
    
    // Sternzeichen-Symbole mit Unicode
    // Plattformübergreifende Symbol-Font Fallback-Kette
    QFont zodiacFont;
    QStringList symbolFonts = {"Segoe UI Symbol", "Apple Symbols", "Noto Sans Symbols2", "DejaVu Sans"};
    for (const QString& fontName : symbolFonts) {
        zodiacFont = QFont(fontName, 18);
        if (QFontInfo(zodiacFont).family().contains(fontName.left(5), Qt::CaseInsensitive)) {
            break;
        }
    }
    painter.setFont(zodiacFont);
    
    // Element-Farben für Sternzeichen (wie in Legacy)
    // Feuer=0,4,8; Erde=1,5,9; Luft=2,6,10; Wasser=3,7,11
    QColor elementColors[4] = {
        sColor[COL_FEUER],   // Feuer: Widder, Löwe, Schütze
        sColor[COL_ERDE],    // Erde: Stier, Jungfrau, Steinbock
        sColor[COL_LUFT],    // Luft: Zwillinge, Waage, Wassermann
        sColor[COL_WASSER]   // Wasser: Krebs, Skorpion, Fische
    };
    
    for (int i = 0; i < 12; ++i) {
        double degree = i * 30.0 + 15.0;  // Mitte des Zeichens
        double symbolRadius = (m_radius + m_radiusStz) / 2.0;
        QPointF pos = degreeToPoint(degree, symbolRadius);
        
        // Element-Farbe
        int element = i % 4;
        painter.setPen(elementColors[element]);
        
        // Unicode-Symbol zentriert zeichnen
        QString symbol = QString::fromUtf8(STERNZEICHEN_SYMBOLS[i]);
        QRectF rect(pos.x() - 12, pos.y() - 12, 24, 24);
        painter.drawText(rect, Qt::AlignCenter, symbol);
    }
}

//==============================================================================
// Grad-Markierungen zeichnen
//==============================================================================

void ChartWidget::drawDegreeMarks(QPainter& painter) {
    painter.setPen(QPen(sColor[COL_RADIX], 1));
    
    for (int i = 0; i < 360; ++i) {
        double degree = static_cast<double>(i);
        double innerRadius, outerRadius;
        
        if (i % 10 == 0) {
            // 10-Grad-Markierung
            innerRadius = m_radius10;
            outerRadius = m_radiusStz;
        } else if (i % 5 == 0 && (m_show3Degree || m_show9Degree)) {
            // 5-Grad-Markierung
            innerRadius = m_radius5;
            outerRadius = m_radius10;
        } else if (i % 3 == 0 && m_show3Degree) {
            // 3-Grad-Markierung
            innerRadius = m_radius3;
            outerRadius = m_radius10;
        } else if (i % 1 == 0 && m_show9Degree) {
            // 1-Grad-Markierung (nur bei 9er)
            innerRadius = m_radius5 + (m_radius10 - m_radius5) * 0.3;
            outerRadius = m_radius10;
        } else {
            continue;
        }
        
        QPointF outer = degreeToPoint(degree, outerRadius);
        QPointF inner = degreeToPoint(degree, innerRadius);
        painter.drawLine(outer, inner);
    }
}

//==============================================================================
// Häuser zeichnen
// Port von: sHaus(HDC, RADIX*, char)
//==============================================================================

void ChartWidget::drawHouses(QPainter& painter) {
    if (m_radix.haus.isEmpty()) return;
    
    // Häuser-Linien
    for (int i = 0; i < MAX_HAUS; ++i) {
        double degree = m_radix.haus[i];
        
        // STRICT LEGACY: ASC(1), IC(4), DSC(7), MC(10) bis zum äußersten Kreis
        // Andere Häuser nur bis zum Planeten-Kreis
        bool isMainAxis = (i == 0 || i == 3 || i == 6 || i == 9);
        
        if (isMainAxis) {
            // Hauptachsen: dick, bis zum äußersten Kreis
            painter.setPen(QPen(sColor[COL_ASZ], 2));
            QPointF outer = degreeToPoint(degree, m_radius);  // Bis zum äußersten Kreis!
            QPointF inner = degreeToPoint(degree, m_radiusAsp);
            painter.drawLine(outer, inner);
        } else {
            // Normale Häuser: dünn, nur bis Planeten-Kreis
            painter.setPen(QPen(sColor[COL_HAUSER], 1));
            QPointF outer = degreeToPoint(degree, m_radiusPlanet);
            QPointF inner = degreeToPoint(degree, m_radiusAsp);
            painter.drawLine(outer, inner);
        }
    }
    
    // STRICT LEGACY: Haus-Nummern am inneren Kreis (Aspekt-Kreis)
    // Bei ASC(1), IC(4), DSC(7), MC(10) keine Nummer, sondern Beschriftung
    painter.setFont(m_mainFont);
    painter.setPen(sColor[COL_TXT]);
    
    for (int i = 0; i < MAX_HAUS; ++i) {
        // STRICT LEGACY: Beschriftung näher an der Häuserlinie (nicht in der Mitte)
        // Nur 1/4 des Weges zur nächsten Häuserlinie
        double start = m_radix.haus[i];
        double end = m_radix.haus[(i + 1) % MAX_HAUS];
        double offset = Calculations::minDist(start, end) * 0.25;  // 25% statt 50%
        double labelPos = Calculations::mod360(start + offset);
        
        // Radius etwas weiter vom Aspekt-Kreis entfernt
        double numberRadius = m_radiusAsp + 22;
        QPointF pos = degreeToPoint(labelPos, numberRadius);
        
        // STRICT LEGACY: Bei Hauptachsen ASC/IC/DSC/MC statt Nummer
        QString label;
        QRectF rect;
        if (i == 0) {
            label = "ASC";
            painter.setPen(sColor[COL_ASZ]);
            rect = QRectF(pos.x() - 12, pos.y() - 8, 24, 16);
        } else if (i == 3) {
            label = "IC";
            painter.setPen(sColor[COL_ASZ]);
            rect = QRectF(pos.x() - 8, pos.y() - 8, 16, 16);
        } else if (i == 6) {
            label = "DSC";
            painter.setPen(sColor[COL_ASZ]);
            rect = QRectF(pos.x() - 12, pos.y() - 8, 24, 16);
        } else if (i == 9) {
            label = "MC";
            painter.setPen(sColor[COL_ASZ]);
            rect = QRectF(pos.x() - 10, pos.y() - 8, 20, 16);
        } else {
            label = QString::number(i + 1);
            painter.setPen(sColor[COL_TXT]);
            rect = QRectF(pos.x() - 8, pos.y() - 8, 16, 16);
        }
        painter.drawText(rect, Qt::AlignCenter, label);
    }
}

//==============================================================================
// Planeten zeichnen
// Port von: sPlanet(HDC, RADIX*, char)
//==============================================================================

void ChartWidget::drawPlanets(QPainter& painter) {
    if (m_radix.planet.isEmpty()) return;
    
    // STRICT LEGACY: Berechne Versatz-Level für jeden Planeten (Kollisionsvermeidung)
    // Planeten innerhalb von 6° werden nach innen versetzt
    QVector<int> offsetLevel(m_radix.anzahlPlanet, 0);
    
    for (int i = 0; i < m_radix.anzahlPlanet; ++i) {
        for (int j = 0; j < i; ++j) {
            double diff = std::fabs(m_radix.planet[i] - m_radix.planet[j]);
            if (diff > 180.0) diff = 360.0 - diff;
            
            if (diff < 6.0) {
                // Kollision gefunden - höheren Level zuweisen
                offsetLevel[i] = qMax(offsetLevel[i], offsetLevel[j] + 1);
            }
        }
    }
    
    // STRICT LEGACY: Erst ALLE Linien zeichnen, dann ALLE Symbole
    // Damit Symbole immer über den Linien liegen
    
    // 1. Alle Linien (Tics) zeichnen
    for (int i = 0; i < m_radix.anzahlPlanet; ++i) {
        double degree = m_radix.planet[i];
        bool isTransit = (m_radix.planetTyp[i] & P_TYP_TRANSIT) != 0;
        drawPlanetTic(painter, degree, isTransit, offsetLevel[i]);
    }
    
    // Transit-Planeten Linien (falls vorhanden)
    QVector<int> transitOffsetLevel;
    if (m_transit != nullptr) {
        transitOffsetLevel.resize(m_transit->anzahlPlanet, 0);
        
        for (int i = 0; i < m_transit->anzahlPlanet; ++i) {
            for (int j = 0; j < i; ++j) {
                double diff = std::fabs(m_transit->planet[i] - m_transit->planet[j]);
                if (diff > 180.0) diff = 360.0 - diff;
                
                if (diff < 6.0) {
                    transitOffsetLevel[i] = qMax(transitOffsetLevel[i], transitOffsetLevel[j] + 1);
                }
            }
        }
        
        for (int i = 0; i < m_transit->anzahlPlanet; ++i) {
            double degree = m_transit->planet[i];
            drawPlanetTic(painter, degree, true, transitOffsetLevel[i]);
        }
    }
    
    // 2. Alle Symbole zeichnen (über den Linien)
    for (int i = 0; i < m_radix.anzahlPlanet; ++i) {
        double degree = m_radix.planet[i];
        bool isTransit = (m_radix.planetTyp[i] & P_TYP_TRANSIT) != 0;
        drawPlanetSymbol(painter, i, degree, isTransit, offsetLevel[i]);
    }
    
    // Transit-Planeten Symbole
    if (m_transit != nullptr) {
        for (int i = 0; i < m_transit->anzahlPlanet; ++i) {
            double degree = m_transit->planet[i];
            drawPlanetSymbol(painter, i, degree, true, transitOffsetLevel[i]);
        }
    }
}

void ChartWidget::drawPlanetTic(QPainter& painter, double angle, bool isTransit, int offsetLevel) {
    QColor color = isTransit ? sColor[COL_PLANET_TICT] : sColor[COL_PLANET_TIC];
    
    // STRICT LEGACY: Alle Linien gleichdick
    painter.setPen(QPen(color, 1));
    
    // STRICT LEGACY: Linie vom Planeten-Kreis bis zum Planeten-Symbol durchziehen
    // Versatz nach innen für jeden Level
    double offset = offsetLevel * 25.0;
    double symbolRadius = m_radiusPlanet - 30 - offset;
    
    QPointF outer = degreeToPoint(angle, m_radiusPlanet);
    QPointF inner = degreeToPoint(angle, symbolRadius + 12);  // Bis kurz vor das Symbol
    painter.drawLine(outer, inner);
}

void ChartWidget::drawPlanetSymbol(QPainter& painter, int planet, double angle, bool isTransit, int offsetLevel) {
    // Planeten-Farbe aus Einstellungen oder Standard
    QColor color;
    if (isTransit) {
        color = sColor[COL_PLAN_T];
    } else if (planet < m_auinit.planetColor.size() && m_auinit.planetColor[planet].isValid()) {
        color = m_auinit.planetColor[planet];
    } else {
        color = sColor[COL_PLAN];
    }
    
    // Hervorhebung: Größerer Font und hellere Farbe
    bool isHighlighted = (planet == m_highlightPlanet) || 
                         (planet == m_highlightAspect1) || 
                         (planet == m_highlightAspect2);
    
    // STRICT LEGACY: Kleinerer Font wenn versetzt
    int fontSize = isHighlighted ? 24 : (offsetLevel > 0 ? 16 : 20);
    
    // Plattformübergreifende Symbol-Font Fallback-Kette:
    // Windows: Segoe UI Symbol
    // macOS: Apple Symbols
    // Linux: Noto Sans Symbols2, DejaVu Sans
    QFont symbolFont;
    QStringList symbolFonts = {"Segoe UI Symbol", "Apple Symbols", "Noto Sans Symbols2", "DejaVu Sans"};
    for (const QString& fontName : symbolFonts) {
        symbolFont = QFont(fontName, fontSize);
        if (QFontInfo(symbolFont).family().contains(fontName.left(5), Qt::CaseInsensitive)) {
            break;  // Font gefunden
        }
    }
    symbolFont.setPointSize(fontSize);
    painter.setFont(symbolFont);
    
    // Position im Inneren des Planeten-Kreises, mit Versatz für Kollisionsvermeidung
    // STRICT LEGACY: Größerer Abstand zwischen versetzten Planeten
    double offset = offsetLevel * 25.0;
    double symbolRadius = m_radiusPlanet - 30 - offset;
    QPointF pos = degreeToPoint(angle, symbolRadius);
    
    if (isHighlighted) {
        // Heller und mit Hintergrund-Kreis an der gleichen Position wie das Symbol
        color = color.lighter(150);
        painter.setBrush(QColor(255, 255, 0, 80));  // Gelber Hintergrund
        painter.setPen(Qt::NoPen);
        painter.drawEllipse(pos, 16, 16);
    }
    
    painter.setPen(color);
    
    // Unicode-Symbol zeichnen - größerer Bereich
    int rectSize = isHighlighted ? 32 : (offsetLevel > 0 ? 22 : 28);
    QRectF rect(pos.x() - rectSize/2, pos.y() - rectSize/2, rectSize, rectSize);
    
    if (planet < MAX_PLANET) {
        QString symbol = QString::fromUtf8(PLANET_SYMBOLS[planet]);
        painter.drawText(rect, Qt::AlignCenter, symbol);
    }
    
    // Rückläufigkeits-Symbol
    if (m_radix.planetTyp[planet] & P_TYP_RUCK) {
        // Systemabhängiger Standard-Font statt fester Arial-Angabe
        QFont retroFont = m_mainFont;
        retroFont.setPointSize(offsetLevel > 0 ? 7 : 9);
        painter.setFont(retroFont);
        painter.setPen(color);
        QRectF retroRect(pos.x() + 12, pos.y() - 6, 12, 12);
        painter.drawText(retroRect, Qt::AlignCenter, "R");
    }
}

//==============================================================================
// Aspekte zeichnen
// Port von: sDrawAspekte(HDC, RADIX*, RADIX*, char)
//==============================================================================

void ChartWidget::drawAspects(QPainter& painter) {
    if (m_radix.aspPlanet.isEmpty()) return;
    
    int numPlanets = m_radix.anzahlPlanet;
    
    // STRICT LEGACY: LB_A Flag bestimmt welche Aspekte angezeigt werden
    // m_showSynastrieAspects = false: Radix-Aspekte (Radix -> Radix)
    // m_showSynastrieAspects = true: Synastrie/Transit-Aspekte (Transit -> Radix)
    
    if (!m_showSynastrieAspects || m_transit == nullptr) {
        // Normale Radix-Aspekte zeichnen (Radix -> Radix)
        for (int i = 0; i < numPlanets; ++i) {
            for (int j = i + 1; j < numPlanets; ++j) {
                // Hervorgehobenen Aspekt überspringen (wird später gezeichnet)
                if ((i == m_highlightAspect1 && j == m_highlightAspect2) ||
                    (i == m_highlightAspect2 && j == m_highlightAspect1)) {
                    continue;
                }
                
                int idx = i * numPlanets + j;
                int8_t asp = m_radix.aspPlanet[idx];
                
                if (asp == KEIN_ASP) continue;
                
                // Aspekt-Linie setzen
                setAspectPen(painter, asp);
                
                // Linien-Endpunkte
                QPointF p1 = degreeToPoint(m_radix.planet[i], m_radiusAsp);
                QPointF p2 = degreeToPoint(m_radix.planet[j], m_radiusAsp);
                
                painter.drawLine(p1, p2);
            }
        }
    } else {
        // STRICT LEGACY: Synastrie/Transit-Aspekte zeichnen (Transit-Planet zu Radix-Planet)
        int numTransit = m_transit->anzahlPlanet;
        
        for (int i = 0; i < numTransit; ++i) {
            for (int j = 0; j < numPlanets; ++j) {
                // Aspekt zwischen Transit-Planet i und Radix-Planet j berechnen
                double transitPos = m_transit->planet[i];
                double radixPos = m_radix.planet[j];
                double diff = std::abs(transitPos - radixPos);
                if (diff > 180.0) diff = 360.0 - diff;
                
                // Aspekt prüfen (mit Standard-Orben)
                int8_t asp = KEIN_ASP;
                if (diff <= 10.0) asp = KONJUNKTION;
                else if (std::abs(diff - 30.0) <= 3.0) asp = HALBSEX;
                else if (std::abs(diff - 60.0) <= 6.0) asp = SEXTIL;
                else if (std::abs(diff - 90.0) <= 8.0) asp = QUADRATUR;
                else if (std::abs(diff - 120.0) <= 8.0) asp = TRIGON;
                else if (std::abs(diff - 150.0) <= 3.0) asp = QUINCUNX;
                else if (std::abs(diff - 180.0) <= 10.0) asp = OPOSITION;
                
                if (asp == KEIN_ASP) continue;
                
                // Hervorgehobenen Aspekt überspringen (wird später gezeichnet)
                if (i == m_highlightTransitPlanet && j == m_highlightRadixPlanet) {
                    continue;
                }
                
                // Aspekt-Linie setzen
                setAspectPen(painter, asp);
                
                // Linien-Endpunkte
                QPointF p1 = degreeToPoint(transitPos, m_radiusAsp);
                QPointF p2 = degreeToPoint(radixPos, m_radiusAsp);
                
                painter.drawLine(p1, p2);
            }
        }
    }
    
    // Hervorgehobenen Aspekt zuletzt zeichnen (oben drauf)
    if (m_highlightAspect1 >= 0 && m_highlightAspect2 >= 0 && !m_showSynastrieAspects) {
        int i = qMin(m_highlightAspect1, m_highlightAspect2);
        int j = qMax(m_highlightAspect1, m_highlightAspect2);
        int idx = i * numPlanets + j;
        int asp = static_cast<int>(m_radix.aspPlanet[idx]);
        
        if (asp != KEIN_ASP) {
            // Dickere, hellere Linie für Hervorhebung
            QColor color;
            switch (asp) {
                case KONJUNKTION: color = sColor[CKONJUNKTION]; break;
                case SEXTIL:      color = sColor[CSEXTIL]; break;
                case QUADRATUR:   color = sColor[CQUADRATUR]; break;
                case TRIGON:      color = sColor[CTRIGON]; break;
                case OPOSITION:   color = sColor[COPOSITION]; break;
                default:          color = sColor[COL_TXT]; break;
            }
            color = color.lighter(150);
            painter.setPen(QPen(color, 4, Qt::SolidLine));
            
            QPointF p1 = degreeToPoint(m_radix.planet[m_highlightAspect1], m_radiusAsp);
            QPointF p2 = degreeToPoint(m_radix.planet[m_highlightAspect2], m_radiusAsp);
            
            painter.drawLine(p1, p2);
        }
    }
    
    // STRICT LEGACY: Hervorgehobenen Transit-Aspekt zeichnen
    if (m_highlightTransitPlanet >= 0 && m_highlightRadixPlanet >= 0 && m_transit) {
        // Transit-Planet Position
        if (m_highlightTransitPlanet < m_transit->planet.size() &&
            m_highlightRadixPlanet < m_radix.anzahlPlanet) {
            
            double transitPos = m_transit->planet[m_highlightTransitPlanet];
            double radixPos = m_radix.planet[m_highlightRadixPlanet];
            
            // Aspekt berechnen
            double diff = std::abs(transitPos - radixPos);
            if (diff > 180.0) diff = 360.0 - diff;
            
            int asp = KEIN_ASP;
            if (diff <= 10.0) asp = KONJUNKTION;
            else if (std::abs(diff - 30.0) <= 5.0) asp = HALBSEX;
            else if (std::abs(diff - 60.0) <= 8.0) asp = SEXTIL;
            else if (std::abs(diff - 90.0) <= 10.0) asp = QUADRATUR;
            else if (std::abs(diff - 120.0) <= 10.0) asp = TRIGON;
            else if (std::abs(diff - 150.0) <= 5.0) asp = QUINCUNX;
            else if (std::abs(diff - 180.0) <= 10.0) asp = OPOSITION;
            
            // Dickere, hellere Linie für Hervorhebung
            QColor color;
            switch (asp) {
                case KONJUNKTION: color = sColor[CKONJUNKTION]; break;
                case HALBSEX:     color = sColor[CHALBSEX]; break;
                case SEXTIL:      color = sColor[CSEXTIL]; break;
                case QUADRATUR:   color = sColor[CQUADRATUR]; break;
                case TRIGON:      color = sColor[CTRIGON]; break;
                case QUINCUNX:    color = sColor[CQUINCUNX]; break;
                case OPOSITION:   color = sColor[COPOSITION]; break;
                default:          color = sColor[COL_TXT]; break;
            }
            color = color.lighter(150);
            painter.setPen(QPen(color, 4, Qt::SolidLine));
            
            // Linie vom Transit-Planeten zum Radix-Planeten
            QPointF p1 = degreeToPoint(transitPos, m_radiusAsp);
            QPointF p2 = degreeToPoint(radixPos, m_radiusAsp);
            
            painter.drawLine(p1, p2);
        }
    }
}

void ChartWidget::setAspectPen(QPainter& painter, int aspect) {
    QColor color;
    Qt::PenStyle style = Qt::SolidLine;
    int width = 1;
    
    // STRICT LEGACY: Aspekt-Linienstile wie im Original
    // Konjunktion: durchgezogen
    // Sextil: lang gestrichelt (DashLine)
    // Quadrat: durchgezogen
    // Trigon: kurz gestrichelt (DashDotLine)
    // Opposition: durchgezogen
    // Halbsextil: punktiert
    // Quincunx: Strich-Punkt
    
    switch (aspect) {
        case KONJUNKTION:
            color = sColor[CKONJUNKTION];
            style = Qt::SolidLine;
            width = 1;
            break;
        case HALBSEX:
            color = sColor[CHALBSEX];
            style = Qt::DotLine;  // Punktiert
            width = 1;
            break;
        case SEXTIL:
            color = sColor[CSEXTIL];
            style = Qt::DashLine;  // Lang gestrichelt
            width = 1;
            break;
        case QUADRATUR:
            color = sColor[CQUADRATUR];
            style = Qt::SolidLine;  // Durchgezogen
            width = 1;
            break;
        case TRIGON:
            color = sColor[CTRIGON];
            style = Qt::DashDotLine;  // Kurz gestrichelt (Strich-Punkt)
            width = 1;
            break;
        case QUINCUNX:
            color = sColor[CQUINCUNX];
            style = Qt::DashDotDotLine;  // Strich-Punkt-Punkt
            width = 1;
            break;
        case OPOSITION:
            color = sColor[COPOSITION];
            style = Qt::SolidLine;  // Durchgezogen
            width = 1;
            break;
        default:
            color = sColor[COL_TXT];
    }
    
    painter.setPen(QPen(color, width, style));
}

} // namespace astro
