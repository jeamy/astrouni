/**
 * @file radix_window.cpp
 * @brief Implementierung des Radix-Widgets (als Tab im Hauptfenster)
 */

#include "radix_window.h"
#include "chart_widget.h"
#include "html_item_delegate.h"
#include "dialogs/person_dialog.h"
#include "../core/calculations.h"
#include "../core/chart_calc.h"
#include "../core/astro_font_provider.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSplitter>

namespace astro {

//==============================================================================
// Konstruktor / Destruktor
//==============================================================================

RadixWindow::RadixWindow(QWidget* parent, const AuInit& auinit, const Radix& radix)
    : QWidget(parent)
    , m_auinit(auinit)   // Kopie erstellen
    , m_radix(radix)     // Kopie erstellen
    , m_listMode(Daten)
    , m_showPerson2(false)  // STRICT LEGACY: LB_G initial auf Person 1
    , m_showSynastrieAspects(false)  // STRICT LEGACY: LB_A initial auf Radix-Aspekte
    , m_updatingSpinBoxes(false) {
    
    setupUI();
    
    // Initiale Anzeige
    updateDisplay();
    updateTimeSpinBoxes();
}

RadixWindow::~RadixWindow() {
}

//==============================================================================
// Tab-Titel
//==============================================================================

QString RadixWindow::getTabTitle() const {
    // Häusersystem-Namen
    static const char* hausTypen[] = {
        "Koch", "Placidus", "Equal", "Equal(MC)", "Whole",
        "Topo", "Campanus", "Meridian", "Regio",
        "Porphyry", "Neo-Porph", "Morinus", "Alcab", "Null"
    };
    
    QString name = m_radix.rFix.vorname;
    if (!m_radix.rFix.name.isEmpty()) {
        if (!name.isEmpty()) name += " ";
        name += m_radix.rFix.name;
    }
    if (name.isEmpty()) name = tr("Radix");
    
    // STRICT LEGACY: Bei Synastrie beide Namen anzeigen
    if (m_radix.horoTyp == TYP_SYNASTRIE && m_radix.synastrie) {
        QString name2 = m_radix.synastrie->rFix.vorname;
        if (!m_radix.synastrie->rFix.name.isEmpty()) {
            if (!name2.isEmpty()) name2 += " ";
            name2 += m_radix.synastrie->rFix.name;
        }
        name = tr("Synastrie: %1 / %2").arg(name).arg(name2);
    } else if (m_radix.horoTyp == TYP_TRANSIT && m_radix.synastrie) {
        // STRICT LEGACY: Bei Transit Name + Transit-Datum anzeigen
        QString transitDatum = QString("%1.%2.%3")
            .arg(m_radix.synastrie->rFix.tag, 2, 10, QChar('0'))
            .arg(m_radix.synastrie->rFix.monat, 2, 10, QChar('0'))
            .arg(m_radix.synastrie->rFix.jahr, 4, 10, QChar('0'));
        name = tr("Transit: %1 (%2)").arg(name).arg(transitDatum);
    } else {
        // Häusersystem anhängen
        int hausIdx = m_radix.hausSys;
        if (hausIdx >= 0 && hausIdx < 14) {
            name += QString(" (%1)").arg(hausTypen[hausIdx]);
        }
    }
    
    return name;
}

//==============================================================================
// UI-Setup
//==============================================================================

void RadixWindow::setupUI() {
    QHBoxLayout* mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    
    // Splitter für Chart und Liste
    QSplitter* splitter = new QSplitter(Qt::Horizontal, this);
    mainLayout->addWidget(splitter);
    
    // Chart-Widget
    m_chartWidget = new ChartWidget(splitter);
    m_chartWidget->setSettings(m_auinit);
    // Maus-Signale verbinden (Planet/Haus-Klick)
    connect(m_chartWidget, &ChartWidget::planetClicked, this, &RadixWindow::onPlanetClicked);
    connect(m_chartWidget, &ChartWidget::houseClicked, this, &RadixWindow::onHouseClicked);
    connect(m_chartWidget, &ChartWidget::aspectClicked, this, &RadixWindow::onAspectClicked);
    splitter->addWidget(m_chartWidget);
    
    // Rechte Seite: Liste und Buttons
    QWidget* rightWidget = new QWidget(splitter);
    QVBoxLayout* rightLayout = new QVBoxLayout(rightWidget);
    
    // Listbox (Port von LB_DATEN, LB_POSITION, LB_ASPEKTE)
    m_listWidget = new QListWidget(rightWidget);
    m_listWidget->setMinimumWidth(200);
    m_listWidget->setItemDelegate(new HtmlItemDelegate(m_listWidget));
    // STRICT LEGACY: Listen-Hintergrund weiß, nur Einträge grau (per Item)
    // Der Hintergrund der Liste bleibt weiß, jedes Item bekommt grauen Hintergrund
    connect(m_listWidget, &QListWidget::itemClicked, 
            this, &RadixWindow::onListItemClicked);
    rightLayout->addWidget(m_listWidget);
    
    // Buttons (Port von PB_DATEN, PB_POSITION, PB_ASPEKTE)
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    
    m_datenButton = new QPushButton(tr("Daten"), rightWidget);
    connect(m_datenButton, &QPushButton::clicked, this, &RadixWindow::onDatenClicked);
    buttonLayout->addWidget(m_datenButton);
    
    m_positionenButton = new QPushButton(tr("Positionen"), rightWidget);
    connect(m_positionenButton, &QPushButton::clicked, this, &RadixWindow::onPositionenClicked);
    buttonLayout->addWidget(m_positionenButton);
    
    m_aspekteButton = new QPushButton(tr("Aspekte"), rightWidget);
    connect(m_aspekteButton, &QPushButton::clicked, this, &RadixWindow::onAspekteClicked);
    buttonLayout->addWidget(m_aspekteButton);
    
    rightLayout->addLayout(buttonLayout);
    
    // STRICT LEGACY: Zeit-Scroll Bereich (Port von SC_MIN, SC_STU, SC_TAG, SC_MON, SC_JAH)
    QWidget* timeScrollWidget = new QWidget(rightWidget);
    QVBoxLayout* timeScrollLayout = new QVBoxLayout(timeScrollWidget);
    timeScrollLayout->setContentsMargins(0, 5, 0, 0);
    timeScrollLayout->setSpacing(2);
    
    // Header-Zeile mit Labels
    QHBoxLayout* headerLayout = new QHBoxLayout();
    headerLayout->addWidget(new QLabel(tr("Min"), timeScrollWidget));
    headerLayout->addWidget(new QLabel(tr("Stunde"), timeScrollWidget));
    headerLayout->addWidget(new QLabel(tr("Tag"), timeScrollWidget));
    headerLayout->addWidget(new QLabel(tr("Monat"), timeScrollWidget));
    headerLayout->addWidget(new QLabel(tr("Jahr"), timeScrollWidget));
    timeScrollLayout->addLayout(headerLayout);
    
    // SpinBox-Zeile mit "Neu" Button
    QHBoxLayout* spinBoxLayout = new QHBoxLayout();
    
    m_neuButton = new QPushButton(tr("Neu"), timeScrollWidget);
    m_neuButton->setMaximumWidth(40);
    connect(m_neuButton, &QPushButton::clicked, this, &RadixWindow::onNeuClicked);
    spinBoxLayout->addWidget(m_neuButton);
    
    m_minuteSpinBox = new QSpinBox(timeScrollWidget);
    m_minuteSpinBox->setRange(0, 59);
    m_minuteSpinBox->setWrapping(true);
    connect(m_minuteSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), 
            this, &RadixWindow::onMinuteChanged);
    spinBoxLayout->addWidget(m_minuteSpinBox);
    
    m_hourSpinBox = new QSpinBox(timeScrollWidget);
    m_hourSpinBox->setRange(0, 23);
    m_hourSpinBox->setWrapping(true);
    connect(m_hourSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), 
            this, &RadixWindow::onHourChanged);
    spinBoxLayout->addWidget(m_hourSpinBox);
    
    m_daySpinBox = new QSpinBox(timeScrollWidget);
    m_daySpinBox->setRange(1, 31);
    m_daySpinBox->setWrapping(true);
    connect(m_daySpinBox, QOverload<int>::of(&QSpinBox::valueChanged), 
            this, &RadixWindow::onDayChanged);
    spinBoxLayout->addWidget(m_daySpinBox);
    
    m_monthSpinBox = new QSpinBox(timeScrollWidget);
    m_monthSpinBox->setRange(1, 12);
    m_monthSpinBox->setWrapping(true);
    connect(m_monthSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), 
            this, &RadixWindow::onMonthChanged);
    spinBoxLayout->addWidget(m_monthSpinBox);
    
    m_yearSpinBox = new QSpinBox(timeScrollWidget);
    m_yearSpinBox->setRange(1600, 2400);
    connect(m_yearSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), 
            this, &RadixWindow::onYearChanged);
    spinBoxLayout->addWidget(m_yearSpinBox);
    
    timeScrollLayout->addLayout(spinBoxLayout);
    rightLayout->addWidget(timeScrollWidget);
    
    splitter->addWidget(rightWidget);
    
    // Splitter-Proportionen
    splitter->setStretchFactor(0, 3);  // Chart größer
    splitter->setStretchFactor(1, 1);  // Liste kleiner
}

//==============================================================================
// Setter
//==============================================================================

void RadixWindow::setRadix(const Radix& radix) {
    m_radix = radix;
    updateDisplay();
}

void RadixWindow::setTransitSelection(const QVector<QVector<bool>>& sel) {
    m_transitSelection = sel;
    if (m_chartWidget) {
        m_chartWidget->setTransitSelection(sel);
        m_chartWidget->update();
    }
}
void RadixWindow::updateDisplay() {
    // Chart aktualisieren
    m_chartWidget->setRadix(m_radix);
    
    // Synastrie/Transit: Zweiten Radix setzen
    if (m_radix.synastrie) {
        m_chartWidget->setTransitRadix(m_radix.synastrie.get());
        // STRICT LEGACY: Bei Transit/Synastrie standardmäßig Transit-Aspekte anzeigen
        m_showSynastrieAspects = true;
        m_chartWidget->setShowSynastrieAspects(true);
        if (!m_transitSelection.isEmpty()) {
            m_chartWidget->setTransitSelection(m_transitSelection);
        }
    } else {
        m_chartWidget->setTransitRadix(nullptr);
        m_showSynastrieAspects = false;
        m_chartWidget->setShowSynastrieAspects(false);
    }
    
    // Liste aktualisieren
    switch (m_listMode) {
        case Daten:
            fillDatenList();
            break;
        case Positionen:
            fillPositionenList();
            break;
        case Aspekte:
            fillAspekteList();
            break;
    }
    
    // Fenster-Titel
    QString title = tr("AstroUniverse - ");
    if (!m_radix.rFix.name.isEmpty()) {
        title += m_radix.rFix.vorname + " " + m_radix.rFix.name;
    } else {
        title += tr("Radix");
    }
    setWindowTitle(title);
}

//==============================================================================
// Listen füllen
//==============================================================================

void RadixWindow::fillDatenList() {
    m_listWidget->clear();
    
    // Hilfsfunktion für grauen Hintergrund
    auto addGrayItem = [this](const QString& text) {
        QListWidgetItem* item = new QListWidgetItem(text);
        item->setBackground(sColor[COL_LBREC_NORM]);  // Grauer Hintergrund
        m_listWidget->addItem(item);
    };
    
    // Personen-Daten
    addGrayItem(tr("*Radix*"));
    addGrayItem(QString("Name: %1 %2")
        .arg(m_radix.rFix.vorname, m_radix.rFix.name));
    addGrayItem(QString("Geburtsdatum: %1.%2.%3")
        .arg(m_radix.rFix.tag, 2, 10, QChar('0'))
        .arg(m_radix.rFix.monat, 2, 10, QChar('0'))
        .arg(m_radix.rFix.jahr));
    
    int16_t stunden, minuten;
    Calculations::decimalToTime(m_radix.rFix.zeit, stunden, minuten);
    addGrayItem(QString("Geburtszeit: %1:%2")
        .arg(stunden, 2, 10, QChar('0'))
        .arg(minuten, 2, 10, QChar('0')));
    
    addGrayItem(QString("Geburtsort: %1").arg(m_radix.rFix.ort));
    addGrayItem(QString("Land: %1").arg(m_radix.rFix.land));
    addGrayItem(QString("Länge: %1").arg(m_radix.rFix.laenge, 0, 'f', 4));
    addGrayItem(QString("Breite: %1").arg(m_radix.rFix.breite, 0, 'f', 4));
    addGrayItem(QString("Zeitzone: %1").arg(m_radix.rFix.zone, 0, 'f', 1));
    
    // Häusersystem
    static const char* hausTypen[] = {
        "Koch", "Placidus", "Equal", "Equal (MC)", "Whole Sign",
        "Topocentric", "Campanus", "Meridian", "Regiomontanus",
        "Porphyry", "Neo-Porphyry", "Morinus", "Alcabitius", "Null"
    };
    addGrayItem(QString("Häusersystem: %1")
        .arg(hausTypen[m_radix.hausSys]));
    
    // STRICT LEGACY: Bei Synastrie auch Person 2 anzeigen (DRW_LBD_P2)
    if (m_radix.horoTyp == TYP_SYNASTRIE && m_radix.synastrie) {
        const Radix& syn = *m_radix.synastrie;
        
        addGrayItem(tr("*Person 2*"));
        addGrayItem(QString("Name: %1 %2")
            .arg(syn.rFix.vorname, syn.rFix.name));
        addGrayItem(QString("Geburtsdatum: %1.%2.%3")
            .arg(syn.rFix.tag, 2, 10, QChar('0'))
            .arg(syn.rFix.monat, 2, 10, QChar('0'))
            .arg(syn.rFix.jahr));
        
        int16_t stunden2, minuten2;
        Calculations::decimalToTime(syn.rFix.zeit, stunden2, minuten2);
        addGrayItem(QString("Geburtszeit: %1:%2")
            .arg(stunden2, 2, 10, QChar('0'))
            .arg(minuten2, 2, 10, QChar('0')));
        
        addGrayItem(QString("Geburtsort: %1").arg(syn.rFix.ort));
        addGrayItem(QString("Land: %1").arg(syn.rFix.land));
        addGrayItem(QString("Länge: %1").arg(syn.rFix.laenge, 0, 'f', 4));
        addGrayItem(QString("Breite: %1").arg(syn.rFix.breite, 0, 'f', 4));
        addGrayItem(QString("Zeitzone: %1").arg(syn.rFix.zone, 0, 'f', 1));
    }
}

//==============================================================================
// Maus-Slots aus ChartWidget
//==============================================================================

void RadixWindow::onPlanetClicked(int planetIndex, bool isTransit) {
    // STRICT LEGACY: Bei Klick in Grafik immer Positionen-Liste aktivieren
    m_listMode = Positionen;
    if (m_radix.horoTyp == TYP_SYNASTRIE) {
        m_showPerson2 = isTransit;  // Transit-Flag entspricht Person 2 bei Synastrie
    }
    updateDisplay();
    
    // Ziel-UserRole-Wert ermitteln
    int target = -1;
    if (isTransit) {
        target = (m_radix.horoTyp == TYP_SYNASTRIE) ? planetIndex + 200 : planetIndex + 100;
    } else {
        target = m_showPerson2 ? planetIndex + 200 : planetIndex;
    }
    
    // Eintrag selektieren
    for (int i = 0; i < m_listWidget->count(); ++i) {
        QListWidgetItem* it = m_listWidget->item(i);
        if (it->data(Qt::UserRole).toInt() == target) {
            m_listWidget->setCurrentItem(it);
            m_listWidget->scrollToItem(it, QAbstractItemView::PositionAtCenter);
            break;
        }
    }
    
    // Hervorhebung im Chart
    if (!isTransit) {
        m_chartWidget->highlightPlanet(planetIndex);
    } else {
        m_chartWidget->highlightPlanet(-1);
        m_chartWidget->highlightTransitAspect(-1, -1);
    }
}

void RadixWindow::onHouseClicked(int houseIndex) {
    Q_UNUSED(houseIndex);
    // Häuser nur in Person 1 anzeigen (Legacy-Verhalten)
    m_listMode = Positionen;
    m_showPerson2 = false;
    updateDisplay();
    m_chartWidget->highlightPlanet(-1);
    m_chartWidget->highlightAspect(-1, -1);
}

void RadixWindow::onAspectClicked(int idx1, int idx2, bool isTransit) {
    m_listMode = Aspekte;
    m_showSynastrieAspects = isTransit;
    updateDisplay();
    
    // Ziel-UserRole Wert berechnen
    int targetOffset = 0;
    if (isTransit) {
        targetOffset = (m_radix.horoTyp == TYP_SYNASTRIE) ? 2000 : 1000;
    }
    QPoint target(idx1 + targetOffset, idx2);
    
    for (int i = 0; i < m_listWidget->count(); ++i) {
        QListWidgetItem* it = m_listWidget->item(i);
        QVariant v = it->data(Qt::UserRole);
        if (v.canConvert<QPoint>() && v.toPoint() == target) {
            m_listWidget->setCurrentItem(it);
            m_listWidget->scrollToItem(it, QAbstractItemView::PositionAtCenter);
            break;
        }
    }
    
    if (isTransit) {
        m_chartWidget->highlightTransitAspect(idx1, idx2);
    } else {
        m_chartWidget->highlightAspect(idx1, idx2);
    }
}

// Hilfsfunktion: QColor zu HTML-Farbstring
static QString colorToHtml(const QColor& color) {
    return QString("#%1%2%3")
        .arg(color.red(), 2, 16, QChar('0'))
        .arg(color.green(), 2, 16, QChar('0'))
        .arg(color.blue(), 2, 16, QChar('0'));
}

void RadixWindow::fillPositionenList() {
    m_listWidget->clear();
    
    // STRICT LEGACY: Bei Synastrie Toggle zwischen Person 1 und Person 2 (LB_G Flag)
    const Radix* displayRadix = &m_radix;
    QString personHeader;
    bool isSynastrie = (m_radix.horoTyp == TYP_SYNASTRIE && m_radix.synastrie);
    bool isTransit = (m_radix.horoTyp == TYP_TRANSIT && m_radix.synastrie);
    
    if (isSynastrie) {
        if (m_showPerson2) {
            displayRadix = m_radix.synastrie.get();
            QString name2 = displayRadix->rFix.vorname;
            if (!displayRadix->rFix.name.isEmpty()) {
                if (!name2.isEmpty()) name2 += " ";
                name2 += displayRadix->rFix.name;
            }
            personHeader = tr("-Person 2: %1-").arg(name2);
        } else {
            QString name1 = m_radix.rFix.vorname;
            if (!m_radix.rFix.name.isEmpty()) {
                if (!name1.isEmpty()) name1 += " ";
                name1 += m_radix.rFix.name;
            }
            personHeader = tr("-Person 1: %1-").arg(name1);
        }
        
        // Person-Header (klickbar zum Umschalten)
        QListWidgetItem* synHeader = new QListWidgetItem(
            QString("<span style='color:#FFFFFF'>%1</span>").arg(personHeader));
        synHeader->setBackground(m_showPerson2 ? QColor(0, 128, 0) : QColor(0, 0, 128));
        synHeader->setData(Qt::UserRole, -999);  // Marker für Toggle
        m_listWidget->addItem(synHeader);
    }
    
    // Bei Transit: Transit-Planeten ZUERST anzeigen
    if (isTransit) {
        const Radix& transit = *m_radix.synastrie;
        
        // Transit-Header
        QString transitDatum = QString("%1.%2.%3")
            .arg(transit.rFix.tag, 2, 10, QChar('0'))
            .arg(transit.rFix.monat, 2, 10, QChar('0'))
            .arg(transit.rFix.jahr, 4, 10, QChar('0'));
        QListWidgetItem* transitHeader = new QListWidgetItem(
            QString("<span style='color:#FFFFFF'>") + tr("-Transit %1-").arg(transitDatum) + "</span>");
        transitHeader->setBackground(QColor(128, 0, 0));  // Dunkelrot für Transit
        m_listWidget->addItem(transitHeader);
        
        // Transit-Planeten
        for (int i = 0; i < transit.anzahlPlanet && i < transit.planet.size(); ++i) {
            QString symbol = astroFont().planetSymbol(i);
            QString pos = Calculations::degToZeichenString(transit.planet[i], true);
            int stz = static_cast<int>(transit.planet[i] / 30.0) % 12;
            QString stzSymbol = astroFont().sternzeichenSymbol(stz);
            QString retro = (transit.planetTyp.size() > i && (transit.planetTyp[i] & P_TYP_RUCK)) ? " R" : "";
            
            QString planetColor = colorToHtml(sColor[COL_PLAN_T]);
            int element = stz % 4;
            QString stzColor = colorToHtml(sColor[COL_FEUER + element]);
            
            QString html = QString("<span style='color:%1'>%2</span> %3 <span style='color:%4'>%5</span>%6")
                .arg(planetColor).arg(symbol).arg(pos).arg(stzColor).arg(stzSymbol).arg(retro);
            
            QListWidgetItem* item = new QListWidgetItem(html);
            item->setData(Qt::UserRole, i + 100);  // Transit-Planeten ab 100
            item->setBackground(sColor[COL_LBREC_NORM]);
            m_listWidget->addItem(item);
        }
    }
    
    // STRICT LEGACY: Planeten-Header wie im Original (weiße Schrift auf dunkelblau)
    QListWidgetItem* headerItem = new QListWidgetItem("<span style='color:#FFFFFF'>" + tr("-Planeten-") + "</span>");
    headerItem->setBackground(QColor(0, 0, 128));  // Dunkelblau wie Legacy
    m_listWidget->addItem(headerItem);
    
    // STRICT LEGACY: Format mit verschiedenen Farben für Symbol, Position, Zeichen
    for (int i = 0; i < displayRadix->anzahlPlanet; ++i) {
        QString symbol = astroFont().planetSymbol(i);
        
        // Position im Sternzeichen (0-30°)
        QString pos = Calculations::degToZeichenString(displayRadix->planet[i], true);
        
        // Sternzeichen-Symbol und Farbe
        int stz = static_cast<int>(displayRadix->planet[i] / 30.0) % 12;
        QString stzSymbol = astroFont().sternzeichenSymbol(stz);
        
        // Rückläufigkeit
        QString retro = (displayRadix->planetTyp.size() > i && (displayRadix->planetTyp[i] & P_TYP_RUCK)) ? " R" : "";
        
        // Farben - Sternzeichen in Element-Farbe
        bool isSecondPerson = isSynastrie && m_showPerson2;
        QString planetColor = colorToHtml(isSecondPerson ? sColor[COL_PLAN_T] : sColor[COL_PLAN]);
        int element = stz % 4;  // 0=Feuer, 1=Erde, 2=Luft, 3=Wasser
        QString stzColor = colorToHtml(sColor[COL_FEUER + element]);
        
        // HTML-formatierter Text
        QString html = QString("<span style='color:%1'>%2</span> %3 <span style='color:%4'>%5</span>%6")
            .arg(planetColor)
            .arg(symbol)
            .arg(pos)
            .arg(stzColor)
            .arg(stzSymbol)
            .arg(retro);
        
        QListWidgetItem* item = new QListWidgetItem(html);
        item->setData(Qt::UserRole, m_showPerson2 ? i + 200 : i);
        item->setBackground(sColor[COL_LBREC_NORM]);  // Grauer Hintergrund pro Eintrag
        m_listWidget->addItem(item);
    }
    
    // STRICT LEGACY: Häuser-Header wie im Original (weiße Schrift auf dunkelblau)
    // Häuser nur bei Person 1 oder wenn nicht Synastrie
    if (!isSynastrie || !m_showPerson2) {
        QListWidgetItem* hausHeader = new QListWidgetItem("<span style='color:#FFFFFF'>" + tr("-Häuser-") + "</span>");
        hausHeader->setBackground(QColor(0, 0, 128));  // Dunkelblau wie Legacy
        m_listWidget->addItem(hausHeader);
        
        // STRICT LEGACY: Format mit ASC, IC, DSC, MC Beschriftung wie im Original
        for (int i = 0; i < MAX_HAUS; ++i) {
            // Position im Sternzeichen (0-30°)
            QString pos = Calculations::degToZeichenString(displayRadix->haus[i], true);
            
            // Sternzeichen-Symbol
            int stz = static_cast<int>(displayRadix->haus[i] / 30.0) % 12;
            QString stzSymbol = QString::fromUtf8(STERNZEICHEN_SYMBOLS[stz]);
            
            // Farben - Sternzeichen in Element-Farbe
            QString hausColor = colorToHtml(sColor[COL_HAUSER]);
            int element = stz % 4;  // 0=Feuer, 1=Erde, 2=Luft, 3=Wasser
            QString stzColor = colorToHtml(sColor[COL_FEUER + element]);
            
            // STRICT LEGACY: Haus-Bezeichnung (ASC, IC, DSC, MC oder Nummer)
            QString hausLabel;
            switch (i) {
                case 0:  hausLabel = "ASC"; break;
                case 3:  hausLabel = "IC"; break;
                case 6:  hausLabel = "DSC"; break;
                case 9:  hausLabel = "MC"; break;
                default: hausLabel = QString("◇%1").arg(i + 1, 2, 10, QChar('0')); break;
            }
            
            // HTML-formatierter Text
            QString html = QString("<span style='color:%1'>%2</span> %3 <span style='color:%4'>%5</span>")
                .arg(hausColor)
                .arg(hausLabel)
                .arg(pos)
                .arg(stzColor)
                .arg(stzSymbol);
            
            QListWidgetItem* item = new QListWidgetItem(html);
            item->setBackground(sColor[COL_LBREC_NORM]);  // Grauer Hintergrund pro Eintrag
            m_listWidget->addItem(item);
        }
    }
    // Transit-Planeten werden jetzt am Anfang der Liste angezeigt (siehe oben)
}

void RadixWindow::fillAspekteList() {
    m_listWidget->clear();
    
    // Aspekt-Index zu Farb-Index Mapping
    auto getAspektColorIndex = [](int asp) -> int {
        switch (asp) {
            case KONJUNKTION: return CKONJUNKTION;
            case HALBSEX:     return CHALBSEX;
            case SEXTIL:      return CSEXTIL;
            case QUADRATUR:   return CQUADRATUR;
            case TRIGON:      return CTRIGON;
            case QUINCUNX:    return CQUINCUNX;
            case OPOSITION:   return COPOSITION;
            default:          return COL_TXT;
        }
    };
    
    // Aspekt-Index zu Symbol-Index Mapping
    auto getAspektSymbolIndex = [](int asp) -> int {
        switch (asp) {
            case KONJUNKTION: return 0;
            case HALBSEX:     return 1;
            case SEXTIL:      return 2;
            case QUADRATUR:   return 3;
            case TRIGON:      return 4;
            case QUINCUNX:    return 5;
            case OPOSITION:   return 6;
            default:          return 0;
        }
    };
    
    int numPlanets = m_radix.anzahlPlanet;
    bool hasSynastrie = (m_radix.horoTyp == TYP_SYNASTRIE || m_radix.horoTyp == TYP_TRANSIT) && m_radix.synastrie;
    
    // STRICT LEGACY: Bei Synastrie/Transit Toggle-Header anzeigen (LB_A Flag)
    if (hasSynastrie) {
        QString headerText;
        if (m_showSynastrieAspects) {
            if (m_radix.horoTyp == TYP_SYNASTRIE) {
                QString name2 = m_radix.synastrie->rFix.vorname;
                if (!m_radix.synastrie->rFix.name.isEmpty()) {
                    if (!name2.isEmpty()) name2 += " ";
                    name2 += m_radix.synastrie->rFix.name;
                }
                headerText = tr("-Synastrie: %1-").arg(name2);
            } else {
                QString transitDatum = QString("%1.%2.%3")
                    .arg(m_radix.synastrie->rFix.tag, 2, 10, QChar('0'))
                    .arg(m_radix.synastrie->rFix.monat, 2, 10, QChar('0'))
                    .arg(m_radix.synastrie->rFix.jahr, 4, 10, QChar('0'));
                headerText = tr("-Transit %1-").arg(transitDatum);
            }
        } else {
            headerText = tr("-Radix Aspekte-");
        }
        
        QListWidgetItem* toggleHeader = new QListWidgetItem(
            QString("<span style='color:#FFFFFF'>%1</span>").arg(headerText));
        toggleHeader->setBackground(m_showSynastrieAspects ? 
            (m_radix.horoTyp == TYP_SYNASTRIE ? QColor(0, 128, 0) : QColor(128, 0, 0)) : 
            QColor(0, 0, 128));
        toggleHeader->setData(Qt::UserRole, -998);  // Marker für Toggle
        m_listWidget->addItem(toggleHeader);
    }
    
    if (!m_showSynastrieAspects || !hasSynastrie) {
        // STRICT LEGACY: Normale Radix-Aspekte anzeigen
        
        // Planeten-Aspekte Header
        QListWidgetItem* headerItem = new QListWidgetItem("<span style='color:#FFFFFF'>" + tr("-Planeten-") + "</span>");
        headerItem->setBackground(QColor(0, 0, 128));
        m_listWidget->addItem(headerItem);
        
        // STRICT LEGACY Format: "☉ in ♈ △ ☽ in ♉" mit verschiedenen Farben
        for (int i = 0; i < numPlanets; ++i) {
            for (int j = i + 1; j < numPlanets; ++j) {
                int idx = i * numPlanets + j;
                int8_t asp = m_radix.aspPlanet[idx];
                
                if (asp == KEIN_ASP) continue;
                
                QString symbol1 = astroFont().planetSymbol(i);
                QString symbol2 = astroFont().planetSymbol(j);
                
                int stz1 = static_cast<int>(m_radix.planet[i] / 30.0) % 12;
                int stz2 = static_cast<int>(m_radix.planet[j] / 30.0) % 12;
                QString stzSymbol1 = astroFont().sternzeichenSymbol(stz1);
                QString stzSymbol2 = astroFont().sternzeichenSymbol(stz2);
                
                int aspSymIdx = getAspektSymbolIndex(asp);
                QString aspSymbol = astroFont().aspektSymbol(aspSymIdx);
                
                QString planetColor = colorToHtml(sColor[COL_PLAN]);
                int element1 = stz1 % 4;
                int element2 = stz2 % 4;
                QString stzColor1 = colorToHtml(sColor[COL_FEUER + element1]);
                QString stzColor2 = colorToHtml(sColor[COL_FEUER + element2]);
                QString aspColor = colorToHtml(sColor[getAspektColorIndex(asp)]);
                
                QString html = QString("<span style='color:%1'>%2</span> in <span style='color:%3'>%4</span> "
                                       "<span style='color:%5'>%6</span> "
                                       "<span style='color:%7'>%8</span> in <span style='color:%9'>%10</span>")
                    .arg(planetColor).arg(symbol1)
                    .arg(stzColor1).arg(stzSymbol1)
                    .arg(aspColor).arg(aspSymbol)
                    .arg(planetColor).arg(symbol2)
                    .arg(stzColor2).arg(stzSymbol2);
                
                QListWidgetItem* item = new QListWidgetItem(html);
                item->setData(Qt::UserRole, QPoint(i, j));
                item->setBackground(sColor[COL_LBREC_NORM]);
                m_listWidget->addItem(item);
            }
        }
        
        // Häuser-Aspekte Header
        QListWidgetItem* hausHeader = new QListWidgetItem("<span style='color:#FFFFFF'>" + tr("-Häuser-") + "</span>");
        hausHeader->setBackground(QColor(0, 0, 128));
        m_listWidget->addItem(hausHeader);
        
        // Häuser-Aspekte
        for (int i = 0; i < numPlanets; ++i) {
            for (int h = 0; h < MAX_HAUS; ++h) {
                int idx = i * MAX_HAUS + h;
                if (idx >= m_radix.aspHaus.size()) continue;
                
                int8_t asp = m_radix.aspHaus[idx];
                if (asp == KEIN_ASP) continue;
                
                QString symbol = astroFont().planetSymbol(i);
                
                int stzPlanet = static_cast<int>(m_radix.planet[i] / 30.0) % 12;
                int stzHaus = static_cast<int>(m_radix.haus[h] / 30.0) % 12;
                QString stzSymbol1 = astroFont().sternzeichenSymbol(stzPlanet);
                QString stzSymbol2 = astroFont().sternzeichenSymbol(stzHaus);
                
                int aspSymIdx = getAspektSymbolIndex(asp);
                QString aspSymbol = astroFont().aspektSymbol(aspSymIdx);
                
                QString planetColor = colorToHtml(sColor[COL_PLAN]);
                QString hausColor = colorToHtml(sColor[COL_HAUSER]);
                int element1 = stzPlanet % 4;
                int element2 = stzHaus % 4;
                QString stzColor1 = colorToHtml(sColor[COL_FEUER + element1]);
                QString stzColor2 = colorToHtml(sColor[COL_FEUER + element2]);
                QString aspColor = colorToHtml(sColor[getAspektColorIndex(asp)]);
                
                QString html = QString("<span style='color:%1'>%2</span> in <span style='color:%3'>%4</span> "
                                       "<span style='color:%5'>%6</span> "
                                       "<span style='color:%7'>H%8</span> in <span style='color:%9'>%10</span>")
                    .arg(planetColor).arg(symbol)
                    .arg(stzColor1).arg(stzSymbol1)
                    .arg(aspColor).arg(aspSymbol)
                    .arg(hausColor).arg(h + 1, 2, 10, QChar('0'))
                    .arg(stzColor2).arg(stzSymbol2);
                
                QListWidgetItem* item = new QListWidgetItem(html);
                item->setBackground(sColor[COL_LBREC_NORM]);
                m_listWidget->addItem(item);
            }
        }
    } else {
        // STRICT LEGACY: Synastrie/Transit-Aspekte anzeigen (Person1/Radix zu Person2/Transit)
        const Radix& syn = *m_radix.synastrie;
        int numSyn = qMin(syn.anzahlPlanet, static_cast<int16_t>(syn.planet.size()));
        
        // Planeten-Aspekte Header
        QListWidgetItem* headerItem = new QListWidgetItem("<span style='color:#FFFFFF'>" + tr("-Planeten-") + "</span>");
        headerItem->setBackground(QColor(0, 0, 128));
        m_listWidget->addItem(headerItem);
        
        // Synastrie/Transit-Aspekte: Alle Kombinationen (Person1/Radix zu Person2/Transit)
        for (int i = 0; i < numSyn; ++i) {
            for (int j = 0; j < numPlanets; ++j) {
                // Filter anhand TransitSelection (TransSelDialog)
                if (!m_transitSelection.isEmpty()) {
                    if (i >= 0 && i < m_transitSelection.size()) {
                        if (j >= m_transitSelection[i].size() || !m_transitSelection[i][j]) {
                            continue;
                        }
                    }
                }
                double pos1 = syn.planet[i];
                double pos2 = m_radix.planet[j];
                double diff = std::abs(pos1 - pos2);
                if (diff > 180.0) diff = 360.0 - diff;
                
                // Aspekt prüfen mit passenden Orben
                const QVector<float>& orben = (m_radix.horoTyp == TYP_SYNASTRIE) ? 
                    m_auinit.orbenSPlanet : m_auinit.orbenTPlanet;
                
                int8_t asp = KEIN_ASP;
                if (diff <= orben[0]) asp = KONJUNKTION;
                else if (std::abs(diff - 30.0) <= orben[1]) asp = HALBSEX;
                else if (std::abs(diff - 60.0) <= orben[2]) asp = SEXTIL;
                else if (std::abs(diff - 90.0) <= orben[3]) asp = QUADRATUR;
                else if (std::abs(diff - 120.0) <= orben[4]) asp = TRIGON;
                else if (std::abs(diff - 150.0) <= orben[5]) asp = QUINCUNX;
                else if (std::abs(diff - 180.0) <= orben[6]) asp = OPOSITION;
                
                if (asp == KEIN_ASP) continue;
                
                QString symbol1 = astroFont().planetSymbol(i);
                QString symbol2 = astroFont().planetSymbol(j);
                
                int stz1 = static_cast<int>(pos1 / 30.0) % 12;
                int stz2 = static_cast<int>(pos2 / 30.0) % 12;
                QString stzSymbol1 = astroFont().sternzeichenSymbol(stz1);
                QString stzSymbol2 = astroFont().sternzeichenSymbol(stz2);
                
                int aspSymIdx = getAspektSymbolIndex(asp);
                QString aspSymbol = astroFont().aspektSymbol(aspSymIdx);
                
                QString p1Color = colorToHtml(sColor[COL_PLAN_T]);  // Synastrie/Transit-Farbe
                QString p2Color = colorToHtml(sColor[COL_PLAN]);
                int element1 = stz1 % 4;
                int element2 = stz2 % 4;
                QString stzColor1 = colorToHtml(sColor[COL_FEUER + element1]);
                QString stzColor2 = colorToHtml(sColor[COL_FEUER + element2]);
                QString aspColor = colorToHtml(sColor[getAspektColorIndex(asp)]);
                
                QString html = QString("<span style='color:%1'>%2</span> in <span style='color:%3'>%4</span> "
                                       "<span style='color:%5'>%6</span> "
                                       "<span style='color:%7'>%8</span> in <span style='color:%9'>%10</span>")
                    .arg(p1Color).arg(symbol1)
                    .arg(stzColor1).arg(stzSymbol1)
                    .arg(aspColor).arg(aspSymbol)
                    .arg(p2Color).arg(symbol2)
                    .arg(stzColor2).arg(stzSymbol2);
                
                QListWidgetItem* item = new QListWidgetItem(html);
                // Synastrie/Transit-Aspekt: i+1000/2000 und j
                int offset = (m_radix.horoTyp == TYP_SYNASTRIE) ? 2000 : 1000;
                item->setData(Qt::UserRole, QPoint(i + offset, j));
                item->setBackground(sColor[COL_LBREC_NORM]);
                m_listWidget->addItem(item);
            }
        }
    }
    
    // Grafik-Aspekt-Modus synchronisieren
    m_chartWidget->setShowSynastrieAspects(m_showSynastrieAspects && hasSynastrie);
}

// LEGACY REMOVED: Alte Synastrie/Transit-Aspekte Blöcke wurden entfernt
// Sie werden jetzt durch den Toggle-Mechanismus gesteuert

//==============================================================================
// Slots
//==============================================================================

void RadixWindow::onDatenClicked() {
    m_listMode = Daten;
    fillDatenList();
}

void RadixWindow::onPositionenClicked() {
    m_listMode = Positionen;
    fillPositionenList();
}

void RadixWindow::onAspekteClicked() {
    m_listMode = Aspekte;
    // STRICT LEGACY: Bei Transit/Synastrie standardmäßig Transit-Aspekte anzeigen
    if (m_radix.synastrie) {
        m_showSynastrieAspects = true;
    } else {
        m_showSynastrieAspects = false;
    }
    fillAspekteList();
}

void RadixWindow::onListItemClicked(QListWidgetItem* item) {
    if (!item) return;
    
    int row = m_listWidget->row(item);
    
    if (m_listMode == Positionen) {
        // STRICT LEGACY: Klick auf Person-Header toggled zwischen Person 1 und Person 2
        QVariant data = item->data(Qt::UserRole);
        if (data.isValid() && data.toInt() == -999) {
            // Toggle Person 1 / Person 2
            m_showPerson2 = !m_showPerson2;
            fillPositionenList();
            return;
        }
        
        // Erste Zeile ist Header "-Planeten-" (oder Person-Header bei Synastrie)
        if (row == 0 || (m_radix.horoTyp == TYP_SYNASTRIE && m_radix.synastrie && row == 1)) {
            m_chartWidget->highlightPlanet(-1);  // Keine Hervorhebung
            return;
        }
        
        // Planeten: Zeile nach Header
        int planetIndex = data.isValid() ? data.toInt() : -1;
        if (planetIndex >= 0 && planetIndex < 100) {
            // Radix-Planet
            m_chartWidget->highlightPlanet(planetIndex, false);
        } else if (planetIndex >= 100 && planetIndex < 200) {
            // Transit-Planet (Index 100-199)
            m_chartWidget->highlightPlanet(planetIndex - 100, true);
        } else if (planetIndex >= 200) {
            // Person 2 Planet - Synastrie (Index 200+)
            m_chartWidget->highlightPlanet(planetIndex - 200, true);
        } else {
            // Häuser-Header oder Haus
            m_chartWidget->highlightPlanet(-1);
        }
    } else if (m_listMode == Aspekte) {
        // STRICT LEGACY: Klick auf Toggle-Header wechselt zwischen Radix und Synastrie/Transit-Aspekten
        QVariant data = item->data(Qt::UserRole);
        if (data.isValid() && data.toInt() == -998) {
            // Toggle Radix / Synastrie-Transit Aspekte
            m_showSynastrieAspects = !m_showSynastrieAspects;
            fillAspekteList();
            return;
        }
        
        // Header-Zeilen ignorieren
        if (row == 0) {
            m_chartWidget->highlightAspect(-1, -1);
            return;
        }
        
        // Aspekt-Index aus Zeile berechnen
        // Wir müssen die Planeten-Indizes aus dem Item-Data holen
        if (data.isValid()) {
            QPoint planets = data.toPoint();
            // Synastrie-Aspekte haben x >= 2000 (Person1-Planet + 2000)
            if (planets.x() >= 2000) {
                // Synastrie-Aspekt: Person1-Planet (x-2000) zu Person2-Planet (y)
                m_chartWidget->highlightTransitAspect(planets.x() - 2000, planets.y());
            } else if (planets.x() >= 1000) {
                // Transit-Aspekt: Transit-Planet (x-1000) zu Radix-Planet (y)
                m_chartWidget->highlightTransitAspect(planets.x() - 1000, planets.y());
            } else {
                // Normaler Aspekt: Radix-Planet zu Radix-Planet
                m_chartWidget->highlightAspect(planets.x(), planets.y());
            }
        }
    } else {
        // Daten-Modus: keine Hervorhebung
        m_chartWidget->highlightPlanet(-1);
    }
}

//==============================================================================
// STRICT LEGACY: Zeit-Scroll Funktionen (Port von sSetRadixScroll, vSetRadixScrollIni)
//==============================================================================

void RadixWindow::onNeuClicked() {
    // STRICT LEGACY: "Neu" öffnet den Personen-Erfassungs-Dialog (CM_U_PERSON -> DlgPErfassen)
    PersonDialog dialog(this, m_auinit, m_radix);
    if (dialog.exec() == QDialog::Accepted && dialog.wasCalculated()) {
        updateTimeSpinBoxes();
        updateDisplay();
    }
}

void RadixWindow::onMinuteChanged(int value) {
    if (m_updatingSpinBoxes) return;
    // Zeit als Dezimalzahl aktualisieren (Stunde bleibt, Minute ändert sich)
    int hour = static_cast<int>(m_radix.rFix.zeit);
    m_radix.rFix.zeit = hour + value / 60.0;
    m_radix.zeit = QString("%1:%2").arg(hour, 2, 10, QChar('0')).arg(value, 2, 10, QChar('0'));
    recalculateChart();
}

void RadixWindow::onHourChanged(int value) {
    if (m_updatingSpinBoxes) return;
    // Zeit als Dezimalzahl aktualisieren (Minute bleibt, Stunde ändert sich)
    double minute = (m_radix.rFix.zeit - static_cast<int>(m_radix.rFix.zeit)) * 60.0;
    m_radix.rFix.zeit = value + minute / 60.0;
    m_radix.zeit = QString("%1:%2").arg(value, 2, 10, QChar('0')).arg(static_cast<int>(minute), 2, 10, QChar('0'));
    recalculateChart();
}

void RadixWindow::onDayChanged(int value) {
    if (m_updatingSpinBoxes) return;
    m_radix.rFix.tag = value;
    m_radix.datum = QString("%1.%2.%3").arg(value, 2, 10, QChar('0'))
                                        .arg(m_radix.rFix.monat, 2, 10, QChar('0'))
                                        .arg(m_radix.rFix.jahr);
    recalculateChart();
}

void RadixWindow::onMonthChanged(int value) {
    if (m_updatingSpinBoxes) return;
    m_radix.rFix.monat = value;
    m_radix.datum = QString("%1.%2.%3").arg(m_radix.rFix.tag, 2, 10, QChar('0'))
                                        .arg(value, 2, 10, QChar('0'))
                                        .arg(m_radix.rFix.jahr);
    recalculateChart();
}

void RadixWindow::onYearChanged(int value) {
    if (m_updatingSpinBoxes) return;
    m_radix.rFix.jahr = value;
    m_radix.datum = QString("%1.%2.%3").arg(m_radix.rFix.tag, 2, 10, QChar('0'))
                                        .arg(m_radix.rFix.monat, 2, 10, QChar('0'))
                                        .arg(value);
    recalculateChart();
}

void RadixWindow::updateTimeSpinBoxes() {
    m_updatingSpinBoxes = true;
    
    // Zeit aus Dezimalzahl extrahieren
    int hour = static_cast<int>(m_radix.rFix.zeit);
    int minute = static_cast<int>((m_radix.rFix.zeit - hour) * 60.0 + 0.5);
    
    m_minuteSpinBox->setValue(minute);
    m_hourSpinBox->setValue(hour);
    m_daySpinBox->setValue(m_radix.rFix.tag);
    m_monthSpinBox->setValue(m_radix.rFix.monat);
    m_yearSpinBox->setValue(m_radix.rFix.jahr);
    
    m_updatingSpinBoxes = false;
}

void RadixWindow::recalculateChart() {
    // Chart neu berechnen mit geänderten Zeitdaten
    if (ChartCalc::calculate(m_radix, nullptr, TYP_RADIX) >= 0) {
        updateDisplay();
    }
}

} // namespace astro
