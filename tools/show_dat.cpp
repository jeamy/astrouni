#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <vector>
#include <cstring>

// Konstanten aus dem Legacy-Code
constexpr int MAX_PLANET = 17;
constexpr int MAX_HAUS = 12;
constexpr int ASPEKTE = 7;

struct SeekPositions {
    long lStart;
    long lOrben;
    long lTrans;
    long lFarben;
};

const char* planet_names[] = {
    "Sonne", "Mond", "Merkur", "Venus", "Mars", "Jupiter", "Saturn",
    "Uranus", "Neptun", "Pluto", "Mondknoten", "Chiron", "Lilith",
    "Aszendent", "MC", "Pars Fortunae", "Vesta"
};

const char* aspect_names[] = {
    "Konjunktion", "Sextil", "Quadrat", "Trigon", "Opposition", "Quincunx", "Halbsextil"
};

void show_astroini_dat(const std::string& filepath) {
    std::cout << "\n=== ASTROINI.DAT ===" << std::endl;
    std::cout << "Datei: " << filepath << std::endl;
    
    std::ifstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Fehler: Konnte Datei nicht öffnen!" << std::endl;
        return;
    }
    
    // Lese Seek-Positionen
    SeekPositions seek;
    file.read(reinterpret_cast<char*>(&seek), sizeof(SeekPositions));
    
    std::cout << "\nSeek-Positionen:" << std::endl;
    std::cout << "  lStart:  " << seek.lStart << std::endl;
    std::cout << "  lOrben:  " << seek.lOrben << std::endl;
    std::cout << "  lTrans:  " << seek.lTrans << std::endl;
    std::cout << "  lFarben: " << seek.lFarben << std::endl;
    
    if (seek.lOrben > 0) {
        file.seekg(seek.lOrben, std::ios::beg);
        
        // Lese Orben-Daten (nur Radix Planeten als Beispiel)
        std::vector<float> radix_planet(MAX_PLANET * MAX_PLANET * ASPEKTE);
        file.read(reinterpret_cast<char*>(radix_planet.data()), radix_planet.size() * sizeof(float));
        
        std::cout << "\nRadix Planeten Orben (Diagonal-Werte):" << std::endl;
        std::cout << std::fixed << std::setprecision(1);
        
        // Zeige nur die Diagonal-Werte (Planet mit sich selbst)
        for (int aspekt = 0; aspekt < ASPEKTE; ++aspekt) {
            std::cout << "\n" << aspect_names[aspekt] << ":" << std::endl;
            for (int planet = 0; planet < MAX_PLANET; ++planet) {
                int index = (planet * MAX_PLANET + planet) + aspekt * MAX_PLANET * MAX_PLANET;
                std::cout << "  " << std::setw(15) << std::left << planet_names[planet] 
                          << ": " << std::setw(5) << std::right << radix_planet[index] << "°" << std::endl;
            }
        }
    }
    
    file.close();
}

void show_astronam_dat(const std::string& filepath) {
    std::cout << "\n=== ASTRONAM.DAT ===" << std::endl;
    std::cout << "Datei: " << filepath << std::endl;
    
    std::ifstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Fehler: Konnte Datei nicht öffnen!" << std::endl;
        return;
    }
    
    // Zeige Dateigröße
    file.seekg(0, std::ios::end);
    long size = file.tellg();
    file.seekg(0, std::ios::beg);
    
    std::cout << "Dateigröße: " << size << " Bytes" << std::endl;
    
    // Zeige erste 500 Bytes als Hex-Dump
    std::cout << "\nErste 500 Bytes (Hex):" << std::endl;
    char buffer[500];
    file.read(buffer, std::min(500L, size));
    int bytes_read = file.gcount();
    
    for (int i = 0; i < bytes_read; i += 16) {
        std::cout << std::hex << std::setfill('0') << std::setw(4) << i << ": ";
        
        // Hex-Werte
        for (int j = 0; j < 16 && (i + j) < bytes_read; ++j) {
            std::cout << std::setw(2) << (int)(unsigned char)buffer[i + j] << " ";
        }
        
        // ASCII-Darstellung
        std::cout << " | ";
        for (int j = 0; j < 16 && (i + j) < bytes_read; ++j) {
            char c = buffer[i + j];
            std::cout << (isprint(c) ? c : '.');
        }
        std::cout << std::endl;
    }
    
    file.close();
}

void show_default_dat(const std::string& filepath) {
    std::cout << "\n=== DEFAULT.DAT ===" << std::endl;
    std::cout << "Datei: " << filepath << std::endl;
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Fehler: Konnte Datei nicht öffnen!" << std::endl;
        return;
    }
    
    std::cout << "\nInhalt (Text-Format):" << std::endl;
    std::cout << "---" << std::endl;
    
    std::string line;
    int line_num = 0;
    while (std::getline(file, line) && line_num < 100) {
        std::cout << std::setw(3) << ++line_num << ": " << line << std::endl;
    }
    
    if (!file.eof()) {
        std::cout << "... (weitere Zeilen vorhanden)" << std::endl;
    }
    
    file.close();
}

void show_file_info(const std::string& filepath) {
    std::ifstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Fehler: Konnte " << filepath << " nicht öffnen!" << std::endl;
        return;
    }
    
    file.seekg(0, std::ios::end);
    long size = file.tellg();
    file.close();
    
    std::cout << "\n" << filepath << ": " << size << " Bytes" << std::endl;
}

int main(int argc, char* argv[]) {
    std::string data_dir = "data/";
    
    if (argc > 1) {
        data_dir = argv[1];
        if (data_dir.back() != '/') {
            data_dir += '/';
        }
    }
    
    std::cout << "==================================================" << std::endl;
    std::cout << "  AstroUniverse .DAT Datei-Viewer" << std::endl;
    std::cout << "==================================================" << std::endl;
    std::cout << "Daten-Verzeichnis: " << data_dir << std::endl;
    
    // Zeige Dateigrößen
    std::cout << "\n--- Dateigrößen ---" << std::endl;
    show_file_info(data_dir + "astroini.dat");
    show_file_info(data_dir + "astronam.dat");
    show_file_info(data_dir + "astrorad.dat");
    show_file_info(data_dir + "astroort.dat");
    show_file_info(data_dir + "astroger.dat");
    show_file_info(data_dir + "default.dat");
    show_file_info(data_dir + "astroorb.dat");
    show_file_info(data_dir + "astroeph.dat");
    
    // Zeige detaillierte Inhalte
    show_astroini_dat(data_dir + "astroini.dat");
    show_default_dat(data_dir + "default.dat");
    show_astronam_dat(data_dir + "astronam.dat");
    
    std::cout << "\n==================================================" << std::endl;
    std::cout << "Hinweis: Verwende './show_dat [pfad]' um ein anderes" << std::endl;
    std::cout << "         Verzeichnis zu analysieren." << std::endl;
    std::cout << "==================================================" << std::endl;
    
    return 0;
}
