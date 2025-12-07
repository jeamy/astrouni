#pragma once
/**
 * @file ort_duplikat_dialog.h
 * @brief Dialog bei Orts-Duplikat (Port von DlgODProc)
 */

#include <QDialog>
#include <QListWidget>

namespace astro {

struct Orte;

/**
 * @brief Dialog zur Auswahl eines bereits vorhandenen Ortes
 */
class OrtDuplikatDialog : public QDialog {
    Q_OBJECT
public:
    OrtDuplikatDialog(QWidget* parent, const Orte& existing);
    bool useExisting() const { return m_useExisting; }

private slots:
    void onAccept();
    void onReject();

private:
    void setupUI(const Orte& existing);

    QListWidget* m_list;
    bool m_useExisting {false};
};

} // namespace astro
