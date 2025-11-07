// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (C) 2025 AstroUniverse Project
// 1:1 Port of astrofil.c database functions

#include "radix_database.h"
#include <iostream>
#include <sstream>
#include <algorithm>

RadixDatabase::RadixDatabase() {
    // Standard-Orte laden (1:1 Legacy)
    astro::GeoLocation berlin;
    berlin.latitude = 52.5200;
    berlin.longitude = 13.4050;
    berlin.altitude = 34;
    berlin.country_code = "Deutschland";
    berlin.timezone = "Europe/Berlin";
    orte_.push_back(std::make_pair("Berlin", berlin));
    
    astro::GeoLocation muenchen;
    muenchen.latitude = 48.1351;
    muenchen.longitude = 11.5820;
    muenchen.altitude = 519;
    muenchen.country_code = "Deutschland";
    muenchen.timezone = "Europe/Berlin";
    orte_.push_back(std::make_pair("München", muenchen));
    
    astro::GeoLocation wien;
    wien.latitude = 48.2082;
    wien.longitude = 16.3738;
    wien.altitude = 171;
    wien.country_code = "Österreich";
    wien.timezone = "Europe/Vienna";
    orte_.push_back(std::make_pair("Wien", wien));
    
    astro::GeoLocation zuerich;
    zuerich.latitude = 47.3769;
    zuerich.longitude = 8.5417;
    zuerich.altitude = 408;
    zuerich.country_code = "Schweiz";
    zuerich.timezone = "Europe/Zurich";
    orte_.push_back(std::make_pair("Zürich", zuerich));
}

RadixDatabase::~RadixDatabase() {
    // Auto-Save beim Beenden
    SavePersonen();
    SaveOrte();
}

// Personen-Datenbank (1:1 Legacy)
bool RadixDatabase::LoadPersonen(const std::string& filename) {
    ClearError();
    
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        SetError("Kann Personen-Datei nicht öffnen: " + filename);
        return false;
    }
    
    personen_.clear();
    
    while (file.good() && !file.eof()) {
        astro::RadixData person;
        if (ReadPersonFromFile(file, person)) {
            personen_.push_back(person);
        }
    }
    
    file.close();
    return true;
}

bool RadixDatabase::SavePersonen(const std::string& filename) {
    ClearError();
    
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        SetError("Kann Personen-Datei nicht schreiben: " + filename);
        return false;
    }
    
    for (const auto& person : personen_) {
        if (!WritePersonToFile(file, person)) {
            file.close();
            return false;
        }
    }
    
    file.close();
    return true;
}

bool RadixDatabase::AddPerson(const astro::RadixData& person) {
    if (person.name.empty()) {
        SetError("Person muss einen Namen haben");
        return false;
    }
    
    personen_.push_back(person);
    return SavePersonen();
}

bool RadixDatabase::DeletePerson(size_t index) {
    if (index >= personen_.size()) {
        SetError("Ungültiger Person-Index");
        return false;
    }
    
    personen_.erase(personen_.begin() + index);
    return SavePersonen();
}

bool RadixDatabase::UpdatePerson(size_t index, const astro::RadixData& person) {
    if (index >= personen_.size()) {
        SetError("Ungültiger Person-Index");
        return false;
    }
    
    personen_[index] = person;
    return SavePersonen();
}

astro::RadixData RadixDatabase::GetPerson(size_t index) const {
    if (index >= personen_.size()) {
        return astro::RadixData();
    }
    return personen_[index];
}

// Orte-Datenbank (1:1 Legacy)
bool RadixDatabase::LoadOrte(const std::string& filename) {
    ClearError();
    
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        SetError("Kann Orte-Datei nicht öffnen: " + filename);
        return false;
    }
    
    orte_.clear();
    
    while (file.good() && !file.eof()) {
        std::string name;
        astro::GeoLocation ort;
        if (ReadOrtFromFile(file, name, ort)) {
            orte_.push_back(std::make_pair(name, ort));
        }
    }
    
    file.close();
    return true;
}

bool RadixDatabase::SaveOrte(const std::string& filename) {
    ClearError();
    
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        SetError("Kann Orte-Datei nicht schreiben: " + filename);
        return false;
    }
    
    for (const auto& ort_pair : orte_) {
        if (!WriteOrtToFile(file, ort_pair.first, ort_pair.second)) {
            file.close();
            return false;
        }
    }
    
    file.close();
    return true;
}

bool RadixDatabase::AddOrt(const astro::GeoLocation& ort, const std::string& name) {
    if (name.empty()) {
        SetError("Ort muss einen Namen haben");
        return false;
    }
    
    orte_.push_back(std::make_pair(name, ort));
    return SaveOrte();
}

bool RadixDatabase::DeleteOrt(size_t index) {
    if (index >= orte_.size()) {
        SetError("Ungültiger Ort-Index");
        return false;
    }
    
    orte_.erase(orte_.begin() + index);
    return SaveOrte();
}

bool RadixDatabase::UpdateOrt(size_t index, const astro::GeoLocation& ort, const std::string& name) {
    if (index >= orte_.size()) {
        SetError("Ungültiger Ort-Index");
        return false;
    }
    
    orte_[index] = std::make_pair(name, ort);
    return SaveOrte();
}

std::pair<std::string, astro::GeoLocation> RadixDatabase::GetOrt(size_t index) const {
    if (index >= orte_.size()) {
        return std::make_pair("", astro::GeoLocation());
    }
    return orte_[index];
}

// Suche-Funktionen (1:1 Legacy)
std::vector<size_t> RadixDatabase::FindPersonen(const std::string& suchtext) const {
    std::vector<size_t> results;
    
    std::string search_lower = suchtext;
    std::transform(search_lower.begin(), search_lower.end(), search_lower.begin(), ::tolower);
    
    for (size_t i = 0; i < personen_.size(); ++i) {
        std::string name_lower = personen_[i].name;
        std::transform(name_lower.begin(), name_lower.end(), name_lower.begin(), ::tolower);
        
        if (name_lower.find(search_lower) != std::string::npos) {
            results.push_back(i);
        }
    }
    
    return results;
}

std::vector<size_t> RadixDatabase::FindOrte(const std::string& suchtext) const {
    std::vector<size_t> results;
    
    std::string search_lower = suchtext;
    std::transform(search_lower.begin(), search_lower.end(), search_lower.begin(), ::tolower);
    
    for (size_t i = 0; i < orte_.size(); ++i) {
        std::string name_lower = orte_[i].first;
        std::transform(name_lower.begin(), name_lower.end(), name_lower.begin(), ::tolower);
        
        std::string country_lower = orte_[i].second.country_code;
        std::transform(country_lower.begin(), country_lower.end(), country_lower.begin(), ::tolower);
        
        if (name_lower.find(search_lower) != std::string::npos ||
            country_lower.find(search_lower) != std::string::npos) {
            results.push_back(i);
        }
    }
    
    return results;
}

// Hilfsfunktionen (1:1 Legacy Dateiformat)
bool RadixDatabase::WritePersonToFile(std::ofstream& file, const astro::RadixData& person) {
    try {
        // Name (String mit Länge)
        uint32_t name_len = static_cast<uint32_t>(person.name.length());
        file.write(reinterpret_cast<const char*>(&name_len), sizeof(name_len));
        file.write(person.name.c_str(), name_len);
        
        // Geburtsdatum
        file.write(reinterpret_cast<const char*>(&person.birth_datetime.year), sizeof(person.birth_datetime.year));
        file.write(reinterpret_cast<const char*>(&person.birth_datetime.month), sizeof(person.birth_datetime.month));
        file.write(reinterpret_cast<const char*>(&person.birth_datetime.day), sizeof(person.birth_datetime.day));
        file.write(reinterpret_cast<const char*>(&person.birth_datetime.hour), sizeof(person.birth_datetime.hour));
        file.write(reinterpret_cast<const char*>(&person.birth_datetime.minute), sizeof(person.birth_datetime.minute));
        file.write(reinterpret_cast<const char*>(&person.birth_datetime.second), sizeof(person.birth_datetime.second));
        
        // Geburtsort
        file.write(reinterpret_cast<const char*>(&person.birth_location.latitude), sizeof(person.birth_location.latitude));
        file.write(reinterpret_cast<const char*>(&person.birth_location.longitude), sizeof(person.birth_location.longitude));
        file.write(reinterpret_cast<const char*>(&person.birth_location.altitude), sizeof(person.birth_location.altitude));
        
        // Country Code
        uint32_t country_len = static_cast<uint32_t>(person.birth_location.country_code.length());
        file.write(reinterpret_cast<const char*>(&country_len), sizeof(country_len));
        file.write(person.birth_location.country_code.c_str(), country_len);
        
        // Timezone
        uint32_t tz_len = static_cast<uint32_t>(person.birth_location.timezone.length());
        file.write(reinterpret_cast<const char*>(&tz_len), sizeof(tz_len));
        file.write(person.birth_location.timezone.c_str(), tz_len);
        
        return file.good();
    } catch (...) {
        SetError("Fehler beim Schreiben der Person-Daten");
        return false;
    }
}

bool RadixDatabase::ReadPersonFromFile(std::ifstream& file, astro::RadixData& person) {
    try {
        // Name
        uint32_t name_len;
        file.read(reinterpret_cast<char*>(&name_len), sizeof(name_len));
        if (!file.good() || name_len > 1000) return false;
        
        person.name.resize(name_len);
        file.read(&person.name[0], name_len);
        
        // Geburtsdatum
        file.read(reinterpret_cast<char*>(&person.birth_datetime.year), sizeof(person.birth_datetime.year));
        file.read(reinterpret_cast<char*>(&person.birth_datetime.month), sizeof(person.birth_datetime.month));
        file.read(reinterpret_cast<char*>(&person.birth_datetime.day), sizeof(person.birth_datetime.day));
        file.read(reinterpret_cast<char*>(&person.birth_datetime.hour), sizeof(person.birth_datetime.hour));
        file.read(reinterpret_cast<char*>(&person.birth_datetime.minute), sizeof(person.birth_datetime.minute));
        file.read(reinterpret_cast<char*>(&person.birth_datetime.second), sizeof(person.birth_datetime.second));
        
        // Geburtsort
        file.read(reinterpret_cast<char*>(&person.birth_location.latitude), sizeof(person.birth_location.latitude));
        file.read(reinterpret_cast<char*>(&person.birth_location.longitude), sizeof(person.birth_location.longitude));
        file.read(reinterpret_cast<char*>(&person.birth_location.altitude), sizeof(person.birth_location.altitude));
        
        // Country Code
        uint32_t country_len;
        file.read(reinterpret_cast<char*>(&country_len), sizeof(country_len));
        if (!file.good() || country_len > 100) return false;
        
        person.birth_location.country_code.resize(country_len);
        file.read(&person.birth_location.country_code[0], country_len);
        
        // Timezone
        uint32_t tz_len;
        file.read(reinterpret_cast<char*>(&tz_len), sizeof(tz_len));
        if (!file.good() || tz_len > 100) return false;
        
        person.birth_location.timezone.resize(tz_len);
        file.read(&person.birth_location.timezone[0], tz_len);
        
        // Julianisches Datum berechnen
        person.julian_date = astro::JulianDate(person.birth_datetime);
        
        return file.good();
    } catch (...) {
        SetError("Fehler beim Lesen der Person-Daten");
        return false;
    }
}

bool RadixDatabase::WriteOrtToFile(std::ofstream& file, const std::string& name, const astro::GeoLocation& ort) {
    try {
        // Name
        uint32_t name_len = static_cast<uint32_t>(name.length());
        file.write(reinterpret_cast<const char*>(&name_len), sizeof(name_len));
        file.write(name.c_str(), name_len);
        
        // Koordinaten
        file.write(reinterpret_cast<const char*>(&ort.latitude), sizeof(ort.latitude));
        file.write(reinterpret_cast<const char*>(&ort.longitude), sizeof(ort.longitude));
        file.write(reinterpret_cast<const char*>(&ort.altitude), sizeof(ort.altitude));
        
        // Country Code
        uint32_t country_len = static_cast<uint32_t>(ort.country_code.length());
        file.write(reinterpret_cast<const char*>(&country_len), sizeof(country_len));
        file.write(ort.country_code.c_str(), country_len);
        
        // Timezone
        uint32_t tz_len = static_cast<uint32_t>(ort.timezone.length());
        file.write(reinterpret_cast<const char*>(&tz_len), sizeof(tz_len));
        file.write(ort.timezone.c_str(), tz_len);
        
        return file.good();
    } catch (...) {
        SetError("Fehler beim Schreiben der Ort-Daten");
        return false;
    }
}

bool RadixDatabase::ReadOrtFromFile(std::ifstream& file, std::string& name, astro::GeoLocation& ort) {
    try {
        // Name
        uint32_t name_len;
        file.read(reinterpret_cast<char*>(&name_len), sizeof(name_len));
        if (!file.good() || name_len > 1000) return false;
        
        name.resize(name_len);
        file.read(&name[0], name_len);
        
        // Koordinaten
        file.read(reinterpret_cast<char*>(&ort.latitude), sizeof(ort.latitude));
        file.read(reinterpret_cast<char*>(&ort.longitude), sizeof(ort.longitude));
        file.read(reinterpret_cast<char*>(&ort.altitude), sizeof(ort.altitude));
        
        // Country Code
        uint32_t country_len;
        file.read(reinterpret_cast<char*>(&country_len), sizeof(country_len));
        if (!file.good() || country_len > 100) return false;
        
        ort.country_code.resize(country_len);
        file.read(&ort.country_code[0], country_len);
        
        // Timezone
        uint32_t tz_len;
        file.read(reinterpret_cast<char*>(&tz_len), sizeof(tz_len));
        if (!file.good() || tz_len > 100) return false;
        
        ort.timezone.resize(tz_len);
        file.read(&ort.timezone[0], tz_len);
        
        return file.good();
    } catch (...) {
        SetError("Fehler beim Lesen der Ort-Daten");
        return false;
    }
}
