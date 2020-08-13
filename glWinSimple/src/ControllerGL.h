///////////////////////////////////////////////////////////////////////////////
// ControllerGL.h
// ==============
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

#ifndef WIN_CONTROLLER_GL_H
#define WIN_CONTROLLER_GL_H

#include <thread>       // for thread
#include <chrono>       // for milliseconds()
#include "Controller.h"
#include "ViewGL.h"
#include "ModelGL.h"
#include "ViewGL.h"


namespace Win
{
    class ControllerGL : public Controller
    {
    public:
        ControllerGL(ModelGL* model, ViewGL* view); // ctor with params
        ~ControllerGL() {};                         // dtor

        LRESULT close();                            // close the RC and destroy OpenGL window
        LRESULT command(int id, int cmd, LPARAM msg); // for WM_COMMAND
        LRESULT create();                           // create RC for OpenGL window and start new thread for rendering
        LRESULT destroy();
        LRESULT paint();
        LRESULT lButtonDown(WPARAM state, int x, int y);
        LRESULT lButtonUp(WPARAM state, int x, int y);
        LRESULT rButtonDown(WPARAM state, int x, int y);
        LRESULT rButtonUp(WPARAM state, int x, int y);
        LRESULT mouseMove(WPARAM state, int x, int y);
        LRESULT keyDown(int key, LPARAM lParam);
        LRESULT size(int width, int height, WPARAM type);

    private:
        void runThread();                           // thread for OpenGL rendering

        ModelGL* modelGL;                           //
        ViewGL* viewGL;                             //
        volatile bool loopFlag;                     // rendering loop flag
        std::thread glThread;                       // opengl rendering thread
        bool resizeFlag;
        int clientWidth;                            // width of client area
        int clientHeight;                           // height of client area
    };
}

#endif
