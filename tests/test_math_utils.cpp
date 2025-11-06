#include <gtest/gtest.h>
#include "astro/math_utils.h"

using namespace astro;
using namespace astro::math;

TEST(MathUtilsTest, ModDegrees) {
    EXPECT_DOUBLE_EQ(mod_degrees(0.0), 0.0);
    EXPECT_DOUBLE_EQ(mod_degrees(360.0), 0.0);
    EXPECT_DOUBLE_EQ(mod_degrees(720.0), 0.0);
    EXPECT_DOUBLE_EQ(mod_degrees(-90.0), 270.0);
    EXPECT_DOUBLE_EQ(mod_degrees(450.0), 90.0);
}

TEST(MathUtilsTest, AngleDifference) {
    EXPECT_DOUBLE_EQ(angle_difference(0.0, 90.0), 90.0);
    EXPECT_DOUBLE_EQ(angle_difference(90.0, 0.0), -90.0);
    EXPECT_DOUBLE_EQ(angle_difference(350.0, 10.0), 20.0);
    EXPECT_DOUBLE_EQ(angle_difference(10.0, 350.0), -20.0);
}

TEST(MathUtilsTest, JulianDay) {
    // 1. Januar 2000, 12:00 UT = JD 2451545.0
    double jd = calculate_julian_day(2000, 1, 1, 12, 0, 0.0);
    EXPECT_NEAR(jd, 2451545.0, 0.001);
    
    // 1. Januar 1970, 00:00 UT (Unix Epoch)
    jd = calculate_julian_day(1970, 1, 1, 0, 0, 0.0);
    EXPECT_NEAR(jd, 2440587.5, 0.001);
}

TEST(MathUtilsTest, DegreesToDMS) {
    auto dms = degrees_to_dms(45.5);
    EXPECT_EQ(dms.degrees, 45);
    EXPECT_EQ(dms.minutes, 30);
    EXPECT_NEAR(dms.seconds, 0.0, 0.01);
    EXPECT_FALSE(dms.is_negative);
    
    dms = degrees_to_dms(-10.25);
    EXPECT_EQ(dms.degrees, 10);
    EXPECT_EQ(dms.minutes, 15);
    EXPECT_TRUE(dms.is_negative);
}

TEST(MathUtilsTest, DMSToDegrees) {
    DMS dms{45, 30, 0.0, false};
    EXPECT_DOUBLE_EQ(dms_to_degrees(dms), 45.5);
    
    dms = {10, 15, 0.0, true};
    EXPECT_DOUBLE_EQ(dms_to_degrees(dms), -10.25);
}

TEST(MathUtilsTest, PolarToRectangular) {
    auto rect = polar_to_rectangular(0.0, 0.0, 1.0);
    EXPECT_NEAR(rect.x, 1.0, 0.001);
    EXPECT_NEAR(rect.y, 0.0, 0.001);
    EXPECT_NEAR(rect.z, 0.0, 0.001);
    
    rect = polar_to_rectangular(90.0, 0.0, 1.0);
    EXPECT_NEAR(rect.x, 0.0, 0.001);
    EXPECT_NEAR(rect.y, 1.0, 0.001);
    EXPECT_NEAR(rect.z, 0.0, 0.001);
}

TEST(MathUtilsTest, RectangularToPolar) {
    auto polar = rectangular_to_polar(1.0, 0.0, 0.0);
    EXPECT_NEAR(polar.longitude, 0.0, 0.001);
    EXPECT_NEAR(polar.latitude, 0.0, 0.001);
    EXPECT_NEAR(polar.radius, 1.0, 0.001);
    
    polar = rectangular_to_polar(0.0, 1.0, 0.0);
    EXPECT_NEAR(polar.longitude, 90.0, 0.001);
    EXPECT_NEAR(polar.latitude, 0.0, 0.001);
    EXPECT_NEAR(polar.radius, 1.0, 0.001);
}
