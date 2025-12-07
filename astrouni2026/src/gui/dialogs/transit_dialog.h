#pragma once
/**
 * @file transit_dialog.h
 * @brief Transit-Dialog zum Eingeben von Transit-Datum/Zeit
 * 
 * 1:1 Port von DlgTransit aus legacy/astrofil.c
 */

#include <QDialog>
#include <QLineEdit>
#include <QDateEdit>
#include <QTimeEdit>
#include <QRadioButton>
#include <QButtonGroup>
#include <QVector>
#include "../../core/data_types.h"
#include "../../core/transit_calc.h"

namespace astro {

/**
 * @brief Transit-Dialog
 * 
 * Port von: TRANSIT Dialog, DlgTransit
 * 
 * Felder:
 * - Name (readonly, von Basis-Radix)
 * - Ort (editierbar, Standard: Ort des Basis-Radix)
 * - Datum (Start-Datum für Transit)
 * - Zeit (Start-Zeit für Transit)
 * - Bis-Datum (End-Datum für Multi-Transit)
 * - Bis-Zeit (End-Zeit für Multi-Transit)
 * 
 * HINWEIS: Die Listbox, Inkrement-Radiobuttons und Rückläufigkeit-Button
 * sind im Legacy im separaten Transit-Ergebnis-Fenster (WndTransit),
 * nicht im Eingabe-Dialog.
 */
class TransitDialog : public QDialog {
    Q_OBJECT
    
public:
    TransitDialog(QWidget* parent, AuInit& auinit, const Radix& basisRadix);
    ~TransitDialog();
    
    /**
     * @brief Gibt den Transit-Radix zurück
     */
    const Radix& getTransitRadix() const { return m_transitRadix; }
    
    /**
     * @brief Gibt alle berechneten Transits zurück (Multi-Transit)
     */
    const QVector<Radix>& getTransits() const { return m_transits; }
    
    /**
     * @brief Multi-Transit Metadaten und Auswahl
     */
    const QVector<TransitAspekt>& getTransitAspekte() const { return m_aspekte; }
    QVector<QVector<bool>> getTransSel() const { return m_transSel; }
    QString getVonDatum() const;
    QString getBisDatum() const;
    bool wasCalculated() const { return m_calculated; }
    
private slots:
    void onOrtSuchen();
    void onAccept();
    
private:
    void setupUI();
    void loadFromRadix();
    bool validateInput();
    void calculateTransit();
    void ensureDefaultTransSel();
    
    // Referenzen
    AuInit& m_auinit;
    const Radix& m_basisRadix;
    Radix m_transitRadix;
    QVector<Radix> m_transits;
    QVector<QString> m_transitTexts;
    QVector<TransitAspekt> m_aspekte;
    QVector<QVector<bool>> m_transSel; // Auswahlmatrix (TransitPlanet x Radix/Haus)
    bool m_abortFlag = false;
    
    // Eingabefelder (Port von TRANSIT Dialog)
    QLineEdit* m_nameEdit;         // ETXT_TName (readonly)
    QLineEdit* m_ortEdit;          // ETXT_TOrt
    QDateEdit* m_datumEdit;        // ETXT_TDatum
    QTimeEdit* m_zeitEdit;         // ETXT_TZeit
    QDateEdit* m_bisDatumEdit;     // ETXT_TBDatum
    QTimeEdit* m_bisZeitEdit;      // ETXT_TBZeit
    
    // Inkrement-Auswahl (STRICT LEGACY: RB_TAG, RB_MONAT, RB_JAHR)
    QButtonGroup* m_inkrementGroup;
    QRadioButton* m_tagRadio;
    QRadioButton* m_monatRadio;
    QRadioButton* m_jahrRadio;
    
    // Flag ob berechnet wurde
    bool m_calculated = false;
};

} // namespace astro
