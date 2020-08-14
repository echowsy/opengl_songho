///////////////////////////////////////////////////////////////////////////////
// main.cpp
// ========
// main driver
//
//  AUTHOR: Song Ho Ahn (song.ahn@gmail.com)
// CREATED: 2016-05-26
// UPDATED: 2016-07-22
///////////////////////////////////////////////////////////////////////////////

#define WIN32_LEAN_AND_MEAN             // exclude rarely-used stuff from Windows headers

#include <windows.h>
#include <commctrl.h>                   // common controls
#include "Window.h"
#include "DialogWindow.h"
#include "ControllerMain.h"
#include "ControllerGL1.h"
#include "ControllerGL2.h"
#include "ControllerForm.h"
#include "ModelGL.h"
#include "ViewGL.h"
#include "ViewForm.h"
#include "resource.h"
#include "Log.h"


// function declarations
int mainMessageLoop(HACCEL hAccelTable=0);




///////////////////////////////////////////////////////////////////////////////
// main function of a windows application
///////////////////////////////////////////////////////////////////////////////
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR cmdArgs, int cmdShow)
{
    //Win::logMode(Win::LOG_MODE_DIALOG);
    //Win::logMode(Win::LOG_MODE_BOTH);

    // init comctl32.dll before creating windows
    INITCOMMONCONTROLSEX commonCtrls;
    commonCtrls.dwSize = sizeof(commonCtrls);
    commonCtrls.dwICC = ICC_STANDARD_CLASSES | ICC_BAR_CLASSES | ICC_LINK_CLASS| ICC_UPDOWN_CLASS;
    ::InitCommonControlsEx(&commonCtrls);

    // get app name from resource file
    wchar_t appName[256];
    ::LoadString(hInst, IDS_APP_NAME, appName, 256);

    // create main window
    Win::ControllerMain mainCtrl;
    Win::Window mainWin(hInst, appName, 0, &mainCtrl);

    // create a window and show
    if(mainWin.create())
        Win::log("Main window is created.");
    else
        Win::log("[ERROR] Failed to create main window.");

    //@@ MS ArticleID: 272222
    // There is a clipping bug when the window is resized larger.
    // Create a window with the max size initially to avoid this clipping bug.
    // Subsequent SetWindowPos() calls to resize the window dimension do not
    // generate the clipping issue.
    int fullWidth = ::GetSystemMetrics(SM_CXSCREEN);    // primary display only
    int fullHeight = ::GetSystemMetrics(SM_CYSCREEN);
    Win::log("Display Resolution: %dx%d", fullWidth, fullHeight);

    // create model and view components for controller
    ModelGL modelGL;
    Win::ViewGL viewGL1;
    Win::ViewGL viewGL2;

    // create OpenGL rendering window 1 as a child
    Win::ControllerGL1 glCtrl1(&modelGL, &viewGL1);
    Win::Window glWin1(hInst, L"GL1", mainWin.getHandle(), &glCtrl1);
    glWin1.setWindowStyle(WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
    glWin1.setWindowStyleEx(WS_EX_WINDOWEDGE);
    glWin1.setClassStyle(CS_OWNDC);
    glWin1.setWidth(fullWidth);
    glWin1.setHeight(fullHeight);
    if(glWin1.create())
        Win::log("OpenGL rendering window 1 is created.");
    else
        Win::log("[ERROR] Failed to create OpenGL window 1.");

    // create OpenGL rendering window 2 as a child
    Win::ControllerGL2 glCtrl2(&modelGL, &viewGL2);
    // set RC with existing RC/pixelformat from view1
    glCtrl2.setRC(glCtrl1.getRC(), glCtrl1.getPixelFormat());
    Win::Window glWin2(hInst, L"GL2", mainWin.getHandle(), &glCtrl2);
    glWin2.setWindowStyle(WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
    glWin2.setWindowStyleEx(WS_EX_WINDOWEDGE);
    glWin2.setClassStyle(CS_OWNDC);
    glWin2.setWidth(fullWidth);
    glWin2.setHeight(fullHeight);
    if(glWin2.create())
        Win::log("OpenGL rendering window 2 is created.");
    else
        Win::log("[ERROR] Failed to create OpenGL window 2.");

    // create a child dialog box contains controls
    Win::ViewForm viewForm(&modelGL);
    Win::ControllerForm formCtrl(&modelGL, &viewForm);
    Win::DialogWindow formDialog(hInst, IDD_FORM_MAIN, mainWin.getHandle(), &formCtrl);
    if(formDialog.create())
        Win::log("Form dialog is created.");
    else
        Win::log("[ERROR] Failed to create Form dialog.");

    // send window handles to mainCtrl, they are used for resizing window
    mainCtrl.setGLHandles(glWin1.getHandle(), glWin2.getHandle());
    mainCtrl.setFormHandle(formDialog.getHandle());

    // send gl controllers to form controller to draw after any event
    formCtrl.setGLControllers(&glCtrl1, &glCtrl2);

    // place windows in the right position ================
    RECT rect;

    // compute dim of dialog window (for higher DPI display)
    ::GetWindowRect(formDialog.getHandle(), &rect);
    int dialogWidth = rect.right - rect.left;
    int dialogHeight = rect.bottom - rect.top;
    //Win::log("Form Dialog Dimension: %dx%d", dialogWidth, dialogHeight);

    // set dim of opengl windows
    int glWidth = dialogWidth / 2;
    int glHeight = glWidth;
    ::SetWindowPos(glWin1.getHandle(), 0, 0, 0, glWidth, glHeight, SWP_NOZORDER);
    ::SetWindowPos(glWin2.getHandle(), 0, 0, 0, glWidth, glHeight, SWP_NOZORDER);

    // place the form dialog at the bottom of the opengl rendering window
    ::SetWindowPos(formDialog.getHandle(), 0, 0, glHeight, dialogWidth, dialogHeight, SWP_NOZORDER);

    // set dim of main window
    rect.left = 0;
    rect.right = dialogWidth;
    rect.top = 0;
    rect.bottom = dialogHeight + glHeight;
    DWORD style = (DWORD)::GetWindowLongPtr(mainWin.getHandle(), GWL_STYLE);
    DWORD styleEx = (DWORD)::GetWindowLongPtr(mainWin.getHandle(), GWL_EXSTYLE);
    ::AdjustWindowRectEx(&rect, style, FALSE, styleEx);
    ::SetWindowPos(mainWin.getHandle(), 0, 0, 0, (rect.right-rect.left), (rect.bottom-rect.top), SWP_NOZORDER);

    // show all windows
    glWin1.show();
    glWin2.show();
    formDialog.show();
    mainWin.show();
    Win::log("Main window is open.\n");

    // main message loop //////////////////////////////////////////////////////
    int exitCode;
    HACCEL hAccelTable = 0;
    //hAccelTable = ::LoadAccelerators(hInst, MAKEINTRESOURCE(ID_ACCEL));
    exitCode = mainMessageLoop(hAccelTable);
    Win::log("Application is terminated.");

    return exitCode;
}



///////////////////////////////////////////////////////////////////////////////
// main message loop
///////////////////////////////////////////////////////////////////////////////
int mainMessageLoop(HACCEL hAccelTable)
{
    HWND activeHandle;
    MSG msg;

    while(::GetMessage(&msg, 0, 0, 0) > 0)  // loop until WM_QUIT(0) received
    {
        // determine the activated window is dialog box
        // skip if messages are for the dialog windows
        activeHandle = GetActiveWindow();
        if(::GetWindowLongPtr(activeHandle, GWL_EXSTYLE) & WS_EX_CONTROLPARENT) // WS_EX_CONTROLPARENT is automatically added by CreateDialogBox()
        {
            if(::IsDialogMessage(activeHandle, &msg))
                continue;   // message handled, back to while-loop
        }

        // now, handle window messages
        if(!::TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;                 // return nExitCode of PostQuitMessage()
}
