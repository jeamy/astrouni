#pragma once

// Orben-Daten-Struktur (1:1 Port von pfOrbenPlanet/pfOrbenHaus aus AU_INIT)
// Diese Struktur ist unabhängig von wxWidgets und kann überall verwendet werden
struct OrbenData {
    float radix_planet[17][7];   // 17 Planeten × 7 Aspekte (Radix)
    float radix_haus[12][7];     // 12 Häuser × 7 Aspekte (Radix)
    float transit_planet[17][7]; // 17 Planeten × 7 Aspekte (Transit)
    float transit_haus[12][7];   // 12 Häuser × 7 Aspekte (Transit)
    float synastrie_planet[17][7]; // 17 Planeten × 7 Aspekte (Synastrie)
    float synastrie_haus[12][7];   // 12 Häuser × 7 Aspekte (Synastrie)
};
