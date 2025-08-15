#include "AspectGridWindow.hpp"

wxBEGIN_EVENT_TABLE(AspectGridWindow, wxFrame)
    EVT_CLOSE(AspectGridWindow::OnClose)
wxEND_EVENT_TABLE()

AspectGridWindow::AspectGridWindow(wxWindow* parent, const wxString& title)
    : wxFrame(parent, wxID_ANY, title, wxDefaultPosition, wxSize(500, 300))
{
    // Create a sizer for the window
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    
    // Create the aspect grid
    m_aspectGrid = new AspectGrid(this);
    
    // Add the aspect grid to the sizer
    sizer->Add(m_aspectGrid, 1, wxEXPAND | wxALL, 5);
    
    // Set the sizer
    SetSizer(sizer);
    
    // Center the window
    CenterOnParent();
}

void AspectGridWindow::ShowWindow(bool show)
{
    if (show) {
        Show();
        Raise();
    } else {
        Hide();
    }
}

void AspectGridWindow::OnClose(wxCloseEvent& event)
{
    // Hide the window instead of closing it
    Hide();
}
