#include "person_dialog.h"
#include "person_search_dialog.h"
#include "legacy_data_manager.h"
#include <wx/gbsizer.h>

// 1:1 Port von DlgPErfassen aus astrofil.c
PersonDialog::PersonDialog(wxWindow* parent, astro::LegacyRadix& radix_data)
    : wxDialog(parent, wxID_ANY, "Person Erfassen/Bearbeiten"), radix_data_(radix_data) {

    wxGridBagSizer* sizer = new wxGridBagSizer(5, 5);

    // Zeile 0: Vorname, Name
    sizer->Add(new wxStaticText(this, wxID_ANY, "Vorname:"), wxGBPosition(0, 0), wxGBSpan(1, 1), wxALIGN_CENTER_VERTICAL | wxALL, 5);
    vorname_ctrl_ = new wxTextCtrl(this, wxID_ANY, radix_data_.szVorName);
    sizer->Add(vorname_ctrl_, wxGBPosition(0, 1), wxGBSpan(1, 1), wxEXPAND);
    sizer->Add(new wxStaticText(this, wxID_ANY, "Name:"), wxGBPosition(0, 2), wxGBSpan(1, 1), wxALIGN_CENTER_VERTICAL | wxALL, 5);
    name_ctrl_ = new wxTextCtrl(this, wxID_ANY, radix_data_.szName);
    sizer->Add(name_ctrl_, wxGBPosition(0, 3), wxGBSpan(1, 1), wxEXPAND);
    wxButton* search_name_button = new wxButton(this, wxID_ANY, "Suchen...");
    sizer->Add(search_name_button, wxGBPosition(0, 4), wxGBSpan(1, 1), wxEXPAND);

    // Zeile 1: Datum, Zeit
    sizer->Add(new wxStaticText(this, wxID_ANY, "Geburtsdatum:"), wxGBPosition(1, 0), wxGBSpan(1, 1), wxALIGN_CENTER_VERTICAL | wxALL, 5);
    datum_ctrl_ = new wxTextCtrl(this, wxID_ANY, ""); // Placeholder
    sizer->Add(datum_ctrl_, wxGBPosition(1, 1), wxGBSpan(1, 1), wxEXPAND);
    sizer->Add(new wxStaticText(this, wxID_ANY, "Zeit:"), wxGBPosition(1, 2), wxGBSpan(1, 1), wxALIGN_CENTER_VERTICAL | wxALL, 5);
    zeit_ctrl_ = new wxTextCtrl(this, wxID_ANY, ""); // Placeholder
    sizer->Add(zeit_ctrl_, wxGBPosition(1, 3), wxGBSpan(1, 1), wxEXPAND);

    // Zeile 2: Ort
    sizer->Add(new wxStaticText(this, wxID_ANY, "Geburtsort:"), wxGBPosition(2, 0), wxGBSpan(1, 1), wxALIGN_CENTER_VERTICAL | wxALL, 5);
    ort_ctrl_ = new wxTextCtrl(this, wxID_ANY, radix_data_.o.szOrt);
    sizer->Add(ort_ctrl_, wxGBPosition(2, 1), wxGBSpan(1, 3), wxEXPAND);
    wxButton* search_ort_button = new wxButton(this, wxID_ANY, "Suchen...");
    sizer->Add(search_ort_button, wxGBPosition(2, 4), wxGBSpan(1, 1), wxEXPAND);

    // Zeile 3: Zeitzone, Sommerzeit
    sizer->Add(new wxStaticText(this, wxID_ANY, "Zeitzone:"), wxGBPosition(3, 0), wxGBSpan(1, 1), wxALIGN_CENTER_VERTICAL | wxALL, 5);
    zeitzone_ctrl_ = new wxTextCtrl(this, wxID_ANY, ""); // Placeholder
    sizer->Add(zeitzone_ctrl_, wxGBPosition(3, 1), wxGBSpan(1, 1), wxEXPAND);
    sizer->Add(new wxStaticText(this, wxID_ANY, "Sommerzeit:"), wxGBPosition(3, 2), wxGBSpan(1, 1), wxALIGN_CENTER_VERTICAL | wxALL, 5);
    sommer_ctrl_ = new wxTextCtrl(this, wxID_ANY, ""); // Placeholder
    sizer->Add(sommer_ctrl_, wxGBPosition(3, 3), wxGBSpan(1, 1), wxEXPAND);

    // Zeile 4: Beruf
    sizer->Add(new wxStaticText(this, wxID_ANY, "Beruf:"), wxGBPosition(4, 0), wxGBSpan(1, 1), wxALIGN_CENTER_VERTICAL | wxALL, 5);
    beruf_ctrl_ = new wxTextCtrl(this, wxID_ANY, ""); // Placeholder
    sizer->Add(beruf_ctrl_, wxGBPosition(4, 1), wxGBSpan(1, 3), wxEXPAND);

    // Zeile 5: Buttons
    wxBoxSizer* button_sizer = new wxBoxSizer(wxHORIZONTAL);
    button_sizer->Add(new wxButton(this, wxID_OK, "OK"), 0, wxALL, 5);
    button_sizer->Add(new wxButton(this, wxID_ANY, "Berechnen"), 0, wxALL, 5);
    button_sizer->Add(new wxButton(this, wxID_CANCEL, "Abbrechen"), 0, wxALL, 5);
    sizer->Add(button_sizer, wxGBPosition(6, 0), wxGBSpan(1, 5), wxALIGN_CENTER);

    this->SetSizerAndFit(sizer);
    Centre();

    // Event-Handler
    Bind(wxEVT_BUTTON, &PersonDialog::OnOk, this, wxID_OK);
    Bind(wxEVT_BUTTON, &PersonDialog::OnSearchName, this, search_name_button->GetId());
    Bind(wxEVT_BUTTON, &PersonDialog::OnSearchOrt, this, search_ort_button->GetId());
}

void PersonDialog::OnSearchName(wxCommandEvent& event) {
    // Lade alle Personen (vereinfacht, sollte gecacht werden)
    astro::LegacyDataManager dm;
    std::vector<astro::LegacyRadix> all_persons;
    // Annahme: Die Datei liegt im selben Verzeichnis
    if (dm.ReadAllPersons("astronam.dat", all_persons)) {
        PersonSearchDialog dlg(this, all_persons);
        if (dlg.ShowModal() == wxID_OK) {
            int selected_index = dlg.GetSelectedIndex();
            if (selected_index != -1) {
                radix_data_ = all_persons[selected_index];
                // UI vollständig aktualisieren
                vorname_ctrl_->SetValue(radix_data_.szVorName);
                name_ctrl_->SetValue(radix_data_.szName);
                ort_ctrl_->SetValue(radix_data_.o.szOrt);

                // Datum und Zeit formatieren und setzen
                datum_ctrl_->SetValue(wxString::Format("%02d.%02d.%04d", radix_data_.o.dt.iTag, radix_data_.o.dt.iMon, radix_data_.o.dt.iJah));
                zeit_ctrl_->SetValue(wxString::Format("%02d:%02d", radix_data_.o.dt.iStu, radix_data_.o.dt.iMin));

                // Zeitzone und Sommerzeit setzen
                zeitzone_ctrl_->SetValue(wxString::Format("%d", radix_data_.o.dt.iZone));
                sommer_ctrl_->SetValue(wxString::Format("%d", radix_data_.o.dt.iSom));

                // Beruf (falls vorhanden, in LegacyRadix nicht standardmäßig)
                // beruf_ctrl_->SetValue(...);
            }
        }
    } else {
        wxMessageBox("Fehler beim Lesen der Personendaten!", "Fehler", wxOK | wxICON_ERROR);
    }
}

void PersonDialog::OnSearchOrt(wxCommandEvent& event) {
    wxMessageBox("Orte-Suche (noch nicht implementiert)", "Platzhalter");
}

void PersonDialog::OnOk(wxCommandEvent& event) {
    // Daten aus den UI-Elementen in radix_data_ speichern
    strncpy(radix_data_.szName, name_ctrl_->GetValue().ToStdString().c_str(), 30);
    strncpy(radix_data_.szVorName, vorname_ctrl_->GetValue().ToStdString().c_str(), 30);
    strncpy(radix_data_.o.szOrt, ort_ctrl_->GetValue().ToStdString().c_str(), 30);
    // Datum und Zeit parsen
    long tag, mon, jah, stu, min;
    wxString datum = datum_ctrl_->GetValue();
    wxString zeit = zeit_ctrl_->GetValue();
    if (sscanf(datum.c_str(), "%ld.%ld.%ld", &tag, &mon, &jah) == 3) {
        radix_data_.o.dt.iTag = tag;
        radix_data_.o.dt.iMon = mon;
        radix_data_.o.dt.iJah = jah;
    }
    if (sscanf(zeit.c_str(), "%ld:%ld", &stu, &min) == 2) {
        radix_data_.o.dt.iStu = stu;
        radix_data_.o.dt.iMin = min;
    }

    // Zeitzone und Sommerzeit parsen
    long zone, sommer;
    zeitzone_ctrl_->GetValue().ToLong(&zone);
    sommer_ctrl_->GetValue().ToLong(&sommer);
    radix_data_.o.dt.iZone = zone;
    radix_data_.o.dt.iSom = sommer;

    EndModal(wxID_OK);
}
