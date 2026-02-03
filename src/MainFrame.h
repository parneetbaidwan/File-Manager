#pragma once

#include <wx/wx.h>
#include <wx/listctrl.h>
#include <filesystem>
#include <optional>

namespace fs = std::filesystem;

class MainFrame final : public wxFrame
{
public:
    explicit MainFrame(const wxString& title);

private:
    wxTextCtrl* m_pathCtrl = nullptr;
    wxListCtrl* m_listCtrl = nullptr;
    fs::path m_currentDir;

    void BuildUi();
    void SetDirectory(const fs::path& dir);
    void RefreshListing();

    std::optional<fs::path> GetSelectedPath() const;

    void OnPathEnter(wxCommandEvent& evt);
    void OnItemActivated(wxListEvent& evt);

    wxDECLARE_EVENT_TABLE();
};
