#include "astro_text_analyzer.h"
#include "astro_text_store.h"
#include "constants.h"

#include <QCoreApplication>
#include <QFile>
#include <QSet>
#include <QStringConverter>
#include <QTextStream>

namespace {

QString escapeValue(const QString& value) {
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

struct ParsedKeys {
    QSet<QString> de;
    QSet<QString> en;
};

ParsedKeys collectExistingKeys(const QStringList& lines) {
    ParsedKeys out;

    QString currentLang;
    for (const QString& line : lines) {
        const QString trimmed = line.trimmed();
        if (trimmed.isEmpty()) {
            continue;
        }
        if (trimmed.startsWith('#') || trimmed.startsWith(';')) {
            continue;
        }

        if (trimmed.startsWith('[') && trimmed.endsWith(']')) {
            currentLang = trimmed.mid(1, trimmed.size() - 2).trimmed();
            continue;
        }

        const int eq = trimmed.indexOf('=');
        if (eq <= 0) {
            continue;
        }

        const QString key = trimmed.left(eq).trimmed();
        if (currentLang == "de") {
            out.de.insert(key);
        } else if (currentLang == "en") {
            out.en.insert(key);
        }
    }

    return out;
}

QStringList sortedKeys(const QMap<QString, QString>& map) {
    QStringList keys = map.keys();
    std::sort(keys.begin(), keys.end());
    return keys;
}

void appendMissingForLang(QStringList& outLines, const QString& lang, const QMap<QString, QString>& missing) {
    if (missing.isEmpty()) {
        return;
    }

    outLines << "";
    const QStringList keys = sortedKeys(missing);
    for (const QString& key : keys) {
        outLines << (key + "=" + escapeValue(missing.value(key)));
    }
}

} // namespace

int main(int argc, char* argv[]) {
    QCoreApplication app(argc, argv);

    const QStringList args = app.arguments();
    if (args.size() < 2) {
        QTextStream err(stderr);
        err << "Usage: " << args.value(0) << " /path/to/astrotext.dat\n";
        return 2;
    }

    const QString filePath = args.at(1);

    QStringList lines;
    {
        QFile f(filePath);
        if (f.exists()) {
            if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
                QTextStream err(stderr);
                err << "Cannot read file: " << filePath << "\n";
                return 3;
            }
            QTextStream in(&f);
            in.setEncoding(QStringConverter::Utf8);
            while (!in.atEnd()) {
                lines << in.readLine();
            }
        } else {
            lines << "[de]";
            lines << "";
            lines << "[en]";
        }
    }

    const ParsedKeys existing = collectExistingKeys(lines);

    astro::astroTextStore().setFilePath(filePath + ".__export_nonexistent__");

    astro::AstroTextAnalyzer analyzer;

    const QList<int16_t> aspects = {
        static_cast<int16_t>(astro::KONJUNKTION),
        static_cast<int16_t>(astro::HALBSEX),
        static_cast<int16_t>(astro::SEXTIL),
        static_cast<int16_t>(astro::QUADRATUR),
        static_cast<int16_t>(astro::TRIGON),
        static_cast<int16_t>(astro::QUINCUNX),
        static_cast<int16_t>(astro::OPOSITION),
    };

    QMap<QString, QString> missingDe;
    QMap<QString, QString> missingEn;

    for (int p1 = 0; p1 < astro::MAX_PLANET; ++p1) {
        for (int p2 = p1; p2 < astro::MAX_PLANET; ++p2) {
            for (int16_t asp : aspects) {
                const QString text = analyzer.planetPairText(p1, p2, asp);
                if (text.isEmpty()) {
                    continue;
                }

                const QString key = QString("aspect.pair.%1.%2.%3").arg(p1).arg(p2).arg(static_cast<int>(asp));

                if (!existing.de.contains(key)) {
                    missingDe.insert(key, text);
                }
                if (!existing.en.contains(key)) {
                    missingEn.insert(key, text);
                }
            }
        }
    }

    QStringList out;
    QString currentLang;
    bool wroteDe = false;
    bool wroteEn = false;

    for (const QString& line : lines) {
        const QString trimmed = line.trimmed();
        if (trimmed.startsWith('[') && trimmed.endsWith(']')) {
            if (currentLang == "de" && !wroteDe) {
                appendMissingForLang(out, "de", missingDe);
                wroteDe = true;
            } else if (currentLang == "en" && !wroteEn) {
                appendMissingForLang(out, "en", missingEn);
                wroteEn = true;
            }

            currentLang = trimmed.mid(1, trimmed.size() - 2).trimmed();
        }

        out << line;
    }

    if (currentLang == "de" && !wroteDe) {
        appendMissingForLang(out, "de", missingDe);
        wroteDe = true;
    } else if (currentLang == "en" && !wroteEn) {
        appendMissingForLang(out, "en", missingEn);
        wroteEn = true;
    }

    if (!wroteDe) {
        out << "";
        out << "[de]";
        appendMissingForLang(out, "de", missingDe);
    }
    if (!wroteEn) {
        out << "";
        out << "[en]";
        appendMissingForLang(out, "en", missingEn);
    }

    QFile outFile(filePath);
    if (!outFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream err(stderr);
        err << "Cannot write file: " << filePath << "\n";
        return 4;
    }

    QTextStream outStream(&outFile);
    outStream.setEncoding(QStringConverter::Utf8);
    for (int i = 0; i < out.size(); ++i) {
        outStream << out.at(i);
        outStream << "\n";
    }

    QTextStream log(stdout);
    log << "Added missing keys: de=" << missingDe.size() << ", en=" << missingEn.size() << "\n";

    return 0;
}
