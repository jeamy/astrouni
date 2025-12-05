#include "orben_dialog.h"
#include <QVBoxLayout>
#include <QLabel>
namespace astro {
OrbenDialog::OrbenDialog(QWidget* parent) : QDialog(parent) {
    setWindowTitle(tr("Dialog"));
    auto* layout = new QVBoxLayout(this);
    layout->addWidget(new QLabel(tr("TODO: Implementieren"), this));
}
} // namespace astro
