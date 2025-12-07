#pragma once
#include <QDialog>
#include "../../core/data_types.h"
namespace astro {
class HausDialog : public QDialog {
    Q_OBJECT
public:
    explicit HausDialog(QWidget* parent = nullptr);
};
} // namespace astro
