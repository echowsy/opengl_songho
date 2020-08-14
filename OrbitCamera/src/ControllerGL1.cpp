///////////////////////////////////////////////////////////////////////////////
// ControllerGL1.cpp
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
// UPDATED: 2017-10-25
///////////////////////////////////////////////////////////////////////////////

#include <string>
#include <sstream>
#include "ControllerGL1.h"
#include "wcharUtil.h"
#include "Log.h"
using namespace Win;



///////////////////////////////////////////////////////////////////////////////
// default contructor
///////////////////////////////////////////////////////////////////////////////
ControllerGL1::ControllerGL1(ModelGL* model, ViewGL* view)
    : model(model), view(view), mouseHovered(false)
{
}



///////////////////////////////////////////////////////////////////////////////
// handle WM_DESTROY
///////////////////////////////////////////////////////////////////////////////
int ControllerGL1::destroy()
{
    // close OpenGL Rendering Context (RC)
    view->closeContext(handle);
    Win::log("Closed OpenGL rendering context for screen 1.");
    Win::log("OpenGL window is destroyed for screen 1.");
    return 0;
}



///////////////////////////////////////////////////////////////////////////////
// handle WM_CREATE
///////////////////////////////////////////////////////////////////////////////
int ControllerGL1::create()
{
    // create a OpenGL rendering context
    if(view->createContext(handle, 32, 24, 8, 8))
    {
        Win::log(L"Created OpenGL rendering context for screen 1.");
    }
    else
    {
        Win::log(L"[ERROR] Failed to create OpenGL rendering context from ControllerGL::create().");
        return -1;
    }

    // init OpenGL stuff ==================================
    // set the current RC in this thread
    view->activateContext();

    // initialize OpenGL states
    model->init();
    Win::log(L"Initialized OpenGL states. for screen 1.");

    bool result = model->initShaders();
    if(result)
        Win::log("GLSL shader objects are initialized.");
    else
        Win::log("[ERROR] Failed to initialize GLSL.");

    // load OBJ models for rendering
    result = model->loadObjs();
    if(result)
        Win::log(L"Loaded OBJ models.");
    else
        Win::log(L"[ERROR] Failed to load OBJs.");

    return 0;
}



///////////////////////////////////////////////////////////////////////////////
// handle WM_PAINT
///////////////////////////////////////////////////////////////////////////////
int ControllerGL1::paint()
{
    // redraw
    view->activateContext(); // make current
    model->draw(1);
    view->swapBuffers();
    return 0;
}



///////////////////////////////////////////////////////////////////////////////
// handle WM_COMMAND
///////////////////////////////////////////////////////////////////////////////
int ControllerGL1::command(int id, int cmd, LPARAM msg)
{
    return 0;
}



///////////////////////////////////////////////////////////////////////////////
// handle Left mouse down
///////////////////////////////////////////////////////////////////////////////
int ControllerGL1::lButtonDown(WPARAM state, int x, int y)
{
    // update mouse position
    model->setMousePosition(x, y);

    if(state == MK_LBUTTON)
    {
        model->setMouseLeft(true);
    }

    // set focus to receive wm_mousewheel event
    ::SetFocus(handle);
    return 0;
}



///////////////////////////////////////////////////////////////////////////////
// handle Left mouse up
///////////////////////////////////////////////////////////////////////////////
int ControllerGL1::lButtonUp(WPARAM state, int x, int y)
{
    // update mouse position
    model->setMousePosition(x, y);
    model->setMouseLeft(false);
    return 0;
}



///////////////////////////////////////////////////////////////////////////////
// handle reft mouse down
///////////////////////////////////////////////////////////////////////////////
int ControllerGL1::rButtonDown(WPARAM state, int x, int y)
{
    // update mouse position
    model->setMousePosition(x, y);

    if(state == MK_RBUTTON)
    {
        model->setMouseRight(true);
    }

    // set focus to receive wm_mousewheel event
    ::SetFocus(handle);
    return 0;
}



///////////////////////////////////////////////////////////////////////////////
// handle reft mouse up
///////////////////////////////////////////////////////////////////////////////
int ControllerGL1::rButtonUp(WPARAM state, int x, int y)
{
    // update mouse position
    model->setMousePosition(x, y);
    model->setMouseRight(false);
    return 0;
}



///////////////////////////////////////////////////////////////////////////////
// handle WM_MOUSEMOVE
///////////////////////////////////////////////////////////////////////////////
int ControllerGL1::mouseMove(WPARAM state, int x, int y)
{
    // for tracking mouse hover/leave tracking
    if(!mouseHovered)
    {
        // mouse hover/leave tracking
        TRACKMOUSEEVENT tme;
        tme.cbSize = sizeof(tme);
        tme.dwFlags = TME_HOVER | TME_LEAVE;
        tme.hwndTrack = handle;
        tme.dwHoverTime = HOVER_DEFAULT;
        ::TrackMouseEvent(&tme);
        mouseHovered = true;
    }

    if(state == MK_LBUTTON)
    {
        model->rotateCamera(x, y);
        paint();
    }
    if(state == MK_RBUTTON)
    {
        model->zoomCamera(y);
        paint();
    }

    return 0;
}



///////////////////////////////////////////////////////////////////////////////
// handle WM_MOUSEHOVER
///////////////////////////////////////////////////////////////////////////////
int ControllerGL1::mouseHover(int state, int x, int y)
{
    mouseHovered = true;
    return 0;
}



///////////////////////////////////////////////////////////////////////////////
// handle WM_MOUSELEAVE
///////////////////////////////////////////////////////////////////////////////
int ControllerGL1::mouseLeave()
{
    mouseHovered = false;
    return 0;
}



///////////////////////////////////////////////////////////////////////////////
// handle WM_MOUSEWHEEL
///////////////////////////////////////////////////////////////////////////////
int ControllerGL1::mouseWheel(int state, int delta, int x, int y)
{
    if(mouseHovered)
    {
        model->zoomCameraDelta(delta / 120.0f);
        paint();
    }
    return 0;
}



///////////////////////////////////////////////////////////////////////////////
// handle WM_SIZE
///////////////////////////////////////////////////////////////////////////////
int ControllerGL1::size(int w, int h, WPARAM wParam)
{
    model->setWindowSize(w, h);
    paint();    // repaint glWindow
    Win::log(L"Changed OpenGL rendering window size: %dx%d for screen 1.", w, h);
    return 0;
}
