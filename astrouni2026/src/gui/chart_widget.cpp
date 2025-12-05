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
#include <cmath>

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
    , m_show9Degree(true) {
    
    // Hintergrundfarbe
    setAutoFillBackground(true);
    QPalette pal = palette();
    pal.setColor(QPalette::Window, sColor[COL_HINTER]);
    setPalette(pal);
    
    // Fonts
    m_mainFont = QFont("Arial", 10);
    m_astroFont = QFont("AstroUniverse", 14);  // Astrologie-Font
    
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
    
    // Rotation berechnen (ASC links = 180° - ASC)
    if (m_auinit.sRotRadix == ROT_ASC) {
        m_rotation = 180.0 - m_radix.asc;
    } else if (m_auinit.sRotRadix == ROT_SONNE) {
        m_rotation = 180.0 - m_radix.planet[P_SONNE];
    } else {
        m_rotation = 0.0;  // 0° Widder links
    }
    
    update();
}

void ChartWidget::setTransitRadix(const Radix* transit) {
    m_transit = transit;
    update();
}

void ChartWidget::setSettings(const AuInit& auinit) {
    m_auinit = auinit;
    
    // Teilung (3er/9er Grad-Markierungen)
    m_show3Degree = !(m_auinit.sTeilung & S_3er);
    m_show9Degree = !(m_auinit.sTeilung & S_9er);
    
    update();
}

void ChartWidget::updateChart() {
    calculateRadii();
    update();
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
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // Radien berechnen
    calculateRadii();
    
    // Zeichnen
    drawRadix(painter);
    drawZodiacSigns(painter);
    drawDegreeMarks(painter);
    drawHouses(painter);
    drawPlanets(painter);
    
    if (m_showAspects) {
        drawAspects(painter);
    }
}

void ChartWidget::mousePressEvent(QMouseEvent* event) {
    // TODO: Klick auf Planet/Haus erkennen
    QWidget::mousePressEvent(event);
}

void ChartWidget::resizeEvent(QResizeEvent* event) {
    calculateRadii();
    QWidget::resizeEvent(event);
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
    // Rotation anwenden
    double rotatedDegree = degree + m_rotation;
    
    // In Radiant umrechnen (0° = rechts, gegen Uhrzeigersinn)
    // Astrologie: 0° Widder = links, im Uhrzeigersinn
    double rad = (180.0 - rotatedDegree) * PI / 180.0;
    
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
    
    // Sternzeichen-Symbole
    painter.setFont(m_astroFont);
    
    // Element-Farben für Sternzeichen
    QColor elementColors[4] = {
        sColor[COL_FEUER],   // Feuer: Widder, Löwe, Schütze
        sColor[COL_ERDE],    // Erde: Stier, Jungfrau, Steinbock
        sColor[COL_LUFT],    // Luft: Zwillinge, Waage, Wassermann
        sColor[COL_WASSER]   // Wasser: Krebs, Skorpion, Fische
    };
    
    // Sternzeichen-Symbole (Unicode oder Font-Zeichen)
    QString symbols[12] = {
        "a", "b", "c", "d", "e", "f",  // Widder bis Jungfrau
        "g", "h", "i", "j", "k", "l"   // Waage bis Fische
    };
    
    for (int i = 0; i < 12; ++i) {
        double degree = i * 30.0 + 15.0;  // Mitte des Zeichens
        double symbolRadius = (m_radius + m_radiusStz) / 2.0;
        QPointF pos = degreeToPoint(degree, symbolRadius);
        
        // Element-Farbe (Feuer=0,4,8; Erde=1,5,9; Luft=2,6,10; Wasser=3,7,11)
        int element = i % 4;
        painter.setPen(elementColors[element]);
        
        // Symbol zentriert zeichnen
        QRectF rect(pos.x() - 10, pos.y() - 10, 20, 20);
        painter.drawText(rect, Qt::AlignCenter, symbols[i]);
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
        
        // ASC und MC dicker zeichnen
        if (i == 0 || i == 9) {
            painter.setPen(QPen(sColor[COL_ASZ], 2));
        } else if (i == 3 || i == 6) {
            // IC und DC
            painter.setPen(QPen(sColor[COL_ASZ], 1.5));
        } else {
            painter.setPen(QPen(sColor[COL_HAUSER], 1));
        }
        
        QPointF outer = degreeToPoint(degree, m_radiusPlanet);
        QPointF inner = degreeToPoint(degree, m_radiusAsp);
        painter.drawLine(outer, inner);
    }
    
    // Haus-Nummern
    painter.setFont(m_mainFont);
    painter.setPen(sColor[COL_TXT]);
    
    for (int i = 0; i < MAX_HAUS; ++i) {
        // Mitte des Hauses
        double start = m_radix.haus[i];
        double end = m_radix.haus[(i + 1) % MAX_HAUS];
        double mid = start + Calculations::minDist(start, end) / 2.0;
        mid = Calculations::mod360(mid);
        
        double numberRadius = (m_radiusPlanet + m_radiusAsp) / 2.0;
        QPointF pos = degreeToPoint(mid, numberRadius);
        
        QString number = QString::number(i + 1);
        QRectF rect(pos.x() - 8, pos.y() - 8, 16, 16);
        painter.drawText(rect, Qt::AlignCenter, number);
    }
}

//==============================================================================
// Planeten zeichnen
// Port von: sPlanet(HDC, RADIX*, char)
//==============================================================================

void ChartWidget::drawPlanets(QPainter& painter) {
    if (m_radix.planet.isEmpty()) return;
    
    for (int i = 0; i < m_radix.anzahlPlanet; ++i) {
        double degree = m_radix.planet[i];
        bool isTransit = (m_radix.planetTyp[i] & P_TYP_TRANSIT) != 0;
        
        // Markierung (Tic)
        drawPlanetTic(painter, degree, isTransit);
        
        // Symbol
        drawPlanetSymbol(painter, i, degree, isTransit);
    }
    
    // Transit-Planeten (falls vorhanden)
    if (m_transit != nullptr) {
        for (int i = 0; i < m_transit->anzahlPlanet; ++i) {
            double degree = m_transit->planet[i];
            drawPlanetTic(painter, degree, true);
            drawPlanetSymbol(painter, i, degree, true);
        }
    }
}

void ChartWidget::drawPlanetTic(QPainter& painter, double angle, bool isTransit) {
    QColor color = isTransit ? sColor[COL_PLANET_TICT] : sColor[COL_PLANET_TIC];
    painter.setPen(QPen(color, 2));
    
    QPointF outer = degreeToPoint(angle, m_radiusPlanet);
    QPointF inner = degreeToPoint(angle, m_radiusPlanet - 10);
    painter.drawLine(outer, inner);
}

void ChartWidget::drawPlanetSymbol(QPainter& painter, int planet, double angle, bool isTransit) {
    // Planeten-Symbole (für Astrologie-Font)
    QString symbols[MAX_PLANET] = {
        "A", "B", "C", "D", "E", "F", "G", "H", "I", "J",  // Sonne bis Pluto
        "K", "L", "M", "N", "O", "P", "Q"  // Mondknoten bis Vesta
    };
    
    QColor color = isTransit ? sColor[COL_PLAN_T] : sColor[COL_PLAN];
    painter.setPen(color);
    painter.setFont(m_astroFont);
    
    // Position außerhalb des Planeten-Kreises
    double symbolRadius = m_radiusPlanet + 15;
    QPointF pos = degreeToPoint(angle, symbolRadius);
    
    // Symbol zeichnen
    QRectF rect(pos.x() - 10, pos.y() - 10, 20, 20);
    
    if (planet < MAX_PLANET) {
        painter.drawText(rect, Qt::AlignCenter, symbols[planet]);
    }
    
    // Rückläufigkeits-Symbol
    if (m_radix.planetTyp[planet] & P_TYP_RUCK) {
        painter.setFont(QFont("Arial", 8));
        QRectF retroRect(pos.x() + 8, pos.y() - 5, 10, 10);
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
    
    for (int i = 0; i < numPlanets; ++i) {
        for (int j = i + 1; j < numPlanets; ++j) {
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
}

void ChartWidget::setAspectPen(QPainter& painter, int aspect) {
    QColor color;
    Qt::PenStyle style = Qt::SolidLine;
    int width = 1;
    
    switch (aspect) {
        case KONJUNKTION:
            color = sColor[CKONJUNKTION];
            width = 2;
            break;
        case HALBSEX:
            color = sColor[CHALBSEX];
            style = Qt::DotLine;
            break;
        case SEXTIL:
            color = sColor[CSEXTIL];
            break;
        case QUADRATUR:
            color = sColor[CQUADRATUR];
            width = 2;
            break;
        case TRIGON:
            color = sColor[CTRIGON];
            break;
        case QUINCUNX:
            color = sColor[CQUINCUNX];
            style = Qt::DashLine;
            break;
        case OPOSITION:
            color = sColor[COPOSITION];
            width = 2;
            break;
        default:
            color = sColor[COL_TXT];
    }
    
    painter.setPen(QPen(color, width, style));
}

} // namespace astro
