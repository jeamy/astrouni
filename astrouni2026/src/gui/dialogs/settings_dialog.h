#pragma once
/**
 * @file settings_dialog.h
 * @brief Aspekte-Einstellungen Dialog
 * 
 * Port von DlgAspekte aus legacy/astrofil.c
 * Im Legacy heißt der Menüpunkt "Einstellungen" aber der Dialog ist "EINSTELL"
 * und zeigt Aspekt-Auswahl Checkboxen.
 */

#include <QDialog>
#include <QCheckBox>
#include "../../core/data_types.h"

namespace astro {

class SettingsDialog : public QDialog {
    Q_OBJECT
public:
    explicit SettingsDialog(AuInit& auinit, QWidget* parent = nullptr);
    
private slots:
    void onAccept();
    void onDefault();
    
private:
    void setupUI();
    void loadSettings();
    void saveSettings();
    
    AuInit& m_auinit;
    
    // Aspekte
    QCheckBox* m_konCheck;      // Konjunktion
    QCheckBox* m_hsxCheck;      // Halbsextil
    QCheckBox* m_sexCheck;      // Sextil
    QCheckBox* m_quaCheck;      // Quadrat
    QCheckBox* m_triCheck;      // Trigon
    QCheckBox* m_quiCheck;      // Quincunx
    QCheckBox* m_opoCheck;      // Opposition
    
    // Teilung
    QCheckBox* m_3erCheck;      // 3er Teilung
    QCheckBox* m_9erCheck;      // 9er Teilung
    
    // Objekte
    QCheckBox* m_planCheck;     // Planeten
    QCheckBox* m_asterCheck;    // Asteroiden
    QCheckBox* m_hausCheck;     // Häuser
};

} // namespace astro
