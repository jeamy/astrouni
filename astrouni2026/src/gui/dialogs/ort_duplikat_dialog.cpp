/**
 * @file ort_duplikat_dialog.cpp
 * @brief Dialog bei Orts-Duplikat (Port von DlgODProc)
 */

#include "ort_duplikat_dialog.h"
#include "../../core/data_types.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>

namespace astro {

OrtDuplikatDialog::OrtDuplikatDialog(QWidget* parent, const Orte& existing)
    : QDialog(parent) {
    setWindowTitle(tr("Ort bereits vorhanden"));
    setModal(true);
    setupUI(existing);
}

void OrtDuplikatDialog::setupUI(const Orte& existing) {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    QLabel* info = new QLabel(tr("Ein Ort mit diesem Namen existiert bereits.\n"
                                 "Möchten Sie den vorhandenen Eintrag verwenden?"), this);
    mainLayout->addWidget(info);

    m_list = new QListWidget(this);
    QString itemText = QString("%1 (%2)  Läng: %3  Breit: %4  Zone: %5")
        .arg(existing.name)
        .arg(existing.land)
        .arg(existing.laenge, 0, 'f', 4)
        .arg(existing.breite, 0, 'f', 4)
        .arg(existing.zone, 0, 'f', 1);
    QListWidgetItem* item = new QListWidgetItem(itemText, m_list);
    item->setSelected(true);
    mainLayout->addWidget(m_list);

    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnLayout->addStretch();
    QPushButton* okBtn = new QPushButton(tr("Vorhandenen nutzen"), this);
    QPushButton* cancelBtn = new QPushButton(tr("Abbrechen"), this);
    connect(okBtn, &QPushButton::clicked, this, &OrtDuplikatDialog::onAccept);
    connect(cancelBtn, &QPushButton::clicked, this, &OrtDuplikatDialog::onReject);
    btnLayout->addWidget(okBtn);
    btnLayout->addWidget(cancelBtn);
    mainLayout->addLayout(btnLayout);

    setMinimumWidth(420);
}

void OrtDuplikatDialog::onAccept() {
    m_useExisting = true;
    accept();
}

void OrtDuplikatDialog::onReject() {
    m_useExisting = false;
    reject();
}

} // namespace astro
