#pragma once
/**
 * @file astro_text_analyzer.h
 * @brief Automatische Textanalyse für Horoskope
 * 
 * Erstellt lesbare Interpretationen von:
 * - Radix (Geburtshoroskop)
 * - Transit
 * - Synastrie
 */

#include "data_types.h"
#include <QString>
#include <QMap>

namespace astro {

/**
 * @brief Textanalyse für astrologische Horoskope
 */
class AstroTextAnalyzer {
public:
    AstroTextAnalyzer();
    
    /**
     * @brief Erstellt vollständige Radix-Analyse
     * @param radix Das Radix-Horoskop
     * @return HTML-formatierter Text
     */
    QString analyzeRadix(const Radix& radix) const;
    
    /**
     * @brief Analysiert Sonne im Sternzeichen
     */
    QString analyzeSunSign(int8_t sternzeichen) const;
    
    /**
     * @brief Analysiert Aszendent
     */
    QString analyzeAscendant(int8_t sternzeichen) const;
    
    /**
     * @brief Analysiert einen Aspekt
     */
    QString analyzeAspect(int planet1, int planet2, int8_t aspect) const;
    
    /**
     * @brief Analysiert Mond im Sternzeichen
     */
    QString analyzeMoonSign(int8_t sternzeichen) const;
    
    /**
     * @brief Gibt Planeten-Namen zurück
     */
    QString getPlanetName(int planetIndex) const;
    
    /**
     * @brief Gibt Sternzeichen-Namen zurück
     */
    QString getSignName(int8_t signIndex) const;
    
    /**
     * @brief Gibt Aspekt-Namen zurück
     */
    QString getAspectName(int8_t aspectIndex) const;

private:
    // Texte für Sonnenzeichen
    QMap<int, QString> m_sunSignTexts;
    
    // Texte für Aszendenten
    QMap<int, QString> m_ascendantTexts;
    
    // Texte für Mondzeichen
    QMap<int, QString> m_moonSignTexts;
    
    // Texte für Aspekte (Key: Planet1 << 16 | Planet2 << 8 | Aspect)
    QMap<uint32_t, QString> m_aspectTexts;
    
    // Planeten-Namen
    QMap<int, QString> m_planetNames;
    
    // Sternzeichen-Namen
    QMap<int, QString> m_signNames;
    
    // Aspekt-Namen
    QMap<int, QString> m_aspectNames;
    
    // Initialisierung
    void initializePlanetNames();
    void initializeSignNames();
    void initializeAspectNames();
    void initializeSunSignTexts();
    void initializeAscendantTexts();
    void initializeMoonSignTexts();
    void initializeAspectTexts();
    
    // Hilfsfunktionen
    uint32_t makeAspectKey(int planet1, int planet2, int8_t aspect) const;
    QString getGenericAspectText(int planet1, int planet2, int8_t aspect) const;
};

} // namespace astro
