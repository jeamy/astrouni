#pragma once
/**
 * @file radix_window.h
 * @brief Radix-Widget für die Horoskop-Anzeige (als Tab im Hauptfenster)
 * 
 * 1:1 Port von RadixWndProc aus legacy/astrouni.c
 */

#include <QWidget>
#include <QListWidget>
#include <QPushButton>
#include "../core/data_types.h"

namespace astro {

class ChartWidget;

/**
 * @brief Radix-Widget (als Tab im Hauptfenster)
 * 
 * Port von: RadixWndProc, PaintRadixWnd
 */
class RadixWindow : public QWidget {
    Q_OBJECT
    
public:
    RadixWindow(QWidget* parent, const AuInit& auinit, const Radix& radix);
    ~RadixWindow();
    
    /**
     * @brief Gibt den Tab-Titel zurück
     */
    QString getTabTitle() const;
    
    /**
     * @brief Setzt die Radix-Daten
     */
    void setRadix(const Radix& radix);
    
    /**
     * @brief Aktualisiert die Anzeige
     */
    void updateDisplay();
    
private slots:
    // Buttons
    void onDatenClicked();
    void onPositionenClicked();
    void onAspekteClicked();
    
    // Listbox
    void onListItemClicked(QListWidgetItem* item);
    
private:
    void setupUI();
    void fillDatenList();
    void fillPositionenList();
    void fillAspekteList();
    
    // Eigene Kopien der Daten (für parallele Fenster)
    AuInit m_auinit;
    Radix m_radix;
    
    // Widgets
    ChartWidget* m_chartWidget;
    QListWidget* m_listWidget;
    QPushButton* m_datenButton;
    QPushButton* m_positionenButton;
    QPushButton* m_aspekteButton;
    
    // Aktuelle Ansicht
    enum ListMode { Daten, Positionen, Aspekte };
    ListMode m_listMode;
};

} // namespace astro
