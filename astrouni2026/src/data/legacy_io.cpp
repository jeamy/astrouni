/**
 * @file legacy_io.cpp
 * @brief Implementierung der Legacy-Datei I/O
 */

#include "legacy_io.h"
#include <QFile>
#include <QDataStream>
#include <QDir>
#include <QTextStream>
#include <QRegularExpression>

namespace astro {

//==============================================================================
// Konstruktor / Destruktor
//==============================================================================

LegacyIO::LegacyIO() {
}

LegacyIO::~LegacyIO() {
}

//==============================================================================
// Pfad-Verwaltung
//==============================================================================

void LegacyIO::setDataPath(const QString& path) {
    m_dataPath = path;
    if (!m_dataPath.endsWith('/') && !m_dataPath.endsWith('\\')) {
        m_dataPath += '/';
    }
}

QString LegacyIO::getDataPath() const {
    return m_dataPath;
}

QString LegacyIO::getFilePath(const QString& filename) const {
    return m_dataPath + filename;
}

//==============================================================================
// Fehlerbehandlung
//==============================================================================

QString LegacyIO::getLastError() const {
    return m_lastError;
}

void LegacyIO::setError(const QString& error) {
    m_lastError = error;
}

//==============================================================================
// Farb-Konvertierung
//==============================================================================

uint32_t LegacyIO::colorToLegacy(const QColor& color) const {
    // Legacy-Format: RGB (Windows COLORREF)
    return static_cast<uint32_t>(color.red()) |
           (static_cast<uint32_t>(color.green()) << 8) |
           (static_cast<uint32_t>(color.blue()) << 16);
}

QColor LegacyIO::legacyToColor(uint32_t legacy) const {
    return QColor(
        legacy & 0xFF,
        (legacy >> 8) & 0xFF,
        (legacy >> 16) & 0xFF
    );
}

//==============================================================================
// INI-Datei
//==============================================================================

int LegacyIO::readIni(AuInit& auinit) {
    QString filepath = getFilePath(INIDAT);
    QFile file(filepath);
    
    if (!file.open(QIODevice::ReadOnly)) {
        // astroini.dat existiert nicht - Defaults aus default.dat laden
        return readDefaultOrben(auinit);
    }
    
    // Header lesen (20 Bytes)
    int16_t version;
    file.read(reinterpret_cast<char*>(&version), sizeof(version));
    file.read(reinterpret_cast<char*>(&auinit.sSelHaus), sizeof(auinit.sSelHaus));
    file.read(reinterpret_cast<char*>(&auinit.sSelHoro), sizeof(auinit.sSelHoro));
    file.read(reinterpret_cast<char*>(&auinit.sRotRadix), sizeof(auinit.sRotRadix));
    file.read(reinterpret_cast<char*>(&auinit.sAspekte), sizeof(auinit.sAspekte));
    file.read(reinterpret_cast<char*>(&auinit.sTeilung), sizeof(auinit.sTeilung));
    
    // Padding überspringen
    file.seek(20);
    
    // Orben lesen
    int sizePlanet = MAX_PLANET * MAX_PLANET * ASPEKTE;
    int sizeHaus = MAX_PLANET * MAX_HAUS * ASPEKTE;
    
    auinit.orbenPlanet.resize(sizePlanet);
    auinit.orbenHaus.resize(sizeHaus);
    auinit.orbenTPlanet.resize(sizePlanet);
    auinit.orbenTHaus.resize(sizeHaus);
    auinit.orbenSPlanet.resize(sizePlanet);
    auinit.orbenSHaus.resize(sizeHaus);
    
    qint64 bytesRead = 0;
    bytesRead += file.read(reinterpret_cast<char*>(auinit.orbenPlanet.data()), sizePlanet * sizeof(float));
    bytesRead += file.read(reinterpret_cast<char*>(auinit.orbenHaus.data()), sizeHaus * sizeof(float));
    bytesRead += file.read(reinterpret_cast<char*>(auinit.orbenTPlanet.data()), sizePlanet * sizeof(float));
    bytesRead += file.read(reinterpret_cast<char*>(auinit.orbenTHaus.data()), sizeHaus * sizeof(float));
    bytesRead += file.read(reinterpret_cast<char*>(auinit.orbenSPlanet.data()), sizePlanet * sizeof(float));
    bytesRead += file.read(reinterpret_cast<char*>(auinit.orbenSHaus.data()), sizeHaus * sizeof(float));
    
    file.close();
    
    // Prüfen ob Orben gültig sind (nicht alle 0)
    // Wenn alle Werte 0 sind, wurden keine gültigen Orben gespeichert
    bool hasValidOrbs = false;
    for (int i = 0; i < qMin(100, sizePlanet); ++i) {
        if (auinit.orbenPlanet[i] > 0.0f) {
            hasValidOrbs = true;
            break;
        }
    }
    
    if (!hasValidOrbs || bytesRead == 0) {
        // Keine gültigen Orben in astroini.dat - aus default.dat laden
        return readDefaultOrben(auinit);
    }
    
    return ERR_OK;
}

int LegacyIO::writeIni(const AuInit& auinit) {
    QString filepath = getFilePath(INIDAT);
    QFile file(filepath);
    
    if (!file.open(QIODevice::WriteOnly)) {
        setError(QString("Kann Datei nicht schreiben: %1").arg(filepath));
        return ERR_FILE;
    }
    
    // Header schreiben (20 Bytes wie im Legacy)
    int16_t version = MAIN_VERSION;
    file.write(reinterpret_cast<const char*>(&version), sizeof(version));
    file.write(reinterpret_cast<const char*>(&auinit.sSelHaus), sizeof(auinit.sSelHaus));
    file.write(reinterpret_cast<const char*>(&auinit.sSelHoro), sizeof(auinit.sSelHoro));
    file.write(reinterpret_cast<const char*>(&auinit.sRotRadix), sizeof(auinit.sRotRadix));
    file.write(reinterpret_cast<const char*>(&auinit.sAspekte), sizeof(auinit.sAspekte));
    file.write(reinterpret_cast<const char*>(&auinit.sTeilung), sizeof(auinit.sTeilung));
    
    // Padding auf 20 Bytes
    QByteArray padding(20 - 12, '\0');
    file.write(padding);
    
    // Orben direkt in die gleiche Datei schreiben
    int sizePlanet = MAX_PLANET * MAX_PLANET * ASPEKTE;
    int sizeHaus = MAX_PLANET * MAX_HAUS * ASPEKTE;
    
    if (auinit.orbenPlanet.size() >= sizePlanet) {
        file.write(reinterpret_cast<const char*>(auinit.orbenPlanet.constData()), sizePlanet * sizeof(float));
    }
    if (auinit.orbenHaus.size() >= sizeHaus) {
        file.write(reinterpret_cast<const char*>(auinit.orbenHaus.constData()), sizeHaus * sizeof(float));
    }
    if (auinit.orbenTPlanet.size() >= sizePlanet) {
        file.write(reinterpret_cast<const char*>(auinit.orbenTPlanet.constData()), sizePlanet * sizeof(float));
    }
    if (auinit.orbenTHaus.size() >= sizeHaus) {
        file.write(reinterpret_cast<const char*>(auinit.orbenTHaus.constData()), sizeHaus * sizeof(float));
    }
    if (auinit.orbenSPlanet.size() >= sizePlanet) {
        file.write(reinterpret_cast<const char*>(auinit.orbenSPlanet.constData()), sizePlanet * sizeof(float));
    }
    if (auinit.orbenSHaus.size() >= sizeHaus) {
        file.write(reinterpret_cast<const char*>(auinit.orbenSHaus.constData()), sizeHaus * sizeof(float));
    }
    
    file.close();
    return ERR_OK;
}

//==============================================================================
// Orben-Datei
//==============================================================================

int LegacyIO::readOrben(AuInit& auinit) {
    QString filepath = getFilePath(INIDAT);
    QFile file(filepath);
    
    if (!file.open(QIODevice::ReadOnly)) {
        // Datei nicht vorhanden - Defaults verwenden
        auinit.initOrben();
        return ERR_OK;
    }
    
    // Orben-Größen wie im Legacy: MAX_PLANET * MAX_PLANET * ASPEKTE
    int sizePlanet = MAX_PLANET * MAX_PLANET * ASPEKTE;
    int sizeHaus = MAX_PLANET * MAX_HAUS * ASPEKTE;
    
    auinit.orbenPlanet.resize(sizePlanet);
    auinit.orbenHaus.resize(sizeHaus);
    auinit.orbenTPlanet.resize(sizePlanet);
    auinit.orbenTHaus.resize(sizeHaus);
    auinit.orbenSPlanet.resize(sizePlanet);
    auinit.orbenSHaus.resize(sizeHaus);
    
    // Suche nach Orben-Offset im Header
    // Legacy-Format: seek.lOrben zeigt auf Orben-Daten
    // Vereinfacht: Wir lesen direkt nach dem Header (Offset 20)
    file.seek(20);  // Nach Header + 5 shorts
    
    // Direkt binär lesen
    file.read(reinterpret_cast<char*>(auinit.orbenPlanet.data()), sizePlanet * sizeof(float));
    file.read(reinterpret_cast<char*>(auinit.orbenHaus.data()), sizeHaus * sizeof(float));
    file.read(reinterpret_cast<char*>(auinit.orbenTPlanet.data()), sizePlanet * sizeof(float));
    file.read(reinterpret_cast<char*>(auinit.orbenTHaus.data()), sizeHaus * sizeof(float));
    file.read(reinterpret_cast<char*>(auinit.orbenSPlanet.data()), sizePlanet * sizeof(float));
    file.read(reinterpret_cast<char*>(auinit.orbenSHaus.data()), sizeHaus * sizeof(float));
    
    file.close();
    
    // Prüfen ob Daten gültig sind (nicht alle 0)
    bool allZero = true;
    for (int i = 0; i < qMin(100, sizePlanet); ++i) {
        if (auinit.orbenPlanet[i] != 0.0f) {
            allZero = false;
            break;
        }
    }
    if (allZero) {
        auinit.initOrben();
    }
    
    return ERR_OK;
}

int LegacyIO::writeOrben(const AuInit& auinit) {
    QString filepath = getFilePath(INIDAT);
    QFile file(filepath);
    
    if (!file.open(QIODevice::ReadWrite)) {
        // Datei existiert nicht - neu erstellen
        if (!file.open(QIODevice::WriteOnly)) {
            setError(QString("Kann Datei nicht schreiben: %1").arg(filepath));
            return ERR_FILE;
        }
        // Header schreiben
        QByteArray header(20, '\0');
        file.write(header);
    } else {
        // Zur Orben-Position springen
        file.seek(20);
    }
    
    // Orben-Größen wie im Legacy
    int sizePlanet = MAX_PLANET * MAX_PLANET * ASPEKTE;
    int sizeHaus = MAX_PLANET * MAX_HAUS * ASPEKTE;
    
    // Direkt binär schreiben
    if (auinit.orbenPlanet.size() >= sizePlanet) {
        file.write(reinterpret_cast<const char*>(auinit.orbenPlanet.constData()), sizePlanet * sizeof(float));
    }
    if (auinit.orbenHaus.size() >= sizeHaus) {
        file.write(reinterpret_cast<const char*>(auinit.orbenHaus.constData()), sizeHaus * sizeof(float));
    }
    if (auinit.orbenTPlanet.size() >= sizePlanet) {
        file.write(reinterpret_cast<const char*>(auinit.orbenTPlanet.constData()), sizePlanet * sizeof(float));
    }
    if (auinit.orbenTHaus.size() >= sizeHaus) {
        file.write(reinterpret_cast<const char*>(auinit.orbenTHaus.constData()), sizeHaus * sizeof(float));
    }
    if (auinit.orbenSPlanet.size() >= sizePlanet) {
        file.write(reinterpret_cast<const char*>(auinit.orbenSPlanet.constData()), sizePlanet * sizeof(float));
    }
    if (auinit.orbenSHaus.size() >= sizeHaus) {
        file.write(reinterpret_cast<const char*>(auinit.orbenSHaus.constData()), sizeHaus * sizeof(float));
    }
    
    file.close();
    return ERR_OK;
}

//==============================================================================
// Default-Orben aus Textdatei (Port von sOrben in auhelper.c)
//==============================================================================

int LegacyIO::readDefaultOrben(AuInit& auinit) {
    QString filepath = getFilePath(ORBDAT);
    QFile file(filepath);
    
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        // Datei nicht vorhanden - Basis-Defaults verwenden
        auinit.initOrben();
        return ERR_OK;
    }
    
    // Arrays initialisieren mit Basis-Werten wie im Legacy
    int sizePlanet = MAX_PLANET * MAX_PLANET * ASPEKTE;
    int sizeHaus = MAX_PLANET * MAX_HAUS * ASPEKTE;
    
    auinit.orbenPlanet.resize(sizePlanet);
    auinit.orbenHaus.resize(sizeHaus);
    auinit.orbenTPlanet.resize(sizePlanet);
    auinit.orbenTHaus.resize(sizeHaus);
    auinit.orbenSPlanet.resize(sizePlanet);
    auinit.orbenSHaus.resize(sizeHaus);
    
    // Basis-Werte setzen (wie im Legacy sOrben)
    for (int i = 0; i < sizePlanet; ++i) {
        auinit.orbenPlanet[i] = 2.5f;
        auinit.orbenTPlanet[i] = 0.8f;
        auinit.orbenSPlanet[i] = 2.0f;
    }
    for (int i = 0; i < sizeHaus; ++i) {
        auinit.orbenHaus[i] = 0.2f;
        auinit.orbenTHaus[i] = 0.15f;
        auinit.orbenSHaus[i] = 0.15f;
    }
    
    // Aspekt-Namen für die Suche (wie szAspNamen im Legacy)
    static const char* aspNames[3 * ASPEKTE] = {
        // Radix
        "KONJUNKTION", "HALBSEXTIL", "SEXTIL", "QUADRAT", "TRIGON", "QUINCUNX", "OPPOSITION",
        // Transit
        "T_KONJUNKTION", "T_HALBSEXTIL", "T_SEXTIL", "T_QUADRAT", "T_TRIGON", "T_QUINCUNX", "T_OPPOSITION",
        // Synastrie
        "S_KONJUNKTION", "S_HALBSEXTIL", "S_SEXTIL", "S_QUADRAT", "S_TRIGON", "S_QUINCUNX", "S_OPPOSITION"
    };
    
    QTextStream stream(&file);
    QString content = stream.readAll();
    file.close();
    
    // Für jeden Aspekt-Typ (Radix, Transit, Synastrie) und jeden Aspekt
    for (int sA = 0; sA < 3 * ASPEKTE; ++sA) {
        QVector<float>* pfOP = nullptr;
        QVector<float>* pfOH = nullptr;
        
        if (sA < ASPEKTE) {
            pfOP = &auinit.orbenPlanet;
            pfOH = &auinit.orbenHaus;
        } else if (sA < 2 * ASPEKTE) {
            pfOP = &auinit.orbenTPlanet;
            pfOH = &auinit.orbenTHaus;
        } else {
            pfOP = &auinit.orbenSPlanet;
            pfOH = &auinit.orbenSHaus;
        }
        
        // Suche nach [ASPEKTNAME] Sektion
        QString sectionName = QString("[%1]").arg(aspNames[sA]);
        int sectionPos = content.indexOf(sectionName, 0, Qt::CaseInsensitive);
        if (sectionPos < 0) continue;
        
        // Lese die Zeilen nach der Sektion
        int lineStart = content.indexOf('\n', sectionPos) + 1;
        int sPlanetA = 0;
        
        while (sPlanetA < MAX_PLANET && lineStart < content.length()) {
            int lineEnd = content.indexOf('\n', lineStart);
            if (lineEnd < 0) lineEnd = content.length();
            
            QString line = content.mid(lineStart, lineEnd - lineStart).trimmed();
            lineStart = lineEnd + 1;
            
            // Kommentare und leere Zeilen überspringen
            if (line.isEmpty() || line.startsWith(';') || line.startsWith('[')) {
                if (line.startsWith('[')) break;  // Nächste Sektion
                continue;
            }
            
            // Zeile parsen: "SON  10  10  10 ..."
            QStringList parts = line.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
            if (parts.size() < 2) continue;
            
            // Erstes Element ist der Planet-Name, Rest sind Orben
            int sPlanetB = (sA < ASPEKTE) ? 1 : 0;  // Radix startet bei 1
            
            for (int i = 1; i < parts.size() && sPlanetB < MAX_PLANET + 5; ++i) {
                float fOrb = parts[i].toFloat();
                
                if (fOrb != -1.0f) {
                    if (sPlanetB < MAX_PLANET) {
                        // Planet-zu-Planet Orb
                        int idx = (sPlanetA * MAX_PLANET + sPlanetB) + (sA % ASPEKTE) * MAX_PLANET * MAX_PLANET;
                        if (idx < pfOP->size()) {
                            (*pfOP)[idx] = fOrb / 2.0f;  // Legacy teilt durch 2!
                        }
                    } else {
                        // Planet-zu-Haus Orb (ASC, IC, DSC, MC, HAU)
                        int hausIdx = -1;
                        switch (sPlanetB) {
                            case MAX_PLANET:     hausIdx = 0; break;  // ASC
                            case MAX_PLANET + 1: hausIdx = 3; break;  // IC
                            case MAX_PLANET + 2: hausIdx = 6; break;  // DSC
                            case MAX_PLANET + 3: hausIdx = 9; break;  // MC
                            case MAX_PLANET + 4:  // HAU - alle anderen Häuser
                                for (int h = 1; h < MAX_HAUS; ++h) {
                                    if (h != 3 && h != 6 && h != 9) {
                                        int hIdx = (sPlanetA * MAX_HAUS + h) + (sA % ASPEKTE) * MAX_PLANET * MAX_HAUS;
                                        if (hIdx < pfOH->size()) {
                                            (*pfOH)[hIdx] = fOrb / 2.0f;
                                        }
                                    }
                                }
                                break;
                        }
                        if (hausIdx >= 0) {
                            int hIdx = (sPlanetA * MAX_HAUS + hausIdx) + (sA % ASPEKTE) * MAX_PLANET * MAX_HAUS;
                            if (hIdx < pfOH->size()) {
                                (*pfOH)[hIdx] = fOrb / 2.0f;
                            }
                        }
                    }
                }
                sPlanetB++;
            }
            sPlanetA++;
        }
    }
    
    return ERR_OK;
}

//==============================================================================
// Farben
//==============================================================================

int LegacyIO::readColors(QColor* screenColors, QColor* printColors) {
    QString filepath = getFilePath(ORBDAT);
    QFile file(filepath);
    
    if (!file.open(QIODevice::ReadOnly)) {
        // Defaults verwenden
        initDefaultColors();
        return ERR_OK;
    }
    
    // Zur Farb-Position springen (nach Orben)
    int orbenSize = ASPEKTE * MAX_PLANET * 6 * sizeof(float);
    file.seek(orbenSize);
    
    QDataStream stream(&file);
    stream.setByteOrder(QDataStream::LittleEndian);
    
    // Bildschirm-Farben lesen
    for (int i = 0; i < COLORS; ++i) {
        uint32_t col;
        stream >> col;
        screenColors[i] = legacyToColor(col);
    }
    
    // Druck-Farben lesen
    for (int i = 0; i < COLORS; ++i) {
        uint32_t col;
        stream >> col;
        printColors[i] = legacyToColor(col);
    }
    
    file.close();
    return ERR_OK;
}

int LegacyIO::writeColors(const QColor* screenColors, const QColor* printColors) {
    QString filepath = getFilePath(ORBDAT);
    QFile file(filepath);
    
    if (!file.open(QIODevice::ReadWrite)) {
        setError(QString("Kann Datei nicht schreiben: %1").arg(filepath));
        return ERR_FILE;
    }
    
    // Zur Farb-Position springen
    int orbenSize = ASPEKTE * MAX_PLANET * 6 * sizeof(float);
    file.seek(orbenSize);
    
    QDataStream stream(&file);
    stream.setByteOrder(QDataStream::LittleEndian);
    
    // Bildschirm-Farben schreiben
    for (int i = 0; i < COLORS; ++i) {
        stream << colorToLegacy(screenColors[i]);
    }
    
    // Druck-Farben schreiben
    for (int i = 0; i < COLORS; ++i) {
        stream << colorToLegacy(printColors[i]);
    }
    
    file.close();
    return ERR_OK;
}

//==============================================================================
// Radix-Daten
//==============================================================================

int LegacyIO::readRadix(Radix& radix, const QString& name) {
    // TODO: Implementierung des Radix-Lesens aus der Datenbank
    // Dies erfordert das Lesen aus mehreren Dateien:
    // - astronam.dat (Namen)
    // - astrorad.dat (Radix-Daten)
    // - astronot.dat (Notizen)
    
    setError("readRadix noch nicht implementiert");
    return ERR_FILE;
}

int LegacyIO::writeRadix(const Radix& radix) {
    // TODO: Implementierung des Radix-Schreibens
    setError("writeRadix noch nicht implementiert");
    return ERR_FILE;
}

int LegacyIO::deleteRadix(const Radix& radix, int index) {
    // TODO: Implementierung des Radix-Löschens
    setError("deleteRadix noch nicht implementiert");
    return ERR_FILE;
}

//==============================================================================
// Globale Instanz
//==============================================================================

LegacyIO& legacyIO() {
    static LegacyIO instance;
    return instance;
}

} // namespace astro
