/**
 * @file transit_dialog.cpp
 * @brief Implementierung des Transit-Dialogs
 * 
 * 1:1 Port von DlgTransit aus legacy/astrofil.c
 */

#include "transit_dialog.h"
#include "trans_sel_dialog.h"
#include "ort_search_dialog.h"
#include "../../core/chart_calc.h"
#include "../../data/orte_db.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QGroupBox>
#include <QMessageBox>
#include <QProgressDialog>
#include <QCoreApplication>

namespace astro {

TransitDialog::TransitDialog(QWidget* parent, AuInit& auinit, const Radix& basisRadix)
    : QDialog(parent)
    , m_auinit(auinit)
    , m_basisRadix(basisRadix)
    , m_calculated(false) {
    
    setWindowTitle(tr("Transit"));
    setModal(true);
    
    setupUI();
    loadFromRadix();
}

TransitDialog::~TransitDialog() {
}

void TransitDialog::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // Grid für Eingabefelder
    QGridLayout* gridLayout = new QGridLayout();
    
    // Name (readonly)
    gridLayout->addWidget(new QLabel(tr("Name:")), 0, 0);
    m_nameEdit = new QLineEdit(this);
    m_nameEdit->setReadOnly(true);
    gridLayout->addWidget(m_nameEdit, 0, 1, 1, 2);
    
    // Ort mit Suchen-Button
    QPushButton* ortButton = new QPushButton(tr("Ort:"), this);
    connect(ortButton, &QPushButton::clicked, this, &TransitDialog::onOrtSuchen);
    gridLayout->addWidget(ortButton, 1, 0);
    m_ortEdit = new QLineEdit(this);
    gridLayout->addWidget(m_ortEdit, 1, 1, 1, 2);
    
    // Datum/Zeit
    gridLayout->addWidget(new QLabel(tr("Datum:")), 2, 0);
    m_datumEdit = new QDateEdit(this);
    m_datumEdit->setDisplayFormat("dd.MM.yyyy");
    m_datumEdit->setCalendarPopup(true);
    gridLayout->addWidget(m_datumEdit, 2, 1);
    
    gridLayout->addWidget(new QLabel(tr("bis")), 2, 2);
    m_bisDatumEdit = new QDateEdit(this);
    m_bisDatumEdit->setDisplayFormat("dd.MM.yyyy");
    m_bisDatumEdit->setCalendarPopup(true);
    gridLayout->addWidget(m_bisDatumEdit, 2, 3);
    
    gridLayout->addWidget(new QLabel(tr("Zeit:")), 3, 0);
    m_zeitEdit = new QTimeEdit(this);
    m_zeitEdit->setDisplayFormat("HH:mm");
    gridLayout->addWidget(m_zeitEdit, 3, 1);
    
    gridLayout->addWidget(new QLabel(tr("bis")), 3, 2);
    m_bisZeitEdit = new QTimeEdit(this);
    m_bisZeitEdit->setDisplayFormat("HH:mm");
    gridLayout->addWidget(m_bisZeitEdit, 3, 3);
    
    mainLayout->addLayout(gridLayout);
    
    // Inkrement-Auswahl (STRICT LEGACY: RB_TAG, RB_MONAT, RB_JAHR)
    QGroupBox* inkrementBox = new QGroupBox(tr("Berechnen"), this);
    QHBoxLayout* inkrementLayout = new QHBoxLayout(inkrementBox);
    
    m_inkrementGroup = new QButtonGroup(this);
    m_tagRadio = new QRadioButton(tr("Tag"), inkrementBox);
    m_monatRadio = new QRadioButton(tr("Monat"), inkrementBox);
    m_jahrRadio = new QRadioButton(tr("Jahr"), inkrementBox);
    
    m_inkrementGroup->addButton(m_tagRadio, INC_TAGE);
    m_inkrementGroup->addButton(m_monatRadio, INC_MONATE);
    m_inkrementGroup->addButton(m_jahrRadio, INC_JAHRE);
    
    inkrementLayout->addWidget(m_tagRadio);
    inkrementLayout->addWidget(m_monatRadio);
    inkrementLayout->addWidget(m_jahrRadio);
    
    m_tagRadio->setChecked(true);
    
    mainLayout->addWidget(inkrementBox);
    
    // Buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    
    QPushButton* okButton = new QPushButton(tr("OK"), this);
    okButton->setDefault(true);
    connect(okButton, &QPushButton::clicked, this, &TransitDialog::onAccept);
    buttonLayout->addWidget(okButton);
    
    QPushButton* cancelButton = new QPushButton(tr("Abbruch"), this);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    buttonLayout->addWidget(cancelButton);
    
    mainLayout->addLayout(buttonLayout);
}

void TransitDialog::loadFromRadix() {
    // Name aus Basis-Radix
    QString name = m_basisRadix.rFix.vorname;
    if (!m_basisRadix.rFix.name.isEmpty()) {
        if (!name.isEmpty()) name += " ";
        name += m_basisRadix.rFix.name;
    }
    m_nameEdit->setText(name);
    
    // Ort aus Basis-Radix
    m_ortEdit->setText(m_basisRadix.rFix.ort);
    
    // Aktuelles Datum/Zeit als Standard
    QDate today = QDate::currentDate();
    QTime now = QTime::currentTime();
    
    m_datumEdit->setDate(today);
    m_zeitEdit->setTime(QTime(12, 0));  // STRICT LEGACY: Standard 12:00
    
    // Bis-Datum/Zeit = Start + 1 Tag
    m_bisDatumEdit->setDate(today);
    m_bisZeitEdit->setTime(QTime(12, 0));
}

void TransitDialog::onOrtSuchen() {
    // Port von: PB_TOrt -> DlgOrtAuswahl
    OrtSearchDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        Orte ort = dialog.getSelectedOrt();
        if (ort.gultig) {
            m_ortEdit->setText(ort.name);
            // Koordinaten werden beim Berechnen aus der Datenbank geholt
        }
    }
}

bool TransitDialog::validateInput() {
    if (m_ortEdit->text().isEmpty()) {
        QMessageBox::warning(this, tr("Fehler"), tr("Bitte einen Ort eingeben."));
        m_ortEdit->setFocus();
        return false;
    }
    
    QDate startD = m_datumEdit->date();
    QDate endD = m_bisDatumEdit->date();
    QTime startT = m_zeitEdit->time();
    QTime endT = m_bisZeitEdit->time();
    
    if (startD > endD || (startD == endD && startT > endT)) {
        QMessageBox::warning(this, tr("Fehler"), tr("Start-Datum/Zeit muss vor End-Datum/Zeit liegen."));
        m_datumEdit->setFocus();
        return false;
    }
    
    return true;
}

void TransitDialog::calculateTransit() {
    // STRICT LEGACY: Multi-Transit berechnen (sCalcMultiTransit)
    m_transits.clear();
    m_transitTexts.clear();
    m_aspekte.clear();
    
    QDate startDate = m_datumEdit->date();
    QTime startTime = m_zeitEdit->time();
    QDate endDate = m_bisDatumEdit->date();
    QTime endTime = m_bisZeitEdit->time();
    int inkrement = m_inkrementGroup->checkedId();
    
    // Anzahl Schritte berechnen für Progress
    int steps = 0;
    {
        QDate d = startDate;
        QTime t = startTime;
        while (d < endDate || (d == endDate && t <= endTime)) {
            ++steps;
            if (TransitCalc::incDate(d, t, inkrement, 1) != ERR_OK) break;
            if (steps > 100000) break;
        }
    }
    
    // Progress-Dialog (STRICT LEGACY: DlgTransAbort)
    m_abortFlag = false;
    QProgressDialog progress(tr("Transite werden berechnet..."), tr("Abbruch"), 0, steps > 0 ? steps : 1, this);
    progress.setWindowModality(Qt::ApplicationModal);
    progress.setMinimumDuration(0);
    progress.setValue(0);
    connect(&progress, &QProgressDialog::canceled, this, [&]() {
        m_abortFlag = true;
    });
    
    int result = TransitCalc::calcMultiTransit(
        m_basisRadix,
        startDate, startTime,
        endDate, endTime,
        inkrement,
        &m_transits,
        &m_aspekte,
        m_auinit.orbenTPlanet,
        m_auinit.orbenTHaus,
        &m_abortFlag,
        [&](int idx, const QDate&, const QTime&) {
            if (steps > 0) {
                progress.setValue(qMin(idx, steps));
            }
            QCoreApplication::processEvents();
        });
    
    progress.setValue(steps);
    
    if (result >= 0 && !m_transits.isEmpty()) {
        m_transitRadix = m_transits.first();
        m_calculated = true;
    } else if (result >= 0 && m_transits.isEmpty()) {
        // Fallback: Einzel-Transit
        result = TransitCalc::calcTransit(m_basisRadix, m_transitRadix, startDate, startTime);
        m_calculated = (result == ERR_OK) && !m_abortFlag;
    } else {
        QMessageBox::warning(this, tr("Fehler"),
            tr("Fehler bei der Transit-Berechnung (Code: %1)").arg(result));
        m_calculated = false;
    }
    
    if (m_calculated) {
        ensureDefaultTransSel();
    }
}

QString TransitDialog::getVonDatum() const {
    return m_datumEdit->date().toString("dd.MM.yyyy");
}

QString TransitDialog::getBisDatum() const {
    return m_bisDatumEdit->date().toString("dd.MM.yyyy");
}

void TransitDialog::ensureDefaultTransSel() {
    // Größe ableiten: Transit-Planeten aus erster Transit-Radix (falls vorhanden)
    int transitCount = m_transitRadix.planet.size();
    if (!m_transits.isEmpty()) {
        transitCount = m_transits.first().planet.size();
    }
    int radixPlanets = m_basisRadix.planet.size();
    int radixCount = radixPlanets + MAX_HAUS;  // Planeten + Häuser (Legacy: MAX_PLANET+5)
    
    if (transitCount <= 0 || radixCount <= 0) {
        m_transSel.clear();
        return;
    }
    
    if (m_transSel.size() != transitCount || (!m_transSel.isEmpty() && m_transSel.first().size() != radixCount)) {
        m_transSel = QVector<QVector<bool>>(transitCount, QVector<bool>(radixCount, true));
    }
}

void TransitDialog::onAccept() {
    if (!validateInput()) {
        return;
    }
    
    calculateTransit();
    
    m_auinit.bZeit = m_bisZeitEdit->time().toString("HH:mm");
    
    accept();
}

} // namespace astro
