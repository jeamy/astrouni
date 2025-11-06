#pragma once

#include <string>
#include <chrono>
#include <cstdint>

namespace astro {

// Planeten-Enum (entspricht Swiss Ephemeris Konstanten)
enum class Planet : int32_t {
    Sun = 0,
    Moon = 1,
    Mercury = 2,
    Venus = 3,
    Mars = 4,
    Jupiter = 5,
    Saturn = 6,
    Uranus = 7,
    Neptune = 8,
    Pluto = 9,
    MeanNode = 10,
    TrueNode = 11,
    Chiron = 15
};

// Häusersysteme
enum class HouseSystem : char {
    Placidus = 'P',
    Koch = 'K',
    Porphyry = 'O',
    Regiomontanus = 'R',
    Campanus = 'C',
    Equal = 'E',
    WholeSign = 'W',
    Alcabitius = 'B',
    Morinus = 'M',
    Topocentric = 'T'
};

// Geografische Position (Legacy: ORTE)
struct GeoLocation {
    double latitude;     // Breite in Grad (-90 bis +90)
    double longitude;    // Länge in Grad (-180 bis +180)
    double altitude;     // Höhe in Metern
    std::string timezone;
    std::string country_code;
    
    GeoLocation() : latitude(0.0), longitude(0.0), altitude(0.0) {}
    GeoLocation(double lat, double lon, double alt = 0.0)
        : latitude(lat), longitude(lon), altitude(alt) {}
};

// Datum/Zeit-Struktur
struct DateTime {
    int32_t year;
    int32_t month;   // 1-12
    int32_t day;     // 1-31
    int32_t hour;    // 0-23
    int32_t minute;  // 0-59
    double second;   // 0.0-59.999...
    
    DateTime() : year(2000), month(1), day(1), hour(0), minute(0), second(0.0) {}
    DateTime(int32_t y, int32_t m, int32_t d, int32_t h = 0, int32_t min = 0, double s = 0.0)
        : year(y), month(m), day(d), hour(h), minute(min), second(s) {}
};

// Julianisches Datum
struct JulianDate {
    double jd;  // Julianisches Datum (UT)
    
    explicit JulianDate(double julian_day = 0.0) : jd(julian_day) {}
    JulianDate(const DateTime& dt);  // Konvertierung von DateTime
    
    double to_julian() const { return jd; }
};

// Planeten-Position
struct PlanetPosition {
    double longitude;        // Ekliptikale Länge (0-360°)
    double latitude;         // Ekliptikale Breite (-90 bis +90°)
    double distance;         // Distanz in AU
    double speed_longitude;  // Geschwindigkeit in °/Tag
    double speed_latitude;   // Geschwindigkeit in °/Tag
    double speed_distance;   // Geschwindigkeit in AU/Tag
    bool is_retrograde;      // Rückläufig?
    
    PlanetPosition() 
        : longitude(0.0), latitude(0.0), distance(0.0),
          speed_longitude(0.0), speed_latitude(0.0), speed_distance(0.0),
          is_retrograde(false) {}
};

// Häuser-Spitze
struct HouseCusp {
    int32_t house_number;  // 1-12
    double longitude;      // Ekliptikale Länge (0-360°)
    
    HouseCusp() : house_number(0), longitude(0.0) {}
    HouseCusp(int32_t num, double lon) : house_number(num), longitude(lon) {}
};

// Radix-Daten (Legacy: RADIXFIX)
struct RadixData {
    std::string name;           // Name der Person
    DateTime birth_datetime;    // Geburtsdatum/-zeit
    GeoLocation birth_location; // Geburtsort
    JulianDate julian_date;     // Berechnetes Julianisches Datum
    
    RadixData() = default;
    RadixData(const std::string& n, const DateTime& dt, const GeoLocation& loc)
        : name(n), birth_datetime(dt), birth_location(loc), julian_date(dt) {}
    
    bool is_valid() const {
        return !name.empty() && 
               birth_datetime.year >= -13000 && birth_datetime.year <= 17000;
    }
};

// Aspekt-Typ
enum class AspectType {
    Conjunction = 0,    // 0°
    Opposition = 180,   // 180°
    Trine = 120,        // 120°
    Square = 90,        // 90°
    Sextile = 60,       // 60°
    Quincunx = 150,     // 150°
    Semisextile = 30,   // 30°
    Semisquare = 45,    // 45°
    Sesquisquare = 135  // 135°
};

// Aspekt zwischen zwei Planeten
struct Aspect {
    Planet planet1;
    Planet planet2;
    AspectType type;
    double orb;         // Orbis (Abweichung vom exakten Aspekt)
    bool is_applying;   // Zunehmend oder abnehmend
    
    Aspect() : planet1(Planet::Sun), planet2(Planet::Moon), 
               type(AspectType::Conjunction), orb(0.0), is_applying(false) {}
};

} // namespace astro
