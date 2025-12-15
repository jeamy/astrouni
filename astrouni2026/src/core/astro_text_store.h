#pragma once

#include <QMap>
#include <QSet>
#include <QString>

namespace astro {

class AstroTextStore {
public:
    AstroTextStore();

    void setFilePath(const QString& path);
    QString filePath() const;

    bool ensureLoaded();
    bool load();
    bool save();

    bool hasOverride(const QString& lang, const QString& key) const;
    QString overrideText(const QString& lang, const QString& key) const;

    QString text(const QString& lang, const QString& key, const QString& defaultValue) const;

    void setOverride(const QString& lang, const QString& key, const QString& value);
    void revertOverride(const QString& lang, const QString& key);
    void revertAll();

    bool isDirty() const;
    bool isDirty(const QString& lang, const QString& key) const;

    static QString systemLanguageCode();

private:
    QString m_filePath;
    bool m_loaded;

    QMap<QString, QMap<QString, QString>> m_overrides;
    QMap<QString, QMap<QString, QString>> m_snapshot;
    QSet<QString> m_dirty;

    static QString escapeValue(const QString& value);
    static QString unescapeValue(const QString& value);

    QString dirtyId(const QString& lang, const QString& key) const;
};

AstroTextStore& astroTextStore();

} // namespace astro
