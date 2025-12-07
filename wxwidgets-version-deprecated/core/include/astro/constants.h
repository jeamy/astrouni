#pragma once

namespace astro {

// Menü-IDs aus astrouni.h (CM_U_*)
// Starten bei 10000, um Konflikte mit wxWidgets Stock-IDs zu vermeiden.
enum class MenuID {
    ID_BASE = 10000,

    // Datei
    PERSON = ID_BASE + 1,
    ORT,
    DRUCK,
    TRANSIT,
    RUCKLAUF,
    APP_EXIT,

    // Bearbeiten
    KOPIE = ID_BASE + 100,
    EINFUG,

    // Optionen
    HAUSER = ID_BASE + 200,
    ORBEN,
    FARBEN,
    EINST,
    HOROTYP,

    // Hilfe
    APP_INFO = ID_BASE + 800,
    HILFE_P,
    HILFE_O,

    // Radix-Fenster Kontextmenü (CM_UR_*)
    UR_EXIT = ID_BASE + 1000,
    UR_SAVE,
    UR_PRINT,
    UR_HOROTYP,
    UR_HELP,

    // Transit-Fenster Kontextmenü (CM_UT_*)
    UT_EXIT = ID_BASE + 1100,
    UT_SAVE,
    UT_PRINT,
    UT_HOROTYP,
    UT_HELP,
};

} // namespace astro
