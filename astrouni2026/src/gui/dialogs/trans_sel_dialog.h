#pragma once
/**
 * @file trans_sel_dialog.h
 * @brief Port des Legacy-Dialogs DlgTransEin (Transit-Auswahl)
 *
 * Der Legacy-Dialog zeigt eine Matrix Transit-Planet x Radix-Planet/Haus,
 * die per Klick ein-/ausgeschaltet werden kann. Dies ist eine 1:1 Portierung
 * mit Scrollbarkeit statt fester Pixelkoordinaten.
 */

#include <QDialog>
#include <QVector>
#include <QCheckBox>
#include <QStringList>
#include <QLabel>
#include <QSpinBox>
#include <QPushButton>
#include "../../core/constants.h"

namespace astro {

class TransSelDialog : public QDialog {
    Q_OBJECT
public:
    explicit TransSelDialog(QWidget* parent = nullptr);

    /**
     * @brief Setzt die Auswahlmatrix (Transit x Radix)
     * @param sel Matrix der Größe transitCount x radixCount
     */
    void setSelection(const QVector<QVector<bool>>& sel);

    /**
     * @brief Liefert die aktuelle Auswahlmatrix
     */
    QVector<QVector<bool>> selection() const;
    
    /**
     * @brief Setzt optionale Beschriftungen für Transit- und Radix-Zeilen/Spalten
     */
    void setLabels(const QStringList& transitLabels, const QStringList& radixLabels);
    
    /**
     * @brief Setzt Orben-Texte (berechnet aus Einstellungen)
     */
    void setOrbs(const QVector<float>& orbenTPlanet, const QVector<float>& orbenTHaus);

private:
    void buildUI();
    void updateChecks();
    void updateHeaderStates();
    void toggleRow(int row, Qt::CheckState state);
    void toggleColumn(int col, Qt::CheckState state);
    QString formatOrbs() const;
    void applyPresetAll(bool value);
    void applyPresetPlanets(bool value);
    void applyPresetHouses(bool value);
    void applyPresetAster(bool value);
    void updatePresetButtons(int mode); // 0=none,1=all,2=planets,3=houses,4=aster

    int m_transitCount;
    int m_radixCount;
    QVector<QVector<bool>> m_sel;
    QVector<QVector<QCheckBox*>> m_checks;
    QVector<QCheckBox*> m_rowHeaders;
    QVector<QCheckBox*> m_colHeaders;
    QStringList m_transitLabels;
    QStringList m_radixLabels;
    QVector<float> m_orbenTPlanet;
    QVector<float> m_orbenTHaus;
    QLabel* m_orbLabel = nullptr;
    QSpinBox* m_hScroll = nullptr;
    QSpinBox* m_vScroll = nullptr;
    int m_xOffset = 0;
    int m_yOffset = 0;
    QPushButton* m_allBtn = nullptr;
    QPushButton* m_noneBtn = nullptr;
    QPushButton* m_planetsBtn = nullptr;
    QPushButton* m_housesBtn = nullptr;
    QPushButton* m_asterBtn = nullptr;
};

} // namespace astro
