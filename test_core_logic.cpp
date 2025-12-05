#include "astro/calc/astro_calc.h"
#include "database/legacy_data_manager.h"
#include <cmath>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

std::string FormatZodiac(double decimal_degrees) {
  const char *signs[] = {"Widder",  "Stier",     "Zwillinge",  "Krebs",
                         "Löwe",    "Jungfrau",  "Waage",      "Skorpion",
                         "Schütze", "Steinbock", "Wassermann", "Fische"};

  // Normalize to 0-360
  while (decimal_degrees < 0)
    decimal_degrees += 360.0;
  while (decimal_degrees >= 360.0)
    decimal_degrees -= 360.0;

  int sign_index = static_cast<int>(decimal_degrees / 30.0);
  double degrees_in_sign = decimal_degrees - (sign_index * 30.0);

  int d = static_cast<int>(degrees_in_sign);
  double remainder_minutes = (degrees_in_sign - d) * 60.0;
  int m = static_cast<int>(remainder_minutes);
  double remainder_seconds = (remainder_minutes - m) * 60.0;
  int s = static_cast<int>(remainder_seconds + 0.5); // Round to nearest second

  if (s == 60) {
    s = 0;
    m++;
  }
  if (m == 60) {
    m = 0;
    d++;
  }

  char buffer[64];
  snprintf(buffer, sizeof(buffer), "%d°%02d'%02d\" %s", d, m, s,
           signs[sign_index]);
  return std::string(buffer);
}

int main() {
  astro::LegacyDataManager dataManager;

  astro::LegacyRadix p;
  if (!dataManager.ReadPerson("legacy/astronam.dat", p, 0)) {
    std::cerr << "Failed to read person from legacy/astronam.dat" << std::endl;
    return 1;
  }

  std::cout << "Calculating chart for: " << p.name << " " << p.vorname
            << std::endl;
  std::cout << "Date: " << p.rFix.sTag << "." << p.rFix.sMonat << "."
            << p.rFix.sJahr << std::endl;
  std::cout << "Time: " << p.rFix.dTime << std::endl;
  std::cout << "Lat: " << p.rFix.dBreite << ", Lon: " << p.rFix.dLange
            << std::endl;

  astro::AstroRadix radix;
  radix.data = p;

  astro::AstroCalculator calc;
  // Set house system to Koch (as per user's screenshot request)
  radix.cHausSys = astro::TYP_KOCH;

  if (calc.CalcChart(radix)) {
    std::cout << "Calculation successful." << std::endl;
    std::cout << "Julian Day: " << std::fixed << std::setprecision(4)
              << radix.dJD << std::endl;
    std::cout << "Sidereal Time: " << radix.dSid << std::endl;
    std::cout << "MC: " << FormatZodiac(radix.MC) << " (" << radix.MC << ")"
              << std::endl;
    std::cout << "ASC: " << FormatZodiac(radix.ASC) << " (" << radix.ASC << ")"
              << std::endl;

    std::cout << "Houses (Koch):" << std::endl;
    for (int i = 0; i < 12; ++i) {
      std::cout << "House " << (i + 1) << ": " << FormatZodiac(radix.houses[i])
                << " (" << radix.houses[i] << ")" << std::endl;
    }

    std::cout << "Planets:" << std::endl;
    const char *planet_names[] = {"Sun", "Moon", "Mercury", "Venus", "Mars"};
    for (int i = 0; i <= astro::P_MARS; ++i) {
      std::cout << planet_names[i] << ": " << FormatZodiac(radix.planets[i])
                << " (" << radix.planets[i] << ")" << std::endl;
    }
  } else {
    std::cerr << "Calculation failed!" << std::endl;
    return 1;
  }

  return 0;
}
