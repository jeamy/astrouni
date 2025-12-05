#pragma once
#include <QDialog>
#include "../../core/data_types.h"
namespace astro {
class OrbenDialog : public QDialog {
    Q_OBJECT
public:
    explicit OrbenDialog(QWidget* parent = nullptr);
};
} // namespace astro
