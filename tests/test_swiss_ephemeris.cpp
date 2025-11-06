#include <gtest/gtest.h>
#include "astro/swiss_ephemeris.h"

using namespace astro;

class SwissEphemerisTest : public ::testing::Test {
protected:
    void SetUp() override {
        ephemeris = std::make_unique<SwissEphemeris>();
        // Pfad zu Ephemeriden-Files (relativ zum Build-Verzeichnis)
        ephemeris->initialize("../swisseph/ephe");
    }
    
    std::unique_ptr<SwissEphemeris> ephemeris;
};

TEST_F(SwissEphemerisTest, Initialization) {
    EXPECT_TRUE(ephemeris->is_initialized());
    EXPECT_FALSE(ephemeris->get_ephe_path().empty());
}

TEST_F(SwissEphemerisTest, SunPosition) {
    // 1. Januar 2000, 12:00 UT
    DateTime dt(2000, 1, 1, 12, 0, 0.0);
    JulianDate jd(dt);
    
    auto sun = ephemeris->calculate_planet(Planet::Sun, jd);
    
    // Sonne sollte bei ~280° sein (Steinbock, Anfang Januar)
    EXPECT_GT(sun.longitude, 270.0);
    EXPECT_LT(sun.longitude, 290.0);
    
    // Sonne auf Ekliptik
    EXPECT_NEAR(sun.latitude, 0.0, 0.1);
    
    // Sonne bewegt sich ~1° pro Tag
    EXPECT_GT(sun.speed_longitude, 0.9);
    EXPECT_LT(sun.speed_longitude, 1.1);
    
    // Sonne ist nie retrograde
    EXPECT_FALSE(sun.is_retrograde);
}

TEST_F(SwissEphemerisTest, MoonPosition) {
    DateTime dt(2000, 1, 1, 0, 0, 0.0);
    JulianDate jd(dt);
    
    auto moon = ephemeris->calculate_planet(Planet::Moon, jd);
    
    // Mond bewegt sich ~12-15° pro Tag
    EXPECT_GT(moon.speed_longitude, 11.0);
    EXPECT_LT(moon.speed_longitude, 16.0);
    
    // Mond-Breite sollte innerhalb ±5° sein
    EXPECT_GT(moon.latitude, -6.0);
    EXPECT_LT(moon.latitude, 6.0);
}

TEST_F(SwissEphemerisTest, AllPlanets) {
    DateTime dt(2000, 1, 1, 12, 0, 0.0);
    JulianDate jd(dt);
    
    std::vector<Planet> planets = {
        Planet::Sun, Planet::Moon, Planet::Mercury, Planet::Venus,
        Planet::Mars, Planet::Jupiter, Planet::Saturn, Planet::Uranus,
        Planet::Neptune, Planet::Pluto
    };
    
    for (Planet planet : planets) {
        EXPECT_NO_THROW({
            auto pos = ephemeris->calculate_planet(planet, jd);
            EXPECT_GE(pos.longitude, 0.0);
            EXPECT_LT(pos.longitude, 360.0);
        });
    }
}

TEST_F(SwissEphemerisTest, PlacidusHouses) {
    DateTime dt(2000, 1, 1, 12, 0, 0.0);
    JulianDate jd(dt);
    GeoLocation berlin(52.52, 13.40);  // Berlin
    
    auto cusps = ephemeris->calculate_houses(HouseSystem::Placidus, jd, berlin);
    
    ASSERT_EQ(cusps.size(), 12);
    
    // Häuser sollten nummeriert sein 1-12
    for (size_t i = 0; i < cusps.size(); ++i) {
        EXPECT_EQ(cusps[i].house_number, static_cast<int32_t>(i + 1));
        EXPECT_GE(cusps[i].longitude, 0.0);
        EXPECT_LT(cusps[i].longitude, 360.0);
    }
}

TEST_F(SwissEphemerisTest, DeltaT) {
    DateTime dt(2000, 1, 1, 12, 0, 0.0);
    JulianDate jd(dt);
    
    double delta_t = ephemeris->calculate_delta_t(jd);
    
    // Delta T für Jahr 2000 sollte ~64 Sekunden sein
    EXPECT_GT(delta_t, 60.0 / 86400.0);  // Umrechnung in Tage
    EXPECT_LT(delta_t, 70.0 / 86400.0);
}

TEST_F(SwissEphemerisTest, PlanetNames) {
    EXPECT_EQ(ephemeris->get_planet_name(Planet::Sun), "Sun");
    EXPECT_EQ(ephemeris->get_planet_name(Planet::Moon), "Moon");
    EXPECT_EQ(ephemeris->get_planet_name(Planet::Mars), "Mars");
}
