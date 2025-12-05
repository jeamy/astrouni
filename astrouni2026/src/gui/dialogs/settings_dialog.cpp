/**
 * @file settings_dialog.cpp
 * @brief Implementierung des Aspekte-Einstellungen Dialogs
 * 
 * Port von DlgAspekte aus legacy/astrofil.c
 */

#include "settings_dialog.h"
#include "../../core/constants.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QPushButton>

namespace astro {

SettingsDialog::SettingsDialog(AuInit& auinit, QWidget* parent)
    : QDialog(parent)
    , m_auinit(auinit) {
    
    setWindowTitle(tr("Einstellungen"));
    setModal(true);
    
    setupUI();
    loadSettings();
}

void SettingsDialog::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // Aspekte-Gruppe
    QGroupBox* aspektGroup = new QGroupBox(tr("Aspekte"), this);
    QVBoxLayout* aspektLayout = new QVBoxLayout(aspektGroup);
    
    m_konCheck = new QCheckBox(tr("Konjunktion (0°)"), this);
    m_hsxCheck = new QCheckBox(tr("Halbsextil (30°)"), this);
    m_sexCheck = new QCheckBox(tr("Sextil (60°)"), this);
    m_quaCheck = new QCheckBox(tr("Quadrat (90°)"), this);
    m_triCheck = new QCheckBox(tr("Trigon (120°)"), this);
    m_quiCheck = new QCheckBox(tr("Quincunx (150°)"), this);
    m_opoCheck = new QCheckBox(tr("Opposition (180°)"), this);
    
    aspektLayout->addWidget(m_konCheck);
    aspektLayout->addWidget(m_hsxCheck);
    aspektLayout->addWidget(m_sexCheck);
    aspektLayout->addWidget(m_quaCheck);
    aspektLayout->addWidget(m_triCheck);
    aspektLayout->addWidget(m_quiCheck);
    aspektLayout->addWidget(m_opoCheck);
    
    mainLayout->addWidget(aspektGroup);
    
    // Teilung-Gruppe
    QGroupBox* teilungGroup = new QGroupBox(tr("Teilung"), this);
    QHBoxLayout* teilungLayout = new QHBoxLayout(teilungGroup);
    
    m_3erCheck = new QCheckBox(tr("3er Teilung"), this);
    m_9erCheck = new QCheckBox(tr("9er Teilung"), this);
    
    teilungLayout->addWidget(m_3erCheck);
    teilungLayout->addWidget(m_9erCheck);
    teilungLayout->addStretch();
    
    mainLayout->addWidget(teilungGroup);
    
    // Objekte-Gruppe
    QGroupBox* objektGroup = new QGroupBox(tr("Objekte"), this);
    QHBoxLayout* objektLayout = new QHBoxLayout(objektGroup);
    
    m_planCheck = new QCheckBox(tr("Planeten"), this);
    m_asterCheck = new QCheckBox(tr("Asteroiden"), this);
    m_hausCheck = new QCheckBox(tr("Häuser"), this);
    
    objektLayout->addWidget(m_planCheck);
    objektLayout->addWidget(m_asterCheck);
    objektLayout->addWidget(m_hausCheck);
    
    mainLayout->addWidget(objektGroup);
    
    // Buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    
    QPushButton* defaultButton = new QPushButton(tr("Standard"), this);
    connect(defaultButton, &QPushButton::clicked, this, &SettingsDialog::onDefault);
    buttonLayout->addWidget(defaultButton);
    
    buttonLayout->addStretch();
    
    QPushButton* okButton = new QPushButton(tr("OK"), this);
    okButton->setDefault(true);
    connect(okButton, &QPushButton::clicked, this, &SettingsDialog::onAccept);
    buttonLayout->addWidget(okButton);
    
    QPushButton* cancelButton = new QPushButton(tr("Abbrechen"), this);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    buttonLayout->addWidget(cancelButton);
    
    mainLayout->addLayout(buttonLayout);
}

void SettingsDialog::loadSettings() {
    m_konCheck->setChecked(m_auinit.sAspekte & S_KON);
    m_hsxCheck->setChecked(m_auinit.sAspekte & S_HAL);
    m_sexCheck->setChecked(m_auinit.sAspekte & S_SEX);
    m_quaCheck->setChecked(m_auinit.sAspekte & S_QUA);
    m_triCheck->setChecked(m_auinit.sAspekte & S_TRI);
    m_quiCheck->setChecked(m_auinit.sAspekte & S_QUI);
    m_opoCheck->setChecked(m_auinit.sAspekte & S_OPO);
    
    m_3erCheck->setChecked(m_auinit.sTeilung & S_3er);
    m_9erCheck->setChecked(m_auinit.sTeilung & S_9er);
    
    m_planCheck->setChecked(m_auinit.sAspekte & S_PLAN);
    m_asterCheck->setChecked(m_auinit.sAspekte & S_ASTER);
    m_hausCheck->setChecked(m_auinit.sAspekte & S_HAUS);
}

void SettingsDialog::saveSettings() {
    m_auinit.sAspekte = 0;
    if (m_konCheck->isChecked()) m_auinit.sAspekte |= S_KON;
    if (m_hsxCheck->isChecked()) m_auinit.sAspekte |= S_HAL;
    if (m_sexCheck->isChecked()) m_auinit.sAspekte |= S_SEX;
    if (m_quaCheck->isChecked()) m_auinit.sAspekte |= S_QUA;
    if (m_triCheck->isChecked()) m_auinit.sAspekte |= S_TRI;
    if (m_quiCheck->isChecked()) m_auinit.sAspekte |= S_QUI;
    if (m_opoCheck->isChecked()) m_auinit.sAspekte |= S_OPO;
    if (m_planCheck->isChecked()) m_auinit.sAspekte |= S_PLAN;
    if (m_asterCheck->isChecked()) m_auinit.sAspekte |= S_ASTER;
    if (m_hausCheck->isChecked()) m_auinit.sAspekte |= S_HAUS;
    
    m_auinit.sTeilung = 0;
    if (m_3erCheck->isChecked()) m_auinit.sTeilung |= S_3er;
    if (m_9erCheck->isChecked()) m_auinit.sTeilung |= S_9er;
}

void SettingsDialog::onDefault() {
    // Standard-Aspekte: Konjunktion, Sextil, Quadrat, Trigon, Opposition
    m_konCheck->setChecked(true);
    m_hsxCheck->setChecked(false);
    m_sexCheck->setChecked(true);
    m_quaCheck->setChecked(true);
    m_triCheck->setChecked(true);
    m_quiCheck->setChecked(false);
    m_opoCheck->setChecked(true);
    
    m_3erCheck->setChecked(false);
    m_9erCheck->setChecked(false);
    
    m_planCheck->setChecked(true);
    m_asterCheck->setChecked(false);
    m_hausCheck->setChecked(false);
}

void SettingsDialog::onAccept() {
    saveSettings();
    accept();
}

} // namespace astro
