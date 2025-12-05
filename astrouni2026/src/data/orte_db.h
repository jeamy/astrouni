#pragma once
/**
 * @file orte_db.h
 * @brief Orte-Datenbank Verwaltung
 * 
 * 1:1 Port der Orte-Funktionen aus legacy/astrofil.c
 */

#include <QString>
#include <QVector>
#include "../core/data_types.h"

namespace astro {

/**
 * @brief Orte-Datenbank
 * 
 * Verwaltet die Orte-Datenbank (astroger.dat, europa.dat, astroorg.dat)
 */
class OrteDB {
public:
    OrteDB();
    ~OrteDB();
    
    //==========================================================================
    // Datenbank laden
    //==========================================================================
    
    /**
     * @brief Lädt die Orte-Datenbank
     * @param filepath Pfad zur .dat Datei
     * @return Anzahl geladener Orte, oder -1 bei Fehler
     */
    int load(const QString& filepath);
    
    /**
     * @brief Lädt alle Orte-Datenbanken aus einem Verzeichnis
     * @param dataPath Pfad zum Datenverzeichnis
     * @return Anzahl geladener Orte
     */
    int loadAll(const QString& dataPath);
    
    /**
     * @brief Gibt die Anzahl der Orte zurück
     */
    int count() const;
    
    /**
     * @brief Prüft ob die Datenbank geladen ist
     */
    bool isLoaded() const;
    
    //==========================================================================
    // Suche
    //==========================================================================
    
    /**
     * @brief Sucht nach Orten
     * @param suchtext Suchtext (Anfang des Ortsnamens)
     * @param maxResults Maximale Anzahl Ergebnisse
     * @return Liste der gefundenen Orte
     * 
     * Port von: sSearchOrt(HWND, short, char*, ORTE*)
     */
    QVector<Orte> search(const QString& suchtext, int maxResults = 100) const;
    
    /**
     * @brief Gibt einen Ort nach Index zurück
     * @param index Index in der Datenbank
     * @return Ort, oder leerer Ort wenn nicht gefunden
     */
    Orte getByIndex(int index) const;
    
    /**
     * @brief Sucht einen Ort nach exaktem Namen
     * @param name Ortsname
     * @return Ort, oder leerer Ort wenn nicht gefunden
     */
    Orte getByName(const QString& name) const;
    
    //==========================================================================
    // Bearbeiten
    //==========================================================================
    
    /**
     * @brief Fügt einen neuen Ort hinzu
     * @param ort Ort-Daten
     * @return Index des neuen Orts, oder -1 bei Fehler
     * 
     * Port von: sAddOrt(HWND, short, ORTE*, char*)
     */
    int add(const Orte& ort);
    
    /**
     * @brief Aktualisiert einen Ort
     * @param index Index des Orts
     * @param ort Neue Ort-Daten
     * @return true bei Erfolg
     */
    bool update(int index, const Orte& ort);
    
    /**
     * @brief Löscht einen Ort
     * @param index Index des Orts
     * @return true bei Erfolg
     */
    bool remove(int index);
    
    //==========================================================================
    // Speichern
    //==========================================================================
    
    /**
     * @brief Speichert die Datenbank
     * @param filepath Pfad zur .dat Datei (optional, sonst Original-Pfad)
     * @return true bei Erfolg
     * 
     * Port von: sOrteWrite(HWND)
     */
    bool save(const QString& filepath = QString());
    
    //==========================================================================
    // Fehlerbehandlung
    //==========================================================================
    
    QString getLastError() const;
    
private:
    QVector<Orte> m_orte;
    QString m_filepath;
    QString m_lastError;
    bool m_loaded;
    bool m_modified;
    
    void setError(const QString& error);
};

/**
 * @brief Globale Orte-Datenbank Instanz
 */
OrteDB& orteDB();

} // namespace astro
