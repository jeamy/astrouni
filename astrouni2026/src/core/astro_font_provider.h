/**
 * @file astro_font_provider.h
 * @brief Provider für Astrologie-Symbole mit Font-Fallback
 * 
 * Prüft ob der AstroUniverse-Font installiert ist und liefert
 * entsprechende Symbole (Legacy-Font oder Unicode-Fallback).
 */

#ifndef ASTRO_FONT_PROVIDER_H
#define ASTRO_FONT_PROVIDER_H

#include <QString>
#include <QStringList>
#include <QFont>

namespace astro {

/**
 * @class AstroFontProvider
 * @brief Singleton für Astrologie-Symbol-Fonts
 * 
 * Legacy-Port: Der Original-Code verwendete den Font "asu_____.ttf"
 * mit speziellen Zeichen für Planeten und Sternzeichen.
 * Diese Klasse prüft ob der Font verfügbar ist und liefert
 * entweder die Legacy-Zeichen oder Unicode-Fallbacks.
 */
class AstroFontProvider {
public:
    static AstroFontProvider& instance();
    
    /**
     * @brief Prüft ob der AstroUniverse-Font verfügbar ist
     */
    bool hasAstroFont() const { return m_hasAstroFont; }
    
    /**
     * @brief Gibt den Font für Astrologie-Symbole zurück
     * @param pointSize Schriftgröße
     * @return QFont (AstroUniverse wenn verfügbar, sonst System-Font)
     */
    QFont getSymbolFont(int pointSize = 12) const;
    
    /**
     * @brief Gibt den Font-Namen zurück
     */
    QString fontName() const { return m_fontName; }
    
    // Planeten-Symbole (Index entspricht P_SONNE, P_MOND, etc.)
    QString planetSymbol(int planet) const;
    
    // Sternzeichen-Symbole (Index 0-11)
    QString sternzeichenSymbol(int zeichen) const;
    
    // Aspekt-Symbole (Index entspricht Aspekt-Typ)
    QString aspektSymbol(int aspekt) const;
    
    // Retrograde-Symbol
    QString retrogradeSymbol() const;
    
    // Grad-Symbol
    QString gradSymbol() const;

private:
    AstroFontProvider();
    ~AstroFontProvider() = default;
    
    void detectFont();
    void initSymbols();
    
    bool m_hasAstroFont;
    QString m_fontName;
    
    // Legacy-Font Zeichen (wenn AstroUniverse installiert)
    // Diese entsprechen den Zeichen aus legacy/auwurzel.h szLNANamen[]
    QStringList m_legacyPlanetSymbols;
    QStringList m_legacySternzeichenSymbols;
    QStringList m_legacyAspektSymbols;
    QString m_legacyRetrograde;
    QString m_legacyGrad;
    
    // Unicode-Fallback Zeichen
    QStringList m_unicodePlanetSymbols;
    QStringList m_unicodeSternzeichenSymbols;
    QStringList m_unicodeAspektSymbols;
    QString m_unicodeRetrograde;
    QString m_unicodeGrad;
};

// Globale Zugriffsfunktion
inline AstroFontProvider& astroFont() {
    return AstroFontProvider::instance();
}

} // namespace astro

#endif // ASTRO_FONT_PROVIDER_H
