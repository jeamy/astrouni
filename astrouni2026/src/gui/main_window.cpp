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
#include <QDialog>
#include <QVBoxLayout>
#include <QTextBrowser>
#include <QDialogButtonBox>

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
    
    m_aspekteAction = new QAction(tr("&Aspekte..."), this);
    connect(m_aspekteAction, &QAction::triggered, this, &MainWindow::onAspekte);
    
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
    m_horoskopMenu->addAction(m_aspekteAction);
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

void MainWindow::onAspekte() {
    // Port von: DlgAspekte - ruft SettingsDialog auf (gleiche Funktion)
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
    QString text;

    // Radix
    text += tr("<h3>Radix</h3>");
    text += tr("<p><b>1. Person erfassen</b><br>" 
               "Menü &quot;Erfassen → Person…&quot; öffnen. "
               "Name, Geburtsdatum, Geburtszeit und Geburtsort eingeben. "
               "Den Ort bei Bedarf über &quot;Ort suchen&quot; auswählen.</p>");
    text += tr("<p><b>2. Horoskoptyp / Häusersystem</b><br>" 
               "Über &quot;Horoskop → Horoskoptyp…&quot; den Typ &quot;Radix&quot; "
               "und das gewünschte Häusersystem wählen.</p>");
    text += tr("<p><b>3. Berechnung und Anzeige</b><br>" 
               "Im Personendialog &quot;OK&quot; wählen. "
               "Bei erfolgreicher Berechnung wird ein neues Radix-Tab geöffnet. "
               "Rechts können über die Schaltflächen &quot;Daten&quot;, &quot;Positionen&quot; "
               "und &quot;Aspekte&quot; die Listen gewechselt werden.</p>");

    // Synastrie
    text += tr("<h3>Synastrie</h3>");
    text += tr("<p><b>1. Horoskoptyp wählen</b><br>" 
               "Unter &quot;Horoskop → Horoskoptyp…&quot; den Typ &quot;Synastrie&quot; wählen.</p>");
    text += tr("<p><b>2. Person 1</b><br>" 
               "Über &quot;Erfassen → Person…&quot; die erste Person erfassen oder aus der Datenbank laden "
               "und berechnen lassen.</p>");
    text += tr("<p><b>3. Person 2</b><br>" 
               "Nach &quot;OK&quot; im Personendialog im Dialog &quot;Person 2 auswählen&quot; "
               "eine zweite Person aus der Liste wählen. "
               "Das Programm berechnet automatisch die Synastrie-Aspekte.</p>");
    text += tr("<p><b>4. Anzeige</b><br>" 
               "Im neuen Synastrie-Tab werden innen die Radix-Planeten der ersten Person "
               "und außen die Planeten der zweiten Person gezeigt. "
               "Die Aspekteliste enthält Radix- und Synastrie-Aspekte.</p>");

    // Transit
    text += tr("<h3>Transit</h3>");
    text += tr("<p><b>1. Horoskoptyp wählen</b><br>" 
               "Unter &quot;Horoskop → Horoskoptyp…&quot; den Typ &quot;Transit&quot; wählen. "
               "Der zugrunde liegende Radix wird aus der aktuellen Person übernommen.</p>");
    text += tr("<p><b>2. Transit-Dialog</b><br>" 
               "Im Transitdialog Start-/Enddatum bzw. Suchzeitraum einstellen.</p>");
    text += tr("<p><b>3. Transit-Auswahl (Matrix)</b><br>" 
               "In der Auswahlmatrix festlegen, welche Transit-Planeten auf welche Radix-Planeten "
               "oder Häuser Aspekte bilden dürfen. Ganze Zeilen/Spalten können gemeinsam ein- oder "
               "ausgeschaltet werden.</p>");
    text += tr("<p><b>4. Ergebnisliste und Grafik</b><br>" 
               "Nach der Berechnung zeigt das Programm eine Liste gefundener Transite. "
               "Über die Schaltfläche &quot;Grafik&quot; kann zu einem Eintrag ein Transit-Radix geöffnet werden. "
               "Innen stehen die Radix-Planeten, außen die Transit-Planeten; es werden nur die "
               "in der Matrix erlaubten Aspekte gezeichnet.</p>");

    // Einstellungen / Orben / Farben / Aspekte
    text += tr("<h3>Einstellungen, Orben, Farben, Aspekte</h3>");
    text += tr("<p><b>Aspekte / Einstellungen</b><br>" 
               "Unter &quot;Horoskop → Aspekte…&quot; können Aspektarten (Konjunktion, Sextil, Quadrat, "
               "Trigon, Opposition, Halbsextil, Quincunx), die Grad-Teilung (3er/9er) "
               "und welche Objekte (Planeten, Asteroiden, Häuser) Aspekte bilden, ein- oder "
               "ausgeschaltet werden.</p>");
    text += tr("<p><b>Orben</b><br>" 
               "Unter &quot;Horoskop → Orben…&quot; werden die zulässigen Orben für Planet–Planet- und "
               "Planet–Haus-Aspekte in Matrizen eingestellt. &quot;Default&quot; stellt die in der INI "
               "hinterlegten Standardwerte wieder her.</p>");
    text += tr("<p><b>Farben</b><br>" 
               "Unter &quot;Horoskop → Farben…&quot; können die Farben von Hintergrund, Kreisen, "
               "Aspektlinien und Planeten angepasst werden.</p>");

    // Personen / Orte erfassen
    text += tr("<h3>Personen und Orte erfassen</h3>");
    text += tr("<p><b>Personen</b><br>" 
               "Menü &quot;Erfassen → Person…&quot; öffnet den Personendialog. "
               "Hier werden Name, Geburtsdaten, Ort und Zeitzone eingegeben. "
               "Über &quot;Namen suchen&quot; kann eine Person aus der Datenbank gewählt werden.</p>");
    text += tr("<p><b>Orte</b><br>" 
               "Im Personendialog über &quot;Ort suchen&quot; einen Ort aus der Orteliste wählen oder "
               "einen neuen Ort erfassen. Bei Namens-Duplikaten zeigt der Ort-Duplikat-Dialog "
               "vorhandene Einträge an; einer davon kann übernommen werden.</p>");

    // Scrollbare Hilfe über eigenen Dialog
    QDialog dlg(this);
    dlg.setWindowTitle(tr("Hilfe – Index"));
    dlg.setModal(true);

    QVBoxLayout* layout = new QVBoxLayout(&dlg);
    QTextBrowser* browser = new QTextBrowser(&dlg);
    browser->setReadOnly(true);
    browser->setHtml(text);
    browser->setMinimumSize(640, 480);
    layout->addWidget(browser);

    QDialogButtonBox* buttons = new QDialogButtonBox(QDialogButtonBox::Ok, &dlg);
    connect(buttons, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    layout->addWidget(buttons);

    dlg.exec();
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
