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
#include <QSpinBox>
#include <QLabel>
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
    void setTransitSelection(const QVector<QVector<bool>>& sel);
    
    /**
     * @brief Aktualisiert die Anzeige
     */
    void updateDisplay();
    
private slots:
    // Buttons
    void onDatenClicked();
    void onPositionenClicked();
    void onAspekteClicked();
    void onNeuClicked();
    
    // Zeit-Scroll
    void onMinuteChanged(int value);
    void onHourChanged(int value);
    void onDayChanged(int value);
    void onMonthChanged(int value);
    void onYearChanged(int value);
    
    // Listbox
    void onListItemClicked(QListWidgetItem* item);
    
    // Maus aus ChartWidget
    void onPlanetClicked(int planetIndex, bool isTransit);
    void onHouseClicked(int houseIndex);
    void onAspectClicked(int idx1, int idx2, bool isTransit);
    
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
    
    // STRICT LEGACY: Zeit-Scroll Widgets (Port von SC_MIN, SC_STU, SC_TAG, SC_MON, SC_JAH)
    QPushButton* m_neuButton;
    QSpinBox* m_minuteSpinBox;
    QSpinBox* m_hourSpinBox;
    QSpinBox* m_daySpinBox;
    QSpinBox* m_monthSpinBox;
    QSpinBox* m_yearSpinBox;
    
    // Aktuelle Ansicht
    enum ListMode { Daten, Positionen, Aspekte };
    ListMode m_listMode;
    
    // STRICT LEGACY: LB_G Flag für Synastrie-Toggle (Person 1 / Person 2)
    bool m_showPerson2;  // false = Person 1, true = Person 2
    
    // STRICT LEGACY: LB_A Flag für Aspekte-Toggle (Radix / Synastrie-Transit)
    bool m_showSynastrieAspects;  // false = Radix-Aspekte, true = Synastrie/Transit-Aspekte
    QVector<QVector<bool>> m_transitSelection; // Auswahlmatrix aus TransSelDialog
    
    // Flag um rekursive Updates zu verhindern
    bool m_updatingSpinBoxes;
    
    // Hilfsfunktion
    void updateTimeSpinBoxes();
    void recalculateChart();
};

} // namespace astro
