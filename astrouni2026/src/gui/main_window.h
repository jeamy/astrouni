#pragma once
/**
 * @file main_window.h
 * @brief Hauptfenster der Anwendung
 * 
 * 1:1 Port von MainWndProc aus legacy/astrouni.c
 */

#include <QMainWindow>
#include <QMenu>
#include <QAction>
#include <QList>
#include <QTabWidget>
#include "../core/data_types.h"

namespace astro {

class RadixWindow;
class PrintManager;

/**
 * @brief Hauptfenster der Anwendung
 * 
 * Port von: MainWndProc, InitApplication, InitInstance
 */
class MainWindow : public QMainWindow {
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();
    
    /**
     * @brief Gibt die globalen Einstellungen zurück
     */
    AuInit& getAuInit();
    
    /**
     * @brief Gibt die aktuelle Radix zurück
     */
    Radix& getCurrentRadix();
    
signals:
    /**
     * @brief Signal wenn sich die Einstellungen geändert haben
     */
    void settingsChanged();
    
private slots:
    // Datei-Menü
    void onFilePrint();
    void onFilePrinterSetup();
    void onFileExit();
    
    // Erfassen-Menü
    void onPersonErfassen();
    void onOrtErfassen();
    
    // Horoskop-Menü
    void onHoroTyp();
    void onOrben();
    void onFarben();
    void onAspekte();
    
    // Hilfe-Menü
    void onHelpIndex();
    void onHelpAbout();
    
    // Radix-Tabs
    void onTabCloseRequested(int index);
    
private:
    void setupMenus();
    void setupActions();
    void loadSettings();
    void saveSettings();
    
    // Menüs
    QMenu* m_fileMenu;
    QMenu* m_erfassenMenu;
    QMenu* m_horoskopMenu;
    QMenu* m_hilfeMenu;
    
    // Aktionen
    QAction* m_printAction;
    QAction* m_printerSetupAction;
    QAction* m_exitAction;
    QAction* m_personAction;
    QAction* m_ortAction;
    QAction* m_horoTypAction;
    QAction* m_orbenAction;
    QAction* m_farbenAction;
    QAction* m_aspekteAction;
    QAction* m_helpIndexAction;
    QAction* m_helpAboutAction;
    
    // Daten
    AuInit m_auinit;
    Radix m_currentRadix;
    
    // Tab-Widget für Radix-Anzeigen
    QTabWidget* m_tabWidget;
    
    // Druck-Manager
    PrintManager* m_printManager;
};

} // namespace astro
