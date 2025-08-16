#include "AspectGridWindow.hpp"
#include <cstdio>

wxBEGIN_EVENT_TABLE(AspectGridWindow, wxFrame)
    EVT_CLOSE(AspectGridWindow::OnClose)
wxEND_EVENT_TABLE()

AspectGridWindow::AspectGridWindow(wxWindow* parent, const wxString& title)
    : wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxSize(500, 300))
{
    std::fprintf(stderr, "[AspectGridWindow] ctor\n");
    // Create a sizer for the window
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    
    // Create the aspect grid
    m_aspectGrid = new AspectGrid(this);
    
    // Add the aspect grid to the sizer
    sizer->Add(m_aspectGrid, 1, wxEXPAND | wxALL, 5);
    
    // Set the sizer
    SetSizer(sizer);
    
    // Center the window (on screen since this is top-level)
    CentreOnScreen();
}

void AspectGridWindow::ShowWindow(bool show)
{
    std::fprintf(stderr, "[AspectGridWindow] ShowWindow(%d)\n", show ? 1 : 0);
    if (show) {
        Show();
        Raise();
    } else {
        Hide();
    }
    if (m_aspectGrid) {
        m_aspectGrid->SetVisible(show);
    }
}

void AspectGridWindow::OnClose(wxCloseEvent& event)
{
    // Hide the window instead of closing it
    Hide();
}
