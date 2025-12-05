#pragma once
#include <QDialog>
#include "../../core/data_types.h"
namespace astro {
class SettingsDialog : public QDialog {
    Q_OBJECT
public:
    explicit SettingsDialog(QWidget* parent = nullptr);
};
} // namespace astro
