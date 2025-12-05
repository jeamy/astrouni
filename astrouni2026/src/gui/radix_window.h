#pragma once
/**
 * @file radix_window.h
 * @brief Radix-Fenster für die Horoskop-Anzeige
 * 
 * 1:1 Port von RadixWndProc aus legacy/astrouni.c
 */

#include <QMainWindow>
#include <QListWidget>
#include <QPushButton>
#include "../core/data_types.h"

namespace astro {

class ChartWidget;

/**
 * @brief Radix-Fenster
 * 
 * Port von: RadixWndProc, PaintRadixWnd
 */
class RadixWindow : public QMainWindow {
    Q_OBJECT
    
public:
    RadixWindow(QWidget* parent, AuInit& auinit, Radix& radix);
    ~RadixWindow();
    
    /**
     * @brief Setzt die Radix-Daten
     */
    void setRadix(const Radix& radix);
    
    /**
     * @brief Aktualisiert die Anzeige
     */
    void updateDisplay();
    
private slots:
    // Menü-Aktionen
    void onSave();
    void onPrint();
    void onClose();
    void onHoroTyp();
    
    // Buttons
    void onDatenClicked();
    void onPositionenClicked();
    void onAspekteClicked();
    
    // Listbox
    void onListItemClicked(QListWidgetItem* item);
    
private:
    void setupMenus();
    void setupUI();
    void fillDatenList();
    void fillPositionenList();
    void fillAspekteList();
    
    // Referenzen
    AuInit& m_auinit;
    Radix& m_radix;
    
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
