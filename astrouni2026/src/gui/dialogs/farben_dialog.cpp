/**
 * @file farben_dialog.cpp
 * @brief Implementierung des Farben-Dialogs
 */

#include "farben_dialog.h"
#include "../../core/constants.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QColorDialog>

namespace astro {

static const char* PLANET_NAMES[] = {
    "Sonne", "Mond", "Merkur", "Venus", "Mars",
    "Jupiter", "Saturn", "Uranus", "Neptun", "Pluto"
};

static const char* ASPEKT_NAMES[] = {
    "Konjunktion", "Sextil", "Quadrat", "Trigon", "Opposition"
};

FarbenDialog::FarbenDialog(AuInit& auinit, QWidget* parent)
    : QDialog(parent)
    , m_auinit(auinit) {
    
    setWindowTitle(tr("Farben einstellen"));
    setModal(true);
    
    setupUI();
    loadSettings();
}

void FarbenDialog::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // Planeten-Farben
    QGroupBox* planetGroup = new QGroupBox(tr("Planeten"), this);
    QGridLayout* planetLayout = new QGridLayout(planetGroup);
    
    m_planetButtons.resize(10);
    m_planetColors.resize(10);
    
    for (int i = 0; i < 10; ++i) {
        QLabel* label = new QLabel(tr(PLANET_NAMES[i]), this);
        planetLayout->addWidget(label, i / 2, (i % 2) * 2);
        
        QPushButton* button = new QPushButton(this);
        button->setFixedSize(60, 24);
        button->setProperty("colorIndex", i);
        button->setProperty("colorType", "planet");
        connect(button, &QPushButton::clicked, this, &FarbenDialog::onColorButton);
        planetLayout->addWidget(button, i / 2, (i % 2) * 2 + 1);
        
        m_planetButtons[i] = button;
    }
    
    mainLayout->addWidget(planetGroup);
    
    // Aspekt-Farben
    QGroupBox* aspektGroup = new QGroupBox(tr("Aspekte"), this);
    QGridLayout* aspektLayout = new QGridLayout(aspektGroup);
    
    m_aspektButtons.resize(5);
    m_aspektColors.resize(5);
    
    for (int i = 0; i < 5; ++i) {
        QLabel* label = new QLabel(tr(ASPEKT_NAMES[i]), this);
        aspektLayout->addWidget(label, i, 0);
        
        QPushButton* button = new QPushButton(this);
        button->setFixedSize(60, 24);
        button->setProperty("colorIndex", i);
        button->setProperty("colorType", "aspekt");
        connect(button, &QPushButton::clicked, this, &FarbenDialog::onColorButton);
        aspektLayout->addWidget(button, i, 1);
        
        m_aspektButtons[i] = button;
    }
    
    mainLayout->addWidget(aspektGroup);
    
    // Buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    
    QPushButton* defaultButton = new QPushButton(tr("Standard"), this);
    connect(defaultButton, &QPushButton::clicked, this, &FarbenDialog::onDefault);
    buttonLayout->addWidget(defaultButton);
    
    buttonLayout->addStretch();
    
    QPushButton* okButton = new QPushButton(tr("OK"), this);
    okButton->setDefault(true);
    connect(okButton, &QPushButton::clicked, this, &FarbenDialog::onAccept);
    buttonLayout->addWidget(okButton);
    
    QPushButton* cancelButton = new QPushButton(tr("Abbrechen"), this);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    buttonLayout->addWidget(cancelButton);
    
    mainLayout->addLayout(buttonLayout);
}

void FarbenDialog::updateButtonColor(QPushButton* button, const QColor& color) {
    QString style = QString("background-color: %1; border: 1px solid black;").arg(color.name());
    button->setStyleSheet(style);
}

void FarbenDialog::loadSettings() {
    // Planeten-Farben aus AuInit laden
    for (int i = 0; i < 10 && i < MAX_PLANET; ++i) {
        m_planetColors[i] = m_auinit.planetColor[i];
        updateButtonColor(m_planetButtons[i], m_planetColors[i]);
    }
    
    // Aspekt-Farben aus AuInit laden
    for (int i = 0; i < 5 && i < MAX_ASPEKT; ++i) {
        m_aspektColors[i] = m_auinit.aspektColor[i];
        updateButtonColor(m_aspektButtons[i], m_aspektColors[i]);
    }
}

void FarbenDialog::saveSettings() {
    for (int i = 0; i < 10 && i < MAX_PLANET; ++i) {
        m_auinit.planetColor[i] = m_planetColors[i];
    }
    
    for (int i = 0; i < 5 && i < MAX_ASPEKT; ++i) {
        m_auinit.aspektColor[i] = m_aspektColors[i];
    }
}

void FarbenDialog::onColorButton() {
    QPushButton* button = qobject_cast<QPushButton*>(sender());
    if (!button) return;
    
    int index = button->property("colorIndex").toInt();
    QString type = button->property("colorType").toString();
    
    QColor currentColor;
    if (type == "planet") {
        currentColor = m_planetColors[index];
    } else {
        currentColor = m_aspektColors[index];
    }
    
    QColor newColor = QColorDialog::getColor(currentColor, this, tr("Farbe wählen"));
    
    if (newColor.isValid()) {
        if (type == "planet") {
            m_planetColors[index] = newColor;
            updateButtonColor(m_planetButtons[index], newColor);
        } else {
            m_aspektColors[index] = newColor;
            updateButtonColor(m_aspektButtons[index], newColor);
        }
    }
}

void FarbenDialog::onDefault() {
    // Standard-Farben setzen
    initDefaultColors();
    loadSettings();
}

void FarbenDialog::onAccept() {
    saveSettings();
    accept();
}

} // namespace astro
