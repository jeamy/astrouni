#include <iostream>
#include <iomanip>
#include <string>
#include <cmath>
#include "astrocore/api.hpp"

int main(int argc, char** argv) {
  using namespace astrocore;
  auto v = version();
  std::cout << "cma  version " << v.major << '.' << v.minor << '.' << v.patch << "\n";

  auto deg2rad = [](double d){ return d * M_PI / 180.0; };
  auto rad2deg = [](double r){ return r * 180.0 / M_PI; };

  if (argc > 1) {
    std::string cmd = argv[1];
    if (cmd == "jd") {
      if (argc < 10) {
        std::cerr << "Usage: " << argv[0] << " jd YYYY MM DD HH MM SS utcOffsetHours dstHours\n";
        return 1;
      }
      Date d{ std::stoi(argv[2]), std::stoi(argv[3]), std::stoi(argv[4]) };
      Time t{ std::stoi(argv[5]), std::stoi(argv[6]), std::stod(argv[7]) };
      TimeZone tz{ std::stod(argv[8]), std::stod(argv[9]) };
      double jd = julian_day(d, t, tz);
      std::cout << std::fixed << std::setprecision(9)
                << "JD = " << jd << "\n";
      return 0;
    } else if (cmd == "xform") {
      if (argc < 5) {
        std::cerr << "Usage: " << argv[0] << " xform azimuth_deg altitude_deg shift_deg\n";
        return 1;
      }
      double azi = deg2rad(std::stod(argv[2]));
      double alt = deg2rad(std::stod(argv[3]));
      double sh  = deg2rad(std::stod(argv[4]));
      coord_transform(azi, alt, sh);
      std::cout << std::fixed << std::setprecision(6)
                << "azimuth_deg= " << rad2deg(azi)
                << ", altitude_deg= " << rad2deg(alt) << "\n";
      return 0;
    } else if (cmd == "gmst") {
      if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " gmst JD\n";
        return 1;
      }
      double jd = std::stod(argv[2]);
      double g = gmst_rad(jd);
      std::cout << std::fixed << std::setprecision(6)
                << "GMST_rad= " << g << ", GMST_deg= " << rad2deg(g) << "\n";
      return 0;
    } else if (cmd == "lst") {
      if (argc < 4) {
        std::cerr << "Usage: " << argv[0] << " lst JD longitude_deg\n";
        return 1;
      }
      double jd = std::stod(argv[2]);
      double lon = deg2rad(std::stod(argv[3]));
      double l = lst_rad(jd, lon);
      std::cout << std::fixed << std::setprecision(6)
                << "LST_rad= " << l << ", LST_deg= " << rad2deg(l) << "\n";
      return 0;
    } else if (cmd == "aspect") {
      if (argc < 5) {
        std::cerr << "Usage: " << argv[0] << " aspect lon1_deg lon2_deg orb_deg\n";
        return 1;
      }
      double lon1 = std::stod(argv[2]);
      double lon2 = std::stod(argv[3]);
      double orb = std::stod(argv[4]);
      AspectResult r = detect_aspect(lon1, lon2, orb);
      std::string name = "None";
      switch (r.type) {
        case AspectType::Conjunction: name = "Conjunction"; break;
        case AspectType::Opposition:  name = "Opposition";  break;
        case AspectType::Trine:       name = "Trine";       break;
        case AspectType::Square:      name = "Square";      break;
        case AspectType::Sextile:     name = "Sextile";     break;
        default: break;
      }
      std::cout << std::fixed << std::setprecision(3)
                << "aspect= " << name
                << ", exact= " << r.exactAngleDeg
                << ", delta= " << r.deltaDeg << "\n";
      return 0;
    } else if (cmd == "ascmc") {
      if (argc < 13) {
        std::cerr << "Usage: " << argv[0] << " ascmc YYYY MM DD HH MM SS utcOffsetHours dstHours longitude_deg latitude_deg\n";
        return 1;
      }
      Date d{ std::stoi(argv[2]), std::stoi(argv[3]), std::stoi(argv[4]) };
      Time t{ std::stoi(argv[5]), std::stoi(argv[6]), std::stod(argv[7]) };
      TimeZone tz{ std::stod(argv[8]), std::stod(argv[9]) };
      double lon = deg2rad(std::stod(argv[10]));
      double lat = deg2rad(std::stod(argv[11]));
      double jd = julian_day(d, t, tz);
      double asc=0.0, mc=0.0;
      asc_mc_longitudes(jd, lon, lat, asc, mc);
      std::cout << std::fixed << std::setprecision(6)
                << "Asc_deg= " << rad2deg(asc) << ", MC_deg= " << rad2deg(mc) << "\n";
      return 0;
    } else if (cmd == "houses") {
      if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " houses asc_deg\n";
        return 1;
      }
      double asc = deg2rad(std::stod(argv[2]));
      double cusps[12];
      equal_house_cusps(asc, cusps);
      std::cout << std::fixed << std::setprecision(3);
      for (int i = 0; i < 12; ++i) {
        if (i) std::cout << ", ";
        std::cout << rad2deg(cusps[i]);
      }
      std::cout << "\n";
      return 0;
    } else {
      // fallback: treat as number for demo_compute
      try {
        long long x = std::stoll(cmd);
        std::cout << "demo_compute(" << x << ") = " << demo_compute(x) << "\n";
        return 0;
      } catch (...) {
        std::cerr << "Unknown command. Use: jd | xform | <number>\n";
        return 1;
      }
    }
  }

  // default demo
  long long x = 21;
  std::cout << "demo_compute(" << x << ") = " << demo_compute(x) << "\n";
  return 0;
}
