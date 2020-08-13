///////////////////////////////////////////////////////////////////////////////
// ControllerFormGL.h
// ==================
// Derived Controller class for OpenGL dialog window
//
//  AUTHOR: Song Ho Ahn (song.ahn@gamil.com)
// CREATED: 2006-07-09
// UPDATED: 2020-01-27
///////////////////////////////////////////////////////////////////////////////

#ifndef WIN_CONTROLLER_FORM_GL_H
#define WIN_CONTROLLER_FORM_GL_H

#include "Controller.h"
#include "ViewFormGL.h"
#include "ModelGL.h"


namespace Win
{
    class ControllerFormGL : public Controller
    {
    public:
        ControllerFormGL(ModelGL* model, ViewFormGL* view);
        ~ControllerFormGL() {};

        LRESULT close();                                // for WM_CLOSE
        LRESULT command(int id, int cmd, LPARAM msg);   // for WM_COMMAND
        LRESULT create();                               // for WM_CREATE
        LRESULT hScroll(WPARAM wParam, LPARAM lParam);  // for WM_HSCROLL

    private:
        ModelGL* model;                                 // pointer to model component
        ViewFormGL* view;                               // pointer to view component
    };
}

#endif
