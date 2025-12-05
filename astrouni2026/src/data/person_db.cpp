/**
 * @file person_db.cpp
 * @brief Implementierung der Personen-Datenbank
 */

#include "person_db.h"
#include "../core/constants.h"
#include <QFile>
#include <QDataStream>
#include <QDir>
#include <algorithm>

namespace astro {

//==============================================================================
// Konstruktor / Destruktor
//==============================================================================

PersonDB::PersonDB()
    : m_loaded(false)
    , m_modified(false) {
}

PersonDB::~PersonDB() {
    if (m_modified && m_loaded) {
        save();
    }
}

//==============================================================================
// Pfade
//==============================================================================

QString PersonDB::getNameFilePath() const {
    return m_dataPath + "/" + NAMDAT;
}

QString PersonDB::getRadixFilePath() const {
    return m_dataPath + "/" + RADDAT;
}

QString PersonDB::getNotizFilePath() const {
    return m_dataPath + "/" + NOTDAT;
}

//==============================================================================
// Datenbank laden
//==============================================================================

int PersonDB::load(const QString& dataPath) {
    m_personen.clear();
    m_loaded = false;
    m_modified = false;
    m_dataPath = dataPath;
    
    QFile nameFile(getNameFilePath());
    if (!nameFile.open(QIODevice::ReadOnly)) {
        setError(QString("Kann Datei nicht öffnen: %1").arg(getNameFilePath()));
        return -1;
    }
    
    QDataStream stream(&nameFile);
    stream.setByteOrder(QDataStream::LittleEndian);
    
    // Namen-Datei lesen
    while (!stream.atEnd()) {
        RadixFixLegacy legacy;
        
        // Legacy-Struktur lesen
        stream >> legacy.cGultig;
        stream >> legacy.lIndexRadix;
        stream >> legacy.slRadix;
        stream >> legacy.lIndexName;
        stream >> legacy.slName;
        stream >> legacy.lIndexNotiz;
        stream >> legacy.slNotiz;
        stream >> legacy.clVorName;
        stream >> legacy.clName;
        stream >> legacy.clBeruf;
        stream >> legacy.clOrt;
        stream >> legacy.fSommer;
        stream >> legacy.fZone;
        stream.readRawData(legacy.szLand, 4);
        stream >> legacy.dLange;
        stream >> legacy.dBreite;
        stream >> legacy.dTime;
        stream >> legacy.sTag;
        stream >> legacy.sMonat;
        stream >> legacy.sJahr;
        
        // Namen lesen (direkt nach der Struktur)
        QByteArray vorname(legacy.clVorName, '\0');
        QByteArray name(legacy.clName, '\0');
        QByteArray beruf(legacy.clBeruf, '\0');
        QByteArray ort(legacy.clOrt, '\0');
        
        stream.readRawData(vorname.data(), legacy.clVorName);
        stream.readRawData(name.data(), legacy.clName);
        stream.readRawData(beruf.data(), legacy.clBeruf);
        stream.readRawData(ort.data(), legacy.clOrt);
        
        // Nur gültige Records übernehmen
        if (legacy.cGultig) {
            RadixFix rf = RadixFix::fromLegacy(legacy);
            rf.vorname = QString::fromLatin1(vorname).trimmed();
            rf.name = QString::fromLatin1(name).trimmed();
            rf.beruf = QString::fromLatin1(beruf).trimmed();
            rf.ort = QString::fromLatin1(ort).trimmed();
            
            m_personen.append(rf);
        }
    }
    
    nameFile.close();
    m_loaded = true;
    
    return m_personen.size();
}

int PersonDB::count() const {
    return m_personen.size();
}

bool PersonDB::isLoaded() const {
    return m_loaded;
}

//==============================================================================
// Suche
//==============================================================================

QVector<RadixFix> PersonDB::search(const QString& suchtext, int maxResults) const {
    QVector<RadixFix> results;
    
    if (!m_loaded || suchtext.isEmpty()) {
        return results;
    }
    
    QString searchLower = suchtext.toLower();
    
    for (const RadixFix& person : m_personen) {
        if (!person.gultig) continue;
        
        if (person.name.toLower().startsWith(searchLower) ||
            person.vorname.toLower().startsWith(searchLower)) {
            results.append(person);
            if (results.size() >= maxResults) {
                break;
            }
        }
    }
    
    // Nach Namen sortieren
    std::sort(results.begin(), results.end(),
              [](const RadixFix& a, const RadixFix& b) {
                  if (a.name.toLower() != b.name.toLower()) {
                      return a.name.toLower() < b.name.toLower();
                  }
                  return a.vorname.toLower() < b.vorname.toLower();
              });
    
    return results;
}

QVector<RadixFix> PersonDB::getAll() const {
    QVector<RadixFix> results;
    
    for (const RadixFix& person : m_personen) {
        if (person.gultig) {
            results.append(person);
        }
    }
    
    // Nach Namen sortieren
    std::sort(results.begin(), results.end(),
              [](const RadixFix& a, const RadixFix& b) {
                  if (a.name.toLower() != b.name.toLower()) {
                      return a.name.toLower() < b.name.toLower();
                  }
                  return a.vorname.toLower() < b.vorname.toLower();
              });
    
    return results;
}

bool PersonDB::loadRadix(int index, Radix& radix) const {
    if (index < 0 || index >= m_personen.size()) {
        return false;
    }
    
    const RadixFix& rf = m_personen[index];
    radix.rFix = rf;
    
    // Notiz laden (falls vorhanden)
    if (rf.indexNotiz > 0) {
        QFile notizFile(getNotizFilePath());
        if (notizFile.open(QIODevice::ReadOnly)) {
            notizFile.seek(rf.indexNotiz);
            QByteArray notiz = notizFile.read(1024);  // Max 1KB Notiz
            radix.rFix.notiz = QString::fromLatin1(notiz).trimmed();
            notizFile.close();
        }
    }
    
    return true;
}

//==============================================================================
// Bearbeiten
//==============================================================================

int PersonDB::add(const Radix& radix) {
    RadixFix rf = radix.rFix;
    rf.gultig = true;
    rf.indexRadix = m_personen.size();
    
    m_personen.append(rf);
    m_modified = true;
    
    return rf.indexRadix;
}

bool PersonDB::update(int index, const Radix& radix) {
    if (index < 0 || index >= m_personen.size()) {
        return false;
    }
    
    m_personen[index] = radix.rFix;
    m_personen[index].indexRadix = index;
    m_modified = true;
    
    return true;
}

bool PersonDB::remove(int index) {
    if (index < 0 || index >= m_personen.size()) {
        return false;
    }
    
    m_personen[index].gultig = false;
    m_modified = true;
    
    return true;
}

//==============================================================================
// Duplikat-Prüfung
//==============================================================================

int PersonDB::findDuplicate(const QString& name, const QString& vorname, 
                            const QDate& datum) const {
    QString nameLower = name.toLower();
    QString vornameLower = vorname.toLower();
    
    for (int i = 0; i < m_personen.size(); ++i) {
        const RadixFix& rf = m_personen[i];
        if (!rf.gultig) continue;
        
        if (rf.name.toLower() == nameLower &&
            rf.vorname.toLower() == vornameLower &&
            rf.tag == datum.day() &&
            rf.monat == datum.month() &&
            rf.jahr == datum.year()) {
            return i;
        }
    }
    
    return -1;
}

//==============================================================================
// Speichern
//==============================================================================

bool PersonDB::save() {
    if (!m_loaded) {
        setError("Datenbank nicht geladen");
        return false;
    }
    
    QFile nameFile(getNameFilePath());
    if (!nameFile.open(QIODevice::WriteOnly)) {
        setError(QString("Kann Datei nicht schreiben: %1").arg(getNameFilePath()));
        return false;
    }
    
    QDataStream stream(&nameFile);
    stream.setByteOrder(QDataStream::LittleEndian);
    
    for (const RadixFix& rf : m_personen) {
        RadixFixLegacy legacy = rf.toLegacy();
        
        // Legacy-Struktur schreiben
        stream << legacy.cGultig;
        stream << legacy.lIndexRadix;
        stream << legacy.slRadix;
        stream << legacy.lIndexName;
        stream << legacy.slName;
        stream << legacy.lIndexNotiz;
        stream << legacy.slNotiz;
        stream << legacy.clVorName;
        stream << legacy.clName;
        stream << legacy.clBeruf;
        stream << legacy.clOrt;
        stream << legacy.fSommer;
        stream << legacy.fZone;
        stream.writeRawData(legacy.szLand, 4);
        stream << legacy.dLange;
        stream << legacy.dBreite;
        stream << legacy.dTime;
        stream << legacy.sTag;
        stream << legacy.sMonat;
        stream << legacy.sJahr;
        
        // Namen schreiben
        QByteArray vorname = rf.vorname.toLatin1();
        QByteArray name = rf.name.toLatin1();
        QByteArray beruf = rf.beruf.toLatin1();
        QByteArray ort = rf.ort.toLatin1();
        
        stream.writeRawData(vorname.constData(), vorname.size());
        stream.writeRawData(name.constData(), name.size());
        stream.writeRawData(beruf.constData(), beruf.size());
        stream.writeRawData(ort.constData(), ort.size());
    }
    
    nameFile.close();
    m_modified = false;
    
    return true;
}

//==============================================================================
// Fehlerbehandlung
//==============================================================================

QString PersonDB::getLastError() const {
    return m_lastError;
}

void PersonDB::setError(const QString& error) {
    m_lastError = error;
}

//==============================================================================
// Globale Instanz
//==============================================================================

PersonDB& personDB() {
    static PersonDB instance;
    return instance;
}

} // namespace astro
