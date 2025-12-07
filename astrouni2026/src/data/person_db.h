#pragma once
/**
 * @file person_db.h
 * @brief Personen-Datenbank Verwaltung
 * 
 * 1:1 Port der Personen-Funktionen aus legacy/astrofil.c
 */

#include <QString>
#include <QVector>
#include <QDate>
#include "../core/data_types.h"

namespace astro {

/**
 * @brief Personen-Datenbank
 * 
 * Verwaltet die Personen-Datenbank (astronam.dat, astrorad.dat, astronot.dat)
 */
class PersonDB {
public:
    PersonDB();
    ~PersonDB();
    
    //==========================================================================
    // Datenbank laden
    //==========================================================================
    
    /**
     * @brief Lädt die Personen-Datenbank
     * @param dataPath Pfad zum Daten-Verzeichnis
     * @return Anzahl geladener Personen, oder -1 bei Fehler
     */
    int load(const QString& dataPath);
    
    /**
     * @brief Gibt die Anzahl der Personen zurück
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
     * @brief Sucht nach Personen
     * @param suchtext Suchtext (Name oder Vorname)
     * @param maxResults Maximale Anzahl Ergebnisse
     * @return Liste der gefundenen Personen (nur Stammdaten)
     * 
     * Port von: sGetName(RADIX*, RADIX*, HWND, short, char*)
     */
    QVector<RadixFix> search(const QString& suchtext, int maxResults = 100) const;
    
    /**
     * @brief Gibt alle Personen zurück
     * @return Liste aller Personen (nur Stammdaten)
     * 
     * Port von: sFillNameLB(HWND, short, short)
     */
    QVector<RadixFix> getAll() const;
    
    /**
     * @brief Lädt vollständige Radix-Daten einer Person
     * @param index Index in der Datenbank
     * @param radix [out] Radix-Daten
     * @return true bei Erfolg
     */
    bool loadRadix(int index, Radix& radix) const;
    
    //==========================================================================
    // Bearbeiten
    //==========================================================================
    
    /**
     * @brief Fügt eine neue Person hinzu
     * @param radix Radix-Daten
     * @return Index der neuen Person, oder -1 bei Fehler
     */
    int add(const Radix& radix);
    
    /**
     * @brief Aktualisiert eine Person
     * @param index Index der Person
     * @param radix Neue Radix-Daten
     * @return true bei Erfolg
     */
    bool update(int index, const Radix& radix);
    
    /**
     * @brief Löscht eine Person
     * @param index Index der Person
     * @return true bei Erfolg
     */
    bool remove(int index);
    
    //==========================================================================
    // Duplikat-Prüfung
    //==========================================================================
    
    /**
     * @brief Prüft ob eine Person bereits existiert
     * @param name Name
     * @param vorname Vorname
     * @param datum Geburtsdatum
     * @return Index der existierenden Person, oder -1 wenn nicht gefunden
     * 
     * Port von: sGetValidRecords(RADIX*, RADIX*, short, short*)
     */
    int findDuplicate(const QString& name, const QString& vorname, 
                      const QDate& datum) const;
    
    //==========================================================================
    // Speichern
    //==========================================================================
    
    /**
     * @brief Speichert die Datenbank
     * @return true bei Erfolg
     */
    bool save();
    
    //==========================================================================
    // Fehlerbehandlung
    //==========================================================================
    
    QString getLastError() const;
    
private:
    QVector<RadixFix> m_personen;
    QString m_dataPath;
    QString m_lastError;
    bool m_loaded;
    bool m_modified;
    
    void setError(const QString& error);
    
    // Datei-Pfade
    QString getNameFilePath() const;
    QString getRadixFilePath() const;
    QString getNotizFilePath() const;
};

/**
 * @brief Globale Personen-Datenbank Instanz
 */
PersonDB& personDB();

} // namespace astro
