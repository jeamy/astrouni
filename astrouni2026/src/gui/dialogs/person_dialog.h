#pragma once
/**
 * @file person_dialog.h
 * @brief Dialog zum Erfassen von Personen
 * 
 * 1:1 Port von DlgPErfassen aus legacy/astrofil.c
 */

#include <QDialog>
#include <QLineEdit>
#include <QDateEdit>
#include <QTimeEdit>
#include <QComboBox>
#include <QDoubleSpinBox>
#include "../../core/data_types.h"

namespace astro {

/**
 * @brief Dialog zum Erfassen von Personen
 * 
 * Port von: E_PERSON Dialog, DlgPErfassen
 */
class PersonDialog : public QDialog {
    Q_OBJECT
    
public:
    PersonDialog(QWidget* parent, AuInit& auinit, Radix& radix);
    ~PersonDialog();
    
    /**
     * @brief Gibt zurück ob Radix berechnet wurde
     */
    bool wasCalculated() const { return m_calculated; }
    
private slots:
    void onNamenSuchen();
    void onOrtSuchen();
    void onCalc();
    void onAccept();
    
private:
    void setupUI();
    void loadFromRadix();
    void saveToRadix();
    bool validateInput();
    
    // Referenzen
    AuInit& m_auinit;
    Radix& m_radix;
    
    // Eingabefelder (Port von E_PERSON Dialog)
    QLineEdit* m_vornameEdit;      // ETXT_Vorname
    QLineEdit* m_nameEdit;         // ETXT_Name
    QLineEdit* m_ortEdit;          // ETXT_Ort
    QLineEdit* m_berufEdit;        // ETXT_Beruf
    QDateEdit* m_datumEdit;        // ETXT_Datum
    QTimeEdit* m_zeitEdit;         // ETXT_Zeit
    QDoubleSpinBox* m_sommerEdit;  // ETXT_Sommer
    QDoubleSpinBox* m_zoneEdit;    // TXT_Zeitzone
    QDoubleSpinBox* m_laengeEdit;  // TXT_Lange
    QDoubleSpinBox* m_breiteEdit;  // TXT_Breite
    QLineEdit* m_landEdit;         // TXT_LAND
    
    // Flag ob berechnet wurde
    bool m_calculated = false;
};

} // namespace astro
