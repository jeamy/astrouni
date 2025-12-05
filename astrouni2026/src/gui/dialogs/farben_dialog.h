#pragma once
#include <QDialog>
#include "../../core/data_types.h"
namespace astro {
class FarbenDialog : public QDialog {
    Q_OBJECT
public:
    explicit FarbenDialog(QWidget* parent = nullptr);
};
} // namespace astro
