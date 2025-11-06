#include <gtest/gtest.h>
#include "astro/calculations.h"
#include <memory>

using namespace astro;

class CalculationsTest : public ::testing::Test {
protected:
    void SetUp() override {
        ephemeris = std::make_shared<SwissEphemeris>();
        ephemeris->initialize("../swisseph/ephe");
        calculator = std::make_unique<AstroCalculator>(ephemeris);
    }
    
    std::shared_ptr<SwissEphemeris> ephemeris;
    std::unique_ptr<AstroCalculator> calculator;
};

TEST_F(CalculationsTest, CalculateAllPlanets) {
    DateTime dt(2000, 1, 1, 12, 0, 0.0);
    JulianDate jd(dt);
    
    auto planets = calculator->calculate_all_planets(jd);
    
    EXPECT_GE(planets.size(), 10);  // Mindestens 10 Planeten
    
    for (const auto& planet : planets) {
        EXPECT_GE(planet.longitude, 0.0);
        EXPECT_LT(planet.longitude, 360.0);
    }
}

TEST_F(CalculationsTest, CalculateChart) {
    RadixData radix;
    radix.name = "Test Person";
    radix.birth_datetime = DateTime(2000, 1, 1, 12, 0, 0.0);
    radix.birth_location = GeoLocation(52.52, 13.40);  // Berlin
    radix.julian_date = JulianDate(radix.birth_datetime);
    
    auto chart = calculator->calculate_chart(radix, HouseSystem::Placidus);
    
    EXPECT_EQ(chart.radix.name, "Test Person");
    EXPECT_GE(chart.planets.size(), 10);
    EXPECT_EQ(chart.houses.size(), 12);
    EXPECT_GT(chart.aspects.size(), 0);
}

TEST_F(CalculationsTest, CalculateAspects) {
    DateTime dt(2000, 1, 1, 12, 0, 0.0);
    JulianDate jd(dt);
    
    auto planets = calculator->calculate_all_planets(jd);
    auto aspects = calculator->calculate_aspects(planets, 8.0);
    
    // Es sollten einige Aspekte gefunden werden
    EXPECT_GT(aspects.size(), 0);
    
    // Alle Aspekte sollten innerhalb des Orbis liegen
    for (const auto& aspect : aspects) {
        EXPECT_LE(std::abs(aspect.orb), 8.0);
    }
}

TEST_F(CalculationsTest, AngleCalculation) {
    PlanetPosition p1, p2;
    
    p1.longitude = 0.0;
    p2.longitude = 90.0;
    
    double angle = calculator->calculate_angle(p1, p2);
    EXPECT_NEAR(angle, 90.0, 0.001);
    
    p1.longitude = 350.0;
    p2.longitude = 10.0;
    angle = calculator->calculate_angle(p1, p2);
    EXPECT_NEAR(angle, 20.0, 0.001);
}

TEST_F(CalculationsTest, AspectDetection) {
    double orb;
    
    // Konjunktion (0°)
    EXPECT_TRUE(calculator->is_aspect(5.0, AspectType::Conjunction, 8.0, orb));
    EXPECT_NEAR(orb, 5.0, 0.001);
    
    // Opposition (180°)
    EXPECT_TRUE(calculator->is_aspect(175.0, AspectType::Opposition, 8.0, orb));
    EXPECT_NEAR(orb, 5.0, 0.001);
    
    // Trigon (120°)
    EXPECT_TRUE(calculator->is_aspect(123.0, AspectType::Trine, 8.0, orb));
    EXPECT_NEAR(orb, 3.0, 0.001);
    
    // Kein Aspekt
    EXPECT_FALSE(calculator->is_aspect(45.0, AspectType::Conjunction, 8.0, orb));
}
