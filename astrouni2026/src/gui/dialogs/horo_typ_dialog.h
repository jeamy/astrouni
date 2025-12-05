#pragma once
#include <QDialog>
#include "../../core/data_types.h"
namespace astro {
class Horo_typDialog : public QDialog {
    Q_OBJECT
public:
    explicit Horo_typDialog(QWidget* parent = nullptr);
};
} // namespace astro
