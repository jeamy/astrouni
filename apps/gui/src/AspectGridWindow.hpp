#pragma once

#include <wx/wx.h>
#include "AspectGrid.hpp"

class AspectGridWindow : public wxFrame {
public:
    AspectGridWindow(wxWindow* parent, const wxString& title);
    virtual ~AspectGridWindow() = default;
    
    // Get the aspect grid
    AspectGrid* GetAspectGrid() { return m_aspectGrid; }
    
    // Show or hide the window
    void ShowWindow(bool show);
    
private:
    AspectGrid* m_aspectGrid;
    
    void OnClose(wxCloseEvent& event);
    
    wxDECLARE_EVENT_TABLE();
};
