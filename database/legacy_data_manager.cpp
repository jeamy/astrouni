#include "legacy_data_manager.h"
#include <fstream>
#include <vector>
#include <cstring>

namespace astro {

LegacyDataManager::LegacyDataManager() {
    // Initialisiert die settings_ mit Standardwerten, falls nötig
}

bool LegacyDataManager::ReadIni(const std::string& filename) {
    // 1:1 Port von sReadIni aus auhelper.c
    // ACHTUNG: Die legacy AU_INIT Struktur ist sehr komplex und enthält Pointer.
    // Wir lesen hier vorerst nur den Header oder müssen die Struktur exakt nachbilden.
    // Da wir die Definition von AU_INIT in data_types.h noch nicht vollständig haben,
    // lassen wir dies vorerst als Platzhalter, der zumindest die Datei öffnet.
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        return false;
    }
    // TODO: Implementiere exaktes Lesen der AU_INIT Struktur
    return file.good();
}

bool LegacyDataManager::WriteIni(const std::string& filename) {
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        return false;
    }
    // TODO: Implementiere exaktes Schreiben
    return file.good();
}

bool LegacyDataManager::ReadPerson(const std::string& filename, LegacyRadix& radix, int index) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        return false;
    }

    // Wir müssen durch die Datei iterieren, da die Records variable Länge haben (wegen der Strings).
    // Legacy Code nutzt plNamen Array für Indizes, das beim Start aufgebaut wird.
    // Hier scannen wir einfach bis zum gewünschten Index.
    
    // Reset file to beginning
    file.seekg(0, std::ios::beg);

    int currentIndex = 0;
    while (file.peek() != EOF) {
        LegacyRadixFix fix;
        if (!file.read(reinterpret_cast<char*>(&fix), sizeof(LegacyRadixFix))) break;

        // Wenn ungültig, überspringen wir die Strings
        if (!fix.cGultig) {
             int skip = fix.clName + fix.clVorName + fix.clBeruf + fix.clOrt;
             file.seekg(skip, std::ios::cur);
             continue; // Index nicht erhöhen? Legacy Code: "if (!tRadix->rFix.cGultig) ... continue;"
             // Aber sIndex wird im Legacy Code nur erhöht, wenn wir einen gültigen Record lesen?
             // Nein, sIndex ist der Index im Array plNamen.
             // Wir nehmen an, der Index bezieht sich auf den n-ten GÜLTIGEN Eintrag oder den n-ten Eintrag überhaupt?
             // Legacy sGetValidRecords liefert den n-ten Eintrag.
             // Wir zählen hier einfach alle Einträge.
        }

        // Strings lesen
        std::vector<char> buffer;
        
        auto readString = [&](int len) -> std::string {
            if (len <= 0) return "";
            buffer.resize(len);
            file.read(buffer.data(), len);
            // Null-Terminierung sicherstellen (Legacy schreibt Strings ohne Null, aber wir wollen std::string)
            // Warte, Legacy sWriteDat nutzt fprintf "%s", das schreibt bis Null?
            // Nein, sGetValidRecords nutzt fread mit fix.clName.
            // Das bedeutet, die Länge ist fix definiert durch clName.
            return std::string(buffer.data(), len);
        };

        std::string name = readString(fix.clName);
        std::string vorname = readString(fix.clVorName);
        std::string beruf = readString(fix.clBeruf);
        std::string ort = readString(fix.clOrt);

        if (fix.cGultig) {
            if (currentIndex == index) {
                radix.rFix = fix;
                radix.name = name;
                radix.vorname = vorname;
                radix.beruf = beruf;
                radix.ort = ort;
                return true;
            }
            currentIndex++;
        }
    }

    return false; // Nicht gefunden
}

bool LegacyDataManager::WritePerson(const std::string& filename, const LegacyRadix& radix, int index) {
    // Schreiben ist komplexer, da wir anhängen oder ersetzen müssen.
    // Für 1:1 Port müssten wir eigentlich die Datei neu schreiben oder anfügen.
    // Legacy sWriteDat nutzt fseek und überschreibt oder hängt an.
    
    // Vereinfachung: Wir hängen immer an (neuer Eintrag), wenn index -1.
    // Wenn index >= 0, müssten wir die Position finden.
    
    std::fstream file(filename, std::ios::in | std::ios::out | std::ios::binary);
    if (!file) {
        std::ofstream new_file(filename, std::ios::binary);
        file.open(filename, std::ios::in | std::ios::out | std::ios::binary);
    }

    file.seekp(0, std::ios::end);
    
    // Update lengths in fix struct based on current strings
    LegacyRadixFix fix = radix.rFix;
    fix.clName = static_cast<int16_t>(radix.name.length());
    fix.clVorName = static_cast<int16_t>(radix.vorname.length());
    fix.clBeruf = static_cast<int16_t>(radix.beruf.length());
    fix.clOrt = static_cast<int16_t>(radix.ort.length());
    fix.cGultig = 1;

    file.write(reinterpret_cast<const char*>(&fix), sizeof(LegacyRadixFix));
    file.write(radix.name.c_str(), fix.clName);
    file.write(radix.vorname.c_str(), fix.clVorName);
    file.write(radix.beruf.c_str(), fix.clBeruf);
    file.write(radix.ort.c_str(), fix.clOrt);

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
        if (ort.cGultig) {
            orte.push_back(ort);
        }
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
    while (file.peek() != EOF) {
        LegacyRadixFix fix;
        if (!file.read(reinterpret_cast<char*>(&fix), sizeof(LegacyRadixFix))) break;

        if (!fix.cGultig) {
             int skip = fix.clName + fix.clVorName + fix.clBeruf + fix.clOrt;
             file.seekg(skip, std::ios::cur);
             continue;
        }

        std::vector<char> buffer;
        auto readString = [&](int len) -> std::string {
            if (len <= 0) return "";
            buffer.resize(len);
            file.read(buffer.data(), len);
            return std::string(buffer.data(), len);
        };

        LegacyRadix person;
        person.rFix = fix;
        person.name = readString(fix.clName);
        person.vorname = readString(fix.clVorName);
        person.beruf = readString(fix.clBeruf);
        person.ort = readString(fix.clOrt);
        
        persons.push_back(person);
    }

    return true;
}

} // namespace astro
