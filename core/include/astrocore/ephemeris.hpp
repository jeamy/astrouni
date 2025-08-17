#pragma once

#include <string>
#include <vector>
#include <map>
#include <optional>
#include <tuple>
#include <cstdint>

namespace astrocore {

// Structure to hold ephemeris data for a specific time
struct EphemerisRecord {
    double jd;                  // Julian day
    std::vector<double> values; // Values for each planet/object
};

// Structure to hold named astronomical objects
struct AstronomicalObject {
    std::string name;           // Object name
    std::string abbreviation;   // Short abbreviation
    std::string description;    // Optional description
};

// Class to handle ephemeris data files
class EphemerisManager {
public:
    // Initialize with path to data directory
    static bool initialize(const std::string& dataPath);
    
    // Check if ephemeris is available
    static bool isInitialized();
    
    // Get ephemeris data range
    static double getStartJD();
    static double getEndJD();
    
    // Get ephemeris data for a specific Julian day
    // Returns interpolated values for all planets
    static std::vector<double> getEphemerisData(double jd);
    
    // Get a specific planet's position at a given Julian day
    // Returns longitude, latitude, distance
    static std::optional<std::tuple<double, double, double>> 
    getPlanetPosition(int planetId, double jd);
    
    // Get astronomical object information
    static std::optional<AstronomicalObject> getObjectInfo(int objectId);
    
    // Get all available astronomical objects
    static std::vector<AstronomicalObject> getAllObjects();

private:
    // Parse ephemeris data file
    static bool parseEphemerisFile(const std::string& filename);
    
    // Parse astronomical names file
    static bool parseNamesFile(const std::string& filename);
    
    // Interpolate between two ephemeris records
    static std::vector<double> interpolate(
        const EphemerisRecord& r1, 
        const EphemerisRecord& r2, 
        double jd);
    
    // Data storage
    static std::vector<EphemerisRecord> s_ephemerisData;
    static std::map<int, AstronomicalObject> s_objects;
    static bool s_initialized;
    static double s_startJD;
    static double s_endJD;
    static std::string s_dataPath;
};

// Helper functions for file parsing
namespace ephemeris_helpers {
    // Parse a line from the ephemeris file
    bool parseEphemerisLine(const std::string& line, EphemerisRecord& record);
    
    // Parse a line from the names file
    bool parseNameLine(const std::string& line, int& id, AstronomicalObject& object);
    
    // Convert string to double with error handling
    std::optional<double> parseDouble(const std::string& str);
    
    // Convert string to int with error handling
    std::optional<int> parseInt(const std::string& str);
    
    // Trim whitespace from string
    std::string trim(const std::string& str);
}

} // namespace astrocore
