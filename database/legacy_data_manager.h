#pragma once

#include "astro/data_types.h"
#include <string>
#include <vector>

namespace astro {

// 1:1 Port der Daten-Handling-Funktionen aus astrofil.c und auhelper.c
class LegacyDataManager {
public:
    LegacyDataManager();

    // Port von sReadIni aus auhelper.c
    bool ReadIni(const std::string& filename);

    // Port von sWriteIni aus auhelper.c
    bool WriteIni(const std::string& filename);

    // Port von sReadDat aus astrofil.c
    bool ReadPerson(const std::string& filename, LegacyRadix& radix, int index);

    // Port von sWriteDat aus astrofil.c
    bool WritePerson(const std::string& filename, const LegacyRadix& radix, int index);

    // Port von sOrteWrite aus astrofil.c (Lese-Teil)
    bool ReadOrte(const std::string& filename, std::vector<LegacyOrte>& orte);

    // Port von sOrteWrite aus astrofil.c (Schreib-Teil)
    bool WriteOrte(const std::string& filename, const std::vector<LegacyOrte>& orte);


public:
    const LegacyAuInit& GetSettings() const { return settings_; }

private:
    LegacyAuInit settings_;
};

} // namespace astro
