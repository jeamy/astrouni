/**
 * @file transit_result_window.cpp
 * @brief Implementierung des Transit-Ergebnis-Fensters
 * 
 * 1:1 Port von WndTransit/TRANSITCLASS aus legacy/astrouni.c
 */

#include "transit_result_window.h"
#include "dialogs/retrograde_dialog.h"
#include "../core/constants.h"
#include "../core/astro_font_provider.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <cmath>

namespace astro {

TransitResultWindow::TransitResultWindow(QWidget* parent, AuInit& auinit, const Radix& basisRadix)
    : QWidget(parent)
    , m_auinit(auinit)
    , m_basisRadix(basisRadix) {
    
    setupUI();
}

TransitResultWindow::~TransitResultWindow() {
}

void TransitResultWindow::setupUI() {
    // STRICT LEGACY: Layout wie in WndTransit
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(8, 8, 8, 8);
    
    // Header: "Transite von [Datum] bis [Datum]" + Buttons
    auto* headerLayout = new QHBoxLayout();
    
    auto* vonLabel = new QLabel(tr("Transite von"), this);
    headerLayout->addWidget(vonLabel);
    
    m_vonDatumLabel = new QLabel("--.--.----", this);
    headerLayout->addWidget(m_vonDatumLabel);
    
    auto* bisLabel = new QLabel(tr("bis"), this);
    headerLayout->addWidget(bisLabel);
    
    m_bisDatumLabel = new QLabel("--.--.----", this);
    headerLayout->addWidget(m_bisDatumLabel);
    
    headerLayout->addStretch();
    
    // Grafik-Button (STRICT LEGACY: PB_UT_GRAF)
    m_grafikButton = new QPushButton(tr("Grafik"), this);
    connect(m_grafikButton, &QPushButton::clicked, this, &TransitResultWindow::onGrafikClicked);
    headerLayout->addWidget(m_grafikButton);
    
    // Rückläufigkeit-Button (STRICT LEGACY: PB_UT_RUCK)
    m_ruecklaufButton = new QPushButton(tr("Rückl."), this);
    connect(m_ruecklaufButton, &QPushButton::clicked, this, &TransitResultWindow::onRuecklaufClicked);
    headerLayout->addWidget(m_ruecklaufButton);
    
    mainLayout->addLayout(headerLayout);
    
    // Listbox (STRICT LEGACY: hWndLBG mit LBS_OWNERDRAWFIXED)
    m_listBox = new QListWidget(this);
    m_listBox->setMinimumHeight(200);
    connect(m_listBox, &QListWidget::itemDoubleClicked, 
            this, &TransitResultWindow::onListItemDoubleClicked);
    mainLayout->addWidget(m_listBox);
}

void TransitResultWindow::setTransits(const QVector<Radix>& transits,
                                      const QVector<TransitAspekt>& aspekte,
                                      const QString& vonDatum, const QString& bisDatum,
                                      const QVector<QVector<bool>>& transSel) {
    m_transits = transits;
    m_transSel = transSel;
    m_aspekte.clear();
    
    if (m_transSel.isEmpty()) {
        m_aspekte = aspekte;
    } else {
        int radixPlanets = m_basisRadix.planet.size();
        int radixCount = radixPlanets + MAX_HAUS;
        for (const auto& ta : aspekte) {
            int tIdx = ta.transitPlanet;
            int rIdx = ta.isHaus ? radixPlanets + ta.radixPlanet : ta.radixPlanet;
            if (tIdx >= 0 && tIdx < m_transSel.size() &&
                rIdx >= 0 && rIdx < m_transSel[tIdx].size() &&
                m_transSel[tIdx][rIdx]) {
                m_aspekte.push_back(ta);
            }
        }
    }
    
    m_vonDatumLabel->setText(vonDatum);
    m_bisDatumLabel->setText(bisDatum);
    
    buildTransitTexts();
}

QString TransitResultWindow::getTabTitle() const {
    QString name = m_basisRadix.rFix.vorname;
    if (!m_basisRadix.rFix.name.isEmpty()) {
        if (!name.isEmpty()) name += " ";
        name += m_basisRadix.rFix.name;
    }
    return tr("Transit: %1").arg(name.isEmpty() ? tr("Unbekannt") : name);
}

static int aspektToIndex(int aspekt) {
    switch (aspekt) {
        case KONJUNKTION: return 0;
        case HALBSEX: return 1;
        case SEXTIL: return 2;
        case QUADRATUR: return 3;
        case TRIGON: return 4;
        case QUINCUNX: return 5;
        case OPOSITION: return 6;
        default: return 0;
    }
}

void TransitResultWindow::buildTransitTexts() {
    // STRICT LEGACY: Port von sTextOut/DRW_LB_TXT_TRANSIT aus aulistbo.c
    // Format: [Transit-Planet][℞] in [Zeichen...] [Aspekt] [Radix-Planet/Haus] in [Zeichen] von [Datum] bis [Datum]
    
    m_listBox->clear();
    
    for (const auto& ta : m_aspekte) {
        if (ta.startIndex < 0 || ta.startIndex >= m_transits.size()) continue;
        int endIdx = qMin(ta.endIndex >= 0 ? ta.endIndex : ta.startIndex, m_transits.size() - 1);
        const Radix& trStart = m_transits.at(ta.startIndex);
        const Radix& trEnd   = m_transits.at(endIdx);
        
        // Transit-Planet mit Retrograde-Symbol
        QString transitPlanet = astroFont().planetSymbol(ta.transitPlanet);
        bool isRetro = (trStart.planetTyp.size() > ta.transitPlanet) && 
                       (trStart.planetTyp[ta.transitPlanet] & P_TYP_RUCK);
        if (isRetro) {
            transitPlanet += QString::fromUtf8("℞");  // Retrograde-Symbol
        }
        
        // Sternzeichen während der Periode sammeln (STRICT LEGACY: alle durchlaufenen Zeichen)
        QString transitZeichen;
        int8_t lastStz = -1;
        for (int i = ta.startIndex; i <= endIdx && i < m_transits.size(); ++i) {
            const Radix& tr = m_transits.at(i);
            if (tr.stzPlanet.size() > ta.transitPlanet) {
                int8_t stz = tr.stzPlanet[ta.transitPlanet];
                if (stz != lastStz && stz >= 0 && stz < 12) {
                    transitZeichen += astroFont().sternzeichenSymbol(stz);
                    lastStz = stz;
                }
            }
        }
        if (transitZeichen.isEmpty()) {
            transitZeichen = "?";
        }
        
        // Aspekt-Symbol
        QString aspekt = astroFont().aspektSymbol(aspektToIndex(ta.aspekt));
        
        // Radix-Planet oder Haus
        QString radixObj;
        QString radixZeichen;
        if (ta.isHaus) {
            // Haus (STRICT LEGACY: ⌂1 - ⌂12)
            radixObj = QString::fromUtf8("⌂%1").arg(ta.radixPlanet + 1);
            // Sternzeichen des Hauses
            if (m_basisRadix.stzHaus.size() > ta.radixPlanet) {
                int8_t stz = m_basisRadix.stzHaus[ta.radixPlanet];
                if (stz >= 0 && stz < 12) {
                    radixZeichen = astroFont().sternzeichenSymbol(stz);
                }
            }
        } else {
            // Planet
            radixObj = astroFont().planetSymbol(ta.radixPlanet);
            // Retrograde-Symbol für Radix-Planet
            if (m_basisRadix.planetTyp.size() > ta.radixPlanet &&
                (m_basisRadix.planetTyp[ta.radixPlanet] & P_TYP_RUCK)) {
                radixObj += QString::fromUtf8("℞");
            }
            // Sternzeichen des Radix-Planeten
            if (m_basisRadix.stzPlanet.size() > ta.radixPlanet) {
                int8_t stz = m_basisRadix.stzPlanet[ta.radixPlanet];
                if (stz >= 0 && stz < 12) {
                    radixZeichen = astroFont().sternzeichenSymbol(stz);
                }
            }
        }
        if (radixZeichen.isEmpty()) {
            radixZeichen = "?";
        }
        
        // Datum von/bis
        QString vonDatum = QString("%1.%2.%3")
            .arg(trStart.rFix.tag, 2, 10, QChar('0'))
            .arg(trStart.rFix.monat, 2, 10, QChar('0'))
            .arg(trStart.rFix.jahr, 4, 10, QChar('0'));
        
        QString bisDatum;
        if (endIdx >= 0 && endIdx < m_transits.size()) {
            bisDatum = QString("%1.%2.%3")
                .arg(trEnd.rFix.tag, 2, 10, QChar('0'))
                .arg(trEnd.rFix.monat, 2, 10, QChar('0'))
                .arg(trEnd.rFix.jahr, 4, 10, QChar('0'));
        } else {
            bisDatum = tr("N/A");
        }
        
        // STRICT LEGACY Format: [Planet][℞] in [Zeichen] [Aspekt] [Radix] in [Zeichen] von [Datum] bis [Datum]
        QString text = QString("%1 in %2 %3 %4 in %5 von %6 bis %7")
            .arg(transitPlanet, transitZeichen, aspekt, radixObj, radixZeichen, vonDatum, bisDatum);
        
        QListWidgetItem* item = new QListWidgetItem(text);
        if (astroFont().hasAstroFont()) {
            QFont f = item->font();
            f.setFamily(astroFont().fontName());
            item->setFont(f);
        }
        m_listBox->addItem(item);
    }
    
    if (m_listBox->count() == 0 && !m_transits.isEmpty()) {
        const Radix& tr = m_transits.first();
        QString date = QString("%1.%2.%3")
            .arg(tr.rFix.tag, 2, 10, QChar('0'))
            .arg(tr.rFix.monat, 2, 10, QChar('0'))
            .arg(tr.rFix.jahr, 4, 10, QChar('0'));
        QString time = QString("%1:%2")
            .arg(static_cast<int>(tr.rFix.zeit), 2, 10, QChar('0'))
            .arg(static_cast<int>(std::round((tr.rFix.zeit - static_cast<int>(tr.rFix.zeit)) * 60.0)), 2, 10, QChar('0'));
        QString text = QString("%1 %2 - Keine Aspekte gefunden").arg(date, time);
        QListWidgetItem* item = new QListWidgetItem(text);
        if (astroFont().hasAstroFont()) {
            QFont f = item->font();
            f.setFamily(astroFont().fontName());
            item->setFont(f);
        }
        m_listBox->addItem(item);
    }
}

void TransitResultWindow::onGrafikClicked() {
    // STRICT LEGACY: PB_UT_GRAF -> RadixCopy + PaintRadixWnd
    QListWidgetItem* item = m_listBox->currentItem();
    int row = item ? m_listBox->row(item) : 0;
    
    if (row >= 0 && row < m_aspekte.size()) {
        emit requestGraphic(m_aspekte.at(row).startIndex);
    } else if (!m_transits.isEmpty()) {
        emit requestGraphic(0);
    }
}

void TransitResultWindow::onRuecklaufClicked() {
    // STRICT LEGACY: PB_UT_RUCK -> sRucklauf
    QListWidgetItem* item = m_listBox->currentItem();
    if (!item) {
        QMessageBox::information(this, tr("Rückläufigkeit"), 
            tr("Bitte einen Eintrag auswählen."));
        return;
    }
    
    int row = m_listBox->row(item);
    if (row < 0 || row >= m_aspekte.size()) return;
    
    int planet = m_aspekte.at(row).transitPlanet;
    
    // Rückläufigkeits-Perioden suchen
    if (m_transits.isEmpty()) return;
    
    QString vonDatum = tr("N/A");
    QString bisDatum = tr("N/A");
    bool inRetrograde = false;
    bool foundAny = false;
    
    for (int i = 0; i < m_transits.size(); ++i) {
        const Radix& tr = m_transits.at(i);
        bool isRetro = (tr.planetTyp.size() > planet) && 
                       (tr.planetTyp[planet] & P_TYP_RUCK) != 0;
        
        if (isRetro && !inRetrograde) {
            vonDatum = QString("%1.%2.%3")
                .arg(tr.rFix.tag, 2, 10, QChar('0'))
                .arg(tr.rFix.monat, 2, 10, QChar('0'))
                .arg(tr.rFix.jahr, 4, 10, QChar('0'));
            inRetrograde = true;
        } else if (!isRetro && inRetrograde) {
            bisDatum = QString("%1.%2.%3")
                .arg(tr.rFix.tag, 2, 10, QChar('0'))
                .arg(tr.rFix.monat, 2, 10, QChar('0'))
                .arg(tr.rFix.jahr, 4, 10, QChar('0'));
            
            RetrogradeDialog dlg(this);
            dlg.setDaten(vonDatum, bisDatum);
            dlg.exec();
            
            foundAny = true;
            vonDatum = QObject::tr("N/A");
            bisDatum = QObject::tr("N/A");
            inRetrograde = false;
        }
    }
    
    if (inRetrograde) {
        RetrogradeDialog dlg(this);
        dlg.setDaten(vonDatum, bisDatum);
        dlg.exec();
        foundAny = true;
    }
    
    if (!foundAny) {
        QMessageBox::information(this, tr("Rückläufigkeit"),
            tr("Keine Rückläufigkeit für %1 im gewählten Zeitraum gefunden.")
                .arg(astroFont().planetSymbol(planet)));
    }
}

void TransitResultWindow::onListItemDoubleClicked(QListWidgetItem* item) {
    // STRICT LEGACY: Doppelklick -> Grafik anzeigen
    if (!item) return;
    
    int row = m_listBox->row(item);
    if (row >= 0 && row < m_aspekte.size()) {
        emit requestGraphic(m_aspekte.at(row).startIndex);
    }
}

} // namespace astro
