/**
 * @file person_dialog.cpp
 * @brief Implementierung des Personen-Dialogs
 */

#include "person_dialog.h"
#include "ort_search_dialog.h"
#include "person_search_dialog.h"
#include "../../core/calculations.h"
#include "../../core/chart_calc.h"
#include "../../data/person_db.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QLabel>
#include <QMessageBox>

namespace astro {

//==============================================================================
// Konstruktor / Destruktor
//==============================================================================

PersonDialog::PersonDialog(QWidget* parent, AuInit& auinit, Radix& radix)
    : QDialog(parent)
    , m_auinit(auinit)
    , m_radix(radix) {
    
    setWindowTitle(tr("Person Erfassen"));
    setModal(true);
    
    setupUI();
    loadFromRadix();
}

PersonDialog::~PersonDialog() {
}

//==============================================================================
// UI-Setup
//==============================================================================

void PersonDialog::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // Personen-Daten
    QGroupBox* personGroup = new QGroupBox(tr("Person"), this);
    QFormLayout* personLayout = new QFormLayout(personGroup);
    
    // Vorname
    QHBoxLayout* vornameLayout = new QHBoxLayout();
    m_vornameEdit = new QLineEdit(this);
    vornameLayout->addWidget(m_vornameEdit);
    QPushButton* suchenButton = new QPushButton(tr("Suchen..."), this);
    connect(suchenButton, &QPushButton::clicked, this, &PersonDialog::onNamenSuchen);
    vornameLayout->addWidget(suchenButton);
    personLayout->addRow(tr("Vorname:"), vornameLayout);
    
    // Name
    m_nameEdit = new QLineEdit(this);
    personLayout->addRow(tr("Name:"), m_nameEdit);
    
    // Beruf
    m_berufEdit = new QLineEdit(this);
    personLayout->addRow(tr("Beruf:"), m_berufEdit);
    
    mainLayout->addWidget(personGroup);
    
    // Geburts-Daten
    QGroupBox* geburtGroup = new QGroupBox(tr("Geburtsdaten"), this);
    QFormLayout* geburtLayout = new QFormLayout(geburtGroup);
    
    // Datum
    m_datumEdit = new QDateEdit(this);
    m_datumEdit->setDisplayFormat("dd.MM.yyyy");
    m_datumEdit->setCalendarPopup(true);
    geburtLayout->addRow(tr("Datum:"), m_datumEdit);
    
    // Zeit
    m_zeitEdit = new QTimeEdit(this);
    m_zeitEdit->setDisplayFormat("HH:mm");
    geburtLayout->addRow(tr("Zeit:"), m_zeitEdit);
    
    // Sommerzeit
    m_sommerEdit = new QDoubleSpinBox(this);
    m_sommerEdit->setRange(-2.0, 2.0);
    m_sommerEdit->setSingleStep(1.0);
    m_sommerEdit->setDecimals(1);
    m_sommerEdit->setSuffix(" h");
    geburtLayout->addRow(tr("Sommerzeit:"), m_sommerEdit);
    
    mainLayout->addWidget(geburtGroup);
    
    // Ort-Daten
    QGroupBox* ortGroup = new QGroupBox(tr("Geburtsort"), this);
    QFormLayout* ortLayout = new QFormLayout(ortGroup);
    
    // Ort
    QHBoxLayout* ortEditLayout = new QHBoxLayout();
    m_ortEdit = new QLineEdit(this);
    ortEditLayout->addWidget(m_ortEdit);
    QPushButton* ortSuchenButton = new QPushButton(tr("Suchen..."), this);
    connect(ortSuchenButton, &QPushButton::clicked, this, &PersonDialog::onOrtSuchen);
    ortEditLayout->addWidget(ortSuchenButton);
    ortLayout->addRow(tr("Ort:"), ortEditLayout);
    
    // Land
    m_landEdit = new QLineEdit(this);
    m_landEdit->setMaxLength(3);
    ortLayout->addRow(tr("Land:"), m_landEdit);
    
    // Koordinaten
    m_laengeEdit = new QDoubleSpinBox(this);
    m_laengeEdit->setRange(-180.0, 180.0);
    m_laengeEdit->setDecimals(4);
    m_laengeEdit->setSuffix("°");
    ortLayout->addRow(tr("Länge:"), m_laengeEdit);
    
    m_breiteEdit = new QDoubleSpinBox(this);
    m_breiteEdit->setRange(-90.0, 90.0);
    m_breiteEdit->setDecimals(4);
    m_breiteEdit->setSuffix("°");
    ortLayout->addRow(tr("Breite:"), m_breiteEdit);
    
    // Zeitzone
    m_zoneEdit = new QDoubleSpinBox(this);
    m_zoneEdit->setRange(-12.0, 12.0);
    m_zoneEdit->setSingleStep(0.5);
    m_zoneEdit->setDecimals(1);
    m_zoneEdit->setSuffix(" h");
    ortLayout->addRow(tr("Zeitzone:"), m_zoneEdit);
    
    mainLayout->addWidget(ortGroup);
    
    // Buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    
    QPushButton* calcButton = new QPushButton(tr("Berechnen"), this);
    connect(calcButton, &QPushButton::clicked, this, &PersonDialog::onCalc);
    buttonLayout->addWidget(calcButton);
    
    buttonLayout->addStretch();
    
    QPushButton* okButton = new QPushButton(tr("OK"), this);
    okButton->setDefault(true);
    connect(okButton, &QPushButton::clicked, this, &PersonDialog::onAccept);
    buttonLayout->addWidget(okButton);
    
    QPushButton* cancelButton = new QPushButton(tr("Abbrechen"), this);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    buttonLayout->addWidget(cancelButton);
    
    mainLayout->addLayout(buttonLayout);
    
    // Dialog-Größe
    setMinimumWidth(400);
}

//==============================================================================
// Daten laden/speichern
//==============================================================================

void PersonDialog::loadFromRadix() {
    m_vornameEdit->setText(m_radix.rFix.vorname);
    m_nameEdit->setText(m_radix.rFix.name);
    m_berufEdit->setText(m_radix.rFix.beruf);
    m_ortEdit->setText(m_radix.rFix.ort);
    m_landEdit->setText(m_radix.rFix.land);
    
    m_datumEdit->setDate(QDate(m_radix.rFix.jahr, m_radix.rFix.monat, m_radix.rFix.tag));
    
    int16_t stunden, minuten;
    Calculations::decimalToTime(m_radix.rFix.zeit, stunden, minuten);
    m_zeitEdit->setTime(QTime(stunden, minuten));
    
    m_sommerEdit->setValue(m_radix.rFix.sommerzeit);
    m_zoneEdit->setValue(m_radix.rFix.zone);
    m_laengeEdit->setValue(m_radix.rFix.laenge);
    m_breiteEdit->setValue(m_radix.rFix.breite);
}

void PersonDialog::saveToRadix() {
    m_radix.rFix.vorname = m_vornameEdit->text();
    m_radix.rFix.name = m_nameEdit->text();
    m_radix.rFix.beruf = m_berufEdit->text();
    m_radix.rFix.ort = m_ortEdit->text();
    m_radix.rFix.land = m_landEdit->text();
    
    QDate date = m_datumEdit->date();
    m_radix.rFix.tag = date.day();
    m_radix.rFix.monat = date.month();
    m_radix.rFix.jahr = date.year();
    
    QTime time = m_zeitEdit->time();
    m_radix.rFix.zeit = Calculations::timeToDecimal(time.hour(), time.minute());
    
    m_radix.rFix.sommerzeit = m_sommerEdit->value();
    m_radix.rFix.zone = m_zoneEdit->value();
    m_radix.rFix.laenge = m_laengeEdit->value();
    m_radix.rFix.breite = m_breiteEdit->value();
    
    m_radix.hausSys = m_auinit.sSelHaus;
    m_radix.horoTyp = m_auinit.sSelHoro;
}

bool PersonDialog::validateInput() {
    if (m_nameEdit->text().isEmpty() && m_vornameEdit->text().isEmpty()) {
        QMessageBox::warning(this, tr("Fehler"), tr("Bitte Name oder Vorname eingeben."));
        m_nameEdit->setFocus();
        return false;
    }
    
    if (!m_datumEdit->date().isValid()) {
        QMessageBox::warning(this, tr("Fehler"), tr("Ungültiges Datum."));
        m_datumEdit->setFocus();
        return false;
    }
    
    return true;
}

//==============================================================================
// Slots
//==============================================================================

void PersonDialog::onNamenSuchen() {
    PersonSearchDialog dialog(this);
    
    if (dialog.exec() == QDialog::Accepted && dialog.hasSelection()) {
        RadixFix person = dialog.getSelectedPerson();
        
        // Felder füllen
        m_vornameEdit->setText(person.vorname);
        m_nameEdit->setText(person.name);
        m_berufEdit->setText(person.beruf);
        m_ortEdit->setText(person.ort);
        m_landEdit->setText(person.land);
        
        m_datumEdit->setDate(QDate(person.jahr, person.monat, person.tag));
        
        int16_t stunden, minuten;
        Calculations::decimalToTime(person.zeit, stunden, minuten);
        m_zeitEdit->setTime(QTime(stunden, minuten));
        
        m_sommerEdit->setValue(person.sommerzeit);
        m_zoneEdit->setValue(person.zone);
        m_laengeEdit->setValue(person.laenge);
        m_breiteEdit->setValue(person.breite);
    }
}

void PersonDialog::onOrtSuchen() {
    OrtSearchDialog dialog(this);
    
    if (dialog.exec() == QDialog::Accepted) {
        Orte ort = dialog.getSelectedOrt();
        m_ortEdit->setText(ort.name);
        m_landEdit->setText(ort.land);
        m_laengeEdit->setValue(ort.laenge);
        m_breiteEdit->setValue(ort.breite);
        m_zoneEdit->setValue(ort.zone);
    }
}

void PersonDialog::onCalc() {
    if (!validateInput()) return;
    
    saveToRadix();
    
    // Horoskop berechnen
    int result = ChartCalc::calculate(m_radix, nullptr, m_auinit.sSelHoro);
    
    if (result == ERR_OK) {
        // Aspekte berechnen
        ChartCalc::calcAspects(m_radix, m_auinit.orbenPlanet);
        ChartCalc::calcAngles(m_radix, nullptr, m_auinit.sSelHoro);
        
        // Person in Datenbank speichern
        personDB().add(m_radix);
        personDB().save();
        
        // Flag setzen dass berechnet wurde
        m_calculated = true;
        
        // Dialog schließen und Radix-Fenster öffnen
        accept();
    } else {
        QMessageBox::warning(this, tr("Fehler"), 
            tr("Fehler bei der Berechnung (Code: %1)").arg(result));
    }
}

void PersonDialog::onAccept() {
    qDebug("PersonDialog::onAccept() - AUFGERUFEN");
    
    if (!validateInput()) {
        qDebug("PersonDialog::onAccept() - Validierung fehlgeschlagen");
        return;
    }
    
    saveToRadix();
    
    qDebug("PersonDialog::onAccept() - Speichere Person: %s %s",
           qPrintable(m_radix.rFix.vorname), qPrintable(m_radix.rFix.name));
    
    // Person in Datenbank speichern (ohne Berechnung)
    int idx = personDB().add(m_radix);
    bool saved = personDB().save();
    
    qDebug("PersonDialog::onAccept() - Index: %d, Gespeichert: %s",
           idx, saved ? "ja" : "nein");
    
    accept();
}

} // namespace astro
