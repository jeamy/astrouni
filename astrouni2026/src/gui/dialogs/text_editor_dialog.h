#pragma once

#include <QDialog>

class QComboBox;
class QLabel;
class QPlainTextEdit;

namespace astro {

class TextEditorDialog : public QDialog {
    Q_OBJECT

public:
    explicit TextEditorDialog(QWidget* parent = nullptr);

protected:
    void reject() override;

private slots:
    void onSelectionChanged();
    void onEditorTextChanged();

    void onSaveCurrent();
    void onRevertCurrent();
    void onAccept();
    void onReject();

private:
    enum class TextType {
        SunSign = 0,
        Ascendant,
        MoonSign,
        PlanetName,
        SignName,
        AspectName,
        AspectSpecific,
        AspectGeneric,
        ContextAspect,
        AnalysisTemplate
    };

    void setupUI();
    void refreshUiForType();

    bool confirmDiscardOrSaveIfDirty();
    bool commitCurrentText();
    void loadCurrentText();

    QString currentLanguage() const;
    TextType currentType() const;

    QString currentKey() const;
    QString currentDefaultText() const;

    QString normalizeTextForEditor(const QString& value) const;
    QString normalizeTextForSave(const QString& value) const;
    void applyWrapTag(const QString& openTag, const QString& closeTag);
    void insertTagText(const QString& tagText);
    void updatePlaceholderHint();

    void setTypeComboItems();
    void setLanguageComboItems();
    void setSignComboItems(QComboBox* combo);
    void setPlanetComboItems(QComboBox* combo);
    void setAspectComboItems(QComboBox* combo);
    void setAnalysisKeyComboItems();

    QComboBox* m_languageCombo;
    QComboBox* m_typeCombo;

    QLabel* m_label1;
    QLabel* m_label2;
    QLabel* m_label3;
    QLabel* m_label4;
    QLabel* m_label5;

    QComboBox* m_combo1;
    QComboBox* m_combo2;
    QComboBox* m_combo3;
    QComboBox* m_combo4;
    QComboBox* m_combo5;

    QLabel* m_analysisKeyLabel;
    QComboBox* m_analysisKeyCombo;

    QPlainTextEdit* m_editor;

    QLabel* m_placeholderHintLabel;

    bool m_updatingUi;
    bool m_updatingEditor;
    bool m_editorDirty;
    QString m_loadedText;

    QString m_currentLang;
    QString m_currentKey;

    int m_prevLangIndex;
    int m_prevTypeIndex;
    int m_prev1;
    int m_prev2;
    int m_prev3;
    int m_prev4;
    int m_prev5;
    int m_prevAnalysisKey;
};

} // namespace astro
