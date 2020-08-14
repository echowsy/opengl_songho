///////////////////////////////////////////////////////////////////////////////
// ControllerForm.h
// ================
// Derived Controller class for dialog window
//
//  AUTHOR: Song Ho Ahn (song.ahn@gamil.com)
// CREATED: 2016-05-28
// UPDATED: 2016-07-15
///////////////////////////////////////////////////////////////////////////////

#ifndef WIN_CONTROLLER_FORM_H
#define WIN_CONTROLLER_FORM_H

#include "Controller.h"
#include "ViewForm.h"
#include "ModelGL.h"
#include "ControllerGL1.h"
#include "ControllerGL2.h"


namespace Win
{
    class ControllerForm : public Controller
    {
    public:
        ControllerForm(ModelGL* model, ViewForm* view);
        ~ControllerForm() {};

        int command(int id, int cmd, LPARAM msg);   // for WM_COMMAND
        int create();                               // for WM_CREATE
        int destroy();                              // for WM_DESTROY
        int hScroll(WPARAM wParam, LPARAM lParam);  // for WM_HSCROLL
        int notify(int id, LPARAM lParam);          // for WM_NOTIFY

        void setGLControllers(ControllerGL1* c1, ControllerGL2* c2);

    private:
        ModelGL* model;                             // pointer to model component
        ViewForm* view;                             // pointer to view component
        ControllerGL1* glCtrl1;
        ControllerGL2* glCtrl2;
    };
}

#endif
