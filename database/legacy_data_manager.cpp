#include "legacy_data_manager.h"
#include <fstream>

namespace astro {

LegacyDataManager::LegacyDataManager() {
    // Initialisiert die settings_ mit Standardwerten, falls nötig
}

bool LegacyDataManager::ReadIni(const std::string& filename) {
    // 1:1 Port von sReadIni aus auhelper.c
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        return false;
    }

    // Lese die LegacyAuInit-Struktur aus der Datei
    file.read(reinterpret_cast<char*>(&settings_), sizeof(LegacyAuInit));

    return file.good();
}

bool LegacyDataManager::WriteIni(const std::string& filename) {
    // 1:1 Port von sWriteIni aus auhelper.c
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        return false;
    }

    // Schreibe die LegacyAuInit-Struktur in die Datei
    file.write(reinterpret_cast<const char*>(&settings_), sizeof(LegacyAuInit));

    return file.good();
}

bool LegacyDataManager::ReadPerson(const std::string& filename, LegacyRadix& radix, int index) {
    // 1:1 Port von sReadDat aus astrofil.c
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        return false;
    }

    file.seekg(index * sizeof(LegacyRadix));
    file.read(reinterpret_cast<char*>(&radix), sizeof(LegacyRadix));

    return file.good();
}

bool LegacyDataManager::WritePerson(const std::string& filename, const LegacyRadix& radix, int index) {
    // 1:1 Port von sWriteDat aus astrofil.c
    std::fstream file(filename, std::ios::in | std::ios::out | std::ios::binary);
    if (!file) {
        // Datei existiert möglicherweise nicht, also im Schreibmodus neu erstellen
        std::ofstream new_file(filename, std::ios::binary);
        if (!new_file) return false;
        file.open(filename, std::ios::in | std::ios::out | std::ios::binary);
    }

    file.seekp(index * sizeof(LegacyRadix));
    file.write(reinterpret_cast<const char*>(&radix), sizeof(LegacyRadix));

    return file.good();
}


bool LegacyDataManager::ReadOrte(const std::string& filename, std::vector<LegacyOrte>& orte) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        return false;
    }

    orte.clear();
    LegacyOrte ort;
    while (file.read(reinterpret_cast<char*>(&ort), sizeof(LegacyOrte))) {
        orte.push_back(ort);
    }

    return true;
}

bool LegacyDataManager::WriteOrte(const std::string& filename, const std::vector<LegacyOrte>& orte) {
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        return false;
    }

    for (const auto& ort : orte) {
        file.write(reinterpret_cast<const char*>(&ort), sizeof(LegacyOrte));
    }

    return file.good();
}

bool LegacyDataManager::ReadAllPersons(const std::string& filename, std::vector<LegacyRadix>& persons) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        return false;
    }

    persons.clear();
    LegacyRadix person;
    while (file.read(reinterpret_cast<char*>(&person), sizeof(LegacyRadix))) {
        persons.push_back(person);
    }

    return true;
}

} // namespace astro
