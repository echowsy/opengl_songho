///////////////////////////////////////////////////////////////////////////////
// ViewForm.h
// ==========
// View component of dialog window
//
//  AUTHORL Song Ho Ahn (song.ahn@gmail.com)
// CREATED: 2016-05-29
// UPDATED: 2016-07-12
///////////////////////////////////////////////////////////////////////////////

#ifndef VIEW_FORM_H
#define VIEW_FORM_H

#include <windows.h>
#include "Controls.h"
#include "ModelGL.h"

namespace Win
{
    class ViewForm
    {
    public:
        ViewForm(ModelGL* model);
        ~ViewForm();

        void initControls(HWND handle);         // init all controls
        void resetControls();
        void updateTrackbars(HWND handle, int position);
        //void handleLink(HWND handle, int index);
        void changeUpDownPosition(HWND handle, int pos);
        void toggleFov();
        void toggleGrid();

    protected:

    private:
        void updateAngle();
        void updatePosition();
        void updateTarget();
        void updateMatrix();
        void updateQuaternion();

        ModelGL* model;
        HWND parentHandle;
        float positionScale;
        float targetScale;

        // controls
        Win::Trackbar sliderAngleX;             // pitch
        Win::Trackbar sliderAngleY;             // yaw
        Win::Trackbar sliderAngleZ;             // roll
        Win::Trackbar sliderPositionX;          // camera position x
        Win::Trackbar sliderPositionY;          // camera position y
        Win::Trackbar sliderPositionZ;          // camera position z
        Win::Trackbar sliderTargetX;            // target x
        Win::Trackbar sliderTargetY;            // target y
        Win::Trackbar sliderTargetZ;            // target z

        Win::Button buttonReset;
        Win::Button buttonAbout;

        Win::TextBox  labelAngleX;              // angle
        Win::TextBox  labelAngleY;
        Win::TextBox  labelAngleZ;
        Win::TextBox  labelPositionX;           // position
        Win::TextBox  labelPositionY;
        Win::TextBox  labelPositionZ;
        Win::TextBox  labelTargetX;             // target
        Win::TextBox  labelTargetY;
        Win::TextBox  labelTargetZ;
        Win::TextBox  labelMatrix00;            // matrix elements
        Win::TextBox  labelMatrix01;
        Win::TextBox  labelMatrix02;
        Win::TextBox  labelMatrix03;
        Win::TextBox  labelMatrix04;
        Win::TextBox  labelMatrix05;
        Win::TextBox  labelMatrix06;
        Win::TextBox  labelMatrix07;
        Win::TextBox  labelMatrix08;
        Win::TextBox  labelMatrix09;
        Win::TextBox  labelMatrix10;
        Win::TextBox  labelMatrix11;
        Win::TextBox  labelMatrix12;
        Win::TextBox  labelMatrix13;
        Win::TextBox  labelMatrix14;
        Win::TextBox  labelMatrix15;

        Win::TextBox  labelQuaternionS;         // matrix elements
        Win::TextBox  labelQuaternionX;
        Win::TextBox  labelQuaternionY;
        Win::TextBox  labelQuaternionZ;

		Win::CheckBox  checkGrid;
        Win::CheckBox  checkFov;
        Win::EditBox   editFov;
        Win::UpDownBox spinFov;

        //Win::SysLink  linkTest;
    };
}

#endif
