///////////////////////////////////////////////////////////////////////////////
// ControllerGL.cpp
// ================
// Derived Controller class for OpenGL window
// It is the controller of OpenGL rendering window. It initializes DC and RC,
// when WM_CREATE called, then, start new thread for OpenGL rendering loop.
//
// When this class is constructed, it gets the pointers to model and view
// components.
//
//  AUTHOR: Song Ho Ahn (song.ahn@gamil.com)
// CREATED: 2006-07-09
// UPDATED: 2020-01-27
///////////////////////////////////////////////////////////////////////////////

#include "ControllerGL.h"
using namespace Win;



///////////////////////////////////////////////////////////////////////////////
// default contructor
///////////////////////////////////////////////////////////////////////////////
ControllerGL::ControllerGL(ModelGL* model, ViewGL* view) : modelGL(model), viewGL(view),
                                                           loopFlag(false), resizeFlag(false),
                                                           clientWidth(0), clientHeight(0)
{
}



///////////////////////////////////////////////////////////////////////////////
// handle WM_CLOSE
///////////////////////////////////////////////////////////////////////////////
LRESULT ControllerGL::close()
{
    // wait for rendering thread is terminated
    loopFlag = false;
    glThread.join();

    ::DestroyWindow(handle);
    return 0;
}



///////////////////////////////////////////////////////////////////////////////
// handle WM_DESTROY
///////////////////////////////////////////////////////////////////////////////
LRESULT ControllerGL::destroy()
{
    ::PostQuitMessage(0);       // exit the message loop
    return 0;
}



///////////////////////////////////////////////////////////////////////////////
// handle WM_CREATE
///////////////////////////////////////////////////////////////////////////////
LRESULT ControllerGL::create()
{
    // create a OpenGL rendering context
    if(!viewGL->createContext(handle, 32, 24, 8))
    {
        //Win::log(L"[ERROR] Failed to create OpenGL rendering context from ControllerGL::create().");
        return -1;
    }

    // create a thread for OpenGL rendering
    glThread = std::thread(&ControllerGL::runThread, this);
    loopFlag = true;

    return 0;
}



///////////////////////////////////////////////////////////////////////////////
// handle WM_PAINT
///////////////////////////////////////////////////////////////////////////////
LRESULT ControllerGL::paint()
{
    return 0;
}

///////////////////////////////////////////////////////////////////////////////
// handle WM_COMMAND
///////////////////////////////////////////////////////////////////////////////
LRESULT ControllerGL::command(int id, int cmd, LPARAM msg)
{
    return 0;
}



///////////////////////////////////////////////////////////////////////////////
// rendering thread
// initialize OpenGL states and start rendering loop
///////////////////////////////////////////////////////////////////////////////
void ControllerGL::runThread()
{
    // set the current RC in this thread
    ::wglMakeCurrent(viewGL->getDC(), viewGL->getRC());

    // initialize OpenGL states
    modelGL->init();

    // cofigure projection matrix
    RECT rect;
    ::GetClientRect(handle, &rect);
    modelGL->setViewport(rect.right, rect.bottom);

    // rendering loop
    while(loopFlag)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));  // yield to other processes or threads

        if(resizeFlag)
        {
            modelGL->setViewport(clientWidth, clientHeight);
            resizeFlag = false;
        }

        modelGL->draw();
        viewGL->swapBuffers();
    }

    // close OpenGL Rendering context
    viewGL->closeContext(handle);
    ::wglMakeCurrent(0, 0);             // unset RC
}



///////////////////////////////////////////////////////////////////////////////
// handle Left mouse down
///////////////////////////////////////////////////////////////////////////////
LRESULT ControllerGL::lButtonDown(WPARAM state, int x, int y)
{
    // update mouse position
    modelGL->setMousePosition(x, y);

    if(state == MK_LBUTTON)
    {
        modelGL->setMouseLeft(true);
    }

    return 0;
}



///////////////////////////////////////////////////////////////////////////////
// handle Left mouse up
///////////////////////////////////////////////////////////////////////////////
LRESULT ControllerGL::lButtonUp(WPARAM state, int x, int y)
{
    // update mouse position
    modelGL->setMousePosition(x, y);

    modelGL->setMouseLeft(false);

    return 0;
}



///////////////////////////////////////////////////////////////////////////////
// handle reft mouse down
///////////////////////////////////////////////////////////////////////////////
LRESULT ControllerGL::rButtonDown(WPARAM state, int x, int y)
{
    // update mouse position
    modelGL->setMousePosition(x, y);

    if(state == MK_RBUTTON)
    {
        modelGL->setMouseRight(true);
    }

    return 0;
}



///////////////////////////////////////////////////////////////////////////////
// handle reft mouse up
///////////////////////////////////////////////////////////////////////////////
LRESULT ControllerGL::rButtonUp(WPARAM state, int x, int y)
{
    // update mouse position
    modelGL->setMousePosition(x, y);

    modelGL->setMouseRight(false);

    return 0;
}



///////////////////////////////////////////////////////////////////////////////
// handle WM_MOUSEMOVE
///////////////////////////////////////////////////////////////////////////////
LRESULT ControllerGL::mouseMove(WPARAM state, int x, int y)
{
    if(state == MK_LBUTTON)
    {
        modelGL->rotateCamera(x, y);
    }
    if(state == MK_RBUTTON)
    {
        modelGL->zoomCamera(y);
    }

    return 0;
}



///////////////////////////////////////////////////////////////////////////////
// handle WM_KEYDOWN
///////////////////////////////////////////////////////////////////////////////
LRESULT ControllerGL::keyDown(int key, LPARAM lParam)
{
    if(key == VK_ESCAPE)
    {
        ::PostMessage(handle, WM_CLOSE, 0, 0);
    }

    return 0;
}



///////////////////////////////////////////////////////////////////////////////
// handle WM_SIZE notification
// Note that the input param, width and height is for client area only.
// It excludes non-client area.
///////////////////////////////////////////////////////////////////////////////
LRESULT ControllerGL::size(int width, int height, WPARAM type)
{
    resizeFlag = true;
    clientWidth = width;
    clientHeight = height;

    return 0;
}
