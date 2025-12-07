#pragma once
/**
 * @file retrograde_dialog.h
 * @brief Port des Legacy DlgRuck (Rückläufigkeits-Dialog)
 * 
 * Zeigt Rückläufigkeits-Perioden für einen Planeten an.
 */

#include <QDialog>
#include <QLabel>

namespace astro {

/**
 * @brief Rückläufigkeits-Dialog
 * 
 * Port von: DlgRuck aus legacy/autransi.c
 * Zeigt Von/Bis-Datum für eine Rückläufigkeits-Periode.
 */
class RetrogradeDialog : public QDialog {
    Q_OBJECT
public:
    explicit RetrogradeDialog(QWidget* parent = nullptr);
    
    /**
     * @brief Setzt die Von/Bis-Daten
     * @param von Start-Datum der Rückläufigkeit
     * @param bis Ende-Datum der Rückläufigkeit
     */
    void setDaten(const QString& von, const QString& bis);

private:
    QLabel* m_vonLabel;
    QLabel* m_bisLabel;
};

} // namespace astro
