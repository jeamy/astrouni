#include "text_editor_dialog.h"

#include "../../core/astro_text_store.h"
#include "../../core/astro_text_analyzer.h"
#include "../../core/constants.h"

#include <QComboBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QRegularExpression>
#include <QTextCursor>
#include <QVBoxLayout>

namespace astro {

TextEditorDialog::TextEditorDialog(QWidget* parent)
    : QDialog(parent)
    , m_languageCombo(nullptr)
    , m_typeCombo(nullptr)
    , m_label1(nullptr)
    , m_label2(nullptr)
    , m_label3(nullptr)
    , m_label4(nullptr)
    , m_label5(nullptr)
    , m_combo1(nullptr)
    , m_combo2(nullptr)
    , m_combo3(nullptr)
    , m_combo4(nullptr)
    , m_combo5(nullptr)
    , m_analysisKeyLabel(nullptr)
    , m_analysisKeyCombo(nullptr)
    , m_editor(nullptr)
    , m_placeholderHintLabel(nullptr)
    , m_updatingUi(false)
    , m_updatingEditor(false)
    , m_editorDirty(false)
    , m_prevLangIndex(0)
    , m_prevTypeIndex(0)
    , m_prev1(0)
    , m_prev2(0)
    , m_prev3(0)
    , m_prev4(0)
    , m_prev5(0)
    , m_prevAnalysisKey(0) {

    setWindowTitle(tr("Texte editieren"));
    setModal(true);

    astroTextStore().ensureLoaded();

    setupUI();

    const QString sysLang = AstroTextStore::systemLanguageCode();
    int langIndex = m_languageCombo->findData(sysLang);
    if (langIndex < 0) {
        langIndex = m_languageCombo->findData(QString("de"));
    }
    if (langIndex < 0) {
        langIndex = 0;
    }

    m_updatingUi = true;
    m_languageCombo->setCurrentIndex(langIndex);
    m_typeCombo->setCurrentIndex(static_cast<int>(TextType::SunSign));
    m_updatingUi = false;

    refreshUiForType();
    loadCurrentText();

    m_prevLangIndex = m_languageCombo->currentIndex();
    m_prevTypeIndex = m_typeCombo->currentIndex();
    m_prev1 = m_combo1->currentIndex();
    m_prev2 = m_combo2->currentIndex();
    m_prev3 = m_combo3->currentIndex();
    m_prev4 = m_combo4->currentIndex();
    m_prev5 = m_combo5->currentIndex();
    m_prevAnalysisKey = m_analysisKeyCombo->currentIndex();
}

void TextEditorDialog::reject() {
    astroTextStore().revertAll();
    QDialog::reject();
}

void TextEditorDialog::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    QGridLayout* grid = new QGridLayout();

    QLabel* langLabel = new QLabel(tr("Sprache"), this);
    m_languageCombo = new QComboBox(this);
    setLanguageComboItems();

    QLabel* typeLabel = new QLabel(tr("Texttyp"), this);
    m_typeCombo = new QComboBox(this);
    setTypeComboItems();

    grid->addWidget(langLabel, 0, 0);
    grid->addWidget(m_languageCombo, 0, 1, 1, 3);

    grid->addWidget(typeLabel, 1, 0);
    grid->addWidget(m_typeCombo, 1, 1, 1, 3);

    m_analysisKeyLabel = new QLabel(tr("Analyse"), this);
    m_analysisKeyCombo = new QComboBox(this);
    setAnalysisKeyComboItems();

    grid->addWidget(m_analysisKeyLabel, 2, 0);
    grid->addWidget(m_analysisKeyCombo, 2, 1, 1, 3);

    m_label1 = new QLabel(tr("Sternzeichen"), this);
    m_label2 = new QLabel(tr("Planet"), this);
    m_label3 = new QLabel(tr("Aspekt"), this);
    m_label4 = new QLabel(tr("Planet 2"), this);
    m_label5 = new QLabel(tr("Sternzeichen 2"), this);

    m_combo1 = new QComboBox(this);
    m_combo2 = new QComboBox(this);
    m_combo3 = new QComboBox(this);
    m_combo4 = new QComboBox(this);
    m_combo5 = new QComboBox(this);

    setSignComboItems(m_combo1);
    setPlanetComboItems(m_combo2);
    setAspectComboItems(m_combo3);
    setPlanetComboItems(m_combo4);
    setSignComboItems(m_combo5);

    // Zeile 1: Sternzeichen + Planet
    grid->addWidget(m_label1, 3, 0);
    grid->addWidget(m_combo1, 3, 1);
    grid->addWidget(m_label2, 3, 2);
    grid->addWidget(m_combo2, 3, 3);

    // Zeile 2: Aspekt
    grid->addWidget(m_label3, 4, 0);
    grid->addWidget(m_combo3, 4, 1, 1, 3);

    // Zeile 3: Sternzeichen 2 + Planet 2
    grid->addWidget(m_label5, 5, 0);
    grid->addWidget(m_combo5, 5, 1);
    grid->addWidget(m_label4, 5, 2);
    grid->addWidget(m_combo4, 5, 3);

    mainLayout->addLayout(grid);

    // Mini-Format-Leiste
    QHBoxLayout* formatLayout = new QHBoxLayout();
    QPushButton* boldButton = new QPushButton("b", this);
    QPushButton* italicButton = new QPushButton("i", this);
    QPushButton* nlButton = new QPushButton("nl", this);

    boldButton->setFixedWidth(28);
    italicButton->setFixedWidth(28);
    nlButton->setFixedWidth(36);

    connect(boldButton, &QPushButton::clicked, this, [this]() { applyWrapTag("<b>", "</b>"); });
    connect(italicButton, &QPushButton::clicked, this, [this]() { applyWrapTag("<i>", "</i>"); });
    connect(nlButton, &QPushButton::clicked, this, [this]() { insertTagText("\n"); });

    formatLayout->addWidget(boldButton);
    formatLayout->addWidget(italicButton);
    formatLayout->addWidget(nlButton);
    formatLayout->addStretch();
    mainLayout->addLayout(formatLayout);

    m_editor = new QPlainTextEdit(this);
    mainLayout->addWidget(m_editor, 1);

    m_placeholderHintLabel = new QLabel(this);
    m_placeholderHintLabel->setWordWrap(true);
    m_placeholderHintLabel->setTextFormat(Qt::PlainText);
    m_placeholderHintLabel->setVisible(false);
    mainLayout->addWidget(m_placeholderHintLabel);

    QHBoxLayout* buttonLayout = new QHBoxLayout();

    QPushButton* saveButton = new QPushButton(tr("Speichern"), this);
    connect(saveButton, &QPushButton::clicked, this, &TextEditorDialog::onSaveCurrent);
    buttonLayout->addWidget(saveButton);

    QPushButton* revertButton = new QPushButton(tr("Verwerfen"), this);
    connect(revertButton, &QPushButton::clicked, this, &TextEditorDialog::onRevertCurrent);
    buttonLayout->addWidget(revertButton);

    buttonLayout->addStretch();

    QPushButton* okButton = new QPushButton(tr("OK"), this);
    okButton->setDefault(true);
    connect(okButton, &QPushButton::clicked, this, &TextEditorDialog::onAccept);
    buttonLayout->addWidget(okButton);

    QPushButton* cancelButton = new QPushButton(tr("Abbrechen"), this);
    connect(cancelButton, &QPushButton::clicked, this, &TextEditorDialog::onReject);
    buttonLayout->addWidget(cancelButton);

    mainLayout->addLayout(buttonLayout);

    connect(m_languageCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &TextEditorDialog::onSelectionChanged);
    connect(m_typeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &TextEditorDialog::onSelectionChanged);
    connect(m_analysisKeyCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &TextEditorDialog::onSelectionChanged);
    connect(m_combo1, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &TextEditorDialog::onSelectionChanged);
    connect(m_combo2, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &TextEditorDialog::onSelectionChanged);
    connect(m_combo3, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &TextEditorDialog::onSelectionChanged);
    connect(m_combo4, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &TextEditorDialog::onSelectionChanged);
    connect(m_combo5, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &TextEditorDialog::onSelectionChanged);

    connect(m_editor, &QPlainTextEdit::textChanged, this, &TextEditorDialog::onEditorTextChanged);
}

void TextEditorDialog::setLanguageComboItems() {
    m_languageCombo->clear();
    m_languageCombo->addItem("de", "de");
    m_languageCombo->addItem("en", "en");
}

void TextEditorDialog::setTypeComboItems() {
    m_typeCombo->clear();
    m_typeCombo->addItem(tr("Sonnenzeichen-Text"), static_cast<int>(TextType::SunSign));
    m_typeCombo->addItem(tr("Aszendent-Text"), static_cast<int>(TextType::Ascendant));
    m_typeCombo->addItem(tr("Mondzeichen-Text"), static_cast<int>(TextType::MoonSign));
    m_typeCombo->addItem(tr("Planet-Name"), static_cast<int>(TextType::PlanetName));
    m_typeCombo->addItem(tr("Sternzeichen-Name"), static_cast<int>(TextType::SignName));
    m_typeCombo->addItem(tr("Aspekt-Name"), static_cast<int>(TextType::AspectName));
    m_typeCombo->addItem(tr("Aspekt-Text (Planeten-Kombi)"), static_cast<int>(TextType::AspectSpecific));
    m_typeCombo->addItem(tr("Aspekt-Text (Generisch)"), static_cast<int>(TextType::AspectGeneric));
    m_typeCombo->addItem(tr("Aspekt-Text (Kontext)"), static_cast<int>(TextType::ContextAspect));
    m_typeCombo->addItem(tr("Analyse-Text"), static_cast<int>(TextType::AnalysisTemplate));
    m_typeCombo->addItem(tr("Aspekt-Text (Planet zu ASC)"), static_cast<int>(TextType::AspectToAsc));
    m_typeCombo->addItem(tr("Aspekt-Text (Planet zu MC)"), static_cast<int>(TextType::AspectToMc));
}

void TextEditorDialog::setSignComboItems(QComboBox* combo) {
    combo->clear();
    AstroTextAnalyzer analyzer;
    for (int i = 0; i < 12; ++i) {
        combo->addItem(QString("%1 %2 %3").arg(i).arg(QString(STERNZEICHEN_SYMBOLS[i])).arg(analyzer.getSignName(i)), i);
    }
}

void TextEditorDialog::setPlanetComboItems(QComboBox* combo) {
    combo->clear();
    AstroTextAnalyzer analyzer;
    for (int i = 0; i < MAX_PLANET; ++i) {
        combo->addItem(QString("%1 %2 %3").arg(i).arg(QString(PLANET_SYMBOLS[i])).arg(analyzer.getPlanetName(i)), i);
    }
}

void TextEditorDialog::setAspectComboItems(QComboBox* combo) {
    combo->clear();
    AstroTextAnalyzer analyzer;
    combo->addItem(QString("%1 (%2°)").arg(QString(ASPEKT_SYMBOLS[0])).arg(KONJUNKTION), KONJUNKTION);
    combo->addItem(QString("%1 (%2°)").arg(QString(ASPEKT_SYMBOLS[1])).arg(HALBSEX), HALBSEX);
    combo->addItem(QString("%1 (%2°)").arg(QString(ASPEKT_SYMBOLS[2])).arg(SEXTIL), SEXTIL);
    combo->addItem(QString("%1 (%2°)").arg(QString(ASPEKT_SYMBOLS[3])).arg(QUADRATUR), QUADRATUR);
    combo->addItem(QString("%1 (%2°)").arg(QString(ASPEKT_SYMBOLS[4])).arg(TRIGON), TRIGON);
    combo->addItem(QString("%1 (%2°)").arg(QString(ASPEKT_SYMBOLS[5])).arg(QUINCUNX), QUINCUNX);
    combo->addItem(QString("%1 (%2°)").arg(QString(ASPEKT_SYMBOLS[6])).arg(OPOSITION), OPOSITION);

    for (int i = 0; i < combo->count(); ++i) {
        const int16_t a = static_cast<int16_t>(combo->itemData(i).toInt());
        combo->setItemText(i, combo->itemText(i) + QString(" - %1").arg(analyzer.getAspectName(a)));
    }
}

void TextEditorDialog::setAnalysisKeyComboItems() {
    m_analysisKeyCombo->clear();

    m_analysisKeyCombo->addItem(tr("Radix: Default-Name"), "analysis.radix.default_name");
    m_analysisKeyCombo->addItem(tr("Radix: Titel"), "analysis.radix.title");
    m_analysisKeyCombo->addItem(tr("Radix: Geboren-Zeile"), "analysis.radix.born_line");
    m_analysisKeyCombo->addItem(tr("Radix: Label Sonnenzeichen"), "analysis.label.sun_sign");
    m_analysisKeyCombo->addItem(tr("Radix: Label Aszendent"), "analysis.label.ascendant");
    m_analysisKeyCombo->addItem(tr("Radix: Label Mondzeichen"), "analysis.label.moon_sign");
    m_analysisKeyCombo->addItem(tr("Radix: Label Wichtige Aspekte"), "analysis.label.important_aspects");
    m_analysisKeyCombo->addItem(tr("Radix: Aspekte Intro"), "analysis.radix.aspects_intro");
    m_analysisKeyCombo->addItem(tr("Radix: Keine Aspekte"), "analysis.radix.no_aspects");

    m_analysisKeyCombo->addItem(tr("Transit: Titel"), "analysis.transit.title");
    m_analysisKeyCombo->addItem(tr("Transit: Datum-String"), "analysis.transit.date_line");
    m_analysisKeyCombo->addItem(tr("Transit: Intro"), "analysis.transit.intro");
    m_analysisKeyCombo->addItem(tr("Transit: Section Aktuelle Einflüsse"), "analysis.transit.section_influences");
    m_analysisKeyCombo->addItem(tr("Transit: Definition"), "analysis.transit.definition");
    m_analysisKeyCombo->addItem(tr("Transit: Keine Aspekte"), "analysis.transit.no_aspects");

    m_analysisKeyCombo->addItem(tr("Synastrie: Titel"), "analysis.synastry.title");
    m_analysisKeyCombo->addItem(tr("Synastrie: Section Beziehungs-Aspekte"), "analysis.synastry.section_relationship");
    m_analysisKeyCombo->addItem(tr("Synastrie: Keine Aspekte"), "analysis.synastry.no_aspects");

    m_analysisKeyCombo->addItem(tr("Kontext: Element Feuer"), "element.fire");
    m_analysisKeyCombo->addItem(tr("Kontext: Element Erde"), "element.earth");
    m_analysisKeyCombo->addItem(tr("Kontext: Element Luft"), "element.air");
    m_analysisKeyCombo->addItem(tr("Kontext: Element Wasser"), "element.water");

    m_analysisKeyCombo->addItem(tr("Kontext: Qualität Kardinal"), "quality.cardinal");
    m_analysisKeyCombo->addItem(tr("Kontext: Qualität Fix"), "quality.fixed");
    m_analysisKeyCombo->addItem(tr("Kontext: Qualität Veränderlich"), "quality.mutable");

    m_analysisKeyCombo->addItem(tr("Kontext: Gleiches Element Feuer"), "context.same.fire");
    m_analysisKeyCombo->addItem(tr("Kontext: Gleiches Element Erde"), "context.same.earth");
    m_analysisKeyCombo->addItem(tr("Kontext: Gleiches Element Luft"), "context.same.air");
    m_analysisKeyCombo->addItem(tr("Kontext: Gleiches Element Wasser"), "context.same.water");

    m_analysisKeyCombo->addItem(tr("Kontext: Kombi Feuer/Luft"), "context.combo.fire_air");
    m_analysisKeyCombo->addItem(tr("Kontext: Kombi Erde/Wasser"), "context.combo.earth_water");
    m_analysisKeyCombo->addItem(tr("Kontext: Kombi Feuer/Erde"), "context.combo.fire_earth");
    m_analysisKeyCombo->addItem(tr("Kontext: Kombi Feuer/Wasser"), "context.combo.fire_water");
    m_analysisKeyCombo->addItem(tr("Kontext: Kombi Erde/Luft"), "context.combo.earth_air");
    m_analysisKeyCombo->addItem(tr("Kontext: Kombi Luft/Wasser"), "context.combo.air_water");

    m_analysisKeyCombo->addItem(tr("Kontext: Gleiche Qualität"), "context.quality.same");
}

QString TextEditorDialog::currentLanguage() const {
    return m_languageCombo->currentData().toString();
}

TextEditorDialog::TextType TextEditorDialog::currentType() const {
    return static_cast<TextType>(m_typeCombo->currentData().toInt());
}

QString TextEditorDialog::currentKey() const {
    const TextType type = currentType();

    const int sign1 = m_combo1->currentData().toInt();
    const int planet1 = m_combo2->currentData().toInt();
    const int16_t aspect = static_cast<int16_t>(m_combo3->currentData().toInt());
    const int planet2 = m_combo4->currentData().toInt();
    const int sign2 = m_combo5->currentData().toInt();

    if (type == TextType::SunSign) {
        return QString("sun_sign.%1").arg(sign1);
    }
    if (type == TextType::Ascendant) {
        return QString("ascendant.%1").arg(sign1);
    }
    if (type == TextType::MoonSign) {
        return QString("moon_sign.%1").arg(sign1);
    }
    if (type == TextType::PlanetName) {
        return QString("planet_name.%1").arg(planet1);
    }
    if (type == TextType::SignName) {
        return QString("sign_name.%1").arg(sign1);
    }
    if (type == TextType::AspectName) {
        return QString("aspect_name.%1").arg(static_cast<int>(aspect));
    }
    if (type == TextType::AspectSpecific) {
        const int p1 = qMin(planet1, planet2);
        const int p2 = qMax(planet1, planet2);
        return QString("aspect.pair.%1.%2.%3").arg(p1).arg(p2).arg(static_cast<int>(aspect));
    }
    if (type == TextType::AspectGeneric) {
        return QString("aspect.generic.%1").arg(static_cast<int>(aspect));
    }
    if (type == TextType::ContextAspect) {
        return QString("aspect.context.%1.%2.%3.%4.%5")
            .arg(sign1)
            .arg(planet1)
            .arg(static_cast<int>(aspect))
            .arg(planet2)
            .arg(sign2);
    }
    if (type == TextType::AnalysisTemplate) {
        return m_analysisKeyCombo->currentData().toString();
    }
    if (type == TextType::AspectToAsc) {
        return QString("aspect.asc.%1.%2").arg(planet1).arg(static_cast<int>(aspect));
    }
    if (type == TextType::AspectToMc) {
        return QString("aspect.mc.%1.%2").arg(planet1).arg(static_cast<int>(aspect));
    }

    return QString();
}

QString TextEditorDialog::currentDefaultText() const {
    const TextType type = currentType();

    const int sign1 = m_combo1->currentData().toInt();
    const int planet1 = m_combo2->currentData().toInt();
    const int16_t aspect = static_cast<int16_t>(m_combo3->currentData().toInt());
    const int planet2 = m_combo4->currentData().toInt();
    const int sign2 = m_combo5->currentData().toInt();

    AstroTextAnalyzer analyzer;

    if (type == TextType::SunSign) {
        return analyzer.analyzeSunSign(static_cast<int8_t>(sign1));
    }
    if (type == TextType::Ascendant) {
        return analyzer.analyzeAscendant(static_cast<int8_t>(sign1));
    }
    if (type == TextType::MoonSign) {
        return analyzer.analyzeMoonSign(static_cast<int8_t>(sign1));
    }
    if (type == TextType::PlanetName) {
        return analyzer.getPlanetName(planet1);
    }
    if (type == TextType::SignName) {
        return analyzer.getSignName(static_cast<int8_t>(sign1));
    }
    if (type == TextType::AspectName) {
        return analyzer.getAspectName(aspect);
    }
    if (type == TextType::AspectSpecific) {
        Q_UNUSED(sign1);
        Q_UNUSED(sign2);
        return analyzer.planetPairText(planet1, planet2, aspect);
    }
    if (type == TextType::AspectGeneric) {
        Q_UNUSED(sign1);
        Q_UNUSED(sign2);
        return analyzer.genericAspectTemplate(aspect);
    }
    if (type == TextType::ContextAspect) {
        Q_UNUSED(sign1);
        Q_UNUSED(sign2);
        return analyzer.contextAspectText(
            planet1,
            static_cast<int8_t>(m_combo1->currentData().toInt()),
            planet2,
            static_cast<int8_t>(m_combo5->currentData().toInt()),
            aspect);
    }
    if (type == TextType::AnalysisTemplate) {
        const QString key = m_analysisKeyCombo->currentData().toString();
        if (key == "analysis.radix.default_name") {
            return "Radix";
        }
        if (key == "analysis.radix.title") {
            return "Horoskop-Analyse: %1";
        }
        if (key == "analysis.radix.born_line") {
            return "<p><i>Geboren am %1 um %2 Uhr in %3</i></p>";
        }
        if (key == "analysis.label.sun_sign") {
            return "Sonnenzeichen";
        }
        if (key == "analysis.label.ascendant") {
            return "Aszendent";
        }
        if (key == "analysis.label.moon_sign") {
            return "Mondzeichen";
        }
        if (key == "analysis.label.important_aspects") {
            return "Wichtige Aspekte";
        }
        if (key == "analysis.radix.aspects_intro") {
            return "<p><i>Aspekte zeigen die Beziehungen zwischen den Planeten.</i></p>";
        }
        if (key == "analysis.radix.no_aspects") {
            return "<p><i>Keine signifikanten Aspekte gefunden.</i></p>";
        }
        if (key == "analysis.transit.title") {
            return "Horoskop-Analyse: Transit";
        }
        if (key == "analysis.transit.date_line") {
            return "Datum: %1.%2.%3 um %4:%5 Uhr";
        }
        if (key == "analysis.transit.intro") {
            return "<p>Diese Analyse beschreibt die aktuellen planetaren Einflüsse auf Ihr Geburtshoroskop.</p>";
        }
        if (key == "analysis.transit.section_influences") {
            return "Aktuelle Einflüsse";
        }
        if (key == "analysis.transit.definition") {
            return "<p>Transite zeigen zeitweilige Einflüsse der aktuellen Planetenstände auf Ihr Geburtshoroskop.</p>";
        }
        if (key == "analysis.transit.no_aspects") {
            return "<p><i>Keine signifikanten Transite für diesen Tag.</i></p>";
        }
        if (key == "analysis.synastry.title") {
            return "Horoskop-Analyse: Partner-Vergleich";
        }
        if (key == "analysis.synastry.section_relationship") {
            return "Beziehungs-Aspekte";
        }
        if (key == "analysis.synastry.no_aspects") {
            return "<p><i>Keine starken Verbindungsaspekte gefunden.</i></p>";
        }

        if (key == "element.fire") {
            return "Feuer";
        }
        if (key == "element.earth") {
            return "Erde";
        }
        if (key == "element.air") {
            return "Luft";
        }
        if (key == "element.water") {
            return "Wasser";
        }

        if (key == "quality.cardinal") {
            return "kardinal";
        }
        if (key == "quality.fixed") {
            return "fix";
        }
        if (key == "quality.mutable") {
            return "veränderlich";
        }

        if (key == "context.same.fire") {
            return "Beide Planeten stehen in %1-Zeichen, was diesem Aspekt besondere Dynamik, Energie und Leidenschaft verleiht. Hier ist viel Initiative und Unternehmungslust vorhanden, aber auch die Gefahr von Impulsivität und Ungeduld.";
        }
        if (key == "context.same.earth") {
            return "Beide Planeten stehen in %1-Zeichen, was diesem Aspekt Stabilität, Ausdauer und Pragmatismus verleiht. Sie gehen methodisch vor und bauen auf solide Fundamente. Materielle Sicherheit ist wichtig.";
        }
        if (key == "context.same.air") {
            return "Beide Planeten stehen in %1-Zeichen, was diesem Aspekt geistige Beweglichkeit und kommunikative Fähigkeiten verleiht. Ideen und sozialer Austausch stehen im Vordergrund. Objektivität ist eine Stärke.";
        }
        if (key == "context.same.water") {
            return "Beide Planeten stehen in %1-Zeichen, was diesem Aspekt emotionale Tiefe und intuitive Qualitäten verleiht. Gefühle und Empfindungen sind hier sehr stark. Einfühlungsvermögen und Sensibilität prägen diesen Bereich.";
        }

        if (key == "context.combo.fire_air") {
            return "%1 (%2) und %3 (%4) verbinden sich hier zu einer energiegeladenen, enthusiastischen Kombination. Ideen werden in Aktion umgesetzt, Inspiration führt zu Taten.";
        }
        if (key == "context.combo.earth_water") {
            return "%1 (%2) und %3 (%4) verbinden sich zu einer fruchtbaren, nährenden Kombination. Praktisches und Emotionales arbeiten zusammen. Gefühle finden konkreten Ausdruck.";
        }
        if (key == "context.combo.fire_earth") {
            return "%1 (%2) und %3 (%4) verbinden Impuls mit Substanz. Der Feuerdrang will schnelle Ergebnisse, die Erde fordert Geduld. Diese Spannung kann zu konkreten Errungenschaften führen.";
        }
        if (key == "context.combo.fire_water") {
            return "%1 (%2) und %3 (%4) erzeugen Dampf - eine kraftvolle, aber volatile Mischung. Leidenschaft trifft auf Gefühl, was zu intensiven, aber auch stürmischen Energien führen kann.";
        }
        if (key == "context.combo.earth_air") {
            return "%1 (%2) und %3 (%4) verbinden Theorie mit Praxis. Ideen brauchen praktische Umsetzung, und die Erde erdet das manchmal zu abstrakte Denken der Luft.";
        }
        if (key == "context.combo.air_water") {
            return "%1 (%2) und %3 (%4) verbinden Verstand mit Gefühl. Das kann zu künstlerischer Sensibilität führen, aber auch zu Konflikten zwischen Kopf und Herz.";
        }

        if (key == "context.quality.same") {
            return " Beide Zeichen sind %1, was die Intensität dieses Aspekts verstärkt.";
        }
        return QString();
    }
    if (type == TextType::AspectToAsc) {
        return analyzer.getAscAspectText(planet1, aspect);
    }
    if (type == TextType::AspectToMc) {
        return analyzer.getMcAspectText(planet1, aspect);
    }

    return QString();
}

QString TextEditorDialog::normalizeTextForEditor(const QString& value) const {
    QString v = value;
    v.replace("\r\n", "\n");
    v.replace('\r', '\n');

    const TextType type = currentType();
    const QString key = m_currentKey;

    const bool isPlain = (type == TextType::PlanetName || type == TextType::SignName || type == TextType::AspectName ||
                          key == "analysis.radix.default_name" ||
                          key.startsWith("analysis.label.") ||
                          key.startsWith("element.") ||
                          key.startsWith("quality."));

    if (isPlain) {
        return v;
    }

    v.replace(QRegularExpression("(?i)<br\\s*/?>"), "\n");

    const bool needsParagraphs = (type == TextType::SunSign || type == TextType::Ascendant || type == TextType::MoonSign || type == TextType::ContextAspect ||
                                 key == "analysis.radix.born_line" ||
                                 key == "analysis.radix.aspects_intro" ||
                                 key == "analysis.radix.no_aspects" ||
                                 key == "analysis.transit.intro" ||
                                 key == "analysis.transit.definition" ||
                                 key == "analysis.transit.no_aspects" ||
                                 key == "analysis.synastry.no_aspects");

    if (needsParagraphs) {
        v.replace(QRegularExpression("(?i)</p>\\s*<p>"), "\n\n");
        v.replace(QRegularExpression("(?i)^\\s*<p>"), "");
        v.replace(QRegularExpression("(?i)</p>\\s*$"), "");
        v.replace(QRegularExpression("(?i)<\\s*/?p\\s*>"), "");
    }

    return v;
}

QString TextEditorDialog::normalizeTextForSave(const QString& value) const {
    QString v = value;
    v.replace("\r\n", "\n");
    v.replace('\r', '\n');

    const TextType type = currentType();
    const QString key = m_currentKey;

    const bool isPlain = (type == TextType::PlanetName || type == TextType::SignName || type == TextType::AspectName ||
                          key == "analysis.radix.default_name" ||
                          key.startsWith("analysis.label.") ||
                          key.startsWith("element.") ||
                          key.startsWith("quality."));

    if (isPlain) {
        return v;
    }

    const bool needsParagraphs = (type == TextType::SunSign || type == TextType::Ascendant || type == TextType::MoonSign || type == TextType::ContextAspect ||
                                 key == "analysis.radix.born_line" ||
                                 key == "analysis.radix.aspects_intro" ||
                                 key == "analysis.radix.no_aspects" ||
                                 key == "analysis.transit.intro" ||
                                 key == "analysis.transit.definition" ||
                                 key == "analysis.transit.no_aspects" ||
                                 key == "analysis.synastry.no_aspects");

    if (needsParagraphs) {
        QString tmp = v;
        const QStringList paras = tmp.split(QRegularExpression("\\n\\s*\\n+"), Qt::SkipEmptyParts);
        QString out;
        for (const QString& paraRaw : paras) {
            QString para = paraRaw;
            para.replace("\n", "<br>");
            if (!out.isEmpty()) {
                out += "</p><p>";
            }
            out += para;
        }
        return QString("<p>%1</p>").arg(out);
    }

    v.replace("\n", "<br>");
    return v;
}

void TextEditorDialog::applyWrapTag(const QString& openTag, const QString& closeTag) {
    if (!m_editor) {
        return;
    }

    QTextCursor cursor = m_editor->textCursor();
    if (!cursor.hasSelection()) {
        return;
    }

    QString sel = cursor.selectedText();
    sel.replace(QChar(0x2029), "\n");
    cursor.beginEditBlock();
    cursor.insertText(openTag + sel + closeTag);
    cursor.endEditBlock();
}

void TextEditorDialog::insertTagText(const QString& tagText) {
    if (!m_editor) {
        return;
    }
    QTextCursor cursor = m_editor->textCursor();
    cursor.insertText(tagText);
}

void TextEditorDialog::updatePlaceholderHint() {
    if (!m_editor || !m_placeholderHintLabel) {
        return;
    }

    const QString text = m_editor->toPlainText();
    QRegularExpression re("%([1-9]\\d*)");
    QRegularExpressionMatchIterator it = re.globalMatch(text);

    QSet<int> nums;
    while (it.hasNext()) {
        const QRegularExpressionMatch m = it.next();
        nums.insert(m.captured(1).toInt());
    }

    if (nums.isEmpty()) {
        m_placeholderHintLabel->clear();
        m_placeholderHintLabel->setVisible(false);
        return;
    }

    const QString key = m_currentKey;
    const TextType type = currentType();

    QString hint;
    if (type == TextType::AspectGeneric || key.startsWith("aspect.generic.")) {
        hint = "%1 = Planet 1\n%2 = Planet 2";
    } else if (key == "analysis.radix.title") {
        hint = "%1 = Name (z.B. Person)";
    } else if (key == "analysis.radix.born_line") {
        hint = "%1 = Datum\n%2 = Uhrzeit\n%3 = Ort";
    } else if (key == "analysis.transit.date_line") {
        hint = "%1 = Tag\n%2 = Monat\n%3 = Jahr\n%4 = Stunde\n%5 = Minute";
    } else if (key.startsWith("context.same.")) {
        hint = "%1 = Element";
    } else if (key.startsWith("context.combo.")) {
        hint = "%1 = Planet 1\n%2 = Element 1\n%3 = Planet 2\n%4 = Element 2";
    } else if (key == "context.quality.same") {
        hint = "%1 = Qualität";
    } else {
        hint = "Platzhalter werden zur Laufzeit ersetzt (z.B. %1, %2, ...).";
    }

    m_placeholderHintLabel->setText(hint);
    m_placeholderHintLabel->setVisible(true);
}

void TextEditorDialog::refreshUiForType() {
    const TextType type = currentType();

    const bool isAnalysis = (type == TextType::AnalysisTemplate);
    const bool isAscMc = (type == TextType::AspectToAsc || type == TextType::AspectToMc);

    m_analysisKeyLabel->setEnabled(isAnalysis);
    m_analysisKeyCombo->setEnabled(isAnalysis);

    const bool enableSign1 = (type == TextType::SunSign || type == TextType::Ascendant || type == TextType::MoonSign || type == TextType::SignName || type == TextType::ContextAspect);
    const bool enablePlanet1 = (type == TextType::PlanetName || type == TextType::AspectSpecific || type == TextType::ContextAspect || isAscMc);
    const bool enableAspect = (type == TextType::AspectName || type == TextType::AspectSpecific || type == TextType::AspectGeneric || type == TextType::ContextAspect || isAscMc);
    const bool enablePlanet2 = (type == TextType::AspectSpecific || type == TextType::ContextAspect);
    const bool enableSign2 = (type == TextType::ContextAspect);

    m_label1->setEnabled(enableSign1);
    m_combo1->setEnabled(enableSign1);

    m_label2->setEnabled(enablePlanet1);
    m_combo2->setEnabled(enablePlanet1);

    m_label3->setEnabled(enableAspect);
    m_combo3->setEnabled(enableAspect);

    m_label4->setEnabled(enablePlanet2);
    m_combo4->setEnabled(enablePlanet2);

    m_label5->setEnabled(enableSign2);
    m_combo5->setEnabled(enableSign2);

    // Labels für ASC/MC anpassen
    if (isAscMc) {
        m_label2->setText(tr("Planet"));
        m_label4->setText(type == TextType::AspectToAsc ? tr("ASC") : tr("MC"));
    } else {
        m_label2->setText(tr("Objekt 1"));
        m_label4->setText(tr("Objekt 2"));
    }

    if (isAnalysis) {
        m_combo1->setEnabled(false);
        m_combo2->setEnabled(false);
        m_combo3->setEnabled(false);
        m_combo4->setEnabled(false);
        m_combo5->setEnabled(false);

        m_label1->setEnabled(false);
        m_label2->setEnabled(false);
        m_label3->setEnabled(false);
        m_label4->setEnabled(false);
        m_label5->setEnabled(false);
    }
}

void TextEditorDialog::onSelectionChanged() {
    if (m_updatingUi) {
        return;
    }

    if (!confirmDiscardOrSaveIfDirty()) {
        m_updatingUi = true;
        m_languageCombo->setCurrentIndex(m_prevLangIndex);
        m_typeCombo->setCurrentIndex(m_prevTypeIndex);
        m_combo1->setCurrentIndex(m_prev1);
        m_combo2->setCurrentIndex(m_prev2);
        m_combo3->setCurrentIndex(m_prev3);
        m_combo4->setCurrentIndex(m_prev4);
        m_combo5->setCurrentIndex(m_prev5);
        m_analysisKeyCombo->setCurrentIndex(m_prevAnalysisKey);
        m_updatingUi = false;

        refreshUiForType();
        loadCurrentText();
        return;
    }

    m_prevLangIndex = m_languageCombo->currentIndex();
    m_prevTypeIndex = m_typeCombo->currentIndex();
    m_prev1 = m_combo1->currentIndex();
    m_prev2 = m_combo2->currentIndex();
    m_prev3 = m_combo3->currentIndex();
    m_prev4 = m_combo4->currentIndex();
    m_prev5 = m_combo5->currentIndex();
    m_prevAnalysisKey = m_analysisKeyCombo->currentIndex();

    refreshUiForType();
    loadCurrentText();
}

void TextEditorDialog::onEditorTextChanged() {
    if (m_updatingEditor) {
        return;
    }
    m_editorDirty = (m_editor->toPlainText() != m_loadedText);
    updatePlaceholderHint();
}

bool TextEditorDialog::confirmDiscardOrSaveIfDirty() {
    if (!m_editorDirty) {
        return true;
    }

    const QMessageBox::StandardButton btn = QMessageBox::warning(
        this,
        tr("Ungespeicherte Änderungen"),
        tr("Der Text wurde geändert. Möchten Sie speichern?"),
        QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
        QMessageBox::Save);

    if (btn == QMessageBox::Cancel) {
        return false;
    }

    if (btn == QMessageBox::Save) {
        return commitCurrentText();
    }

    if (btn == QMessageBox::Discard) {
        // Änderungen am aktuellen (vorherigen) Text verwerfen
        m_updatingEditor = true;
        m_editor->setPlainText(m_loadedText);
        m_updatingEditor = false;
        m_editorDirty = false;
        return true;
    }

    return true;
}

bool TextEditorDialog::commitCurrentText() {
    const QString lang = m_currentLang;
    const QString key = m_currentKey;

    if (key.isEmpty()) {
        return true;
    }

    const QString editorValue = m_editor->toPlainText();
    astroTextStore().setOverride(lang, key, normalizeTextForSave(editorValue));

    m_loadedText = editorValue;
    m_editorDirty = false;

    return true;
}

void TextEditorDialog::loadCurrentText() {
    const QString lang = currentLanguage();
    const QString key = currentKey();

    m_currentLang = lang;
    m_currentKey = key;

    QString value;

    if (!key.isEmpty() && astroTextStore().hasOverride(lang, key)) {
        value = astroTextStore().overrideText(lang, key);
    } else {
        value = currentDefaultText();
    }

    const QString editorValue = normalizeTextForEditor(value);

    m_updatingEditor = true;
    m_editor->setPlainText(editorValue);
    m_updatingEditor = false;

    m_loadedText = editorValue;
    m_editorDirty = false;

    updatePlaceholderHint();
}

void TextEditorDialog::onSaveCurrent() {
    commitCurrentText();
}

void TextEditorDialog::onRevertCurrent() {
    const QString lang = m_currentLang;
    const QString key = m_currentKey;

    if (!key.isEmpty()) {
        astroTextStore().revertOverride(lang, key);
    }

    loadCurrentText();
}

void TextEditorDialog::onAccept() {
    if (!confirmDiscardOrSaveIfDirty()) {
        return;
    }

    if (astroTextStore().isDirty()) {
        astroTextStore().save();
    }

    accept();
}

void TextEditorDialog::onReject() {
    reject();
}

} // namespace astro
