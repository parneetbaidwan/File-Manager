/*
Parneet Baidwan - 251259638
Description: The MainFrame class declaration in this header file functions as the main window for the file manager application. The document establishes user interface elements which include the file list view and directory path text field and menus and status bar and the event handler methods which handle user actions and file handling activities.
February 1, 2026
*/


#ifndef MAINFRAME_H
#define MAINFRAME_H

#include <wx/wx.h>
#include <wx/listctrl.h>
#include <filesystem>
#include <optional>

#include "FileSystemService.h"




namespace fs = std::filesystem;



class MainFrame final : public wxFrame
{
public:
    explicit MainFrame(const wxString& title);

private:
    wxTextCtrl* m_pathCtrl = nullptr;
    wxListCtrl* m_listCtrl = nullptr;

    fs::path m_currentDir;
    VirtualClipboard m_clip;
    FileSystemService m_fs;

    // menu and control ids
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

    void BuildUi();
    void BuildMenus();
    void BuildAccelerators();

    void SetDirectory(const fs::path& dir);
    void RefreshListing();
    std::optional<fs::path> GetSelectedPath() const;

    void DoNew();
    void DoOpen();
    void DoRename();
    void DoDelete();
    void DoCopy(bool cut);
    void DoPaste();
    void DoRefresh();

    // event handlers
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

    // ui utilities
    static wxString ToWx(const fs::path& p);
    static wxString FormatLongDate(std::time_t t);
    void ShowError(const wxString& title, const wxString& msg);

    wxDECLARE_EVENT_TABLE();
};


#endif // MAINFRAME_H

