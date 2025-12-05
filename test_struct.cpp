#include "astro/calc/astro_data.h"
#include <iostream>

using namespace astro;

struct AstroKor test_kor[] = {{1, 0, .884, 288.05, 111, 39.90, 15}};

int main() {
  std::cout << test_kor[0].rphase << std::endl;
  return 0;
}
