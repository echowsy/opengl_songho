///////////////////////////////////////////////////////////////////////////////
// ViewForm.cpp
// ============
// View component of dialog window
//
//  AUTHORL Song Ho Ahn (song.ahn@gmail.com)
// CREATED: 2016-05-29
// UPDATED: 2016-07-12
///////////////////////////////////////////////////////////////////////////////

#include <sstream>
#include <iomanip>
#include <cmath>
#include "ViewForm.h"
#include "resource.h"
#include "Log.h"
#include "wcharUtil.h"
#include "Vectors.h"
#include "Matrices.h"
#include "Quaternion.h"
using namespace Win;


const int UPDOWN_LOW  = 10;
const int UPDOWN_HIGH = 100;



///////////////////////////////////////////////////////////////////////////////
// default ctor
///////////////////////////////////////////////////////////////////////////////
ViewForm::ViewForm(ModelGL* model)
    : model(model), parentHandle(0), positionScale(0.1f), targetScale(0.1f)
{
}


///////////////////////////////////////////////////////////////////////////////
// default dtor
///////////////////////////////////////////////////////////////////////////////
ViewForm::~ViewForm()
{
}



///////////////////////////////////////////////////////////////////////////////
// initialize all controls
///////////////////////////////////////////////////////////////////////////////
void ViewForm::initControls(HWND handle)
{
    // remember the handle to parent window
    parentHandle = handle;

    // set all controls
    sliderAngleX.set(handle, IDC_SLIDER_PITCH);
    sliderAngleX.setRange(0, 360);
    sliderAngleY.set(handle, IDC_SLIDER_YAW);
    sliderAngleY.setRange(0, 360);
    sliderAngleZ.set(handle, IDC_SLIDER_ROLL);
    sliderAngleZ.setRange(0, 360);
    labelAngleX.set(handle, IDC_LABEL_PITCH);
    labelAngleY.set(handle, IDC_LABEL_YAW);
    labelAngleZ.set(handle, IDC_LABEL_ROLL);
    updateAngle();

    sliderPositionX.set(handle, IDC_SLIDER_POSX);
    sliderPositionX.setRange(0, 200);
    sliderPositionY.set(handle, IDC_SLIDER_POSY);
    sliderPositionY.setRange(0, 200);
    sliderPositionZ.set(handle, IDC_SLIDER_POSZ);
    sliderPositionZ.setRange(0, 200);
    labelPositionX.set(handle, IDC_LABEL_POSX);
    labelPositionY.set(handle, IDC_LABEL_POSY);
    labelPositionZ.set(handle, IDC_LABEL_POSZ);
    updatePosition();

    sliderTargetX.set(handle, IDC_SLIDER_TARX);
    sliderTargetX.setRange(0, 200);
    sliderTargetY.set(handle, IDC_SLIDER_TARY);
    sliderTargetY.setRange(0, 200);
    sliderTargetZ.set(handle, IDC_SLIDER_TARZ);
    sliderTargetZ.setRange(0, 200);
    labelTargetX.set(handle, IDC_LABEL_TARX);
    labelTargetY.set(handle, IDC_LABEL_TARY);
    labelTargetZ.set(handle, IDC_LABEL_TARZ);
    updateTarget();

    buttonReset.set(handle, IDC_BUTTON_RESET);
    buttonAbout.set(handle, IDC_BUTTON_ABOUT);
    buttonAbout.setImage(::LoadIcon(0, IDI_INFORMATION));

    labelMatrix00.set(handle, IDC_LABEL_M00);
    labelMatrix01.set(handle, IDC_LABEL_M01);
    labelMatrix02.set(handle, IDC_LABEL_M02);
    labelMatrix03.set(handle, IDC_LABEL_M03);
    labelMatrix04.set(handle, IDC_LABEL_M04);
    labelMatrix05.set(handle, IDC_LABEL_M05);
    labelMatrix06.set(handle, IDC_LABEL_M06);
    labelMatrix07.set(handle, IDC_LABEL_M07);
    labelMatrix08.set(handle, IDC_LABEL_M08);
    labelMatrix09.set(handle, IDC_LABEL_M09);
    labelMatrix10.set(handle, IDC_LABEL_M10);
    labelMatrix11.set(handle, IDC_LABEL_M11);
    labelMatrix12.set(handle, IDC_LABEL_M12);
    labelMatrix13.set(handle, IDC_LABEL_M13);
    labelMatrix14.set(handle, IDC_LABEL_M14);
    labelMatrix15.set(handle, IDC_LABEL_M15);
    updateMatrix();

    labelQuaternionS.set(handle, IDC_LABEL_QS);
    labelQuaternionX.set(handle, IDC_LABEL_QX);
    labelQuaternionY.set(handle, IDC_LABEL_QY);
    labelQuaternionZ.set(handle, IDC_LABEL_QZ);
    updateQuaternion();

    checkGrid.set(handle, IDC_CHECK_GRID);
    checkGrid.check();

    checkFov.set(handle, IDC_CHECK_FOV);
    checkFov.check();
    editFov.set(handle, IDC_EDIT_FOV);
    editFov.setText(L"50");
    spinFov.set(handle, IDC_SPIN_FOV);
    spinFov.setRange(UPDOWN_LOW, UPDOWN_HIGH);
    spinFov.setPos(50);
    spinFov.setBuddy(editFov.getHandle());

    //@@TEST
    //linkTest.set(handle, IDC_LINK1);
    /*
    wchar_t buff[1024];
    memset(buff, 0, sizeof(wchar_t)*1024);
    linkTest.getText(buff, 1024);
    Win::log(L"SysLink: %ls", buff);
    linkTest.setUrl(0, L"http://www.google.ca");
    linkTest.getUrl(0, buff);
    Win::log("SysLink URL: %ls", buff);
    */
}



///////////////////////////////////////////////////////////////////////////////
// update trackbars
///////////////////////////////////////////////////////////////////////////////
void ViewForm::updateTrackbars(HWND handle, int position)
{
    if(handle == sliderAngleX.getHandle())
    {
        sliderAngleX.setPos(position);
        labelAngleX.setText(toWchar((long)position - 180));
        model->setCameraAngleX((float)(position - 180));
        updatePosition();
    }
    else if(handle == sliderAngleY.getHandle())
    {
        sliderAngleY.setPos(position);
        labelAngleY.setText(toWchar((long)position - 180));
        model->setCameraAngleY((float)(position - 180));
        updatePosition();
    }
    else if(handle == sliderAngleZ.getHandle())
    {
        sliderAngleZ.setPos(position);
        labelAngleZ.setText(toWchar((long)position - 180));
        model->setCameraAngleZ((float)(position - 180));
        updatePosition();
    }
    else if(handle == sliderPositionX.getHandle())
    {
        sliderPositionX.setPos(position);
        float value = (position - 100) * positionScale;
        labelPositionX.setText(toWchar(value));
        model->setCameraPositionX(value);
        updateAngle();
    }
    else if(handle == sliderPositionY.getHandle())
    {
        sliderPositionY.setPos(position);
        float value = (position - 100) * positionScale;
        labelPositionY.setText(toWchar(value));
        model->setCameraPositionY(value);
        updateAngle();
    }
    else if(handle == sliderPositionZ.getHandle())
    {
        sliderPositionZ.setPos(position);
        float value = (position - 100) * positionScale;
        labelPositionZ.setText(toWchar(value));
        model->setCameraPositionZ(value);
        updateAngle();
    }
    else if(handle == sliderTargetX.getHandle())
    {
        sliderTargetX.setPos(position);
        float value = (position - 100) * targetScale;
        labelTargetX.setText(toWchar(value));
        model->setCameraTargetX(value);
        updatePosition();
    }
    else if(handle == sliderTargetY.getHandle())
    {
        sliderTargetY.setPos(position);
        float value = (position - 100) * targetScale;
        labelTargetY.setText(toWchar(value));
        model->setCameraTargetY(value);
        updatePosition();
    }
    else if(handle == sliderTargetZ.getHandle())
    {
        sliderTargetZ.setPos(position);
        float value = (position - 100) * targetScale;
        labelTargetZ.setText(toWchar(value));
        model->setCameraTargetZ(value);
        updatePosition();
    }

    updateMatrix();
    updateQuaternion();
}



///////////////////////////////////////////////////////////////////////////////
// reset controls
///////////////////////////////////////////////////////////////////////////////
void ViewForm::resetControls()
{
    updateAngle();
    updatePosition();
    updateTarget();
    updateMatrix();
    updateQuaternion();

    checkGrid.check();
    model->enableGrid();

    checkFov.check();
    spinFov.setPos(50);
    spinFov.enable();
    editFov.enable();
    model->enableFov();
}



///////////////////////////////////////////////////////////////////////////////
// update controls for angle
///////////////////////////////////////////////////////////////////////////////
void ViewForm::updateAngle()
{
    Vector3 angle = model->getCameraAngle();

    sliderAngleX.setPos(180 + (int)(angle.x + 0.5f));
    sliderAngleY.setPos(180 + (int)(angle.y + 0.5f));
    sliderAngleZ.setPos(180 + (int)(angle.z + 0.5f));

    labelAngleX.setText(toWchar((int)(angle.x + 0.5f)));
    labelAngleY.setText(toWchar((int)(angle.y + 0.5f)));
    labelAngleZ.setText(toWchar((int)(angle.z + 0.5f)));
}



///////////////////////////////////////////////////////////////////////////////
// update controls for position
///////////////////////////////////////////////////////////////////////////////
void ViewForm::updatePosition()
{
    std::wstringstream wss;
    wss << std::fixed << std::setprecision(1);

    Vector3 position = model->getCameraPosition();

    sliderPositionX.setPos(100 + (int)(position.x / positionScale + 0.5f));
    sliderPositionY.setPos(100 + (int)(position.y / positionScale + 0.5f));
    sliderPositionZ.setPos(100 + (int)(position.z / positionScale + 0.5f));

    wss << position.x;
    labelPositionX.setText(wss.str().c_str());
    wss.str(L"");
    wss << position.y;
    labelPositionY.setText(wss.str().c_str());
    wss.str(L"");
    wss << position.z;
    labelPositionZ.setText(wss.str().c_str());

    // unset floating format
    wss << std::resetiosflags(std::ios_base::fixed | std::ios_base::floatfield);
}



///////////////////////////////////////////////////////////////////////////////
// update controls for target
///////////////////////////////////////////////////////////////////////////////
void ViewForm::updateTarget()
{
    std::wstringstream wss;
    wss << std::fixed << std::setprecision(1);

    Vector3 target = model->getCameraTarget();

    sliderTargetX.setPos(100 + (int)(target.x / targetScale + 0.5f));
    sliderTargetY.setPos(100 + (int)(target.y / targetScale + 0.5f));
    sliderTargetZ.setPos(100 + (int)(target.z / targetScale + 0.5f));

    wss << target.x;
    labelTargetX.setText(wss.str().c_str());
    wss.str(L"");
    wss << target.y;
    labelTargetY.setText(wss.str().c_str());
    wss.str(L"");
    wss << target.z;
    labelTargetZ.setText(wss.str().c_str());

    // unset floating format
    wss << std::resetiosflags(std::ios_base::fixed | std::ios_base::floatfield);
}



///////////////////////////////////////////////////////////////////////////////
// update matrix elements
///////////////////////////////////////////////////////////////////////////////
void ViewForm::updateMatrix()
{
    std::wstringstream wss;
    wss << std::fixed << std::setprecision(3);

    Matrix4 m = model->getCameraMatrix();
    wss << m[0];
    labelMatrix00.setText(wss.str().c_str());

    wss.str(L"");
    wss << m[1];
    labelMatrix01.setText(wss.str().c_str());

    wss.str(L"");
    wss << m[2];
    labelMatrix02.setText(wss.str().c_str());

    wss.str(L"");
    wss << m[3];
    labelMatrix03.setText(wss.str().c_str());

    wss.str(L"");
    wss << m[4];
    labelMatrix04.setText(wss.str().c_str());

    wss.str(L"");
    wss << m[5];
    labelMatrix05.setText(wss.str().c_str());

    wss.str(L"");
    wss << m[6];
    labelMatrix06.setText(wss.str().c_str());

    wss.str(L"");
    wss << m[7];
    labelMatrix07.setText(wss.str().c_str());

    wss.str(L"");
    wss << m[8];
    labelMatrix08.setText(wss.str().c_str());

    wss.str(L"");
    wss << m[9];
    labelMatrix09.setText(wss.str().c_str());

    wss.str(L"");
    wss << m[10];
    labelMatrix10.setText(wss.str().c_str());

    wss.str(L"");
    wss << m[11];
    labelMatrix11.setText(wss.str().c_str());

    wss.str(L"");
    wss << m[12];
    labelMatrix12.setText(wss.str().c_str());

    wss.str(L"");
    wss << m[13];
    labelMatrix13.setText(wss.str().c_str());

    wss.str(L"");
    wss << m[14];
    labelMatrix14.setText(wss.str().c_str());

    wss.str(L"");
    wss << m[15];
    labelMatrix15.setText(wss.str().c_str());

    // unset floating format
    wss << std::resetiosflags(std::ios_base::fixed | std::ios_base::floatfield);
}



///////////////////////////////////////////////////////////////////////////////
// update qiaternion
///////////////////////////////////////////////////////////////////////////////
void ViewForm::updateQuaternion()
{
    std::wstringstream wss;
    wss << std::fixed << std::setprecision(3);

    Quaternion q = model->getCameraQuaternion();
    wss << q.s;
    labelQuaternionS.setText(wss.str().c_str());

    wss.str(L"");
    wss << q.x;
    labelQuaternionX.setText(wss.str().c_str());

    wss.str(L"");
    wss << q.y;
    labelQuaternionY.setText(wss.str().c_str());

    wss.str(L"");
    wss << q.z;
    labelQuaternionZ.setText(wss.str().c_str());

    // unset floating format
    wss << std::resetiosflags(std::ios_base::fixed | std::ios_base::floatfield);
}



/*
///////////////////////////////////////////////////////////////////////////////
// handle SysLink event
///////////////////////////////////////////////////////////////////////////////
void ViewForm::handleLink(HWND handle, int index)
{
    if(handle == linkTest.getHandle())
    {
        // open url
        wchar_t url[1024];
        memset(url, 0, sizeof(wchar_t)*1024);
        linkTest.getUrl(index, url);
        ::ShellExecute(0, L"open", url, 0, 0, SW_SHOW);
        Win::log(L"URL: %ls", url);
    }
}
*/



///////////////////////////////////////////////////////////////////////////////
// change updown position
///////////////////////////////////////////////////////////////////////////////
void ViewForm::changeUpDownPosition(HWND handle, int position)
{
    if(handle == spinFov.getHandle())
    {
        if(position >= UPDOWN_LOW && position <= UPDOWN_HIGH)
        {
            model->setFov((float)position);
        }
    }
}



///////////////////////////////////////////////////////////////////////////////
// toggle on/off FOV control
///////////////////////////////////////////////////////////////////////////////
void ViewForm::toggleFov()
{
    if(checkFov.isChecked())
    {
        spinFov.enable();
        editFov.enable();
        model->enableFov();
    }
    else
    {
        spinFov.disable();
        editFov.disable();
        model->disableFov();
    }
}
void ViewForm::toggleGrid()
{
    if(checkGrid.isChecked())
    {
        model->enableGrid();
    }
    else
    {
        model->disableGrid();
    }
}


