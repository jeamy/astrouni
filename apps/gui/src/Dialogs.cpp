#include "Dialogs.hpp"
#include <wx/valnum.h>
#include <sstream>

DateTimeDialog::DateTimeDialog(wxWindow* parent,
                               const astrocore::Date& date,
                               const astrocore::Time& time,
                               const astrocore::TimeZone& tz)
: wxDialog(parent, wxID_ANY, "Set Date/Time/Zone", wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
  auto* topsizer = new wxBoxSizer(wxVERTICAL);

  auto* grid = new wxFlexGridSizer(0, 2, 5, 8);
  grid->AddGrowableCol(1, 1);

  // Year
  grid->Add(new wxStaticText(this, wxID_ANY, "Year:"), 0, wxALIGN_CENTER_VERTICAL);
  m_year = new wxSpinCtrl(this, wxID_ANY);
  m_year->SetRange(1, 9999);
  m_year->SetValue(date.year);
  grid->Add(m_year, 1, wxEXPAND);

  // Month
  grid->Add(new wxStaticText(this, wxID_ANY, "Month:"), 0, wxALIGN_CENTER_VERTICAL);
  m_month = new wxSpinCtrl(this, wxID_ANY);
  m_month->SetRange(1, 12);
  m_month->SetValue(date.month);
  grid->Add(m_month, 1, wxEXPAND);

  // Day
  grid->Add(new wxStaticText(this, wxID_ANY, "Day:"), 0, wxALIGN_CENTER_VERTICAL);
  m_day = new wxSpinCtrl(this, wxID_ANY);
  m_day->SetRange(1, 31);
  m_day->SetValue(date.day);
  grid->Add(m_day, 1, wxEXPAND);

  // Hour
  grid->Add(new wxStaticText(this, wxID_ANY, "Hour:"), 0, wxALIGN_CENTER_VERTICAL);
  m_hour = new wxSpinCtrl(this, wxID_ANY);
  m_hour->SetRange(0, 23);
  m_hour->SetValue(time.hour);
  grid->Add(m_hour, 1, wxEXPAND);

  // Minute
  grid->Add(new wxStaticText(this, wxID_ANY, "Minute:"), 0, wxALIGN_CENTER_VERTICAL);
  m_min = new wxSpinCtrl(this, wxID_ANY);
  m_min->SetRange(0, 59);
  m_min->SetValue(time.minute);
  grid->Add(m_min, 1, wxEXPAND);

  // Seconds
  grid->Add(new wxStaticText(this, wxID_ANY, "Seconds:"), 0, wxALIGN_CENTER_VERTICAL);
  m_sec = new wxTextCtrl(this, wxID_ANY, wxString::Format("%.3f", time.second));
  grid->Add(m_sec, 1, wxEXPAND);

  // UTC offset
  grid->Add(new wxStaticText(this, wxID_ANY, "UTC offset (hours):"), 0, wxALIGN_CENTER_VERTICAL);
  m_utcOffset = new wxTextCtrl(this, wxID_ANY, wxString::Format("%.2f", tz.utcOffsetHours));
  grid->Add(m_utcOffset, 1, wxEXPAND);

  // DST
  grid->Add(new wxStaticText(this, wxID_ANY, "DST (hours):"), 0, wxALIGN_CENTER_VERTICAL);
  m_dst = new wxTextCtrl(this, wxID_ANY, wxString::Format("%.2f", tz.dstHours));
  grid->Add(m_dst, 1, wxEXPAND);

  topsizer->Add(grid, 1, wxALL | wxEXPAND, 10);

  auto* btns = CreateStdDialogButtonSizer(wxOK | wxCANCEL);
  topsizer->Add(btns, 0, wxALL | wxALIGN_RIGHT, 10);

  SetSizerAndFit(topsizer);
}

bool DateTimeDialog::GetResult(astrocore::Date& date,
                               astrocore::Time& time,
                               astrocore::TimeZone& tz) const {
  date.year = m_year->GetValue();
  date.month = m_month->GetValue();
  date.day = m_day->GetValue();
  time.hour = m_hour->GetValue();
  time.minute = m_min->GetValue();

  try {
    time.second = std::stod(m_sec->GetValue().ToStdString());
  } catch (...) { return false; }

  try {
    tz.utcOffsetHours = std::stod(m_utcOffset->GetValue().ToStdString());
    tz.dstHours = std::stod(m_dst->GetValue().ToStdString());
  } catch (...) { return false; }

  // Simple validation
  if (time.second < 0.0 || time.second >= 60.0) return false;
  if (tz.utcOffsetHours < -14.0 || tz.utcOffsetHours > 14.0) return false;
  if (tz.dstHours < -2.0 || tz.dstHours > 2.0) return false;
  if (date.month < 1 || date.month > 12) return false;
  if (date.day < 1 || date.day > 31) return false;

  return true;
}

LocationDialog::LocationDialog(wxWindow* parent, double lonDeg, double latDeg)
: wxDialog(parent, wxID_ANY, "Set Location", wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
  auto* topsizer = new wxBoxSizer(wxVERTICAL);
  auto* grid = new wxFlexGridSizer(0, 2, 5, 8);
  grid->AddGrowableCol(1, 1);

  grid->Add(new wxStaticText(this, wxID_ANY, "Longitude (deg, east+):"), 0, wxALIGN_CENTER_VERTICAL);
  m_lon = new wxTextCtrl(this, wxID_ANY, wxString::Format("%.6f", lonDeg));
  grid->Add(m_lon, 1, wxEXPAND);

  grid->Add(new wxStaticText(this, wxID_ANY, "Latitude (deg, north+):"), 0, wxALIGN_CENTER_VERTICAL);
  m_lat = new wxTextCtrl(this, wxID_ANY, wxString::Format("%.6f", latDeg));
  grid->Add(m_lat, 1, wxEXPAND);

  topsizer->Add(grid, 1, wxALL | wxEXPAND, 10);

  auto* btns = CreateStdDialogButtonSizer(wxOK | wxCANCEL);
  topsizer->Add(btns, 0, wxALL | wxALIGN_RIGHT, 10);

  SetSizerAndFit(topsizer);
}

bool LocationDialog::GetResult(double& lonDeg, double& latDeg) const {
  try {
    lonDeg = std::stod(m_lon->GetValue().ToStdString());
    latDeg = std::stod(m_lat->GetValue().ToStdString());
  } catch (...) { return false; }

  if (lonDeg < -180.0 || lonDeg > 180.0) return false;
  if (latDeg < -90.0 || latDeg > 90.0) return false;
  return true;
}
