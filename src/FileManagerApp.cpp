/*
Parneet Baidwan - 251259638
Description: 
The application entry point for the file manager system resides in this document. The document establishes the wxWidgets application class which handles GUI framework initialization and main application window construction. The main frame which operates the file manager interface the program uses to control its operation begins through this file.

February 1, 2026
*/

#include "MainFrame.h"
#include <wx/wx.h>


/*
Function: FileManagerApp::OnInit
Description: Entry point called by wxWidgets during application startup. Creates the main
             application window (MainFrame), shows it, and returns whether initialization
             succeeded. If this returns false, the GUI will not launch.
Parameters:
  - None
Returns:
  - bool: true if initialization succeeded and the event loop should start; false otherwise.
*/
class FileManagerApp : public wxApp {
    public:
    virtual bool OnInit();
};


// wxWidgets macro to hook the app class into the framework
wxIMPLEMENT_APP(FileManagerApp);


// Called on application startup
bool FileManagerApp::OnInit() {

    // Create the main window and show it
    MainFrame* frame = new MainFrame("CS3307 File Manager");
    frame->Show(true);
return true;
}