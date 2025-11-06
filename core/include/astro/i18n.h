// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (C) 2025 AstroUniverse Project

#pragma once

#include "data_types.h"
#include <string>
#include <map>

namespace astro {
namespace i18n {

// Unterstützte Sprachen
enum class Language {
    German,
    English
};

// Internationalisierungs-Manager
class I18n {
public:
    static I18n& instance();
    
    // Sprache setzen/abfragen
    void set_language(Language lang);
    Language get_language() const { return current_language_; }
    
    // Übersetzungen
    std::string planet_name(Planet planet) const;
    std::string house_system_name(HouseSystem system) const;
    std::string aspect_name(AspectType aspect) const;
    std::string zodiac_sign(int sign) const;  // 0-11 (Widder-Fische)
    
    // UI-Texte
    std::string tr(const std::string& key) const;
    
private:
    I18n();
    Language current_language_;
    
    // Übersetzungstabellen
    std::map<std::string, std::map<Language, std::string>> translations_;
    
    void init_translations();
};

// Convenience-Funktionen
inline std::string tr(const std::string& key) {
    return I18n::instance().tr(key);
}

inline std::string planet_name(Planet planet) {
    return I18n::instance().planet_name(planet);
}

inline std::string aspect_name(AspectType aspect) {
    return I18n::instance().aspect_name(aspect);
}

} // namespace i18n
} // namespace astro
