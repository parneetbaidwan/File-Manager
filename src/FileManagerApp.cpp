#include <wx/wx.h>
#include "MainFrame.h"

class FileManagerApp final : public wxApp
{
public:
    bool OnInit() override
    {
        auto* frame = new MainFrame("Simple File Manager");
        frame->Show(true);
        return true;
    }
};

wxIMPLEMENT_APP(FileManagerApp);
