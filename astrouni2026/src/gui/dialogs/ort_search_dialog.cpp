#include "ort_search_dialog.h"
#include "../../data/orte_db.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>

namespace astro {

OrtSearchDialog::OrtSearchDialog(QWidget* parent) : QDialog(parent) {
    setWindowTitle(tr("Ort suchen"));
    setupUI();
}

Orte OrtSearchDialog::getSelectedOrt() const {
    return m_selectedOrt;
}

void OrtSearchDialog::setupUI() {
    auto* layout = new QVBoxLayout(this);
    
    // Suchfeld
    auto* searchLayout = new QHBoxLayout();
    searchLayout->addWidget(new QLabel(tr("Ortsname:"), this));
    m_searchEdit = new QLineEdit(this);
    connect(m_searchEdit, &QLineEdit::textChanged, this, &OrtSearchDialog::onSearchTextChanged);
    searchLayout->addWidget(m_searchEdit);
    layout->addLayout(searchLayout);
    
    // Liste
    m_listWidget = new QListWidget(this);
    connect(m_listWidget, &QListWidget::itemDoubleClicked, this, &OrtSearchDialog::onItemDoubleClicked);
    layout->addWidget(m_listWidget);
    
    // Buttons
    auto* buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    auto* okButton = new QPushButton(tr("OK"), this);
    connect(okButton, &QPushButton::clicked, this, [this]() {
        if (m_listWidget->currentRow() >= 0) {
            accept();
        }
    });
    buttonLayout->addWidget(okButton);
    auto* cancelButton = new QPushButton(tr("Abbrechen"), this);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    buttonLayout->addWidget(cancelButton);
    layout->addLayout(buttonLayout);
    
    setMinimumSize(400, 300);
}

void OrtSearchDialog::onSearchTextChanged(const QString& text) {
    m_listWidget->clear();
    if (text.length() < 2) return;
    
    auto orte = orteDB().search(text, 50);
    for (const auto& ort : orte) {
        auto* item = new QListWidgetItem(QString("%1 (%2)").arg(ort.name, ort.land));
        item->setData(Qt::UserRole, ort.index);
        m_listWidget->addItem(item);
    }
}

void OrtSearchDialog::onItemDoubleClicked(QListWidgetItem* item) {
    int index = item->data(Qt::UserRole).toInt();
    m_selectedOrt = orteDB().getByIndex(index);
    accept();
}

} // namespace astro
