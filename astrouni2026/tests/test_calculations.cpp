/**
 * @file test_calculations.cpp
 * @brief Unit Tests für mathematische Berechnungen
 */

#include <QtTest>
#include "../src/core/calculations.h"
#include "../src/core/constants.h"

using namespace astro;

class TestCalculations : public QObject {
    Q_OBJECT
    
private slots:
    void testJulianDay();
    void testMod360();
    void testDegToGMS();
    void testMinDist();
    void testGetZeichen();
    void testCheckAspekt();
};

void TestCalculations::testJulianDay() {
    // Test: 1. Januar 2000, 12:00 UT = JD 2451545.0
    double jd = Calculations::julianDay(1, 1, 2000, 12.0, true);
    QCOMPARE(qRound(jd * 10) / 10.0, 2451545.0);
    
    // Test: 27. Januar 1996 (Erstellungsdatum des Legacy-Codes)
    jd = Calculations::julianDay(27, 1, 1996, 12.0, true);
    QVERIFY(jd > 2450100 && jd < 2450200);
}

void TestCalculations::testMod360() {
    QCOMPARE(Calculations::mod360(0.0), 0.0);
    QCOMPARE(Calculations::mod360(360.0), 0.0);
    QCOMPARE(Calculations::mod360(720.0), 0.0);
    QCOMPARE(Calculations::mod360(-90.0), 270.0);
    QCOMPARE(Calculations::mod360(450.0), 90.0);
}

void TestCalculations::testDegToGMS() {
    int16_t g, m, s;
    
    // 15°30'45"
    Calculations::degToGMS(15.3045, g, m, s);
    QCOMPARE(g, 15);
    QCOMPARE(m, 30);
    QCOMPARE(s, 45);
    
    // 0°0'0"
    Calculations::degToGMS(0.0, g, m, s);
    QCOMPARE(g, 0);
    QCOMPARE(m, 0);
    QCOMPARE(s, 0);
}

void TestCalculations::testMinDist() {
    QCOMPARE(Calculations::minDist(0.0, 90.0), 90.0);
    QCOMPARE(Calculations::minDist(350.0, 10.0), 20.0);
    QCOMPARE(Calculations::minDist(10.0, 350.0), -20.0);
    QCOMPARE(Calculations::minDist(0.0, 180.0), 180.0);
}

void TestCalculations::testGetZeichen() {
    QCOMPARE(Calculations::getZeichen(0.0), static_cast<int8_t>(iWIDDER));
    QCOMPARE(Calculations::getZeichen(30.0), static_cast<int8_t>(iSTIER));
    QCOMPARE(Calculations::getZeichen(60.0), static_cast<int8_t>(iZWILLINGE));
    QCOMPARE(Calculations::getZeichen(359.0), static_cast<int8_t>(iFISCHE));
}

void TestCalculations::testCheckAspekt() {
    double winkel;
    
    // Konjunktion (0°)
    QVERIFY(Calculations::checkAspekt(0.0, 5.0, KONJUNKTION, 10.0, winkel));
    QCOMPARE(winkel, 5.0);
    
    // Opposition (180°)
    QVERIFY(Calculations::checkAspekt(0.0, 180.0, OPOSITION, 10.0, winkel));
    
    // Quadrat (90°)
    QVERIFY(Calculations::checkAspekt(0.0, 88.0, QUADRATUR, 5.0, winkel));
    QVERIFY(!Calculations::checkAspekt(0.0, 80.0, QUADRATUR, 5.0, winkel));
}

QTEST_MAIN(TestCalculations)
#include "test_calculations.moc"
