// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (C) 2025 AstroUniverse Project

#include "astro/i18n.h"

namespace astro {
namespace i18n {

I18n& I18n::instance() {
    static I18n instance;
    return instance;
}

I18n::I18n() : current_language_(Language::German) {
    init_translations();
}

void I18n::set_language(Language lang) {
    current_language_ = lang;
}

void I18n::init_translations() {
    // UI-Texte
    translations_["app.title"] = {{Language::German, "AstroUniverse"}, {Language::English, "AstroUniverse"}};
    translations_["menu.file"] = {{Language::German, "Datei"}, {Language::English, "File"}};
    translations_["menu.new"] = {{Language::German, "Neues Chart..."}, {Language::English, "New Chart..."}};
    translations_["menu.open"] = {{Language::German, "Öffnen..."}, {Language::English, "Open..."}};
    translations_["menu.save"] = {{Language::German, "Speichern..."}, {Language::English, "Save..."}};
    translations_["menu.export_png"] = {{Language::German, "Export als PNG..."}, {Language::English, "Export as PNG..."}};
    translations_["menu.export_svg"] = {{Language::German, "Export als SVG..."}, {Language::English, "Export as SVG..."}};
    translations_["menu.exit"] = {{Language::German, "Beenden"}, {Language::English, "Exit"}};
    translations_["menu.help"] = {{Language::German, "Hilfe"}, {Language::English, "Help"}};
    translations_["menu.about"] = {{Language::German, "Über..."}, {Language::English, "About..."}};
    
    // Dialog-Texte
    translations_["dialog.new_chart"] = {{Language::German, "Neues Chart"}, {Language::English, "New Chart"}};
    translations_["dialog.person"] = {{Language::German, "Person"}, {Language::English, "Person"}};
    translations_["dialog.name"] = {{Language::German, "Name:"}, {Language::English, "Name:"}};
    translations_["dialog.birth_date"] = {{Language::German, "Geburtsdatum & -zeit"}, {Language::English, "Birth Date & Time"}};
    translations_["dialog.date"] = {{Language::German, "Datum:"}, {Language::English, "Date:"}};
    translations_["dialog.time_ut"] = {{Language::German, "Zeit (UT):"}, {Language::English, "Time (UT):"}};
    translations_["dialog.birth_place"] = {{Language::German, "Geburtsort"}, {Language::English, "Birth Place"}};
    translations_["dialog.city"] = {{Language::German, "Stadt:"}, {Language::English, "City:"}};
    translations_["dialog.latitude"] = {{Language::German, "Breitengrad:"}, {Language::English, "Latitude:"}};
    translations_["dialog.longitude"] = {{Language::German, "Längengrad:"}, {Language::English, "Longitude:"}};
    translations_["dialog.altitude"] = {{Language::German, "Höhe (m):"}, {Language::English, "Altitude (m):"}};
    translations_["dialog.calculate"] = {{Language::German, "Berechnen"}, {Language::English, "Calculate"}};
    translations_["dialog.cancel"] = {{Language::German, "Abbrechen"}, {Language::English, "Cancel"}};
    
    // File-Dialoge
    translations_["dialog.open_chart"] = {{Language::German, "Chart öffnen"}, {Language::English, "Open Chart"}};
    translations_["dialog.save_chart"] = {{Language::German, "Chart speichern"}, {Language::English, "Save Chart"}};
    translations_["dialog.export_png"] = {{Language::German, "Als PNG exportieren"}, {Language::English, "Export as PNG"}};
    translations_["dialog.export_svg"] = {{Language::German, "Als SVG exportieren"}, {Language::English, "Export as SVG"}};
    
    // Status-Texte
    translations_["status.ready"] = {{Language::German, "Bereit"}, {Language::English, "Ready"}};
    translations_["status.chart_created"] = {{Language::German, "Neues Chart erstellt"}, {Language::English, "New chart created"}};
    translations_["status.file_opened"] = {{Language::German, "Geöffnet:"}, {Language::English, "Opened:"}};
    translations_["status.file_saved"] = {{Language::German, "Gespeichert:"}, {Language::English, "Saved:"}};
    translations_["status.exported"] = {{Language::German, "Exportiert:"}, {Language::English, "Exported:"}};
    
    // Chart-Panel
    translations_["chart.no_chart"] = {{Language::German, "Kein Chart geladen\n\nDatei → Neues Chart..."}, 
                                      {Language::English, "No chart loaded\n\nFile → New Chart..."}};
    translations_["chart.planets"] = {{Language::German, "Planeten"}, {Language::English, "Planets"}};
    translations_["chart.houses"] = {{Language::German, "Häuser"}, {Language::English, "Houses"}};
    translations_["chart.aspects"] = {{Language::German, "Aspekte"}, {Language::English, "Aspects"}};
}

std::string I18n::planet_name(Planet planet) const {
    switch (planet) {
        case Planet::Sun:
            return current_language_ == Language::German ? "Sonne" : "Sun";
        case Planet::Moon:
            return current_language_ == Language::German ? "Mond" : "Moon";
        case Planet::Mercury:
            return current_language_ == Language::German ? "Merkur" : "Mercury";
        case Planet::Venus:
            return current_language_ == Language::German ? "Venus" : "Venus";
        case Planet::Mars:
            return current_language_ == Language::German ? "Mars" : "Mars";
        case Planet::Jupiter:
            return current_language_ == Language::German ? "Jupiter" : "Jupiter";
        case Planet::Saturn:
            return current_language_ == Language::German ? "Saturn" : "Saturn";
        case Planet::Uranus:
            return current_language_ == Language::German ? "Uranus" : "Uranus";
        case Planet::Neptune:
            return current_language_ == Language::German ? "Neptun" : "Neptune";
        case Planet::Pluto:
            return current_language_ == Language::German ? "Pluto" : "Pluto";
        case Planet::MeanNode:
            return current_language_ == Language::German ? "Mondknoten" : "North Node";
        case Planet::TrueNode:
            return current_language_ == Language::German ? "Wahrer Mondknoten" : "True Node";
        case Planet::Chiron:
            return current_language_ == Language::German ? "Chiron" : "Chiron";
        default:
            return "?";
    }
}

std::string I18n::house_system_name(HouseSystem system) const {
    switch (system) {
        case HouseSystem::Placidus:
            return "Placidus";
        case HouseSystem::Koch:
            return "Koch";
        case HouseSystem::Porphyry:
            return current_language_ == Language::German ? "Porphyrius" : "Porphyry";
        case HouseSystem::Regiomontanus:
            return "Regiomontanus";
        case HouseSystem::Campanus:
            return "Campanus";
        case HouseSystem::Equal:
            return current_language_ == Language::German ? "Gleiche Häuser" : "Equal Houses";
        case HouseSystem::WholeSign:
            return current_language_ == Language::German ? "Ganze Zeichen" : "Whole Sign";
        default:
            return "?";
    }
}

std::string I18n::aspect_name(AspectType aspect) const {
    switch (aspect) {
        case AspectType::Conjunction:
            return current_language_ == Language::German ? "Konjunktion" : "Conjunction";
        case AspectType::Opposition:
            return current_language_ == Language::German ? "Opposition" : "Opposition";
        case AspectType::Trine:
            return current_language_ == Language::German ? "Trigon" : "Trine";
        case AspectType::Square:
            return current_language_ == Language::German ? "Quadrat" : "Square";
        case AspectType::Sextile:
            return current_language_ == Language::German ? "Sextil" : "Sextile";
        case AspectType::Quincunx:
            return current_language_ == Language::German ? "Quincunx" : "Quincunx";
        case AspectType::Semisextile:
            return current_language_ == Language::German ? "Halbsextil" : "Semisextile";
        case AspectType::Semisquare:
            return current_language_ == Language::German ? "Halbquadrat" : "Semisquare";
        case AspectType::Sesquisquare:
            return current_language_ == Language::German ? "Anderthalbquadrat" : "Sesquiquadrate";
        default:
            return "?";
    }
}

std::string I18n::zodiac_sign(int sign) const {
    const char* german_signs[] = {
        "Widder", "Stier", "Zwillinge", "Krebs", "Löwe", "Jungfrau",
        "Waage", "Skorpion", "Schütze", "Steinbock", "Wassermann", "Fische"
    };
    
    const char* english_signs[] = {
        "Aries", "Taurus", "Gemini", "Cancer", "Leo", "Virgo",
        "Libra", "Scorpio", "Sagittarius", "Capricorn", "Aquarius", "Pisces"
    };
    
    if (sign < 0 || sign > 11) return "?";
    
    return current_language_ == Language::German ? 
           german_signs[sign] : english_signs[sign];
}

std::string I18n::tr(const std::string& key) const {
    auto it = translations_.find(key);
    if (it != translations_.end()) {
        auto lang_it = it->second.find(current_language_);
        if (lang_it != it->second.end()) {
            return lang_it->second;
        }
    }
    return key;  // Fallback: Schlüssel selbst zurückgeben
}

} // namespace i18n
} // namespace astro
