#pragma once
/**
 * @file transit_abort_dialog.h
 * @brief Port des Legacy DlgTransAbort (Abort-Dialog mit Von/Bis/Aktuell)
 */

#include <QDialog>
#include <QLabel>

namespace astro {

class TransitAbortDialog : public QDialog {
    Q_OBJECT
public:
    explicit TransitAbortDialog(QWidget* parent = nullptr);

    void setVon(const QString& text);
    void setBis(const QString& text);
    void setAkt(const QString& text);

private:
    QLabel* m_vonLabel;
    QLabel* m_bisLabel;
    QLabel* m_aktLabel;
};

} // namespace astro
