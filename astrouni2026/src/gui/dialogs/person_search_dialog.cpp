/**
 * @file person_search_dialog.cpp
 * @brief Implementierung des Personen-Suche Dialogs
 * 
 * Port von DlgGetName aus legacy/astrofil.c
 */

#include "person_search_dialog.h"
#include "../../data/person_db.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QCoreApplication>

namespace astro {

PersonSearchDialog::PersonSearchDialog(QWidget* parent)
    : QDialog(parent)
    , m_hasSelection(false) {
    
    setWindowTitle(tr("Person suchen"));
    setModal(true);
    
    setupUI();
    loadInitialPersons();
}

void PersonSearchDialog::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // Suchfeld
    QHBoxLayout* searchLayout = new QHBoxLayout();
    searchLayout->addWidget(new QLabel(tr("Suche:"), this));
    
    m_searchEdit = new QLineEdit(this);
    m_searchEdit->setPlaceholderText(tr("Name oder Vorname eingeben..."));
    connect(m_searchEdit, &QLineEdit::textChanged,
            this, &PersonSearchDialog::onSearchTextChanged);
    searchLayout->addWidget(m_searchEdit);
    
    mainLayout->addLayout(searchLayout);
    
    // Liste
    m_listWidget = new QListWidget(this);
    connect(m_listWidget, &QListWidget::itemDoubleClicked,
            this, &PersonSearchDialog::onItemDoubleClicked);
    connect(m_listWidget, &QListWidget::itemSelectionChanged,
            this, &PersonSearchDialog::onItemSelectionChanged);
    mainLayout->addWidget(m_listWidget);
    
    // Buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    
    m_deleteButton = new QPushButton(tr("Löschen"), this);
    m_deleteButton->setEnabled(false);
    connect(m_deleteButton, &QPushButton::clicked, this, &PersonSearchDialog::onDeleteClicked);
    buttonLayout->addWidget(m_deleteButton);
    
    buttonLayout->addStretch();
    
    m_okButton = new QPushButton(tr("OK"), this);
    m_okButton->setEnabled(false);
    m_okButton->setDefault(true);
    connect(m_okButton, &QPushButton::clicked, this, &PersonSearchDialog::onOkClicked);
    buttonLayout->addWidget(m_okButton);
    
    m_cancelButton = new QPushButton(tr("Abbrechen"), this);
    connect(m_cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    buttonLayout->addWidget(m_cancelButton);
    
    mainLayout->addLayout(buttonLayout);
    
    // Dialog-Größe
    resize(450, 400);
    
    // Fokus auf Suchfeld
    m_searchEdit->setFocus();
}

void PersonSearchDialog::loadInitialPersons() {
    // Datenbank laden falls noch nicht geschehen
    if (!personDB().isLoaded()) {
        QString dataPath = QCoreApplication::applicationDirPath() + "/data";
        personDB().load(dataPath);
    }
    
    // Erste 100 Personen anzeigen
    QVector<RadixFix> persons = personDB().getAll();
    
    m_listWidget->clear();
    int count = 0;
    for (const RadixFix& person : persons) {
        if (count >= 100) break;
        
        QString displayText = QString("%1, %2 (%3.%4.%5)")
            .arg(person.name)
            .arg(person.vorname)
            .arg(person.tag, 2, 10, QChar('0'))
            .arg(person.monat, 2, 10, QChar('0'))
            .arg(person.jahr);
        
        QListWidgetItem* item = new QListWidgetItem(displayText, m_listWidget);
        item->setData(Qt::UserRole, person.indexRadix);
        count++;
    }
}

void PersonSearchDialog::onSearchTextChanged(const QString& text) {
    updateList(text);
}

void PersonSearchDialog::updateList(const QString& searchText) {
    m_listWidget->clear();
    
    QVector<RadixFix> results;
    
    if (searchText.isEmpty()) {
        // Alle Personen (max 100)
        results = personDB().getAll();
    } else {
        // Suche
        results = personDB().search(searchText, 100);
    }
    
    for (const RadixFix& person : results) {
        QString displayText = QString("%1, %2 (%3.%4.%5)")
            .arg(person.name)
            .arg(person.vorname)
            .arg(person.tag, 2, 10, QChar('0'))
            .arg(person.monat, 2, 10, QChar('0'))
            .arg(person.jahr);
        
        QListWidgetItem* item = new QListWidgetItem(displayText, m_listWidget);
        item->setData(Qt::UserRole, person.indexRadix);
    }
}

void PersonSearchDialog::onItemDoubleClicked(QListWidgetItem* item) {
    if (item) {
        onOkClicked();
    }
}

void PersonSearchDialog::onItemSelectionChanged() {
    bool hasSelection = m_listWidget->currentItem() != nullptr;
    m_okButton->setEnabled(hasSelection);
    m_deleteButton->setEnabled(hasSelection);
}

void PersonSearchDialog::onDeleteClicked() {
    QListWidgetItem* item = m_listWidget->currentItem();
    if (!item) {
        return;
    }
    
    int index = item->data(Qt::UserRole).toInt();
    QString personName = item->text();
    
    // Bestätigung
    QMessageBox::StandardButton reply = QMessageBox::question(
        this, 
        tr("Person löschen"),
        tr("Möchten Sie '%1' wirklich löschen?").arg(personName),
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No);
    
    if (reply == QMessageBox::Yes) {
        // Person aus Datenbank löschen
        if (personDB().remove(index)) {
            personDB().save();
            
            // Liste aktualisieren
            updateList(m_searchEdit->text());
            
            QMessageBox::information(this, tr("Gelöscht"), 
                tr("Person wurde gelöscht."));
        } else {
            QMessageBox::warning(this, tr("Fehler"), 
                tr("Person konnte nicht gelöscht werden."));
        }
    }
}

void PersonSearchDialog::onOkClicked() {
    QListWidgetItem* item = m_listWidget->currentItem();
    if (!item) {
        return;
    }
    
    int index = item->data(Qt::UserRole).toInt();
    
    // Person aus Datenbank laden
    QVector<RadixFix> all = personDB().getAll();
    for (const RadixFix& person : all) {
        if (person.indexRadix == index) {
            m_selectedPerson = person;
            m_hasSelection = true;
            accept();
            return;
        }
    }
    
    QMessageBox::warning(this, tr("Fehler"), tr("Person nicht gefunden."));
}

RadixFix PersonSearchDialog::getSelectedPerson() const {
    return m_selectedPerson;
}

bool PersonSearchDialog::hasSelection() const {
    return m_hasSelection;
}

} // namespace astro
