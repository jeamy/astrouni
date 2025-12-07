#include <gtest/gtest.h>
#include "legacy_data_manager.h"
#include <fstream>

// Hilfsfunktion zum Erstellen einer Dummy-Ini-Datei
void CreateDummyIni(const std::string& filename) {
    astro::LegacyAuInit dummy_settings;
    dummy_settings.sHsys = 'P'; // Placidus
    dummy_settings.sAnzPlan = 10;
    dummy_settings.dOrben[0][0] = 10.0; // Beispiel-Orb
    dummy_settings.lColor[0] = 0xFF0000; // Beispiel-Farbe (Blau)

    std::ofstream file(filename, std::ios::binary);
    file.write(reinterpret_cast<const char*>(&dummy_settings), sizeof(dummy_settings));
}

TEST(LegacyDataManager, WriteAndReadIni) {
    const std::string test_filename = "test_astroini.dat";
    CreateDummyIni(test_filename);

    astro::LegacyDataManager dm;
    ASSERT_TRUE(dm.ReadIni(test_filename));

    // Überprüfe, ob die gelesenen Werte mit den Dummy-Werten übereinstimmen
    const auto& settings = dm.GetSettings();
    EXPECT_EQ(settings.sHsys, 'P');
    EXPECT_EQ(settings.sAnzPlan, 10);
    EXPECT_DOUBLE_EQ(settings.dOrben[0][0], 10.0);
    EXPECT_EQ(settings.lColor[0], 0xFF0000);
}

TEST(LegacyDataManager, WriteAndReadPerson) {
    const std::string test_filename = "test_astronam.dat";
    astro::LegacyDataManager dm;

    // Erstelle einen Dummy-Radix-Eintrag
    astro::LegacyRadix person_to_write;
    strcpy(person_to_write.szName, "Mustermann");
    strcpy(person_to_write.szVorName, "Max");
    person_to_write.o.dLaenge = 13.37;

    // Schreibe die Person an Index 0
    ASSERT_TRUE(dm.WritePerson(test_filename, person_to_write, 0));

    // Lese die Person von Index 0
    astro::LegacyRadix person_to_read;
    ASSERT_TRUE(dm.ReadPerson(test_filename, person_to_read, 0));

    // Überprüfe die gelesenen Daten
    EXPECT_STREQ(person_to_read.szName, "Mustermann");
    EXPECT_STREQ(person_to_read.szVorName, "Max");
    EXPECT_DOUBLE_EQ(person_to_read.o.dLaenge, 13.37);
}

TEST(LegacyDataManager, WriteAndReadOrte) {
    const std::string test_filename = "test_astroorg.dat";
    astro::LegacyDataManager dm;

    // Erstelle eine Dummy-Ortsliste
    std::vector<astro::LegacyOrte> orte_to_write;
    astro::LegacyOrte ort1, ort2;
    strcpy(ort1.szOrt, "Berlin");
    ort1.dBreite = 52.52;
    strcpy(ort2.szOrt, "München");
    ort2.dBreite = 48.13;
    orte_to_write.push_back(ort1);
    orte_to_write.push_back(ort2);

    // Schreibe die Orte
    ASSERT_TRUE(dm.WriteOrte(test_filename, orte_to_write));

    // Lese die Orte
    std::vector<astro::LegacyOrte> orte_to_read;
    ASSERT_TRUE(dm.ReadOrte(test_filename, orte_to_read));

    // Überprüfe die gelesenen Daten
    ASSERT_EQ(orte_to_read.size(), 2);
    EXPECT_STREQ(orte_to_read[0].szOrt, "Berlin");
    EXPECT_DOUBLE_EQ(orte_to_read[0].dBreite, 52.52);
    EXPECT_STREQ(orte_to_read[1].szOrt, "München");
    EXPECT_DOUBLE_EQ(orte_to_read[1].dBreite, 48.13);
}
