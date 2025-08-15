#include "astrocore/ephemeris.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <cmath>

namespace astrocore {

// Static member initialization
std::vector<EphemerisRecord> EphemerisManager::s_ephemerisData;
std::map<int, AstronomicalObject> EphemerisManager::s_objects;
bool EphemerisManager::s_initialized = false;
double EphemerisManager::s_startJD = 0.0;
double EphemerisManager::s_endJD = 0.0;
std::string EphemerisManager::s_dataPath;

bool EphemerisManager::initialize(const std::string& dataPath) {
    s_dataPath = dataPath;
    s_initialized = false;
    s_ephemerisData.clear();
    s_objects.clear();
    
    // Check if data directory exists
    if (!std::filesystem::exists(dataPath)) {
        std::cerr << "Ephemeris data directory not found: " << dataPath << std::endl;
        return false;
    }
    
    // Parse ephemeris data file
    std::string ephemerisFile = dataPath + "/astroeph.dat";
    if (!parseEphemerisFile(ephemerisFile)) {
        std::cerr << "Failed to parse ephemeris file: " << ephemerisFile << std::endl;
        return false;
    }
    
    // Parse astronomical names file
    std::string namesFile = dataPath + "/astronam.dat";
    if (!parseNamesFile(namesFile)) {
        std::cerr << "Failed to parse names file: " << namesFile << std::endl;
        return false;
    }
    
    // Sort ephemeris data by Julian day
    std::sort(s_ephemerisData.begin(), s_ephemerisData.end(),
              [](const EphemerisRecord& a, const EphemerisRecord& b) {
                  return a.jd < b.jd;
              });
    
    // Set start and end JD
    if (!s_ephemerisData.empty()) {
        s_startJD = s_ephemerisData.front().jd;
        s_endJD = s_ephemerisData.back().jd;
    }
    
    s_initialized = true;
    return true;
}

bool EphemerisManager::isInitialized() {
    return s_initialized;
}

double EphemerisManager::getStartJD() {
    return s_startJD;
}

double EphemerisManager::getEndJD() {
    return s_endJD;
}

std::vector<double> EphemerisManager::getEphemerisData(double jd) {
    if (!s_initialized || s_ephemerisData.empty()) {
        return {};
    }
    
    // Check if JD is within range
    if (jd < s_startJD || jd > s_endJD) {
        return {};
    }
    
    // Find the records that bracket the requested JD
    auto it = std::lower_bound(s_ephemerisData.begin(), s_ephemerisData.end(), jd,
                              [](const EphemerisRecord& record, double value) {
                                  return record.jd < value;
                              });
    
    // If exact match, return the values directly
    if (it != s_ephemerisData.end() && std::abs(it->jd - jd) < 1e-6) {
        return it->values;
    }
    
    // If we're at the beginning, use the first record
    if (it == s_ephemerisData.begin()) {
        return it->values;
    }
    
    // If we're at the end, use the last record
    if (it == s_ephemerisData.end()) {
        return (--it)->values;
    }
    
    // Otherwise, interpolate between the two records
    auto next = it;
    auto prev = --it;
    return interpolate(*prev, *next, jd);
}

std::optional<std::tuple<double, double, double>> 
EphemerisManager::getPlanetPosition(int planetId, double jd) {
    if (!s_initialized) {
        return std::nullopt;
    }
    
    // Get ephemeris data for the requested JD
    std::vector<double> data = getEphemerisData(jd);
    if (data.empty()) {
        return std::nullopt;
    }
    
    // Check if planetId is valid
    int baseIndex = planetId * 3; // Each planet has 3 values: longitude, latitude, distance
    if (baseIndex + 2 >= data.size()) {
        return std::nullopt;
    }
    
    // Return the planet position
    return std::make_tuple(data[baseIndex], data[baseIndex + 1], data[baseIndex + 2]);
}

std::optional<AstronomicalObject> EphemerisManager::getObjectInfo(int objectId) {
    if (!s_initialized) {
        return std::nullopt;
    }
    
    auto it = s_objects.find(objectId);
    if (it != s_objects.end()) {
        return it->second;
    }
    
    return std::nullopt;
}

std::vector<AstronomicalObject> EphemerisManager::getAllObjects() {
    std::vector<AstronomicalObject> result;
    
    if (!s_initialized) {
        return result;
    }
    
    result.reserve(s_objects.size());
    for (const auto& [id, object] : s_objects) {
        result.push_back(object);
    }
    
    return result;
}

bool EphemerisManager::parseEphemerisFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        EphemerisRecord record;
        if (ephemeris_helpers::parseEphemerisLine(line, record)) {
            s_ephemerisData.push_back(record);
        }
    }
    
    return !s_ephemerisData.empty();
}

bool EphemerisManager::parseNamesFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        int id;
        AstronomicalObject object;
        if (ephemeris_helpers::parseNameLine(line, id, object)) {
            s_objects[id] = object;
        }
    }
    
    return !s_objects.empty();
}

std::vector<double> EphemerisManager::interpolate(
    const EphemerisRecord& r1, 
    const EphemerisRecord& r2, 
    double jd) {
    
    // Linear interpolation
    double t = (jd - r1.jd) / (r2.jd - r1.jd);
    
    std::vector<double> result;
    result.reserve(r1.values.size());
    
    for (size_t i = 0; i < r1.values.size() && i < r2.values.size(); ++i) {
        double v1 = r1.values[i];
        double v2 = r2.values[i];
        
        // Special handling for angles (longitudes) to avoid discontinuities at 0/360
        if (i % 3 == 0) { // Longitude values
            // Ensure the shortest path around the circle
            if (v2 - v1 > 180.0) {
                v1 += 360.0;
            } else if (v1 - v2 > 180.0) {
                v2 += 360.0;
            }
        }
        
        double interpolated = v1 + t * (v2 - v1);
        
        // Normalize longitude to 0-360 range
        if (i % 3 == 0) {
            interpolated = std::fmod(interpolated, 360.0);
            if (interpolated < 0.0) {
                interpolated += 360.0;
            }
        }
        
        result.push_back(interpolated);
    }
    
    return result;
}

namespace ephemeris_helpers {

bool parseEphemerisLine(const std::string& line, EphemerisRecord& record) {
    // Skip empty lines and comments
    if (line.empty() || line[0] == '#') {
        return false;
    }
    
    std::istringstream iss(line);
    std::string token;
    
    // First token is the Julian day
    if (!(iss >> token)) {
        return false;
    }
    
    auto jd = parseDouble(token);
    if (!jd) {
        return false;
    }
    
    record.jd = *jd;
    record.values.clear();
    
    // Parse the rest of the tokens as values
    while (iss >> token) {
        auto value = parseDouble(token);
        if (value) {
            record.values.push_back(*value);
        }
    }
    
    return !record.values.empty();
}

bool parseNameLine(const std::string& line, int& id, AstronomicalObject& object) {
    // Skip empty lines and comments
    if (line.empty() || line[0] == '#') {
        return false;
    }
    
    // Format: ID|Name|Abbreviation|Description
    std::istringstream iss(line);
    std::string idStr, name, abbr, desc;
    
    if (!std::getline(iss, idStr, '|')) {
        return false;
    }
    
    auto idOpt = parseInt(trim(idStr));
    if (!idOpt) {
        return false;
    }
    
    id = *idOpt;
    
    if (!std::getline(iss, name, '|')) {
        return false;
    }
    
    if (!std::getline(iss, abbr, '|')) {
        return false;
    }
    
    // Description is optional
    std::getline(iss, desc);
    
    object.name = trim(name);
    object.abbreviation = trim(abbr);
    object.description = trim(desc);
    
    return !object.name.empty();
}

std::optional<double> parseDouble(const std::string& str) {
    try {
        return std::stod(str);
    } catch (...) {
        return std::nullopt;
    }
}

std::optional<int> parseInt(const std::string& str) {
    try {
        return std::stoi(str);
    } catch (...) {
        return std::nullopt;
    }
}

std::string trim(const std::string& str) {
    const auto start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) {
        return "";
    }
    
    const auto end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

} // namespace ephemeris_helpers

} // namespace astrocore
