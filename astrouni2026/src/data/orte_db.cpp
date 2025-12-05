/**
 * @file orte_db.cpp
 * @brief Implementierung der Orte-Datenbank
 */

#include "orte_db.h"
#include <QFile>
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
    
    // Direkt binär lesen (nicht QDataStream, da das Probleme mit packed structs hat)
    while (!file.atEnd()) {
        OrteLegacy legacy;
        
        // Struktur direkt lesen
        if (file.read(reinterpret_cast<char*>(&legacy), sizeof(OrteLegacy)) != sizeof(OrteLegacy)) {
            break;
        }
        
        // Ortsname lesen (slOrt enthält die Länge)
        if (legacy.slOrt > 0 && legacy.slOrt < 256) {
            QByteArray nameBytes = file.read(legacy.slOrt);
            if (nameBytes.size() != legacy.slOrt) {
                break;
            }
            
            // In Latin1 kodiert (ISO-8859-1)
            QString ortName = QString::fromLatin1(nameBytes).trimmed();
            
            // Nur gültige Records übernehmen
            if (legacy.cGultig) {
                Orte ort = Orte::fromLegacy(legacy, ortName);
                ort.index = m_orte.size();
                m_orte.append(ort);
            }
        } else {
            // Ungültiger Record - überspringen
            break;
        }
    }
    
    file.close();
    m_loaded = true;
    
    return m_orte.size();
}

int OrteDB::loadAll(const QString& dataPath) {
    m_orte.clear();
    m_loaded = false;
    m_modified = false;
    
    int totalLoaded = 0;
    
    // Lade alle Orte-Dateien
    QStringList files = {"astroorg.dat", "astroger.dat", "europa.dat"};
    
    for (const QString& filename : files) {
        QString filepath = dataPath + "/" + filename;
        QFile file(filepath);
        
        if (!file.open(QIODevice::ReadOnly)) {
            continue;  // Datei nicht vorhanden, überspringen
        }
        
        // Direkt binär lesen
        while (!file.atEnd()) {
            OrteLegacy legacy;
            
            if (file.read(reinterpret_cast<char*>(&legacy), sizeof(OrteLegacy)) != sizeof(OrteLegacy)) {
                break;
            }
            
            if (legacy.slOrt > 0 && legacy.slOrt < 256) {
                QByteArray nameBytes = file.read(legacy.slOrt);
                if (nameBytes.size() != legacy.slOrt) {
                    break;
                }
                
                QString ortName = QString::fromLatin1(nameBytes).trimmed();
                
                if (legacy.cGultig && !ortName.isEmpty()) {
                    Orte ort = Orte::fromLegacy(legacy, ortName);
                    ort.index = m_orte.size();
                    m_orte.append(ort);
                    totalLoaded++;
                }
            } else {
                break;
            }
        }
        
        file.close();
    }
    
    m_loaded = (totalLoaded > 0);
    m_filepath = dataPath + "/astroorg.dat";  // Hauptdatei für Speichern
    
    return totalLoaded;
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
    
    for (const Orte& ort : m_orte) {
        OrteLegacy legacy = ort.toLegacy();
        QByteArray nameBytes = ort.name.toLatin1();
        legacy.slOrt = nameBytes.size();
        
        // Struktur direkt schreiben
        file.write(reinterpret_cast<const char*>(&legacy), sizeof(OrteLegacy));
        
        // Ortsname schreiben
        file.write(nameBytes);
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
