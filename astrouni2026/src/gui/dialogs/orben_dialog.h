#pragma once
/**
 * @file orben_dialog.h
 * @brief Orben-Einstellungen Dialog
 * 
 * Port von DlgOrbenEin aus legacy/astrofil.c
 * 
 * Layout wie im Legacy:
 * - Dropdown für Planet1 (Y-Planet)
 * - Matrix: Zeilen = Aspekte (7), Spalten = Planet2 (alle Planeten)
 * - Orben sind 3D: [Planet1][Planet2][Aspekt]
 */

#include <QDialog>
#include <QDoubleSpinBox>
#include <QTableWidget>
#include <QRadioButton>
#include <QButtonGroup>
#include <QComboBox>
#include "../../core/data_types.h"

namespace astro {

// Orben-Typ (wie im Legacy: RBP_RADIX, RBP_TRANSIT, RBP_SYNASTRIE)
enum OrbenTyp {
    ORBEN_RADIX = 0,
    ORBEN_TRANSIT = 1,
    ORBEN_SYNASTRIE = 2
};

class OrbenDialog : public QDialog {
    Q_OBJECT
public:
    explicit OrbenDialog(AuInit& auinit, QWidget* parent = nullptr);
    
private slots:
    void onAccept();
    void onDefault();
    void onTypChanged(int typ);
    void onPlanet1Changed(int index);
    void onCellChanged(int row, int col);
    
private:
    void setupUI();
    void loadSettings();
    void saveSettings();
    void setDefaultOrben();
    void updateTable();
    
    AuInit& m_auinit;
    int m_currentTyp;
    int m_currentPlanet1;   // Ausgewählter Planet1 (Y-Achse via Dropdown)
    bool m_loading;         // Verhindert rekursive Updates
    
    // Typ-Auswahl (Radix/Transit/Synastrie)
    QButtonGroup* m_typGroup;
    QRadioButton* m_radixRadio;
    QRadioButton* m_transitRadio;
    QRadioButton* m_synastrieRadio;
    
    // Planet1 Auswahl (Y-Planet)
    QComboBox* m_planet1Combo;
    
    // Orben-Matrix: Zeilen = Aspekte, Spalten = Planet2
    QTableWidget* m_orbenTable;
};

} // namespace astro
