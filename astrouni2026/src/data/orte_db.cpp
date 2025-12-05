/**
 * @file orte_db.cpp
 * @brief Implementierung der Orte-Datenbank
 */

#include "orte_db.h"
#include <QFile>
#include <QDataStream>
#include <algorithm>

namespace astro {

//==============================================================================
// Konstruktor / Destruktor
//==============================================================================

OrteDB::OrteDB()
    : m_loaded(false)
    , m_modified(false) {
}

OrteDB::~OrteDB() {
    if (m_modified && m_loaded) {
        save();
    }
}

//==============================================================================
// Datenbank laden
//==============================================================================

int OrteDB::load(const QString& filepath) {
    m_orte.clear();
    m_loaded = false;
    m_modified = false;
    m_filepath = filepath;
    
    QFile file(filepath);
    if (!file.open(QIODevice::ReadOnly)) {
        setError(QString("Kann Datei nicht öffnen: %1").arg(filepath));
        return -1;
    }
    
    QDataStream stream(&file);
    stream.setByteOrder(QDataStream::LittleEndian);
    
    // Legacy-Format: Jeder Record hat feste Größe
    // OrteLegacy Struktur + Ortsname (variable Länge)
    
    while (!stream.atEnd()) {
        OrteLegacy legacy;
        
        // Legacy-Struktur lesen
        stream >> legacy.cGultig;
        stream >> legacy.lIndex;
        stream >> legacy.slRec;
        stream >> legacy.slOrt;
        stream.readRawData(legacy.szLand, 4);
        stream >> legacy.fZone;
        stream >> legacy.dBreite;
        stream >> legacy.dLange;
        
        // Ortsname lesen
        QByteArray nameBytes(legacy.slOrt, '\0');
        stream.readRawData(nameBytes.data(), legacy.slOrt);
        
        // In Latin1 kodiert (ISO-8859-1)
        QString ortName = QString::fromLatin1(nameBytes).trimmed();
        
        // Nur gültige Records übernehmen
        if (legacy.cGultig) {
            Orte ort = Orte::fromLegacy(legacy, ortName);
            ort.index = m_orte.size();
            m_orte.append(ort);
        }
    }
    
    file.close();
    m_loaded = true;
    
    return m_orte.size();
}

int OrteDB::count() const {
    return m_orte.size();
}

bool OrteDB::isLoaded() const {
    return m_loaded;
}

//==============================================================================
// Suche
//==============================================================================

QVector<Orte> OrteDB::search(const QString& suchtext, int maxResults) const {
    QVector<Orte> results;
    
    if (!m_loaded || suchtext.isEmpty()) {
        return results;
    }
    
    QString searchLower = suchtext.toLower();
    
    for (const Orte& ort : m_orte) {
        if (ort.name.toLower().startsWith(searchLower)) {
            results.append(ort);
            if (results.size() >= maxResults) {
                break;
            }
        }
    }
    
    // Nach Namen sortieren
    std::sort(results.begin(), results.end(),
              [](const Orte& a, const Orte& b) {
                  return a.name.toLower() < b.name.toLower();
              });
    
    return results;
}

Orte OrteDB::getByIndex(int index) const {
    if (index >= 0 && index < m_orte.size()) {
        return m_orte[index];
    }
    return Orte();
}

Orte OrteDB::getByName(const QString& name) const {
    QString nameLower = name.toLower();
    
    for (const Orte& ort : m_orte) {
        if (ort.name.toLower() == nameLower) {
            return ort;
        }
    }
    return Orte();
}

//==============================================================================
// Bearbeiten
//==============================================================================

int OrteDB::add(const Orte& ort) {
    Orte newOrt = ort;
    newOrt.index = m_orte.size();
    newOrt.gultig = true;
    
    m_orte.append(newOrt);
    m_modified = true;
    
    return newOrt.index;
}

bool OrteDB::update(int index, const Orte& ort) {
    if (index < 0 || index >= m_orte.size()) {
        return false;
    }
    
    m_orte[index] = ort;
    m_orte[index].index = index;
    m_modified = true;
    
    return true;
}

bool OrteDB::remove(int index) {
    if (index < 0 || index >= m_orte.size()) {
        return false;
    }
    
    // Nicht wirklich löschen, nur als ungültig markieren
    m_orte[index].gultig = false;
    m_modified = true;
    
    return true;
}

//==============================================================================
// Speichern
//==============================================================================

bool OrteDB::save(const QString& filepath) {
    QString savePath = filepath.isEmpty() ? m_filepath : filepath;
    
    if (savePath.isEmpty()) {
        setError("Kein Dateipfad angegeben");
        return false;
    }
    
    QFile file(savePath);
    if (!file.open(QIODevice::WriteOnly)) {
        setError(QString("Kann Datei nicht schreiben: %1").arg(savePath));
        return false;
    }
    
    QDataStream stream(&file);
    stream.setByteOrder(QDataStream::LittleEndian);
    
    for (const Orte& ort : m_orte) {
        OrteLegacy legacy = ort.toLegacy();
        QByteArray nameBytes = ort.name.toLatin1();
        legacy.slOrt = nameBytes.size();
        
        // Legacy-Struktur schreiben
        stream << legacy.cGultig;
        stream << legacy.lIndex;
        stream << legacy.slRec;
        stream << legacy.slOrt;
        stream.writeRawData(legacy.szLand, 4);
        stream << legacy.fZone;
        stream << legacy.dBreite;
        stream << legacy.dLange;
        
        // Ortsname schreiben
        stream.writeRawData(nameBytes.constData(), nameBytes.size());
    }
    
    file.close();
    m_modified = false;
    
    return true;
}

//==============================================================================
// Fehlerbehandlung
//==============================================================================

QString OrteDB::getLastError() const {
    return m_lastError;
}

void OrteDB::setError(const QString& error) {
    m_lastError = error;
}

//==============================================================================
// Globale Instanz
//==============================================================================

OrteDB& orteDB() {
    static OrteDB instance;
    return instance;
}

} // namespace astro
