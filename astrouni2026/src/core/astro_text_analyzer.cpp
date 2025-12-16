/**
 * @file astro_text_analyzer.cpp
 * @brief Implementierung der automatischen Textanalyse für Horoskope
 */

#include "astro_text_analyzer.h"
#include "constants.h"
#include "astro_text_store.h"
#include "astro_font_provider.h"
#include "calculations.h"
#include <QStringList>

namespace astro {

using namespace astro; // Make constants available without prefix

static QString analyzerLang() {
  return AstroTextStore::systemLanguageCode();
}

AstroTextAnalyzer::AstroTextAnalyzer() : m_auinit(nullptr) {
  initializePlanetNames();
  initializeSignNames();
  initializeAspectNames();
  initializeSunSignTexts();
  initializeAscendantTexts();
  initializeMoonSignTexts();
  initializeAspectTexts();
}

void AstroTextAnalyzer::setOrben(const AuInit* auinit) {
  m_auinit = auinit;
}

bool AstroTextAnalyzer::checkAspektWithOrben(double pos1, double pos2, int p1, int p2, 
                                             int aspekt, int horoTyp, double& exakterWinkel) const {
  double diff = std::abs(pos1 - pos2);
  if (diff > 180.0) diff = 360.0 - diff;
  exakterWinkel = diff;
  
  double orb = 8.0;  // Default-Orb
  
  if (m_auinit) {
    // Orben aus Einstellungen verwenden
    // Index: [Planet1 * MAX_PLANET + Planet2] * ASPEKTE + Aspekt
    const QVector<float>* orben = nullptr;
    if (horoTyp == TYP_SYNASTRIE) {
      orben = &m_auinit->orbenSPlanet;
    } else if (horoTyp == TYP_TRANSIT) {
      orben = &m_auinit->orbenTPlanet;
    } else {
      orben = &m_auinit->orbenPlanet;
    }
    
    // Aspekt-Index ermitteln
    int aspIdx = 0;
    switch (aspekt) {
      case KONJUNKTION: aspIdx = 0; break;
      case HALBSEX:     aspIdx = 1; break;
      case SEXTIL:      aspIdx = 2; break;
      case QUADRATUR:   aspIdx = 3; break;
      case TRIGON:      aspIdx = 4; break;
      case QUINCUNX:    aspIdx = 5; break;
      case OPOSITION:   aspIdx = 6; break;
      default: return false;
    }
    
    int idx = (p1 * MAX_PLANET + p2) * ASPEKTE + aspIdx;
    if (orben && idx < orben->size() && (*orben)[idx] > 0.0f) {
      orb = (*orben)[idx];
    }
  }
  
  double abweichung = std::abs(diff - static_cast<double>(aspekt));
  return abweichung <= orb;
}

QString AstroTextAnalyzer::analyzeTransit(const Radix &radix) const {
  QString html;

  astroTextStore().ensureLoaded();
  const QString lang = analyzerLang();

  // Font-Namen vom AstroFontProvider holen
  QString signFont = astroFont().fontName();  // AstroUniverse für Sternzeichen
  QString planetFont = astroFont().getPlanetSymbolFont().family();  // DejaVu Sans für Planeten/Asteroiden

  // HTML-Header mit korrekten Font-Zuordnungen
  html += "<html><head><style>";
  html += "body { font-family: Arial, sans-serif; font-size: 11pt; }";
  html += "h1 { color: #2c3e50; font-size: 16pt; margin-top: 20px; }";
  html += "h2 { color: #34495e; font-size: 14pt; margin-top: 15px; }";
  html += "h3 { color: #7f8c8d; font-size: 12pt; margin-top: 10px; }";
  html += "p { margin: 8px 0; line-height: 1.5; }";
  html += QString(".planet-symbol { font-family: '%1'; font-size: 14pt; color: #e74c3c; }").arg(planetFont);
  html += QString(".sign-symbol { font-family: '%1'; font-size: 17pt; color: #3498db; }").arg(signFont);
  html += QString(".aspect-symbol { font-family: '%1'; font-size: 12pt; color: #9b59b6; }").arg(planetFont);
  html += "</style></head><body>";

  // Bei Transit: radix.synastrie enthält das Radix (Geburtsdatum), radix enthält Transit-Datum
  // Aber die Struktur ist so aufgebaut, dass radix.rFix das Geburtsdatum enthält
  // und radix.synastrie->rFix das Transit-Datum
  
  // Radix-Person und Geburtsdatum (aus radix.rFix - das ist die Hauptperson)
  QString name = radix.rFix.vorname;
  if (!radix.rFix.name.isEmpty()) {
    if (!name.isEmpty()) name += " ";
    name += radix.rFix.name;
  }
  if (name.isEmpty()) {
    name = astroTextStore().text(lang, "analysis.transit.person", "Person");
  }
  
  int nStunde = static_cast<int>(radix.rFix.zeit);
  int nMinute = static_cast<int>((radix.rFix.zeit - nStunde) * 60);
  QString natalDatum = QString("%1.%2.%3 um %4:%5 Uhr")
                           .arg(radix.rFix.tag, 2, 10, QChar('0'))
                           .arg(radix.rFix.monat, 2, 10, QChar('0'))
                           .arg(radix.rFix.jahr)
                           .arg(nStunde, 2, 10, QChar('0'))
                           .arg(nMinute, 2, 10, QChar('0'));
  QString ort = radix.rFix.ort;
  if (ort.isEmpty()) ort = "---";
  
  // Transit-Datum (aus radix.synastrie->rFix - das ist das Transit-Horoskop)
  QString transitDatumStr;
  if (radix.synastrie) {
    const Radix& transitRadix = *radix.synastrie;
    int tStunde = static_cast<int>(transitRadix.rFix.zeit);
    int tMinute = static_cast<int>((transitRadix.rFix.zeit - tStunde) * 60);
    QString dateTemplate = astroTextStore().text(lang, "analysis.transit.date_line",
                                                "Transit-Datum: %1.%2.%3 um %4:%5 Uhr");
    transitDatumStr = dateTemplate
                           .arg(transitRadix.rFix.tag, 2, 10, QChar('0'))
                           .arg(transitRadix.rFix.monat, 2, 10, QChar('0'))
                           .arg(transitRadix.rFix.jahr)
                           .arg(tStunde, 2, 10, QChar('0'))
                           .arg(tMinute, 2, 10, QChar('0'));
  }

  QString title = astroTextStore().text(lang, "analysis.transit.title",
                                       "Horoskop-Analyse: Transit");
  html += QString("<h1>%1</h1>").arg(title);
  
  // Radix-Person mit Geburtsdatum
  html += QString("<p><i>%1: %2, geboren am %3</i></p>")
              .arg(astroTextStore().text(lang, "analysis.transit.for_person", "Für"))
              .arg(name)
              .arg(natalDatum);
  
  // Transit-Datum
  if (!transitDatumStr.isEmpty()) {
    html += QString("<h3>%1</h3>").arg(transitDatumStr);
  }
  
  html += astroTextStore().text(lang, "analysis.transit.intro",
                               "<p>Diese Analyse beschreibt die aktuellen planetaren Einflüsse auf Ihr Geburtshoroskop.</p>");
  html += "<hr>";

  // Transit Texte
  html += QString("<h2>%1</h2>")
              .arg(astroTextStore().text(lang, "analysis.transit.section_influences",
                                        "Aktuelle Einflüsse"));
  html += astroTextStore().text(lang, "analysis.transit.definition",
                               "<p>Transite zeigen zeitweilige Einflüsse der aktuellen Planetenstände auf Ihr Geburtshoroskop.</p>");

  // Aspekte zwischen Transit-Planeten (radix) und Radix-Planeten (radix.synastrie)
  int aspectCount = 0;
  static const int aspekte[] = { KONJUNKTION, HALBSEX, SEXTIL, QUADRATUR, TRIGON, QUINCUNX, OPOSITION };

  // Transit-Planeten sind in radix, Radix-Planeten sind in radix.synastrie
  if (radix.synastrie) {
    const Radix& natalRadix = *radix.synastrie;
    
    for (int tp = 0; tp < radix.anzahlPlanet && tp < MAX_PLANET; tp++) {
      for (int rp = 0; rp < natalRadix.anzahlPlanet && rp < MAX_PLANET; rp++) {
        // Aspekt zwischen Transit-Planet tp und Radix-Planet rp berechnen
        double transitPos = radix.planet[tp];
        double radixPos = natalRadix.planet[rp];
        
        for (int a = 0; a < ASPEKTE; ++a) {
          double exakterWinkel;
          if (checkAspektWithOrben(transitPos, radixPos, tp, rp, aspekte[a], TYP_TRANSIT, exakterWinkel)) {
            int8_t sign1 = (tp < radix.stzPlanet.size()) ? radix.stzPlanet[tp] : -1;
            int8_t sign2 = (rp < natalRadix.stzPlanet.size()) ? natalRadix.stzPlanet[rp] : -1;

            html += QString("<h3><span class='planet-symbol'>%1</span> %2 (T) "
                            "<span class='aspect-symbol'>%3</span> <span "
                            "class='planet-symbol'>%4</span> %5 (R)</h3>")
                        .arg(PLANET_SYMBOLS[tp])
                        .arg(getPlanetName(tp))
                        .arg(getAspectName(aspekte[a]))
                        .arg(PLANET_SYMBOLS[rp])
                        .arg(getPlanetName(rp));

            html += getContextAspectText(tp, sign1, rp, sign2, aspekte[a]);
            aspectCount++;
            break;  // Nur engsten Aspekt
          }
        }
      }
    }
  }

  if (aspectCount == 0) {
    html += astroTextStore().text(lang, "analysis.transit.no_aspects",
                                 "<p><i>Keine signifikanten Transite für diesen Tag.</i></p>");
  }

  html += "</body></html>";
  return html;
}

QString AstroTextAnalyzer::analyzeSynastry(const Radix &radix) const {
  QString html;

  astroTextStore().ensureLoaded();
  const QString lang = analyzerLang();

  // Font-Namen vom AstroFontProvider holen
  QString signFont = astroFont().fontName();  // AstroUniverse für Sternzeichen
  QString planetFont = astroFont().getPlanetSymbolFont().family();  // DejaVu Sans für Planeten/Asteroiden

  // HTML-Header mit korrekten Font-Zuordnungen
  html += "<html><head><style>";
  html += "body { font-family: Arial, sans-serif; font-size: 11pt; }";
  html += "h1 { color: #2c3e50; font-size: 16pt; margin-top: 20px; }";
  html += "h2 { color: #34495e; font-size: 14pt; margin-top: 15px; }";
  html += "h3 { color: #7f8c8d; font-size: 12pt; margin-top: 10px; }";
  html += "p { margin: 8px 0; line-height: 1.5; }";
  html += QString(".planet-symbol { font-family: '%1'; font-size: 14pt; color: #e74c3c; }").arg(planetFont);
  html += QString(".sign-symbol { font-family: '%1'; font-size: 17pt; color: #3498db; }").arg(signFont);
  html += QString(".aspect-symbol { font-family: '%1'; font-size: 12pt; color: #9b59b6; }").arg(planetFont);
  html += "</style></head><body>";

  html += QString("<h1>%1</h1>")
              .arg(astroTextStore().text(lang, "analysis.synastry.title",
                                        "Horoskop-Analyse: Partner-Vergleich"));
  html += "<hr>";

  html += QString("<h2>%1</h2>")
              .arg(astroTextStore().text(lang, "analysis.synastry.section_relationship",
                                        "Beziehungs-Aspekte"));

  // Aspekte zwischen Person A (radix) und Person B (radix.synastrie) berechnen
  int aspectCount = 0;
  static const int aspekte[] = { KONJUNKTION, HALBSEX, SEXTIL, QUADRATUR, TRIGON, QUINCUNX, OPOSITION };

  if (radix.synastrie) {
    const Radix& partnerRadix = *radix.synastrie;
    
    for (int pa = 0; pa < radix.anzahlPlanet && pa < MAX_PLANET; pa++) {
      for (int pb = 0; pb < partnerRadix.anzahlPlanet && pb < MAX_PLANET; pb++) {
        // Aspekt zwischen Planet pa (Person A) und Planet pb (Person B) berechnen
        double posA = radix.planet[pa];
        double posB = partnerRadix.planet[pb];
        
        for (int a = 0; a < ASPEKTE; ++a) {
          double exakterWinkel;
          if (checkAspektWithOrben(posA, posB, pa, pb, aspekte[a], TYP_SYNASTRIE, exakterWinkel)) {
            int8_t sign1 = (pa < radix.stzPlanet.size()) ? radix.stzPlanet[pa] : -1;
            int8_t sign2 = (pb < partnerRadix.stzPlanet.size()) ? partnerRadix.stzPlanet[pb] : -1;

            html += QString("<h3><span class='planet-symbol'>%1</span> %2 (A) "
                            "<span class='aspect-symbol'>%3</span> <span "
                            "class='planet-symbol'>%4</span> %5 (B)</h3>")
                        .arg(PLANET_SYMBOLS[pa])
                        .arg(getPlanetName(pa))
                        .arg(getAspectName(aspekte[a]))
                        .arg(PLANET_SYMBOLS[pb])
                        .arg(getPlanetName(pb));

            html += getContextAspectText(pa, sign1, pb, sign2, aspekte[a]);
            aspectCount++;
            break;  // Nur engsten Aspekt
          }
        }
      }
    }
  }

  if (aspectCount == 0) {
    html += astroTextStore().text(lang, "analysis.synastry.no_aspects",
                                 "<p><i>Keine starken Verbindungsaspekte gefunden.</i></p>");
  }

  html += "</body></html>";
  return html;
}

//==============================================================================
// Hauptanalyse-Funktion
//==============================================================================

QString AstroTextAnalyzer::analyzeRadix(const Radix &radix) const {
  astroTextStore().ensureLoaded();
  const QString lang = analyzerLang();

  // Dispatch basierend auf Horoskop-Typ
  if (radix.horoTyp == TYP_TRANSIT) {
    return analyzeTransit(radix);
  } else if (radix.horoTyp == TYP_SYNASTRIE) {
    return analyzeSynastry(radix);
  }

  QString html;

  // Font-Namen vom AstroFontProvider holen
  QString signFont = astroFont().fontName();  // AstroUniverse für Sternzeichen
  QString planetFont = astroFont().getPlanetSymbolFont().family();  // DejaVu Sans für Planeten/Asteroiden

  // Header mit korrekten Font-Zuordnungen
  html += "<html><head><style>";
  html += "body { font-family: Arial, sans-serif; font-size: 11pt; }";
  html += "h1 { color: #2c3e50; font-size: 16pt; margin-top: 20px; }";
  html += "h2 { color: #34495e; font-size: 14pt; margin-top: 15px; }";
  html += "h3 { color: #7f8c8d; font-size: 12pt; margin-top: 10px; }";
  html += "p { margin: 8px 0; line-height: 1.5; }";
  html += QString(".planet-symbol { font-family: '%1'; font-size: 14pt; color: #e74c3c; }").arg(planetFont);
  html += QString(".sign-symbol { font-family: '%1'; font-size: 17pt; color: #3498db; }").arg(signFont);
  html += QString(".aspect-symbol { font-family: '%1'; font-size: 12pt; color: #9b59b6; }").arg(planetFont);
  html += "</style></head><body>";

  // Titel
  QString name = radix.rFix.vorname;
  if (!radix.rFix.name.isEmpty()) {
    if (!name.isEmpty())
      name += " ";
    name += radix.rFix.name;
  }
  if (name.isEmpty())
    name = astroTextStore().text(lang, "analysis.radix.default_name", "Radix");

  QString titleTemplate = astroTextStore().text(lang, "analysis.radix.title", "Horoskop-Analyse: %1");
  html += QString("<h1>%1</h1>").arg(titleTemplate.arg(name));

  // Geburtsdatum und -zeit aus rFix formatieren
  int stunde = static_cast<int>(radix.rFix.zeit);
  int minute = static_cast<int>((radix.rFix.zeit - stunde) * 60);
  QString datumStr = QString("%1.%2.%3")
                         .arg(radix.rFix.tag, 2, 10, QChar('0'))
                         .arg(radix.rFix.monat, 2, 10, QChar('0'))
                         .arg(radix.rFix.jahr);
  QString zeitStr = QString("%1:%2")
                        .arg(stunde, 2, 10, QChar('0'))
                        .arg(minute, 2, 10, QChar('0'));
  
  QString bornTemplate = astroTextStore().text(lang, "analysis.radix.born_line",
                                              "<p><i>Geboren am %1 um %2 Uhr in %3</i></p>");
  html += bornTemplate.arg(datumStr).arg(zeitStr).arg(radix.rFix.ort);

  html += "<hr>";

  // 1. Sonnenzeichen (Hauptthema)
  if (radix.stzPlanet.size() > P_SONNE) {
    int8_t sunSign = radix.stzPlanet[P_SONNE];
    QString sunLabel = astroTextStore().text(lang, "analysis.label.sun_sign", "Sonnenzeichen");
    html += "<h2>" + QString(PLANET_SYMBOLS[P_SONNE]) + " " + sunLabel + ": " + getSignName(sunSign) + "</h2>";
    html += QString("<p class='sign-symbol'>%1</p>")
                .arg(astroFont().sternzeichenSymbol(sunSign));
    html += analyzeSunSign(sunSign);
  }

  // 2. Aszendent
  if (radix.stzHaus.size() > 0) {
    int8_t ascSign = radix.stzHaus[0];
    QString ascLabel = astroTextStore().text(lang, "analysis.label.ascendant", "Aszendent");
    html += "<h2>ASC " + ascLabel + ": " + getSignName(ascSign) + "</h2>";
    html += QString("<p class='sign-symbol'>%1</p>")
                .arg(astroFont().sternzeichenSymbol(ascSign));
    html += analyzeAscendant(ascSign);
  }

  // 3. Mondzeichen
  if (radix.stzPlanet.size() > P_MOND) {
    int8_t moonSign = radix.stzPlanet[P_MOND];
    QString moonLabel = astroTextStore().text(lang, "analysis.label.moon_sign", "Mondzeichen");
    html += "<h2>" + QString(PLANET_SYMBOLS[P_MOND]) + " " + moonLabel + ": " + getSignName(moonSign) + "</h2>";
    html += QString("<p class='sign-symbol'>%1</p>")
                .arg(astroFont().sternzeichenSymbol(moonSign));
    html += analyzeMoonSign(moonSign);
  }

  // 4. Aspekte
  // 4. Aspekte
  QString aspectsLabel = astroTextStore().text(lang, "analysis.label.important_aspects", "Wichtige Aspekte");
  html += "<h2>" + QString(ASPEKT_SYMBOLS[SEXTIL]) + " " + aspectsLabel + "</h2>";
  html += astroTextStore().text(lang, "analysis.radix.aspects_intro",
                               "<p><i>Aspekte zeigen die Beziehungen zwischen den Planeten.</i></p>");

  // Aspekte durchgehen
  int aspectCount = 0;
  int numPlanets = radix.anzahlPlanet;
  for (int i = 0; i < numPlanets && i < MAX_PLANET; i++) {
    for (int j = i + 1; j < numPlanets && j < MAX_PLANET; j++) {
      int aspIndex = i * numPlanets + j;
      if (aspIndex < radix.aspPlanet.size()) {
        int16_t aspect = radix.aspPlanet[aspIndex];
        if (aspect != KEIN_ASP) {
          html += QString("<h3><span class='planet-symbol'>%1</span> %2 <span "
                          "class='aspect-symbol'>%3</span> <span "
                          "class='planet-symbol'>%4</span> %5</h3>")
                      .arg(PLANET_SYMBOLS[i])
                      .arg(getPlanetName(i))
                      .arg(getAspectName(aspect))
                      .arg(PLANET_SYMBOLS[j])
                      .arg(getPlanetName(j));
          // Kontext berücksichtigen (Zeichen)
          int8_t sign1 = -1;
          int8_t sign2 = -1;
          if (i < radix.stzPlanet.size())
            sign1 = radix.stzPlanet[i];
          if (j < radix.stzPlanet.size())
            sign2 = radix.stzPlanet[j];

          html += getContextAspectText(i, sign1, j, sign2, aspect);
          aspectCount++;
        }
      }
    }
  }

  if (aspectCount == 0) {
    html += astroTextStore().text(lang, "analysis.radix.no_aspects",
                                 "<p><i>Keine signifikanten Aspekte gefunden.</i></p>");
  }

  // 5. ASC-Aspekte (Planeten zum Aszendenten)
  if (radix.haus.size() > 0 && radix.planet.size() > 0) {
    double ascPos = radix.haus[0];  // ASC = Hausspitze 1
    static const int aspekte[] = { KONJUNKTION, HALBSEX, SEXTIL, QUADRATUR, TRIGON, QUINCUNX, OPOSITION };
    static const double aspWinkel[] = { 0.0, 30.0, 60.0, 90.0, 120.0, 150.0, 180.0 };
    
    int ascAspectCount = 0;
    QString ascAspectHtml;
    
    for (int p = 0; p < radix.anzahlPlanet && p < MAX_PLANET; p++) {
      double planetPos = radix.planet[p];
      double diff = std::abs(planetPos - ascPos);
      if (diff > 180.0) diff = 360.0 - diff;
      
      // Aspekt prüfen mit Standard-Orb 8°
      for (int a = 0; a < ASPEKTE; ++a) {
        double orb = 8.0;
        if (std::abs(diff - aspWinkel[a]) <= orb) {
          int16_t asp = aspekte[a];
          ascAspectHtml += QString("<h3><span class='planet-symbol'>%1</span> %2 <span "
                          "class='aspect-symbol'>%3</span> ASC</h3>")
                      .arg(PLANET_SYMBOLS[p])
                      .arg(getPlanetName(p))
                      .arg(getAspectName(asp));
          ascAspectHtml += "<p>" + getAscAspectText(p, asp) + "</p>";
          ascAspectCount++;
          break;  // Nur engsten Aspekt
        }
      }
    }
    
    if (ascAspectCount > 0) {
      html += QString("<h2>%1</h2>")
                  .arg(astroTextStore().text(lang, "analysis.label.asc_aspects", "Aspekte zum Aszendenten"));
      html += ascAspectHtml;
    }
  }

  // 6. MC-Aspekte (Planeten zum Medium Coeli)
  if (radix.haus.size() > 9 && radix.planet.size() > 0) {
    double mcPos = radix.haus[9];  // MC = Hausspitze 10
    static const int aspekte[] = { KONJUNKTION, HALBSEX, SEXTIL, QUADRATUR, TRIGON, QUINCUNX, OPOSITION };
    static const double aspWinkel[] = { 0.0, 30.0, 60.0, 90.0, 120.0, 150.0, 180.0 };
    
    int mcAspectCount = 0;
    QString mcAspectHtml;
    
    for (int p = 0; p < radix.anzahlPlanet && p < MAX_PLANET; p++) {
      double planetPos = radix.planet[p];
      double diff = std::abs(planetPos - mcPos);
      if (diff > 180.0) diff = 360.0 - diff;
      
      // Aspekt prüfen mit Standard-Orb 8°
      for (int a = 0; a < ASPEKTE; ++a) {
        double orb = 8.0;
        if (std::abs(diff - aspWinkel[a]) <= orb) {
          int16_t asp = aspekte[a];
          mcAspectHtml += QString("<h3><span class='planet-symbol'>%1</span> %2 <span "
                          "class='aspect-symbol'>%3</span> MC</h3>")
                      .arg(PLANET_SYMBOLS[p])
                      .arg(getPlanetName(p))
                      .arg(getAspectName(asp));
          mcAspectHtml += "<p>" + getMcAspectText(p, asp) + "</p>";
          mcAspectCount++;
          break;  // Nur engsten Aspekt
        }
      }
    }
    
    if (mcAspectCount > 0) {
      html += QString("<h2>%1</h2>")
                  .arg(astroTextStore().text(lang, "analysis.label.mc_aspects", "Aspekte zum MC"));
      html += mcAspectHtml;
    }
  }

  html += "</body></html>";
  return html;
}

//==============================================================================
// Initialisierung der Namen
//==============================================================================

void AstroTextAnalyzer::initializePlanetNames() {
  m_planetNames[P_SONNE] = "Sonne";
  m_planetNames[P_MOND] = "Mond";
  m_planetNames[P_MERKUR] = "Merkur";
  m_planetNames[P_VENUS] = "Venus";
  m_planetNames[P_MARS] = "Mars";
  m_planetNames[P_JUPITER] = "Jupiter";
  m_planetNames[P_SATURN] = "Saturn";
  m_planetNames[P_URANUS] = "Uranus";
  m_planetNames[P_NEPTUN] = "Neptun";
  m_planetNames[P_PLUTO] = "Pluto";
  m_planetNames[P_NKNOTEN] = "Mondknoten";
  m_planetNames[P_LILITH] = "Lilith";
  m_planetNames[P_CHIRON] = "Chiron";
  m_planetNames[P_CERES] = "Ceres";
  m_planetNames[P_PALLAS] = "Pallas";
  m_planetNames[P_JUNO] = "Juno";
  m_planetNames[P_VESTA] = "Vesta";
}

void AstroTextAnalyzer::initializeSignNames() {
  m_signNames[iWIDDER] = "Widder";
  m_signNames[iSTIER] = "Stier";
  m_signNames[iZWILLINGE] = "Zwillinge";
  m_signNames[iKREBS] = "Krebs";
  m_signNames[iLOWE] = "Löwe";
  m_signNames[iJUNGFRAU] = "Jungfrau";
  m_signNames[iWAAGE] = "Waage";
  m_signNames[iSKORPION] = "Skorpion";
  m_signNames[iSCHUTZE] = "Schütze";
  m_signNames[iSTEINBOCK] = "Steinbock";
  m_signNames[iWASSERMANN] = "Wassermann";
  m_signNames[iFISCHE] = "Fische";
}

void AstroTextAnalyzer::initializeAspectNames() {
  m_aspectNames[KONJUNKTION] = "Konjunktion";
  m_aspectNames[HALBSEX] = "Halbsextil";
  m_aspectNames[SEXTIL] = "Sextil";
  m_aspectNames[QUADRATUR] = "Quadrat";
  m_aspectNames[TRIGON] = "Trigon";
  m_aspectNames[QUINCUNX] = "Quincunx";
  m_aspectNames[OPOSITION] = "Opposition";
  // Ergänzung für unbekannte Aspekte
  // Quintil (72°), Biquintil (144°), etc. falls im System
  // Falls das System KEIN_ASP nutzt:
  m_aspectNames[KEIN_ASP] = "Kein Aspekt";
}

//==============================================================================
// Getter-Funktionen
//==============================================================================

QString AstroTextAnalyzer::getPlanetName(int planetIndex) const {
  astroTextStore().ensureLoaded();
  const QString lang = analyzerLang();
  const QString key = QString("planet_name.%1").arg(planetIndex);
  return astroTextStore().text(lang, key, m_planetNames.value(planetIndex, "Unbekannt"));
}

QString AstroTextAnalyzer::getSignName(int8_t signIndex) const {
  astroTextStore().ensureLoaded();
  const QString lang = analyzerLang();
  const QString key = QString("sign_name.%1").arg(static_cast<int>(signIndex));
  return astroTextStore().text(lang, key, m_signNames.value(signIndex, "Unbekannt"));
}

QString AstroTextAnalyzer::getAspectName(int16_t aspectIndex) const {
  astroTextStore().ensureLoaded();
  const QString lang = analyzerLang();
  const QString key = QString("aspect_name.%1").arg(static_cast<int>(aspectIndex));
  const QString fallback = m_aspectNames.contains(aspectIndex)
                               ? m_aspectNames[aspectIndex]
                               : QString("Aspekt_%1").arg(aspectIndex);
  return astroTextStore().text(lang, key, fallback);
}

QString AstroTextAnalyzer::contextAspectText(int planet1, int8_t sign1, int planet2, int8_t sign2, int16_t aspect) const {
  return getContextAspectText(planet1, sign1, planet2, sign2, aspect);
}

QString AstroTextAnalyzer::planetPairText(int planet1, int planet2, int16_t aspect) const {
  return getPlanetPairText(planet1, planet2, aspect);
}

QString AstroTextAnalyzer::genericAspectTemplate(int16_t aspect) const {
  astroTextStore().ensureLoaded();
  const QString lang = analyzerLang();
  const QString key = QString("aspect.generic.%1").arg(static_cast<int>(aspect));
  return astroTextStore().text(lang, key, defaultGenericAspectTemplate(aspect));
}

//==============================================================================
// Hilfsfunktionen
//==============================================================================

uint32_t AstroTextAnalyzer::makeAspectKey(int planet1, int planet2,
                                          int16_t aspect) const {
  return (planet1 << 16) | (planet2 << 8) |
         (static_cast<uint8_t>(aspect) & 0xFF);
}

QString AstroTextAnalyzer::getGenericAspectText(int planet1, int planet2,
                                                int16_t aspect) const {
  QString text = "<p>";
  QString p1 = getPlanetName(planet1);
  QString p2 = getPlanetName(planet2);

  // Spezifische Texte für Planeten-Kombinationen
  QString specificText = getPlanetPairText(planet1, planet2, aspect);
  if (!specificText.isEmpty()) {
    text += specificText;
    text += "</p>";
    return text;
  }

  const QString templ = genericAspectTemplate(aspect);
  text += QString(templ).arg(p1, p2);

  text += "</p>";
  return text;
}

QString AstroTextAnalyzer::defaultGenericAspectTemplate(int16_t aspect) const {
  switch (aspect) {
  case KONJUNKTION:
    return "Die Energien von %1 und %2 verschmelzen hier zu einer Einheit. "
           "Diese Verbindung intensiviert beide Planetenprinzipien und schafft "
           "einen kraftvollen Fokus in diesem Lebensbereich. Die Themen beider "
           "Planeten werden untrennbar miteinander verwoben.";
  case SEXTIL:
    return "Das Sextil zwischen %1 und %2 öffnet Türen für kreative Möglichkeiten. "
           "Diese harmonische Verbindung erfordert jedoch bewusste Aktivierung - "
           "die Chancen sind da, aber Sie müssen sie ergreifen. Es entsteht ein "
           "produktiver Austausch zwischen diesen Lebensbereichen.";
  case QUADRATUR:
    return "Das Quadrat zwischen %1 und %2 erzeugt eine dynamische Spannung, "
           "die zu Wachstum und Entwicklung drängt. Diese Herausforderung ist "
           "ein Motor für Veränderung - sie zwingt Sie, kreative Lösungen zu "
           "finden und über sich hinauszuwachsen.";
  case TRIGON:
    return "Das Trigon zwischen %1 und %2 zeigt natürliche Begabungen und einen "
           "mühelosen Energiefluss. Hier fließen die Kräfte harmonisch zusammen "
           "und unterstützen sich gegenseitig. Dies ist ein Bereich, in dem "
           "Ihnen vieles leicht fällt.";
  case OPOSITION:
    return "Die Opposition zwischen %1 und %2 spiegelt ein inneres Spannungsfeld "
           "wider, das nach Integration verlangt. Oft erleben wir diese Polarität "
           "durch Beziehungen oder Projektionen auf andere. Die Aufgabe liegt "
           "darin, beide Pole in sich zu vereinen und Balance zu finden.";
  case QUINCUNX:
    return "Der Quincunx zwischen %1 und %2 zeigt zwei Lebensbereiche, die "
           "scheinbar nichts miteinander zu tun haben. Diese ungewohnte Verbindung "
           "erfordert ständige Anpassung und Feinabstimmung. Hier liegt ein "
           "verborgenes Wachstumspotenzial, das Flexibilität verlangt.";
  case HALBSEX:
    return "Das Halbsextil zwischen %1 und %2 deutet auf eine subtile Verbindung "
           "hin, die im Hintergrund wirkt. Diese leichte Reibung kann zu kleinen "
           "Anpassungen führen und fördert schrittweise Entwicklung.";
  default:
    return "Der Aspekt zwischen %1 und %2 verbindet diese Lebensbereiche auf besondere Weise.";
  }
}

QString AstroTextAnalyzer::getAscAspectText(int planet, int16_t aspect) const {
  astroTextStore().ensureLoaded();
  const QString lang = analyzerLang();
  const QString key = QString("aspect.asc.%1.%2").arg(planet).arg(static_cast<int>(aspect));
  const QString text = astroTextStore().text(lang, key, QString());
  if (!text.isEmpty()) {
    return text;
  }
  
  // Fallback: Generischer Text
  return QString("Der Aspekt zwischen %1 und dem Aszendenten beeinflusst Ihr äußeres Auftreten.")
             .arg(getPlanetName(planet));
}

QString AstroTextAnalyzer::getMcAspectText(int planet, int16_t aspect) const {
  astroTextStore().ensureLoaded();
  const QString lang = analyzerLang();
  const QString key = QString("aspect.mc.%1.%2").arg(planet).arg(static_cast<int>(aspect));
  const QString text = astroTextStore().text(lang, key, QString());
  if (!text.isEmpty()) {
    return text;
  }
  
  // Fallback: Generischer Text
  return QString("Der Aspekt zwischen %1 und dem MC beeinflusst Ihre Karriere und öffentliche Rolle.")
             .arg(getPlanetName(planet));
}

QString AstroTextAnalyzer::getPlanetPairText(int planet1, int planet2,
                                              int16_t aspect) const {
  // Sortiere Planeten für konsistente Schlüssel
  int p1 = qMin(planet1, planet2);
  int p2 = qMax(planet1, planet2);

  astroTextStore().ensureLoaded();
  const QString lang = analyzerLang();
  const QString key = QString("aspect.pair.%1.%2.%3").arg(p1).arg(p2).arg(static_cast<int>(aspect));
  const QString overrideText = astroTextStore().text(lang, key, QString());
  if (!overrideText.isEmpty()) {
    return overrideText;
  }

  // Fallback: feste Default-Texte aus initializeAspectTexts (falls vorhanden)
  const uint32_t akey = makeAspectKey(p1, p2, aspect);
  if (m_aspectTexts.contains(akey)) {
    QString v = m_aspectTexts.value(akey);
    if (v.startsWith("<p>"))
      v = v.mid(3);
    if (v.endsWith("</p>"))
      v.chop(4);
    return v;
  }

  // === GLEICHE PLANETEN (für Synastrie/Transit) ===
  if (planet1 == planet2) {
    QString planetName = getPlanetName(planet1);
    switch (aspect) {
    case KONJUNKTION:
      if (planet1 == P_SONNE)
        return "Sonne auf Sonne in Synastrie zeigt eine starke Identifikation. "
               "Sie erkennen sich im anderen und teilen grundlegende Lebensziele. "
               "Diese Verbindung kann sehr stärkend, aber auch konkurrierend wirken.";
      if (planet1 == P_MOND)
        return "Mond auf Mond in Synastrie zeigt emotionale Verwandtschaft. "
               "Sie verstehen intuitiv die Gefühle des anderen und fühlen sich "
               "emotional zu Hause miteinander.";
      if (planet1 == P_MERKUR)
        return "Merkur auf Merkur in Synastrie begünstigt geistigen Austausch. "
               "Sie denken ähnlich und kommunizieren mühelos miteinander.";
      if (planet1 == P_VENUS)
        return "Venus auf Venus in Synastrie zeigt geteilte Werte und Ästhetik. "
               "Sie mögen dieselben Dinge und haben ein harmonisches Verhältnis.";
      if (planet1 == P_MARS)
        return "Mars auf Mars in Synastrie kann große Leidenschaft oder Konflikte "
               "erzeugen. Sie teilen dieselbe Energie und Antriebsart.";
      if (planet1 == P_JUPITER)
        return "Jupiter auf Jupiter in Synastrie zeigt gemeinsame Überzeugungen "
               "und Wachstumsziele. Sie inspirieren sich gegenseitig.";
      if (planet1 == P_SATURN)
        return "Saturn auf Saturn in Synastrie verbindet durch gemeinsame "
               "Verantwortung und Reifeprozesse. Sie teilen ähnliche Prüfungen.";
      return QString("%1 auf %1 in Synastrie zeigt eine starke Resonanz "
             "in diesem Lebensbereich.").arg(planetName);
    case SEXTIL:
      return QString("Das Sextil zwischen %1 und %1 (Synastrie/Transit) "
             "ermöglicht harmonischen Austausch in diesem Lebensbereich.").arg(planetName);
    case QUADRATUR:
      return QString("Das Quadrat zwischen %1 und %1 (Synastrie/Transit) "
             "zeigt Spannungen und Herausforderungen, die Wachstum fördern.").arg(planetName);
    case TRIGON:
      return QString("Das Trigon zwischen %1 und %1 (Synastrie/Transit) "
             "schenkt natürliche Harmonie und gegenseitige Unterstützung.").arg(planetName);
    case OPOSITION:
      return QString("Die Opposition zwischen %1 und %1 (Synastrie/Transit) "
             "zeigt Polaritäten, die durch den anderen gespiegelt werden.").arg(planetName);
    case QUINCUNX:
      return QString("Der Quincunx zwischen %1 und %1 (Synastrie/Transit) "
             "erfordert ständige Anpassung und gegenseitiges Verständnis.").arg(planetName);
    case HALBSEX:
      return QString("Das Halbsextil zwischen %1 und %1 (Synastrie/Transit) "
             "zeigt eine subtile Verbindung, die Aufmerksamkeit erfordert.").arg(planetName);
    }
  }

  // === SONNE-ASPEKTE ===
  if (p1 == P_SONNE && p2 == P_MOND) {
    switch (aspect) {
    case KONJUNKTION:
      return "Sonne und Mond vereint zeigen eine starke Übereinstimmung zwischen "
             "Ihrem bewussten Willen und Ihrem emotionalen Selbst. Sie handeln "
             "aus einem Guss, Kopf und Herz sprechen dieselbe Sprache. Diese "
             "Neumond-Konstellation verleiht Fokus und Zielstrebigkeit.";
    case SEXTIL:
      return "Sonne und Mond in harmonischem Austausch ermöglichen eine gute "
             "Balance zwischen Aktivität und Ruhe, zwischen Handeln und Fühlen. "
             "Sie können Ihre Bedürfnisse gut kommunizieren und finden leicht "
             "Wege, Ihre Ziele mit Ihrem emotionalen Wohlbefinden in Einklang zu bringen.";
    case QUADRATUR:
      return "Die Spannung zwischen Sonne und Mond zeigt einen inneren Konflikt "
             "zwischen dem, was Sie wollen, und dem, was Sie brauchen. Ihr Kopf "
             "sagt ja, Ihr Bauch sagt nein - oder umgekehrt. Diese Reibung kann "
             "zu Unzufriedenheit führen, ist aber auch ein Motor für Wachstum.";
    case TRIGON:
      return "Sonne und Mond im Trigon schenken Ihnen innere Harmonie und "
             "emotionale Stabilität. Ihr Selbstbild stimmt mit Ihrem Gefühlsleben "
             "überein. Sie strahlen Authentizität aus, weil Sie mit sich im Reinen sind.";
    case OPOSITION:
      return "Sonne und Mond in Opposition (Vollmond-Konstellation) zeigen eine "
             "Polarität zwischen Ihrem bewussten Ich und Ihrem emotionalen Selbst. "
             "Sie erleben oft Spannungen zwischen Beruf und Privatleben, zwischen "
             "Selbstverwirklichung und Geborgenheit. Integration ist die Aufgabe.";
    case QUINCUNX:
      return "Der Quincunx zwischen Sonne und Mond erfordert ständige Anpassung "
             "zwischen Ihrem bewussten Willen und Ihren emotionalen Bedürfnissen. "
             "Es fällt Ihnen schwer, beide Seiten gleichzeitig zu erfüllen.";
    case HALBSEX:
      return "Das Halbsextil zwischen Sonne und Mond zeigt eine subtile Spannung "
             "zwischen Bewusstsein und Gefühl, die sanfte Integration erfordert.";
    }
  }

  if (p1 == P_SONNE && p2 == P_MERKUR) {
    // Sonne-Merkur kann maximal 28° auseinander sein (nur Konjunktion möglich in Radix)
    // Für Synastrie/Transit sind andere Aspekte möglich
    switch (aspect) {
    case KONJUNKTION:
      return "Sonne und Merkur vereint verbinden Ihr Selbstbewusstsein mit Ihrem "
             "Denken und Ihrer Kommunikation. Sie drücken sich klar und selbstsicher "
             "aus. Ihre Gedanken sind eng mit Ihrer Identität verknüpft - Sie sind, "
             "was Sie denken und sagen.";
    case SEXTIL:
      return "Das Sextil zwischen Sonne und Merkur (in Synastrie/Transit) begünstigt "
             "geistigen Austausch und die Fähigkeit, Ideen klar zu kommunizieren.";
    case QUADRATUR:
      return "Das Quadrat zwischen Sonne und Merkur (in Synastrie/Transit) kann "
             "Missverständnisse oder Meinungsverschiedenheiten anzeigen.";
    case TRIGON:
      return "Das Trigon zwischen Sonne und Merkur (in Synastrie/Transit) schenkt "
             "fließende Kommunikation und geistiges Verständnis.";
    case OPOSITION:
      return "Die Opposition von Sonne und Merkur (in Synastrie/Transit) kann "
             "zu geistigen Konflikten oder unterschiedlichen Denkweisen führen.";
    case QUINCUNX:
      return "Der Quincunx zwischen Sonne und Merkur erfordert Anpassung zwischen "
             "Selbstausdruck und geistiger Kommunikation.";
    case HALBSEX:
      return "Das Halbsextil zwischen Sonne und Merkur zeigt eine feine Verbindung "
             "zwischen Identität und Denken.";
    }
  }

  if (p1 == P_SONNE && p2 == P_VENUS) {
    switch (aspect) {
    case KONJUNKTION:
      return "Sonne und Venus vereint schenken Charme, Anmut und ein natürliches "
             "Gespür für Schönheit. Sie möchten geliebt werden und haben die Gabe, "
             "andere für sich zu gewinnen. Kreativität und Harmoniebedürfnis prägen "
             "Ihre Persönlichkeit.";
    case SEXTIL:
      return "Die harmonische Verbindung von Sonne und Venus begünstigt Ihre sozialen "
             "Fähigkeiten und Ihren künstlerischen Ausdruck. Sie verstehen es, "
             "Beziehungen zu pflegen und Schönheit in Ihr Leben zu integrieren.";
    case QUADRATUR:
      return "Die Spannung zwischen Sonne und Venus kann zu Unsicherheiten in "
             "Liebesbeziehungen führen oder ein übermäßiges Bedürfnis nach Anerkennung "
             "anzeigen. Lernen Sie, sich selbst wertzuschätzen, unabhängig von "
             "äußerer Bestätigung.";
    case TRIGON:
      return "Sonne und Venus im Trigon (in Synastrie/Transit) schenken natürlichen "
             "Charme und harmonische Beziehungen.";
    case OPOSITION:
      return "Die Opposition von Sonne und Venus (in Synastrie/Transit) kann "
             "Beziehungskonflikte oder übertriebenes Harmoniebedarf anzeigen.";
    case QUINCUNX:
      return "Der Quincunx zwischen Sonne und Venus erfordert Anpassung zwischen "
             "Selbstverwirklichung und Beziehungsbedürfnissen.";
    case HALBSEX:
      return "Das Halbsextil zwischen Sonne und Venus zeigt eine subtile Verbindung "
             "zwischen Identität und Liebe.";
    }
  }

  if (p1 == P_SONNE && p2 == P_MARS) {
    switch (aspect) {
    case KONJUNKTION:
      return "Sonne und Mars vereint verleihen enorme Energie, Durchsetzungskraft "
             "und Mut. Sie sind ein Macher, der Herausforderungen sucht und annimmt. "
             "Vorsicht vor Ungeduld und Aggressivität - kanalisieren Sie diese "
             "kraftvolle Energie konstruktiv.";
    case SEXTIL:
      return "Das harmonische Zusammenspiel von Sonne und Mars gibt Ihnen die "
             "Fähigkeit, Ihre Energie gezielt und produktiv einzusetzen. Sie können "
             "sich durchsetzen, ohne andere zu verletzen, und wissen, wann Aktion "
             "gefragt ist.";
    case QUADRATUR:
      return "Die Spannung zwischen Sonne und Mars kann zu Konflikten zwischen "
             "Ihrem Willen und Ihrem Handeln führen. Frustration und Ungeduld "
             "können aufkommen. Die Herausforderung liegt darin, Ihre Energie "
             "konstruktiv zu lenken statt zu explodieren.";
    case TRIGON:
      return "Sonne und Mars im Trigon schenken natürliche Vitalität und die "
             "Fähigkeit, Ihre Ziele mit Leichtigkeit zu verfolgen. Sie haben ein "
             "gesundes Selbstvertrauen und können sich mühelos behaupten.";
    case OPOSITION:
      return "Die Opposition von Sonne und Mars zeigt oft Konflikte mit "
             "Autoritätspersonen oder Konkurrenten. Sie projizieren Ihre eigene "
             "Kampfnatur möglicherweise auf andere. Lernen Sie, Ihre Durchsetzungskraft "
             "als Teil Ihrer selbst anzunehmen.";
    case QUINCUNX:
      return "Der Quincunx zwischen Sonne und Mars erfordert ständige Anpassung "
             "zwischen Ihrem Selbstbild und Ihrem Durchsetzungsvermögen. Energie und "
             "Identität wollen integriert werden.";
    case HALBSEX:
      return "Das Halbsextil zwischen Sonne und Mars zeigt eine feine Spannung "
             "zwischen Wille und Aktion.";
    }
  }

  if (p1 == P_SONNE && p2 == P_JUPITER) {
    switch (aspect) {
    case KONJUNKTION:
      return "Sonne und Jupiter vereint schenken Optimismus, Großzügigkeit und "
             "ein starkes Vertrauen ins Leben. Sie denken groß und haben hohe Ideale. "
             "Glück scheint Sie zu begleiten, doch achten Sie darauf, nicht zu "
             "übertreiben oder arrogant zu werden.";
    case SEXTIL:
      return "Die harmonische Verbindung von Sonne und Jupiter öffnet Türen für "
             "Wachstum und Expansion. Sie erkennen Chancen und wissen sie zu nutzen. "
             "Ihr positives Weltbild zieht günstige Gelegenheiten an.";
    case QUADRATUR:
      return "Die Spannung zwischen Sonne und Jupiter kann zu Übertreibung, "
             "Selbstüberschätzung oder Maßlosigkeit führen. Ihr Optimismus braucht "
             "einen Realitätscheck. Lernen Sie, Ihre hohen Ansprüche mit praktischen "
             "Möglichkeiten in Einklang zu bringen.";
    case TRIGON:
      return "Sonne und Jupiter im Trigon schenken natürliches Glück und eine "
             "positive Lebenseinstellung. Sie haben ein großzügiges Herz und die "
             "Fähigkeit, andere zu inspirieren. Erfolg kommt Ihnen oft mühelos zu.";
    case OPOSITION:
      return "Die Opposition von Sonne und Jupiter kann zu Konflikten zwischen "
             "persönlichen Zielen und gesellschaftlichen Erwartungen führen. Sie "
             "schwanken zwischen Selbstverwirklichung und dem Bedürfnis nach "
             "Anerkennung durch andere.";
    case QUINCUNX:
      return "Der Quincunx zwischen Sonne und Jupiter erfordert Anpassung zwischen "
             "persönlicher Identität und dem Streben nach Wachstum und Sinn.";
    case HALBSEX:
      return "Das Halbsextil zwischen Sonne und Jupiter zeigt eine subtile Verbindung "
             "zwischen Selbstausdruck und Expansion.";
    }
  }

  if (p1 == P_SONNE && p2 == P_SATURN) {
    switch (aspect) {
    case KONJUNKTION:
      return "Sonne und Saturn vereint prägen eine ernste, verantwortungsbewusste "
             "Persönlichkeit. Sie sind diszipliniert und ausdauernd, aber vielleicht "
             "auch zu streng mit sich selbst. Ihr Weg zum Erfolg führt über harte "
             "Arbeit und Geduld.";
    case SEXTIL:
      return "Die harmonische Verbindung von Sonne und Saturn gibt Ihnen die "
             "Fähigkeit, langfristige Ziele mit Disziplin zu verfolgen. Sie verbinden "
             "Ambition mit Realismus und können Verantwortung übernehmen.";
    case QUADRATUR:
      return "Die Spannung zwischen Sonne und Saturn kann Selbstzweifel, Hemmungen "
             "oder das Gefühl von Einschränkung erzeugen. Autoritätskonflikte sind "
             "möglich. Die Herausforderung liegt darin, Ihre eigene innere Autorität "
             "zu entwickeln.";
    case TRIGON:
      return "Sonne und Saturn im Trigon verleihen natürliche Reife, Strukturfähigkeit "
             "und die Gabe, Verantwortung zu übernehmen. Sie sind zuverlässig und "
             "bauen systematisch auf, was von Dauer sein soll.";
    case OPOSITION:
      return "Die Opposition von Sonne und Saturn zeigt oft Spannungen mit "
             "Vaterfiguren oder Autoritäten. Sie fühlen sich möglicherweise "
             "eingeschränkt oder nicht anerkannt. Lernen Sie, Ihre eigene Autorität "
             "zu leben.";
    case QUINCUNX:
      return "Der Quincunx zwischen Sonne und Saturn erfordert ständige Anpassung "
             "zwischen Selbstentfaltung und den Anforderungen der Realität.";
    case HALBSEX:
      return "Das Halbsextil zwischen Sonne und Saturn zeigt eine feine Spannung "
             "zwischen Lebensfreude und Verantwortung.";
    }
  }

  // === SONNE MIT ÄUSSEREN PLANETEN ===
  if (p1 == P_SONNE && p2 == P_URANUS) {
    switch (aspect) {
    case KONJUNKTION:
      return "Sonne und Uranus vereint prägen eine originelle, unabhängige "
             "Persönlichkeit. Sie sind ein Individualist, der seinen eigenen Weg geht. "
             "Rebellion gegen Konventionen und plötzliche Veränderungen sind möglich.";
    case SEXTIL:
      return "Die harmonische Verbindung von Sonne und Uranus begünstigt Originalität "
             "und die Fähigkeit, neue Ideen zu integrieren, ohne alles umzuwerfen.";
    case QUADRATUR:
      return "Die Spannung zwischen Sonne und Uranus kann zu Rebellion, Ruhelosigkeit "
             "oder plötzlichen Umbrüchen führen. Sie schwanken zwischen Anpassung und "
             "radikaler Unabhängigkeit.";
    case TRIGON:
      return "Sonne und Uranus im Trigon schenken natürliche Originalität und die "
             "Fähigkeit, Ihre Einzigartigkeit mühelos auszuleben.";
    case OPOSITION:
      return "Die Opposition von Sonne und Uranus zeigt Konflikte zwischen "
             "Selbstverwirklichung und Freiheitsdrang. Sie projizieren möglicherweise "
             "Ihre eigene Rebellion auf andere.";
    case QUINCUNX:
      return "Der Quincunx zwischen Sonne und Uranus erfordert Anpassung zwischen "
             "stabiler Identität und dem Drang nach Veränderung.";
    case HALBSEX:
      return "Das Halbsextil zwischen Sonne und Uranus zeigt eine subtile Verbindung "
             "zwischen Selbstausdruck und Innovation.";
    }
  }

  if (p1 == P_SONNE && p2 == P_NEPTUN) {
    switch (aspect) {
    case KONJUNKTION:
      return "Sonne und Neptun vereint schenken künstlerische Sensibilität und "
             "spirituelle Empfindsamkeit. Ihre Identität ist durchlässig und empfänglich. "
             "Achten Sie auf klare Grenzen und Realitätsbezug.";
    case SEXTIL:
      return "Die harmonische Verbindung von Sonne und Neptun begünstigt Kreativität, "
             "Empathie und einen Zugang zu höheren Inspirationen.";
    case QUADRATUR:
      return "Die Spannung zwischen Sonne und Neptun kann zu Identitätsverwirrung, "
             "Realitätsflucht oder Täuschungen führen. Unterscheiden Sie zwischen "
             "Vision und Illusion.";
    case TRIGON:
      return "Sonne und Neptun im Trigon schenken natürliche Intuition, künstlerisches "
             "Talent und spirituelle Offenheit.";
    case OPOSITION:
      return "Die Opposition von Sonne und Neptun kann zu Verwirrung über die eigene "
             "Identität oder Täuschung durch andere führen.";
    case QUINCUNX:
      return "Der Quincunx zwischen Sonne und Neptun erfordert Anpassung zwischen "
             "klarer Selbstdefinition und spiritueller Durchlässigkeit.";
    case HALBSEX:
      return "Das Halbsextil zwischen Sonne und Neptun zeigt eine feine Verbindung "
             "zwischen Ego und Transzendenz.";
    }
  }

  if (p1 == P_SONNE && p2 == P_PLUTO) {
    switch (aspect) {
    case KONJUNKTION:
      return "Sonne und Pluto vereint verleihen intensive Willenskraft und "
             "Transformationskraft. Sie gehen den Dingen auf den Grund und haben "
             "ein starkes Bedürfnis nach Kontrolle und Macht. Tiefgreifende "
             "Selbsterneuerung ist möglich.";
    case SEXTIL:
      return "Die harmonische Verbindung von Sonne und Pluto gibt Ihnen die Fähigkeit, "
             "sich tiefgreifend zu erneuern und verborgene Ressourcen zu nutzen.";
    case QUADRATUR:
      return "Die Spannung zwischen Sonne und Pluto kann zu Machtkämpfen, "
             "Kontrollbedürfnis oder intensiven Krisen führen. Transformation "
             "geschieht durch Loslassen.";
    case TRIGON:
      return "Sonne und Pluto im Trigon schenken natürliche Intensität, Führungsqualitäten "
             "und die Fähigkeit zur Selbsterneuerung.";
    case OPOSITION:
      return "Die Opposition von Sonne und Pluto kann intensive Machtkämpfe oder "
             "Projektionen anzeigen. Sie erleben Ihre eigene Intensität durch andere.";
    case QUINCUNX:
      return "Der Quincunx zwischen Sonne und Pluto erfordert Anpassung zwischen "
             "bewusstem Selbstausdruck und tieferen Transformationsprozessen.";
    case HALBSEX:
      return "Das Halbsextil zwischen Sonne und Pluto zeigt eine subtile Verbindung "
             "zwischen Identität und Transformation.";
    }
  }

  // === MOND-ASPEKTE ===
  if (p1 == P_MOND && p2 == P_MERKUR) {
    switch (aspect) {
    case KONJUNKTION:
      return "Mond und Merkur vereint verbinden Gefühl und Verstand eng miteinander. "
             "Sie denken mit dem Herzen und fühlen mit dem Kopf. Ihre Kommunikation "
             "ist emotional gefärbt, und Sie haben ein gutes Gespür für die Stimmungen "
             "anderer.";
    case SEXTIL:
      return "Die harmonische Verbindung von Mond und Merkur ermöglicht es Ihnen, "
             "Ihre Gefühle klar auszudrücken. Sie können gut zuhören und haben ein "
             "Talent dafür, emotionale Situationen zu verstehen und zu vermitteln.";
    case QUADRATUR:
      return "Die Spannung zwischen Mond und Merkur kann zu innerem Zwist zwischen "
             "Logik und Gefühl führen. Manchmal reden Sie sich um Kopf und Kragen oder "
             "rationalisieren Ihre Emotionen weg. Lernen Sie, beide Seiten zu integrieren.";
    case TRIGON:
      return "Mond und Merkur im Trigon schenken emotionale Intelligenz und die "
             "Fähigkeit, Gefühle eloquent auszudrücken. Sie verstehen instinktiv, "
             "was andere bewegt, und können trösten und vermitteln.";
    case OPOSITION:
      return "Die Opposition von Mond und Merkur zeigt einen inneren Konflikt zwischen "
             "Verstand und Gefühl. Sie analysieren Ihre Emotionen oder projizieren "
             "Ihre Gedanken auf andere. Integration von Kopf und Herz ist gefragt.";
    case QUINCUNX:
      return "Der Quincunx zwischen Mond und Merkur erfordert ständige Anpassung "
             "zwischen intuitivem Fühlen und rationalem Denken. Was Sie fühlen und "
             "was Sie denken, scheint nicht zusammenzupassen.";
    case HALBSEX:
      return "Das Halbsextil zwischen Mond und Merkur deutet auf eine subtile "
             "Verbindung zwischen Gefühl und Verstand hin, die bewusste Integration erfordert.";
    }
  }

  if (p1 == P_MOND && p2 == P_VENUS) {
    switch (aspect) {
    case KONJUNKTION:
      return "Mond und Venus vereint schenken ein tiefes Bedürfnis nach Harmonie, "
             "Liebe und Schönheit. Sie sind emotional warmherzig und haben ein "
             "natürliches Talent für zwischenmenschliche Beziehungen. Kunst und "
             "Genuss nähren Ihre Seele.";
    case SEXTIL:
      return "Die harmonische Verbindung von Mond und Venus begünstigt liebevolle "
             "Beziehungen und ein ausgeglichenes Gefühlsleben. Sie verstehen es, "
             "Ihr Zuhause gemütlich zu gestalten und Harmonie zu schaffen.";
    case QUADRATUR:
      return "Die Spannung zwischen Mond und Venus kann emotionale Bedürfnisse "
             "und Beziehungswünsche in Konflikt bringen. Vielleicht fällt es Ihnen "
             "schwer, sich geliebt zu fühlen, oder Sie haben widersprüchliche "
             "Vorstellungen von Nähe.";
    case TRIGON:
      return "Mond und Venus im Trigon schenken ein natürliches Talent für Liebe "
             "und Beziehungen. Sie fühlen sich in harmonischer Umgebung wohl und "
             "haben ein feines Gespür für Ästhetik.";
    case OPOSITION:
      return "Die Opposition von Mond und Venus kann Konflikte zwischen emotionalen "
             "Bedürfnissen und Beziehungswünschen anzeigen. Sie projizieren möglicherweise "
             "Ihre Liebessehnsucht auf Partner oder erleben Spannungen zwischen Geben und Nehmen.";
    case QUINCUNX:
      return "Der Quincunx zwischen Mond und Venus erfordert ständige Nachjustierung "
             "zwischen dem, was Sie emotional brauchen, und dem, was Sie in Beziehungen "
             "suchen. Selbstfürsorge und Partnerliebe wollen ausbalanciert werden.";
    case HALBSEX:
      return "Das Halbsextil zwischen Mond und Venus deutet auf eine feine Spannung "
             "zwischen Gefühlsleben und Beziehungsbedürfnissen hin, die sanfte Aufmerksamkeit erfordert.";
    }
  }

  if (p1 == P_MOND && p2 == P_MARS) {
    switch (aspect) {
    case KONJUNKTION:
      return "Mond und Mars vereint erzeugen intensive, leidenschaftliche Gefühle. "
             "Sie reagieren spontan und manchmal impulsiv auf emotionale Situationen. "
             "Diese Konstellation kann große Leidenschaft, aber auch emotionale "
             "Explosivität anzeigen.";
    case SEXTIL:
      return "Die harmonische Verbindung von Mond und Mars gibt Ihnen emotionalen "
             "Mut und die Fähigkeit, für Ihre Bedürfnisse einzustehen. Sie können "
             "Ihre Gefühle in konstruktive Aktion umsetzen.";
    case QUADRATUR:
      return "Die Spannung zwischen Mond und Mars kann zu emotionalen Ausbrüchen "
             "oder innerem Ärger führen. Ungeduld und Reizbarkeit sind mögliche "
             "Themen. Lernen Sie, Ihre emotionale Energie konstruktiv zu kanalisieren.";
    case TRIGON:
      return "Mond und Mars im Trigon verbinden Gefühl und Tatkraft harmonisch. "
             "Sie können Ihre Emotionen in Handlung umsetzen und haben ein "
             "gesundes emotionales Selbstvertrauen.";
    case OPOSITION:
      return "Die Opposition von Mond und Mars kann emotionale Konflikte in "
             "Beziehungen anzeigen - besonders zwischen Nähebedürfnis und "
             "Unabhängigkeitsdrang. Lernen Sie, beide Pole zu integrieren.";
    case QUINCUNX:
      return "Der Quincunx zwischen Mond und Mars erfordert ständige Anpassung "
             "zwischen Ihrem Bedürfnis nach emotionaler Sicherheit und Ihrem "
             "Drang zu handeln. Sanftheit und Kraft wollen integriert werden.";
    case HALBSEX:
      return "Das Halbsextil zwischen Mond und Mars deutet auf eine subtile Reibung "
             "zwischen Gefühl und Aktion hin, die zu kleinen Anpassungen führt.";
    }
  }

  if (p1 == P_MOND && p2 == P_JUPITER) {
    switch (aspect) {
    case KONJUNKTION:
      return "Mond und Jupiter vereint schenken emotionale Großzügigkeit und "
             "ein tiefes Vertrauen ins Leben. Sie haben ein großes Herz und "
             "sind von Natur aus optimistisch. Ihre Gefühle sind weit und "
             "expansiv - manchmal vielleicht zu weit.";
    case SEXTIL:
      return "Die harmonische Verbindung von Mond und Jupiter begünstigt "
             "emotionales Wohlbefinden und Lebensfreude. Sie finden leicht "
             "Zugang zu innerer Fülle und können andere mit Ihrer positiven "
             "Ausstrahlung anstecken.";
    case QUADRATUR:
      return "Die Spannung zwischen Mond und Jupiter kann zu emotionaler "
             "Maßlosigkeit oder übertriebenen Erwartungen führen. Sie schwanken "
             "zwischen Euphorie und Enttäuschung. Lernen Sie, Ihre emotionalen "
             "Höhenflüge zu erden.";
    case TRIGON:
      return "Mond und Jupiter im Trigon schenken natürliches emotionales Glück "
             "und eine tiefe innere Zufriedenheit. Sie haben ein Talent, andere "
             "zu ermutigen und Optimismus zu verbreiten.";
    case OPOSITION:
      return "Die Opposition von Mond und Jupiter zeigt einen Konflikt zwischen "
             "emotionalen Bedürfnissen und dem Drang nach Expansion. Sie suchen "
             "immer nach mehr, während ein Teil von Ihnen einfach Geborgenheit "
             "braucht.";
    case QUINCUNX:
      return "Der Quincunx zwischen Mond und Jupiter zeigt eine unbequeme "
             "Verbindung zwischen Ihrem emotionalen Sicherheitsbedürfnis und "
             "Ihrem Wunsch nach Wachstum und Horizonterweiterung. Sie müssen "
             "ständig nachjustieren, wie viel Abenteuer Ihre Seele verträgt.";
    case HALBSEX:
      return "Das Halbsextil zwischen Mond und Jupiter deutet auf eine feine "
             "Spannung zwischen emotionaler Geborgenheit und Expansionsdrang hin.";
    }
  }

  if (p1 == P_MOND && p2 == P_SATURN) {
    switch (aspect) {
    case KONJUNKTION:
      return "Mond und Saturn vereint können emotionale Tiefe mit "
             "Verantwortungsbewusstsein verbinden, aber auch Schwermut oder "
             "emotionale Hemmungen anzeigen. Sie nehmen Gefühle ernst und "
             "brauchen Zeit, um sich zu öffnen.";
    case SEXTIL:
      return "Die harmonische Verbindung von Mond und Saturn gibt Ihren "
             "Gefühlen Stabilität und Ausdauer. Sie können emotionale "
             "Verantwortung übernehmen und sind für andere ein Fels in der "
             "Brandung.";
    case QUADRATUR:
      return "Die Spannung zwischen Mond und Saturn kann emotionale Blockaden, "
             "Angst vor Ablehnung oder ein Gefühl des Nicht-genug-seins "
             "erzeugen. Alte Wunden aus der Kindheit können hier eine Rolle "
             "spielen. Selbstmitgefühl ist der Schlüssel.";
    case TRIGON:
      return "Mond und Saturn im Trigon verbinden Gefühl mit Struktur auf "
             "natürliche Weise. Sie haben emotionale Reife und können auch in "
             "schwierigen Zeiten Ihren inneren Halt bewahren.";
    case OPOSITION:
      return "Die Opposition von Mond und Saturn zeigt einen Konflikt zwischen "
             "emotionalen Bedürfnissen und Pflichten oder Einschränkungen. Sie "
             "fühlen sich manchmal zwischen Nähe und Distanz, zwischen Weichheit "
             "und Härte hin- und hergerissen.";
    case QUINCUNX:
      return "Der Quincunx zwischen Mond und Saturn erfordert ständige Anpassung "
             "zwischen Ihrem Bedürfnis nach emotionaler Wärme und den Anforderungen "
             "der Realität. Sie müssen lernen, Struktur und Gefühl in Einklang "
             "zu bringen, ohne eines zu unterdrücken.";
    case HALBSEX:
      return "Das Halbsextil zwischen Mond und Saturn zeigt eine leichte Reibung "
             "zwischen Gefühlsbedürfnissen und Verantwortung, die schrittweise Integration erfordert.";
    }
  }

  if (p1 == P_MOND && p2 == P_URANUS) {
    switch (aspect) {
    case KONJUNKTION:
      return "Mond und Uranus vereint erzeugen ein ungewöhnliches Gefühlsleben. "
             "Sie sind emotional unabhängig, manchmal exzentrisch, und brauchen "
             "Freiheit in Beziehungen. Plötzliche Stimmungsumschwünge sind möglich.";
    case SEXTIL:
      return "Die harmonische Verbindung von Mond und Uranus schenkt emotionale "
             "Originalität und die Fähigkeit, festgefahrene Gefühlsmuster zu "
             "durchbrechen. Sie sind offen für neue emotionale Erfahrungen.";
    case QUADRATUR:
      return "Die Spannung zwischen Mond und Uranus kann emotionale Unruhe, "
             "Bindungsangst oder plötzliche Trennungen anzeigen. Sie rebellieren "
             "gegen emotionale Einengung, brauchen aber auch Stabilität.";
    case TRIGON:
      return "Mond und Uranus im Trigon verbinden Gefühl mit Intuition auf "
             "inspirierende Weise. Sie haben einen einzigartigen emotionalen "
             "Ausdruck und können anderen neue Perspektiven eröffnen.";
    case OPOSITION:
      return "Die Opposition von Mond und Uranus kann Konflikte zwischen "
             "Sicherheitsbedürfnis und Freiheitsdrang anzeigen. Sie projizieren "
             "möglicherweise Ihre eigene Unruhe auf Partner.";
    case QUINCUNX:
      return "Der Quincunx zwischen Mond und Uranus zeigt eine unbehagliche "
             "Spannung zwischen Ihrem Bedürfnis nach emotionaler Geborgenheit "
             "und Ihrem Drang nach Unabhängigkeit und Veränderung.";
    case HALBSEX:
      return "Das Halbsextil zwischen Mond und Uranus deutet auf eine subtile "
             "Spannung zwischen Tradition und Innovation im Gefühlsleben hin.";
    }
  }

  if (p1 == P_MOND && p2 == P_NEPTUN) {
    switch (aspect) {
    case KONJUNKTION:
      return "Mond und Neptun vereint schenken tiefe Sensibilität, Intuition "
             "und künstlerische Begabung. Sie sind wie ein Schwamm für "
             "Stimmungen. Grenzenlosigkeit kann aber auch zu Verwirrung führen.";
    case SEXTIL:
      return "Die harmonische Verbindung von Mond und Neptun begünstigt "
             "Einfühlungsvermögen, Kreativität und spirituelle Offenheit. Sie "
             "haben ein feines Gespür für die unsichtbaren Strömungen des Lebens.";
    case QUADRATUR:
      return "Die Spannung zwischen Mond und Neptun kann zu Illusionen, "
             "emotionaler Verwirrung oder Abhängigkeitstendenzen führen. "
             "Unterscheiden Sie zwischen Intuition und Wunschdenken.";
    case TRIGON:
      return "Mond und Neptun im Trigon schenken natürliche Empathie und "
             "einen Zugang zu höheren Gefühlsebenen. Sie können heilen und "
             "trösten, weil Sie wirklich mitfühlen.";
    case OPOSITION:
      return "Die Opposition von Mond und Neptun kann emotionale Täuschung "
             "oder Selbsttäuschung anzeigen. Sie idealisieren möglicherweise "
             "Partner oder fliehen vor der Realität in Träume.";
    case QUINCUNX:
      return "Der Quincunx zwischen Mond und Neptun erfordert ständige "
             "Nachjustierung zwischen Ihrem Bedürfnis nach klaren emotionalen "
             "Grenzen und Ihrer durchlässigen, empfindsamen Natur.";
    case HALBSEX:
      return "Das Halbsextil zwischen Mond und Neptun deutet auf eine feine "
             "Verbindung zwischen Realitätssinn und Träumerei hin.";
    }
  }

  if (p1 == P_MOND && p2 == P_PLUTO) {
    switch (aspect) {
    case KONJUNKTION:
      return "Mond und Pluto vereint erzeugen intensive, tiefgründige Gefühle. "
             "Sie erleben Emotionen mit extremer Intensität und haben die "
             "Fähigkeit zur emotionalen Transformation. Kontrolle und Macht "
             "in Beziehungen können Themen sein.";
    case SEXTIL:
      return "Die harmonische Verbindung von Mond und Pluto gibt Ihnen "
             "emotionale Tiefe und die Fähigkeit, psychologisch zu verstehen. "
             "Sie können emotionale Krisen als Wachstumschancen nutzen.";
    case QUADRATUR:
      return "Die Spannung zwischen Mond und Pluto kann emotionale Machtkämpfe, "
             "Eifersucht oder zwanghafte Gefühle anzeigen. Alte Traumata wollen "
             "verarbeitet werden. Die Kraft zur Transformation liegt in Ihnen.";
    case TRIGON:
      return "Mond und Pluto im Trigon schenken emotionale Regenerationskraft. "
             "Sie können durch tiefe Krisen gehen und gestärkt daraus "
             "hervorgehen. Ihre emotionale Intuition ist mächtig.";
    case OPOSITION:
      return "Die Opposition von Mond und Pluto kann intensive emotionale "
             "Konflikte in Beziehungen anzeigen. Sie projizieren möglicherweise "
             "Ihre eigene Intensität auf andere und erleben Machtkämpfe.";
    case QUINCUNX:
      return "Der Quincunx zwischen Mond und Pluto zeigt eine unbequeme "
             "Verbindung zwischen Ihrem Bedürfnis nach emotionaler Sicherheit "
             "und tiefen Transformationsprozessen, die Altes sterben lassen.";
    case HALBSEX:
      return "Das Halbsextil zwischen Mond und Pluto zeigt eine subtile "
             "Verbindung zwischen Alltagsgefühlen und tieferen Transformationsprozessen.";
    }
  }

  // === MERKUR-ASPEKTE ===
  if (p1 == P_MERKUR && p2 == P_VENUS) {
    switch (aspect) {
    case KONJUNKTION:
      return "Merkur und Venus vereint schenken eine gewandte, charmante "
             "Ausdrucksweise. Sie haben ein Talent für Diplomatie, Poesie und "
             "künstlerische Kommunikation. Ihre Worte sind wohlklingend und "
             "können andere bezaubern.";
    case SEXTIL:
      return "Die harmonische Verbindung von Merkur und Venus begünstigt "
             "angenehme Gespräche, künstlerischen Ausdruck und diplomatisches "
             "Geschick. Sie können Schönheit in Worte fassen und haben ein "
             "feines Gespür für sprachliche Nuancen.";
    case QUADRATUR:
      return "Die Spannung zwischen Merkur und Venus kann zu Konflikten zwischen "
             "Kopf und Herz in der Kommunikation führen. Vielleicht fällt es Ihnen "
             "schwer, Ihre Zuneigung auszudrücken, oder Sie wirken oberflächlich.";
    case TRIGON:
      return "Merkur und Venus im Trigon schenken natürlichen Charme und die "
             "Fähigkeit, angenehme Gespräche zu führen. Kunst und Kommunikation verbinden sich.";
    case OPOSITION:
      return "Die Opposition von Merkur und Venus kann innere Konflikte zwischen "
             "rationalen und ästhetischen Bedürfnissen anzeigen.";
    case QUINCUNX:
      return "Der Quincunx zwischen Merkur und Venus erfordert Anpassung zwischen "
             "klarer Kommunikation und harmonischem Ausdruck.";
    case HALBSEX:
      return "Das Halbsextil zwischen Merkur und Venus deutet auf eine feine "
             "Spannung zwischen Denken und Genießen hin.";
    }
  }

  if (p1 == P_MERKUR && p2 == P_MARS) {
    switch (aspect) {
    case KONJUNKTION:
      return "Merkur und Mars vereint erzeugen einen scharfen, schnellen Verstand. "
             "Sie denken und sprechen direkt, manchmal scharf oder streitlustig. "
             "Ihr Intellekt ist kampfbereit - nutzen Sie diese Energie für Debatten "
             "und mutige Ideen.";
    case SEXTIL:
      return "Die harmonische Verbindung von Merkur und Mars gibt Ihrem Denken "
             "Durchsetzungskraft und Ihren Worten Wirkung. Sie können Ihre Ideen "
             "überzeugend vertreten und schnell reagieren.";
    case QUADRATUR:
      return "Die Spannung zwischen Merkur und Mars kann zu vorschnellem Reden, "
             "scharfer Kritik oder Streitlust führen. Ihr Verstand ist schnell, "
             "aber manchmal zu aggressiv. Lernen Sie, Ihre mentale Energie zu zügeln.";
    case TRIGON:
      return "Merkur und Mars im Trigon verleihen geistige Schlagfertigkeit und "
             "die Fähigkeit, Ihre Gedanken energisch, aber geschickt zu vertreten.";
    case OPOSITION:
      return "Die Opposition von Merkur und Mars kann zu verbalen Konflikten oder "
             "dem Gefühl führen, Ihre Ideen verteidigen zu müssen.";
    case QUINCUNX:
      return "Der Quincunx zwischen Merkur und Mars erfordert Anpassung zwischen "
             "überlegtem Denken und impulsivem Handeln.";
    case HALBSEX:
      return "Das Halbsextil zwischen Merkur und Mars deutet auf eine feine Reibung "
             "zwischen Nachdenken und Losstürmen hin.";
    }
  }

  if (p1 == P_MERKUR && p2 == P_JUPITER) {
    switch (aspect) {
    case KONJUNKTION:
      return "Merkur und Jupiter vereint schenken ein weites, optimistisches Denken. "
             "Sie interessieren sich für das große Bild, Philosophie und Bildung. "
             "Ihr Geist ist expansiv - achten Sie darauf, nicht zu viel auf einmal "
             "zu wollen.";
    case SEXTIL:
      return "Die harmonische Verbindung von Merkur und Jupiter begünstigt "
             "positives Denken, Lernfähigkeit und erfolgreiche Kommunikation. "
             "Sie können komplexe Themen verständlich vermitteln.";
    case QUADRATUR:
      return "Die Spannung zwischen Merkur und Jupiter kann zu Übertreibung, "
             "Weitschweifigkeit oder unrealistischen Plänen führen. Ihr Denken "
             "ist groß - manchmal zu groß. Bringen Sie Ihre Visionen auf den Boden.";
    case TRIGON:
      return "Merkur und Jupiter im Trigon schenken natürliche Weisheit und die "
             "Gabe, Wissen zu teilen. Sie sind ein geborener Lehrer und Philosoph.";
    case OPOSITION:
      return "Die Opposition von Merkur und Jupiter kann zu übertriebenen Versprechungen "
             "oder Konflikten zwischen Detailarbeit und dem großen Bild führen. Sie "
             "schwanken zwischen praktischem Denken und philosophischen Höhenflügen.";
    case QUINCUNX:
      return "Der Quincunx zwischen Merkur und Jupiter erfordert ständige Anpassung "
             "zwischen Ihrem analytischen Verstand und Ihrem Bedürfnis nach Sinnsuche. "
             "Kleine Fakten und große Wahrheiten wollen integriert werden.";
    case HALBSEX:
      return "Das Halbsextil zwischen Merkur und Jupiter zeigt eine subtile Spannung "
             "zwischen Detailgenauigkeit und großzügigem Denken.";
    }
  }

  if (p1 == P_MERKUR && p2 == P_SATURN) {
    switch (aspect) {
    case KONJUNKTION:
      return "Merkur und Saturn vereint erzeugen ein ernstes, strukturiertes Denken. "
             "Sie sind gründlich, methodisch und haben ein gutes Gedächtnis. "
             "Manchmal neigen Sie zu Pessimismus oder Selbstzweifeln im Denken.";
    case SEXTIL:
      return "Die harmonische Verbindung von Merkur und Saturn gibt Ihrem Denken "
             "Tiefe und Struktur. Sie können komplexe Probleme systematisch lösen "
             "und haben Ausdauer bei geistiger Arbeit.";
    case QUADRATUR:
      return "Die Spannung zwischen Merkur und Saturn kann mentale Blockaden, "
             "Kommunikationshemmungen oder negatives Denken erzeugen. Lernen Sie, "
             "Ihre kritische Stimme konstruktiv einzusetzen.";
    case TRIGON:
      return "Merkur und Saturn im Trigon verleihen einen klaren, logischen "
             "Verstand und die Fähigkeit zu konzentrierter geistiger Arbeit.";
    case OPOSITION:
      return "Die Opposition von Merkur und Saturn kann zu Kommunikationsblockaden "
             "oder dem Gefühl führen, nicht gehört zu werden. Sie kämpfen zwischen "
             "Selbstausdruck und Selbstzensur.";
    case QUINCUNX:
      return "Der Quincunx zwischen Merkur und Saturn zeigt eine unbequeme Verbindung "
             "zwischen Ihrem Bedürfnis, frei zu denken, und dem Druck, ernst genommen "
             "zu werden. Leichtigkeit und Tiefe wollen verbunden werden.";
    case HALBSEX:
      return "Das Halbsextil zwischen Merkur und Saturn deutet auf eine feine Spannung "
             "zwischen spontanem Ausdruck und gründlicher Überlegung hin.";
    }
  }

  if (p1 == P_MERKUR && p2 == P_URANUS) {
    switch (aspect) {
    case KONJUNKTION:
      return "Merkur und Uranus vereint schenken einen blitzschnellen, originellen "
             "Geist. Sie denken unkonventionell und haben geniale Einfälle. Manchmal "
             "ist Ihr Denken zu sprunghaft - lernen Sie, Ihre Ideen zu erden.";
    case SEXTIL:
      return "Die harmonische Verbindung von Merkur und Uranus begünstigt "
             "innovative Ideen und die Fähigkeit, über den Tellerrand zu schauen. "
             "Sie sind geistig flexibel und offen für Neues.";
    case QUADRATUR:
      return "Die Spannung zwischen Merkur und Uranus kann zu Nervosität, "
             "geistiger Unruhe oder exzentrischen Ideen führen. Ihr Geist rast - "
             "lernen Sie, ihn zu beruhigen und zu fokussieren.";
    case TRIGON:
      return "Merkur und Uranus im Trigon schenken intuitive Intelligenz und "
             "die Gabe, Zukunftstrends zu erkennen. Ihre Ideen sind ihrer Zeit voraus.";
    case OPOSITION:
      return "Die Opposition von Merkur und Uranus kann zu Nervosität, rebellischem "
             "Denken oder Kommunikationschaos führen. Sie provozieren gerne mit Ihren "
             "Ideen oder fühlen sich von anderen missverstanden.";
    case QUINCUNX:
      return "Der Quincunx zwischen Merkur und Uranus erfordert Anpassung zwischen "
             "konventionellem und revolutionärem Denken. Sie müssen lernen, Ihre "
             "genialen Einfälle so zu vermitteln, dass andere folgen können.";
    case HALBSEX:
      return "Das Halbsextil zwischen Merkur und Uranus zeigt eine subtile Verbindung "
             "zwischen strukturiertem und originellem Denken.";
    }
  }

  if (p1 == P_MERKUR && p2 == P_NEPTUN) {
    switch (aspect) {
    case KONJUNKTION:
      return "Merkur und Neptun vereint verbinden Denken mit Intuition und "
             "Imagination. Sie haben ein Talent für Poesie, Musik und visionäres "
             "Denken. Achten Sie darauf, nicht zu verträumt oder unklar zu werden.";
    case SEXTIL:
      return "Die harmonische Verbindung von Merkur und Neptun schenkt künstlerische "
             "Sensibilität und die Fähigkeit, das Unsagbare auszudrücken. Sie haben "
             "ein feines Gespür für Stimmungen und Symbole.";
    case QUADRATUR:
      return "Die Spannung zwischen Merkur und Neptun kann zu Verwirrung, "
             "Unklarheit im Denken oder Täuschung führen. Unterscheiden Sie "
             "zwischen Intuition und Wunschdenken. Erdung ist wichtig.";
    case TRIGON:
      return "Merkur und Neptun im Trigon schenken poetische Begabung und "
             "intuitive Einsichten. Sie können das Mystische in Worte fassen.";
    case OPOSITION:
      return "Die Opposition von Merkur und Neptun kann zu Missverständnissen, "
             "Täuschungen oder dem Gefühl führen, nicht klar kommunizieren zu können. "
             "Unterscheiden Sie zwischen Inspiration und Illusion.";
    case QUINCUNX:
      return "Der Quincunx zwischen Merkur und Neptun zeigt eine unbehagliche Spannung "
             "zwischen logischem Denken und intuitiver Wahrnehmung. Sie müssen lernen, "
             "beiden Seiten Raum zu geben, ohne sich zu verlieren.";
    case HALBSEX:
      return "Das Halbsextil zwischen Merkur und Neptun deutet auf eine feine Verbindung "
             "zwischen klarem Denken und träumerischer Intuition hin.";
    }
  }

  if (p1 == P_MERKUR && p2 == P_PLUTO) {
    switch (aspect) {
    case KONJUNKTION:
      return "Merkur und Pluto vereint verleihen einen durchdringenden, "
             "forschenden Geist. Sie gehen den Dingen auf den Grund und haben "
             "ein Talent für Psychologie und Geheimnisse. Ihr Wort hat Macht.";
    case SEXTIL:
      return "Die harmonische Verbindung von Merkur und Pluto gibt Ihrem Denken "
             "Tiefe und Transformationskraft. Sie können versteckte Wahrheiten "
             "erkennen und andere mit Ihren Worten bewegen.";
    case QUADRATUR:
      return "Die Spannung zwischen Merkur und Pluto kann zu obsessivem Denken, "
             "manipulativer Kommunikation oder Machtkämpfen durch Worte führen. "
             "Nutzen Sie Ihre mentale Intensität konstruktiv.";
    case TRIGON:
      return "Merkur und Pluto im Trigon schenken die Fähigkeit, zum Kern "
             "jeder Sache vorzudringen. Ihr Verstand ist ein mächtiges Werkzeug "
             "für Transformation.";
    case OPOSITION:
      return "Die Opposition von Merkur und Pluto kann zu mentalen Machtkämpfen oder "
             "dem Gefühl führen, dass Ihre Gedanken von außen kontrolliert werden. "
             "Lernen Sie, Ihre geistige Intensität als Stärke zu nutzen.";
    case QUINCUNX:
      return "Der Quincunx zwischen Merkur und Pluto erfordert ständige Anpassung "
             "zwischen oberflächlicher Kommunikation und dem Drang, alles tiefgründig "
             "zu analysieren. Nicht alles muss auf den Grund gegangen werden.";
    case HALBSEX:
      return "Das Halbsextil zwischen Merkur und Pluto zeigt eine subtile Verbindung "
             "zwischen Alltagskommunikation und tiefgründiger Analyse.";
    }
  }

  // === VENUS-ASPEKTE ===
  if (p1 == P_VENUS && p2 == P_MARS) {
    switch (aspect) {
    case KONJUNKTION:
      return "Venus und Mars vereint - die große Liebeskonstellation! Hier "
             "verbinden sich weibliches und männliches Prinzip, Anziehung und "
             "Begehren, Empfangen und Erobern. Sie haben eine starke erotische "
             "Ausstrahlung und leben Ihre Leidenschaft intensiv.";
    case SEXTIL:
      return "Venus und Mars in harmonischem Austausch ermöglichen eine "
             "ausgewogene Verbindung von Liebe und Leidenschaft. Sie wissen, "
             "wann Sie zärtlich und wann Sie leidenschaftlich sein sollten. "
             "Beziehungen profitieren von dieser Balance.";
    case QUADRATUR:
      return "Die Spannung zwischen Venus und Mars kann Konflikte zwischen "
             "Herz und Begehren erzeugen. Vielleicht wollen Sie jemanden, den "
             "Sie nicht lieben, oder lieben jemanden, den Sie nicht wollen. "
             "Diese Reibung kann aber auch große kreative Energie freisetzen.";
    case TRIGON:
      return "Venus und Mars im Trigon schenken natürlichen Charme und die "
             "Fähigkeit, Liebe und Leidenschaft harmonisch zu verbinden. Sie "
             "sind attraktiv und wissen, was Sie in Beziehungen wollen.";
    case OPOSITION:
      return "Die Opposition von Venus und Mars zeigt die Polarität zwischen "
             "Geben und Nehmen in Beziehungen. Sie erleben diese Spannung oft "
             "durch Partner, die das verkörpern, was Ihnen fehlt.";
    case QUINCUNX:
      return "Der Quincunx zwischen Venus und Mars erfordert ständige Nachjustierung "
             "zwischen Harmoniebedürfnis und Durchsetzungswillen. Sie müssen lernen, "
             "Ihre sanfte und Ihre kämpferische Seite zu integrieren.";
    case HALBSEX:
      return "Das Halbsextil zwischen Venus und Mars zeigt eine feine Spannung "
             "zwischen Anziehung und Begehren, die bewusste Integration erfordert.";
    }
  }

  if (p1 == P_VENUS && p2 == P_JUPITER) {
    switch (aspect) {
    case KONJUNKTION:
      return "Venus und Jupiter vereint - ein klassischer Glücksaspekt! Sie haben "
             "ein großes Herz, sind großzügig in der Liebe und ziehen Fülle an. "
             "Genuss, Kunst und Lebensfreude sind Ihre Themen.";
    case SEXTIL:
      return "Die harmonische Verbindung von Venus und Jupiter begünstigt "
             "soziales Glück, künstlerischen Erfolg und liebevolle Beziehungen. "
             "Sie sehen das Gute in Menschen und ziehen Positives an.";
    case QUADRATUR:
      return "Die Spannung zwischen Venus und Jupiter kann zu Übertreibung in "
             "Liebe und Genuss führen. Zu viel des Guten - ob in Beziehungen, "
             "Ausgaben oder Vergnügen. Lernen Sie Maß zu halten.";
    case TRIGON:
      return "Venus und Jupiter im Trigon schenken natürliches Glück in "
             "Beziehungen und ein Talent, Schönheit und Fülle anzuziehen.";
    case OPOSITION:
      return "Die Opposition von Venus und Jupiter kann zu Beziehungskonflikten "
             "durch unterschiedliche Wertvorstellungen oder Lebensstile führen. "
             "Sie schwanken zwischen Bescheidenheit und dem Wunsch nach mehr.";
    case QUINCUNX:
      return "Der Quincunx zwischen Venus und Jupiter erfordert Anpassung zwischen "
             "persönlichen Beziehungen und größeren Lebensplänen. Liebe und Freiheit, "
             "Nähe und Expansion wollen ausbalanciert werden.";
    case HALBSEX:
      return "Das Halbsextil zwischen Venus und Jupiter zeigt eine subtile Spannung "
             "zwischen intimer Liebe und großzügiger Expansion.";
    }
  }

  if (p1 == P_VENUS && p2 == P_SATURN) {
    switch (aspect) {
    case KONJUNKTION:
      return "Venus und Saturn vereint können Liebe mit Pflicht verbinden oder "
             "Hemmungen in Beziehungen anzeigen. Sie nehmen die Liebe ernst und "
             "suchen dauerhafte Bindungen. Vielleicht fühlen Sie sich in der "
             "Liebe manchmal eingeschränkt oder nicht würdig.";
    case SEXTIL:
      return "Die harmonische Verbindung von Venus und Saturn gibt Ihren "
             "Beziehungen Stabilität und Tiefe. Sie sind loyal und bereit, "
             "für die Liebe zu arbeiten.";
    case QUADRATUR:
      return "Die Spannung zwischen Venus und Saturn kann Liebeskummer, "
             "Einsamkeitsgefühle oder Beziehungsprobleme anzeigen. Sie haben "
             "hohe Ansprüche, aber vielleicht auch Angst vor Nähe.";
    case TRIGON:
      return "Venus und Saturn im Trigon verbinden Liebe mit Verantwortung. "
             "Ihre Beziehungen sind solide und beständig.";
    case OPOSITION:
      return "Die Opposition von Venus und Saturn kann zu Beziehungskrisen oder "
             "dem Gefühl führen, zwischen Liebe und Pflicht wählen zu müssen. Sie "
             "projizieren möglicherweise emotionale Kälte auf Partner.";
    case QUINCUNX:
      return "Der Quincunx zwischen Venus und Saturn erfordert ständige Anpassung "
             "zwischen Ihrem Bedürfnis nach Liebe und Leichtigkeit und den Anforderungen "
             "der Realität. Genießen und Arbeiten wollen ausbalanciert werden.";
    case HALBSEX:
      return "Das Halbsextil zwischen Venus und Saturn zeigt eine feine Reibung "
             "zwischen Genuss und Disziplin, die schrittweise Integration erfordert.";
    }
  }

  if (p1 == P_VENUS && p2 == P_URANUS) {
    switch (aspect) {
    case KONJUNKTION:
      return "Venus und Uranus vereint schenken ungewöhnliche Anziehungskraft und "
             "ein Bedürfnis nach Freiheit in Beziehungen. Sie lieben das Außergewöhnliche "
             "und langweilen sich schnell mit Routine.";
    case SEXTIL:
      return "Die harmonische Verbindung von Venus und Uranus begünstigt kreative "
             "und unkonventionelle Beziehungen. Sie schätzen Freundschaft in der Liebe.";
    case QUADRATUR:
      return "Die Spannung zwischen Venus und Uranus kann zu plötzlichen Trennungen, "
             "Bindungsangst oder emotionaler Unruhe in Beziehungen führen. Sie brauchen "
             "sowohl Nähe als auch Freiraum.";
    case TRIGON:
      return "Venus und Uranus im Trigon schenken magnetische Ausstrahlung und die "
             "Fähigkeit, Liebe und Freiheit harmonisch zu verbinden.";
    case OPOSITION:
      return "Die Opposition von Venus und Uranus kann zu Beziehungschaos oder dem "
             "Gefühl führen, zwischen Sicherheit und Aufregung wählen zu müssen.";
    case QUINCUNX:
      return "Der Quincunx zwischen Venus und Uranus zeigt eine unbehagliche Spannung "
             "zwischen Harmoniebedarf und Freiheitsdrang. Sie müssen lernen, beide "
             "Bedürfnisse zu integrieren.";
    case HALBSEX:
      return "Das Halbsextil zwischen Venus und Uranus zeigt eine feine Spannung "
             "zwischen Stabilität und Aufregung in Beziehungen.";
    }
  }

  if (p1 == P_VENUS && p2 == P_NEPTUN) {
    switch (aspect) {
    case KONJUNKTION:
      return "Venus und Neptun vereint schenken romantische Idealismus und künstlerische "
             "Sensibilität. Sie lieben das Schöne und Transzendente, neigen aber auch "
             "zu Illusionen in der Liebe.";
    case SEXTIL:
      return "Die harmonische Verbindung von Venus und Neptun begünstigt künstlerische "
             "Begabung, spirituelle Liebe und ein feines Gespür für Ästhetik.";
    case QUADRATUR:
      return "Die Spannung zwischen Venus und Neptun kann zu Täuschungen in der Liebe, "
             "unrealistischen Erwartungen oder Abhängigkeitsmustern führen. Unterscheiden "
             "Sie zwischen Ideal und Realität.";
    case TRIGON:
      return "Venus und Neptun im Trigon schenken natürliche Romantik, künstlerisches "
             "Talent und die Fähigkeit zu bedingungsloser Liebe.";
    case OPOSITION:
      return "Die Opposition von Venus und Neptun kann zu Enttäuschungen in Beziehungen "
             "führen, wenn Ideale auf Realität treffen. Sie projizieren Traumbilder auf Partner.";
    case QUINCUNX:
      return "Der Quincunx zwischen Venus und Neptun erfordert ständige Anpassung zwischen "
             "realistischen Beziehungsbedürfnissen und romantischen Träumen.";
    case HALBSEX:
      return "Das Halbsextil zwischen Venus und Neptun zeigt eine subtile Verbindung "
             "zwischen Realitätssinn und romantischer Sehnsucht.";
    }
  }

  if (p1 == P_VENUS && p2 == P_PLUTO) {
    switch (aspect) {
    case KONJUNKTION:
      return "Venus und Pluto vereint erzeugen intensive, leidenschaftliche Anziehung. "
             "Liebe ist für Sie transformativ und tiefgreifend. Eifersuch, Besitzdenken "
             "und Machtspiele können Themen sein.";
    case SEXTIL:
      return "Die harmonische Verbindung von Venus und Pluto gibt Ihren Beziehungen "
             "Tiefe und Intensität. Sie können durch Liebe heilen und transformiert werden.";
    case QUADRATUR:
      return "Die Spannung zwischen Venus und Pluto kann zu Machtkämpfen in Beziehungen, "
             "obsessiver Liebe oder schmerzlichen Trennungen führen. Transformation ist "
             "der Schlüssel.";
    case TRIGON:
      return "Venus und Pluto im Trigon schenken magnetische Anziehungskraft und die "
             "Fähigkeit, tiefe, transformative Beziehungen zu führen.";
    case OPOSITION:
      return "Die Opposition von Venus und Pluto kann zu intensiven Beziehungskonflikten, "
             "Machtkämpfen oder projizierter Eifersucht führen.";
    case QUINCUNX:
      return "Der Quincunx zwischen Venus und Pluto erfordert ständige Anpassung zwischen "
             "dem Wunsch nach Harmonie und tiefen, transformativen Kräften in Beziehungen.";
    case HALBSEX:
      return "Das Halbsextil zwischen Venus und Pluto zeigt eine feine Verbindung "
             "zwischen oberflächlicher Harmonie und tieferer Leidenschaft.";
    }
  }

  // === MARS-ASPEKTE ===
  if (p1 == P_MARS && p2 == P_JUPITER) {
    switch (aspect) {
    case KONJUNKTION:
      return "Mars und Jupiter vereint schenken enormen Tatendrang und "
             "Unternehmungsgeist. Sie wollen Großes erreichen und haben die "
             "Energie dafür. Achten Sie darauf, Ihre Kräfte nicht zu "
             "überschätzen.";
    case SEXTIL:
      return "Die harmonische Verbindung von Mars und Jupiter begünstigt "
             "erfolgreiche Unternehmungen und die Fähigkeit, Chancen zu "
             "ergreifen. Mut und Optimismus arbeiten zusammen.";
    case QUADRATUR:
      return "Die Spannung zwischen Mars und Jupiter kann zu Übertreibung, "
             "Risikofreude oder Konflikten mit dem Gesetz führen. Ihre Energie "
             "ist groß - kanalisieren Sie sie weise.";
    case TRIGON:
      return "Mars und Jupiter im Trigon schenken natürlichen Erfolg bei "
             "mutigen Unternehmungen. Sie haben das Glück des Tüchtigen.";
    case OPOSITION:
      return "Die Opposition von Mars und Jupiter kann zu übertriebenem Ehrgeiz, "
             "Konflikten mit Autoritäten oder rechtlichen Problemen führen. Ihr "
             "Tatendrang braucht maßvolle Kanäle.";
    case QUINCUNX:
      return "Der Quincunx zwischen Mars und Jupiter erfordert Anpassung zwischen "
             "Ihrem Drang zu handeln und dem Bedürfnis, das große Bild zu sehen. "
             "Aktion und Vision wollen integriert werden.";
    case HALBSEX:
      return "Das Halbsextil zwischen Mars und Jupiter zeigt eine subtile Spannung "
             "zwischen fokussiertem Handeln und expansivem Denken.";
    }
  }

  if (p1 == P_MARS && p2 == P_SATURN) {
    switch (aspect) {
    case KONJUNKTION:
      return "Mars und Saturn vereint können kontrollierte Kraft oder gehemmte "
             "Energie anzeigen. Sie arbeiten ausdauernd und diszipliniert, aber "
             "vielleicht mit innerer Frustration. Lernen Sie, Ihre Energie "
             "geduldig einzusetzen.";
    case SEXTIL:
      return "Die harmonische Verbindung von Mars und Saturn gibt Ihrem "
             "Handeln Struktur und Ausdauer. Sie können langfristige Ziele "
             "konsequent verfolgen.";
    case QUADRATUR:
      return "Die Spannung zwischen Mars und Saturn kann Frustration, "
             "gehemmte Aggression oder Konflikte mit Autoritäten anzeigen. "
             "Sie fahren mit angezogener Handbremse - lernen Sie, sich "
             "zu befreien.";
    case TRIGON:
      return "Mars und Saturn im Trigon verbinden Energie mit Ausdauer. "
             "Sie können Großes aufbauen, weil Sie Kraft mit Geduld verbinden.";
    case OPOSITION:
      return "Die Opposition von Mars und Saturn kann zu Frustration, blockierter "
             "Energie oder Konflikten zwischen Antrieb und Hemmung führen. Sie "
             "erleben diese Spannung oft durch äußere Widersacher.";
    case QUINCUNX:
      return "Der Quincunx zwischen Mars und Saturn erfordert ständige Nachjustierung "
             "zwischen Ihrem Drang loszulegen und der Notwendigkeit, geduldig zu sein. "
             "Tempo und Timing wollen ausbalanciert werden.";
    case HALBSEX:
      return "Das Halbsextil zwischen Mars und Saturn zeigt eine feine Reibung "
             "zwischen Impuls und Zurückhaltung.";
    }
  }

  if (p1 == P_MARS && p2 == P_URANUS) {
    switch (aspect) {
    case KONJUNKTION:
      return "Mars und Uranus vereint erzeugen explosive Energie und revolutionären "
             "Tatendrang. Sie sind unabhängig, erfinderisch, aber auch unberechenbar. "
             "Plötzliche Aktionen sind möglich.";
    case SEXTIL:
      return "Die harmonische Verbindung von Mars und Uranus begünstigt innovative "
             "Unternehmungen und die Fähigkeit, schnell auf Veränderungen zu reagieren.";
    case QUADRATUR:
      return "Die Spannung zwischen Mars und Uranus kann zu Impulsivität, Unfällen "
             "oder plötzlichen Auseinandersetzungen führen. Ihre Energie ist elektrisch "
             "und braucht sichere Kanäle.";
    case TRIGON:
      return "Mars und Uranus im Trigon schenken natürliche Originalität im Handeln "
             "und die Fähigkeit, Innovationen durchzusetzen.";
    case OPOSITION:
      return "Die Opposition von Mars und Uranus kann zu plötzlichen Konflikten, "
             "Rebellion oder Unfällen führen. Sie projizieren möglicherweise "
             "Ihre eigene Unruhe auf andere.";
    case QUINCUNX:
      return "Der Quincunx zwischen Mars und Uranus erfordert Anpassung zwischen "
             "direktem Handeln und ungewöhnlichen Methoden. Tradition und Innovation "
             "wollen verbunden werden.";
    case HALBSEX:
      return "Das Halbsextil zwischen Mars und Uranus zeigt eine subtile Spannung "
             "zwischen geplanter Aktion und spontaner Veränderung.";
    }
  }

  if (p1 == P_MARS && p2 == P_NEPTUN) {
    switch (aspect) {
    case KONJUNKTION:
      return "Mars und Neptun vereint verbinden Aktion mit Inspiration, können aber auch "
             "Energie zerstreuen oder zu Verwirrung im Handeln führen. Kreative oder "
             "spirituelle Tätigkeiten sind begünstigt.";
    case SEXTIL:
      return "Die harmonische Verbindung von Mars und Neptun begünstigt künstlerisches "
             "Schaffen und die Fähigkeit, für Ideale zu kämpfen.";
    case QUADRATUR:
      return "Die Spannung zwischen Mars und Neptun kann zu Energieverlust, Täuschungen "
             "oder fehlgeleiteten Aktionen führen. Unterscheiden Sie zwischen Inspiration "
             "und Illusion.";
    case TRIGON:
      return "Mars und Neptun im Trigon schenken die Fähigkeit, Träume in die Tat "
             "umzusetzen und für höhere Ziele zu kämpfen.";
    case OPOSITION:
      return "Die Opposition von Mars und Neptun kann zu Verwirrung im Handeln, "
             "Opferrolle oder versteckter Aggression führen.";
    case QUINCUNX:
      return "Der Quincunx zwischen Mars und Neptun zeigt eine unbequeme Verbindung "
             "zwischen direktem Handeln und intuitivem Fließen. Sie müssen lernen, "
             "wann Sie kämpfen und wann Sie loslassen sollten.";
    case HALBSEX:
      return "Das Halbsextil zwischen Mars und Neptun zeigt eine feine Verbindung "
             "zwischen konkreter Tat und visionärer Inspiration.";
    }
  }

  if (p1 == P_MARS && p2 == P_PLUTO) {
    switch (aspect) {
    case KONJUNKTION:
      return "Mars und Pluto vereint erzeugen gewaltige Willenskraft und "
             "Durchsetzungsvermögen. Sie haben die Fähigkeit, Berge zu versetzen, "
             "aber auch destruktives Potenzial. Macht will weise eingesetzt werden.";
    case SEXTIL:
      return "Die harmonische Verbindung von Mars und Pluto gibt Ihnen die Fähigkeit, "
             "tief zu graben und große Veränderungen durchzusetzen.";
    case QUADRATUR:
      return "Die Spannung zwischen Mars und Pluto kann zu Machtkämpfen, zwanghaftem "
             "Ehrgeiz oder zerstörerischer Wut führen. Transformation der Aggression "
             "ist gefragt.";
    case TRIGON:
      return "Mars und Pluto im Trigon schenken enorme Willensstärke und die Fähigkeit, "
             "Hindernisse zu überwinden und sich völlig zu erneuern.";
    case OPOSITION:
      return "Die Opposition von Mars und Pluto kann zu intensiven Machtkämpfen oder "
             "Konflikten mit mächtigen Gegnern führen. Sie projizieren möglicherweise "
             "Ihre eigene Intensität auf andere.";
    case QUINCUNX:
      return "Der Quincunx zwischen Mars und Pluto erfordert ständige Anpassung zwischen "
             "oberflächlichem Handeln und tiefgreifender Transformation. Kontrolle und "
             "Loslassen wollen ausbalanciert werden.";
    case HALBSEX:
      return "Das Halbsextil zwischen Mars und Pluto zeigt eine subtile Verbindung "
             "zwischen Alltagshandeln und tieferen Transformationskräften.";
    }
  }

  // === ÄUSSERE PLANETEN ===
  if (p1 == P_JUPITER && p2 == P_SATURN) {
    switch (aspect) {
    case KONJUNKTION:
      return "Jupiter und Saturn vereint verbinden Expansion mit Konsolidierung. "
             "Dieser wichtige Zyklus zeigt, wie Sie Wachstum und Struktur in Einklang "
             "bringen. Große Projekte brauchen Zeit und Planung.";
    case SEXTIL:
      return "Die harmonische Verbindung von Jupiter und Saturn begünstigt nachhaltige "
             "Erfolge und die Fähigkeit, Visionen praktisch umzusetzen.";
    case QUADRATUR:
      return "Die Spannung zwischen Jupiter und Saturn zeigt einen Konflikt zwischen "
             "Expansion und Begrenzung. Sie schwanken zwischen Optimismus und "
             "Pessimismus.";
    case TRIGON:
      return "Jupiter und Saturn im Trigon schenken die seltene Fähigkeit, große "
             "Ziele mit Geduld und Ausdauer zu verwirklichen.";
    case OPOSITION:
      return "Die Opposition von Jupiter und Saturn zeigt den klassischen Konflikt "
             "zwischen Wachstum und Beschränkung, Freiheit und Verantwortung.";
    case QUINCUNX:
      return "Der Quincunx zwischen Jupiter und Saturn erfordert ständige Anpassung "
             "zwischen Ihrem Optimismus und den Realitäten. Träume und Praktikabilität "
             "wollen integriert werden.";
    case HALBSEX:
      return "Das Halbsextil zwischen Jupiter und Saturn zeigt eine feine Spannung "
             "zwischen Expansion und Konsolidierung.";
    }
  }

  if (p1 == P_JUPITER && p2 == P_URANUS) {
    switch (aspect) {
    case KONJUNKTION:
      return "Jupiter und Uranus vereint bringen plötzliche Glücksfälle, "
             "ungewöhnliche Chancen und den Drang nach radikaler Erneuerung.";
    case SEXTIL:
      return "Die harmonische Verbindung von Jupiter und Uranus begünstigt innovative "
             "Durchbrüche und die Fähigkeit, Zukunftstrends zu erkennen.";
    case QUADRATUR:
      return "Die Spannung zwischen Jupiter und Uranus kann zu Übertreibung in der "
             "Rebellion oder unrealistischen Erwartungen an Veränderungen führen.";
    case TRIGON:
      return "Jupiter und Uranus im Trigon schenken natürliches Glück bei "
             "ungewöhnlichen Unternehmungen und fortschrittlichen Ideen.";
    case QUINCUNX:
      return "Der Quincunx zwischen Jupiter und Uranus erfordert Anpassung zwischen "
             "traditionellen Werten und revolutionären Impulsen.";
    case OPOSITION:
      return "Die Opposition von Jupiter und Uranus kann zu radikalen Schwankungen "
             "zwischen Überoptimismus und plötzlichen Umwälzungen führen.";
    case HALBSEX:
      return "Das Halbsextil zwischen Jupiter und Uranus zeigt eine subtile Verbindung "
             "zwischen traditionellem Wachstum und innovativen Impulsen.";
    }
  }

  if (p1 == P_JUPITER && p2 == P_NEPTUN) {
    switch (aspect) {
    case KONJUNKTION:
      return "Jupiter und Neptun vereint steigern Idealismus, spirituelle Sehnsucht "
             "und künstlerische Vision. Achten Sie auf Übertreibung und Realitätsflucht.";
    case SEXTIL:
      return "Die harmonische Verbindung von Jupiter und Neptun begünstigt spirituelles "
             "Wachstum, Mitgefühl und künstlerische Inspiration.";
    case QUADRATUR:
      return "Die Spannung zwischen Jupiter und Neptun kann zu übertriebenem Idealismus, "
             "Enttäuschungen oder Täuschungen führen.";
    case TRIGON:
      return "Jupiter und Neptun im Trigon schenken natürliche Weisheit und einen "
             "Zugang zu höheren spirituellen Wahrheiten.";
    case QUINCUNX:
      return "Der Quincunx zwischen Jupiter und Neptun zeigt eine unbequeme Verbindung "
             "zwischen praktischem Wachstum und spiritueller Sehnsucht.";
    case OPOSITION:
      return "Die Opposition von Jupiter und Neptun kann zu übertriebenem Idealismus "
             "oder Realitätsflucht führen.";
    case HALBSEX:
      return "Das Halbsextil zwischen Jupiter und Neptun zeigt eine feine Verbindung "
             "zwischen weltlichem Erfolg und spiritueller Suche.";
    }
  }

  if (p1 == P_JUPITER && p2 == P_PLUTO) {
    switch (aspect) {
    case KONJUNKTION:
      return "Jupiter und Pluto vereint bringen enormes Wachstumspotenzial und "
             "transformative Kraft. Macht und Einfluss sind Themen - nutzen Sie sie weise.";
    case SEXTIL:
      return "Die harmonische Verbindung von Jupiter und Pluto begünstigt tiefgreifende "
             "Erfolge und die Fähigkeit, große Veränderungen herbeizuführen.";
    case QUADRATUR:
      return "Die Spannung zwischen Jupiter und Pluto kann zu Machtmissbrauch, "
             "extremem Ehrgeiz oder Konflikten mit mächtigen Strukturen führen.";
    case TRIGON:
      return "Jupiter und Pluto im Trigon schenken natürlichen Einfluss und die "
             "Fähigkeit, große Projekte erfolgreich durchzusetzen.";
    case QUINCUNX:
      return "Der Quincunx zwischen Jupiter und Pluto erfordert Anpassung zwischen "
             "Expansion und tiefgreifender Transformation.";
    case OPOSITION:
      return "Die Opposition von Jupiter und Pluto kann zu Machtkämpfen um Einfluss "
             "und Ressourcen führen.";
    case HALBSEX:
      return "Das Halbsextil zwischen Jupiter und Pluto zeigt eine subtile Verbindung "
             "zwischen äußerem Wachstum und innerer Transformation.";
    }
  }

  if (p1 == P_SATURN && p2 == P_URANUS) {
    switch (aspect) {
    case KONJUNKTION:
      return "Saturn und Uranus vereint verbinden Tradition mit Innovation. Dieser "
             "seltene Aspekt zeigt die Spannung zwischen Alt und Neu, Ordnung und Chaos.";
    case SEXTIL:
      return "Die harmonische Verbindung von Saturn und Uranus begünstigt praktische "
             "Innovationen und die Fähigkeit, Veränderungen strukturiert umzusetzen.";
    case QUADRATUR:
      return "Die Spannung zwischen Saturn und Uranus zeigt den Konflikt zwischen "
             "Sicherheit und Freiheit, Struktur und Rebellion.";
    case TRIGON:
      return "Saturn und Uranus im Trigon schenken die seltene Fähigkeit, revolutionäre "
             "Ideen praktisch und dauerhaft umzusetzen.";
    case QUINCUNX:
      return "Der Quincunx zwischen Saturn und Uranus erfordert ständige Anpassung "
             "zwischen Stabilität und Veränderung, Regeln und Freiheit.";
    case OPOSITION:
      return "Die Opposition von Saturn und Uranus zeigt den Konflikt zwischen "
             "etablierten Strukturen und revolutionären Kräften.";
    case HALBSEX:
      return "Das Halbsextil zwischen Saturn und Uranus zeigt eine feine Spannung "
             "zwischen Bewährtem und Neuem.";
    }
  }

  if (p1 == P_SATURN && p2 == P_NEPTUN) {
    switch (aspect) {
    case KONJUNKTION:
      return "Saturn und Neptun vereint verbinden Realität mit Traum. Dieser Aspekt "
             "kann zur Materialisierung von Visionen führen, aber auch zu Desillusionierung.";
    case SEXTIL:
      return "Die harmonische Verbindung von Saturn und Neptun begünstigt die "
             "Verwirklichung spiritueller Ziele und künstlerischer Visionen.";
    case QUADRATUR:
      return "Die Spannung zwischen Saturn und Neptun kann zu Desillusion, Angst oder "
             "dem Gefühl führen, dass Ihre Träume unmöglich sind.";
    case TRIGON:
      return "Saturn und Neptun im Trigon schenken die Fähigkeit, Träume in die "
             "Realität zu bringen und spirituelle Prinzipien praktisch anzuwenden.";
    case QUINCUNX:
      return "Der Quincunx zwischen Saturn und Neptun zeigt eine unbehagliche Spannung "
             "zwischen praktischen Anforderungen und spirituellen Sehnsüchten.";
    case OPOSITION:
      return "Die Opposition von Saturn und Neptun kann zu Desillusion oder dem "
             "Zusammenbruch unrealistischer Strukturen führen.";
    case HALBSEX:
      return "Das Halbsextil zwischen Saturn und Neptun zeigt eine subtile Verbindung "
             "zwischen konkreter Realität und transzendenter Vision.";
    }
  }

  if (p1 == P_SATURN && p2 == P_PLUTO) {
    switch (aspect) {
    case KONJUNKTION:
      return "Saturn und Pluto vereint bringen tiefgreifende strukturelle Transformation. "
             "Altes muss sterben, damit Neues entstehen kann. Ausdauer und Tiefe sind gefragt.";
    case SEXTIL:
      return "Die harmonische Verbindung von Saturn und Pluto begünstigt nachhaltige "
             "Transformation und die Fähigkeit, durch Krisen zu wachsen.";
    case QUADRATUR:
      return "Die Spannung zwischen Saturn und Pluto kann schwere Prüfungen, "
             "Machtkämpfe mit Strukturen oder existenzielle Krisen anzeigen.";
    case TRIGON:
      return "Saturn und Pluto im Trigon schenken enorme Ausdauer und die Fähigkeit, "
             "auch die schwersten Herausforderungen zu meistern.";
    case QUINCUNX:
      return "Der Quincunx zwischen Saturn und Pluto erfordert ständige Anpassung "
             "zwischen Kontrolle und Loslassen, Struktur und Transformation.";
    case OPOSITION:
      return "Die Opposition von Saturn und Pluto kann existenzielle Machtkämpfe "
             "oder den Zusammenbruch alter Strukturen anzeigen.";
    case HALBSEX:
      return "Das Halbsextil zwischen Saturn und Pluto zeigt eine subtile Verbindung "
             "zwischen Alltagsstrukturen und tiefgreifenden Transformationsprozessen.";
    }
  }

  if (p1 == P_URANUS && p2 == P_NEPTUN) {
    switch (aspect) {
    case KONJUNKTION:
      return "Uranus und Neptun vereint (alle ~171 Jahre) markieren eine Generation mit "
             "ungewöhnlicher spiritueller Offenheit und dem Drang nach neuen Idealen.";
    case SEXTIL:
      return "Das Sextil zwischen Uranus und Neptun begünstigt progressive spirituelle "
             "Entwicklung und innovative künstlerische Strömungen.";
    case QUINCUNX:
      return "Der Quincunx zwischen Uranus und Neptun zeigt generationelle Spannungen "
             "zwischen Revolution und Transzendenz.";
    case QUADRATUR:
      return "Das Quadrat zwischen Uranus und Neptun erzeugt Spannungen zwischen "
             "revolutionären Impulsen und spirituellen Idealen.";
    case TRIGON:
      return "Das Trigon zwischen Uranus und Neptun begünstigt inspirierende "
             "Verbindung von Innovation und Vision.";
    case OPOSITION:
      return "Die Opposition von Uranus und Neptun zeigt generationelle Konflikte "
             "zwischen Fortschritt und Spiritualität.";
    case HALBSEX:
      return "Das Halbsextil zwischen Uranus und Neptun zeigt eine feine Verbindung "
             "zwischen Innovation und Transzendenz.";
    }
  }

  if (p1 == P_URANUS && p2 == P_PLUTO) {
    switch (aspect) {
    case KONJUNKTION:
      return "Uranus und Pluto vereint (alle ~127 Jahre) markieren Zeiten radikaler "
             "gesellschaftlicher Umwälzungen und tiefgreifender Transformation.";
    case SEXTIL:
      return "Das Sextil zwischen Uranus und Pluto begünstigt nachhaltige "
             "Erneuerung und die Integration revolutionärer Veränderungen.";
    case QUADRATUR:
      return "Das Quadrat zwischen Uranus und Pluto erzeugt intensive Spannungen "
             "zwischen Freiheitsdrang und Machtstrukturen.";
    case QUINCUNX:
      return "Der Quincunx zwischen Uranus und Pluto erfordert Anpassung zwischen "
             "plötzlichen Veränderungen und tiefgreifender Transformation.";
    case TRIGON:
      return "Das Trigon zwischen Uranus und Pluto begünstigt kraftvolle "
             "Erneuerung und tiefgreifende Veränderung.";
    case OPOSITION:
      return "Die Opposition von Uranus und Pluto markiert Zeiten intensiver "
             "gesellschaftlicher Umwälzungen und Machtkämpfe.";
    case HALBSEX:
      return "Das Halbsextil zwischen Uranus und Pluto zeigt eine subtile Verbindung "
             "zwischen plötzlichen Impulsen und tieferer Transformation.";
    }
  }

  if (p1 == P_NEPTUN && p2 == P_PLUTO) {
    switch (aspect) {
    case KONJUNKTION:
      return "Neptun und Pluto vereint (alle ~493 Jahre) markieren epochale "
             "spirituelle und zivilisatorische Wendepunkte.";
    case SEXTIL:
      return "Das Sextil zwischen Neptun und Pluto begünstigt spirituelle Evolution "
             "und die Integration kollektiver Schattenthemen.";
    case QUINCUNX:
      return "Der Quincunx zwischen Neptun und Pluto zeigt generationelle Herausforderungen "
             "bei der Integration von Spiritualität und Macht.";
    case QUADRATUR:
      return "Das Quadrat zwischen Neptun und Pluto markiert epochale Spannungen "
             "zwischen Spiritualität und Macht.";
    case TRIGON:
      return "Das Trigon zwischen Neptun und Pluto begünstigt die Integration "
             "von spirituellem Bewusstsein und transformativer Kraft.";
    case OPOSITION:
      return "Die Opposition von Neptun und Pluto markiert zivilisatorische "
             "Wendepunkte im Verhältnis von Spiritualität und Macht.";
    case HALBSEX:
      return "Das Halbsextil zwischen Neptun und Pluto zeigt eine feine Verbindung "
             "zwischen Transzendenz und Transformation.";
    }
  }

  // === ASTEROIDEN ===
  if (p2 == P_CHIRON) {
    switch (aspect) {
    case KONJUNKTION:
      return QString("Die Verbindung mit Chiron zeigt einen sensiblen Bereich, "
             "in dem %1 mit der Urverletzung in Berührung kommt. Hier liegt "
             "sowohl Schmerz als auch ein großes Heilungspotenzial für sich "
             "selbst und andere.").arg(getPlanetName(p1));
    case SEXTIL:
      return QString("Die harmonische Verbindung von %1 und Chiron ermöglicht "
             "es Ihnen, aus eigenen Wunden Weisheit zu gewinnen und anderen "
             "zu helfen.").arg(getPlanetName(p1));
    case QUADRATUR:
      return QString("Die Spannung zwischen %1 und Chiron zeigt einen Bereich, "
             "in dem alte Wunden aufbrechen können. Die Heilung erfordert "
             "bewusste Arbeit.").arg(getPlanetName(p1));
    case TRIGON:
      return QString("%1 im Trigon zu Chiron schenkt natürliche Heilfähigkeiten "
             "und die Gabe, aus Erfahrung Weisheit zu schöpfen.").arg(getPlanetName(p1));
    case OPOSITION:
      return QString("Die Opposition von %1 und Chiron kann alte Wunden in "
             "Beziehungen aktivieren. Heilung kommt durch Integration.").arg(getPlanetName(p1));
    case QUINCUNX:
      return QString("Der Quincunx zwischen %1 und Chiron erfordert Anpassung "
             "zwischen Alltagsleben und Heilungsprozessen.").arg(getPlanetName(p1));
    case HALBSEX:
      return QString("Das Halbsextil zwischen %1 und Chiron zeigt eine feine "
             "Verbindung zwischen Lebenserfahrung und Heilung.").arg(getPlanetName(p1));
    }
  }

  if (p2 == P_CERES) {
    switch (aspect) {
    case KONJUNKTION:
      return QString("Die Verbindung von %1 mit Ceres betont Themen der "
             "Fürsorge, Nahrung und des Loslassens. Sie haben ein tiefes "
             "Bedürfnis, zu nähren und genährt zu werden.").arg(getPlanetName(p1));
    case SEXTIL:
      return QString("Die harmonische Verbindung von %1 und Ceres begünstigt "
             "einen natürlichen Zugang zu Fürsorge und Selbstfürsorge.")
             .arg(getPlanetName(p1));
    case TRIGON:
      return QString("%1 im Trigon zu Ceres schenkt ein natürliches Talent "
             "für Pflege und das Schaffen einer nährenden Umgebung.")
             .arg(getPlanetName(p1));
    case QUADRATUR:
      return QString("Die Spannung zwischen %1 und Ceres kann Konflikte um "
             "Fürsorge und Loslassen anzeigen.").arg(getPlanetName(p1));
    case OPOSITION:
      return QString("Die Opposition von %1 und Ceres kann Themen des "
             "Nährens versus Loslassens aktivieren.").arg(getPlanetName(p1));
    case QUINCUNX:
      return QString("Der Quincunx zwischen %1 und Ceres erfordert Anpassung "
             "zwischen Selbstfürsorge und Fürsorge für andere.").arg(getPlanetName(p1));
    case HALBSEX:
      return QString("Das Halbsextil zwischen %1 und Ceres zeigt eine subtile "
             "Verbindung zu Fürsorgethemen.").arg(getPlanetName(p1));
    }
  }

  if (p2 == P_PALLAS) {
    switch (aspect) {
    case KONJUNKTION:
      return QString("Die Verbindung von %1 mit Pallas Athene betont "
             "strategisches Denken, kreative Intelligenz und die Fähigkeit, "
             "Muster zu erkennen.").arg(getPlanetName(p1));
    case SEXTIL:
      return QString("Die harmonische Verbindung von %1 und Pallas fördert "
             "kreative Problemlösung und diplomatisches Geschick.")
             .arg(getPlanetName(p1));
    case TRIGON:
      return QString("%1 im Trigon zu Pallas schenkt natürliche Weisheit "
             "und die Gabe, kreative Lösungen zu finden.").arg(getPlanetName(p1));
    case QUADRATUR:
      return QString("Die Spannung zwischen %1 und Pallas kann Konflikte zwischen "
             "Intuition und Strategie anzeigen.").arg(getPlanetName(p1));
    case OPOSITION:
      return QString("Die Opposition von %1 und Pallas zeigt den Konflikt "
             "zwischen Instinkt und kluger Planung.").arg(getPlanetName(p1));
    case QUINCUNX:
      return QString("Der Quincunx zwischen %1 und Pallas erfordert Anpassung "
             "zwischen spontanem Handeln und strategischem Denken.").arg(getPlanetName(p1));
    case HALBSEX:
      return QString("Das Halbsextil zwischen %1 und Pallas zeigt eine feine "
             "Verbindung zwischen Kreativität und Weisheit.").arg(getPlanetName(p1));
    }
  }

  if (p2 == P_JUNO) {
    switch (aspect) {
    case KONJUNKTION:
      return QString("Die Verbindung von %1 mit Juno betont Partnerschaftsthemen, "
             "Treue und das Bedürfnis nach einer gleichberechtigten Beziehung.")
             .arg(getPlanetName(p1));
    case SEXTIL:
      return QString("Die harmonische Verbindung von %1 und Juno begünstigt "
             "harmonische Partnerschaften und Fairness in Beziehungen.")
             .arg(getPlanetName(p1));
    case TRIGON:
      return QString("%1 im Trigon zu Juno schenkt ein natürliches Talent "
             "für dauerhafte, loyale Partnerschaften.").arg(getPlanetName(p1));
    case QUADRATUR:
      return QString("Die Spannung zwischen %1 und Juno kann Beziehungskonflikte "
             "um Treue und Gleichberechtigung anzeigen.").arg(getPlanetName(p1));
    case OPOSITION:
      return QString("Die Opposition von %1 und Juno zeigt Partnerschaftsthemen, "
             "die durch andere gespiegelt werden.").arg(getPlanetName(p1));
    case QUINCUNX:
      return QString("Der Quincunx zwischen %1 und Juno erfordert Anpassung "
             "zwischen persönlichen und Partnerschaftsbedürfnissen.").arg(getPlanetName(p1));
    case HALBSEX:
      return QString("Das Halbsextil zwischen %1 und Juno zeigt eine subtile "
             "Verbindung zu Partnerschaftsthemen.").arg(getPlanetName(p1));
    }
  }

  if (p2 == P_VESTA) {
    switch (aspect) {
    case KONJUNKTION:
      return QString("Die Verbindung von %1 mit Vesta betont Hingabe, Fokus "
             "und das heilige Feuer der inneren Berufung. Hier können Sie "
             "sich ganz einer Sache widmen.").arg(getPlanetName(p1));
    case SEXTIL:
      return QString("Die harmonische Verbindung von %1 und Vesta fördert "
             "konzentriertes Arbeiten und die Fähigkeit, sich einer Aufgabe "
             "mit Hingabe zu widmen.").arg(getPlanetName(p1));
    case TRIGON:
      return QString("%1 im Trigon zu Vesta schenkt natürliche Fokussierung "
             "und die Gabe, das innere Feuer der Hingabe zu bewahren.")
             .arg(getPlanetName(p1));
    case QUADRATUR:
      return QString("Die Spannung zwischen %1 und Vesta kann Konflikte zwischen "
             "persönlichen Bedürfnissen und Hingabe anzeigen.").arg(getPlanetName(p1));
    case OPOSITION:
      return QString("Die Opposition von %1 und Vesta zeigt den Konflikt "
             "zwischen Weltlichem und Heiligem.").arg(getPlanetName(p1));
    case QUINCUNX:
      return QString("Der Quincunx zwischen %1 und Vesta erfordert Anpassung "
             "zwischen Alltagsleben und innerer Berufung.").arg(getPlanetName(p1));
    case HALBSEX:
      return QString("Das Halbsextil zwischen %1 und Vesta zeigt eine feine "
             "Verbindung zwischen Fokus und Hingabe.").arg(getPlanetName(p1));
    }
  }

  if (p2 == P_LILITH) {
    switch (aspect) {
    case KONJUNKTION:
      return QString("Die Verbindung von %1 mit Lilith berührt Ihre wilde, "
             "ungezähmte Seite und Themen von Unterdrückung und Befreiung. "
             "Hier liegt verborgene Kraft.").arg(getPlanetName(p1));
    case SEXTIL:
      return QString("Die harmonische Verbindung von %1 und Lilith ermöglicht "
             "Zugang zu Ihrer inneren Wildheit auf konstruktive Weise.")
             .arg(getPlanetName(p1));
    case QUADRATUR:
      return QString("Die Spannung zwischen %1 und Lilith kann verdrängte "
             "Aspekte Ihrer Persönlichkeit aktivieren. Integration ist gefragt.")
             .arg(getPlanetName(p1));
    case TRIGON:
      return QString("%1 im Trigon zu Lilith schenkt natürlichen Zugang zu "
             "Ihrer inneren Wildheit und ungezähmten Kraft.").arg(getPlanetName(p1));
    case OPOSITION:
      return QString("Die Opposition von %1 und Lilith kann verdrängte "
             "Schattenaspekte durch andere aktivieren.").arg(getPlanetName(p1));
    case QUINCUNX:
      return QString("Der Quincunx zwischen %1 und Lilith erfordert Anpassung "
             "zwischen zivilisiertem Verhalten und innerer Wildheit.").arg(getPlanetName(p1));
    case HALBSEX:
      return QString("Das Halbsextil zwischen %1 und Lilith zeigt eine subtile "
             "Verbindung zu unterdrückten Aspekten Ihrer Natur.").arg(getPlanetName(p1));
    }
  }

  if (p2 == P_NKNOTEN) {
    switch (aspect) {
    case KONJUNKTION:
      return QString("Die Konjunktion von %1 und Mondknoten verbindet diese "
             "Planetenenergie mit Ihrem karmischen Lebensweg. Begegnungen und "
             "Erfahrungen in diesem Bereich können schicksalhaft sein.")
             .arg(getPlanetName(p1));
    case SEXTIL:
      return QString("Das Sextil zwischen %1 und Mondknoten öffnet Möglichkeiten, "
             "diese Energie konstruktiv in Ihren Lebensweg zu integrieren.")
             .arg(getPlanetName(p1));
    case QUADRATUR:
      return QString("Das Quadrat zwischen %1 und Mondknoten zeigt Herausforderungen "
             "auf Ihrem karmischen Weg. Alte Muster in diesem Bereich wollen "
             "überwunden werden.").arg(getPlanetName(p1));
    case TRIGON:
      return QString("Das Trigon zwischen %1 und Mondknoten schenkt einen "
             "natürlichen Fluss zwischen dieser Energie und Ihrem Lebensweg. "
             "Sie finden hier leicht Ihren Platz.").arg(getPlanetName(p1));
    case OPOSITION:
      return QString("Die Opposition zwischen %1 und Mondknoten zeigt eine "
             "Spannung zwischen vergangenen Mustern und zukünftigem Wachstum "
             "in diesem Bereich.").arg(getPlanetName(p1));
    case QUINCUNX:
      return QString("Der Quincunx zwischen %1 und Mondknoten erfordert ständige "
             "Anpassung zwischen dieser Energie und Ihrem karmischen Weg.")
             .arg(getPlanetName(p1));
    case HALBSEX:
      return QString("Das Halbsextil zwischen %1 und Mondknoten zeigt eine "
             "subtile Verbindung zwischen dieser Energie und Ihrem Lebensweg.")
             .arg(getPlanetName(p1));
    }
  }

  // Kein spezifischer Text gefunden
  return QString();
}

//==============================================================================
// Sonnenzeichen-Texte
//==============================================================================

void AstroTextAnalyzer::initializeSunSignTexts() {
  m_sunSignTexts[iWIDDER] =
      "<p>Als <b>Widder</b> sind Sie ein geborener Pionier. Ihre Energie ist "
      "direkt, spontan "
      "und mutig. Sie lieben es, neue Wege zu gehen und Herausforderungen "
      "anzunehmen. "
      "Ihr Durchsetzungsvermögen und Ihre Entschlossenheit sind "
      "beeindruckend.</p>"
      "<p>Sie sind initiativ, unabhängig und haben einen starken Willen. "
      "Ungeduld kann "
      "manchmal eine Schwäche sein, aber Ihre Begeisterungsfähigkeit gleicht "
      "das aus. "
      "Sie sind ein natürlicher Anführer mit viel Selbstvertrauen.</p>";

  m_sunSignTexts[iSTIER] = "<p>Als <b>Stier</b> schätzen Sie Stabilität, "
                           "Sicherheit und die schönen Dinge des Lebens. "
                           "Sie sind praktisch veranlagt, zuverlässig und "
                           "haben eine natürliche Geduld. "
                           "Ihre Sinne sind besonders ausgeprägt - Sie "
                           "genießen Komfort und Qualität.</p>"
                           "<p>Sie sind beständig, treu und haben einen "
                           "ausgeprägten Sinn für Ästhetik. "
                           "Ihre Bodenständigkeit macht Sie zu einem "
                           "verlässlichen Partner. Manchmal kann "
                           "Ihre Sturheit herausfordernd sein, aber sie gibt "
                           "Ihnen auch Durchhaltevermögen.</p>";

  m_sunSignTexts[iZWILLINGE] = "<p>Als <b>Zwilling</b> sind Sie vielseitig, "
                               "neugierig und kommunikativ. Ihr Geist ist "
                               "beweglich und Sie lieben den Austausch von "
                               "Ideen. Sie sind flexibel, anpassungsfähig "
                               "und haben einen natürlichen Humor.</p>"
                               "<p>Ihre Stärke liegt in der Kommunikation und "
                               "im logischen Denken. Sie brauchen "
                               "geistige Anregung und Abwechslung. Ihre "
                               "Fähigkeit, mehrere Dinge gleichzeitig "
                               "zu tun, ist bemerkenswert. Oberflächlichkeit "
                               "kann eine Falle sein, aber Ihre "
                               "Vielseitigkeit ist ein großes Geschenk.</p>";

  m_sunSignTexts[iKREBS] =
      "<p>Als <b>Krebs</b> sind Sie sensibel, fürsorglich und emotional "
      "tiefgründig. "
      "Familie und Zuhause sind Ihnen besonders wichtig. Sie haben eine starke "
      "Intuition "
      "und ein ausgeprägtes Gefühlsleben.</p>"
      "<p>Sie sind beschützend, empathisch und haben ein gutes Gedächtnis. "
      "Ihre emotionale "
      "Intelligenz ist hoch entwickelt. Sie brauchen emotionale Sicherheit und "
      "schaffen "
      "gerne ein gemütliches Heim. Ihre Launenhaftigkeit kann herausfordernd "
      "sein, aber "
      "Ihre Fürsorglichkeit macht Sie zu einem wertvollen Menschen.</p>";

  m_sunSignTexts[iLOWE] = "<p>Als <b>Löwe</b> strahlen Sie natürliche Würde "
                          "und Selbstbewusstsein aus. Sie sind "
                          "großzügig, kreativ und lieben es, im Mittelpunkt zu "
                          "stehen. Ihre Lebensfreude "
                          "und Ihr Optimismus sind ansteckend.</p>"
                          "<p>Sie haben ein starkes Bedürfnis nach Anerkennung "
                          "und Ausdruck Ihrer Kreativität. "
                          "Ihr Herz ist groß und loyal. Sie sind stolz, "
                          "dramatisch und haben einen natürlichen "
                          "Sinn für Theater. Manchmal kann Ihr Ego im Weg "
                          "stehen, aber Ihre Großzügigkeit "
                          "und Wärme machen Sie beliebt.</p>";

  m_sunSignTexts[iJUNGFRAU] =
      "<p>Als <b>Jungfrau</b> sind Sie analytisch, präzise und dienstbereit. "
      "Sie haben einen "
      "ausgeprägten Sinn für Details und Ordnung. Ihre praktische Intelligenz "
      "und "
      "Ihr Perfektionismus zeichnen Sie aus.</p>"
      "<p>Sie sind fleißig, zuverlässig und kritisch - vor allem mit sich "
      "selbst. Ihre "
      "Fähigkeit zu organisieren und zu verbessern ist bemerkenswert. Sie "
      "brauchen "
      "nützliche Tätigkeiten und schätzen Effizienz. Überkritik kann eine "
      "Schwäche sein, "
      "aber Ihre Hilfsbereitschaft ist ein großes Geschenk.</p>";

  m_sunSignTexts[iWAAGE] = "<p>Als <b>Waage</b> suchen Sie Harmonie, Balance "
                           "und Schönheit. Sie sind diplomatisch, "
                           "charmant und haben einen ausgeprägten Sinn für "
                           "Gerechtigkeit. Beziehungen sind "
                           "Ihnen besonders wichtig.</p>"
                           "<p>Sie sind höflich, kooperativ und haben einen "
                           "natürlichen Geschmackssinn. Ihre "
                           "Fähigkeit, verschiedene Perspektiven zu sehen, "
                           "macht Sie zu einem guten Vermittler. "
                           "Sie brauchen Partnerschaft und Ästhetik. "
                           "Entscheidungsschwierigkeiten können "
                           "herausfordernd sein, aber Ihr Streben nach "
                           "Ausgewogenheit ist wertvoll.</p>";

  m_sunSignTexts[iSKORPION] = "<p>Als <b>Skorpion</b> sind Sie intensiv, "
                              "leidenschaftlich und transformativ. Sie gehen "
                              "in die Tiefe und scheuen keine emotionalen "
                              "Extreme. Ihre Willenskraft und "
                              "Durchsetzungsfähigkeit sind beeindruckend.</p>"
                              "<p>Sie sind magnetisch, geheimnisvoll und haben "
                              "eine starke Regenerationskraft. "
                              "Ihre Fähigkeit zur Transformation ist "
                              "einzigartig. Sie brauchen emotionale Tiefe "
                              "und Authentizität. Kontrollbedürfnis kann eine "
                              "Falle sein, aber Ihre Intensität "
                              "und Treue sind außergewöhnlich.</p>";

  m_sunSignTexts[iSCHUTZE] = "<p>Als <b>Schütze</b> sind Sie optimistisch, "
                             "philosophisch und freiheitsliebend. Sie "
                             "haben einen weiten Horizont und lieben es zu "
                             "reisen - physisch oder geistig. "
                             "Ihre Begeisterungsfähigkeit ist ansteckend.</p>"
                             "<p>Sie sind ehrlich, großzügig und haben einen "
                             "ausgeprägten Sinn für Humor. Ihre "
                             "Suche nach Wahrheit und Bedeutung treibt Sie an. "
                             "Sie brauchen Freiheit und "
                             "Expansion. Übertreibung kann eine Schwäche sein, "
                             "aber Ihr Optimismus und Ihre "
                             "Weitsicht sind inspirierend.</p>";

  m_sunSignTexts[iSTEINBOCK] = "<p>Als <b>Steinbock</b> sind Sie ambitioniert, "
                               "diszipliniert und verantwortungsbewusst. "
                               "Sie haben einen klaren Plan und arbeiten "
                               "beharrlich auf Ihre Ziele hin. Ihre "
                               "Ausdauer ist bewundernswert.</p>"
                               "<p>Sie sind praktisch, traditionsbewusst und "
                               "haben einen trockenen Humor. Ihre "
                               "Fähigkeit zu strukturieren und aufzubauen ist "
                               "bemerkenswert. Sie brauchen Erfolg "
                               "und Anerkennung für Ihre Leistungen. "
                               "Übermäßige Strenge kann herausfordernd sein, "
                               "aber Ihre Zuverlässigkeit und Ihr "
                               "Durchhaltevermögen sind ein Fundament.</p>";

  m_sunSignTexts[iWASSERMANN] =
      "<p>Als <b>Wassermann</b> sind Sie individualistisch, innovativ und "
      "humanitär orientiert. "
      "Sie denken fortschrittlich und schätzen Ihre Unabhängigkeit. Ihre "
      "Originalität "
      "und Ihr Ideenreichtum sind beeindruckend.</p>"
      "<p>Sie sind freundschaftlich, objektiv und haben einen ausgeprägten "
      "Gemeinschaftssinn. "
      "Ihre Fähigkeit, über den Tellerrand zu schauen, ist wertvoll. Sie "
      "brauchen geistige "
      "Freiheit und soziale Verbindungen. Distanziertheit kann eine Schwäche "
      "sein, aber "
      "Ihre Vision einer besseren Welt ist inspirierend.</p>";

  m_sunSignTexts[iFISCHE] =
      "<p>Als <b>Fische</b> sind Sie einfühlsam, kreativ und spirituell "
      "veranlagt. Sie haben "
      "eine reiche Innenwelt und starke Intuition. Ihre Empathie und Ihr "
      "Mitgefühl "
      "sind außergewöhnlich.</p>"
      "<p>Sie sind anpassungsfähig, romantisch und haben eine künstlerische "
      "Ader. Ihre "
      "Fähigkeit, zwischen den Welten zu wandeln, ist einzigartig. Sie "
      "brauchen "
      "Inspiration und transzendente Erfahrungen. Grenzenlosigkeit kann "
      "herausfordernd "
      "sein, aber Ihre Sensibilität und Ihr Mitgefühl sind ein Geschenk.</p>";
}

//==============================================================================
// Aszendenten-Texte
//==============================================================================

void AstroTextAnalyzer::initializeAscendantTexts() {
  m_ascendantTexts[iWIDDER] = "<p>Mit <b>Widder-Aszendent</b> wirken Sie "
                              "energiegeladen, direkt und selbstbewusst. "
                              "Sie treten mutig und entschlossen auf. Andere "
                              "erleben Sie als dynamisch und initiativ.</p>";

  m_ascendantTexts[iSTIER] = "<p>Mit <b>Stier-Aszendent</b> wirken Sie ruhig, "
                             "beständig und verlässlich. "
                             "Sie strahlen Stabilität und Genuss aus. Andere "
                             "erleben Sie als geerdet und sinnlich.</p>";

  m_ascendantTexts[iZWILLINGE] =
      "<p>Mit <b>Zwillinge-Aszendent</b> wirken Sie neugierig, kommunikativ "
      "und beweglich. "
      "Sie treten gesprächig und vielseitig auf. Andere erleben Sie als "
      "geistreich und anpassungsfähig.</p>";

  m_ascendantTexts[iKREBS] = "<p>Mit <b>Krebs-Aszendent</b> wirken Sie "
                             "sensibel, fürsorglich und zurückhaltend. "
                             "Sie strahlen Wärme und Empathie aus. Andere "
                             "erleben Sie als emotional und schützend.</p>";

  m_ascendantTexts[iLOWE] = "<p>Mit <b>Löwe-Aszendent</b> wirken Sie "
                            "selbstbewusst, großzügig und würdevoll. "
                            "Sie treten strahlend und stolz auf. Andere "
                            "erleben Sie als charismatisch und warmherzig.</p>";

  m_ascendantTexts[iJUNGFRAU] =
      "<p>Mit <b>Jungfrau-Aszendent</b> wirken Sie ordentlich, präzise und "
      "zurückhaltend. "
      "Sie strahlen Kompetenz und Bescheidenheit aus. Andere erleben Sie als "
      "hilfsbereit und analytisch.</p>";

  m_ascendantTexts[iWAAGE] = "<p>Mit <b>Waage-Aszendent</b> wirken Sie "
                             "charmant, harmonisch und diplomatisch. "
                             "Sie treten höflich und ausgeglichen auf. Andere "
                             "erleben Sie als ästhetisch und kooperativ.</p>";

  m_ascendantTexts[iSKORPION] =
      "<p>Mit <b>Skorpion-Aszendent</b> wirken Sie intensiv, magnetisch und "
      "geheimnisvoll. "
      "Sie strahlen Tiefe und Macht aus. Andere erleben Sie als "
      "leidenschaftlich und durchdringend.</p>";

  m_ascendantTexts[iSCHUTZE] =
      "<p>Mit <b>Schütze-Aszendent</b> wirken Sie optimistisch, offen und "
      "philosophisch. "
      "Sie treten enthusiastisch und weitherzig auf. Andere erleben Sie als "
      "inspirierend und freiheitsliebend.</p>";

  m_ascendantTexts[iSTEINBOCK] =
      "<p>Mit <b>Steinbock-Aszendent</b> wirken Sie ernst, zielstrebig und "
      "verantwortungsvoll. "
      "Sie strahlen Autorität und Disziplin aus. Andere erleben Sie als "
      "kompetent und ausdauernd.</p>";

  m_ascendantTexts[iWASSERMANN] =
      "<p>Mit <b>Wassermann-Aszendent</b> wirken Sie individuell, innovativ "
      "und unabhängig. "
      "Sie treten originell und freundschaftlich auf. Andere erleben Sie als "
      "fortschrittlich und unkonventionell.</p>";

  m_ascendantTexts[iFISCHE] =
      "<p>Mit <b>Fische-Aszendent</b> wirken Sie sanft, einfühlsam und "
      "verträumt. "
      "Sie strahlen Mitgefühl und Spiritualität aus. Andere erleben Sie als "
      "sensitiv und künstlerisch.</p>";
}

//==============================================================================
// Mondzeichen-Texte
//==============================================================================

void AstroTextAnalyzer::initializeMoonSignTexts() {
  m_moonSignTexts[iWIDDER] = "<p>Mit <b>Mond in Widder</b> reagieren Sie "
                             "spontan und direkt auf emotionale Situationen. "
                             "Sie brauchen Action und Unabhängigkeit. Ihre "
                             "Gefühle sind intensiv aber oft kurz.</p>";

  m_moonSignTexts[iSTIER] = "<p>Mit <b>Mond in Stier</b> suchen Sie emotionale "
                            "Sicherheit und Stabilität. "
                            "Sie brauchen Komfort und Beständigkeit. Ihre "
                            "Gefühle sind tief und dauerhaft.</p>";

  m_moonSignTexts[iZWILLINGE] = "<p>Mit <b>Mond in Zwillinge</b> brauchen Sie "
                                "geistige Stimulation und Kommunikation. "
                                "Sie verarbeiten Gefühle durch Sprechen. Ihre "
                                "Emotionen sind beweglich und vielseitig.</p>";

  m_moonSignTexts[iKREBS] = "<p>Mit <b>Mond in Krebs</b> sind Ihre Gefühle "
                            "sehr ausgeprägt und intensiv. "
                            "Sie brauchen ein emotionales Zuhause. Ihre "
                            "Fürsorglichkeit ist tief verwurzelt.</p>";

  m_moonSignTexts[iLOWE] = "<p>Mit <b>Mond in Löwe</b> brauchen Sie emotionale "
                           "Anerkennung und Wärme. "
                           "Sie drücken Gefühle großzügig aus. Ihr inneres "
                           "Kind ist lebendig und kreativ.</p>";

  m_moonSignTexts[iJUNGFRAU] =
      "<p>Mit <b>Mond in Jungfrau</b> verarbeiten Sie Emotionen analytisch. "
      "Sie brauchen Ordnung auch im Gefühlsleben. Ihre Fürsorge zeigt sich "
      "praktisch.</p>";

  m_moonSignTexts[iWAAGE] =
      "<p>Mit <b>Mond in Waage</b> suchen Sie emotionale Harmonie und Balance. "
      "Sie brauchen Beziehungen für Ihr Wohlbefinden. Ihre Gefühle sind auf "
      "Ausgleich ausgerichtet.</p>";

  m_moonSignTexts[iSKORPION] =
      "<p>Mit <b>Mond in Skorpion</b> erleben Sie Emotionen mit extremer "
      "Intensität. "
      "Sie brauchen emotionale Tiefe und Transformation. Ihre Gefühle sind "
      "leidenschaftlich.</p>";

  m_moonSignTexts[iSCHUTZE] =
      "<p>Mit <b>Mond in Schütze</b> brauchen Sie emotionale Freiheit und "
      "Optimismus. "
      "Sie verarbeiten Gefühle philosophisch. Ihr inneres Selbst sucht nach "
      "Sinn und Expansion.</p>";

  m_moonSignTexts[iSTEINBOCK] =
      "<p>Mit <b>Mond in Steinbock</b> kontrollieren Sie Ihre Emotionen "
      "diszipliniert. "
      "Sie brauchen emotionale Sicherheit durch Leistung. Ihre Gefühle sind "
      "verantwortungsvoll.</p>";

  m_moonSignTexts[iWASSERMANN] = "<p>Mit <b>Mond in Wassermann</b> brauchen "
                                 "Sie emotionale Unabhängigkeit und Freiheit. "
                                 "Sie verarbeiten Gefühle objektiv. Ihr "
                                 "inneres Selbst ist individualistisch.</p>";

  m_moonSignTexts[iFISCHE] = "<p>Mit <b>Mond in Fische</b> sind Sie emotional "
                             "sehr empfänglich und mitfühlend. "
                             "Sie brauchen spirituelle Verbindung. Ihre "
                             "Gefühle sind grenzenlos und intuitiv.</p>";
}

//==============================================================================
// Aspekte-Texte (Beispiele für wichtige Aspekte)
//==============================================================================

void AstroTextAnalyzer::initializeAspectTexts() {
  // Sonne-Mond Aspekte
  m_aspectTexts[makeAspectKey(P_SONNE, P_MOND, KONJUNKTION)] =
      "<p>Die Verbindung von Sonne und Mond zeigt Einheit von Wille und "
      "Gefühl. "
      "Sie sind im Einklang mit sich selbst.</p>";

  m_aspectTexts[makeAspectKey(P_SONNE, P_MOND, TRIGON)] =
      "<p>Harmonie zwischen Ihrem bewussten Willen und Ihren Gefühlen. "
      "Sie fühlen sich wohl in Ihrer Haut.</p>";

  m_aspectTexts[makeAspectKey(P_SONNE, P_MOND, QUADRATUR)] =
      "<p>Spannung zwischen Wille und Gefühl erfordert Bewusstheit. "
      "Diese Reibung kann zu Wachstum führen.</p>";

  // Sonne-Merkur (immer nahe beieinander)
  m_aspectTexts[makeAspectKey(P_SONNE, P_MERKUR, KONJUNKTION)] =
      "<p>Ihr Denken und Selbstausdruck sind eng verbunden. "
      "Sie kommunizieren mit Selbstbewusstsein.</p>";

  // Venus-Mars Aspekte (Beziehungsdynamik)
  m_aspectTexts[makeAspectKey(P_VENUS, P_MARS, KONJUNKTION)] =
      "<p>Harmonie zwischen Ihren männlichen und weiblichen Energien. "
      "Leidenschaft und Anziehungskraft sind ausgeprägt.</p>";

  m_aspectTexts[makeAspectKey(P_VENUS, P_MARS, TRIGON)] =
      "<p>Natürliche Harmonie zwischen Liebe und Begehren. "
      "Sie drücken Zuneigung leicht aus.</p>";

  m_aspectTexts[makeAspectKey(P_VENUS, P_MARS, QUADRATUR)] =
      "<p>Spannung zwischen Herz und Leidenschaft. "
      "Diese Dynamik kann kreativ genutzt werden.</p>";

  // Saturn-Aspekte (Struktur und Herausforderung)
  m_aspectTexts[makeAspectKey(P_SONNE, P_SATURN, KONJUNKTION)] =
      "<p>Verantwortung und Disziplin prägen Ihre Identität. "
      "Sie nehmen das Leben ernst und arbeiten beständig.</p>";

  m_aspectTexts[makeAspectKey(P_MOND, P_SATURN, QUADRATUR)] =
      "<p>Emotionale Einschränkungen fordern Sie heraus. "
      "Lernen, Gefühle zu akzeptieren, ist wichtig.</p>";
}

//==============================================================================
// Analyse-Funktionen
//==============================================================================

QString AstroTextAnalyzer::analyzeSunSign(int8_t sternzeichen) const {
  astroTextStore().ensureLoaded();
  const QString lang = analyzerLang();
  const QString key = QString("sun_sign.%1").arg(static_cast<int>(sternzeichen));
  return astroTextStore().text(
      lang,
      key,
      m_sunSignTexts.value(
          sternzeichen,
          "<p><i>Keine detaillierte Beschreibung verfügbar.</i></p>"));
}

QString AstroTextAnalyzer::analyzeAscendant(int8_t sternzeichen) const {
  astroTextStore().ensureLoaded();
  const QString lang = analyzerLang();
  const QString key = QString("ascendant.%1").arg(static_cast<int>(sternzeichen));
  return astroTextStore().text(
      lang,
      key,
      m_ascendantTexts.value(
          sternzeichen,
          "<p><i>Keine detaillierte Beschreibung verfügbar.</i></p>"));
}

QString AstroTextAnalyzer::analyzeMoonSign(int8_t sternzeichen) const {
  astroTextStore().ensureLoaded();
  const QString lang = analyzerLang();
  const QString key = QString("moon_sign.%1").arg(static_cast<int>(sternzeichen));
  return astroTextStore().text(
      lang,
      key,
      m_moonSignTexts.value(
          sternzeichen,
          "<p><i>Keine detaillierte Beschreibung verfügbar.</i></p>"));
}

// Alte Methode (Forwarder)
QString AstroTextAnalyzer::analyzeAspect(int planet1, int planet2,
                                         int16_t aspect) const {
  return getGenericAspectText(planet1, planet2, aspect);
}

// Neue Methode mit Kontext
QString AstroTextAnalyzer::getContextAspectText(int planet1, int8_t sign1,
                                                int planet2, int8_t sign2,
                                                int16_t aspect) const {
  astroTextStore().ensureLoaded();
  const QString lang = analyzerLang();

  if (sign1 >= 0 && sign2 >= 0) {
    const QString k = QString("aspect.context.%1.%2.%3.%4.%5")
                          .arg(static_cast<int>(sign1))
                          .arg(planet1)
                          .arg(static_cast<int>(aspect))
                          .arg(planet2)
                          .arg(static_cast<int>(sign2));
    const QString ov = astroTextStore().text(lang, k, QString());
    if (!ov.isEmpty()) {
      return ov;
    }
  }

  QString text = "<p>";

  // Element-Berechnung
  // Feuer: 0 Widder, 4 Löwe, 8 Schütze
  // Erde: 1 Stier, 5 Jungfrau, 9 Steinbock
  // Luft: 2 Zwillinge, 6 Waage, 10 Wassermann
  // Wasser: 3 Krebs, 7 Skorpion, 11 Fische
  auto getElement = [](int s) -> int { return (s >= 0) ? s % 4 : -1; };

  // Qualität/Modalität: Kardinal, Fix, Veränderlich
  // Kardinal: 0 Widder, 3 Krebs, 6 Waage, 9 Steinbock
  // Fix: 1 Stier, 4 Löwe, 7 Skorpion, 10 Wassermann
  // Veränderlich: 2 Zwillinge, 5 Jungfrau, 8 Schütze, 11 Fische
  auto getQuality = [](int s) -> int { return (s >= 0) ? s % 3 : -1; };

  int el1 = getElement(sign1);
  int el2 = getElement(sign2);
  int qual1 = getQuality(sign1);
  int qual2 = getQuality(sign2);

  QString signName1 = (sign1 >= 0) ? getSignName(sign1) : "";
  QString signName2 = (sign2 >= 0) ? getSignName(sign2) : "";

  // Element-Namen
  auto elementName = [](int e) -> QString {
    const QString langLocal = analyzerLang();
    switch (e) {
    case 0: return astroTextStore().text(langLocal, "element.fire", "Feuer");
    case 1: return astroTextStore().text(langLocal, "element.earth", "Erde");
    case 2: return astroTextStore().text(langLocal, "element.air", "Luft");
    case 3: return astroTextStore().text(langLocal, "element.water", "Wasser");
    default: return "";
    }
  };

  // Qualitäts-Namen
  auto qualityName = [](int q) -> QString {
    const QString langLocal = analyzerLang();
    switch (q) {
    case 0: return astroTextStore().text(langLocal, "quality.cardinal", "kardinal");
    case 1: return astroTextStore().text(langLocal, "quality.fixed", "fix");
    case 2: return astroTextStore().text(langLocal, "quality.mutable", "veränderlich");
    default: return "";
    }
  };

  // Generischen Text als Basis holen
  QString generic = getGenericAspectText(planet1, planet2, aspect);
  if (generic.startsWith("<p>"))
    generic = generic.mid(3);
  if (generic.endsWith("</p>"))
    generic.chop(4);

  // Planetenposition hinzufügen
  if (!signName1.isEmpty() && !signName2.isEmpty()) {
    text += QString("<b>%1 in %2 %3 %4 in %5</b><br>")
                .arg(getPlanetName(planet1), signName1,
                     getAspectName(aspect),
                     getPlanetName(planet2), signName2);
  }

  // Aspekt-Text einfügen
  text += generic;

  // Element-Kontext hinzufügen
  if (el1 >= 0 && el2 >= 0) {
    text += "<br><br><i>";

    if (el1 == el2) {
      // Gleiche Elemente verstärken sich
      QString elName = elementName(el1);
      switch (el1) {
      case 0: // Feuer
        text += astroTextStore().text(lang, "context.same.fire",
                        "Beide Planeten stehen in %1-Zeichen, was diesem Aspekt besondere Dynamik, Energie und Leidenschaft verleiht. Hier ist viel Initiative und Unternehmungslust vorhanden, aber auch die Gefahr von Impulsivität und Ungeduld.")
                    .arg(elName);
        break;
      case 1: // Erde
        text += astroTextStore().text(lang, "context.same.earth",
                        "Beide Planeten stehen in %1-Zeichen, was diesem Aspekt Stabilität, Ausdauer und Pragmatismus verleiht. Sie gehen methodisch vor und bauen auf solide Fundamente. Materielle Sicherheit ist wichtig.")
                    .arg(elName);
        break;
      case 2: // Luft
        text += astroTextStore().text(lang, "context.same.air",
                        "Beide Planeten stehen in %1-Zeichen, was diesem Aspekt geistige Beweglichkeit und kommunikative Fähigkeiten verleiht. Ideen und sozialer Austausch stehen im Vordergrund. Objektivität ist eine Stärke.")
                    .arg(elName);
        break;
      case 3: // Wasser
        text += astroTextStore().text(lang, "context.same.water",
                        "Beide Planeten stehen in %1-Zeichen, was diesem Aspekt emotionale Tiefe und intuitive Qualitäten verleiht. Gefühle und Empfindungen sind hier sehr stark. Einfühlungsvermögen und Sensibilität prägen diesen Bereich.")
                    .arg(elName);
        break;
      }
    } else {
      // Unterschiedliche Elemente - Kombination beschreiben
      QString elName1 = elementName(el1);
      QString elName2 = elementName(el2);

      // Harmonische Element-Kombinationen
      if ((el1 == 0 && el2 == 2) || (el1 == 2 && el2 == 0)) {
        // Feuer-Luft
        text += astroTextStore().text(lang, "context.combo.fire_air",
                        "%1 (%2) und %3 (%4) verbinden sich hier zu einer energiegeladenen, enthusiastischen Kombination. Ideen werden in Aktion umgesetzt, Inspiration führt zu Taten.")
                    .arg(getPlanetName(planet1), elName1, getPlanetName(planet2), elName2);
      } else if ((el1 == 1 && el2 == 3) || (el1 == 3 && el2 == 1)) {
        // Erde-Wasser
        text += astroTextStore().text(lang, "context.combo.earth_water",
                        "%1 (%2) und %3 (%4) verbinden sich zu einer fruchtbaren, nährenden Kombination. Praktisches und Emotionales arbeiten zusammen. Gefühle finden konkreten Ausdruck.")
                    .arg(getPlanetName(planet1), elName1, getPlanetName(planet2), elName2);
      } else if ((el1 == 0 && el2 == 1) || (el1 == 1 && el2 == 0)) {
        // Feuer-Erde
        text += astroTextStore().text(lang, "context.combo.fire_earth",
                        "%1 (%2) und %3 (%4) verbinden Impuls mit Substanz. Der Feuerdrang will schnelle Ergebnisse, die Erde fordert Geduld. Diese Spannung kann zu konkreten Errungenschaften führen.")
                    .arg(getPlanetName(planet1), elName1, getPlanetName(planet2), elName2);
      } else if ((el1 == 0 && el2 == 3) || (el1 == 3 && el2 == 0)) {
        // Feuer-Wasser
        text += astroTextStore().text(lang, "context.combo.fire_water",
                        "%1 (%2) und %3 (%4) erzeugen Dampf - eine kraftvolle, aber volatile Mischung. Leidenschaft trifft auf Gefühl, was zu intensiven, aber auch stürmischen Energien führen kann.")
                    .arg(getPlanetName(planet1), elName1, getPlanetName(planet2), elName2);
      } else if ((el1 == 1 && el2 == 2) || (el1 == 2 && el2 == 1)) {
        // Erde-Luft
        text += astroTextStore().text(lang, "context.combo.earth_air",
                        "%1 (%2) und %3 (%4) verbinden Theorie mit Praxis. Ideen brauchen praktische Umsetzung, und die Erde erdet das manchmal zu abstrakte Denken der Luft.")
                    .arg(getPlanetName(planet1), elName1, getPlanetName(planet2), elName2);
      } else if ((el1 == 2 && el2 == 3) || (el1 == 3 && el2 == 2)) {
        // Luft-Wasser
        text += astroTextStore().text(lang, "context.combo.air_water",
                        "%1 (%2) und %3 (%4) verbinden Verstand mit Gefühl. Das kann zu künstlerischer Sensibilität führen, aber auch zu Konflikten zwischen Kopf und Herz.")
                    .arg(getPlanetName(planet1), elName1, getPlanetName(planet2), elName2);
      }
    }

    // Qualitäts-Kontext hinzufügen
    if (qual1 >= 0 && qual2 >= 0 && qual1 == qual2) {
      QString qualName = qualityName(qual1);
      text += astroTextStore().text(lang, "context.quality.same",
                                   " Beide Zeichen sind %1, was die Intensität dieses Aspekts verstärkt.")
                  .arg(qualName);
    }

    text += "</i>";
  }

  text += "</p>";
  return text;
}

} // namespace astro
