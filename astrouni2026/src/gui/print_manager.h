#pragma once
/**
 * @file print_manager.h
 * @brief Druck-Funktionen für Horoskope
 * 
 * 1:1 Port von sDlgPrint, sPrintAsp, sPrintText, sPrintKoord aus legacy/auprint.c
 */

#include <QObject>
#include <QPrinter>
#include <QPainter>
#include "../core/data_types.h"
#include "../core/transit_calc.h"

namespace astro {

/**
 * @brief Druck-Manager für Horoskope
 * 
 * Port von: sDlgPrint(), sPrintAsp(), sPrintText(), sPrintKoord(), sPrintMTransit()
 */
class PrintManager : public QObject {
    Q_OBJECT
    
public:
    explicit PrintManager(QObject* parent = nullptr);
    ~PrintManager();
    
    /**
     * @brief Druckt ein Radix-Horoskop (Grafik + Text + Aspekte)
     * Port von: sDlgPrint(Radix, PRN_NORMAL)
     * 
     * @param radix Radix-Daten
     * @param auinit Einstellungen
     * @param parent Parent-Widget für Dialoge
     * @return true bei Erfolg
     */
    bool printRadix(const Radix& radix, const AuInit& auinit, QWidget* parent = nullptr);
    
    /**
     * @brief Druckt Multi-Transit-Liste
     * Port von: sDlgPrint(Radix, PRN_MULTITRANS) -> sPrintMTransit()
     * 
     * @param basisRadix Basis-Radix
     * @param transits Transit-Liste
     * @param aspekte Transit-Aspekte
     * @param auinit Einstellungen
     * @param parent Parent-Widget für Dialoge
     * @return true bei Erfolg
     */
    bool printTransits(const Radix& basisRadix,
                       const QVector<Radix>& transits,
                       const QVector<TransitAspekt>& aspekte,
                       const AuInit& auinit,
                       QWidget* parent = nullptr);
    
    /**
     * @brief Zeigt Drucker-Setup-Dialog
     * Port von: hdcPrinterIni(PRN_SETUP, ...)
     */
    bool showPrinterSetup(QWidget* parent = nullptr);
    
private:
    // Druck-Hilfsfunktionen (Port von auprint.c)
    
    /**
     * @brief Druckt Personen-Daten und Positionen
     * Port von: sPrintText()
     */
    void printText(QPainter& painter, const Radix& radix, double factor);
    
    /**
     * @brief Druckt Aspekte-Tabelle
     * Port von: sPrintAsp()
     */
    void printAspects(QPainter& painter, const Radix& radix, double factor);
    
    /**
     * @brief Druckt Häuser-Aspekte
     * Port von: sPrintHausAsp()
     */
    void printHouseAspects(QPainter& painter, const Radix& radix, int yStart, double factor);
    
    /**
     * @brief Druckt Koordinaten-Seite
     * Port von: sPrintKoord()
     */
    void printCoordinates(QPainter& painter, const Radix& radix, double factor);
    
    /**
     * @brief Zeichnet Radix-Grafik für Druck
     * Port von: sRadix(), sHaus(), sPlanet(), sDrawAspekte() mit Druck-Skalierung
     */
    void drawRadixForPrint(QPainter& painter, const Radix& radix, const AuInit& auinit, 
                           int centerX, int centerY, int radius, double factor);
    
    // Hilfsfunktionen
    QString formatDegree(double degree) const;
    QString formatPosition(double degree) const;
    
    // Drucker
    QPrinter* m_printer;
    
    // Druck-Parameter (Port von Sys.xPrn, Sys.yPrn, Sys.PrnFactor)
    int m_pageWidth;
    int m_pageHeight;
    double m_factor;
    int m_centerX;
    int m_centerY;
    int m_radius;
};

} // namespace astro
