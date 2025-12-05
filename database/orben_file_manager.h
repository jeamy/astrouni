#pragma once

#include <string>
#include <fstream>
#include "astro/orben_data.h"

// 1:1 Port von sOpenIni, sReadIniOrb, sWriteOrb, sOrben aus legacy/auhelper.c

class OrbenFileManager {
public:
    OrbenFileManager();
    
    // Lädt Orben-Daten aus astroini.dat (gespeicherte Werte)
    bool LoadFromIniFile(OrbenData& data, const std::string& filepath = "data/astroini.dat");
    
    // Speichert Orben-Daten in astroini.dat
    bool SaveToIniFile(const OrbenData& data, const std::string& filepath = "data/astroini.dat");
    
    // Lädt Default-Orben aus default.dat (absolute Defaults)
    bool LoadFromDefaultFile(OrbenData& data, const std::string& filepath = "data/default.dat");
    
    // Initialisiert mit Hard-Coded Defaults (falls keine Dateien vorhanden)
    void InitializeHardcodedDefaults(OrbenData& data);

private:
    // Seek-Positionen in astroini.dat (wie im Legacy-Code)
    struct SeekPositions {
        long lStart;    // Start der Datei
        long lOrben;    // Position der Orben-Daten
        long lTrans;    // Position der Transit-Selektion
        long lFarben;   // Position der Farben
    };
    
    SeekPositions seek_positions_;
    
    // Hilfsfunktionen
    bool ReadSeekPositions(std::istream& file);
    bool WriteSeekPositions(std::ostream& file);
    bool ReadOrbenData(std::istream& file, OrbenData& data);
    bool WriteOrbenData(std::ostream& file, const OrbenData& data);
};
