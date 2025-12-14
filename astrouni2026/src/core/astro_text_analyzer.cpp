/**
 * @file astro_text_analyzer.cpp
 * @brief Implementierung der automatischen Textanalyse für Horoskope
 */

#include "astro_text_analyzer.h"
#include "constants.h"
#include <QStringList>

namespace astro {

AstroTextAnalyzer::AstroTextAnalyzer() {
    initializePlanetNames();
    initializeSignNames();
    initializeAspectNames();
    initializeSunSignTexts();
    initializeAscendantTexts();
    initializeMoonSignTexts();
    initializeAspectTexts();
}

//==============================================================================
// Hauptanalyse-Funktion
//==============================================================================

QString AstroTextAnalyzer::analyzeRadix(const Radix& radix) const {
    QString html;
    
    // Header
    html += "<html><head><style>";
    html += "body { font-family: Arial, sans-serif; font-size: 11pt; }";
    html += "h1 { color: #2c3e50; font-size: 16pt; margin-top: 20px; }";
    html += "h2 { color: #34495e; font-size: 14pt; margin-top: 15px; }";
    html += "h3 { color: #7f8c8d; font-size: 12pt; margin-top: 10px; }";
    html += "p { margin: 8px 0; line-height: 1.5; }";
    html += ".planet-symbol { font-size: 14pt; color: #e74c3c; }";
    html += ".sign-symbol { font-size: 14pt; color: #3498db; }";
    html += ".aspect-symbol { font-size: 12pt; color: #9b59b6; }";
    html += "</style></head><body>";
    
    // Titel
    QString name = radix.rFix.vorname;
    if (!radix.rFix.name.isEmpty()) {
        if (!name.isEmpty()) name += " ";
        name += radix.rFix.name;
    }
    if (name.isEmpty()) name = "Radix";
    
    html += QString("<h1>Horoskop-Analyse: %1</h1>").arg(name);
    html += QString("<p><i>Geboren am %1 um %2 Uhr in %3</i></p>")
        .arg(radix.datum)
        .arg(radix.zeit)
        .arg(radix.rFix.ort);
    
    html += "<hr>";
    
    // 1. Sonnenzeichen (Hauptthema)
    if (radix.stzPlanet.size() > P_SONNE) {
        int8_t sunSign = radix.stzPlanet[P_SONNE];
        html += "<h2>🌟 Sonnenzeichen: " + getSignName(sunSign) + "</h2>";
        html += QString("<p class='sign-symbol'>%1</p>").arg(STERNZEICHEN_SYMBOLS[sunSign]);
        html += analyzeSunSign(sunSign);
    }
    
    // 2. Aszendent
    if (radix.stzHaus.size() > 0) {
        int8_t ascSign = radix.stzHaus[0];
        html += "<h2>🌅 Aszendent: " + getSignName(ascSign) + "</h2>";
        html += QString("<p class='sign-symbol'>%1</p>").arg(STERNZEICHEN_SYMBOLS[ascSign]);
        html += analyzeAscendant(ascSign);
    }
    
    // 3. Mondzeichen
    if (radix.stzPlanet.size() > P_MOND) {
        int8_t moonSign = radix.stzPlanet[P_MOND];
        html += "<h2>🌙 Mondzeichen: " + getSignName(moonSign) + "</h2>";
        html += QString("<p class='sign-symbol'>%1</p>").arg(STERNZEICHEN_SYMBOLS[moonSign]);
        html += analyzeMoonSign(moonSign);
    }
    
    // 4. Aspekte
    html += "<h2>✨ Wichtige Aspekte</h2>";
    html += "<p><i>Aspekte zeigen die Beziehungen zwischen den Planeten.</i></p>";
    
    // Aspekte durchgehen
    int aspectCount = 0;
    for (int i = 0; i < radix.anzahlPlanet && i < MAX_PLANET; i++) {
        for (int j = i + 1; j < radix.anzahlPlanet && j < MAX_PLANET; j++) {
            int aspIndex = i * MAX_PLANET + j;
            if (aspIndex < radix.aspPlanet.size()) {
                int8_t aspect = radix.aspPlanet[aspIndex];
                if (aspect != KEIN_ASP) {
                    html += QString("<h3><span class='planet-symbol'>%1</span> %2 <span class='aspect-symbol'>%3</span> <span class='planet-symbol'>%4</span> %5</h3>")
                        .arg(PLANET_SYMBOLS[i])
                        .arg(getPlanetName(i))
                        .arg(getAspectName(aspect))
                        .arg(PLANET_SYMBOLS[j])
                        .arg(getPlanetName(j));
                    html += analyzeAspect(i, j, aspect);
                    aspectCount++;
                    
                    // Maximal 10 Aspekte anzeigen
                    if (aspectCount >= 10) break;
                }
            }
        }
        if (aspectCount >= 10) break;
    }
    
    if (aspectCount == 0) {
        html += "<p><i>Keine signifikanten Aspekte gefunden.</i></p>";
    }
    
    html += "</body></html>";
    return html;
}

//==============================================================================
// Initialisierung der Namen
//==============================================================================

void AstroTextAnalyzer::initializePlanetNames() {
    m_planetNames[P_SONNE]   = "Sonne";
    m_planetNames[P_MOND]    = "Mond";
    m_planetNames[P_MERKUR]  = "Merkur";
    m_planetNames[P_VENUS]   = "Venus";
    m_planetNames[P_MARS]    = "Mars";
    m_planetNames[P_JUPITER] = "Jupiter";
    m_planetNames[P_SATURN]  = "Saturn";
    m_planetNames[P_URANUS]  = "Uranus";
    m_planetNames[P_NEPTUN]  = "Neptun";
    m_planetNames[P_PLUTO]   = "Pluto";
    m_planetNames[P_NKNOTEN] = "Mondknoten";
    m_planetNames[P_LILITH]  = "Lilith";
    m_planetNames[P_CHIRON]  = "Chiron";
    m_planetNames[P_CERES]   = "Ceres";
    m_planetNames[P_PALLAS]  = "Pallas";
    m_planetNames[P_JUNO]    = "Juno";
    m_planetNames[P_VESTA]   = "Vesta";
}

void AstroTextAnalyzer::initializeSignNames() {
    m_signNames[iWIDDER]     = "Widder";
    m_signNames[iSTIER]      = "Stier";
    m_signNames[iZWILLINGE]  = "Zwillinge";
    m_signNames[iKREBS]      = "Krebs";
    m_signNames[iLOWE]       = "Löwe";
    m_signNames[iJUNGFRAU]   = "Jungfrau";
    m_signNames[iWAAGE]      = "Waage";
    m_signNames[iSKORPION]   = "Skorpion";
    m_signNames[iSCHUTZE]    = "Schütze";
    m_signNames[iSTEINBOCK]  = "Steinbock";
    m_signNames[iWASSERMANN] = "Wassermann";
    m_signNames[iFISCHE]     = "Fische";
}

void AstroTextAnalyzer::initializeAspectNames() {
    m_aspectNames[KONJUNKTION] = "Konjunktion";
    m_aspectNames[HALBSEX]     = "Halbsextil";
    m_aspectNames[SEXTIL]      = "Sextil";
    m_aspectNames[QUADRATUR]   = "Quadrat";
    m_aspectNames[TRIGON]      = "Trigon";
    m_aspectNames[QUINCUNX]    = "Quincunx";
    m_aspectNames[OPOSITION]   = "Opposition";
}

//==============================================================================
// Getter-Funktionen
//==============================================================================

QString AstroTextAnalyzer::getPlanetName(int planetIndex) const {
    return m_planetNames.value(planetIndex, "Unbekannt");
}

QString AstroTextAnalyzer::getSignName(int8_t signIndex) const {
    return m_signNames.value(signIndex, "Unbekannt");
}

QString AstroTextAnalyzer::getAspectName(int8_t aspectIndex) const {
    return m_aspectNames.value(aspectIndex, "Unbekannt");
}

//==============================================================================
// Hilfsfunktionen
//==============================================================================

uint32_t AstroTextAnalyzer::makeAspectKey(int planet1, int planet2, int8_t aspect) const {
    return (planet1 << 16) | (planet2 << 8) | aspect;
}

QString AstroTextAnalyzer::getGenericAspectText(int planet1, int planet2, int8_t aspect) const {
    QString text = "<p>";
    
    // Generische Beschreibung basierend auf Aspekt-Typ
    switch (aspect) {
        case KONJUNKTION:
            text += QString("Die <b>Konjunktion</b> zwischen %1 und %2 verbindet diese beiden "
                          "Energien zu einer kraftvollen Einheit. Sie verstärken sich gegenseitig.")
                .arg(getPlanetName(planet1)).arg(getPlanetName(planet2));
            break;
            
        case SEXTIL:
            text += QString("Das <b>Sextil</b> zwischen %1 und %2 zeigt harmonische Möglichkeiten. "
                          "Dieser Aspekt fördert kreative Lösungen und positive Entwicklung.")
                .arg(getPlanetName(planet1)).arg(getPlanetName(planet2));
            break;
            
        case QUADRATUR:
            text += QString("Das <b>Quadrat</b> zwischen %1 und %2 erzeugt Spannung und Herausforderungen. "
                          "Diese Reibung kann zu Wachstum und Entwicklung führen.")
                .arg(getPlanetName(planet1)).arg(getPlanetName(planet2));
            break;
            
        case TRIGON:
            text += QString("Das <b>Trigon</b> zwischen %1 und %2 bringt natürliche Harmonie. "
                          "Diese Energie fließt leicht und unterstützt Ihre Talente.")
                .arg(getPlanetName(planet1)).arg(getPlanetName(planet2));
            break;
            
        case OPOSITION:
            text += QString("Die <b>Opposition</b> zwischen %1 und %2 zeigt Polarität. "
                          "Diese Spannung erfordert Balance und Integration beider Seiten.")
                .arg(getPlanetName(planet1)).arg(getPlanetName(planet2));
            break;
            
        default:
            text += QString("Der Aspekt %1 zwischen %2 und %3 beeinflusst Ihre Persönlichkeit.")
                .arg(getAspectName(aspect))
                .arg(getPlanetName(planet1))
                .arg(getPlanetName(planet2));
            break;
    }
    
    text += "</p>";
    return text;
}

} // namespace astro

//==============================================================================
// Sonnenzeichen-Texte
//==============================================================================

void AstroTextAnalyzer::initializeSunSignTexts() {
    m_sunSignTexts[iWIDDER] = 
        "<p>Als <b>Widder</b> sind Sie ein geborener Pionier. Ihre Energie ist direkt, spontan "
        "und mutig. Sie lieben es, neue Wege zu gehen und Herausforderungen anzunehmen. "
        "Ihr Durchsetzungsvermögen und Ihre Entschlossenheit sind beeindruckend.</p>"
        "<p>Sie sind initiativ, unabhängig und haben einen starken Willen. Ungeduld kann "
        "manchmal eine Schwäche sein, aber Ihre Begeisterungsfähigkeit gleicht das aus. "
        "Sie sind ein natürlicher Anführer mit viel Selbstvertrauen.</p>";
    
    m_sunSignTexts[iSTIER] = 
        "<p>Als <b>Stier</b> schätzen Sie Stabilität, Sicherheit und die schönen Dinge des Lebens. "
        "Sie sind praktisch veranlagt, zuverlässig und haben eine natürliche Geduld. "
        "Ihre Sinne sind besonders ausgeprägt - Sie genießen Komfort und Qualität.</p>"
        "<p>Sie sind beständig, treu und haben einen ausgeprägten Sinn für Ästhetik. "
        "Ihre Bodenständigkeit macht Sie zu einem verlässlichen Partner. Manchmal kann "
        "Ihre Sturheit herausfordernd sein, aber sie gibt Ihnen auch Durchhaltevermögen.</p>";
    
    m_sunSignTexts[iZWILLINGE] = 
        "<p>Als <b>Zwilling</b> sind Sie vielseitig, neugierig und kommunikativ. Ihr Geist ist "
        "beweglich und Sie lieben den Austausch von Ideen. Sie sind flexibel, anpassungsfähig "
        "und haben einen natürlichen Humor.</p>"
        "<p>Ihre Stärke liegt in der Kommunikation und im logischen Denken. Sie brauchen "
        "geistige Anregung und Abwechslung. Ihre Fähigkeit, mehrere Dinge gleichzeitig "
        "zu tun, ist bemerkenswert. Oberflächlichkeit kann eine Falle sein, aber Ihre "
        "Vielseitigkeit ist ein großes Geschenk.</p>";
    
    m_sunSignTexts[iKREBS] = 
        "<p>Als <b>Krebs</b> sind Sie sensibel, fürsorglich und emotional tiefgründig. "
        "Familie und Zuhause sind Ihnen besonders wichtig. Sie haben eine starke Intuition "
        "und ein ausgeprägtes Gefühlsleben.</p>"
        "<p>Sie sind beschützend, empathisch und haben ein gutes Gedächtnis. Ihre emotionale "
        "Intelligenz ist hoch entwickelt. Sie brauchen emotionale Sicherheit und schaffen "
        "gerne ein gemütliches Heim. Ihre Launenhaftigkeit kann herausfordernd sein, aber "
        "Ihre Fürsorglichkeit macht Sie zu einem wertvollen Menschen.</p>";
    
    m_sunSignTexts[iLOWE] = 
        "<p>Als <b>Löwe</b> strahlen Sie natürliche Würde und Selbstbewusstsein aus. Sie sind "
        "großzügig, kreativ und lieben es, im Mittelpunkt zu stehen. Ihre Lebensfreude "
        "und Ihr Optimismus sind ansteckend.</p>"
        "<p>Sie haben ein starkes Bedürfnis nach Anerkennung und Ausdruck Ihrer Kreativität. "
        "Ihr Herz ist groß und loyal. Sie sind stolz, dramatisch und haben einen natürlichen "
        "Sinn für Theater. Manchmal kann Ihr Ego im Weg stehen, aber Ihre Großzügigkeit "
        "und Wärme machen Sie beliebt.</p>";
    
    m_sunSignTexts[iJUNGFRAU] = 
        "<p>Als <b>Jungfrau</b> sind Sie analytisch, präzise und dienstbereit. Sie haben einen "
        "ausgeprägten Sinn für Details und Ordnung. Ihre praktische Intelligenz und "
        "Ihr Perfektionismus zeichnen Sie aus.</p>"
        "<p>Sie sind fleißig, zuverlässig und kritisch - vor allem mit sich selbst. Ihre "
        "Fähigkeit zu organisieren und zu verbessern ist bemerkenswert. Sie brauchen "
        "nützliche Tätigkeiten und schätzen Effizienz. Überkritik kann eine Schwäche sein, "
        "aber Ihre Hilfsbereitschaft ist ein großes Geschenk.</p>";
    
    m_sunSignTexts[iWAAGE] = 
        "<p>Als <b>Waage</b> suchen Sie Harmonie, Balance und Schönheit. Sie sind diplomatisch, "
        "charmant und haben einen ausgeprägten Sinn für Gerechtigkeit. Beziehungen sind "
        "Ihnen besonders wichtig.</p>"
        "<p>Sie sind höflich, kooperativ und haben einen natürlichen Geschmackssinn. Ihre "
        "Fähigkeit, verschiedene Perspektiven zu sehen, macht Sie zu einem guten Vermittler. "
        "Sie brauchen Partnerschaft und Ästhetik. Entscheidungsschwierigkeiten können "
        "herausfordernd sein, aber Ihr Streben nach Ausgewogenheit ist wertvoll.</p>";
    
    m_sunSignTexts[iSKORPION] = 
        "<p>Als <b>Skorpion</b> sind Sie intensiv, leidenschaftlich und transformativ. Sie gehen "
        "in die Tiefe und scheuen keine emotionalen Extreme. Ihre Willenskraft und "
        "Durchsetzungsfähigkeit sind beeindruckend.</p>"
        "<p>Sie sind magnetisch, geheimnisvoll und haben eine starke Regenerationskraft. "
        "Ihre Fähigkeit zur Transformation ist einzigartig. Sie brauchen emotionale Tiefe "
        "und Authentizität. Kontrollbedürfnis kann eine Falle sein, aber Ihre Intensität "
        "und Treue sind außergewöhnlich.</p>";
    
    m_sunSignTexts[iSCHUTZE] = 
        "<p>Als <b>Schütze</b> sind Sie optimistisch, philosophisch und freiheitsliebend. Sie "
        "haben einen weiten Horizont und lieben es zu reisen - physisch oder geistig. "
        "Ihre Begeisterungsfähigkeit ist ansteckend.</p>"
        "<p>Sie sind ehrlich, großzügig und haben einen ausgeprägten Sinn für Humor. Ihre "
        "Suche nach Wahrheit und Bedeutung treibt Sie an. Sie brauchen Freiheit und "
        "Expansion. Übertreibung kann eine Schwäche sein, aber Ihr Optimismus und Ihre "
        "Weitsicht sind inspirierend.</p>";
    
    m_sunSignTexts[iSTEINBOCK] = 
        "<p>Als <b>Steinbock</b> sind Sie ambitioniert, diszipliniert und verantwortungsbewusst. "
        "Sie haben einen klaren Plan und arbeiten beharrlich auf Ihre Ziele hin. Ihre "
        "Ausdauer ist bewundernswert.</p>"
        "<p>Sie sind praktisch, traditionsbewusst und haben einen trockenen Humor. Ihre "
        "Fähigkeit zu strukturieren und aufzubauen ist bemerkenswert. Sie brauchen Erfolg "
        "und Anerkennung für Ihre Leistungen. Übermäßige Strenge kann herausfordernd sein, "
        "aber Ihre Zuverlässigkeit und Ihr Durchhaltevermögen sind ein Fundament.</p>";
    
    m_sunSignTexts[iWASSERMANN] = 
        "<p>Als <b>Wassermann</b> sind Sie individualistisch, innovativ und humanitär orientiert. "
        "Sie denken fortschrittlich und schätzen Ihre Unabhängigkeit. Ihre Originalität "
        "und Ihr Ideenreichtum sind beeindruckend.</p>"
        "<p>Sie sind freundschaftlich, objektiv und haben einen ausgeprägten Gemeinschaftssinn. "
        "Ihre Fähigkeit, über den Tellerrand zu schauen, ist wertvoll. Sie brauchen geistige "
        "Freiheit und soziale Verbindungen. Distanziertheit kann eine Schwäche sein, aber "
        "Ihre Vision einer besseren Welt ist inspirierend.</p>";
    
    m_sunSignTexts[iFISCHE] = 
        "<p>Als <b>Fische</b> sind Sie einfühlsam, kreativ und spirituell veranlagt. Sie haben "
        "eine reiche Innenwelt und starke Intuition. Ihre Empathie und Ihr Mitgefühl "
        "sind außergewöhnlich.</p>"
        "<p>Sie sind anpassungsfähig, romantisch und haben eine künstlerische Ader. Ihre "
        "Fähigkeit, zwischen den Welten zu wandeln, ist einzigartig. Sie brauchen "
        "Inspiration und transzendente Erfahrungen. Grenzenlosigkeit kann herausfordernd "
        "sein, aber Ihre Sensibilität und Ihr Mitgefühl sind ein Geschenk.</p>";
}


//==============================================================================
// Aszendenten-Texte
//==============================================================================

void AstroTextAnalyzer::initializeAscendantTexts() {
    m_ascendantTexts[iWIDDER] = 
        "<p>Mit <b>Widder-Aszendent</b> wirken Sie energiegeladen, direkt und selbstbewusst. "
        "Sie treten mutig und entschlossen auf. Andere erleben Sie als dynamisch und initiativ.</p>";
    
    m_ascendantTexts[iSTIER] = 
        "<p>Mit <b>Stier-Aszendent</b> wirken Sie ruhig, beständig und verlässlich. "
        "Sie strahlen Stabilität und Genuss aus. Andere erleben Sie als geerdet und sinnlich.</p>";
    
    m_ascendantTexts[iZWILLINGE] = 
        "<p>Mit <b>Zwillinge-Aszendent</b> wirken Sie neugierig, kommunikativ und beweglich. "
        "Sie treten gesprächig und vielseitig auf. Andere erleben Sie als geistreich und anpassungsfähig.</p>";
    
    m_ascendantTexts[iKREBS] = 
        "<p>Mit <b>Krebs-Aszendent</b> wirken Sie sensibel, fürsorglich und zurückhaltend. "
        "Sie strahlen Wärme und Empathie aus. Andere erleben Sie als emotional und schützend.</p>";
    
    m_ascendantTexts[iLOWE] = 
        "<p>Mit <b>Löwe-Aszendent</b> wirken Sie selbstbewusst, großzügig und würdevoll. "
        "Sie treten strahlend und stolz auf. Andere erleben Sie als charismatisch und warmherzig.</p>";
    
    m_ascendantTexts[iJUNGFRAU] = 
        "<p>Mit <b>Jungfrau-Aszendent</b> wirken Sie ordentlich, präzise und zurückhaltend. "
        "Sie strahlen Kompetenz und Bescheidenheit aus. Andere erleben Sie als hilfsbereit und analytisch.</p>";
    
    m_ascendantTexts[iWAAGE] = 
        "<p>Mit <b>Waage-Aszendent</b> wirken Sie charmant, harmonisch und diplomatisch. "
        "Sie treten höflich und ausgeglichen auf. Andere erleben Sie als ästhetisch und kooperativ.</p>";
    
    m_ascendantTexts[iSKORPION] = 
        "<p>Mit <b>Skorpion-Aszendent</b> wirken Sie intensiv, magnetisch und geheimnisvoll. "
        "Sie strahlen Tiefe und Macht aus. Andere erleben Sie als leidenschaftlich und durchdringend.</p>";
    
    m_ascendantTexts[iSCHUTZE] = 
        "<p>Mit <b>Schütze-Aszendent</b> wirken Sie optimistisch, offen und philosophisch. "
        "Sie treten enthusiastisch und weitherzig auf. Andere erleben Sie als inspirierend und freiheitsliebend.</p>";
    
    m_ascendantTexts[iSTEINBOCK] = 
        "<p>Mit <b>Steinbock-Aszendent</b> wirken Sie ernst, zielstrebig und verantwortungsvoll. "
        "Sie strahlen Autorität und Disziplin aus. Andere erleben Sie als kompetent und ausdauernd.</p>";
    
    m_ascendantTexts[iWASSERMANN] = 
        "<p>Mit <b>Wassermann-Aszendent</b> wirken Sie individuell, innovativ und unabhängig. "
        "Sie treten originell und freundschaftlich auf. Andere erleben Sie als fortschrittlich und unkonventionell.</p>";
    
    m_ascendantTexts[iFISCHE] = 
        "<p>Mit <b>Fische-Aszendent</b> wirken Sie sanft, einfühlsam und verträumt. "
        "Sie strahlen Mitgefühl und Spiritualität aus. Andere erleben Sie als sensitiv und künstlerisch.</p>";
}

//==============================================================================
// Mondzeichen-Texte
//==============================================================================

void AstroTextAnalyzer::initializeMoonSignTexts() {
    m_moonSignTexts[iWIDDER] = 
        "<p>Mit <b>Mond in Widder</b> reagieren Sie spontan und direkt auf emotionale Situationen. "
        "Sie brauchen Action und Unabhängigkeit. Ihre Gefühle sind intensiv aber oft kurz.</p>";
    
    m_moonSignTexts[iSTIER] = 
        "<p>Mit <b>Mond in Stier</b> suchen Sie emotionale Sicherheit und Stabilität. "
        "Sie brauchen Komfort und Beständigkeit. Ihre Gefühle sind tief und dauerhaft.</p>";
    
    m_moonSignTexts[iZWILLINGE] = 
        "<p>Mit <b>Mond in Zwillinge</b> brauchen Sie geistige Stimulation und Kommunikation. "
        "Sie verarbeiten Gefühle durch Sprechen. Ihre Emotionen sind beweglich und vielseitig.</p>";
    
    m_moonSignTexts[iKREBS] = 
        "<p>Mit <b>Mond in Krebs</b> sind Ihre Gefühle sehr ausgeprägt und intensiv. "
        "Sie brauchen ein emotionales Zuhause. Ihre Fürsorglichkeit ist tief verwurzelt.</p>";
    
    m_moonSignTexts[iLOWE] = 
        "<p>Mit <b>Mond in Löwe</b> brauchen Sie emotionale Anerkennung und Wärme. "
        "Sie drücken Gefühle großzügig aus. Ihr inneres Kind ist lebendig und kreativ.</p>";
    
    m_moonSignTexts[iJUNGFRAU] = 
        "<p>Mit <b>Mond in Jungfrau</b> verarbeiten Sie Emotionen analytisch. "
        "Sie brauchen Ordnung auch im Gefühlsleben. Ihre Fürsorge zeigt sich praktisch.</p>";
    
    m_moonSignTexts[iWAAGE] = 
        "<p>Mit <b>Mond in Waage</b> suchen Sie emotionale Harmonie und Balance. "
        "Sie brauchen Beziehungen für Ihr Wohlbefinden. Ihre Gefühle sind auf Ausgleich ausgerichtet.</p>";
    
    m_moonSignTexts[iSKORPION] = 
        "<p>Mit <b>Mond in Skorpion</b> erleben Sie Emotionen mit extremer Intensität. "
        "Sie brauchen emotionale Tiefe und Transformation. Ihre Gefühle sind leidenschaftlich.</p>";
    
    m_moonSignTexts[iSCHUTZE] = 
        "<p>Mit <b>Mond in Schütze</b> brauchen Sie emotionale Freiheit und Optimismus. "
        "Sie verarbeiten Gefühle philosophisch. Ihr inneres Selbst sucht nach Sinn und Expansion.</p>";
    
    m_moonSignTexts[iSTEINBOCK] = 
        "<p>Mit <b>Mond in Steinbock</b> kontrollieren Sie Ihre Emotionen diszipliniert. "
        "Sie brauchen emotionale Sicherheit durch Leistung. Ihre Gefühle sind verantwortungsvoll.</p>";
    
    m_moonSignTexts[iWASSERMANN] = 
        "<p>Mit <b>Mond in Wassermann</b> brauchen Sie emotionale Unabhängigkeit und Freiheit. "
        "Sie verarbeiten Gefühle objektiv. Ihr inneres Selbst ist individualistisch.</p>";
    
    m_moonSignTexts[iFISCHE] = 
        "<p>Mit <b>Mond in Fische</b> sind Sie emotional sehr empfänglich und mitfühlend. "
        "Sie brauchen spirituelle Verbindung. Ihre Gefühle sind grenzenlos und intuitiv.</p>";
}

//==============================================================================
// Aspekte-Texte (Beispiele für wichtige Aspekte)
//==============================================================================

void AstroTextAnalyzer::initializeAspectTexts() {
    // Sonne-Mond Aspekte
    m_aspectTexts[makeAspectKey(P_SONNE, P_MOND, KONJUNKTION)] =
        "<p>Die Verbindung von Sonne und Mond zeigt Einheit von Wille und Gefühl. "
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
    return m_sunSignTexts.value(sternzeichen, 
        "<p><i>Keine detaillierte Beschreibung verfügbar.</i></p>");
}

QString AstroTextAnalyzer::analyzeAscendant(int8_t sternzeichen) const {
    return m_ascendantTexts.value(sternzeichen,
        "<p><i>Keine detaillierte Beschreibung verfügbar.</i></p>");
}

QString AstroTextAnalyzer::analyzeMoonSign(int8_t sternzeichen) const {
    return m_moonSignTexts.value(sternzeichen,
        "<p><i>Keine detaillierte Beschreibung verfügbar.</i></p>");
}

QString AstroTextAnalyzer::analyzeAspect(int planet1, int planet2, int8_t aspect) const {
    uint32_t key = makeAspectKey(planet1, planet2, aspect);
    
    // Spezifischen Text suchen
    if (m_aspectTexts.contains(key)) {
        return m_aspectTexts[key];
    }
    
    // Auch umgekehrte Reihenfolge probieren
    uint32_t reverseKey = makeAspectKey(planet2, planet1, aspect);
    if (m_aspectTexts.contains(reverseKey)) {
        return m_aspectTexts[reverseKey];
    }
    
    // Generische Beschreibung verwenden
    return getGenericAspectText(planet1, planet2, aspect);
}

} // namespace astro
