/**
 * @file retrograde_dialog.cpp
 * @brief Implementierung des Rückläufigkeits-Dialogs
 * 
 * 1:1 Port von DlgRuck aus legacy/autransi.c
 */

#include "retrograde_dialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>

namespace astro {

RetrogradeDialog::RetrogradeDialog(QWidget* parent)
    : QDialog(parent)
    , m_vonLabel(new QLabel(this))
    , m_bisLabel(new QLabel(this)) {
    
    setWindowTitle(tr("Rückläufigkeit"));
    setModal(true);
    
    auto* layout = new QVBoxLayout(this);
    
    // Von-Zeile (STRICT LEGACY: TA_VON)
    auto* vonLayout = new QHBoxLayout();
    vonLayout->addWidget(new QLabel(tr("Von:"), this));
    m_vonLabel->setMinimumWidth(100);
    vonLayout->addWidget(m_vonLabel);
    vonLayout->addStretch();
    layout->addLayout(vonLayout);
    
    // Bis-Zeile (STRICT LEGACY: TA_BIS)
    auto* bisLayout = new QHBoxLayout();
    bisLayout->addWidget(new QLabel(tr("Bis:"), this));
    m_bisLabel->setMinimumWidth(100);
    bisLayout->addWidget(m_bisLabel);
    bisLayout->addStretch();
    layout->addLayout(bisLayout);
    
    layout->addStretch();
    
    // OK-Button (STRICT LEGACY: PB_Ok)
    auto* buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    auto* okButton = new QPushButton(tr("OK"), this);
    okButton->setDefault(true);
    connect(okButton, &QPushButton::clicked, this, &QDialog::accept);
    buttonLayout->addWidget(okButton);
    layout->addLayout(buttonLayout);
    
    setFixedSize(250, 120);
}

void RetrogradeDialog::setDaten(const QString& von, const QString& bis) {
    m_vonLabel->setText(von.isEmpty() ? tr("N/A") : von);
    m_bisLabel->setText(bis.isEmpty() ? tr("N/A") : bis);
}

} // namespace astro
