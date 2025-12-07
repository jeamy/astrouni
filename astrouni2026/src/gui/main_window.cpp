/**
 * @file main_window.cpp
 * @brief Implementierung des Hauptfensters
 * 
 * 1:1 Port von MainWndProc aus legacy/astrouni.c
 */

#include "main_window.h"
#include "radix_window.h"
#include "dialogs/person_dialog.h"
#include "dialogs/person_search_dialog.h"
#include "dialogs/ort_dialog.h"
#include "dialogs/horo_typ_dialog.h"
#include "dialogs/transit_dialog.h"
#include "dialogs/trans_sel_dialog.h"
#include "transit_result_window.h"
#include "dialogs/orben_dialog.h"
#include "dialogs/farben_dialog.h"
#include "dialogs/settings_dialog.h"
#include "../data/legacy_io.h"
#include "../data/orte_db.h"
#include "../data/person_db.h"
#include "../core/swiss_eph.h"
#include "../core/chart_calc.h"

#include <QMenuBar>
#include <QMessageBox>
#include <QApplication>
#include <QDir>
#include <QIcon>
#include <QFile>

namespace astro {

//==============================================================================
// Konstruktor / Destruktor
//==============================================================================

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , m_tabWidget(nullptr) {
    
    // Fenster-Titel (wie im Legacy: MainMenuText)
    setWindowTitle(tr("AstroUniverse 2026"));
    
    // Fenster-Icon setzen
    QString iconPath = QApplication::applicationDirPath() + "/resources/icons/astrouni.ico";
    if (QFile::exists(iconPath)) {
        setWindowIcon(QIcon(iconPath));
    } else {
        // Fallback: Icon aus Ressourcen-Verzeichnis relativ zum Build
        iconPath = QApplication::applicationDirPath() + "/../resources/icons/astrouni.ico";
        if (QFile::exists(iconPath)) {
            setWindowIcon(QIcon(iconPath));
        }
    }
    
    // Fenster maximiert starten (wie im Legacy: SW_SHOWMAXIMIZED)
    setWindowState(Qt::WindowMaximized);
    
    // Tab-Widget als zentrales Widget
    m_tabWidget = new QTabWidget(this);
    m_tabWidget->setTabsClosable(true);
    m_tabWidget->setMovable(true);
    connect(m_tabWidget, &QTabWidget::tabCloseRequested, 
            this, &MainWindow::onTabCloseRequested);
    setCentralWidget(m_tabWidget);
    
    // Menüs und Aktionen einrichten
    setupActions();
    setupMenus();
    
    // Einstellungen laden
    loadSettings();
    
    // Swiss Ephemeris initialisieren
    // Versuche verschiedene Pfade für die Ephemeriden-Dateien
    QStringList ephePaths = {
        QApplication::applicationDirPath() + "/swisseph/ephe",
        QApplication::applicationDirPath() + "/../swisseph/ephe",
        QApplication::applicationDirPath() + "/../swisseph/src/ephe"
    };
    
    QString ephePath;
    for (const QString& path : ephePaths) {
        if (QDir(path).exists()) {
            ephePath = path;
            break;
        }
    }
    
    if (ephePath.isEmpty()) {
        qWarning("Keine Ephemeriden-Dateien gefunden!");
    } else {
        swissEph().setEphePath(ephePath);
    }
    
    // Datenbanken laden
    QString dataPath = QApplication::applicationDirPath() + "/data";
    legacyIO().setDataPath(dataPath);
    orteDB().load(dataPath + "/" + ORTDAT);
    personDB().load(dataPath);
    
    // Farben initialisieren
    initDefaultColors();
}

MainWindow::~MainWindow() {
    saveSettings();
}

//==============================================================================
// Getter
//==============================================================================

AuInit& MainWindow::getAuInit() {
    return m_auinit;
}

Radix& MainWindow::getCurrentRadix() {
    return m_currentRadix;
}

//==============================================================================
// Menü-Setup
//==============================================================================

void MainWindow::setupActions() {
    // Datei-Menü Aktionen
    m_exitAction = new QAction(tr("&Beenden"), this);
    m_exitAction->setShortcut(QKeySequence::Quit);
    connect(m_exitAction, &QAction::triggered, this, &MainWindow::onFileExit);
    
    // Erfassen-Menü Aktionen
    m_personAction = new QAction(tr("&Person erfassen"), this);
    connect(m_personAction, &QAction::triggered, this, &MainWindow::onPersonErfassen);
    
    m_ortAction = new QAction(tr("&Ort erfassen"), this);
    connect(m_ortAction, &QAction::triggered, this, &MainWindow::onOrtErfassen);
    
    // Horoskop-Menü Aktionen
    m_horoTypAction = new QAction(tr("&Horoskoptyp"), this);
    connect(m_horoTypAction, &QAction::triggered, this, &MainWindow::onHoroTyp);
    
    m_orbenAction = new QAction(tr("&Orben"), this);
    connect(m_orbenAction, &QAction::triggered, this, &MainWindow::onOrben);
    
    m_farbenAction = new QAction(tr("&Farben"), this);
    connect(m_farbenAction, &QAction::triggered, this, &MainWindow::onFarben);
    
    m_einstAction = new QAction(tr("&Einstellungen"), this);
    connect(m_einstAction, &QAction::triggered, this, &MainWindow::onEinstellungen);
    
    // Hilfe-Menü Aktionen
    m_helpIndexAction = new QAction(tr("Inde&x"), this);
    m_helpIndexAction->setShortcut(QKeySequence::HelpContents);
    connect(m_helpIndexAction, &QAction::triggered, this, &MainWindow::onHelpIndex);
    
    m_helpAboutAction = new QAction(tr("Ü&ber AstroUniverse"), this);
    connect(m_helpAboutAction, &QAction::triggered, this, &MainWindow::onHelpAbout);
}

void MainWindow::setupMenus() {
    // Datei-Menü (Port von MAINMENU -> "&Datei")
    m_fileMenu = menuBar()->addMenu(tr("&Datei"));
    m_fileMenu->addAction(m_exitAction);
    m_fileMenu->setMinimumWidth(200);  // Mindestbreite für vollständige Texte

    // Erfassen-Menü (Port von MAINMENU -> "&Erfassen")
    m_erfassenMenu = menuBar()->addMenu(tr("&Erfassen"));
    m_erfassenMenu->addAction(m_personAction);
    m_erfassenMenu->addAction(m_ortAction);
    m_erfassenMenu->setMinimumWidth(200);  // Mindestbreite für vollständige Texte
    
    // Horoskop-Menü (Port von MAINMENU -> "&Horoskop")
    m_horoskopMenu = menuBar()->addMenu(tr("&Horoskop"));
    m_horoskopMenu->addAction(m_horoTypAction);
    m_horoskopMenu->addAction(m_orbenAction);
    m_horoskopMenu->addAction(m_farbenAction);
    m_horoskopMenu->addAction(m_einstAction);
    m_horoskopMenu->setMinimumWidth(200);  // Mindestbreite für vollständige Texte
    
    // Hilfe-Menü (Port von MAINMENU -> "&Hilfe")
    m_hilfeMenu = menuBar()->addMenu(tr("&Hilfe"));
    m_hilfeMenu->addAction(m_helpIndexAction);
    m_hilfeMenu->addSeparator();
    m_hilfeMenu->addAction(m_helpAboutAction);
    m_hilfeMenu->setMinimumWidth(250);  // Mindestbreite für vollständige Texte

}

//==============================================================================
// Einstellungen
//==============================================================================

void MainWindow::loadSettings() {
    legacyIO().readIni(m_auinit);
    legacyIO().readColors(sColor, pColor);
}

void MainWindow::saveSettings() {
    legacyIO().writeIni(m_auinit);
    legacyIO().writeColors(sColor, pColor);
}

//==============================================================================
// Datei-Menü Slots
//==============================================================================

void MainWindow::onFileExit() {
    close();
}

//==============================================================================
// Erfassen-Menü Slots
//==============================================================================

void MainWindow::onPersonErfassen() {
    // Port von: DlgPErfassen
    PersonDialog dialog(this, m_auinit, m_currentRadix);
    
    if (dialog.exec() == QDialog::Accepted) {
        // Nur Radix-Tab öffnen wenn berechnet wurde
        if (dialog.wasCalculated()) {
            // Bei Synastrie: Zweite Person auswählen
            if (m_auinit.sSelHoro == TYP_SYNASTRIE) {
                PersonSearchDialog searchDialog2(this);
                searchDialog2.setWindowTitle(tr("Person 2 auswählen"));
                if (searchDialog2.exec() != QDialog::Accepted || !searchDialog2.hasSelection()) {
                    return;  // Abgebrochen
                }
                
                // Zweiten Radix erstellen
                m_currentRadix.synastrie = std::make_shared<Radix>();
                m_currentRadix.synastrie->rFix = searchDialog2.getSelectedPerson();
                m_currentRadix.synastrie->hausSys = m_auinit.sSelHaus;
                m_currentRadix.synastrie->horoTyp = TYP_SYNASTRIE;
                
                // Zweiten Radix berechnen
                int result2 = ChartCalc::calculate(*m_currentRadix.synastrie, nullptr, TYP_SYNASTRIE);
                if (result2 != ERR_OK) {
                    QMessageBox::warning(this, tr("Fehler"), 
                        tr("Fehler bei der Berechnung der zweiten Person (Code: %1)").arg(result2));
                    m_currentRadix.synastrie.reset();
                    return;
                }
                ChartCalc::calcAspects(*m_currentRadix.synastrie, m_auinit.orbenSPlanet);
                
                // Aspekte für Synastrie neu berechnen
                ChartCalc::calcAspects(m_currentRadix, m_auinit.orbenSPlanet);
                ChartCalc::calcAngles(m_currentRadix, m_currentRadix.synastrie.get(), TYP_SYNASTRIE);
                ChartCalc::calcHouseAspects(m_currentRadix, m_auinit.orbenHaus, m_auinit.sAspekte);
            }
            
            // Neues Radix-Widget als Tab erstellen
            RadixWindow* radixWidget = new RadixWindow(m_tabWidget, m_auinit, m_currentRadix);
            int tabIndex = m_tabWidget->addTab(radixWidget, radixWidget->getTabTitle());
            m_tabWidget->setCurrentIndex(tabIndex);
        }
        // Bei OK ohne Berechnung: nur gespeichert, kein Radix-Tab
    }
}

void MainWindow::onOrtErfassen() {
    // Port von: DlgOErfassen
    OrtDialog dialog(this);
    dialog.exec();
}

//==============================================================================
// Horoskop-Menü Slots
//==============================================================================

void MainWindow::onHoroTyp() {
    // Port von: DlgHoroAuswahl
    Horo_typDialog dialog(m_auinit, this);
    
    if (dialog.exec() != QDialog::Accepted) {
        return;
    }
    
    // STRICT LEGACY: Person(en) IMMER neu auswählen
    // Person 1 auswählen
    PersonSearchDialog searchDialog1(this);
    searchDialog1.setWindowTitle(m_auinit.sSelHoro == TYP_SYNASTRIE 
        ? tr("Person 1 auswählen") : tr("Person auswählen"));
    if (searchDialog1.exec() != QDialog::Accepted || !searchDialog1.hasSelection()) {
        return;  // Abgebrochen
    }
    
    // Radix für Person 1 erstellen
    m_currentRadix.clear();
    m_currentRadix.rFix = searchDialog1.getSelectedPerson();
    m_currentRadix.hausSys = m_auinit.sSelHaus;
    m_currentRadix.horoTyp = m_auinit.sSelHoro;
    m_currentRadix.synastrie.reset();
    
    // Bei Synastrie: Person 2 auswählen
    if (m_auinit.sSelHoro == TYP_SYNASTRIE) {
        PersonSearchDialog searchDialog2(this);
        searchDialog2.setWindowTitle(tr("Person 2 auswählen"));
        if (searchDialog2.exec() != QDialog::Accepted || !searchDialog2.hasSelection()) {
            return;  // Abgebrochen
        }
        
        // Zweiten Radix erstellen
        m_currentRadix.synastrie = std::make_shared<Radix>();
        m_currentRadix.synastrie->rFix = searchDialog2.getSelectedPerson();
        m_currentRadix.synastrie->hausSys = m_auinit.sSelHaus;
        m_currentRadix.synastrie->horoTyp = TYP_SYNASTRIE;
        
        // Zweiten Radix berechnen
        int result2 = ChartCalc::calculate(*m_currentRadix.synastrie, nullptr, TYP_SYNASTRIE);
        if (result2 != ERR_OK) {
            QMessageBox::warning(this, tr("Fehler"), 
                tr("Fehler bei der Berechnung der zweiten Person (Code: %1)").arg(result2));
            m_currentRadix.synastrie.reset();
            return;
        }
        ChartCalc::calcAspects(*m_currentRadix.synastrie, m_auinit.orbenSPlanet);
    }
    
    // STRICT LEGACY: Bei Transit erst Basis-Radix berechnen, dann Transit-Dialog öffnen
    if (m_auinit.sSelHoro == TYP_TRANSIT) {
        // Erst Basis-Radix berechnen
        int result = ChartCalc::calculate(m_currentRadix, nullptr, TYP_RADIX);
        if (result != ERR_OK) {
            QMessageBox::warning(this, tr("Fehler"), 
                tr("Fehler bei der Berechnung des Basis-Radix (Code: %1)").arg(result));
            return;
        }
        ChartCalc::calcAspects(m_currentRadix, m_auinit.orbenPlanet);
        
        // Transit-Dialog öffnen (Port von DlgTransit)
        TransitDialog transitDialog(this, m_auinit, m_currentRadix);
        if (transitDialog.exec() != QDialog::Accepted || !transitDialog.wasCalculated()) {
            return;  // Abgebrochen
        }
        
        // Transit-Radix als synastrie setzen (für Anzeige im Chart)
        m_currentRadix.synastrie = std::make_shared<Radix>(transitDialog.getTransitRadix());
        m_currentRadix.horoTyp = TYP_TRANSIT;
        
        // Aspekte zwischen Radix und Transit berechnen
        ChartCalc::calcAspects(m_currentRadix, m_auinit.orbenTPlanet);
        ChartCalc::calcAngles(m_currentRadix, m_currentRadix.synastrie.get(), TYP_TRANSIT);
        ChartCalc::calcHouseAspects(m_currentRadix, m_auinit.orbenHaus, m_auinit.sAspekte);
        
        // Transit-Auswahl-Dialog (Port von DlgTransEin)
        TransSelDialog transSelDialog(this);
        transSelDialog.setSelection(transitDialog.getTransSel());
        if (transSelDialog.exec() != QDialog::Accepted) {
            return;
        }
        QVector<QVector<bool>> transSel = transSelDialog.selection();
        
        // STRICT LEGACY: Transit-Ergebnis-Fenster öffnen (Port von WndTransit)
        TransitResultWindow* transitResult = new TransitResultWindow(m_tabWidget, m_auinit, m_currentRadix);
        transitResult->setTransits(transitDialog.getTransits(), 
                                   transitDialog.getTransitAspekte(),
                                   transitDialog.getVonDatum(),
                                   transitDialog.getBisDatum(),
                                   transSel);
        transitResult->setTransitSelection(transSel);
        
        // Transits kopieren für Lambda (Dialog wird nach Scope zerstört)
        QVector<Radix> transits = transitDialog.getTransits();
        Radix basisRadix = m_currentRadix;
        
        // Signal für Grafik-Anzeige verbinden
        connect(transitResult, &TransitResultWindow::requestGraphic, this, [this, transits, basisRadix, transSel](int transitIndex) {
            if (transitIndex >= 0 && transitIndex < transits.size()) {
                Radix transitRadix = basisRadix;
                transitRadix.synastrie = std::make_shared<Radix>(transits.at(transitIndex));
                transitRadix.horoTyp = TYP_TRANSIT;
                
                RadixWindow* radixWidget = new RadixWindow(m_tabWidget, m_auinit, transitRadix);
                radixWidget->setTransitSelection(transSel);
                int tabIndex = m_tabWidget->addTab(radixWidget, radixWidget->getTabTitle());
                m_tabWidget->setCurrentIndex(tabIndex);
            }
        });
        
        // Nur Transit-Ergebnis-Tab öffnen, kein automatisches Chart
        // Chart wird erst bei Auswahl aus der Liste geöffnet (Grafik-Button oder Doppelklick)
        int tabIndex = m_tabWidget->addTab(transitResult, transitResult->getTabTitle());
        m_tabWidget->setCurrentIndex(tabIndex);
        
        emit settingsChanged();
        return;
    }
    
    // Ersten Radix berechnen (Radix oder Synastrie)
    int result = ChartCalc::calculate(m_currentRadix, 
        m_currentRadix.synastrie.get(), m_auinit.sSelHoro);
    
    if (result == ERR_OK) {
        // Aspekte mit passenden Orben berechnen
        if (m_auinit.sSelHoro == TYP_SYNASTRIE) {
            ChartCalc::calcAspects(m_currentRadix, m_auinit.orbenSPlanet);
        } else {
            ChartCalc::calcAspects(m_currentRadix, m_auinit.orbenPlanet);
        }
        ChartCalc::calcAngles(m_currentRadix, m_currentRadix.synastrie.get(), m_auinit.sSelHoro);
        // Häuser-Aspekte (nur wenn in den Einstellungen aktiviert)
        ChartCalc::calcHouseAspects(m_currentRadix, m_auinit.orbenHaus, m_auinit.sAspekte);
        
        // Neues Radix-Widget als Tab erstellen
        RadixWindow* radixWidget = new RadixWindow(m_tabWidget, m_auinit, m_currentRadix);
        int tabIndex = m_tabWidget->addTab(radixWidget, radixWidget->getTabTitle());
        m_tabWidget->setCurrentIndex(tabIndex);
    } else {
        QMessageBox::warning(this, tr("Fehler"), 
            tr("Fehler bei der Berechnung (Code: %1)").arg(result));
    }
    
    emit settingsChanged();
}

void MainWindow::onOrben() {
    // Port von: DlgOrbenEin
    OrbenDialog dialog(m_auinit, this);
    
    if (dialog.exec() == QDialog::Accepted) {
        // Einstellungen sofort speichern
        saveSettings();
        emit settingsChanged();
    }
}

void MainWindow::onFarben() {
    // Port von: DlgFarben
    FarbenDialog dialog(m_auinit, this);
    
    if (dialog.exec() == QDialog::Accepted) {
        emit settingsChanged();
    }
}

void MainWindow::onEinstellungen() {
    // Port von: EINSTELL Dialog
    SettingsDialog dialog(m_auinit, this);
    
    if (dialog.exec() == QDialog::Accepted) {
        emit settingsChanged();
    }
}

//==============================================================================
// Hilfe-Menü Slots
//==============================================================================

void MainWindow::onHelpIndex() {
    // TODO: Hilfe-System implementieren
    QMessageBox::information(this, tr("Hilfe"), 
                            tr("Hilfe-System noch nicht implementiert."));
}

void MainWindow::onHelpAbout() {
    // Port von: szAbout
    QMessageBox::about(this, tr("Über AstroUniverse"),
        tr("<h2>AstroUniverse 2026</h2>"
           "<p>Version 1.0.0</p>"
           "<p>Basierend auf AstroUniverse v0.04Beta</p>"
           "<p>by A.S. and J.L.</p>"
           "<p>©1997-2026</p>"
           "<p>Portiert nach Qt6/C++20</p>"));
}

//==============================================================================
// Tab-Verwaltung
//==============================================================================

void MainWindow::onTabCloseRequested(int index) {
    // Tab schließen und Widget löschen
    QWidget* widget = m_tabWidget->widget(index);
    m_tabWidget->removeTab(index);
    delete widget;
}

} // namespace astro
