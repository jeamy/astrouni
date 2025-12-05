#include "ort_dialog.h"
#include <QVBoxLayout>
#include <QLabel>
namespace astro {
OrtDialog::OrtDialog(QWidget* parent) : QDialog(parent) {
    setWindowTitle(tr("Ort Erfassen"));
    auto* layout = new QVBoxLayout(this);
    layout->addWidget(new QLabel(tr("TODO: Ort-Erfassung implementieren"), this));
}
} // namespace astro
