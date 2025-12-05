/**
 * @file main_window.cpp
 * @brief Implementierung des Hauptfensters
 * 
 * 1:1 Port von MainWndProc aus legacy/astrouni.c
 */

#include "main_window.h"
#include "radix_window.h"
#include "dialogs/person_dialog.h"
#include "dialogs/ort_dialog.h"
#include "dialogs/horo_typ_dialog.h"
#include "dialogs/orben_dialog.h"
#include "dialogs/farben_dialog.h"
#include "dialogs/settings_dialog.h"
#include "../data/legacy_io.h"
#include "../data/orte_db.h"
#include "../data/person_db.h"
#include "../core/swiss_eph.h"

#include <QMenuBar>
#include <QMessageBox>
#include <QApplication>
#include <QDir>
#include <QIcon>

namespace astro {

//==============================================================================
// Konstruktor / Destruktor
//==============================================================================

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , m_radixWindow(nullptr) {
    
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
    
    // Menüs und Aktionen einrichten
    setupActions();
    setupMenus();
    
    // Einstellungen laden
    loadSettings();
    
    // Swiss Ephemeris initialisieren
    QString ephePath = QApplication::applicationDirPath() + "/swisseph/ephe";
    swissEph().setEphePath(ephePath);
    
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
    m_personAction = new QAction(tr("&Person erfassen..."), this);
    connect(m_personAction, &QAction::triggered, this, &MainWindow::onPersonErfassen);
    
    m_ortAction = new QAction(tr("&Ort erfassen..."), this);
    connect(m_ortAction, &QAction::triggered, this, &MainWindow::onOrtErfassen);
    
    // Horoskop-Menü Aktionen
    m_horoTypAction = new QAction(tr("&Horoskoptyp..."), this);
    connect(m_horoTypAction, &QAction::triggered, this, &MainWindow::onHoroTyp);
    
    m_orbenAction = new QAction(tr("&Orben..."), this);
    connect(m_orbenAction, &QAction::triggered, this, &MainWindow::onOrben);
    
    m_farbenAction = new QAction(tr("&Farben..."), this);
    connect(m_farbenAction, &QAction::triggered, this, &MainWindow::onFarben);
    
    m_einstAction = new QAction(tr("&Einstellungen..."), this);
    connect(m_einstAction, &QAction::triggered, this, &MainWindow::onEinstellungen);
    
    // Hilfe-Menü Aktionen
    m_helpIndexAction = new QAction(tr("Inde&x"), this);
    m_helpIndexAction->setShortcut(QKeySequence::HelpContents);
    connect(m_helpIndexAction, &QAction::triggered, this, &MainWindow::onHelpIndex);
    
    m_helpAboutAction = new QAction(tr("Ü&ber AstroUniverse..."), this);
    connect(m_helpAboutAction, &QAction::triggered, this, &MainWindow::onHelpAbout);
}

void MainWindow::setupMenus() {
    // Datei-Menü (Port von MAINMENU -> "&Datei")
    m_fileMenu = menuBar()->addMenu(tr("&Datei"));
    m_fileMenu->addAction(m_exitAction);
    
    // Erfassen-Menü (Port von MAINMENU -> "&Erfassen")
    m_erfassenMenu = menuBar()->addMenu(tr("&Erfassen"));
    m_erfassenMenu->addAction(m_personAction);
    m_erfassenMenu->addAction(m_ortAction);
    
    // Horoskop-Menü (Port von MAINMENU -> "&Horoskop")
    m_horoskopMenu = menuBar()->addMenu(tr("&Horoskop"));
    m_horoskopMenu->addAction(m_horoTypAction);
    m_horoskopMenu->addAction(m_orbenAction);
    m_horoskopMenu->addAction(m_farbenAction);
    m_horoskopMenu->addAction(m_einstAction);
    
    // Hilfe-Menü (Port von MAINMENU -> "&Hilfe")
    m_hilfeMenu = menuBar()->addMenu(tr("&Hilfe"));
    m_hilfeMenu->addAction(m_helpIndexAction);
    m_hilfeMenu->addSeparator();
    m_hilfeMenu->addAction(m_helpAboutAction);
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
        // Radix berechnen und Fenster öffnen
        if (m_radixWindow == nullptr) {
            m_radixWindow = new RadixWindow(this, m_auinit, m_currentRadix);
            connect(m_radixWindow, &RadixWindow::destroyed, 
                    this, &MainWindow::onRadixWindowClosed);
        }
        m_radixWindow->setRadix(m_currentRadix);
        m_radixWindow->show();
        m_radixWindow->raise();
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
    Horo_typDialog dialog(this);
    
    if (dialog.exec() == QDialog::Accepted) {
        emit settingsChanged();
    }
}

void MainWindow::onOrben() {
    // Port von: DlgOrbenEin
    OrbenDialog dialog(this);
    
    if (dialog.exec() == QDialog::Accepted) {
        emit settingsChanged();
    }
}

void MainWindow::onFarben() {
    // Port von: DlgFarben
    FarbenDialog dialog(this);
    
    if (dialog.exec() == QDialog::Accepted) {
        emit settingsChanged();
    }
}

void MainWindow::onEinstellungen() {
    // Port von: EINSTELL Dialog
    SettingsDialog dialog(this);
    
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
// Radix-Fenster
//==============================================================================

void MainWindow::onRadixWindowClosed() {
    m_radixWindow = nullptr;
}

} // namespace astro
