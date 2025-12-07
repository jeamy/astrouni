#pragma once
/**
 * @file transit_result_window.h
 * @brief Transit-Ergebnis-Fenster
 * 
 * 1:1 Port von WndTransit/TRANSITCLASS aus legacy/astrouni.c
 * 
 * Zeigt Multi-Transit-Ergebnisse mit:
 * - Listbox für Aspekt-Sequenzen (hWndLBG)
 * - "Grafik"-Button (PB_UT_GRAF)
 * - "Rückl."-Button (PB_UT_RUCK)
 * - Datum-Anzeige "Transite von [Datum] bis [Datum]"
 */

#include <QWidget>
#include <QListWidget>
#include <QLabel>
#include <QPushButton>
#include <QVector>
#include "../core/data_types.h"
#include "../core/transit_calc.h"

namespace astro {

class TransitResultWindow : public QWidget {
    Q_OBJECT
    
public:
    TransitResultWindow(QWidget* parent, AuInit& auinit, const Radix& basisRadix);
    ~TransitResultWindow();
    
    /**
     * @brief Setzt die berechneten Transits und Aspekte
     */
    void setTransits(const QVector<Radix>& transits, 
                     const QVector<TransitAspekt>& aspekte,
                     const QString& vonDatum, const QString& bisDatum,
                     const QVector<QVector<bool>>& transSel = {});
    void setTransitSelection(const QVector<QVector<bool>>& transSel) { m_transSel = transSel; }
    
    /**
     * @brief Gibt den Tab-Titel zurück
     */
    QString getTabTitle() const;

signals:
    void requestGraphic(int transitIndex);
    
private slots:
    void onGrafikClicked();
    void onRuecklaufClicked();
    void onListItemDoubleClicked(QListWidgetItem* item);
    
private:
    void setupUI();
    void buildTransitTexts();
    
    // Referenzen
    AuInit& m_auinit;
    const Radix& m_basisRadix;
    QVector<Radix> m_transits;
    QVector<TransitAspekt> m_aspekte;
    QVector<QVector<bool>> m_transSel; // Auswahlmatrix Transit x Radix/Haus
    
    // UI-Elemente (Port von WndTransit)
    QListWidget* m_listBox;        // hWndLBG
    QPushButton* m_grafikButton;   // PB_UT_GRAF
    QPushButton* m_ruecklaufButton; // PB_UT_RUCK
    QLabel* m_vonDatumLabel;       // TXT_UT_DATUM
    QLabel* m_bisDatumLabel;       // TXT_UT_BDATUM
};

} // namespace astro
