///////////////////////////////////////////////////////////////////////////////
// ViewGL.h
// ========
// View component of OpenGL window
//
//  AUTHORL Song Ho Ahn (song.ahn@gmail.com)
// CREATED: 2016-05-29
// UPDATED: 2016-07-18
///////////////////////////////////////////////////////////////////////////////

#ifndef VIEW_GL_H
#define VIEW_GL_H

#include <windows.h>

namespace Win
{
    class ViewGL
    {
    public:
        ViewGL();
        ~ViewGL();

        bool createContext(HWND handle, int colorBits, int depthBits, int stencilBits, int msaaSamples=0);  // create OpenGL rendering context
        bool setContext(HWND handle, HGLRC rc, int pixelFormat);
        void closeContext(HWND handle);
        void activateContext();
        void swapBuffers();

        HDC getDC() const                   { return hdc; }
        HGLRC getRC() const                 { return hglrc; }

    protected:

    private:
        // member functions
        static bool setPixelFormat(HDC hdc, int colorBits, int depthBits, int stencilBits);
        static int findPixelFormat(HDC hdc, int colorbits, int depthBits, int stencilBits); // return best matched format ID
        static int findPixelFormatWithAttributes(int colorbits, int depthBits, int stencilBits, int msaaSamples);

        HDC hdc;                                        // handle to device context
        HGLRC hglrc;                                    // handle to OpenGL rendering context

    };
}

#endif
