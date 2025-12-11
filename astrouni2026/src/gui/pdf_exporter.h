/**
 * @file pdf_exporter.h
 * @brief PDF-Export für Horoskope
 */

#ifndef PDF_EXPORTER_H
#define PDF_EXPORTER_H

#include <QObject>
#include <QString>
#include <QWidget>
#include <QPainter>
#include <QPrinter>

#include "../core/data_types.h"

namespace astro {

class ChartWidget;

/**
 * @brief Exportiert Horoskope als PDF
 * 
 * Generiert ein schönes PDF mit:
 * - Radix-Grafik
 * - Positionen-Tabelle
 * - Aspekte-Tabelle
 * - Häuser-Tabelle
 * - Personen-Daten
 */
class PdfExporter : public QObject {
    Q_OBJECT

public:
    explicit PdfExporter(QObject* parent = nullptr);
    ~PdfExporter() = default;

    /**
     * @brief Exportiert ein Radix als PDF
     * @param radix Das zu exportierende Radix
     * @param auinit Die Einstellungen
     * @param chartWidget Das Chart-Widget für die Grafik
     * @param parent Parent-Widget für Dialoge
     * @return true bei Erfolg
     */
    bool exportRadix(const Radix& radix, const AuInit& auinit, 
                     ChartWidget* chartWidget, QWidget* parent);

    /**
     * @brief Druckt ein Radix direkt
     * @param radix Das zu druckende Radix
     * @param auinit Die Einstellungen
     * @param chartWidget Das Chart-Widget für die Grafik
     * @param parent Parent-Widget für Dialoge
     * @return true bei Erfolg
     */
    bool printRadix(const Radix& radix, const AuInit& auinit,
                    ChartWidget* chartWidget, QWidget* parent);

    /**
     * @brief Exportiert ein Synastrie/Transit als PDF
     * @param radix1 Erste Person (Radix)
     * @param radix2 Zweite Person (Transit/Synastrie)
     * @param auinit Die Einstellungen
     * @param chartWidget Das Chart-Widget für die Grafik
     * @param parent Parent-Widget für Dialoge
     * @return true bei Erfolg
     */
    bool exportSynastrie(const Radix& radix1, const Radix& radix2,
                         const AuInit& auinit, ChartWidget* chartWidget, QWidget* parent);

    /**
     * @brief Druckt ein Synastrie direkt
     */
    bool printSynastrie(const Radix& radix1, const Radix& radix2,
                        const AuInit& auinit, ChartWidget* chartWidget, QWidget* parent);

    /**
     * @brief Exportiert ein Transit als PDF
     */
    bool exportTransit(const Radix& radix, const Radix& transit,
                       const AuInit& auinit, ChartWidget* chartWidget, QWidget* parent);

    /**
     * @brief Druckt ein Transit direkt
     */
    bool printTransit(const Radix& radix, const Radix& transit,
                      const AuInit& auinit, ChartWidget* chartWidget, QWidget* parent);

private:
    /**
     * @brief Generiert das HTML für das PDF
     */
    QString generateHtml(const Radix& radix, const AuInit& auinit,
                         const QString& chartImageBase64);

    /**
     * @brief Generiert die Positionen-Tabelle als HTML
     */
    QString generatePositionenHtml(const Radix& radix, const AuInit& auinit);

    /**
     * @brief Generiert die Aspekte-Tabelle als HTML
     */
    QString generateAspekteHtml(const Radix& radix, const AuInit& auinit);

    /**
     * @brief Generiert die Häuser-Tabelle als HTML
     */
    QString generateHaeuserHtml(const Radix& radix);

    /**
     * @brief Konvertiert ein ChartWidget in ein Base64-Bild
     */
    QString chartToBase64(ChartWidget* chartWidget, int size);

    /**
     * @brief Formatiert einen Grad-Wert als String (z.B. "15°23'45" Widder")
     */
    QString formatDegree(double degree, bool withSign = true);

    /**
     * @brief Gibt das Sternzeichen-Index (0-11) für einen Grad zurück
     */
    int getZeichen(double degree);

    /**
     * @brief Gibt die Planeten-Farbe zurück
     */
    QString getPlanetColorHtml(int planet, const AuInit& auinit);

    /**
     * @brief Gibt die Planeten-Farbe zurück
     */
    QColor getPlanetColor(int planet, const AuInit& auinit);

    /**
     * @brief Gibt die Aspekt-Farbe zurück
     */
    QColor getAspektColor(int aspekt);

    /**
     * @brief Gibt die Sternzeichen-Farbe zurück (Element-Farben)
     */
    QColor getZeichenColor(int zeichen);

    /**
     * @brief Rendert die Seite mit QPainter
     */
    void renderPage(QPainter& painter, QPrinter& printer,
                    const Radix& radix, const AuInit& auinit,
                    ChartWidget* chartWidget);

    /**
     * @brief Rendert Synastrie-Seite mit QPainter
     */
    void renderSynastriePage(QPainter& painter, QPrinter& printer,
                              const Radix& radix1, const Radix& radix2,
                              const AuInit& auinit, ChartWidget* chartWidget);

    /**
     * @brief Rendert Transit-Seite mit QPainter
     * Seite 1: Person-Daten + Grafik + Positionen Person
     * Seite 2: Transit-Positionen + Aspekte
     */
    void renderTransitPage(QPainter& painter, QPrinter& printer,
                            const Radix& radix, const Radix& transit,
                            const AuInit& auinit, ChartWidget* chartWidget);
};

} // namespace astro

#endif // PDF_EXPORTER_H
