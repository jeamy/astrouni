/**
 * @file ort_dialog.cpp
 * @brief Implementierung des Ort-Erfassen Dialogs
 */

#include "ort_dialog.h"
#include "../../data/orte_db.h"
#include "ort_duplikat_dialog.h"

#include <QVBoxLayout>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QLabel>
#include <QMessageBox>

namespace astro {

OrtDialog::OrtDialog(QWidget* parent)
    : QDialog(parent)
    , m_isNew(true) {
    
    setWindowTitle(tr("Ort erfassen"));
    setModal(true);
    
    setupUI();
}

void OrtDialog::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // Ort-Daten
    QGroupBox* ortGroup = new QGroupBox(tr("Ortsdaten"), this);
    QFormLayout* formLayout = new QFormLayout(ortGroup);
    
    m_nameEdit = new QLineEdit(this);
    formLayout->addRow(tr("Ortsname:"), m_nameEdit);
    
    m_landEdit = new QLineEdit(this);
    m_landEdit->setMaxLength(3);
    formLayout->addRow(tr("Land (Kürzel):"), m_landEdit);
    
    m_laengeEdit = new QDoubleSpinBox(this);
    m_laengeEdit->setRange(-180.0, 180.0);
    m_laengeEdit->setDecimals(4);
    m_laengeEdit->setSuffix("° (Ost +, West -)");
    formLayout->addRow(tr("Länge:"), m_laengeEdit);
    
    m_breiteEdit = new QDoubleSpinBox(this);
    m_breiteEdit->setRange(-90.0, 90.0);
    m_breiteEdit->setDecimals(4);
    m_breiteEdit->setSuffix("° (Nord +, Süd -)");
    formLayout->addRow(tr("Breite:"), m_breiteEdit);
    
    m_zoneEdit = new QDoubleSpinBox(this);
    m_zoneEdit->setRange(-12.0, 12.0);
    m_zoneEdit->setSingleStep(0.5);
    m_zoneEdit->setDecimals(1);
    m_zoneEdit->setSuffix(" h");
    m_zoneEdit->setValue(1.0);  // MEZ als Standard
    formLayout->addRow(tr("Zeitzone:"), m_zoneEdit);
    
    mainLayout->addWidget(ortGroup);
    
    // Buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    
    QPushButton* okButton = new QPushButton(tr("Speichern"), this);
    okButton->setDefault(true);
    connect(okButton, &QPushButton::clicked, this, &OrtDialog::onAccept);
    buttonLayout->addWidget(okButton);
    
    QPushButton* cancelButton = new QPushButton(tr("Abbrechen"), this);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    buttonLayout->addWidget(cancelButton);
    
    mainLayout->addLayout(buttonLayout);
    
    setMinimumWidth(350);
}

void OrtDialog::setOrt(const Orte& ort) {
    m_ort = ort;
    m_isNew = false;
    
    m_nameEdit->setText(ort.name);
    m_landEdit->setText(ort.land);
    m_laengeEdit->setValue(ort.laenge);
    m_breiteEdit->setValue(ort.breite);
    m_zoneEdit->setValue(ort.zone);
    
    setWindowTitle(tr("Ort bearbeiten"));
}

Orte OrtDialog::getOrt() const {
    return m_ort;
}

bool OrtDialog::validateInput() {
    if (m_nameEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, tr("Fehler"), tr("Bitte Ortsnamen eingeben."));
        m_nameEdit->setFocus();
        return false;
    }
    
    if (m_landEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, tr("Fehler"), tr("Bitte Länderkürzel eingeben (z.B. D, A, CH)."));
        m_landEdit->setFocus();
        return false;
    }
    
    return true;
}

void OrtDialog::onAccept() {
    if (!validateInput()) return;
    
    QString name = m_nameEdit->text().trimmed();
    
    // Prüfen ob Ort bereits existiert (nur bei neuen Orten)
    if (m_isNew) {
        Orte existing = orteDB().getByName(name);
        if (existing.gultig) {
            // Duplikat-Dialog anbieten
            OrtDuplikatDialog dupDlg(this, existing);
            if (dupDlg.exec() != QDialog::Accepted || !dupDlg.useExisting()) {
                m_nameEdit->setFocus();
                return;
            }
            // Vorhandenen Ort übernehmen und akzeptieren
            m_ort = existing;
            accept();
            return;
        }
    }
    
    // Ort-Daten übernehmen
    m_ort.name = name;
    m_ort.land = m_landEdit->text().trimmed().toUpper();
    m_ort.laenge = m_laengeEdit->value();
    m_ort.breite = m_breiteEdit->value();
    m_ort.zone = static_cast<float>(m_zoneEdit->value());
    m_ort.gultig = true;
    
    // In Datenbank speichern
    if (m_isNew) {
        int index = orteDB().add(m_ort);
        if (index >= 0) {
            m_ort.index = index;
            orteDB().save();  // Sofort speichern
            accept();
        } else {
            QMessageBox::warning(this, tr("Fehler"), tr("Fehler beim Speichern des Ortes."));
        }
    } else {
        if (orteDB().update(m_ort.index, m_ort)) {
            orteDB().save();
            accept();
        } else {
            QMessageBox::warning(this, tr("Fehler"), tr("Fehler beim Aktualisieren des Ortes."));
        }
    }
}

} // namespace astro
