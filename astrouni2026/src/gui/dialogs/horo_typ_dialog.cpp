/**
 * @file horo_typ_dialog.cpp
 * @brief Implementierung des Horoskoptyp-Dialogs
 * 
 * Port von DlgHoroAuswahl aus legacy/astrofil.c
 */

#include "horo_typ_dialog.h"
#include "../../core/constants.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QLabel>

namespace astro {

Horo_typDialog::Horo_typDialog(AuInit& auinit, QWidget* parent)
    : QDialog(parent)
    , m_auinit(auinit) {
    
    setWindowTitle(tr("Horoskoptyp"));
    setModal(true);
    
    setupUI();
    loadSettings();
}

void Horo_typDialog::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSizeConstraint(QLayout::SetFixedSize); // Dialog auf Inhalt anpassen, damit alle Häusersysteme sichtbar sind
    
    // Horoskoptyp-Gruppe (STRICT LEGACY: nur Radix, Synastrie, Transit)
    QGroupBox* horoGroup = new QGroupBox(tr("Horoskoptyp"), this);
    QVBoxLayout* horoLayout = new QVBoxLayout(horoGroup);
    
    m_horoGroup = new QButtonGroup(this);
    
    m_radixRadio = new QRadioButton(tr("Radix"), this);
    m_synastrieRadio = new QRadioButton(tr("Synastrie"), this);
    m_transitRadio = new QRadioButton(tr("Transit"), this);
    
    m_horoGroup->addButton(m_radixRadio, TYP_RADIX);
    m_horoGroup->addButton(m_synastrieRadio, TYP_SYNASTRIE);
    m_horoGroup->addButton(m_transitRadio, TYP_TRANSIT);
    
    horoLayout->addWidget(m_radixRadio);
    horoLayout->addWidget(m_synastrieRadio);
    horoLayout->addWidget(m_transitRadio);
    
    mainLayout->addWidget(horoGroup);
    
    // Häusersystem-Gruppe
    QGroupBox* hausGroup = new QGroupBox(tr("Häusersystem"), this);
    QVBoxLayout* hausLayout = new QVBoxLayout(hausGroup);
    
    m_hausGroup = new QButtonGroup(this);
    
    m_placidusRadio = new QRadioButton(tr("Placidus"), this);
    m_kochRadio = new QRadioButton(tr("Koch"), this);
    m_equalRadio = new QRadioButton(tr("Äqual (ASC)"), this);
    m_equalMcRadio = new QRadioButton(tr("Äqual (MC)"), this);
    m_wholeRadio = new QRadioButton(tr("Ganze Zeichen"), this);
    m_topocentricRadio = new QRadioButton(tr("Topozentrisch"), this);
    m_campanoRadio = new QRadioButton(tr("Campanus"), this);
    m_meridianRadio = new QRadioButton(tr("Meridian"), this);
    m_regiomontanRadio = new QRadioButton(tr("Regiomontanus"), this);
    m_porphyryRadio = new QRadioButton(tr("Porphyry"), this);
    m_porphyryNewRadio = new QRadioButton(tr("Porphyry (neu)"), this);
    m_morinusRadio = new QRadioButton(tr("Morinus"), this);
    m_alcabitiusRadio = new QRadioButton(tr("Alcabitius"), this);
    m_nullRadio = new QRadioButton(tr("Keine Häuser"), this);
    
    m_hausGroup->addButton(m_kochRadio, TYP_KOCH);
    m_hausGroup->addButton(m_placidusRadio, TYP_PLACIDUS);
    m_hausGroup->addButton(m_equalRadio, TYP_EQUAL);
    m_hausGroup->addButton(m_equalMcRadio, TYP_EQUALMID);
    m_hausGroup->addButton(m_wholeRadio, TYP_WHOLE);
    m_hausGroup->addButton(m_topocentricRadio, TYP_TOPOZEN);
    m_hausGroup->addButton(m_campanoRadio, TYP_CAMPANUS);
    m_hausGroup->addButton(m_meridianRadio, TYP_MERIDIAN);
    m_hausGroup->addButton(m_regiomontanRadio, TYP_REGIOMONTANUS);
    m_hausGroup->addButton(m_porphyryRadio, TYP_PORPHYRY);
    m_hausGroup->addButton(m_porphyryNewRadio, TYP_PORPHYRYN);
    m_hausGroup->addButton(m_morinusRadio, TYP_MORINUS);
    m_hausGroup->addButton(m_alcabitiusRadio, TYP_ALCABIT);
    m_hausGroup->addButton(m_nullRadio, TYP_NULL);
    
    hausLayout->addWidget(m_kochRadio);
    hausLayout->addWidget(m_placidusRadio);
    hausLayout->addWidget(m_equalRadio);
    hausLayout->addWidget(m_equalMcRadio);
    hausLayout->addWidget(m_wholeRadio);
    hausLayout->addWidget(m_topocentricRadio);
    hausLayout->addWidget(m_campanoRadio);
    hausLayout->addWidget(m_meridianRadio);
    hausLayout->addWidget(m_regiomontanRadio);
    hausLayout->addWidget(m_porphyryRadio);
    hausLayout->addWidget(m_porphyryNewRadio);
    hausLayout->addWidget(m_morinusRadio);
    hausLayout->addWidget(m_alcabitiusRadio);
    hausLayout->addWidget(m_nullRadio);
    
    mainLayout->addWidget(hausGroup);
    
    // Buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    
    QPushButton* okButton = new QPushButton(tr("OK"), this);
    okButton->setDefault(true);
    connect(okButton, &QPushButton::clicked, this, &Horo_typDialog::onAccept);
    buttonLayout->addWidget(okButton);
    
    QPushButton* cancelButton = new QPushButton(tr("Abbrechen"), this);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    buttonLayout->addWidget(cancelButton);
    
    mainLayout->addLayout(buttonLayout);
}

void Horo_typDialog::loadSettings() {
    // Horoskoptyp
    QAbstractButton* horoButton = m_horoGroup->button(m_auinit.sSelHoro);
    if (horoButton) horoButton->setChecked(true);
    else m_radixRadio->setChecked(true);
    
    // Häusersystem
    QAbstractButton* hausButton = m_hausGroup->button(m_auinit.sSelHaus);
    if (hausButton) hausButton->setChecked(true);
    else m_placidusRadio->setChecked(true);
}

void Horo_typDialog::saveSettings() {
    m_auinit.sSelHoro = static_cast<int16_t>(m_horoGroup->checkedId());
    m_auinit.sSelHaus = static_cast<int16_t>(m_hausGroup->checkedId());
}

int16_t Horo_typDialog::getSelectedHoroTyp() const {
    return static_cast<int16_t>(m_horoGroup->checkedId());
}

int16_t Horo_typDialog::getSelectedHausTyp() const {
    return static_cast<int16_t>(m_hausGroup->checkedId());
}

void Horo_typDialog::onAccept() {
    saveSettings();
    accept();
}

} // namespace astro
