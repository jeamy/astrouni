#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace astro {

#pragma pack(push, 2)

// 1:1 Port der ORTE Struktur aus legacy/astrouni.h
struct LegacyOrte {
  int16_t cGultig;
  int32_t lIndex;
  int16_t slRec;
  int16_t slOrt;
  char szLand[4];
  float fZone;
  double dBreite;
  double dLange;
};

// 1:1 Port der RADIXFIX Struktur aus legacy/astrouni.h
// Dies ist das exakte Speicherlayout in der Datei astronam.dat
struct LegacyRadixFix {
  char cGultig;        // Record gueltig?
  int32_t lIndexRadix; // Index Record im Radix-Datenfile
  int16_t slRadix;     // Recordlange RadixDaten
  int32_t lIndexName;  // Index im Namen-Datenfile
  int16_t slName;      // Recordlänge Namen
  int32_t lIndexNotiz; // Index Notiz-Datenfile
  int16_t slNotiz;     // länge Notiz
  int16_t clVorName;
  int16_t clName;
  int16_t clBeruf;
  int16_t clOrt;
  double fSommer;
  float fZone;
  char szLand[4];
  double dLange;
  double dBreite;
  double dTime;
  int16_t sTag;
  int16_t sMonat;
  int16_t sJahr;
};

#pragma pack(pop)

// In-Memory Repräsentation der RADIX Struktur
// Enthält die Fixed-Daten und die Strings (die im Legacy Code char* waren)
struct LegacyRadix {
  LegacyRadixFix rFix;
  std::string name;
  std::string vorname;
  std::string beruf;
  std::string ort;
  // Weitere Felder aus RADIX struct können hier ergänzt werden,
  // wenn wir die Berechnungslogik portieren.
};

struct LegacyAuInit {
  int16_t sSelHaus;
  int16_t sSelHoro;
  int16_t sRotRadix;
  int16_t sAspekte;
  int16_t sTeilung;
  // ... weitere Felder ...
};

// Represents the calculated state of a chart
struct AstroRadix {
  LegacyRadix data; // The raw input data

  // Calculated data (mirrors legacy RADIX pointers/arrays)
  std::vector<double> planets;   // pdPlanet (0..MAX_PLANET)
  std::vector<double> houses;    // pdHaus (0..12)
  std::vector<int> planet_types; // psPlanetTyp
  std::vector<int> in_house;     // pszInHaus

  // Calculated variables
  double dBreite; // Latitude in radians
  double dSid;
  double RA;
  double OB;
  double MC;
  double ASC;
  double VER;
  double dJD;
  double dT;

  int cHausSys; // House system selector

  AstroRadix()
      : planets(20), houses(13), planet_types(20), in_house(20), cHausSys(0) {}
};

} // namespace astro
