#include "trans_sel_dialog.h"

#include <QVBoxLayout>
#include <QGridLayout>
#include <QScrollArea>
#include <QLabel>
#include <QCheckBox>
#include <QDialogButtonBox>
#include <QHBoxLayout>
#include <QPushButton>
#include <QSignalBlocker>
#include "../../core/astro_font_provider.h"

namespace astro {

TransSelDialog::TransSelDialog(QWidget* parent)
    : QDialog(parent)
    , m_transitCount(0)
    , m_radixCount(0) {
    setWindowTitle(tr("Transite auswählen"));
    setModal(true);
    buildUI();
}

void TransSelDialog::buildUI() {
    auto* mainLayout = new QVBoxLayout(this);

    // Scroll-Offsets (Port von SB_HOR/SB_VER)
    QHBoxLayout* scrollLayout = new QHBoxLayout();
    scrollLayout->addWidget(new QLabel(tr("Spalte:"), this));
    m_hScroll = new QSpinBox(this);
    m_hScroll->setRange(0, MAX_PLANET + 5 - 11); // Legacy: MAX_PLANET+5 - XORB (XORB=11)
    connect(m_hScroll, QOverload<int>::of(&QSpinBox::valueChanged), this, [this](int v) {
        m_xOffset = v;
        updateChecks();
    });
    scrollLayout->addWidget(m_hScroll);

    scrollLayout->addSpacing(12);
    scrollLayout->addWidget(new QLabel(tr("Zeile:"), this));
    m_vScroll = new QSpinBox(this);
    m_vScroll->setRange(0, MAX_PLANET - 1);
    connect(m_vScroll, QOverload<int>::of(&QSpinBox::valueChanged), this, [this](int v) {
        m_yOffset = v;
        updateChecks();
    });
    scrollLayout->addWidget(m_vScroll);
    scrollLayout->addStretch();
    mainLayout->addLayout(scrollLayout);

    // Preset-Buttons (Port von PB_Default/PB_None/Haus/Aster)
    QHBoxLayout* presetLayout = new QHBoxLayout();
    m_allBtn = new QPushButton(tr("Alle"), this);
    connect(m_allBtn, &QPushButton::clicked, this, [this]() { applyPresetAll(true); updatePresetButtons(1); });
    presetLayout->addWidget(m_allBtn);
    m_noneBtn = new QPushButton(tr("Keine"), this);
    connect(m_noneBtn, &QPushButton::clicked, this, [this]() { applyPresetAll(false); updatePresetButtons(0); });
    presetLayout->addWidget(m_noneBtn);
    m_planetsBtn = new QPushButton(tr("Planeten"), this);
    connect(m_planetsBtn, &QPushButton::clicked, this, [this]() { applyPresetPlanets(true); updatePresetButtons(2); });
    presetLayout->addWidget(m_planetsBtn);
    m_housesBtn = new QPushButton(tr("Häuser"), this);
    connect(m_housesBtn, &QPushButton::clicked, this, [this]() { applyPresetHouses(true); updatePresetButtons(3); });
    presetLayout->addWidget(m_housesBtn);
    m_asterBtn = new QPushButton(tr("Aster."), this);
    connect(m_asterBtn, &QPushButton::clicked, this, [this]() { applyPresetAster(true); updatePresetButtons(4); });
    presetLayout->addWidget(m_asterBtn);
    presetLayout->addStretch();
    mainLayout->addLayout(presetLayout);

    // Scrollbarer Bereich mit Matrix
    auto* scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    QWidget* container = new QWidget(scrollArea);
    auto* grid = new QGridLayout(container);
    grid->setSpacing(4);

    // Platzhalter: wird in updateChecks() gefüllt
    scrollArea->setWidget(container);
    mainLayout->addWidget(scrollArea);

    // Orben-Label (vereinfachte Anzeige der Legacy-Orben)
    m_orbLabel = new QLabel(this);
    m_orbLabel->setWordWrap(true);
    mainLayout->addWidget(m_orbLabel);

    // Buttons OK/Cancel
    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    mainLayout->addWidget(buttons);

    // Store layouts for later rebuild
    container->setLayout(grid);
    setLayout(mainLayout);
}

void TransSelDialog::setSelection(const QVector<QVector<bool>>& sel) {
    m_sel = sel;
    m_transitCount = sel.size();
    m_radixCount = m_transitCount > 0 ? sel.first().size() : 0;
    updateChecks();
    updatePresetButtons(-1);
}

QVector<QVector<bool>> TransSelDialog::selection() const {
    QVector<QVector<bool>> result = m_sel;
    for (int i = 0; i < m_checks.size(); ++i) {
        for (int j = 0; j < m_checks[i].size(); ++j) {
            result[i][j] = m_checks[i][j]->isChecked();
        }
    }
    return result;
}

void TransSelDialog::setLabels(const QStringList& transitLabels, const QStringList& radixLabels) {
    m_transitLabels = transitLabels;
    m_radixLabels = radixLabels;
    updateChecks();
}

void TransSelDialog::setOrbs(const QVector<float>& orbenTPlanet, const QVector<float>& orbenTHaus) {
    m_orbenTPlanet = orbenTPlanet;
    m_orbenTHaus = orbenTHaus;
    if (m_orbLabel) {
        m_orbLabel->setText(formatOrbs());
    }
}

void TransSelDialog::updateChecks() {
    // Grenzen basierend auf Scroll-Offsets (Port von SB_HOR/SB_VER)
    const int horVisible = 11; // XORB
    const int startCol = qBound(0, m_xOffset, qMax(0, m_radixCount - 1));
    const int endCol = qMin(m_radixCount, startCol + horVisible);
    const int visCols = endCol - startCol;

    const int startRow = qBound(0, m_yOffset, qMax(0, m_transitCount - 1));
    const int endRow = m_transitCount; // zeilen komplett ab Offset
    const int visRows = endRow - startRow;

    // Suche Grid-Layout
    auto* scrollArea = findChild<QScrollArea*>();
    if (!scrollArea) return;
    QWidget* container = scrollArea->widget();
    auto* grid = qobject_cast<QGridLayout*>(container->layout());
    if (!grid) return;

    // Bestehende Widgets entfernen (sofort löschen, kein deleteLater → vermeidet Überlagerungen)
    while (QLayoutItem* item = grid->takeAt(0)) {
        if (QWidget* w = item->widget()) {
            delete w;
        }
        delete item;
    }
    m_checks.clear();
    m_rowHeaders.clear();
    m_colHeaders.clear();

    // Font für Symbole (nur falls AstroUniverse verfügbar)
    QFont symFont;
    bool haveAstroFont = AstroFontProvider::instance().hasAstroFont();
    if (haveAstroFont) {
        symFont.setFamily(AstroFontProvider::instance().fontName());
    }

    // Header Zeile: Radix-Targets
    grid->addWidget(new QLabel(tr("Transit \\ Radix"), container), 0, 0);
    m_colHeaders.resize(visCols);
    for (int c = 0; c < visCols; ++c) {
        int actualCol = startCol + c;
        QCheckBox* cb = new QCheckBox(container);
        QString text = (actualCol < m_radixLabels.size()) ? m_radixLabels.at(actualCol) : QString::number(actualCol + 1);
        cb->setText(text);
        if (haveAstroFont) cb->setFont(symFont);
        cb->setTristate(true);
        cb->setCheckState(Qt::Unchecked);
        connect(cb, &QCheckBox::stateChanged, this, [this, actualCol](int state) {
            toggleColumn(actualCol, static_cast<Qt::CheckState>(state));
        });
        grid->addWidget(cb, 0, c + 1);
        m_colHeaders[c] = cb;
    }

    // Datenzeilen
    m_checks.resize(visRows);
    for (int r = 0; r < visRows; ++r) {
        int actualRow = startRow + r;
        QCheckBox* rowCb = new QCheckBox(container);
        QString text = (actualRow < m_transitLabels.size()) ? m_transitLabels.at(actualRow) : QString::number(actualRow + 1);
        rowCb->setText(text);
        if (haveAstroFont) rowCb->setFont(symFont);
        rowCb->setTristate(true);
        rowCb->setCheckState(Qt::Unchecked);
        connect(rowCb, &QCheckBox::stateChanged, this, [this, actualRow](int state) {
            toggleRow(actualRow, static_cast<Qt::CheckState>(state));
        });
        grid->addWidget(rowCb, r + 1, 0);
        m_rowHeaders.push_back(rowCb);

        m_checks[r].resize(visCols);
        for (int c = 0; c < visCols; ++c) {
            int actualCol = startCol + c;
            QCheckBox* cb = new QCheckBox(container);
            bool checked = (actualRow < m_sel.size() && actualCol < m_sel[actualRow].size()) ? m_sel[actualRow][actualCol] : false;
            cb->setChecked(checked);
            grid->addWidget(cb, r + 1, c + 1);
            m_checks[r][c] = cb;
            connect(cb, &QCheckBox::stateChanged, this, [this, actualRow, actualCol](int) {
                if (actualRow < m_sel.size() && actualCol < m_sel[actualRow].size()) {
                    m_sel[actualRow][actualCol] = m_checks[actualRow - m_yOffset][actualCol - m_xOffset]->isChecked();
                }
                updateHeaderStates();
            });
        }
    }

    updateHeaderStates();
    if (m_orbLabel) {
        m_orbLabel->setText(formatOrbs());
    }
}

void TransSelDialog::toggleRow(int row, Qt::CheckState state) {
    if (row < 0 || row >= m_sel.size()) return;
    bool value = (state != Qt::Unchecked);
    for (int c = 0; c < m_sel[row].size(); ++c) {
        m_sel[row][c] = value;
    }
    updateHeaderStates();
}

void TransSelDialog::toggleColumn(int col, Qt::CheckState state) {
    if (col < 0) return;
    bool value = (state != Qt::Unchecked);
    for (int r = 0; r < m_sel.size(); ++r) {
        if (col < m_sel[r].size()) {
            m_sel[r][col] = value;
        }
    }
    updateHeaderStates();
}

void TransSelDialog::updateHeaderStates() {
    // Columns
    for (int c = 0; c < m_colHeaders.size(); ++c) {
        int actualCol = m_xOffset + c;
        int checked = 0;
        int total = 0;
        for (int r = 0; r < m_checks.size(); ++r) {
            int actualRow = m_yOffset + r;
            if (actualRow < m_sel.size() && actualCol < m_sel[actualRow].size()) {
                ++total;
                if (m_sel[actualRow][actualCol]) ++checked;
            }
        }
        if (total == 0) continue;
        QSignalBlocker blocker(m_colHeaders[c]);
        if (checked == 0) m_colHeaders[c]->setCheckState(Qt::Unchecked);
        else if (checked == total) m_colHeaders[c]->setCheckState(Qt::Checked);
        else m_colHeaders[c]->setCheckState(Qt::PartiallyChecked);
    }
    // Rows
    for (int r = 0; r < m_rowHeaders.size(); ++r) {
        int actualRow = m_yOffset + r;
        int checked = 0;
        int total = m_sel.value(actualRow).size();
        for (int c = 0; c < total; ++c) {
            if (m_sel[actualRow][c]) ++checked;
        }
        if (total == 0) continue;
        QSignalBlocker blocker(m_rowHeaders[r]);
        if (checked == 0) m_rowHeaders[r]->setCheckState(Qt::Unchecked);
        else if (checked == total) m_rowHeaders[r]->setCheckState(Qt::Checked);
        else m_rowHeaders[r]->setCheckState(Qt::PartiallyChecked);
    }
}

QString TransSelDialog::formatOrbs() const {
    QStringList parts;
    if (!m_orbenTPlanet.isEmpty()) {
        parts << tr("Orben Transit-Planeten: %1°").arg(m_orbenTPlanet.first(), 0, 'f', 1);
    }
    if (!m_orbenTHaus.isEmpty()) {
        parts << tr("Orben Transit-Häuser: %1°").arg(m_orbenTHaus.first(), 0, 'f', 1);
    }
    return parts.join(" | ");
}

void TransSelDialog::applyPresetAll(bool value) {
    for (auto& row : m_sel) {
        for (auto& cell : row) cell = value;
    }
    updateChecks();
}

void TransSelDialog::applyPresetPlanets(bool value) {
    int radixPlanets = m_radixLabels.isEmpty() ? m_radixCount - MAX_HAUS : m_radixLabels.size() - MAX_HAUS;
    if (radixPlanets < 0) radixPlanets = m_radixCount - MAX_HAUS;
    for (int r = 0; r < m_sel.size(); ++r) {
        for (int c = 0; c < m_sel[r].size(); ++c) {
            if (c < radixPlanets) m_sel[r][c] = value;
            else m_sel[r][c] = false;
        }
    }
    updateChecks();
}

void TransSelDialog::applyPresetHouses(bool value) {
    int radixPlanets = m_radixLabels.isEmpty() ? m_radixCount - MAX_HAUS : m_radixLabels.size() - MAX_HAUS;
    int start = radixPlanets;
    int end = qMin(m_radixCount, start + MAX_HAUS);
    for (int r = 0; r < m_sel.size(); ++r) {
        for (int c = 0; c < m_sel[r].size(); ++c) {
            if (c >= start && c < end) m_sel[r][c] = value;
            else m_sel[r][c] = false;
        }
    }
    updateChecks();
}

void TransSelDialog::applyPresetAster(bool value) {
    int start = m_radixCount - 1; // falls Platzhalter, sonst keine Wirkung
    for (int r = 0; r < m_sel.size(); ++r) {
        for (int c = 0; c < m_sel[r].size(); ++c) {
            if (c >= start) m_sel[r][c] = value;
            else m_sel[r][c] = false;
        }
    }
    updateChecks();
}

void TransSelDialog::updatePresetButtons(int mode) {
    auto setColor = [](QPushButton* btn, bool active) {
        if (!btn) return;
        btn->setStyleSheet(active ? "background-color: #5cb85c; color: white;" : "");
    };
    setColor(m_allBtn, mode == 1);
    setColor(m_noneBtn, mode == 0);
    setColor(m_planetsBtn, mode == 2);
    setColor(m_housesBtn, mode == 3);
    setColor(m_asterBtn, mode == 4);
}

} // namespace astro
