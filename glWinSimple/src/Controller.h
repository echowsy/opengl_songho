///////////////////////////////////////////////////////////////////////////////
// Controller.h
// ============
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

#ifndef WIN_CONTROLLER_H
#define WIN_CONTROLLER_H

#include <windows.h>


namespace Win
{
    class Controller
    {
    public:
        // ctor/dtor
        Controller();
        virtual ~Controller ();

        HWND getHandle();                                       // get window handle
        void setHandle(HWND handle);                            // set window handle

        virtual LRESULT close();                                    // for WM_CLOSE
        virtual LRESULT command(int id, int cmd, LPARAM msg);       // for WM_COMMAND: id, cmd, msg
        virtual LRESULT contextMenu(HWND handle, int x, int y);     // for WM_CONTEXTMENU: handle, x, y
        virtual LRESULT create();                                   // for WM_CREATE
        virtual LRESULT ctlColorBtn(HDC hdc, HWND handle);          // for WM_CTLCOLORBTN
        virtual LRESULT ctlColorDlg(HDC hdc, HWND handle);          // for WM_CTLCOLORDLG
        virtual LRESULT ctlColorEdit(HDC hdc, HWND handle);         // for WM_CTLCOLOREDIT
        virtual LRESULT ctlColorListBox(HDC hdc, HWND handle);      // for WM_CTLCOLORLISTBOX
        virtual LRESULT ctlColorScrollBar(HDC hdc, HWND handle);    // for WM_CTLCOLORSCROLLBAR
        virtual LRESULT ctlColorStatic(HDC hdc, HWND handle);       // for WM_CTLCOLORSTATIC
        virtual LRESULT destroy();                                  // for WM_DESTROY
        virtual LRESULT dpiChanged(int x, int y, RECT* rect);       // for WM_DPICHANGED: xDpi, yDpi, rect
        virtual LRESULT enable(bool flag);                          // for WM_ENABLE: flag(true/false)
        virtual LRESULT eraseBkgnd(HDC hdc);                        // for WM_ERASEBKGND: HDC
        virtual LRESULT hScroll(WPARAM wParam, LPARAM lParam);      // for WM_HSCROLL
        virtual LRESULT keyDown(int key, LPARAM lParam);            // for WM_KEYDOWN: keyCode, detailInfo
        virtual LRESULT keyUp(int key, LPARAM lParam);              // for WM_KEYUP: keyCode, detailInfo
        virtual LRESULT lButtonDown(WPARAM state, int x, int y);    // for WM_LBUTTONDOWN: state, x, y
        virtual LRESULT lButtonUp(WPARAM state, int x, int y);      // for WM_LBUTTONUP: state, x, y
        virtual LRESULT mButtonDown(WPARAM state, int x, int y);    // for WM_MBUTTONDOWN: state, x, y
        virtual LRESULT mButtonUp(WPARAM state, int x, int y);      // for WM_MBUTTONUP: state, x, y
        virtual LRESULT mouseHover(int state, int x, int y);        // for WM_MOUSEHOVER: state, x, y
        virtual LRESULT mouseLeave();                               // for WM_MOUSELEAVE
        virtual LRESULT mouseMove(WPARAM state, int x, int y);      // for WM_MOUSEMOVE: state, x, y
        virtual LRESULT mouseWheel(int state, int d, int x, int y); // for WM_MOUSEWHEEL: state, delta, x, y
        virtual LRESULT notify(int id, LPARAM lParam);              // for WM_NOTIFY: controllerID, NMHDR
        virtual LRESULT paint();                                    // for WM_PAINT
        virtual LRESULT rButtonDown(WPARAM wParam, int x, int y);   // for WM_RBUTTONDOWN: state, x, y
        virtual LRESULT rButtonUp(WPARAM wParam, int x, int y);     // for WM_RBUTTONUP: state, x, y
        virtual LRESULT setCursor(HWND handle, int hit, int msgId); // for WM_SETCURSOR: handle, hit-test, ID of mouse event(move, down, etc)
        virtual LRESULT size(int w, int h, WPARAM wParam);          // for WM_SIZE: width, height, type(SIZE_MAXIMIZED...)
        virtual LRESULT timer(WPARAM id, LPARAM lParam);            // for WM_TIMER: ID, ptr to callback ftn
        virtual LRESULT vScroll(WPARAM wParam, LPARAM lParam);      // for WM_VSCROLL


    protected:
        HWND handle;                                            // window handle to map window to controller

    private:
        // Controller class must reference to Model and View components.
        // Controller receives an event from the user and translates it to
        // Model and View to perform actions based on that input.
        //Model model;
        //View  view;
    };

    ///////////////////////////////////////////////////////////////////////////
    // inline functions
    ///////////////////////////////////////////////////////////////////////////
    inline HWND Controller::getHandle() { return handle; }
    inline void Controller::setHandle(HWND hwnd) { handle = hwnd; }
    inline LRESULT Controller::close() { ::DestroyWindow(handle); return 0; }
    inline LRESULT Controller::command(int id, int cmd, LPARAM msg) { return 0; }
    inline LRESULT Controller::contextMenu(HWND handle, int x, int y) { return 0; }
    inline LRESULT Controller::create() { return 0; }
    inline LRESULT Controller::ctlColorBtn(HDC hdc, HWND handle) { return 0; }
    inline LRESULT Controller::ctlColorDlg(HDC hdc, HWND handle) { return 0; }
    inline LRESULT Controller::ctlColorEdit(HDC hdc, HWND handle) {return 0; }
    inline LRESULT Controller::ctlColorListBox(HDC hdc, HWND handle) { return 0; }
    inline LRESULT Controller::ctlColorScrollBar(HDC hdc, HWND handle) { return 0; }
    inline LRESULT Controller::ctlColorStatic(HDC hdc, HWND handle) { return 0; }
    inline LRESULT Controller::destroy() { return 0; }
    inline LRESULT Controller::dpiChanged(int x, int y, RECT* rect) { return 0; }
    inline LRESULT Controller::enable(bool flag) { return 0; }
    inline LRESULT Controller::eraseBkgnd(HDC hdc) { return 0; }
    inline LRESULT Controller::hScroll(WPARAM wParam, LPARAM lParam) { return 0; }
    inline LRESULT Controller::keyDown(int key, LPARAM lParam) { return 0; }
    inline LRESULT Controller::keyUp(int key, LPARAM lParam) { return 0; }
    inline LRESULT Controller::lButtonDown(WPARAM wParam, int x, int y) { return 0; }
    inline LRESULT Controller::lButtonUp(WPARAM wParam, int x, int y) { return 0; }
    inline LRESULT Controller::mButtonDown(WPARAM wParam, int x, int y) { return 0; }
    inline LRESULT Controller::mButtonUp(WPARAM wParam, int x, int y) { return 0; }
    inline LRESULT Controller::mouseHover(int state, int x, int y) { return 0; }
    inline LRESULT Controller::mouseLeave() { return 0; }
    inline LRESULT Controller::mouseMove(WPARAM keyState, int x, int y) { return 0; }
    inline LRESULT Controller::mouseWheel(int state, int delta, int x, int y) { return 0; }
    inline LRESULT Controller::notify(int id, LPARAM lParam) { return 0; }
    inline LRESULT Controller::paint() { return 0; }
    inline LRESULT Controller::rButtonDown(WPARAM wParam, int x, int y) { return 0; }
    inline LRESULT Controller::rButtonUp(WPARAM wParam, int x, int y) { return 0; }
    inline LRESULT Controller::setCursor(HWND handle, int hitTest, int msgId) { return 0; }
    inline LRESULT Controller::size(int w, int h, WPARAM type) { return 0; }
    inline LRESULT Controller::timer(WPARAM id, LPARAM lParam) { return 0; }
    inline LRESULT Controller::vScroll(WPARAM wParam, LPARAM lParam) { return 0; }

}
#endif
