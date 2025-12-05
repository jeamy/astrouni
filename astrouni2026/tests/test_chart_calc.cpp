/**
 * @file test_chart_calc.cpp
 * @brief Unit Tests für Horoskop-Berechnungen
 */

#include <QtTest>
#include "../src/core/chart_calc.h"
#include "../src/core/calculations.h"
#include "../src/core/swiss_eph.h"

using namespace astro;

class TestChartCalc : public QObject {
    Q_OBJECT
    
private slots:
    void initTestCase();
    void testHousesEqual();
    void testHousesWhole();
    void testGetHouseOfPlanet();
    void testCalcQualities();
    void cleanupTestCase();
};

void TestChartCalc::initTestCase() {
    // Swiss Ephemeris initialisieren (falls verfügbar)
    QString ephePath = QCoreApplication::applicationDirPath() + "/../swisseph/ephe";
    swissEph().setEphePath(ephePath);
}

void TestChartCalc::cleanupTestCase() {
}

void TestChartCalc::testHousesEqual() {
    Radix radix;
    radix.asc = 45.0;  // ASC bei 45° (Stier)
    radix.mc = 315.0;  // MC bei 315° (Wassermann)
    radix.hausSys = TYP_EQUAL;
    
    ChartCalc::housesEqual(radix);
    
    // Equal Houses: Alle 30° vom ASC
    QCOMPARE(radix.haus[0], 45.0);   // Haus 1 = ASC
    QCOMPARE(radix.haus[1], 75.0);   // Haus 2
    QCOMPARE(radix.haus[6], 225.0);  // Haus 7 = DSC
}

void TestChartCalc::testHousesWhole() {
    Radix radix;
    radix.asc = 45.0;  // ASC bei 45° (Stier)
    radix.hausSys = TYP_WHOLE;
    
    ChartCalc::housesWhole(radix);
    
    // Whole Sign: ASC-Zeichen = Haus 1
    // ASC in Stier (30-60°), also Haus 1 beginnt bei 30°
    QCOMPARE(radix.haus[0], 30.0);   // Stier
    QCOMPARE(radix.haus[1], 60.0);   // Zwillinge
    QCOMPARE(radix.haus[2], 90.0);   // Krebs
}

void TestChartCalc::testGetHouseOfPlanet() {
    QVector<double> hausSpitzen = {0.0, 30.0, 60.0, 90.0, 120.0, 150.0,
                                   180.0, 210.0, 240.0, 270.0, 300.0, 330.0};
    
    // Planet bei 15° -> Haus 1
    QCOMPARE(ChartCalc::getHouseOfPlanet(15.0, hausSpitzen), static_cast<int8_t>(0));
    
    // Planet bei 45° -> Haus 2
    QCOMPARE(ChartCalc::getHouseOfPlanet(45.0, hausSpitzen), static_cast<int8_t>(1));
    
    // Planet bei 355° -> Haus 12
    QCOMPARE(ChartCalc::getHouseOfPlanet(355.0, hausSpitzen), static_cast<int8_t>(11));
}

void TestChartCalc::testCalcQualities() {
    Radix radix;
    radix.allocate(4);  // Nur 4 Planeten für Test
    radix.anzahlPlanet = 4;  // Explizit setzen
    
    // Qualitäten zurücksetzen
    for (int i = 0; i < MAX_QUALITATEN; ++i) {
        radix.qualitaten[i] = 0;
    }
    
    // Sonne in Widder (Feuer, Cardinal, Männlich) - Index 0
    radix.stzPlanet[0] = iWIDDER;
    // Mond in Stier (Erde, Fix, Weiblich) - Index 1
    radix.stzPlanet[1] = iSTIER;
    // Merkur in Zwillinge (Luft, Beweglich, Männlich) - Index 2
    radix.stzPlanet[2] = iZWILLINGE;
    // Venus in Krebs (Wasser, Cardinal, Weiblich) - Index 3
    radix.stzPlanet[3] = iKREBS;
    
    ChartCalc::calcQualities(radix);
    
    // Elemente: Widder=0%4=0=Feuer, Stier=1%4=1=Erde, Zwillinge=2%4=2=Luft, Krebs=3%4=3=Wasser
    QCOMPARE(radix.qualitaten[QU_FEUER], static_cast<int16_t>(1));
    QCOMPARE(radix.qualitaten[QU_ERDE], static_cast<int16_t>(1));
    QCOMPARE(radix.qualitaten[QU_LUFT], static_cast<int16_t>(1));
    QCOMPARE(radix.qualitaten[QU_WASSER], static_cast<int16_t>(1));
    
    // Modi: Widder=0%3=0=Cardinal, Stier=1%3=1=Fix, Zwillinge=2%3=2=Beweglich, Krebs=3%3=0=Cardinal
    QCOMPARE(radix.qualitaten[QU_CARDINAL], static_cast<int16_t>(2));
    QCOMPARE(radix.qualitaten[QU_FIX], static_cast<int16_t>(1));
    QCOMPARE(radix.qualitaten[QU_BEWEGLICH], static_cast<int16_t>(1));
}

QTEST_MAIN(TestChartCalc)
#include "test_chart_calc.moc"
