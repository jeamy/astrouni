#pragma once
/**
 * @file ort_search_dialog.h
 * @brief Dialog zur Ort-Suche
 */

#include <QDialog>
#include <QLineEdit>
#include <QListWidget>
#include "../../core/data_types.h"

namespace astro {

class OrtSearchDialog : public QDialog {
    Q_OBJECT
    
public:
    explicit OrtSearchDialog(QWidget* parent = nullptr);
    Orte getSelectedOrt() const;
    
private slots:
    void onSearchTextChanged(const QString& text);
    void onItemDoubleClicked(QListWidgetItem* item);
    
private:
    void setupUI();
    void updateList();
    
    QLineEdit* m_searchEdit;
    QListWidget* m_listWidget;
    Orte m_selectedOrt;
};

} // namespace astro
