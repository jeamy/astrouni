/**
 * @file main.cpp
 * @brief Haupteinstiegspunkt der Anwendung
 * 
 * 1:1 Port von WinMain aus legacy/astrouni.c
 */

#include <QApplication>
#include <QDir>
#include <QStandardPaths>
#include <QMessageBox>

#include "gui/main_window.h"
#include "core/swiss_eph.h"
#include "core/data_types.h"
#include "data/legacy_io.h"
#include "data/orte_db.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    // Anwendungs-Informationen
    app.setApplicationName("AstroUniverse");
    app.setApplicationVersion("2026.1.0");
    app.setOrganizationName("AstroUniverse");
    
    // Farben initialisieren
    astro::initDefaultColors();
    
    // Pfade ermitteln
    QString appPath = QApplication::applicationDirPath();
    QString dataPath = appPath + "/data";
    QString ephePath = appPath + "/swisseph/ephe";
    
    // Prüfen ob Daten-Verzeichnis existiert
    if (!QDir(dataPath).exists()) {
        // Versuche im übergeordneten Verzeichnis
        dataPath = QDir(appPath).filePath("../data");
        if (!QDir(dataPath).exists()) {
            QMessageBox::warning(nullptr, "Warnung",
                QString("Daten-Verzeichnis nicht gefunden:\n%1\n\n"
                        "Bitte stellen Sie sicher, dass die Daten-Dateien vorhanden sind.")
                .arg(dataPath));
        }
    }
    
    // Prüfen ob Ephemeriden-Verzeichnis existiert
    if (!QDir(ephePath).exists()) {
        ephePath = QDir(appPath).filePath("../swisseph/ephe");
        if (!QDir(ephePath).exists()) {
            QMessageBox::warning(nullptr, "Warnung",
                QString("Ephemeriden-Verzeichnis nicht gefunden:\n%1\n\n"
                        "Bitte laden Sie die Swiss Ephemeris Dateien herunter.")
                .arg(ephePath));
        }
    }
    
    // Swiss Ephemeris initialisieren
    astro::swissEph().setEphePath(ephePath);
    
    // Legacy I/O initialisieren
    astro::legacyIO().setDataPath(dataPath);
    
    // Orte-Datenbank laden (alle Dateien: astroorg.dat, astroger.dat, europa.dat)
    int orteCount = astro::orteDB().loadAll(dataPath);
    if (orteCount == 0) {
        QMessageBox::warning(nullptr, "Warnung",
            QString("Keine Orte-Datenbank gefunden in:\n%1\n\n"
                    "Erwartet: astroorg.dat, astroger.dat, europa.dat")
            .arg(dataPath));
    }
    
    // Hauptfenster erstellen und anzeigen
    astro::MainWindow mainWindow;
    mainWindow.show();
    
    return app.exec();
}
