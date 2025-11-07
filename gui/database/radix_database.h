// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (C) 2025 AstroUniverse Project
// 1:1 Port of astrofil.c database functions

#pragma once

#include <string>
#include <vector>
#include <fstream>
#include "astro/data_types.h"

// 1:1 Port der Legacy-Datenbank-Funktionen aus astrofil.c
class RadixDatabase {
public:
    RadixDatabase();
    ~RadixDatabase();
    
    // Personen-Datenbank (1:1 Legacy)
    bool LoadPersonen(const std::string& filename = "personen.dat");
    bool SavePersonen(const std::string& filename = "personen.dat");
    bool AddPerson(const astro::RadixData& person);
    bool DeletePerson(size_t index);
    bool UpdatePerson(size_t index, const astro::RadixData& person);
    std::vector<astro::RadixData> GetPersonen() const { return personen_; }
    astro::RadixData GetPerson(size_t index) const;
    size_t GetPersonenCount() const { return personen_.size(); }
    
    // Orte-Datenbank (1:1 Legacy)
    bool LoadOrte(const std::string& filename = "orte.dat");
    bool SaveOrte(const std::string& filename = "orte.dat");
    bool AddOrt(const astro::GeoLocation& ort, const std::string& name);
    bool DeleteOrt(size_t index);
    bool UpdateOrt(size_t index, const astro::GeoLocation& ort, const std::string& name);
    std::vector<std::pair<std::string, astro::GeoLocation>> GetOrte() const { return orte_; }
    std::pair<std::string, astro::GeoLocation> GetOrt(size_t index) const;
    size_t GetOrteCount() const { return orte_.size(); }
    
    // Suche-Funktionen (1:1 Legacy)
    std::vector<size_t> FindPersonen(const std::string& suchtext) const;
    std::vector<size_t> FindOrte(const std::string& suchtext) const;
    
    // Fehler-Handling
    std::string GetLastError() const { return last_error_; }
    bool HasError() const { return !last_error_.empty(); }
    void ClearError() { last_error_.clear(); }
    
private:
    // Daten
    std::vector<astro::RadixData> personen_;
    std::vector<std::pair<std::string, astro::GeoLocation>> orte_;  // Name + Location
    
    // Fehler-Handling
    std::string last_error_;
    
    // Hilfsfunktionen (1:1 Legacy Dateiformat)
    bool WritePersonToFile(std::ofstream& file, const astro::RadixData& person);
    bool ReadPersonFromFile(std::ifstream& file, astro::RadixData& person);
    bool WriteOrtToFile(std::ofstream& file, const std::string& name, const astro::GeoLocation& ort);
    bool ReadOrtFromFile(std::ifstream& file, std::string& name, astro::GeoLocation& ort);
    
    void SetError(const std::string& error) { last_error_ = error; }
};
