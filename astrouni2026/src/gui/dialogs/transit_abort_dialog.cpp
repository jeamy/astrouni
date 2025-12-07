#include "transit_abort_dialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>

namespace astro {

TransitAbortDialog::TransitAbortDialog(QWidget* parent)
    : QDialog(parent)
    , m_vonLabel(new QLabel(this))
    , m_bisLabel(new QLabel(this))
    , m_aktLabel(new QLabel(this)) {
    setWindowTitle(tr("Transite"));
    setModal(true);

    auto* layout = new QVBoxLayout(this);

    auto* vonLayout = new QHBoxLayout();
    vonLayout->addWidget(new QLabel(tr("Von:"), this));
    vonLayout->addWidget(m_vonLabel);
    layout->addLayout(vonLayout);

    auto* bisLayout = new QHBoxLayout();
    bisLayout->addWidget(new QLabel(tr("Bis:"), this));
    bisLayout->addWidget(m_bisLabel);
    layout->addLayout(bisLayout);

    auto* aktLayout = new QHBoxLayout();
    aktLayout->addWidget(new QLabel(tr("Aktuell:"), this));
    aktLayout->addWidget(m_aktLabel);
    layout->addLayout(aktLayout);

    auto* buttons = new QHBoxLayout();
    buttons->addStretch();
    auto* cancel = new QPushButton(tr("Abbruch"), this);
    connect(cancel, &QPushButton::clicked, this, &QDialog::reject);
    buttons->addWidget(cancel);
    layout->addLayout(buttons);

    setFixedSize(320, 160);
}

void TransitAbortDialog::setVon(const QString& text) {
    m_vonLabel->setText(text);
}

void TransitAbortDialog::setBis(const QString& text) {
    m_bisLabel->setText(text);
}

void TransitAbortDialog::setAkt(const QString& text) {
    m_aktLabel->setText(text);
}

} // namespace astro
