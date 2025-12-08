/**
 * @file person_db.cpp
 * @brief Implementierung der Personen-Datenbank
 */

#include "person_db.h"
#include "../core/constants.h"
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <algorithm>
#include <cstring>

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
    
    qDebug("PersonDB::load() - Lade von: %s", qPrintable(m_dataPath));
    qDebug("PersonDB::load() - Datei: %s", qPrintable(getNameFilePath()));
    
    QFile nameFile(getNameFilePath());
    if (!nameFile.open(QIODevice::ReadOnly)) {
        // Datei existiert nicht - das ist OK, wir starten mit leerer Datenbank
        m_loaded = true;  // Erlaubt speichern
        return 0;
    }
    
    // Legacy-Format: Variable Recordlänge!
    // Jeder Record besteht aus:
    // - RadixFixLegacy Struktur (73 Bytes)
    // - Namen mit variabler Länge (clName + clVorName + clBeruf + clOrt Bytes)
    // Reihenfolge der Namen: Name, Vorname, Beruf, Ort
    
    QByteArray allData = nameFile.readAll();
    nameFile.close();
    
    // Direkt Records lesen (kein Header)
    int pos = 0;
    
    while (pos + RADIXFIX_SIZE <= allData.size()) {
        RadixFixLegacy legacy;
        
        // Struktur kopieren (73 Bytes)
        std::memcpy(&legacy, allData.data() + pos, RADIXFIX_SIZE);
        pos += RADIXFIX_SIZE;
        
        // Berechne Gesamtlänge der Namen
        int nameLen = legacy.clName + legacy.clVorName + legacy.clBeruf + legacy.clOrt;
        
        // Sanity check für Namenslängen
        if (nameLen < 0 || nameLen > 1000 || pos + nameLen > allData.size()) {
            // Ungültige Daten - versuche nächste Position
            pos -= RADIXFIX_SIZE - 1;  // Zurück und 1 Byte weiter
            continue;
        }
        
        // Nur gültige Records übernehmen (cGultig != 0)
        if (legacy.cGultig != 0) {
            RadixFix rf;
            rf.gultig = true;
            rf.indexRadix = legacy.lIndexRadix;
            rf.indexName = legacy.lIndexName;
            rf.indexNotiz = legacy.lIndexNotiz;
            rf.sommerzeit = legacy.fSommer;
            rf.zone = legacy.fZone;
            rf.laenge = legacy.dLange;
            rf.breite = legacy.dBreite;
            rf.zeit = legacy.dTime;
            rf.tag = legacy.sTag;
            rf.monat = legacy.sMonat;
            rf.jahr = legacy.sJahr;
            // Ländercode: bis Nullterminator, max 3 Zeichen (Legacy char[4])
            int landLen = 0;
            for (int i = 0; i < 4 && legacy.szLand[i] != '\0'; ++i) {
                landLen++;
            }
            rf.land = QString::fromLatin1(legacy.szLand, landLen).trimmed();
            
            // Namen lesen (Reihenfolge: Name, Vorname, Beruf, Ort)
            int namePos = pos;
            if (legacy.clName > 0 && legacy.clName < 100) {
                rf.name = QString::fromLatin1(allData.data() + namePos, legacy.clName);
                namePos += legacy.clName;
            }
            if (legacy.clVorName > 0 && legacy.clVorName < 100) {
                rf.vorname = QString::fromLatin1(allData.data() + namePos, legacy.clVorName);
                namePos += legacy.clVorName;
            }
            if (legacy.clBeruf > 0 && legacy.clBeruf < 100) {
                rf.beruf = QString::fromLatin1(allData.data() + namePos, legacy.clBeruf);
                namePos += legacy.clBeruf;
            }
            if (legacy.clOrt > 0 && legacy.clOrt < 100) {
                rf.ort = QString::fromLatin1(allData.data() + namePos, legacy.clOrt);
            }
            
            m_personen.append(rf);
        }
        
        // Zur nächsten Position springen
        pos += nameLen;
    }
    
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
    const RadixFix& rf = radix.rFix;
    
    // Duplikat-Prüfung: Name, Vorname und Geburtsdatum
    for (int i = 0; i < m_personen.size(); ++i) {
        const RadixFix& existing = m_personen[i];
        if (existing.name == rf.name &&
            existing.vorname == rf.vorname &&
            existing.tag == rf.tag &&
            existing.monat == rf.monat &&
            existing.jahr == rf.jahr) {
            // Person existiert bereits - aktualisieren statt hinzufügen
            qDebug("PersonDB::add() - Person existiert bereits, aktualisiere Index %d", i);
            m_personen[i] = rf;
            m_personen[i].gultig = true;
            m_personen[i].indexRadix = i;
            m_modified = true;
            return i;
        }
    }
    
    // Neue Person hinzufügen
    RadixFix newRf = rf;
    newRf.gultig = true;
    newRf.indexRadix = m_personen.size();
    
    m_personen.append(newRf);
    m_modified = true;
    
    qDebug("PersonDB::add() - Person hinzugefügt: %s %s, Index: %d, Gesamt: %lld",
           qPrintable(newRf.vorname), qPrintable(newRf.name), 
           newRf.indexRadix, static_cast<long long>(m_personen.size()));
    
    return newRf.indexRadix;
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
    // Auch speichern wenn Datenbank leer war oder neu erstellt wird
    if (m_dataPath.isEmpty()) {
        setError("Kein Datenpfad gesetzt");
        qWarning("PersonDB::save() - Kein Datenpfad gesetzt!");
        return false;
    }
    
    qDebug("PersonDB::save() - Speichere %lld Personen nach %s",
           static_cast<long long>(m_personen.size()), qPrintable(getNameFilePath()));
    
    QFile nameFile(getNameFilePath());
    if (!nameFile.open(QIODevice::WriteOnly)) {
        setError(QString("Kann Datei nicht schreiben: %1").arg(getNameFilePath()));
        qWarning("PersonDB::save() - Kann Datei nicht öffnen: %s", qPrintable(getNameFilePath()));
        return false;
    }
    
    // STRICT LEGACY: Format wie im Original
    // Kein Header, direkt Records
    // Jeder Record: RadixFixLegacy (73 Bytes) + Namen (variable Länge)
    // Reihenfolge Namen: Name, Vorname, Beruf, Ort
    
    for (const RadixFix& rf : m_personen) {
        RadixFixLegacy legacy = rf.toLegacy();
        
        // Namen als Latin1 kodieren
        QByteArray nameBytes = rf.name.toLatin1();
        QByteArray vornameBytes = rf.vorname.toLatin1();
        QByteArray berufBytes = rf.beruf.toLatin1();
        QByteArray ortBytes = rf.ort.toLatin1();
        
        // Längen in Legacy-Struktur setzen
        legacy.clName = nameBytes.size();
        legacy.clVorName = vornameBytes.size();
        legacy.clBeruf = berufBytes.size();
        legacy.clOrt = ortBytes.size();
        
        // Struktur schreiben (73 Bytes)
        nameFile.write(reinterpret_cast<const char*>(&legacy), RADIXFIX_SIZE);
        
        // Namen schreiben (variable Länge, Reihenfolge: Name, Vorname, Beruf, Ort)
        nameFile.write(nameBytes);
        nameFile.write(vornameBytes);
        nameFile.write(berufBytes);
        nameFile.write(ortBytes);
        
        qDebug("PersonDB::save() - Record: %s %s, Längen: %d/%d/%d/%d",
               qPrintable(rf.name), qPrintable(rf.vorname),
               legacy.clName, legacy.clVorName, legacy.clBeruf, legacy.clOrt);
    }
    
    nameFile.close();
    m_modified = false;
    
    qDebug("PersonDB::save() - Erfolgreich gespeichert: %lld Bytes", 
           QFileInfo(getNameFilePath()).size());
    
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
