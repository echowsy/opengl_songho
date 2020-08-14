///////////////////////////////////////////////////////////////////////////////
// ControllerMain.cpp
// ==================
// Derived Controller class for main window
//
//  AUTHOR: Song Ho Ahn (song.ahn@gmail.com)
// CREATED: 2016-05-28
// UPDATED: 2016-07-12
///////////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <commctrl.h>                   // common controls
#include "ControllerMain.h"
#include "resource.h"
#include "Log.h"

using namespace Win;


ControllerMain::ControllerMain() : glHandle1(0), glHandle2(0), formHandle(0)
{
}



int ControllerMain::command(int id, int cmd, LPARAM msg)
{
    return 0;
}



int ControllerMain::close()
{
    Win::log("Closing the application...");
    ::DestroyWindow(handle);    // destroy this window and children
    return 0;
}



int ControllerMain::destroy()
{
    ::PostQuitMessage(0);       // exit the message loop
    Win::log("Main windows is destroyed.");
    return 0;
}



int ControllerMain::create()
{
    return 0;
}



int ControllerMain::size(int w, int h, WPARAM wParam)
{
    RECT rect;

    // get client dim of mainWin
    ::GetClientRect(handle, &rect);
    int mainClientWidth = rect.right - rect.left;
    int mainClientHeight = rect.bottom - rect.top;

    // get dim of form dialog
    ::GetWindowRect(formHandle, &rect);
    int formHeight = rect.bottom - rect.top;

    // compute dims of 2 gl windows
    int glWidth = mainClientWidth / 2;
    int glHeight = mainClientHeight - formHeight;

    // resize glWin and reposition form
    ::SetWindowPos(glHandle1, 0, 0, 0, glWidth, glHeight, SWP_NOZORDER);
    ::SetWindowPos(glHandle2, 0, glWidth+1, 0, glWidth, glHeight, SWP_NOZORDER);
    ::SetWindowPos(formHandle, 0, 0, mainClientHeight-formHeight, mainClientWidth, formHeight, SWP_NOZORDER);
    ::InvalidateRect(formHandle, 0, TRUE);      // force to repaint

    return 0;
}
