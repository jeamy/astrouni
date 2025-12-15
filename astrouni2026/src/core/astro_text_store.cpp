#include "astro_text_store.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QLocale>
#include <QTextStream>
#include <QStringConverter>

namespace astro {

AstroTextStore::AstroTextStore()
    : m_loaded(false) {
}

void AstroTextStore::setFilePath(const QString& path) {
    m_filePath = path;
}

QString AstroTextStore::filePath() const {
    if (!m_filePath.isEmpty()) {
        return m_filePath;
    }

    return QCoreApplication::applicationDirPath() + "/data/astrotext.dat";
}

bool AstroTextStore::ensureLoaded() {
    if (m_loaded) {
        return true;
    }
    return load();
}

QString AstroTextStore::dirtyId(const QString& lang, const QString& key) const {
    return lang + "|" + key;
}

QString AstroTextStore::escapeValue(const QString& value) {
    QString out;
    out.reserve(value.size());

    for (QChar ch : value) {
        if (ch == '\\') {
            out += "\\\\";
        } else if (ch == '\n') {
            out += "\\n";
        } else if (ch == '\r') {
            out += "\\r";
        } else if (ch == '\t') {
            out += "\\t";
        } else {
            out += ch;
        }
    }

    return out;
}

QString AstroTextStore::unescapeValue(const QString& value) {
    QString out;
    out.reserve(value.size());

    for (int i = 0; i < value.size(); ++i) {
        QChar ch = value[i];
        if (ch != '\\' || i + 1 >= value.size()) {
            out += ch;
            continue;
        }

        QChar n = value[i + 1];
        if (n == 'n') {
            out += '\n';
            ++i;
        } else if (n == 'r') {
            out += '\r';
            ++i;
        } else if (n == 't') {
            out += '\t';
            ++i;
        } else if (n == '\\') {
            out += '\\';
            ++i;
        } else {
            out += ch;
        }
    }

    return out;
}

bool AstroTextStore::load() {
    m_overrides.clear();
    m_snapshot.clear();
    m_dirty.clear();

    const QString path = filePath();
    QFile f(path);
    if (!f.exists()) {
        m_loaded = true;
        return true;
    }

    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        m_loaded = true;
        return false;
    }

    QTextStream in(&f);
    in.setEncoding(QStringConverter::Utf8);

    QString currentLang = "de";
    while (!in.atEnd()) {
        QString line = in.readLine();
        const QString trimmed = line.trimmed();

        if (trimmed.isEmpty()) {
            continue;
        }
        if (trimmed.startsWith('#') || trimmed.startsWith(';')) {
            continue;
        }

        if (trimmed.startsWith('[') && trimmed.endsWith(']')) {
            currentLang = trimmed.mid(1, trimmed.size() - 2).trimmed();
            if (currentLang.isEmpty()) {
                currentLang = "de";
            }
            continue;
        }

        const int eq = trimmed.indexOf('=');
        if (eq <= 0) {
            continue;
        }

        const QString key = trimmed.left(eq).trimmed();
        const QString valueEsc = trimmed.mid(eq + 1);
        const QString value = unescapeValue(valueEsc);

        m_overrides[currentLang][key] = value;
        m_snapshot[currentLang][key] = value;
    }

    m_loaded = true;
    return true;
}

bool AstroTextStore::save() {
    const QString path = filePath();

    QDir dir = QFileInfo(path).dir();
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    const QString tmpPath = path + ".tmp";

    QFile f(tmpPath);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }

    QTextStream out(&f);
    out.setEncoding(QStringConverter::Utf8);

    const QStringList langs = m_overrides.keys();
    for (int li = 0; li < langs.size(); ++li) {
        const QString& lang = langs[li];
        out << "[" << lang << "]\n";

        const auto& map = m_overrides[lang];
        const QStringList keys = map.keys();
        for (const QString& key : keys) {
            out << key << "=" << escapeValue(map.value(key)) << "\n";
        }

        if (li + 1 < langs.size()) {
            out << "\n";
        }
    }

    f.close();

    QFile::remove(path);

    const bool ok = QFile::rename(tmpPath, path);
    if (ok) {
        m_snapshot = m_overrides;
        m_dirty.clear();
    }
    return ok;
}

bool AstroTextStore::hasOverride(const QString& lang, const QString& key) const {
    auto itLang = m_overrides.constFind(lang);
    if (itLang == m_overrides.constEnd()) {
        return false;
    }
    return itLang->contains(key);
}

QString AstroTextStore::overrideText(const QString& lang, const QString& key) const {
    auto itLang = m_overrides.constFind(lang);
    if (itLang == m_overrides.constEnd()) {
        return QString();
    }
    return itLang->value(key);
}

QString AstroTextStore::text(const QString& lang, const QString& key, const QString& defaultValue) const {
    auto itLang = m_overrides.constFind(lang);
    if (itLang != m_overrides.constEnd() && itLang->contains(key)) {
        return itLang->value(key);
    }

    auto itDe = m_overrides.constFind("de");
    if (itDe != m_overrides.constEnd() && itDe->contains(key)) {
        return itDe->value(key);
    }

    return defaultValue;
}

void AstroTextStore::setOverride(const QString& lang, const QString& key, const QString& value) {
    ensureLoaded();

    m_overrides[lang][key] = value;

    QString snap = m_snapshot.value(lang).value(key);
    if (snap != value) {
        m_dirty.insert(dirtyId(lang, key));
    } else {
        m_dirty.remove(dirtyId(lang, key));
    }
}

void AstroTextStore::revertOverride(const QString& lang, const QString& key) {
    ensureLoaded();

    if (m_snapshot.value(lang).contains(key)) {
        m_overrides[lang][key] = m_snapshot.value(lang).value(key);
    } else {
        m_overrides[lang].remove(key);
    }

    m_dirty.remove(dirtyId(lang, key));
}

void AstroTextStore::revertAll() {
    ensureLoaded();

    m_overrides = m_snapshot;
    m_dirty.clear();
}

bool AstroTextStore::isDirty() const {
    return !m_dirty.isEmpty();
}

bool AstroTextStore::isDirty(const QString& lang, const QString& key) const {
    return m_dirty.contains(dirtyId(lang, key));
}

QString AstroTextStore::systemLanguageCode() {
    QString name = QLocale::system().name();
    const int idx = name.indexOf('_');
    if (idx > 0) {
        name = name.left(idx);
    }
    if (name.isEmpty()) {
        name = "de";
    }
    return name.toLower();
}

AstroTextStore& astroTextStore() {
    static AstroTextStore instance;
    return instance;
}

} // namespace astro
