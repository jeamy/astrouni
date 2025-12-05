#pragma once
/**
 * @file legacy_io.h
 * @brief 1:1 Port der Datei-I/O Funktionen aus legacy/io.c und astrofil.c
 * 
 * Liest und schreibt die Legacy .dat Dateien im Original-Format.
 */

#include <QString>
#include <QVector>
#include "../core/data_types.h"

namespace astro {

/**
 * @brief Legacy-Datei I/O
 * 
 * Verwaltet das Lesen und Schreiben der .dat Dateien im Legacy-Format.
 */
class LegacyIO {
public:
    LegacyIO();
    ~LegacyIO();
    
    //==========================================================================
    // Pfad-Verwaltung
    //==========================================================================
    
    /**
     * @brief Setzt den Basis-Pfad für Daten-Dateien
     */
    void setDataPath(const QString& path);
    
    /**
     * @brief Gibt den Basis-Pfad zurück
     */
    QString getDataPath() const;
    
    //==========================================================================
    // INI-Datei (astroini.dat)
    //==========================================================================
    
    /**
     * @brief Liest die Initialisierungs-Datei
     * @param auinit [out] Einstellungen
     * @return ERR_OK bei Erfolg
     * 
     * Port von: sReadIni(void)
     */
    int readIni(AuInit& auinit);
    
    /**
     * @brief Schreibt die Initialisierungs-Datei
     * @param auinit Einstellungen
     * @return ERR_OK bei Erfolg
     * 
     * Port von: sWriteini(void)
     */
    int writeIni(const AuInit& auinit);
    
    //==========================================================================
    // Orben-Datei (default.dat)
    //==========================================================================
    
    /**
     * @brief Liest die Orben aus der Default-Datei
     * @param auinit [out] Einstellungen mit Orben
     * @return ERR_OK bei Erfolg
     * 
     * Port von: sOrben(FILE*)
     */
    int readOrben(AuInit& auinit);
    
    /**
     * @brief Schreibt die Orben in die Default-Datei
     * @param auinit Einstellungen mit Orben
     * @return ERR_OK bei Erfolg
     * 
     * Port von: sWriteOrb(FILE*)
     */
    int writeOrben(const AuInit& auinit);
    
    //==========================================================================
    // Farben
    //==========================================================================
    
    /**
     * @brief Liest die Farb-Einstellungen
     * @param screenColors [out] Bildschirm-Farben
     * @param printColors [out] Druck-Farben
     * @return ERR_OK bei Erfolg
     * 
     * Port von: sReadDefColor(FILE*, short, long)
     */
    int readColors(QColor* screenColors, QColor* printColors);
    
    /**
     * @brief Schreibt die Farb-Einstellungen
     * @param screenColors Bildschirm-Farben
     * @param printColors Druck-Farben
     * @return ERR_OK bei Erfolg
     * 
     * Port von: sWriteDefColor(void)
     */
    int writeColors(const QColor* screenColors, const QColor* printColors);
    
    //==========================================================================
    // Radix-Daten
    //==========================================================================
    
    /**
     * @brief Liest Radix-Daten aus der Datenbank
     * @param radix [out] Radix-Daten
     * @param name Name der Person
     * @return ERR_OK bei Erfolg
     * 
     * Port von: sReadDat(RADIX*, HWND, char*)
     */
    int readRadix(Radix& radix, const QString& name);
    
    /**
     * @brief Schreibt Radix-Daten in die Datenbank
     * @param radix Radix-Daten
     * @return ERR_OK bei Erfolg
     * 
     * Port von: sWriteDat(RADIX*, HWND)
     */
    int writeRadix(const Radix& radix);
    
    /**
     * @brief Löscht Radix-Daten aus der Datenbank
     * @param radix Radix-Daten
     * @param index Index in der Datenbank
     * @return ERR_OK bei Erfolg
     * 
     * Port von: sDeleteDat(RADIX*, HWND, short)
     */
    int deleteRadix(const Radix& radix, int index);
    
    //==========================================================================
    // Fehlerbehandlung
    //==========================================================================
    
    /**
     * @brief Gibt die letzte Fehlermeldung zurück
     */
    QString getLastError() const;
    
private:
    QString m_dataPath;
    QString m_lastError;
    
    // Hilfsfunktionen
    QString getFilePath(const QString& filename) const;
    void setError(const QString& error);
    
    // Legacy-Format Konvertierung
    uint32_t colorToLegacy(const QColor& color) const;
    QColor legacyToColor(uint32_t legacy) const;
};

/**
 * @brief Globale LegacyIO Instanz
 */
LegacyIO& legacyIO();

} // namespace astro
