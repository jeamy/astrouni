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

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QPushButton>
#include <QLabel>

namespace astro {

// Planeten-Symbole (Unicode) - wie im Legacy
static const char* PLANET_SYMBOLS[] = {
    "☉",  // Sonne
    "☽",  // Mond
    "☿",  // Merkur
    "♀",  // Venus
    "♂",  // Mars
    "♃",  // Jupiter
    "♄",  // Saturn
    "♅",  // Uranus
    "♆",  // Neptun
    "♇",  // Pluto
    "☊",  // Mondknoten
    "⚸",  // Lilith
    "⚷"   // Chiron
};
static const int NUM_PLANETS = 13;

// Aspekt-Symbole (Unicode)
static const char* ASPEKT_SYMBOLS[] = {
    "☌",  // Konjunktion (0°)
    "⚺",  // Halbsextil (30°)
    "⚹",  // Sextil (60°)
    "□",  // Quadrat (90°)
    "△",  // Trigon (120°)
    "⚻",  // Quincunx (150°)
    "☍"   // Opposition (180°)
};

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

// Volle Planeten-Namen für Dropdown
static const char* PLANET_FULL_NAMES[] = {
    "☉ Sonne", "☽ Mond", "☿ Merkur", "♀ Venus", "♂ Mars",
    "♃ Jupiter", "♄ Saturn", "♅ Uranus", "♆ Neptun", "♇ Pluto",
    "☊ Mondknoten", "⚸ Lilith", "⚷ Chiron"
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
    for (int i = 0; i < NUM_PLANETS; ++i) {
        m_planet1Combo->addItem(tr(PLANET_FULL_NAMES[i]), i);
    }
    connect(m_planet1Combo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &OrbenDialog::onPlanet1Changed);
    selectLayout->addWidget(m_planet1Combo);
    selectLayout->addStretch();
    mainLayout->addLayout(selectLayout);
    
    // Matrix: Zeilen = Planeten (13), Spalten = Aspekte (7)
    m_orbenTable = new QTableWidget(NUM_PLANETS, ASPEKTE, this);
    
    // Spalten-Header (Aspekt-Symbole - X-Achse) mit Farben
    QFont headerFont;
    headerFont.setPointSize(16);
    headerFont.setBold(true);
    
    for (int i = 0; i < ASPEKTE; ++i) {
        QTableWidgetItem* item = new QTableWidgetItem(QString::fromUtf8(ASPEKT_SYMBOLS[i]));
        item->setFont(headerFont);
        item->setForeground(ASPEKT_COLORS[i]);
        m_orbenTable->setHorizontalHeaderItem(i, item);
    }
    
    // Zeilen-Header (Planeten-Symbole - Y-Achse) mit Farben
    for (int i = 0; i < NUM_PLANETS; ++i) {
        QTableWidgetItem* item = new QTableWidgetItem(QString::fromUtf8(PLANET_SYMBOLS[i]));
        item->setFont(headerFont);
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
            QDoubleSpinBox* spinBox = qobject_cast<QDoubleSpinBox*>(
                m_orbenTable->cellWidget(pl2, asp));
            if (spinBox) {
                // Index: [Planet1 * MAX_PLANET + Planet2] * ASPEKTE + Aspekt
                int idx = (m_currentPlanet1 * MAX_PLANET + pl2) * ASPEKTE + asp;
                if (idx < orbenArray->size()) {
                    spinBox->setValue((*orbenArray)[idx]);
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
    // Standard-Orben (wie im Legacy)
    float defaultOrben[ASPEKTE] = {
        10.0f,  // Konjunktion
        2.0f,   // Halbsextil
        6.0f,   // Sextil
        8.0f,   // Quadratur
        8.0f,   // Trigon
        3.0f,   // Quincunx
        10.0f   // Opposition
    };
    
    // Setze Standardwerte für alle Zellen in der Matrix
    for (int pl2 = 0; pl2 < NUM_PLANETS; ++pl2) {
        for (int asp = 0; asp < ASPEKTE; ++asp) {
            QDoubleSpinBox* spinBox = qobject_cast<QDoubleSpinBox*>(
                m_orbenTable->cellWidget(pl2, asp));
            if (spinBox) {
                float adjustedOrb = defaultOrben[asp];
                // Sonne/Mond haben größere Orben
                if (m_currentPlanet1 <= 1 || pl2 <= 1) {
                    adjustedOrb += 2.0f;
                }
                // Asteroiden haben kleinere Orben
                if (m_currentPlanet1 >= 10 || pl2 >= 10) {
                    adjustedOrb -= 2.0f;
                    if (adjustedOrb < 1.0f) adjustedOrb = 1.0f;
                }
                spinBox->setValue(adjustedOrb);
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

} // namespace astro
