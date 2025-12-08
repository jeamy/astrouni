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
    void testAllHouseSystemsSingleRadix();
    void testAllHouseSystemsMultipleRadix();
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

// Hilfsfunktion: Beispiel-Radix mit fixen Daten initialisieren
static void initSampleRadix(Radix& radix, int tag, int monat, int jahr,
                            double zeit, double laenge, double breite,
                            int hausSys) {
    radix.clear();
    radix.hausSys = static_cast<int8_t>(hausSys);
    radix.rFix.tag = static_cast<int16_t>(tag);
    radix.rFix.monat = static_cast<int16_t>(monat);
    radix.rFix.jahr = static_cast<int16_t>(jahr);
    radix.rFix.zeit = zeit;
    radix.rFix.laenge = laenge;
    radix.rFix.breite = breite;
}

// Test: alle Häusersysteme für einen Beispiel-Radix durchrechnen
void TestChartCalc::testAllHouseSystemsSingleRadix() {
    // Beispiel: 04.04.1918, 02:20, Wien (wie Demo-Datensatz)
    const int tag = 4;
    const int monat = 4;
    const int jahr = 1918;
    const double zeit = 2.333333333; // 02:20
    const double laenge = 16.3667;   // ca. Wien
    const double breite = 48.2000;

    const int houseTypes[] = {
        TYP_KOCH, TYP_PLACIDUS, TYP_EQUAL, TYP_EQUALMID,
        TYP_WHOLE, TYP_TOPOZEN, TYP_CAMPANUS, TYP_MERIDIAN,
        TYP_REGIOMONTANUS, TYP_PORPHYRY, TYP_PORPHYRYN,
        TYP_MORINUS, TYP_ALCABIT, TYP_NULL
    };

    for (int hsys : houseTypes) {
        Radix radix;
        initSampleRadix(radix, tag, monat, jahr, zeit, laenge, breite, hsys);

        int result = ChartCalc::calculate(radix, nullptr, TYP_RADIX);
        QVERIFY2(result == ERR_OK, QByteArray("House system calc failed for type ") + QByteArray::number(hsys));

        // Grundlegende Invarianten prüfen
        QCOMPARE(radix.hausSys, static_cast<int8_t>(hsys));
        // haus wird intern als MAX_HAUS+1 (13 Einträge, 0-12) angelegt
        QCOMPARE(radix.haus.size(), static_cast<int>(MAX_HAUS) + 1);

        // Häuser müssen im Bereich [0,360) liegen
        for (int i = 0; i < MAX_HAUS; ++i) {
            QVERIFY2(radix.haus[i] >= 0.0 && radix.haus[i] < 360.0,
                     QByteArray("haus[") + QByteArray::number(i) + "] out of range for type " + QByteArray::number(hsys));
        }
    }
}

// Test: mehrere Beispiel-Radixe (unterschiedliche Orte/Zeiten) über alle Häusersysteme
void TestChartCalc::testAllHouseSystemsMultipleRadix() {
    struct Sample {
        int tag;
        int monat;
        int jahr;
        double zeit;    // Dezimalstunden
        double laenge;  // Ost positiv
        double breite;  // Nord positiv
    };

    // Drei typische Beispiele (Europa, USA, Südhalbkugel)
    const Sample samples[] = {
        { 4,  4, 1918,  2.333333333,  16.3667,  48.2000 }, // Wien
        { 20, 7, 1984, 14.500000000, -74.0060,  40.7128 }, // New York (ca.)
        { 10,12,2000,  6.750000000, 151.2093, -33.8688 }  // Sydney (ca.)
    };

    const int houseTypes[] = {
        TYP_KOCH, TYP_PLACIDUS, TYP_EQUAL, TYP_EQUALMID,
        TYP_WHOLE, TYP_TOPOZEN, TYP_CAMPANUS, TYP_MERIDIAN,
        TYP_REGIOMONTANUS, TYP_PORPHYRY, TYP_PORPHYRYN,
        TYP_MORINUS, TYP_ALCABIT, TYP_NULL
    };

    for (const auto& s : samples) {
        for (int hsys : houseTypes) {
            Radix radix;
            initSampleRadix(radix, s.tag, s.monat, s.jahr,
                            s.zeit, s.laenge, s.breite, hsys);

            int result = ChartCalc::calculate(radix, nullptr, TYP_RADIX);
            QVERIFY2(result == ERR_OK,
                     QByteArray("calculate() failed for sample and house type ") + QByteArray::number(hsys));

            // Prüfen: ASC und MC im gültigen Bereich
            QVERIFY(radix.asc >= 0.0 && radix.asc < 360.0);
            QVERIFY(radix.mc  >= 0.0 && radix.mc  < 360.0);

            // Planetenanzahl sollte gesetzt sein und mindestens Sonne/Mond enthalten
            QVERIFY(radix.anzahlPlanet >= 2);
            QVERIFY(radix.planet[0] >= 0.0 && radix.planet[0] < 360.0);
            QVERIFY(radix.planet[1] >= 0.0 && radix.planet[1] < 360.0);
        }
    }
}

QTEST_MAIN(TestChartCalc)
#include "test_chart_calc.moc"
