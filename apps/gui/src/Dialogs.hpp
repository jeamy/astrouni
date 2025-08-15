#pragma once

#include <wx/wx.h>
#include <wx/spinctrl.h>
#include "astrocore/api.hpp"

class DateTimeDialog : public wxDialog {
public:
  DateTimeDialog(wxWindow* parent,
                 const astrocore::Date& date,
                 const astrocore::Time& time,
                 const astrocore::TimeZone& tz);

  bool GetResult(astrocore::Date& date,
                 astrocore::Time& time,
                 astrocore::TimeZone& tz) const;

private:
  wxSpinCtrl* m_year{nullptr};
  wxSpinCtrl* m_month{nullptr};
  wxSpinCtrl* m_day{nullptr};
  wxSpinCtrl* m_hour{nullptr};
  wxSpinCtrl* m_min{nullptr};
  wxTextCtrl* m_sec{nullptr};
  wxTextCtrl* m_utcOffset{nullptr};
  wxTextCtrl* m_dst{nullptr};
};

class LocationDialog : public wxDialog {
public:
  LocationDialog(wxWindow* parent, double lonDeg, double latDeg);

  bool GetResult(double& lonDeg, double& latDeg) const;

private:
  wxTextCtrl* m_lon{nullptr};
  wxTextCtrl* m_lat{nullptr};
};
