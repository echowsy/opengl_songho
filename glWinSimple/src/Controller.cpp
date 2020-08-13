///////////////////////////////////////////////////////////////////////////////
// Controller.cpp
// ==============
// a base class of Windows Controller component
// This class is a bridge between window procedure and the application.
// Controller interacts with the events of the application. It receives the
// input from the user and translates it to Model and View in order to perform
// actions based on that input. Controller informs Model with a setter
// function in order to update its associated value. And, it also notifies
// View to update visuals.
//
// This class does not handle any message here, therefore the client must
// create a derived class and handle the specific messages.
//
// LRESULT type is 64bit in _WIN64 and 32bit in _WIN32.
//
//  AUTHOR: Song Ho Ahn (song.ahn@gmail.com)
// CREATED: 2005-03-31
// UPDATED: 2020-01-14
///////////////////////////////////////////////////////////////////////////////

#include "Controller.h"
using namespace Win;




///////////////////////////////////////////////////////////////////////////////
// default ctor
///////////////////////////////////////////////////////////////////////////////
Controller::Controller() : handle(0)
{
}



///////////////////////////////////////////////////////////////////////////////
// dtor
///////////////////////////////////////////////////////////////////////////////
Controller::~Controller()
{
    ::DestroyWindow(handle);
}



