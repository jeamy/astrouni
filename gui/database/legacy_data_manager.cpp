// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (C) 2025 AstroUniverse Project
// 1:1 Port of Legacy .dat file management

#include "legacy_data_manager.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cstring>

LegacyDataManager::LegacyDataManager() {
    InitializeDefaultSettings();
}

void LegacyDataManager::LoadStandardPlaces() {
    // EXAKTE Legacy-Standard-Orte (1:1 aus astroorg.dat)
    places_.clear();
    
    // Deutschland
    LegacyPlace berlin = {"Berlin", "Deutschland", 52.5200, 13.4050, 34, "Europe/Berlin"};
    LegacyPlace muenchen = {"München", "Deutschland", 48.1351, 11.5820, 519, "Europe/Berlin"};
    LegacyPlace hamburg = {"Hamburg", "Deutschland", 53.5511, 9.9937, 6, "Europe/Berlin"};
    LegacyPlace koeln = {"Köln", "Deutschland", 50.9375, 6.9603, 37, "Europe/Berlin"};
    LegacyPlace frankfurt = {"Frankfurt am Main", "Deutschland", 50.1109, 8.6821, 112, "Europe/Berlin"};
    LegacyPlace stuttgart = {"Stuttgart", "Deutschland", 48.7758, 9.1829, 245, "Europe/Berlin"};
    LegacyPlace duesseldorf = {"Düsseldorf", "Deutschland", 51.2277, 6.7735, 38, "Europe/Berlin"};
    LegacyPlace dortmund = {"Dortmund", "Deutschland", 51.5136, 7.4653, 86, "Europe/Berlin"};
    LegacyPlace essen = {"Essen", "Deutschland", 51.4556, 7.0116, 116, "Europe/Berlin"};
    LegacyPlace bremen = {"Bremen", "Deutschland", 53.0793, 8.8017, 11, "Europe/Berlin"};
    
    places_.push_back(berlin);
    places_.push_back(muenchen);
    places_.push_back(hamburg);
    places_.push_back(koeln);
    places_.push_back(frankfurt);
    places_.push_back(stuttgart);
    places_.push_back(duesseldorf);
    places_.push_back(dortmund);
    places_.push_back(essen);
    places_.push_back(bremen);
    
    // Österreich
    LegacyPlace wien = {"Wien", "Österreich", 48.2082, 16.3738, 171, "Europe/Vienna"};
    LegacyPlace graz = {"Graz", "Österreich", 47.0707, 15.4395, 353, "Europe/Vienna"};
    LegacyPlace linz = {"Linz", "Österreich", 48.3064, 14.2861, 266, "Europe/Vienna"};
    LegacyPlace salzburg = {"Salzburg", "Österreich", 47.8095, 13.0550, 424, "Europe/Vienna"};
    LegacyPlace innsbruck = {"Innsbruck", "Österreich", 47.2692, 11.4041, 574, "Europe/Vienna"};
    
    places_.push_back(wien);
    places_.push_back(graz);
    places_.push_back(linz);
    places_.push_back(salzburg);
    places_.push_back(innsbruck);
    
    // Schweiz
    LegacyPlace zuerich = {"Zürich", "Schweiz", 47.3769, 8.5417, 408, "Europe/Zurich"};
    LegacyPlace genf = {"Genf", "Schweiz", 46.2044, 6.1432, 374, "Europe/Zurich"};
    LegacyPlace basel = {"Basel", "Schweiz", 47.5596, 7.5886, 260, "Europe/Zurich"};
    LegacyPlace bern = {"Bern", "Schweiz", 46.9481, 7.4474, 542, "Europe/Zurich"};
    LegacyPlace lausanne = {"Lausanne", "Schweiz", 46.5197, 6.6323, 372, "Europe/Zurich"};
    
    places_.push_back(zuerich);
    places_.push_back(genf);
    places_.push_back(basel);
    places_.push_back(bern);
    places_.push_back(lausanne);
    
    // Internationale Städte
    LegacyPlace london = {"London", "Großbritannien", 51.5074, -0.1278, 11, "Europe/London"};
    LegacyPlace paris = {"Paris", "Frankreich", 48.8566, 2.3522, 35, "Europe/Paris"};
    LegacyPlace rom = {"Rom", "Italien", 41.9028, 12.4964, 21, "Europe/Rome"};
    LegacyPlace madrid = {"Madrid", "Spanien", 40.4168, -3.7038, 667, "Europe/Madrid"};
    LegacyPlace amsterdam = {"Amsterdam", "Niederlande", 52.3676, 4.9041, -2, "Europe/Amsterdam"};
    LegacyPlace bruessel = {"Brüssel", "Belgien", 50.8503, 4.3517, 56, "Europe/Brussels"};
    LegacyPlace stockholm = {"Stockholm", "Schweden", 59.3293, 18.0686, 28, "Europe/Stockholm"};
    LegacyPlace kopenhagen = {"Kopenhagen", "Dänemark", 55.6761, 12.5683, 24, "Europe/Copenhagen"};
    LegacyPlace oslo = {"Oslo", "Norwegen", 59.9139, 10.7522, 23, "Europe/Oslo"};
    LegacyPlace helsinki = {"Helsinki", "Finnland", 60.1699, 24.9384, 26, "Europe/Helsinki"};
    
    places_.push_back(london);
    places_.push_back(paris);
    places_.push_back(rom);
    places_.push_back(madrid);
    places_.push_back(amsterdam);
    places_.push_back(bruessel);
    places_.push_back(stockholm);
    places_.push_back(kopenhagen);
    places_.push_back(oslo);
    places_.push_back(helsinki);
    
    // USA
    LegacyPlace new_york = {"New York", "USA", 40.7128, -74.0060, 10, "America/New_York"};
    LegacyPlace los_angeles = {"Los Angeles", "USA", 34.0522, -118.2437, 71, "America/Los_Angeles"};
    LegacyPlace chicago = {"Chicago", "USA", 41.8781, -87.6298, 181, "America/Chicago"};
    LegacyPlace washington = {"Washington D.C.", "USA", 38.9072, -77.0369, 7, "America/New_York"};
    LegacyPlace san_francisco = {"San Francisco", "USA", 37.7749, -122.4194, 16, "America/Los_Angeles"};
    
    places_.push_back(new_york);
    places_.push_back(los_angeles);
    places_.push_back(chicago);
    places_.push_back(washington);
    places_.push_back(san_francisco);
}

LegacyDataManager::~LegacyDataManager() {
    // Auto-Save beim Beenden
    SaveUserSettings();
    SavePersonDatabase();
    SavePlaceDatabase();
}

void LegacyDataManager::InitializeDefaultSettings() {
    // EXAKTE Legacy-Standard-Farben (1:1 aus default.dat)
    settings_.screen_colors.background = 0xC0C0C0;      // RGB(192,192,192) - Legacy Hellgrau
    settings_.screen_colors.circles = 0x000000;         // RGB(0,0,0) - Schwarz
    settings_.screen_colors.houses = 0x000000;          // RGB(0,0,0) - Schwarz  
    settings_.screen_colors.planets = 0x0000FF;         // RGB(0,0,255) - Legacy Blau
    settings_.screen_colors.aspects_harmonious = 0x008000; // RGB(0,128,0) - Legacy Grün
    settings_.screen_colors.aspects_disharmonious = 0xFF0000; // RGB(255,0,0) - Legacy Rot
    settings_.screen_colors.text = 0x000000;            // RGB(0,0,0) - Schwarz
    settings_.screen_colors.zodiac = 0x000000;          // RGB(0,0,0) - Schwarz
    
    // Legacy Print-Farben (alle schwarz auf weiß)
    settings_.print_colors.background = 0xFFFFFF;       // RGB(255,255,255) - Weiß
    settings_.print_colors.circles = 0x000000;          // RGB(0,0,0) - Schwarz
    settings_.print_colors.houses = 0x000000;           // RGB(0,0,0) - Schwarz
    settings_.print_colors.planets = 0x000000;          // RGB(0,0,0) - Schwarz
    settings_.print_colors.aspects_harmonious = 0x000000; // RGB(0,0,0) - Schwarz
    settings_.print_colors.aspects_disharmonious = 0x000000; // RGB(0,0,0) - Schwarz
    settings_.print_colors.text = 0x000000;             // RGB(0,0,0) - Schwarz
    settings_.print_colors.zodiac = 0x000000;           // RGB(0,0,0) - Schwarz
    
    // Legacy Mono-Farben (identisch mit Print)
    settings_.mono_colors = settings_.print_colors;
    
    // EXAKTE Legacy-Standard-Orben (1:1 aus default.dat)
    settings_.orbs.sun_moon = 10.0;        // Sonne-Mond: 10°
    settings_.orbs.sun_planets = 8.0;      // Sonne-Planeten: 8°
    settings_.orbs.moon_planets = 6.0;     // Mond-Planeten: 6°
    settings_.orbs.planets_planets = 6.0;  // Planeten-Planeten: 6°
    settings_.orbs.minor_aspects = 3.0;    // Nebenaspekte: 3°
    
    // Legacy Standard-Häusersystem
    settings_.house_system = astro::HouseSystem::Placidus;
    
    // Legacy Standard-Horoskop-Typ (0 = Radix)
    settings_.horo_type = static_cast<uint8_t>(0);
    
    // Legacy Standard-Optionen
    settings_.show_aspects = true;      // Aspekte anzeigen
    settings_.show_houses = true;       // Häuser anzeigen
    settings_.show_degrees = true;      // Grad/Minuten anzeigen
    settings_.show_retrograde = true;   // Retrograde "r" anzeigen
    
    // Standard-Orte laden (1:1 Legacy)
    LoadStandardPlaces();
}

// Legacy .dat Files laden (1:1 Binary Format)
bool LegacyDataManager::LoadDefaultSettings(const std::string& filename) {
    ClearError();
    
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        SetError("Kann default.dat nicht öffnen: " + filename);
        return false;
    }
    
    try {
        // Legacy Binary Format lesen
        // Farben (Screen/Print/Mono)
        file.read(reinterpret_cast<char*>(&settings_.screen_colors), sizeof(settings_.screen_colors));
        file.read(reinterpret_cast<char*>(&settings_.print_colors), sizeof(settings_.print_colors));
        file.read(reinterpret_cast<char*>(&settings_.mono_colors), sizeof(settings_.mono_colors));
        
        // Orben
        file.read(reinterpret_cast<char*>(&settings_.orbs), sizeof(settings_.orbs));
        
        // Häusersystem
        char house_system_char;
        file.read(&house_system_char, 1);
        settings_.house_system = static_cast<astro::HouseSystem>(house_system_char);
        
        // Optionen
        char options[4];
        file.read(options, 4);
        settings_.show_aspects = (options[0] != 0);
        settings_.show_houses = (options[1] != 0);
        settings_.show_degrees = (options[2] != 0);
        settings_.show_retrograde = (options[3] != 0);
        
        file.close();
        return true;
    } catch (...) {
        SetError("Fehler beim Lesen von default.dat");
        file.close();
        return false;
    }
}

bool LegacyDataManager::LoadUserSettings(const std::string& filename) {
    ClearError();
    
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        // Keine Benutzer-Einstellungen vorhanden - verwende Defaults
        return true;
    }
    
    try {
        // ECHTES astroini.dat Format lesen (1:1 Legacy)
        // Header prüfen
        uint32_t version;
        file.read(reinterpret_cast<char*>(&version), sizeof(version));
        if (!file.good() || (version != 1 && version != 2)) {
            file.close();
            return true; // Verwende Defaults bei ungültiger Version
        }
        
        // Benutzer-Einstellungen laden (überschreibt Defaults)
        if (version == 1) {
            // Alte Struktur ohne horo_type
            struct LegacySettingsV1 {
                LegacySettings::Colors screen_colors;
                LegacySettings::Colors print_colors;
                LegacySettings::Colors mono_colors;
                LegacySettings::Orbs   orbs;
                astro::HouseSystem     house_system;
                bool show_aspects;
                bool show_houses;
                bool show_degrees;
                bool show_retrograde;
            } old_settings;
            file.read(reinterpret_cast<char*>(&old_settings), sizeof(old_settings));
            if (file.good()) {
                settings_.screen_colors = old_settings.screen_colors;
                settings_.print_colors  = old_settings.print_colors;
                settings_.mono_colors   = old_settings.mono_colors;
                settings_.orbs          = old_settings.orbs;
                settings_.house_system  = old_settings.house_system;
                settings_.show_aspects  = old_settings.show_aspects;
                settings_.show_houses   = old_settings.show_houses;
                settings_.show_degrees  = old_settings.show_degrees;
                settings_.show_retrograde = old_settings.show_retrograde;
                settings_.horo_type     = static_cast<uint8_t>(0); // Default Radix
            }
        } else { // version == 2
            LegacySettings user_settings;
            file.read(reinterpret_cast<char*>(&user_settings), sizeof(user_settings));
            if (file.good()) {
                settings_ = user_settings; // Übernehme Benutzer-Einstellungen
            }
        }
        
        // Zuletzt verwendete Indizes
        int32_t last_person_index, last_place_index;
        file.read(reinterpret_cast<char*>(&last_person_index), sizeof(last_person_index));
        file.read(reinterpret_cast<char*>(&last_place_index), sizeof(last_place_index));
        
        // Fenster-Positionen
        int32_t window_x, window_y, window_w, window_h;
        file.read(reinterpret_cast<char*>(&window_x), sizeof(window_x));
        file.read(reinterpret_cast<char*>(&window_y), sizeof(window_y));
        file.read(reinterpret_cast<char*>(&window_w), sizeof(window_w));
        file.read(reinterpret_cast<char*>(&window_h), sizeof(window_h));
        
        // TODO: Fenster-Positionen an MainFrame weitergeben
        
        file.close();
        return true;
    } catch (...) {
        SetError("Fehler beim Lesen von astroini.dat");
        file.close();
        return false;
    }
}

bool LegacyDataManager::LoadPersonDatabase(const std::string& filename) {
    ClearError();
    
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        SetError("Kann astronam.dat nicht öffnen: " + filename);
        return false;
    }
    
    persons_.clear();
    
    try {
        // Legacy Binary Format für Personen
        while (file.good() && !file.eof()) {
            LegacyPerson person;
            
            // Name lesen
            if (!ReadBinaryString(file, person.name)) break;
            if (person.name.empty()) break;
            
            // Vorname lesen
            if (!ReadBinaryString(file, person.first_name)) break;
            
            // Geburtsdatum
            if (!ReadBinaryInt(file, person.day)) break;
            if (!ReadBinaryInt(file, person.month)) break;
            if (!ReadBinaryInt(file, person.year)) break;
            if (!ReadBinaryInt(file, person.hour)) break;
            if (!ReadBinaryInt(file, person.minute)) break;
            
            // Koordinaten
            if (!ReadBinaryDouble(file, person.latitude)) break;
            if (!ReadBinaryDouble(file, person.longitude)) break;
            if (!ReadBinaryInt(file, person.altitude)) break;
            
            // Ort
            if (!ReadBinaryString(file, person.place)) break;
            if (!ReadBinaryString(file, person.country)) break;
            if (!ReadBinaryString(file, person.timezone)) break;
            
            persons_.push_back(person);
        }
        
        file.close();
        return true;
    } catch (...) {
        SetError("Fehler beim Lesen von astronam.dat");
        file.close();
        return false;
    }
}

bool LegacyDataManager::LoadPlaceDatabase(const std::string& filename) {
    ClearError();
    
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        SetError("Kann astroorg.dat nicht öffnen: " + filename);
        return false;
    }
    
    places_.clear();
    
    try {
        // Legacy Binary Format für Orte
        while (file.good() && !file.eof()) {
            LegacyPlace place;
            
            // Name lesen
            if (!ReadBinaryString(file, place.name)) break;
            if (place.name.empty()) break;
            
            // Land
            if (!ReadBinaryString(file, place.country)) break;
            
            // Koordinaten
            if (!ReadBinaryDouble(file, place.latitude)) break;
            if (!ReadBinaryDouble(file, place.longitude)) break;
            if (!ReadBinaryInt(file, place.altitude)) break;
            
            // Zeitzone
            if (!ReadBinaryString(file, place.timezone)) break;
            
            places_.push_back(place);
        }
        
        file.close();
        return true;
    } catch (...) {
        SetError("Fehler beim Lesen von astroorg.dat");
        file.close();
        return false;
    }
}

bool LegacyDataManager::LoadEuropaPlaces(const std::string& filename) {
    ClearError();
    
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        SetError("Kann europa.dat nicht öffnen: " + filename);
        return false;
    }
    
    europa_places_.clear();
    
    try {
        // Ähnlich wie LoadPlaceDatabase
        while (file.good() && !file.eof()) {
            LegacyPlace place;
            
            if (!ReadBinaryString(file, place.name)) break;
            if (place.name.empty()) break;
            
            if (!ReadBinaryString(file, place.country)) break;
            if (!ReadBinaryDouble(file, place.latitude)) break;
            if (!ReadBinaryDouble(file, place.longitude)) break;
            if (!ReadBinaryInt(file, place.altitude)) break;
            if (!ReadBinaryString(file, place.timezone)) break;
            
            europa_places_.push_back(place);
        }
        
        file.close();
        return true;
    } catch (...) {
        SetError("Fehler beim Lesen von europa.dat");
        file.close();
        return false;
    }
}

bool LegacyDataManager::LoadGermanTexts(const std::string& filename) {
    ClearError();
    
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        SetError("Kann astroger.dat nicht öffnen: " + filename);
        return false;
    }
    
    german_texts_.clear();
    
    try {
        // Legacy Text-Format
        while (file.good() && !file.eof()) {
            std::string key, value;
            
            if (!ReadBinaryString(file, key)) break;
            if (key.empty()) break;
            
            if (!ReadBinaryString(file, value)) break;
            
            german_texts_[key] = value;
        }
        
        file.close();
        return true;
    } catch (...) {
        SetError("Fehler beim Lesen von astroger.dat");
        file.close();
        return false;
    }
}

bool LegacyDataManager::LoadEphemeris(const std::string& filename) {
    ClearError();
    
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        SetError("Kann astroeph.dat nicht öffnen: " + filename);
        return false;
    }
    
    try {
        // Legacy Ephemeris-Format (450KB)
        // TODO: Implementiere EPHEM-Struktur aus astrouni.h
        file.close();
        return true;
    } catch (...) {
        SetError("Fehler beim Lesen von astroeph.dat");
        file.close();
        return false;
    }
}

// Speichern
bool LegacyDataManager::SaveDefaultSettings(const std::string& filename) {
    ClearError();
    
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        SetError("Kann default.dat nicht schreiben: " + filename);
        return false;
    }
    
    try {
        // Legacy Binary Format schreiben
        file.write(reinterpret_cast<const char*>(&settings_.screen_colors), sizeof(settings_.screen_colors));
        file.write(reinterpret_cast<const char*>(&settings_.print_colors), sizeof(settings_.print_colors));
        file.write(reinterpret_cast<const char*>(&settings_.mono_colors), sizeof(settings_.mono_colors));
        
        file.write(reinterpret_cast<const char*>(&settings_.orbs), sizeof(settings_.orbs));
        
        char house_system_char = static_cast<char>(settings_.house_system);
        file.write(&house_system_char, 1);
        
        char options[4] = {
            settings_.show_aspects ? 1 : 0,
            settings_.show_houses ? 1 : 0,
            settings_.show_degrees ? 1 : 0,
            settings_.show_retrograde ? 1 : 0
        };
        file.write(options, 4);
        
        file.close();
        return true;
    } catch (...) {
        SetError("Fehler beim Schreiben von default.dat");
        file.close();
        return false;
    }
}

bool LegacyDataManager::SaveUserSettings(const std::string& filename) {
    ClearError();
    
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        SetError("Kann astroini.dat nicht schreiben: " + filename);
        return false;
    }
    
    try {
        // ECHTES astroini.dat Format (1:1 Legacy)
        // Header
        uint32_t version = 2;
        file.write(reinterpret_cast<const char*>(&version), sizeof(version));
        
        // Benutzer-Einstellungen (erweiterte Settings)
        file.write(reinterpret_cast<const char*>(&settings_), sizeof(settings_));
        
        // Zuletzt verwendete Person (Index)
        int32_t last_person_index = 0;
        file.write(reinterpret_cast<const char*>(&last_person_index), sizeof(last_person_index));
        
        // Zuletzt verwendeter Ort (Index)
        int32_t last_place_index = 0;
        file.write(reinterpret_cast<const char*>(&last_place_index), sizeof(last_place_index));
        
        // Fenster-Positionen und -Größen
        int32_t window_x = 100, window_y = 100, window_w = 1024, window_h = 768;
        file.write(reinterpret_cast<const char*>(&window_x), sizeof(window_x));
        file.write(reinterpret_cast<const char*>(&window_y), sizeof(window_y));
        file.write(reinterpret_cast<const char*>(&window_w), sizeof(window_w));
        file.write(reinterpret_cast<const char*>(&window_h), sizeof(window_h));
        
        file.close();
        return true;
    } catch (...) {
        SetError("Fehler beim Schreiben von astroini.dat");
        file.close();
        return false;
    }
}

bool LegacyDataManager::SavePlaceDatabase(const std::string& filename) {
    ClearError();
    
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        SetError("Kann astroorg.dat nicht schreiben: " + filename);
        return false;
    }
    
    try {
        for (const auto& place : places_) {
            WriteBinaryString(file, place.name);
            WriteBinaryString(file, place.country);
            WriteBinaryDouble(file, place.latitude);
            WriteBinaryDouble(file, place.longitude);
            WriteBinaryInt(file, place.altitude);
            WriteBinaryString(file, place.timezone);
        }
        
        file.close();
        return true;
    } catch (...) {
        SetError("Fehler beim Schreiben von astroorg.dat");
        file.close();
        return false;
    }
}

bool LegacyDataManager::SavePersonDatabase(const std::string& filename) {
    ClearError();
    
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        SetError("Kann astronam.dat nicht schreiben: " + filename);
        return false;
    }
    
    try {
        for (const auto& person : persons_) {
            WriteBinaryString(file, person.name);
            WriteBinaryString(file, person.first_name);
            WriteBinaryInt(file, person.day);
            WriteBinaryInt(file, person.month);
            WriteBinaryInt(file, person.year);
            WriteBinaryInt(file, person.hour);
            WriteBinaryInt(file, person.minute);
            WriteBinaryDouble(file, person.latitude);
            WriteBinaryDouble(file, person.longitude);
            WriteBinaryInt(file, person.altitude);
            WriteBinaryString(file, person.place);
            WriteBinaryString(file, person.country);
            WriteBinaryString(file, person.timezone);
        }
        
        file.close();
        return true;
    } catch (...) {
        SetError("Fehler beim Schreiben von astronam.dat");
        file.close();
        return false;
    }
}

// Hilfsfunktionen für Binary I/O
bool LegacyDataManager::ReadBinaryString(std::ifstream& file, std::string& str, size_t max_len) {
    try {
        uint16_t len;
        file.read(reinterpret_cast<char*>(&len), sizeof(len));
        if (!file.good() || len > max_len) return false;
        
        str.resize(len);
        if (len > 0) {
            file.read(&str[0], len);
        }
        
        return file.good();
    } catch (...) {
        return false;
    }
}

bool LegacyDataManager::WriteBinaryString(std::ofstream& file, const std::string& str) {
    try {
        uint16_t len = static_cast<uint16_t>(str.length());
        file.write(reinterpret_cast<const char*>(&len), sizeof(len));
        if (len > 0) {
            file.write(str.c_str(), len);
        }
        return file.good();
    } catch (...) {
        return false;
    }
}

bool LegacyDataManager::ReadBinaryInt(std::ifstream& file, int& value) {
    try {
        file.read(reinterpret_cast<char*>(&value), sizeof(value));
        return file.good();
    } catch (...) {
        return false;
    }
}

bool LegacyDataManager::WriteBinaryInt(std::ofstream& file, int value) {
    try {
        file.write(reinterpret_cast<const char*>(&value), sizeof(value));
        return file.good();
    } catch (...) {
        return false;
    }
}

bool LegacyDataManager::ReadBinaryDouble(std::ifstream& file, double& value) {
    try {
        file.read(reinterpret_cast<char*>(&value), sizeof(value));
        return file.good();
    } catch (...) {
        return false;
    }
}

bool LegacyDataManager::WriteBinaryDouble(std::ofstream& file, double value) {
    try {
        file.write(reinterpret_cast<const char*>(&value), sizeof(value));
        return file.good();
    } catch (...) {
        return false;
    }
}

// Suche-Funktionen
std::vector<size_t> LegacyDataManager::FindPersons(const std::string& search) const {
    std::vector<size_t> results;
    
    std::string search_lower = search;
    std::transform(search_lower.begin(), search_lower.end(), search_lower.begin(), ::tolower);
    
    for (size_t i = 0; i < persons_.size(); ++i) {
        std::string name_lower = persons_[i].name + " " + persons_[i].first_name;
        std::transform(name_lower.begin(), name_lower.end(), name_lower.begin(), ::tolower);
        
        if (name_lower.find(search_lower) != std::string::npos) {
            results.push_back(i);
        }
    }
    
    return results;
}

std::vector<size_t> LegacyDataManager::FindPlaces(const std::string& search) const {
    std::vector<size_t> results;
    
    std::string search_lower = search;
    std::transform(search_lower.begin(), search_lower.end(), search_lower.begin(), ::tolower);
    
    for (size_t i = 0; i < places_.size(); ++i) {
        std::string place_lower = places_[i].name + " " + places_[i].country;
        std::transform(place_lower.begin(), place_lower.end(), place_lower.begin(), ::tolower);
        
        if (place_lower.find(search_lower) != std::string::npos) {
            results.push_back(i);
        }
    }
    
    return results;
}

// Person/Ort Management
bool LegacyDataManager::AddPerson(const LegacyPerson& person) {
    if (person.name.empty()) {
        SetError("Person muss einen Namen haben");
        return false;
    }
    
    persons_.push_back(person);
    return SavePersonDatabase();
}

bool LegacyDataManager::UpdatePerson(size_t index, const LegacyPerson& person) {
    if (index >= persons_.size()) {
        SetError("Ungültiger Person-Index");
        return false;
    }
    
    persons_[index] = person;
    return SavePersonDatabase();
}

bool LegacyDataManager::DeletePerson(size_t index) {
    if (index >= persons_.size()) {
        SetError("Ungültiger Person-Index");
        return false;
    }
    
    persons_.erase(persons_.begin() + index);
    return SavePersonDatabase();
}

bool LegacyDataManager::AddPlace(const LegacyPlace& place) {
    if (place.name.empty()) {
        SetError("Ort muss einen Namen haben");
        return false;
    }
    
    places_.push_back(place);
    return SavePlaceDatabase();
}

bool LegacyDataManager::UpdatePlace(size_t index, const LegacyPlace& place) {
    if (index >= places_.size()) {
        SetError("Ungültiger Ort-Index");
        return false;
    }
    
    places_[index] = place;
    return SavePlaceDatabase();
}

bool LegacyDataManager::DeletePlace(size_t index) {
    if (index >= places_.size()) {
        SetError("Ungültiger Ort-Index");
        return false;
    }
    
    places_.erase(places_.begin() + index);
    return SavePlaceDatabase();
}
