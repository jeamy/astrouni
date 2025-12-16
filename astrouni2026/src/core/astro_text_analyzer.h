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
#include <QMap>
#include <QString>

namespace astro {

/**
 * @brief Textanalyse für astrologische Horoskope
 */
class AstroTextAnalyzer {
public:
  AstroTextAnalyzer();
  
  /**
   * @brief Setzt die Orben-Einstellungen für Aspekt-Prüfung
   */
  void setOrben(const AuInit* auinit);

  /**
   * @brief Erstellt vollständige Radix-Analyse
   * @param radix Das Radix-Horoskop
   * @return HTML-formatierter Text
   */
  QString analyzeRadix(const Radix &radix) const;

  /**
   * @brief Erstellt Transit-Analyse
   */
  QString analyzeTransit(const Radix &radix) const;

  /**
   * @brief Erstellt Synastrie-Analyse
   */
  QString analyzeSynastry(const Radix &radix) const;

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
  QString analyzeAspect(int planet1, int planet2, int16_t aspect) const;

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
  QString getAspectName(int16_t aspectIndex) const;

  QString contextAspectText(int planet1, int8_t sign1, int planet2, int8_t sign2, int16_t aspect) const;
  QString planetPairText(int planet1, int planet2, int16_t aspect) const;
  QString genericAspectTemplate(int16_t aspect) const;
  
  // ASC-Aspekt-Text abrufen
  QString getAscAspectText(int planet, int16_t aspect) const;
  
  // MC-Aspekt-Text abrufen
  QString getMcAspectText(int planet, int16_t aspect) const;

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
  uint32_t makeAspectKey(int planet1, int planet2, int16_t aspect) const;
  QString getGenericAspectText(int planet1, int planet2, int16_t aspect) const;
  QString getPlanetPairText(int planet1, int planet2, int16_t aspect) const;
  QString getContextAspectText(int planet1, int8_t sign1, int planet2,
                               int8_t sign2, int16_t aspect) const;

  QString defaultGenericAspectTemplate(int16_t aspect) const;
  
  // Orben-Einstellungen (optional)
  const AuInit* m_auinit = nullptr;
  
  // Hilfsfunktion für Aspekt-Prüfung mit konfigurierten Orben
  bool checkAspektWithOrben(double pos1, double pos2, int p1, int p2, int aspekt, 
                            int horoTyp, double& exakterWinkel) const;
};

} // namespace astro
