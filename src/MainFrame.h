#pragma once
#include <wx/wx.h>
#include <wx/listctrl.h>
#include <filesystem>
#include <optional>

#include "FileSystemService.h"

namespace fs = std::filesystem;

/*
 * MainFrame (wxWidgets "Hello World" style)
 * - In wxWidgets docs, the main window is a wxFrame subclass
 * - Event routing uses an event table (EVT_... macros)
 *
 * Responsibilities:
 * - UI layout (path bar + list + status bar)
 * - Menus + keyboard shortcuts
 * - Dispatch user actions -> FileSystemService
 */
class MainFrame final : public wxFrame
{
public:
    explicit MainFrame(const wxString& title);

private:
    // ---- UI ----
    wxTextCtrl* m_pathCtrl = nullptr;
    wxListCtrl* m_listCtrl = nullptr;

    // ---- State ----
    fs::path m_currentDir;
    VirtualClipboard m_clip;
    FileSystemService m_fs;

    // ---- Menu / control IDs ----
    enum
    {
        ID_Path = wxID_HIGHEST + 1,
        ID_List,

        ID_New,
        ID_Open,
        ID_Rename,
        ID_Delete,

        ID_Copy,
        ID_Cut,
        ID_Paste,

        ID_Refresh,
        ID_Exit
    };

    // ---- Setup helpers ----
    void BuildUi();
    void BuildMenus();
    void BuildAccelerators();

    // ---- Core behavior ----
    void SetDirectory(const fs::path& dir);
    void RefreshListing();
    std::optional<fs::path> GetSelectedPath() const;

    // ---- Operations ----
    void DoNew();
    void DoOpen();
    void DoRename();
    void DoDelete();
    void DoCopy(bool cut);
    void DoPaste();
    void DoRefresh();

    // ---- Event handlers ----
    void OnPathEnter(wxCommandEvent& event);
    void OnItemActivated(wxListEvent& event);

    void OnMenuNew(wxCommandEvent& event);
    void OnMenuOpen(wxCommandEvent& event);
    void OnMenuRename(wxCommandEvent& event);
    void OnMenuDelete(wxCommandEvent& event);

    void OnMenuCopy(wxCommandEvent& event);
    void OnMenuCut(wxCommandEvent& event);
    void OnMenuPaste(wxCommandEvent& event);

    void OnMenuRefresh(wxCommandEvent& event);
    void OnMenuExit(wxCommandEvent& event);

    // ---- UI utilities ----
    static wxString ToWx(const fs::path& p);
    static wxString FormatLongDate(std::time_t t);
    void ShowError(const wxString& title, const wxString& msg);

    wxDECLARE_EVENT_TABLE();
};
