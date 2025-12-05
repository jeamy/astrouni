#pragma once
/**
 * @file chart_widget.h
 * @brief Widget für die Radix-Zeichnung
 * 
 * 1:1 Port der Zeichenfunktionen aus legacy/auwurzel.c
 */

#include <QWidget>
#include <QPainter>
#include <QFont>
#include "../core/data_types.h"

namespace astro {

/**
 * @brief Widget für die Radix-Zeichnung
 * 
 * Port von: sRadix, sHaus, sPlanet, sDrawAspekte aus auwurzel.c
 */
class ChartWidget : public QWidget {
    Q_OBJECT
    
public:
    explicit ChartWidget(QWidget* parent = nullptr);
    ~ChartWidget();
    
    /**
     * @brief Setzt die Radix-Daten zum Zeichnen
     */
    void setRadix(const Radix& radix);
    
    /**
     * @brief Setzt die Transit-Radix (für Transit/Synastrie)
     */
    void setTransitRadix(const Radix* transit);
    
    /**
     * @brief Setzt die Einstellungen
     */
    void setSettings(const AuInit& auinit);
    
    /**
     * @brief Aktualisiert die Zeichnung
     */
    void updateChart();
    
    /**
     * @brief Hebt einen Planeten hervor
     * @param planetIndex Index des Planeten (-1 für keine Hervorhebung)
     */
    void highlightPlanet(int planetIndex);
    
    /**
     * @brief Hebt einen Aspekt hervor
     * @param planet1 Erster Planet
     * @param planet2 Zweiter Planet
     */
    void highlightAspect(int planet1, int planet2);
    
    /**
     * @brief Gibt die minimale Größe zurück
     */
    QSize minimumSizeHint() const override;
    
    /**
     * @brief Gibt die bevorzugte Größe zurück
     */
    QSize sizeHint() const override;
    
signals:
    /**
     * @brief Signal wenn auf einen Planeten geklickt wurde
     */
    void planetClicked(int planetIndex);
    
    /**
     * @brief Signal wenn auf ein Haus geklickt wurde
     */
    void houseClicked(int houseIndex);
    
protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    
private:
    // Zeichenfunktionen (Port von auwurzel.c)
    
    /**
     * @brief Zeichnet den Radix-Kreis
     * Port von: sRadix(HDC, RADIX*)
     */
    void drawRadix(QPainter& painter);
    
    /**
     * @brief Zeichnet die Häuser
     * Port von: sHaus(HDC, RADIX*, char)
     */
    void drawHouses(QPainter& painter);
    
    /**
     * @brief Zeichnet die Planeten
     * Port von: sPlanet(HDC, RADIX*, char)
     */
    void drawPlanets(QPainter& painter);
    
    /**
     * @brief Zeichnet ein Planeten-Symbol
     * Port von: vPlanetDraw(HDC, RADIX*, char*, char, short)
     */
    void drawPlanetSymbol(QPainter& painter, int planet, double angle, bool isTransit);
    
    /**
     * @brief Zeichnet die Planeten-Markierung (Tic)
     * Port von: vPlanetTicDraw(HDC, RADIX*, char*, char, short)
     */
    void drawPlanetTic(QPainter& painter, double angle, bool isTransit);
    
    /**
     * @brief Zeichnet die Aspekte
     * Port von: sDrawAspekte(HDC, RADIX*, RADIX*, char)
     */
    void drawAspects(QPainter& painter);
    
    /**
     * @brief Zeichnet die Sternzeichen
     */
    void drawZodiacSigns(QPainter& painter);
    
    /**
     * @brief Zeichnet die Grad-Einteilung
     */
    void drawDegreeMarks(QPainter& painter);
    
    // Hilfsfunktionen
    
    /**
     * @brief Konvertiert Grad in Bildschirm-Koordinaten
     */
    QPointF degreeToPoint(double degree, double radius) const;
    
    /**
     * @brief Setzt die Aspekt-Linien-Eigenschaften
     * Port von: sSetAspLine(HDC, short, char)
     */
    void setAspectPen(QPainter& painter, int aspect);
    
    /**
     * @brief Berechnet die Radien für die verschiedenen Kreise
     */
    void calculateRadii();
    
    // Daten
    Radix m_radix;
    const Radix* m_transit;
    AuInit m_auinit;
    
    // Zeichnungs-Parameter
    QPointF m_center;           // Mittelpunkt
    double m_radius;            // Basis-Radius
    double m_radiusStz;         // Radius Sternzeichen-Kreis
    double m_radius10;          // Radius 10-Grad-Markierung
    double m_radius3;           // Radius 3-Grad-Markierung
    double m_radius5;           // Radius 5-Grad-Markierung
    double m_radiusPlanet;      // Radius Planeten-Kreis
    double m_radiusAsp;         // Radius Aspekt-Kreis
    
    // Rotation (0° Widder oder ASC oben)
    double m_rotation;
    
    // Fonts
    QFont m_mainFont;
    QFont m_astroFont;          // Astrologie-Symbole
    
    // Flags
    bool m_showAspects;
    bool m_show3Degree;
    bool m_show9Degree;
    
    // Hervorhebung
    int m_highlightPlanet;      // -1 = keine Hervorhebung
    int m_highlightAspect1;     // Erster Planet des Aspekts
    int m_highlightAspect2;     // Zweiter Planet des Aspekts
};

} // namespace astro
