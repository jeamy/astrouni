#pragma once
/**
 * @file horo_typ_dialog.h
 * @brief Horoskoptyp-Auswahl Dialog
 * 
 * Port von DlgHoroAuswahl aus legacy/astrofil.c
 */

#include <QDialog>
#include <QRadioButton>
#include <QButtonGroup>
#include "../../core/data_types.h"

namespace astro {

class Horo_typDialog : public QDialog {
    Q_OBJECT
public:
    explicit Horo_typDialog(AuInit& auinit, QWidget* parent = nullptr);
    
    int16_t getSelectedHoroTyp() const;
    int16_t getSelectedHausTyp() const;
    
private slots:
    void onAccept();
    
private:
    void setupUI();
    void loadSettings();
    void saveSettings();
    
    AuInit& m_auinit;
    
    // Horoskoptyp
    QButtonGroup* m_horoGroup;
    QRadioButton* m_radixRadio;
    QRadioButton* m_solarRadio;
    QRadioButton* m_lunarRadio;
    
    // Häusersystem
    QButtonGroup* m_hausGroup;
    QRadioButton* m_placidusRadio;
    QRadioButton* m_kochRadio;
    QRadioButton* m_regiomontanRadio;
    QRadioButton* m_campanoRadio;
    QRadioButton* m_equalRadio;
    QRadioButton* m_wholeRadio;
};

} // namespace astro
