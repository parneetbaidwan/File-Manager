#include <wx/wx.h>
#include "MainFrame.h"

/*
 * FileManagerApp
 * - HelloWorld pattern: wxApp subclass + OnInit creates the main frame
 */
class FileManagerApp final : public wxApp
{
public:
    bool OnInit() override
    {
        auto* frame = new MainFrame("FileManager");
        frame->Show(true);
        return true;
    }
};

wxIMPLEMENT_APP(FileManagerApp);
