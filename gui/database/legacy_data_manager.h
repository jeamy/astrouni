// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (C) 2025 AstroUniverse Project
// 1:1 Port of Legacy .dat file management

#pragma once

#include <string>
#include <vector>
#include <map>
#include "astro/data_types.h"

// Legacy-Datenstrukturen (1:1 aus astrouni.h)
struct LegacySettings {
    // Farben (Screen/Print/Mono)
    struct Colors {
        uint32_t background;
        uint32_t circles;
        uint32_t houses;
        uint32_t planets;
        uint32_t aspects_harmonious;
        uint32_t aspects_disharmonious;
        uint32_t text;
        uint32_t zodiac;
    } screen_colors, print_colors, mono_colors;
    
    // Orbis-Werte
    struct Orbs {
        double sun_moon;        // Sonne-Mond
        double sun_planets;     // Sonne-Planeten
        double moon_planets;    // Mond-Planeten
        double planets_planets; // Planeten-Planeten
        double minor_aspects;   // Nebenaspekte
    } orbs;
    
    // Häusersystem
    astro::HouseSystem house_system;
    
    // Sonstige Einstellungen
    bool show_aspects;
    bool show_houses;
    bool show_degrees;
    bool show_retrograde;
};

struct LegacyPerson {
    std::string name;
    std::string first_name;
    int day, month, year;
    int hour, minute;
    double latitude, longitude;
    int altitude;
    std::string place;
    std::string country;
    std::string timezone;
};

struct LegacyPlace {
    std::string name;
    std::string country;
    double latitude, longitude;
    int altitude;
    std::string timezone;
};

// 1:1 Legacy .dat File Manager
class LegacyDataManager {
public:
    LegacyDataManager();
    ~LegacyDataManager();
    
    // Legacy .dat Files laden (1:1 Binary Format)
    bool LoadDefaultSettings(const std::string& filename = "default.dat");
    bool LoadUserSettings(const std::string& filename = "astroini.dat");
    bool LoadPersonDatabase(const std::string& filename = "astronam.dat");
    bool LoadPlaceDatabase(const std::string& filename = "astroorg.dat");
    bool LoadEuropaPlaces(const std::string& filename = "europa.dat");
    bool LoadGermanTexts(const std::string& filename = "astroger.dat");
    bool LoadEphemeris(const std::string& filename = "astroeph.dat");
    
    // Legacy .dat Files speichern (1:1 Binary Format)
    bool SaveDefaultSettings(const std::string& filename = "default.dat");
    bool SaveUserSettings(const std::string& filename = "astroini.dat");
    bool SavePersonDatabase(const std::string& filename = "astronam.dat");
    bool SavePlaceDatabase(const std::string& filename = "astroorg.dat");
    
    // Daten-Zugriff
    const LegacySettings& GetSettings() const { return settings_; }
    void SetSettings(const LegacySettings& settings) { settings_ = settings; }
    
    const std::vector<LegacyPerson>& GetPersons() const { return persons_; }
    const std::vector<LegacyPlace>& GetPlaces() const { return places_; }
    const std::vector<LegacyPlace>& GetEuropaPlaces() const { return europa_places_; }
    
    // Suche-Funktionen
    std::vector<size_t> FindPersons(const std::string& search) const;
    std::vector<size_t> FindPlaces(const std::string& search) const;
    
    // Person/Ort hinzufügen/ändern/löschen
    bool AddPerson(const LegacyPerson& person);
    bool UpdatePerson(size_t index, const LegacyPerson& person);
    bool DeletePerson(size_t index);
    
    bool AddPlace(const LegacyPlace& place);
    bool UpdatePlace(size_t index, const LegacyPlace& place);
    bool DeletePlace(size_t index);
    
    // Fehler-Handling
    std::string GetLastError() const { return last_error_; }
    bool HasError() const { return !last_error_.empty(); }
    void ClearError() { last_error_.clear(); }
    
private:
    // Daten
    LegacySettings settings_;
    std::vector<LegacyPerson> persons_;
    std::vector<LegacyPlace> places_;
    std::vector<LegacyPlace> europa_places_;
    std::map<std::string, std::string> german_texts_;
    
    // Fehler-Handling
    std::string last_error_;
    
    // Hilfsfunktionen für Binary I/O (1:1 Legacy Format)
    bool ReadBinaryString(std::ifstream& file, std::string& str, size_t max_len = 256);
    bool WriteBinaryString(std::ofstream& file, const std::string& str);
    bool ReadBinaryInt(std::ifstream& file, int& value);
    bool WriteBinaryInt(std::ofstream& file, int value);
    bool ReadBinaryDouble(std::ifstream& file, double& value);
    bool WriteBinaryDouble(std::ofstream& file, double value);
    
    void SetError(const std::string& error) { last_error_ = error; }
    void InitializeDefaultSettings();
    void LoadStandardPlaces();  // Standard-Orte laden (1:1 Legacy)
};
