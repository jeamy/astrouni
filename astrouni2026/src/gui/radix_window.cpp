/**
 * @file radix_window.cpp
 * @brief Implementierung des Radix-Fensters
 */

#include "radix_window.h"
#include "chart_widget.h"
#include "../core/calculations.h"
#include "../core/chart_calc.h"

#include <QMenuBar>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSplitter>
#include <QPrintDialog>
#include <QPrinter>
#include <QFileDialog>

namespace astro {

//==============================================================================
// Konstruktor / Destruktor
//==============================================================================

RadixWindow::RadixWindow(QWidget* parent, AuInit& auinit, Radix& radix)
    : QMainWindow(parent)
    , m_auinit(auinit)
    , m_radix(radix)
    , m_listMode(Daten) {
    
    setWindowTitle(tr("AstroUniverse - Radix"));
    setAttribute(Qt::WA_DeleteOnClose);
    
    setupMenus();
    setupUI();
    
    // Initiale Anzeige
    updateDisplay();
}

RadixWindow::~RadixWindow() {
}

//==============================================================================
// Menü-Setup
//==============================================================================

void RadixWindow::setupMenus() {
    // Datei-Menü (Port von RADIXMENU)
    QMenu* fileMenu = menuBar()->addMenu(tr("&Datei"));
    
    QAction* saveAction = fileMenu->addAction(tr("&Speichern"));
    connect(saveAction, &QAction::triggered, this, &RadixWindow::onSave);
    
    QAction* printAction = fileMenu->addAction(tr("D&rucken"));
    printAction->setShortcut(QKeySequence::Print);
    connect(printAction, &QAction::triggered, this, &RadixWindow::onPrint);
    
    fileMenu->addSeparator();
    
    QAction* closeAction = fileMenu->addAction(tr("E&xit"));
    connect(closeAction, &QAction::triggered, this, &RadixWindow::onClose);
    
    // Horoskop-Menü
    QAction* horoAction = menuBar()->addAction(tr("&Horoskop"));
    connect(horoAction, &QAction::triggered, this, &RadixWindow::onHoroTyp);
    
    // Hilfe-Menü
    menuBar()->addAction(tr("&Help"));
}

//==============================================================================
// UI-Setup
//==============================================================================

void RadixWindow::setupUI() {
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    QHBoxLayout* mainLayout = new QHBoxLayout(centralWidget);
    
    // Splitter für Chart und Liste
    QSplitter* splitter = new QSplitter(Qt::Horizontal, centralWidget);
    mainLayout->addWidget(splitter);
    
    // Chart-Widget
    m_chartWidget = new ChartWidget(splitter);
    m_chartWidget->setSettings(m_auinit);
    splitter->addWidget(m_chartWidget);
    
    // Rechte Seite: Liste und Buttons
    QWidget* rightWidget = new QWidget(splitter);
    QVBoxLayout* rightLayout = new QVBoxLayout(rightWidget);
    
    // Listbox (Port von LB_DATEN, LB_POSITION, LB_ASPEKTE)
    m_listWidget = new QListWidget(rightWidget);
    m_listWidget->setMinimumWidth(200);
    connect(m_listWidget, &QListWidget::itemClicked, 
            this, &RadixWindow::onListItemClicked);
    rightLayout->addWidget(m_listWidget);
    
    // Buttons (Port von PB_DATEN, PB_POSITION, PB_ASPEKTE)
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    
    m_datenButton = new QPushButton(tr("Daten"), rightWidget);
    connect(m_datenButton, &QPushButton::clicked, this, &RadixWindow::onDatenClicked);
    buttonLayout->addWidget(m_datenButton);
    
    m_positionenButton = new QPushButton(tr("Positionen"), rightWidget);
    connect(m_positionenButton, &QPushButton::clicked, this, &RadixWindow::onPositionenClicked);
    buttonLayout->addWidget(m_positionenButton);
    
    m_aspekteButton = new QPushButton(tr("Aspekte"), rightWidget);
    connect(m_aspekteButton, &QPushButton::clicked, this, &RadixWindow::onAspekteClicked);
    buttonLayout->addWidget(m_aspekteButton);
    
    rightLayout->addLayout(buttonLayout);
    
    splitter->addWidget(rightWidget);
    
    // Splitter-Proportionen
    splitter->setStretchFactor(0, 3);  // Chart größer
    splitter->setStretchFactor(1, 1);  // Liste kleiner
    
    // Fenster-Größe
    resize(900, 700);
}

//==============================================================================
// Setter
//==============================================================================

void RadixWindow::setRadix(const Radix& radix) {
    m_radix = radix;
    updateDisplay();
}

void RadixWindow::updateDisplay() {
    // Chart aktualisieren
    m_chartWidget->setRadix(m_radix);
    
    // Liste aktualisieren
    switch (m_listMode) {
        case Daten:
            fillDatenList();
            break;
        case Positionen:
            fillPositionenList();
            break;
        case Aspekte:
            fillAspekteList();
            break;
    }
    
    // Fenster-Titel
    QString title = tr("AstroUniverse - ");
    if (!m_radix.rFix.name.isEmpty()) {
        title += m_radix.rFix.vorname + " " + m_radix.rFix.name;
    } else {
        title += tr("Radix");
    }
    setWindowTitle(title);
}

//==============================================================================
// Listen füllen
//==============================================================================

void RadixWindow::fillDatenList() {
    m_listWidget->clear();
    
    // Personen-Daten
    m_listWidget->addItem(tr("*Radix*"));
    m_listWidget->addItem(QString("Name: %1 %2")
        .arg(m_radix.rFix.vorname, m_radix.rFix.name));
    m_listWidget->addItem(QString("Geburtsdatum: %1.%2.%3")
        .arg(m_radix.rFix.tag, 2, 10, QChar('0'))
        .arg(m_radix.rFix.monat, 2, 10, QChar('0'))
        .arg(m_radix.rFix.jahr));
    
    int16_t stunden, minuten;
    Calculations::decimalToTime(m_radix.rFix.zeit, stunden, minuten);
    m_listWidget->addItem(QString("Geburtszeit: %1:%2")
        .arg(stunden, 2, 10, QChar('0'))
        .arg(minuten, 2, 10, QChar('0')));
    
    m_listWidget->addItem(QString("Geburtsort: %1").arg(m_radix.rFix.ort));
    m_listWidget->addItem(QString("Land: %1").arg(m_radix.rFix.land));
    m_listWidget->addItem(QString("Länge: %1").arg(m_radix.rFix.laenge, 0, 'f', 4));
    m_listWidget->addItem(QString("Breite: %1").arg(m_radix.rFix.breite, 0, 'f', 4));
    m_listWidget->addItem(QString("Zeitzone: %1").arg(m_radix.rFix.zone, 0, 'f', 1));
    
    // Häusersystem
    static const char* hausTypen[] = {
        "Koch", "Placidus", "Equal", "Equal (MC)", "Whole Sign",
        "Topocentric", "Campanus", "Meridian", "Regiomontanus",
        "Porphyry", "Neo-Porphyry", "Morinus", "Alcabitius", "Null"
    };
    m_listWidget->addItem(QString("Häusersystem: %1")
        .arg(hausTypen[m_radix.hausSys]));
}

void RadixWindow::fillPositionenList() {
    m_listWidget->clear();
    
    // Planeten-Positionen
    m_listWidget->addItem(tr("-Planeten-"));
    
    static const char* planetenNamen[] = {
        SONNE_TXT, MOND_TXT, MERKUR_TXT, VENUS_TXT, MARS_TXT,
        JUPITER_TXT, SATURN_TXT, URANUS_TXT, NEPTUN_TXT, PLUTO_TXT,
        NMKL_TXT, LILITH_TXT, CHIRON_TXT, CERES_TXT, PALLAS_TXT,
        JUNO_TXT, VESTA_TXT
    };
    
    for (int i = 0; i < m_radix.anzahlPlanet; ++i) {
        QString pos = Calculations::degToString(m_radix.planet[i], true, true);
        QString retro = (m_radix.planetTyp[i] & P_TYP_RUCK) ? " R" : "";
        QString haus = QString(" (H%1)").arg(m_radix.inHaus[i] + 1);
        
        m_listWidget->addItem(QString("%1: %2%3%4")
            .arg(QString::fromUtf8(planetenNamen[i]), -12)
            .arg(pos)
            .arg(retro)
            .arg(haus));
    }
    
    // Häuser-Positionen
    m_listWidget->addItem(tr("-Häuser-"));
    
    static const char* hausNamen[] = {
        ASZENDENT_TXT, HAUS2_TXT, HAUS3_TXT, HAUS4_TXT,
        HAUS5_TXT, HAUS6_TXT, HAUS7_TXT, HAUS8_TXT,
        HAUS9_TXT, HAUS10_TXT, HAUS11_TXT, HAUS12_TXT
    };
    
    for (int i = 0; i < MAX_HAUS; ++i) {
        QString pos = Calculations::degToString(m_radix.haus[i], true, true);
        m_listWidget->addItem(QString("%1: %2")
            .arg(QString::fromUtf8(hausNamen[i]), -12)
            .arg(pos));
    }
}

void RadixWindow::fillAspekteList() {
    m_listWidget->clear();
    
    m_listWidget->addItem(tr("-Aspekte-"));
    
    static const char* planetenNamen[] = {
        SONNE_TXT, MOND_TXT, MERKUR_TXT, VENUS_TXT, MARS_TXT,
        JUPITER_TXT, SATURN_TXT, URANUS_TXT, NEPTUN_TXT, PLUTO_TXT,
        NMKL_TXT, LILITH_TXT, CHIRON_TXT, CERES_TXT, PALLAS_TXT,
        JUNO_TXT, VESTA_TXT
    };
    
    int numPlanets = m_radix.anzahlPlanet;
    
    for (int i = 0; i < numPlanets; ++i) {
        for (int j = i + 1; j < numPlanets; ++j) {
            int idx = i * numPlanets + j;
            int8_t asp = m_radix.aspPlanet[idx];
            
            if (asp == KEIN_ASP) continue;
            
            QString aspName = Calculations::getAspektName(asp);
            double winkel = m_radix.winkelPlanet[idx];
            double orb = std::fabs(winkel - asp);
            
            m_listWidget->addItem(QString("%1 - %2: %3 (%4°)")
                .arg(QString::fromUtf8(planetenNamen[i]))
                .arg(QString::fromUtf8(planetenNamen[j]))
                .arg(aspName)
                .arg(orb, 0, 'f', 2));
        }
    }
}

//==============================================================================
// Slots
//==============================================================================

void RadixWindow::onSave() {
    // TODO: Radix speichern
}

void RadixWindow::onPrint() {
    QPrinter printer;
    QPrintDialog dialog(&printer, this);
    
    if (dialog.exec() == QDialog::Accepted) {
        // TODO: Drucken implementieren
    }
}

void RadixWindow::onClose() {
    close();
}

void RadixWindow::onHoroTyp() {
    // TODO: Horoskop-Typ Dialog
}

void RadixWindow::onDatenClicked() {
    m_listMode = Daten;
    fillDatenList();
}

void RadixWindow::onPositionenClicked() {
    m_listMode = Positionen;
    fillPositionenList();
}

void RadixWindow::onAspekteClicked() {
    m_listMode = Aspekte;
    fillAspekteList();
}

void RadixWindow::onListItemClicked(QListWidgetItem* /*item*/) {
    // TODO: Aspekt-Linie im Chart hervorheben
}

} // namespace astro
