#pragma once
/**
 * @file ort_dialog.h
 * @brief Ort-Erfassen Dialog
 * 
 * Port von DlgOErfassen aus legacy/astrofil.c
 */

#include <QDialog>
#include <QLineEdit>
#include <QDoubleSpinBox>
#include "../../core/data_types.h"

namespace astro {

class OrtDialog : public QDialog {
    Q_OBJECT
public:
    explicit OrtDialog(QWidget* parent = nullptr);
    
    // Ort zum Bearbeiten setzen
    void setOrt(const Orte& ort);
    Orte getOrt() const;
    
private slots:
    void onAccept();
    
private:
    void setupUI();
    bool validateInput();
    
    // UI-Elemente
    QLineEdit* m_nameEdit;
    QLineEdit* m_landEdit;
    QDoubleSpinBox* m_laengeEdit;
    QDoubleSpinBox* m_breiteEdit;
    QDoubleSpinBox* m_zoneEdit;
    
    Orte m_ort;
    bool m_isNew;
};

} // namespace astro
