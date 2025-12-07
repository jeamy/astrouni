#pragma once
/**
 * @file person_search_dialog.h
 * @brief Dialog zur Suche und Auswahl von Personen
 * 
 * Port von DlgGetName aus legacy/astrofil.c
 */

#include <QDialog>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include "../../core/data_types.h"

namespace astro {

/**
 * @brief Dialog zur Suche und Auswahl von Personen
 */
class PersonSearchDialog : public QDialog {
    Q_OBJECT
public:
    explicit PersonSearchDialog(QWidget* parent = nullptr);
    
    /**
     * @brief Gibt die ausgewählte Person zurück
     */
    RadixFix getSelectedPerson() const;
    
    /**
     * @brief Prüft ob eine Person ausgewählt wurde
     */
    bool hasSelection() const;
    
private slots:
    void onSearchTextChanged(const QString& text);
    void onItemDoubleClicked(QListWidgetItem* item);
    void onItemSelectionChanged();
    void onOkClicked();
    void onDeleteClicked();
    
private:
    void setupUI();
    void loadInitialPersons();
    void updateList(const QString& searchText);
    
    QLineEdit* m_searchEdit;
    QListWidget* m_listWidget;
    QPushButton* m_okButton;
    QPushButton* m_deleteButton;
    QPushButton* m_cancelButton;
    
    RadixFix m_selectedPerson;
    bool m_hasSelection;
};

} // namespace astro
