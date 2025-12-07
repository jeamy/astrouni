# AstroUniverse Datenformat-Spezifikation

## Übersicht

AstroUniverse unterstützt zwei Datenformate:
1. **Legacy-Format** (.dat) - Kompatibilität mit AstroUnivers v0.04Beta
2. **Modernes Format** (.auc) - JSON-basiert, erweiterbar

## Legacy-Format (.dat)

### Struktur (Binär)

```c
// Original RADIXFIX Struktur
struct RADIXFIX {
    char name[41];           // Name (40 + \0)
    int tag;                 // Tag (1-31)
    int monat;               // Monat (1-12)
    int jahr;                // Jahr
    int stunde;              // Stunde (0-23)
    int minute;              // Minute (0-59)
    int sekunde;             // Sekunde (0-59)
    double laenge;           // Längengrad (Ost positiv)
    double breite;           // Breitengrad (Nord positiv)
    int zeitzone;            // Zeitzone (Minuten von GMT)
    char ort[41];            // Ortsname (40 + \0)
};
```

**Dateiformat:**
- Binäre Datei mit fester Struktur
- Keine Versionierung
- Keine Metadaten
- Encoding: CP437 (DOS)

### Lesen von Legacy-Dateien

```cpp
class LegacyReader {
public:
    static RadixData read_dat_file(const std::string& filename);
    static std::vector<RadixData> read_dat_batch(const std::string& filename);
    
private:
    static std::string convert_cp437_to_utf8(const char* str);
};
```

## Modernes Format (.auc)

### JSON-Struktur

```json
{
  "version": "1.0",
  "format": "AstroUniverse Chart",
  "created": "2025-11-06T21:00:00Z",
  "modified": "2025-11-06T21:00:00Z",
  
  "chart": {
    "name": "Beispiel Person",
    "birth_datetime": {
      "year": 2000,
      "month": 1,
      "day": 1,
      "hour": 12,
      "minute": 0,
      "second": 0.0,
      "timezone": "UTC",
      "timezone_offset_minutes": 0
    },
    "birth_location": {
      "name": "Berlin",
      "country": "DE",
      "latitude": 52.52,
      "longitude": 13.40,
      "altitude": 34.0,
      "timezone": "Europe/Berlin"
    },
    "house_system": "Placidus",
    "calculation_flags": {
      "sidereal": false,
      "topocentric": false
    }
  },
  
  "calculated_data": {
    "julian_date": 2451545.0,
    "sidereal_time": 18.697374558,
    
    "planets": [
      {
        "planet": "Sun",
        "longitude": 280.37,
        "latitude": 0.0,
        "distance": 0.98334,
        "speed_longitude": 1.02,
        "speed_latitude": 0.0,
        "speed_distance": 0.0,
        "is_retrograde": false,
        "house": 10
      }
    ],
    
    "houses": [
      {
        "house_number": 1,
        "longitude": 51.62,
        "system": "Placidus"
      }
    ],
    
    "aspects": [
      {
        "planet1": "Sun",
        "planet2": "Saturn",
        "type": "Sextile",
        "angle": 60.0,
        "orb": 0.03,
        "is_applying": true
      }
    ]
  },
  
  "metadata": {
    "notes": "",
    "tags": ["natal", "example"],
    "source": "manual_entry",
    "legacy_file": "beispiel.dat"
  }
}
```

### Vorteile

- ✅ **Versionierung** - Format kann erweitert werden
- ✅ **UTF-8** - Internationale Zeichen
- ✅ **Metadaten** - Notizen, Tags, Quelle
- ✅ **Erweiterbar** - Neue Felder ohne Breaking Changes
- ✅ **Lesbar** - Menschen und Maschinen
- ✅ **Validierbar** - JSON Schema

## Orte-Datenbank

### Struktur (SQLite)

```sql
CREATE TABLE locations (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL,
    country_code TEXT(2),
    latitude REAL NOT NULL,
    longitude REAL NOT NULL,
    altitude REAL DEFAULT 0,
    timezone TEXT,
    population INTEGER,
    source TEXT,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE INDEX idx_locations_name ON locations(name);
CREATE INDEX idx_locations_country ON locations(country_code);

-- Beispiel-Daten
INSERT INTO locations (name, country_code, latitude, longitude, altitude, timezone, population)
VALUES 
    ('Berlin', 'DE', 52.52, 13.40, 34, 'Europe/Berlin', 3644826),
    ('München', 'DE', 48.14, 11.58, 520, 'Europe/Berlin', 1471508),
    ('Hamburg', 'DE', 53.55, 9.99, 6, 'Europe/Berlin', 1841179),
    ('Wien', 'AT', 48.21, 16.37, 171, 'Europe/Vienna', 1897491),
    ('Zürich', 'CH', 47.37, 8.54, 408, 'Europe/Zurich', 415367);
```

### API

```cpp
class LocationDatabase {
public:
    LocationDatabase(const std::string& db_path);
    
    // Suche
    std::vector<Location> search(const std::string& query, int limit = 10);
    std::vector<Location> search_by_country(const std::string& country_code);
    Location get_by_id(int id);
    
    // CRUD
    int add_location(const Location& loc);
    bool update_location(int id, const Location& loc);
    bool delete_location(int id);
    
    // Import
    void import_from_geonames(const std::string& file);
    void import_legacy_orte(const std::string& file);
};
```

## Migration Legacy → Modern

### Automatische Konvertierung

```cpp
class DataMigration {
public:
    // Konvertiere .dat → .auc
    static bool convert_dat_to_auc(
        const std::string& dat_file,
        const std::string& auc_file
    );
    
    // Batch-Konvertierung
    static int convert_directory(
        const std::string& input_dir,
        const std::string& output_dir
    );
    
    // Legacy-Daten einbetten
    static void embed_legacy_data(
        const std::string& auc_file,
        const std::string& dat_file
    );
};
```

### Beim Laden

```cpp
RadixData load_chart(const std::string& filename) {
    if (ends_with(filename, ".dat")) {
        // Legacy-Format
        auto data = LegacyReader::read_dat_file(filename);
        
        // Zeige Hinweis: "Legacy-Datei geladen"
        // Biete an: "Als .auc speichern?"
        
        return data;
    }
    else if (ends_with(filename, ".auc")) {
        // Modernes Format
        auto data = ModernReader::read_auc_file(filename);
        
        // Prüfe ob Legacy-Daten eingebettet sind
        if (data.has_legacy_data()) {
            // Zeige: "Original: beispiel.dat"
        }
        
        return data;
    }
}
```

## Datei-Struktur

```
~/.astro_universe/
├── charts/                  # Gespeicherte Charts
│   ├── beispiel.auc
│   ├── transit_2025.auc
│   └── legacy/              # Importierte Legacy-Dateien
│       └── alt.dat
│
├── locations.db             # SQLite Orte-Datenbank
├── settings.json            # Benutzer-Einstellungen
└── recent.json              # Zuletzt geöffnete Dateien
```

## Kompatibilität

### Lesen
- ✅ `.dat` (Legacy) - Vollständig unterstützt
- ✅ `.auc` (Modern) - Natives Format

### Schreiben
- ✅ `.auc` (Modern) - Standard
- ⚠️ `.dat` (Legacy) - Nur für Kompatibilität (Datenverlust möglich)

### Empfehlung
- Neue Charts: `.auc`
- Legacy-Import: `.dat` → `.auc` konvertieren
- Backup: Beide Formate behalten
