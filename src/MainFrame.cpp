/*
Parneet Baidwan - 251259638
Description: The MainFrame class in this file develops its graphical user interface through wxWidgets and creates event handlers which respond to user activities. The system connects user commands with filesystem tasks while the system shows changes to the user interface.

February 1, 2026
*/


#include "MainFrame.h"
#include "FileSystemService.h"

#include <wx/textdlg.h>
#include <wx/msgdlg.h>
#include <vector>

// bind event ids to handler methods
wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
    // Controls
    EVT_TEXT_ENTER(MainFrame::ID_Path, MainFrame::OnPathEnter)
    EVT_LIST_ITEM_ACTIVATED(MainFrame::ID_List, MainFrame::OnItemActivated)

    // Menus
    EVT_MENU(MainFrame::ID_New,     MainFrame::OnMenuNew)
    EVT_MENU(MainFrame::ID_Open,    MainFrame::OnMenuOpen)
    EVT_MENU(MainFrame::ID_Rename,  MainFrame::OnMenuRename)
    EVT_MENU(MainFrame::ID_Delete,  MainFrame::OnMenuDelete)

    EVT_MENU(MainFrame::ID_Copy,    MainFrame::OnMenuCopy)
    EVT_MENU(MainFrame::ID_Cut,     MainFrame::OnMenuCut)
    EVT_MENU(MainFrame::ID_Paste,   MainFrame::OnMenuPaste)

    EVT_MENU(MainFrame::ID_Refresh, MainFrame::OnMenuRefresh)
    EVT_MENU(MainFrame::ID_Exit,    MainFrame::OnMenuExit)
wxEND_EVENT_TABLE()

/*
Function: MainFrame::ToWx
Description: Converts a std::filesystem::path into a wxString suitable for display in the UI.
             This is used to keep filesystem paths and wxWidgets strings consistent.
Parameters:
  - p: Filesystem path to convert.
Returns:
  - wxString: Converted path string for wxWidgets UI elements.
*/
wxString MainFrame::ToWx(const fs::path& p)
{
    return wxString::FromUTF8(p.u8string());
}

/*
Function: MainFrame::FormatLongDate
Description: Converts a time_t timestamp into a human-readable string for the “Date Modified”
             column. If formatting fails, a fallback string may be produced.
Parameters:
  - t: Timestamp (typically last_write_time converted to time_t).
Returns:
  - wxString: Formatted date/time string for display.
*/
wxString MainFrame::FormatLongDate(std::time_t t)
{
    if (t <= 0) return "";

    wxDateTime dt((time_t)t);
    if (!dt.IsValid()) return "";

    return dt.Format("%A, %B %d, %Y %H:%M:%S");
}

/*
Function: MainFrame::ShowError
Description: Displays an error dialog to the user. Centralizing this logic ensures consistent
             error messaging for filesystem failures and invalid operations.
Parameters:
  - title: Dialog title.
  - msg: Error message content to display.
Returns:
  - None
*/
void MainFrame::ShowError(const wxString& title, const wxString& msg)
{
    wxMessageBox(msg, title, wxOK | wxICON_ERROR, this);
}

// constructor
MainFrame::MainFrame(const wxString& title)
    : wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxSize(980, 620))
{
    // layout path bar and file list
    BuildUi();

    // menu bar
    BuildMenus();

    // keyboard shortcuts
    BuildAccelerators();

    CreateStatusBar(1);
    SetStatusText("Welcome to wxWidgets File Manager!");

    // start from current working directory
    SetDirectory(fs::current_path());
}

/*
Function: MainFrame::BuildUi
Description: Constructs the main window UI: directory path bar, file list control, layout,
             and status bar. This is called once during frame creation.
Parameters:
  - None
Returns:
  - None
*/
void MainFrame::BuildUi()
{
    auto* panel = new wxPanel(this);

    // oath bar
    m_pathCtrl = new wxTextCtrl(
        panel,
        ID_Path,
        "",
        wxDefaultPosition,
        wxDefaultSize,
        wxTE_PROCESS_ENTER
    );

    // list
    m_listCtrl = new wxListCtrl(
        panel,
        ID_List,
        wxDefaultPosition,
        wxDefaultSize,
        wxLC_REPORT | wxLC_SINGLE_SEL
    );

    // columns: Name, Type, Size, Date
    m_listCtrl->InsertColumn(0, "Name", wxLIST_FORMAT_LEFT, 380);
    m_listCtrl->InsertColumn(1, "Type", wxLIST_FORMAT_LEFT, 100);
    m_listCtrl->InsertColumn(2, "Size", wxLIST_FORMAT_RIGHT, 130);
    m_listCtrl->InsertColumn(3, "Date", wxLIST_FORMAT_LEFT, 320);

    auto* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(m_pathCtrl, 0, wxEXPAND | wxALL, 10);
    sizer->Add(m_listCtrl, 1, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 10);
    panel->SetSizer(sizer);
}

/*
Function: MainFrame::BuildMenus
Description: Creates the menu bar and all menu items required for file operations (open, new
             directory, rename, delete, copy/cut/paste, refresh, exit). Menu IDs are bound
             to event handlers via the event table.
Parameters:
  - None
Returns:
  - None
*/
void MainFrame::BuildMenus()
{

    auto* fileMenu = new wxMenu;
    fileMenu->Append(ID_New,    "New...\tCtrl+N");
    fileMenu->Append(ID_Open,   "Open...\tCtrl+O");
    fileMenu->AppendSeparator();
    fileMenu->Append(ID_Rename, "Rename...\tCtrl+E");
    fileMenu->Append(ID_Delete, "Delete...\tDel");
    fileMenu->AppendSeparator();
    fileMenu->Append(ID_Exit,   "Exit\tCtrl+Q");

    auto* editMenu = new wxMenu;
    editMenu->Append(ID_Copy,  "Copy\tCtrl+C");
    editMenu->Append(ID_Cut,   "Cut\tCtrl+X");
    editMenu->Append(ID_Paste, "Paste\tCtrl+V");

    auto* viewMenu = new wxMenu;
    viewMenu->Append(ID_Refresh, "Refresh\tF5");

    auto* menuBar = new wxMenuBar;
    menuBar->Append(fileMenu, "&File");
    menuBar->Append(editMenu, "&Edit");
    menuBar->Append(viewMenu, "&View");

    SetMenuBar(menuBar);
}

/*
Function: MainFrame::BuildAccelerators
Description: Sets up keyboard shortcuts (accelerators) for menu operations (e.g., Ctrl+C,
             Ctrl+X, Ctrl+V, F5, Ctrl+Q). This allows operations without mouse interaction.
Parameters:
  - None
Returns:
  - None
*/
void MainFrame::BuildAccelerators()
{
    std::vector<wxAcceleratorEntry> entries;

    entries.emplace_back(wxACCEL_CTRL, (int)'N', ID_New);
    entries.emplace_back(wxACCEL_CTRL, (int)'O', ID_Open);
    entries.emplace_back(wxACCEL_CTRL, (int)'E', ID_Rename);
    entries.emplace_back(wxACCEL_CTRL, (int)'Q', ID_Exit);

    entries.emplace_back(wxACCEL_CTRL, (int)'C', ID_Copy);
    entries.emplace_back(wxACCEL_CTRL, (int)'X', ID_Cut);
    entries.emplace_back(wxACCEL_CTRL, (int)'V', ID_Paste);

    entries.emplace_back(0, WXK_F5, ID_Refresh);
    entries.emplace_back(0, WXK_DELETE, ID_Delete);

    wxAcceleratorTable table((int)entries.size(), &entries[0]);
    SetAcceleratorTable(table);
}

/*
Function: MainFrame::SetDirectory
Description: Attempts to change the active directory being displayed. Updates the path bar
             and refreshes the listing. If the directory is invalid, an error dialog may be
             shown and the previous directory remains active.
Parameters:
  - dir: Target directory path to switch to.
Returns:
  - None
*/
void MainFrame::SetDirectory(const fs::path& dir)
{
    if (!FileSystemService::Exists(dir) || !FileSystemService::IsDirectory(dir))
    {
        ShowError("Invalid Path", "Not a directory:\n" + ToWx(dir));
        return;
    }

    m_currentDir = FileSystemService::CanonicalOrSame(dir);
    m_pathCtrl->SetValue(ToWx(m_currentDir));
    RefreshListing();
}

/*
Function: MainFrame::RefreshListing
Description: Rebuilds the file list display for the current directory. Uses FileSystemService
             to retrieve file metadata, clears the list control, optionally inserts a ".."
             entry for parent navigation, and populates rows with name/type/size/date.
             Failures (e.g., permission errors) are surfaced to the user.
Parameters:
  - None
Returns:
  - None
*/
void MainFrame::RefreshListing()
{
    m_listCtrl->DeleteAllItems();

    if (m_currentDir.has_parent_path() && m_currentDir != m_currentDir.root_path())
    {
        const long idx = m_listCtrl->InsertItem(m_listCtrl->GetItemCount(), "..");
        m_listCtrl->SetItem(idx, 1, "Dir");
        m_listCtrl->SetItem(idx, 2, "0");
        m_listCtrl->SetItem(idx, 3, "");
    }

    std::string err;
    const auto items = m_fs.ListDirectory(m_currentDir, err);
    if (!err.empty())
    {
        ShowError("Listing Error", wxString::FromUTF8(err));
        return;
    }

    // populate rows
    for (const auto& item : items)
    {
        const wxString name = ToWx(item.fullPath.filename());

        const long idx = m_listCtrl->InsertItem(m_listCtrl->GetItemCount(), name);
        m_listCtrl->SetItem(idx, 1, item.isDir ? "Dir" : "File");
        m_listCtrl->SetItem(idx, 2, item.isDir ? "0" : wxString::Format("%llu", (unsigned long long)item.sizeBytes));
        m_listCtrl->SetItem(idx, 3, FormatLongDate(item.modified));
    }
}

/*
Function: MainFrame::GetSelectedPath
Description: Retrieves the filesystem path for the currently selected list item. Handles the
             special ".." entry by returning the parent directory path. Returns empty if no
             selection exists.
Parameters:
  - None
Returns:
  - std::optional<fs::path>: Selected item path, or empty if nothing is selected.
*/
std::optional<fs::path> MainFrame::GetSelectedPath() const
{
    const long sel = m_listCtrl->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    if (sel == -1) return std::nullopt;

    const wxString name = m_listCtrl->GetItemText(sel);
    if (name == "..")
        return m_currentDir.parent_path();

    return m_currentDir / fs::path(name.ToStdString());
}

/*
Function: MainFrame::DoNew
Description: Creates a new directory in the current directory. Prompts the user for a folder
             name, validates input, calls FileSystemService to create it, and refreshes the
             listing on success. Displays an error dialog if creation fails.
Parameters:
  - None
Returns:
  - None
*/
void MainFrame::DoNew()
{
    wxTextEntryDialog dlg(this, "Enter directory name:", "New Directory");
    if (dlg.ShowModal() != wxID_OK) return;

    const wxString nameWx = dlg.GetValue().Trim(true).Trim(false);
    if (nameWx.empty()) return;

    std::string err;
    if (!m_fs.CreateDirectory(m_currentDir, nameWx.ToStdString(), err))
    {
        ShowError("New Directory", wxString::FromUTF8(err));
        return;
    }

    SetStatusText("Created directory: " + nameWx);
    RefreshListing();
}

/*
Function: MainFrame::DoOpen
Description: Opens the selected item. If a directory is selected, navigates into it by calling
             SetDirectory. If a file is selected, launches it using the system default
             application through wxWidgets. Displays an informational dialog if nothing is
             selected.
Parameters:
  - None
Returns:
  - None
*/
void MainFrame::DoOpen()
{
    const auto sp = GetSelectedPath();
    if (!sp)
    {
        wxMessageBox("Select a file or directory first.", "Open", wxOK | wxICON_INFORMATION, this);
        return;
    }

    const fs::path target = *sp;

    // go into the directory or laungh the file
    if (FileSystemService::IsDirectory(target))
    {
        SetDirectory(target);
        return;
    }

    if (!wxLaunchDefaultApplication(ToWx(target)))
    {
        ShowError("Open Failed", "Could not open file with the default application.");
    }
}

/*
Function: MainFrame::DoRename
Description: Renames the selected file or directory. Prompts the user for a new name, then
             uses FileSystemService to perform the rename. Refreshes the listing on success
             and displays errors for invalid selection, invalid name, or filesystem failures.
Parameters:
  - None
Returns:
  - None
*/
void MainFrame::DoRename()
{
    const auto sp = GetSelectedPath();
    if (!sp)
    {
        wxMessageBox("Select a file or directory first.", "Rename", wxOK | wxICON_INFORMATION, this);
        return;
    }

    const fs::path src = *sp;

    wxTextEntryDialog dlg(this, "Enter new name:", "Rename", ToWx(src.filename()));
    if (dlg.ShowModal() != wxID_OK) return;

    const wxString newNameWx = dlg.GetValue().Trim(true).Trim(false);
    if (newNameWx.empty()) return;

    std::string err;
    if (!m_fs.RenamePath(src, newNameWx.ToStdString(), err))
    {
        ShowError("Rename", wxString::FromUTF8(err));
        return;
    }

    SetStatusText("Renamed: " + newNameWx);
    RefreshListing();
}

/*
Function: MainFrame::DoDelete
Description: Deletes the selected file or directory after prompting for confirmation. Uses
             FileSystemService to remove files/directories (recursive for directories if
             supported). Refreshes the listing after successful deletion and shows errors
             if the operation fails (e.g., permissions).
Parameters:
  - None
Returns:
  - None
*/
void MainFrame::DoDelete()
{
    const auto sp = GetSelectedPath();
    if (!sp)
    {
        wxMessageBox("Select a file or directory first.", "Delete", wxOK | wxICON_INFORMATION, this);
        return;
    }

    const fs::path target = *sp;

    const wxString msg =
        "Are you sure you want to delete:\n" + ToWx(target) +
        "\n\nThis cannot be undone.";

    if (wxMessageBox(msg, "Confirm Delete", wxYES_NO | wxNO_DEFAULT | wxICON_WARNING, this) != wxYES)
        return;

    std::uintmax_t removed = 0;
    std::string err;
    if (!m_fs.RemoveRecursive(target, removed, err))
    {
        ShowError("Delete", wxString::FromUTF8(err));
        return;
    }

    SetStatusText(wxString::Format("Deleted (%llu item(s)).", (unsigned long long)removed));
    RefreshListing();
}

/*
Function: MainFrame::DoCopy
Description: Places the selected file/directory into the application’s virtual clipboard.
             If cut==false, the item will be copied on paste; if cut==true, the item will
             be moved on paste. Updates the status bar to reflect clipboard state.
Parameters:
  - cut: If true, mark as “Cut” (move on paste); otherwise mark as “Copy”.
Returns:
  - None
*/
void MainFrame::DoCopy(bool cut)
{
    const auto sp = GetSelectedPath();
    if (!sp)
    {
        wxMessageBox("Select a file or directory first.", cut ? "Cut" : "Copy", wxOK | wxICON_INFORMATION, this);
        return;
    }

    m_clip.source = *sp;
    m_clip.isCut = cut;
    m_clip.hasItem = true;

    SetStatusText((cut ? "Cut: " : "Copied: ") + ToWx(m_clip.source));
}

/*
Function: MainFrame::DoPaste
Description: Completes a copy/cut operation by pasting the clipboard item into the current
             directory. If a target with the same name exists, prompts the user for overwrite.
             Calls FileSystemService to perform the copy/move. On success clears clipboard and
             updates status bar; on failure shows an error dialog.
Parameters:
  - None
Returns:
  - None
*/
void MainFrame::DoPaste()
{
    if (!m_clip.hasItem)
    {
        wxMessageBox("Clipboard is empty.", "Paste", wxOK | wxICON_INFORMATION, this);
        return;
    }

    const fs::path dest = m_currentDir / m_clip.source.filename();
    bool overwrite = false;

    if (FileSystemService::Exists(dest))
    {
        const wxString q =
            "Destination already exists:\n" + ToWx(dest) +
            "\n\nOverwrite?";

        if (wxMessageBox(q, "Overwrite?", wxYES_NO | wxNO_DEFAULT | wxICON_WARNING, this) != wxYES)
            return;

        overwrite = true;
    }

    std::string err;
    if (!m_fs.PasteInto(m_clip, m_currentDir, overwrite, err))
    {
        ShowError("Paste", wxString::FromUTF8(err));
        return;
    }

    // Assignment expectation: clipboard clears after paste
    m_clip.Clear();
    SetStatusText("Paste complete | Clipboard cleared");
    RefreshListing();
}

/*
Function: MainFrame::DoRefresh
Description: Refreshes the file listing for the current directory by calling RefreshListing.
             Used for the Refresh menu item and F5 accelerator.
Parameters:
  - None
Returns:
  - None
*/
void MainFrame::DoRefresh()
{
    RefreshListing();
}

/*
Function: MainFrame::OnPathEnter
Description: Event handler triggered when the user presses Enter in the path text control.
             Attempts to navigate to the typed directory path. Invalid paths produce an
             error dialog and do not change the current directory.
Parameters:
  - event: wxWidgets command event for the Enter action.
Returns:
  - None
*/
void MainFrame::OnPathEnter(wxCommandEvent&)
{
    const wxString text = m_pathCtrl->GetValue().Trim(true).Trim(false);
    if (text.empty()) return;

    SetDirectory(fs::path(text.ToStdString()));
}

/*
Function: MainFrame::OnItemActivated
Description: Event handler for double-click/activation on a list item. This mirrors “Open”:
             directories are entered; files are launched using the system default application.
Parameters:
  - event: wxListEvent containing the activated item information.
Returns:
  - None
*/
void MainFrame::OnItemActivated(wxListEvent&)
{
    DoOpen();
}

/*
Function: MainFrame::OnMenuNew
Description: Menu event handler for “New Directory”. Delegates to DoNew().
Parameters:
  - event: wxWidgets menu command event.
Returns:
  - None
*/
void MainFrame::OnMenuNew(wxCommandEvent&)
{ 
    DoNew(); 
}

/*
Function: MainFrame::OnMenuOpen
Description: Menu event handler for “Open”. Delegates to DoOpen().
Parameters:
  - event: wxWidgets menu command event.
Returns:
  - None
*/
void MainFrame::OnMenuOpen(wxCommandEvent&)
{ 
    DoOpen(); 
}

/*
Function: MainFrame::OnMenuRename
Description: Menu event handler for “Rename”. Delegates to DoRename().
Parameters:
  - event: wxWidgets menu command event.
Returns:
  - None
  */
void MainFrame::OnMenuRename(wxCommandEvent&)  
{ 
    DoRename(); 
}

/*
Function: MainFrame::OnMenuDelete
Description: Menu event handler for “Delete”. Delegates to DoDelete().
Parameters:
  - event: wxWidgets menu command event.
Returns:
  - None
*/
void MainFrame::OnMenuDelete(wxCommandEvent&)  
{ 
    DoDelete(); 
}

/*
Function: MainFrame::OnMenuCopy
Description: Menu event handler for “Copy”. Marks selection for copy by calling DoCopy(false).
Parameters:
  - event: wxWidgets menu command event.
Returns:
  - None
*/
void MainFrame::OnMenuCopy(wxCommandEvent&)    
{ 
    DoCopy(false); 
}

/*
Function: MainFrame::OnMenuCut
Description: Menu event handler for “Cut”. Marks selection for move by calling DoCopy(true).
Parameters:
  - event: wxWidgets menu command event.
Returns:
  - None
*/
void MainFrame::OnMenuCut(wxCommandEvent&)     
{ 
    DoCopy(true); 
}

/*
Function: MainFrame::OnMenuPaste
Description: Menu event handler for “Paste”. Delegates to DoPaste().
Parameters:
  - event: wxWidgets menu command event.
Returns:
  - None
*/
void MainFrame::OnMenuPaste(wxCommandEvent&)   
{ 
    DoPaste(); 
}

/*
Function: MainFrame::OnMenuRefresh
Description: Menu event handler for “Refresh”. Delegates to DoRefresh().
Parameters:
  - event: wxWidgets menu command event.
Returns:
  - None
*/
void MainFrame::OnMenuRefresh(wxCommandEvent&) 
{ 
    DoRefresh(); 
}

/*
Function: MainFrame::OnMenuExit
Description: Menu event handler for “Exit”. Closes the application window cleanly.
Parameters:
  - event: wxWidgets menu command event.
Returns:
  - None
*/
void MainFrame::OnMenuExit(wxCommandEvent&)    
{ 
    Close(true); 
}
