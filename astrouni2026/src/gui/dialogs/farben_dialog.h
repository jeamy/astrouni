#pragma once
/**
 * @file farben_dialog.h
 * @brief Farben-Einstellungen Dialog
 * 
 * Port von DlgFarben aus legacy/astrofil.c
 */

#include <QDialog>
#include <QPushButton>
#include <QVector>
#include "../../core/data_types.h"

namespace astro {

class FarbenDialog : public QDialog {
    Q_OBJECT
public:
    explicit FarbenDialog(AuInit& auinit, QWidget* parent = nullptr);
    
private slots:
    void onColorButton();
    void onAccept();
    void onDefault();
    
private:
    void setupUI();
    void loadSettings();
    void saveSettings();
    void updateButtonColor(QPushButton* button, const QColor& color);
    
    AuInit& m_auinit;
    
    // Farb-Buttons für Planeten
    QVector<QPushButton*> m_planetButtons;
    QVector<QColor> m_planetColors;
    
    // Farb-Buttons für Aspekte
    QVector<QPushButton*> m_aspektButtons;
    QVector<QColor> m_aspektColors;
};

} // namespace astro
