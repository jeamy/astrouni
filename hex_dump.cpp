#include <iostream>
#include <fstream>
#include <iomanip>

int main() {
    std::ifstream file("legacy/astronam.dat", std::ios::binary);
    if (!file) {
        std::cerr << "Could not open file." << std::endl;
        return 1;
    }

    unsigned char buffer[128];
    file.read(reinterpret_cast<char*>(buffer), 128);
    int bytesRead = file.gcount();

    std::cout << "Hex dump of first " << bytesRead << " bytes:" << std::endl;
    for (int i = 0; i < bytesRead; ++i) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)buffer[i] << " ";
        if ((i + 1) % 16 == 0) std::cout << std::endl;
    }
    std::cout << std::endl;

    return 0;
}
