///////////////////////////////////////////////////////////////////////////////
// ControllerForm.cpp
// ==================
// Derived Controller class for dialog window
//
//  AUTHOR: Song Ho Ahn (song.ahn@gamil.com)
// CREATED: 2016-05-28
// UPDATED: 2016-07-13
///////////////////////////////////////////////////////////////////////////////

#include <cstdlib>
#include <string>
#include <sstream>
#include "ControllerForm.h"
#include "wcharUtil.h"
#include "resource.h"
#include "Log.h"
using namespace Win;


INT_PTR CALLBACK aboutDialogProcedure(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
void initAboutDialogControls(HWND hwnd);
void getSettingsFromAboutDialog(HWND dialogHandle);



///////////////////////////////////////////////////////////////////////////////
// default contructor
///////////////////////////////////////////////////////////////////////////////
ControllerForm::ControllerForm(ModelGL* model, ViewForm* view)
    : model(model), view(view), glCtrl1(0), glCtrl2(0)
{
}



///////////////////////////////////////////////////////////////////////////////
// handle WM_DESTROY
///////////////////////////////////////////////////////////////////////////////
int ControllerForm::destroy()
{
    Win::log("Form dialog is destroyed.");
    return 0;
}



///////////////////////////////////////////////////////////////////////////////
// handle WM_CREATE
///////////////////////////////////////////////////////////////////////////////
int ControllerForm::create()
{
    // initialize all controls
    view->initControls(handle);

    return 0;
}



///////////////////////////////////////////////////////////////////////////////
// handle WM_COMMAND
///////////////////////////////////////////////////////////////////////////////
int ControllerForm::command(int id, int command, LPARAM msg)
{
    switch(id)
    {
    case IDC_BUTTON_RESET:
        if(command == BN_CLICKED)
        {
            model->resetCamera();
            view->resetControls();
            glCtrl1->paint();
            glCtrl2->paint();
        }
        break;

    case IDC_BUTTON_ABOUT:
        if(command == BN_CLICKED)
        {
            // open About dialog
            ::DialogBox((HINSTANCE)::GetWindowLongPtr(handle, GWLP_HINSTANCE), MAKEINTRESOURCE(IDD_DIALOG_ABOUT), handle, aboutDialogProcedure);
        }
        break;

    case IDC_CHECK_FOV:
        if(command == BN_CLICKED)
        {
            view->toggleFov();
            glCtrl1->paint();
        }
        break;

    case IDC_CHECK_GRID:
        if(command == BN_CLICKED)
        {
            view->toggleGrid();
            glCtrl1->paint();
            glCtrl2->paint();
        }
        break;
    }

    return 0;
}



///////////////////////////////////////////////////////////////////////////////
// handle horizontal scroll notification
///////////////////////////////////////////////////////////////////////////////
int ControllerForm::hScroll(WPARAM wParam, LPARAM lParam)
{
    // check if the message comming from trackbar
    HWND trackbarHandle = (HWND)lParam;

    int position = HIWORD(wParam);              // current tick mark position
    if(trackbarHandle)
    {
        // get control ID
        int trackbarId = ::GetDlgCtrlID(trackbarHandle);

        switch(LOWORD(wParam))
        {
        case TB_THUMBTRACK:     // user dragged the slider
            //Win::log("trackbar: %d", position);
            view->updateTrackbars(trackbarHandle, position);
            break;

        case TB_THUMBPOSITION:  // by WM_LBUTTONUP
            break;

        case TB_LINEUP:         // by VK_RIGHT, VK_DOWN
            break;

        case TB_LINEDOWN:       // by VK_LEFT, VK_UP
            break;

        case TB_TOP:            // by VK_HOME
            break;

        case TB_BOTTOM:         // by VK_END
            break;

        case TB_PAGEUP:         // by VK_PRIOR (User click the channel to prior.)
            break;

        case TB_PAGEDOWN:       // by VK_NEXT (User click the channel to next.)
            break;

        case TB_ENDTRACK:       // by WM_KEYUP (User release a key.)
            position = (int)::SendMessage(trackbarHandle, TBM_GETPOS, 0, 0);
            view->updateTrackbars(trackbarHandle, position);
            break;
        }
    }

    glCtrl1->paint();
    glCtrl2->paint();
    return 0;
}



///////////////////////////////////////////////////////////////////////////////
// handle WM_NOTIFY
// The id is not guaranteed to be unique, so use NMHDR.hwndFrom and NMHDR.idFrom.
///////////////////////////////////////////////////////////////////////////////
int ControllerForm::notify(int id, LPARAM lParam)
{
    // first cast lParam to NMHDR* to know what the control is
    NMHDR* nmhdr = (NMHDR*)lParam;
    HWND from = nmhdr->hwndFrom;

    switch(nmhdr->code)
    {
    /*
    // SysLink notifications ==========
    case NM_CLICK:
    case NM_RETURN:
        {
        // cast again lParam to NMLINK* {NMHDR, LITEM}
        NMLINK* nmlink = (NMLINK*)lParam;
        LITEM litem = nmlink->item; // SysLink item {maks, iLink, state, stateMask, szID, szUrl}
        view->handleLink(from, litem.iLink);
        }
        break;
    */
    // UpDownBox notifications =========
    case UDN_DELTAPOS:         // the change of position has begun
        {
        // cast again lParam to NMUPDOWN*
        NMUPDOWN* nmUpDown = (NMUPDOWN*)lParam;
        view->changeUpDownPosition(from, nmUpDown->iPos + nmUpDown->iDelta);
        glCtrl1->paint();
        glCtrl2->paint();
        break;
        }
    }

    return 0;
}



///////////////////////////////////////////////////////////////////////////////
// dialog procedure for About window
///////////////////////////////////////////////////////////////////////////////
INT_PTR CALLBACK aboutDialogProcedure(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
    case WM_INITDIALOG:
        break;

    case WM_CLOSE:
        {
            ::EndDialog(hwnd, 0);
        }
        break;

    case WM_COMMAND:
        if(LOWORD(wParam) == IDOK && HIWORD(wParam) == BN_CLICKED)
        {
            ::EndDialog(hwnd, 0);
        }
        break;

    case WM_NOTIFY:
        NMHDR* nmhdr = (NMHDR*)lParam;
        HWND from = nmhdr->hwndFrom;
        if(from == ::GetDlgItem(hwnd, IDC_LINK1) && (nmhdr->code == NM_CLICK || nmhdr->code == NM_RETURN))
        {
            // cast again lParam to NMLINK*
            NMLINK* nmlink = (NMLINK*)lParam;
            ::ShellExecute(0, L"open", nmlink->item.szUrl, 0, 0, SW_SHOW);
        }
        break;
    }

    return false;
}



///////////////////////////////////////////////////////////////////////////////
// remember GL controllers
///////////////////////////////////////////////////////////////////////////////
void ControllerForm::setGLControllers(ControllerGL1* c1, ControllerGL2* c2)
{
    glCtrl1 = c1;
    glCtrl2 = c2;
    glCtrl1->paint();
    glCtrl2->paint();
}
