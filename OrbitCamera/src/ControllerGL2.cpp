///////////////////////////////////////////////////////////////////////////////
// ControllerGL2.cpp
// =================
// Derived Controller class for OpenGL window
// It is the controller of OpenGL rendering window. It initializes DC and RC,
// when WM_CREATE called, then, start new thread for OpenGL rendering loop.
//
// When this class is constructed, it gets the pointers to model and view
// components.
//
//  AUTHOR: Song Ho Ahn (song.ahn@gamil.com)
// CREATED: 2016-05-29
// UPDATED: 2016-07-18
///////////////////////////////////////////////////////////////////////////////

#include <string>
#include <sstream>
#include "ControllerGL2.h"
#include "wcharUtil.h"
#include "Log.h"
using namespace Win;



///////////////////////////////////////////////////////////////////////////////
// default contructor
///////////////////////////////////////////////////////////////////////////////
ControllerGL2::ControllerGL2(ModelGL* model, ViewGL* view)
    : model(model), view(view), hglrc(0), pixelFormat(0)
{
}



///////////////////////////////////////////////////////////////////////////////
// handle WM_DESTROY
///////////////////////////////////////////////////////////////////////////////
int ControllerGL2::destroy()
{
    ::wglMakeCurrent(0, 0);
    ::ReleaseDC(handle, view->getDC());
    Win::log("OpenGL window is destroyed for screen 2.");
    return 0;
}



///////////////////////////////////////////////////////////////////////////////
// handle WM_CREATE
///////////////////////////////////////////////////////////////////////////////
int ControllerGL2::create()
{
    // set DC only, and use RC from view 1
    bool result = view->setContext(handle, hglrc, pixelFormat);
    if(result)
        Win::log("Created OpenGL window for screen 2.");
    else
        Win::log("[ERROR] Failed to create OpenGL window for screen 2.");

    return 0;
}



///////////////////////////////////////////////////////////////////////////////
// handle WM_PAINT
///////////////////////////////////////////////////////////////////////////////
int ControllerGL2::paint()
{
    // redraw
    view->activateContext(); // make it active first
    model->draw(2);
    view->swapBuffers();
    return 0;
}



///////////////////////////////////////////////////////////////////////////////
// handle WM_SIZE
///////////////////////////////////////////////////////////////////////////////
int ControllerGL2::size(int w, int h, WPARAM wParam)
{
    model->setWindowSize(w, h);
    paint();    // repaint glWindow
    Win::log(L"Changed OpenGL rendering window size: %dx%d for screen 2.", w, h);
    return 0;
}
