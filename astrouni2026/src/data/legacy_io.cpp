/**
 * @file legacy_io.cpp
 * @brief Implementierung der Legacy-Datei I/O
 */

#include "legacy_io.h"
#include <QFile>
#include <QDataStream>
#include <QDir>

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
        setError(QString("Kann Datei nicht öffnen: %1").arg(filepath));
        return ERR_FILE;
    }
    
    QDataStream stream(&file);
    stream.setByteOrder(QDataStream::LittleEndian);
    
    // Header lesen
    int16_t version;
    stream >> version;
    
    if (version != MAIN_VERSION) {
        // Alte Version - mit Defaults initialisieren
        auinit = AuInit();
        file.close();
        return ERR_OK;
    }
    
    // Einstellungen lesen
    stream >> auinit.sSelHaus;
    stream >> auinit.sSelHoro;
    stream >> auinit.sRotRadix;
    stream >> auinit.sAspekte;
    stream >> auinit.sTeilung;
    
    file.close();
    
    // Orben separat lesen
    readOrben(auinit);
    
    return ERR_OK;
}

int LegacyIO::writeIni(const AuInit& auinit) {
    QString filepath = getFilePath(INIDAT);
    QFile file(filepath);
    
    if (!file.open(QIODevice::WriteOnly)) {
        setError(QString("Kann Datei nicht schreiben: %1").arg(filepath));
        return ERR_FILE;
    }
    
    QDataStream stream(&file);
    stream.setByteOrder(QDataStream::LittleEndian);
    
    // Header schreiben
    int16_t version = MAIN_VERSION;
    stream << version;
    
    // Einstellungen schreiben
    stream << auinit.sSelHaus;
    stream << auinit.sSelHoro;
    stream << auinit.sRotRadix;
    stream << auinit.sAspekte;
    stream << auinit.sTeilung;
    
    file.close();
    
    // Orben separat schreiben
    writeOrben(auinit);
    
    return ERR_OK;
}

//==============================================================================
// Orben-Datei
//==============================================================================

int LegacyIO::readOrben(AuInit& auinit) {
    QString filepath = getFilePath(ORBDAT);
    QFile file(filepath);
    
    if (!file.open(QIODevice::ReadOnly)) {
        // Datei nicht vorhanden - Defaults verwenden
        auinit.initOrben();
        return ERR_OK;
    }
    
    QDataStream stream(&file);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream.setFloatingPointPrecision(QDataStream::SinglePrecision);
    
    // Orben lesen
    int size = ASPEKTE * MAX_PLANET;
    
    auinit.orbenPlanet.resize(size);
    auinit.orbenHaus.resize(size);
    auinit.orbenTPlanet.resize(size);
    auinit.orbenTHaus.resize(size);
    auinit.orbenSPlanet.resize(size);
    auinit.orbenSHaus.resize(size);
    
    for (int i = 0; i < size; ++i) {
        stream >> auinit.orbenPlanet[i];
    }
    for (int i = 0; i < size; ++i) {
        stream >> auinit.orbenHaus[i];
    }
    for (int i = 0; i < size; ++i) {
        stream >> auinit.orbenTPlanet[i];
    }
    for (int i = 0; i < size; ++i) {
        stream >> auinit.orbenTHaus[i];
    }
    for (int i = 0; i < size; ++i) {
        stream >> auinit.orbenSPlanet[i];
    }
    for (int i = 0; i < size; ++i) {
        stream >> auinit.orbenSHaus[i];
    }
    
    file.close();
    return ERR_OK;
}

int LegacyIO::writeOrben(const AuInit& auinit) {
    QString filepath = getFilePath(ORBDAT);
    QFile file(filepath);
    
    if (!file.open(QIODevice::WriteOnly)) {
        setError(QString("Kann Datei nicht schreiben: %1").arg(filepath));
        return ERR_FILE;
    }
    
    QDataStream stream(&file);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream.setFloatingPointPrecision(QDataStream::SinglePrecision);
    
    int size = ASPEKTE * MAX_PLANET;
    
    for (int i = 0; i < size; ++i) {
        stream << auinit.orbenPlanet[i];
    }
    for (int i = 0; i < size; ++i) {
        stream << auinit.orbenHaus[i];
    }
    for (int i = 0; i < size; ++i) {
        stream << auinit.orbenTPlanet[i];
    }
    for (int i = 0; i < size; ++i) {
        stream << auinit.orbenTHaus[i];
    }
    for (int i = 0; i < size; ++i) {
        stream << auinit.orbenSPlanet[i];
    }
    for (int i = 0; i < size; ++i) {
        stream << auinit.orbenSHaus[i];
    }
    
    file.close();
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
