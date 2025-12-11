/**
 * @file orben_dialog.cpp
 * @brief Implementierung des Orben-Dialogs
 * 
 * Port von DlgOrbenEin aus legacy/astrofil.c
 * 
 * Layout wie im Legacy:
 * - Dropdown für Planet1 (Y-Planet)
 * - Matrix: Zeilen = Aspekte (7), Spalten = Planet2 (alle Planeten + Knoten etc.)
 * - Orben sind 3D: [Planet1][Planet2][Aspekt]
 */

#include "orben_dialog.h"
#include "../../core/constants.h"
#include "../../core/astro_font_provider.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QPushButton>
#include <QLabel>

namespace astro {

// Anzahl der Planeten für Orben-Dialog
static const int NUM_PLANETS = 13;

// Planeten-Farben (wie im Legacy)
static const QColor PLANET_COLORS[] = {
    QColor(255, 200, 0),    // Sonne - Gold
    QColor(192, 192, 192),  // Mond - Silber
    QColor(255, 255, 0),    // Merkur - Gelb
    QColor(0, 200, 0),      // Venus - Grün
    QColor(255, 0, 0),      // Mars - Rot
    QColor(128, 0, 255),    // Jupiter - Violett
    QColor(128, 64, 0),     // Saturn - Braun
    QColor(0, 255, 255),    // Uranus - Cyan
    QColor(0, 128, 255),    // Neptun - Blau
    QColor(128, 0, 0),      // Pluto - Dunkelrot
    QColor(128, 128, 128),  // Mondknoten - Grau
    QColor(64, 64, 64),     // Lilith - Dunkelgrau
    QColor(255, 128, 0)     // Chiron - Orange
};

// Aspekt-Farben
static const QColor ASPEKT_COLORS[] = {
    QColor(255, 0, 0),      // Konjunktion - Rot
    QColor(0, 128, 0),      // Halbsextil - Dunkelgrün
    QColor(0, 200, 0),      // Sextil - Grün
    QColor(255, 0, 0),      // Quadrat - Rot
    QColor(0, 255, 0),      // Trigon - Hellgrün
    QColor(128, 128, 0),    // Quincunx - Olive
    QColor(255, 0, 255)     // Opposition - Magenta
};

// Volle Planeten-Namen für Dropdown (nur Text, Symbole werden dynamisch hinzugefügt)
static const char* PLANET_TEXT_NAMES[] = {
    "Sonne", "Mond", "Merkur", "Venus", "Mars",
    "Jupiter", "Saturn", "Uranus", "Neptun", "Pluto",
    "Mondknoten", "Lilith", "Chiron"
};

OrbenDialog::OrbenDialog(AuInit& auinit, QWidget* parent)
    : QDialog(parent)
    , m_auinit(auinit)
    , m_currentTyp(ORBEN_RADIX)
    , m_currentPlanet1(0)
    , m_loading(false) {
    
    setWindowTitle(tr("Orben einstellen"));
    setModal(true);
    
    setupUI();
    loadSettings();
}

void OrbenDialog::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // Typ-Auswahl (Radix/Transit/Synastrie)
    QGroupBox* typGroup = new QGroupBox(tr("Orben für"), this);
    QHBoxLayout* typLayout = new QHBoxLayout(typGroup);
    
    m_typGroup = new QButtonGroup(this);
    
    m_radixRadio = new QRadioButton(tr("Radix"), this);
    m_transitRadio = new QRadioButton(tr("Transit"), this);
    m_synastrieRadio = new QRadioButton(tr("Synastrie"), this);
    
    m_typGroup->addButton(m_radixRadio, ORBEN_RADIX);
    m_typGroup->addButton(m_transitRadio, ORBEN_TRANSIT);
    m_typGroup->addButton(m_synastrieRadio, ORBEN_SYNASTRIE);
    
    typLayout->addWidget(m_radixRadio);
    typLayout->addWidget(m_transitRadio);
    typLayout->addWidget(m_synastrieRadio);
    typLayout->addStretch();
    
    m_radixRadio->setChecked(true);
    
    connect(m_typGroup, &QButtonGroup::idClicked, this, &OrbenDialog::onTypChanged);
    
    mainLayout->addWidget(typGroup);
    
    // Planet1 Auswahl (Y-Planet) - wie im Legacy Scrollbar
    QHBoxLayout* selectLayout = new QHBoxLayout();
    selectLayout->addWidget(new QLabel(tr("Planet 1:"), this));
    m_planet1Combo = new QComboBox(this);
    // Planeten-Symbole mit korrektem Font (DejaVu Sans für Asteroiden)
    m_planet1Combo->setFont(astroFont().getPlanetSymbolFont(m_planet1Combo->font().pointSize()));
    for (int i = 0; i < NUM_PLANETS; ++i) {
        QString symbol = astroFont().planetSymbol(i);
        m_planet1Combo->addItem(QString("%1 %2").arg(symbol, tr(PLANET_TEXT_NAMES[i])), i);
    }
    connect(m_planet1Combo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &OrbenDialog::onPlanet1Changed);
    selectLayout->addWidget(m_planet1Combo);
    selectLayout->addStretch();
    mainLayout->addLayout(selectLayout);
    
    // Matrix: Zeilen = Planeten (13), Spalten = Aspekte (7)
    m_orbenTable = new QTableWidget(NUM_PLANETS, ASPEKTE, this);
    
    // Font für Symbole (DejaVu Sans für Asteroiden und Aspekte)
    QFont symbolFont = astroFont().getPlanetSymbolFont(16);
    symbolFont.setBold(true);
    
    // Header-Font auf die gesamte Header-Sektion setzen
    m_orbenTable->horizontalHeader()->setFont(symbolFont);
    m_orbenTable->verticalHeader()->setFont(symbolFont);
    
    // Spalten-Header (Aspekt-Symbole - X-Achse) mit Farben
    for (int i = 0; i < ASPEKTE; ++i) {
        QTableWidgetItem* item = new QTableWidgetItem(astroFont().aspektSymbol(i));
        item->setFont(symbolFont);
        item->setForeground(ASPEKT_COLORS[i]);
        m_orbenTable->setHorizontalHeaderItem(i, item);
    }
    
    // Zeilen-Header (Planeten-Symbole - Y-Achse) mit Farben
    for (int i = 0; i < NUM_PLANETS; ++i) {
        QTableWidgetItem* item = new QTableWidgetItem(astroFont().planetSymbol(i));
        item->setFont(symbolFont);
        item->setForeground(PLANET_COLORS[i]);
        m_orbenTable->setVerticalHeaderItem(i, item);
    }
    
    // Spaltenbreite anpassen
    m_orbenTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_orbenTable->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    m_orbenTable->verticalHeader()->setDefaultSectionSize(30);
    
    // SpinBoxen für jede Zelle
    // Diagonale (Planet1 == Planet2) wird mit "—" markiert
    for (int pl2 = 0; pl2 < NUM_PLANETS; ++pl2) {
        for (int asp = 0; asp < ASPEKTE; ++asp) {
            QDoubleSpinBox* spinBox = new QDoubleSpinBox(this);
            spinBox->setRange(0.0, 15.0);
            spinBox->setDecimals(1);
            spinBox->setSingleStep(0.5);
            m_orbenTable->setCellWidget(pl2, asp, spinBox);
        }
    }
    
    mainLayout->addWidget(m_orbenTable);
    
    // Buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    
    QPushButton* defaultButton = new QPushButton(tr("Standard"), this);
    connect(defaultButton, &QPushButton::clicked, this, &OrbenDialog::onDefault);
    buttonLayout->addWidget(defaultButton);
    
    buttonLayout->addStretch();
    
    QPushButton* okButton = new QPushButton(tr("OK"), this);
    okButton->setDefault(true);
    connect(okButton, &QPushButton::clicked, this, &OrbenDialog::onAccept);
    buttonLayout->addWidget(okButton);
    
    QPushButton* cancelButton = new QPushButton(tr("Abbrechen"), this);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    buttonLayout->addWidget(cancelButton);
    
    mainLayout->addLayout(buttonLayout);
    
    // Dialog-Größe für Matrix (13 Zeilen x 7 Spalten)
    resize(600, 500);
}

void OrbenDialog::onTypChanged(int typ) {
    saveSettings();
    m_currentTyp = typ;
    loadSettings();
}

void OrbenDialog::onPlanet1Changed(int index) {
    saveSettings();
    m_currentPlanet1 = index;
    loadSettings();
}

void OrbenDialog::onCellChanged(int row, int col) {
    // Wird nicht mehr benötigt - Werte werden bei saveSettings gespeichert
    Q_UNUSED(row);
    Q_UNUSED(col);
}

void OrbenDialog::updateTable() {
    loadSettings();
}

void OrbenDialog::loadSettings() {
    if (m_loading) return;
    m_loading = true;
    
    // Wähle das richtige Array basierend auf dem Typ
    QVector<float>* orbenArray = nullptr;
    
    switch (m_currentTyp) {
        case ORBEN_TRANSIT:
            orbenArray = &m_auinit.orbenTPlanet;
            break;
        case ORBEN_SYNASTRIE:
            orbenArray = &m_auinit.orbenSPlanet;
            break;
        default:  // ORBEN_RADIX
            orbenArray = &m_auinit.orbenPlanet;
            break;
    }
    
    int requiredSize = MAX_PLANET * MAX_PLANET * ASPEKTE;
    if (orbenArray->size() < requiredSize) {
        // Array ist zu klein - initialisieren
        m_auinit.initOrben();
        // Nochmal prüfen
        if (orbenArray->size() < requiredSize) {
            setDefaultOrben();
            m_loading = false;
            return;
        }
    }
    
    // Lade Matrix: Zeilen = Planeten, Spalten = Aspekte
    // Für ausgewählten Planet1 (Y-Planet via Dropdown)
    for (int pl2 = 0; pl2 < NUM_PLANETS; ++pl2) {
        for (int asp = 0; asp < ASPEKTE; ++asp) {
            // Diagonale: Planet1 == Planet2 -> kein Aspekt möglich
            if (m_currentPlanet1 == pl2) {
                // SpinBox entfernen und Label setzen
                m_orbenTable->removeCellWidget(pl2, asp);
                QTableWidgetItem* item = new QTableWidgetItem("—");
                item->setTextAlignment(Qt::AlignCenter);
                item->setFlags(item->flags() & ~Qt::ItemIsEditable);
                m_orbenTable->setItem(pl2, asp, item);
            } else {
                // Normaler Fall: SpinBox anzeigen
                m_orbenTable->takeItem(pl2, asp);  // Entferne eventuelles "—" Item
                QDoubleSpinBox* spinBox = qobject_cast<QDoubleSpinBox*>(
                    m_orbenTable->cellWidget(pl2, asp));
                if (!spinBox) {
                    spinBox = new QDoubleSpinBox(m_orbenTable);
                    spinBox->setRange(0.0, 15.0);
                    spinBox->setDecimals(1);
                    spinBox->setSingleStep(0.5);
                    // Bei Änderung speichern
                    connect(spinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                            this, &OrbenDialog::saveSettings);
                    m_orbenTable->setCellWidget(pl2, asp, spinBox);
                }
                // Index: [Planet1 * MAX_PLANET + Planet2] * ASPEKTE + Aspekt
                int idx = (m_currentPlanet1 * MAX_PLANET + pl2) * ASPEKTE + asp;
                if (idx < orbenArray->size()) {
                    float value = (*orbenArray)[idx];
                    // Wenn Wert 0 ist, Default-Wert verwenden
                    if (value <= 0.0f) {
                        value = getDefaultOrb(asp, m_currentPlanet1, pl2);
                        // Auch im Array speichern
                        (*orbenArray)[idx] = value;
                    }
                    spinBox->setValue(value);
                }
            }
        }
    }
    
    m_loading = false;
}

void OrbenDialog::saveSettings() {
    if (m_loading) return;
    
    // Wähle das richtige Array basierend auf dem Typ
    QVector<float>* orbenArray = nullptr;
    
    switch (m_currentTyp) {
        case ORBEN_TRANSIT:
            orbenArray = &m_auinit.orbenTPlanet;
            break;
        case ORBEN_SYNASTRIE:
            orbenArray = &m_auinit.orbenSPlanet;
            break;
        default:  // ORBEN_RADIX
            orbenArray = &m_auinit.orbenPlanet;
            break;
    }
    
    int requiredSize = MAX_PLANET * MAX_PLANET * ASPEKTE;
    if (orbenArray->size() < requiredSize) {
        return;
    }
    
    // Speichere Matrix: Zeilen = Planeten, Spalten = Aspekte
    for (int pl2 = 0; pl2 < NUM_PLANETS; ++pl2) {
        // Diagonale überspringen
        if (m_currentPlanet1 == pl2) continue;
        
        for (int asp = 0; asp < ASPEKTE; ++asp) {
            QDoubleSpinBox* spinBox = qobject_cast<QDoubleSpinBox*>(
                m_orbenTable->cellWidget(pl2, asp));
            if (spinBox) {
                int idx = (m_currentPlanet1 * MAX_PLANET + pl2) * ASPEKTE + asp;
                if (idx < orbenArray->size()) {
                    (*orbenArray)[idx] = static_cast<float>(spinBox->value());
                }
            }
        }
    }
}

void OrbenDialog::setDefaultOrben() {
    // Standard-Orben aus Legacy (sOrben in auhelper.c)
    // Basis-Werte: Radix=2.5, Transit=0.8, Synastrie=2.0
    float defaultOrb = getDefaultOrb(0, 0, 0);
    
    // Setze Standardwerte für alle Zellen in der Matrix
    for (int pl2 = 0; pl2 < NUM_PLANETS; ++pl2) {
        // Diagonale überspringen
        if (m_currentPlanet1 == pl2) continue;
        
        for (int asp = 0; asp < ASPEKTE; ++asp) {
            QDoubleSpinBox* spinBox = qobject_cast<QDoubleSpinBox*>(
                m_orbenTable->cellWidget(pl2, asp));
            if (spinBox) {
                spinBox->setValue(defaultOrb);
            }
        }
    }
}

void OrbenDialog::onDefault() {
    setDefaultOrben();
}

void OrbenDialog::onAccept() {
    saveSettings();
    accept();
}

float OrbenDialog::getDefaultOrb(int aspekt, int planet1, int planet2) {
    // Standard-Orben aus Legacy (sOrben in auhelper.c)
    // Basis-Wert für Radix Planet-zu-Planet: 2.5
    Q_UNUSED(aspekt);
    Q_UNUSED(planet1);
    Q_UNUSED(planet2);
    
    switch (m_currentTyp) {
        case ORBEN_TRANSIT:
            return 0.8f;
        case ORBEN_SYNASTRIE:
            return 2.0f;
        default:  // ORBEN_RADIX
            return 2.5f;
    }
}

} // namespace astro
