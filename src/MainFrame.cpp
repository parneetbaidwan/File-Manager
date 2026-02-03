#include "MainFrame.h"
#include <wx/msgdlg.h>

wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
    EVT_TEXT_ENTER(wxID_ANY, MainFrame::OnPathEnter)
    EVT_LIST_ITEM_ACTIVATED(wxID_ANY, MainFrame::OnItemActivated)
wxEND_EVENT_TABLE()

static wxString ToWx(const fs::path& p)
{
    return wxString::FromUTF8(p.u8string());
}

MainFrame::MainFrame(const wxString& title)
    : wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxSize(900, 600))
{
    BuildUi();
    CreateStatusBar(1);
    SetDirectory(fs::current_path());
}

void MainFrame::BuildUi()
{
    auto* panel = new wxPanel(this);

    m_pathCtrl = new wxTextCtrl(panel, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);

    m_listCtrl = new wxListCtrl(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxLC_SINGLE_SEL);
    m_listCtrl->InsertColumn(0, "Name", wxLIST_FORMAT_LEFT, 420);
    m_listCtrl->InsertColumn(1, "Type", wxLIST_FORMAT_LEFT, 90);
    m_listCtrl->InsertColumn(2, "Size", wxLIST_FORMAT_RIGHT, 120);

    auto* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(m_pathCtrl, 0, wxEXPAND | wxALL, 8);
    sizer->Add(m_listCtrl, 1, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 8);
    panel->SetSizer(sizer);
}

void MainFrame::SetDirectory(const fs::path& dir)
{
    std::error_code ec;
    if (!fs::exists(dir, ec) || !fs::is_directory(dir, ec))
    {
        wxMessageBox("Not a directory:\n" + ToWx(dir), "Invalid Path", wxOK | wxICON_ERROR, this);
        return;
    }

    m_currentDir = fs::canonical(dir);
    m_pathCtrl->SetValue(ToWx(m_currentDir));
    RefreshListing();
}

void MainFrame::RefreshListing()
{
    m_listCtrl->DeleteAllItems();

    if (m_currentDir.has_parent_path() && m_currentDir != m_currentDir.root_path())
    {
        long idx = m_listCtrl->InsertItem(m_listCtrl->GetItemCount(), "..");
        m_listCtrl->SetItem(idx, 1, "DIR");
        m_listCtrl->SetItem(idx, 2, "");
    }

    try
    {
        for (const auto& entry : fs::directory_iterator(m_currentDir))
        {
            const fs::path p = entry.path();
            const bool isDir = entry.is_directory();

            long idx = m_listCtrl->InsertItem(m_listCtrl->GetItemCount(), ToWx(p.filename()));
            m_listCtrl->SetItem(idx, 1, isDir ? "DIR" : "FILE");

            if (!isDir)
            {
                std::error_code sec;
                auto sz = fs::file_size(p, sec);
                if (!sec)
                    m_listCtrl->SetItem(idx, 2, wxString::Format("%llu", (unsigned long long)sz));
            }
        }

        SetStatusText("Ready");
    }
    catch (const std::exception& e)
    {
        wxMessageBox(wxString("Listing failed:\n") + e.what(), "Error", wxOK | wxICON_ERROR, this);
    }
}

std::optional<fs::path> MainFrame::GetSelectedPath() const
{
    long sel = m_listCtrl->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    if (sel == -1) return std::nullopt;

    wxString name = m_listCtrl->GetItemText(sel);
    if (name == "..") return m_currentDir.parent_path();
    return m_currentDir / name.ToStdString();
}

void MainFrame::OnPathEnter(wxCommandEvent&)
{
    SetDirectory(m_pathCtrl->GetValue().ToStdString());
}

void MainFrame::OnItemActivated(wxListEvent&)
{
    auto sp = GetSelectedPath();
    if (!sp) return;

    const fs::path target = *sp;
    std::error_code ec;

    if (fs::is_directory(target, ec))
    {
        SetDirectory(target);
        return;
    }

    if (!wxLaunchDefaultApplication(ToWx(target)))
    {
        wxMessageBox("Could not open file.", "Open Failed", wxOK | wxICON_ERROR, this);
    }
}
