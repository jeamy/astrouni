#pragma once
#include <QDialog>
namespace astro {
class OrtDialog : public QDialog {
    Q_OBJECT
public:
    explicit OrtDialog(QWidget* parent = nullptr);
};
} // namespace astro
