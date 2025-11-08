#pragma once

#include <cstdint>
#include <vector>

namespace astro {

// 1:1 Port der DATZEIT Struktur aus legacy/astrouni.h
struct LegacyDateTime {
    int16_t iMin;
    int16_t iStu;
    int16_t iTag;
    int16_t iMon;
    int16_t iJah;
    int16_t iSom;
    int16_t iZone;
    double dGmt;
};

// 1:1 Port der ORTE Struktur aus legacy/astrouni.h
struct LegacyOrte {
    char szOrt[31];
    char szStaat[31];
    char cLaenge;
    double dLaenge;
    char cBreite;
    double dBreite;
    LegacyDateTime dt;
};

// 1:1 Port der RADIX Struktur aus legacy/astrouni.h
struct LegacyRadix {
    char szName[31];
    char szVorName[31];
    LegacyOrte o;
    double dHaeuser[13];
    double dPlaneten[12];
    double dAspekte[12][12];
    uint8_t cRetro[12];
    char cBesetzt[13];
};

// 1:1 Port der AUINIT Struktur aus legacy/astrouni.h
struct LegacyAuInit {
    LegacyRadix Radix;
    char szPersonFile[81];
    char szOrteFile[81];
    char szHoroFile[81];
    int16_t sSelHoro;
    int16_t sHsys;
    int16_t sAnzPlan;
    int16_t sAnzAsp;
    int16_t sAnzOrben;
    double dOrben[12][12];
    uint32_t lColor[20];
};

} // namespace astro
