///////////////////////////////////////////////////////////////////////////////
// glExtension.h
// =============
// OpenGL extension helper
// NOTE: The size of HDC (void*) in 64bit Windows is 8 bytes.
//
//  AUTHOR: Song Ho Ahn (song.ahn@gmail.com)
// CREATED: 2013-03-05
// UPDATED: 2017-11-07
///////////////////////////////////////////////////////////////////////////////

#ifndef GL_EXTENSION_H
#define GL_EXTENSION_H

// in order to get function prototypes from glext.h, define GL_GLEXT_PROTOTYPES before including glext.h
#define GL_GLEXT_PROTOTYPES

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 1
#endif
#include <windows.h>
#endif

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include <string>
#include <vector>
#include "glext.h"

class glExtension
{
public:
    ~glExtension();
    static glExtension& getInstance(void* param=0);         // must be called after RC is open

    bool isSupported(const std::string& extStr);            // check if a extension is available
    const std::vector<std::string>& getExtensions();

private:
    glExtension(void* param=0);                             // prevent calling ctor
    glExtension(const glExtension& rhs);                    // no implementation
    void getExtensionStrings();
    void getFunctionPointers();
    std::string toLower(const std::string& str);

    std::vector <std::string> extensions;
    void* hdc;                                              // for win32 HDC (void*) only
};



#ifdef _WIN32 //===============================================================
// GL_ARB_framebuffer_object
extern PFNGLGENFRAMEBUFFERSPROC                     pglGenFramebuffers;                     // FBO name generation procedure
extern PFNGLDELETEFRAMEBUFFERSPROC                  pglDeleteFramebuffers;                  // FBO deletion procedure
extern PFNGLBINDFRAMEBUFFERPROC                     pglBindFramebuffer;                     // FBO bind procedure
extern PFNGLCHECKFRAMEBUFFERSTATUSPROC              pglCheckFramebufferStatus;              // FBO completeness test procedure
extern PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC pglGetFramebufferAttachmentParameteriv; // return various FBO parameters
extern PFNGLGENERATEMIPMAPPROC                      pglGenerateMipmap;                      // FBO automatic mipmap generation procedure
extern PFNGLFRAMEBUFFERTEXTURE1DPROC                pglFramebufferTexture1D;                // FBO texture attachement procedure
extern PFNGLFRAMEBUFFERTEXTURE2DPROC                pglFramebufferTexture2D;                // FBO texture attachement procedure
extern PFNGLFRAMEBUFFERTEXTURE3DPROC                pglFramebufferTexture3D;                // FBO texture attachement procedure
extern PFNGLFRAMEBUFFERTEXTURELAYERPROC             pglFramebufferTextureLayer;             // FBO texture layer procedure
extern PFNGLFRAMEBUFFERRENDERBUFFERPROC             pglFramebufferRenderbuffer;             // FBO renderbuffer attachement procedure
extern PFNGLISFRAMEBUFFERPROC                       pglIsFramebuffer;                       // FBO state = true/false
extern PFNGLBLITFRAMEBUFFERPROC                     pglBlitFramebuffer;                     // FBO copy
extern PFNGLGENRENDERBUFFERSPROC                    pglGenRenderbuffers;                    // renderbuffer generation procedure
extern PFNGLDELETERENDERBUFFERSPROC                 pglDeleteRenderbuffers;                 // renderbuffer deletion procedure
extern PFNGLBINDRENDERBUFFERPROC                    pglBindRenderbuffer;                    // renderbuffer bind procedure
extern PFNGLRENDERBUFFERSTORAGEPROC                 pglRenderbufferStorage;                 // renderbuffer memory allocation procedure
extern PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC      pglRenderbufferStorageMultisample;      // renderbuffer memory allocation with multisample
extern PFNGLGETRENDERBUFFERPARAMETERIVPROC          pglGetRenderbufferParameteriv;          // return various renderbuffer parameters
extern PFNGLISRENDERBUFFERPROC                      pglIsRenderbuffer;                      // determine renderbuffer object type
#define glGenFramebuffers                           pglGenFramebuffers
#define glDeleteFramebuffers                        pglDeleteFramebuffers
#define glBindFramebuffer                           pglBindFramebuffer
#define glCheckFramebufferStatus                    pglCheckFramebufferStatus
#define glGetFramebufferAttachmentParameteriv       pglGetFramebufferAttachmentParameteriv
#define glGenerateMipmap                            pglGenerateMipmap
#define glFramebufferTexture1D                      pglFramebufferTexture1D
#define glFramebufferTexture2D                      pglFramebufferTexture2D
#define glFramebufferTexture3D                      pglFramebufferTexture3D
#define glFramebufferTextureLayer                   pglFramebufferTextureLayer
#define glFramebufferRenderbuffer                   pglFramebufferRenderbuffer
#define glIsFramebuffer                             pglIsFramebuffer
#define glBlitFramebuffer                           pglBlitFramebuffer
#define glGenRenderbuffers                          pglGenRenderbuffers
#define glDeleteRenderbuffers                       pglDeleteRenderbuffers
#define glBindRenderbuffer                          pglBindRenderbuffer
#define glRenderbufferStorage                       pglRenderbufferStorage
#define glRenderbufferStorageMultisample            pglRenderbufferStorageMultisample
#define glGetRenderbufferParameteriv                pglGetRenderbufferParameteriv
#define glIsRenderbuffer                            pglIsRenderbuffer

// GL_ARB_multisample
extern PFNGLSAMPLECOVERAGEARBPROC   pglSampleCoverageARB;
#define glSampleCoverageARB         pglSampleCoverageARB

// GL_ARB_multitexture
extern PFNGLACTIVETEXTUREARBPROC    pglActiveTextureARB;
#define glActiveTextureARB          pglActiveTextureARB

// GL_ARB_pixel_buffer_objects & GL_ARB_vertex_buffer_object
extern PFNGLGENBUFFERSARBPROC           pglGenBuffersARB;           // VBO Name Generation Procedure
extern PFNGLBINDBUFFERARBPROC           pglBindBufferARB;           // VBO Bind Procedure
extern PFNGLBUFFERDATAARBPROC           pglBufferDataARB;           // VBO Data Loading Procedure
extern PFNGLBUFFERSUBDATAARBPROC        pglBufferSubDataARB;        // VBO Sub Data Loading Procedure
extern PFNGLDELETEBUFFERSARBPROC        pglDeleteBuffersARB;        // VBO Deletion Procedure
extern PFNGLGETBUFFERPARAMETERIVARBPROC pglGetBufferParameterivARB; // return various parameters of VBO
extern PFNGLMAPBUFFERARBPROC            pglMapBufferARB;            // map VBO procedure
extern PFNGLUNMAPBUFFERARBPROC          pglUnmapBufferARB;          // unmap VBO procedure
#define glGenBuffersARB                 pglGenBuffersARB
#define glBindBufferARB                 pglBindBufferARB
#define glBufferDataARB                 pglBufferDataARB
#define glBufferSubDataARB              pglBufferSubDataARB
#define glDeleteBuffersARB              pglDeleteBuffersARB
#define glGetBufferParameterivARB       pglGetBufferParameterivARB
#define glMapBufferARB                  pglMapBufferARB
#define glUnmapBufferARB                pglUnmapBufferARB

// GL_ARB_shader_objects
extern PFNGLDELETEOBJECTARBPROC         pglDeleteObjectARB;         // delete shader object
extern PFNGLGETHANDLEARBPROC            pglGetHandleARB;            // return handle of program
extern PFNGLDETACHOBJECTARBPROC         pglDetachObjectARB;         // detatch a shader from a program
extern PFNGLCREATESHADEROBJECTARBPROC   pglCreateShaderObjectARB;   // create a shader
extern PFNGLSHADERSOURCEARBPROC         pglShaderSourceARB;         // set a shader source(codes)
extern PFNGLCOMPILESHADERARBPROC        pglCompileShaderARB;        // compile shader source
extern PFNGLCREATEPROGRAMOBJECTARBPROC  pglCreateProgramObjectARB;  // create a program
extern PFNGLATTACHOBJECTARBPROC         pglAttachObjectARB;         // attach a shader to a program
extern PFNGLLINKPROGRAMARBPROC          pglLinkProgramARB;          // link a program
extern PFNGLUSEPROGRAMOBJECTARBPROC     pglUseProgramObjectARB;     // use a program
extern PFNGLVALIDATEPROGRAMARBPROC      pglValidateProgramARB;      // validate a program
extern PFNGLUNIFORM1FARBPROC            pglUniform1fARB;            //
extern PFNGLUNIFORM2FARBPROC            pglUniform2fARB;            //
extern PFNGLUNIFORM3FARBPROC            pglUniform3fARB;            //
extern PFNGLUNIFORM4FARBPROC            pglUniform4fARB;            //
extern PFNGLUNIFORM1IARBPROC            pglUniform1iARB;            //
extern PFNGLUNIFORM2IARBPROC            pglUniform2iARB;            //
extern PFNGLUNIFORM3IARBPROC            pglUniform3iARB;            //
extern PFNGLUNIFORM4IARBPROC            pglUniform4iARB;            //
extern PFNGLUNIFORM1FVARBPROC           pglUniform1fvARB;           //
extern PFNGLUNIFORM2FVARBPROC           pglUniform2fvARB;           //
extern PFNGLUNIFORM3FVARBPROC           pglUniform3fvARB;           //
extern PFNGLUNIFORM4FVARBPROC           pglUniform4fvARB;           //
extern PFNGLUNIFORM1FVARBPROC           pglUniform1ivARB;           //
extern PFNGLUNIFORM2FVARBPROC           pglUniform2ivARB;           //
extern PFNGLUNIFORM3FVARBPROC           pglUniform3ivARB;           //
extern PFNGLUNIFORM4FVARBPROC           pglUniform4ivARB;           //
extern PFNGLUNIFORMMATRIX2FVARBPROC     pglUniformMatrix2fvARB;     //
extern PFNGLUNIFORMMATRIX3FVARBPROC     pglUniformMatrix3fvARB;     //
extern PFNGLUNIFORMMATRIX4FVARBPROC     pglUniformMatrix4fvARB;     //
extern PFNGLGETOBJECTPARAMETERFVARBPROC pglGetObjectParameterfvARB; // get shader/program param
extern PFNGLGETOBJECTPARAMETERIVARBPROC pglGetObjectParameterivARB; //
extern PFNGLGETINFOLOGARBPROC           pglGetInfoLogARB;           // get log
extern PFNGLGETATTACHEDOBJECTSARBPROC   pglGetAttachedObjectsARB;   // get attached shader to a program
extern PFNGLGETUNIFORMLOCATIONARBPROC   pglGetUniformLocationARB;   // get index of uniform var
extern PFNGLGETACTIVEUNIFORMARBPROC     pglGetActiveUniformARB;     // get info of uniform var
extern PFNGLGETUNIFORMFVARBPROC         pglGetUniformfvARB;         // get value of uniform var
extern PFNGLGETUNIFORMIVARBPROC         pglGetUniformivARB;         //
extern PFNGLGETSHADERSOURCEARBPROC      pglGetShaderSourceARB;      // get shader source codes
#define glDeleteObjectARB               pglDeleteObjectARB
#define glGetHandleARB                  pglGetHandleARB
#define glDetachObjectARB               pglDetachObjectARB
#define glCreateShaderObjectARB         pglCreateShaderObjectARB
#define glShaderSourceARB               pglShaderSourceARB
#define glCompileShaderARB              pglCompileShaderARB
#define glCreateProgramObjectARB        pglCreateProgramObjectARB
#define glAttachObjectARB               pglAttachObjectARB
#define glLinkProgramARB                pglLinkProgramARB
#define glUseProgramObjectARB           pglUseProgramObjectARB
#define glValidateProgramARB            pglValidateProgramARB
#define glUniform1fARB                  pglUniform1fARB
#define glUniform2fARB                  pglUniform2fARB
#define glUniform3fARB                  pglUniform3fARB
#define glUniform4fARB                  pglUniform4fARB
#define glUniform1iARB                  pglUniform1iARB
#define glUniform2iARB                  pglUniform2iARB
#define glUniform3iARB                  pglUniform3iARB
#define glUniform4iARB                  pglUniform4iARB
#define glUniform1fvARB                 pglUniform1fvARB
#define glUniform2fvARB                 pglUniform2fvARB
#define glUniform3fvARB                 pglUniform3fvARB
#define glUniform4fvARB                 pglUniform4fvARB
#define glUniform1ivARB                 pglUniform1ivARB
#define glUniform2ivARB                 pglUniform2ivARB
#define glUniform3ivARB                 pglUniform3ivARB
#define glUniform4ivARB                 pglUniform4ivARB
#define glUniformMatrix2fvARB           pglUniformMatrix2fvARB
#define glUniformMatrix3fvARB           pglUniformMatrix3fvARB
#define glUniformMatrix4fvARB           pglUniformMatrix4fvARB
#define glGetObjectParameterfvARB       pglGetObjectParameterfvARB
#define glGetObjectParameterivARB       pglGetObjectParameterivARB
#define glGetInfoLogARB                 pglGetInfoLogARB
#define glGetAttachedObjectsARB         pglGetAttachedObjectsARB
#define glGetUniformLocationARB         pglGetUniformLocationARB
#define glGetActiveUniformARB           pglGetActiveUniformARB
#define glGetUniformfvARB               pglGetUniformfvARB
#define glGetUniformivARB               pglGetUniformivARB
#define glGetShaderSourceARB            pglGetShaderSourceARB

// GL_ARB_sync extension
extern PFNGLFENCESYNCPROC       pglFenceSync;
extern PFNGLISSYNCPROC          pglIsSync;
extern PFNGLDELETESYNCPROC      pglDeleteSync;
extern PFNGLCLIENTWAITSYNCPROC  pglClientWaitSync;
extern PFNGLWAITSYNCPROC        pglWaitSync;
extern PFNGLGETINTEGER64VPROC   pglGetInteger64v;
extern PFNGLGETSYNCIVPROC       pglGetSynciv;
#define glFenceSync             pglFenceSync
#define glIsSync                pglIsSync
#define glDeleteSync            pglDeleteSync
#define glClientWaitSync        pglClientWaitSync
#define glWaitSync              pglWaitSync
#define glGetInteger64v         pglGetInteger64v
#define glGetSynciv             pglGetSynciv

// GL_ARB_vertex_shader and GL_ARB_fragment_shader extensions
extern PFNGLBINDATTRIBLOCATIONARBPROC   pglBindAttribLocationARB;   // bind vertex attrib var with index
extern PFNGLGETACTIVEATTRIBARBPROC      pglGetActiveAttribARB;      // get attrib value
extern PFNGLGETATTRIBLOCATIONARBPROC    pglGetAttribLocationARB;    // get lndex of attrib var
#define glBindAttribLocationARB         pglBindAttribLocationARB
#define glGetActiveAttribARB            pglGetActiveAttribARB
#define glGetAttribLocationARB          pglGetAttribLocationARB

// GL_ARB_vertex_program
extern PFNGLVERTEXATTRIB1DARBPROC               pglVertexAttrib1dARB;
extern PFNGLVERTEXATTRIB1DVARBPROC              pglVertexAttrib1dvARB;
extern PFNGLVERTEXATTRIB1FARBPROC               pglVertexAttrib1fARB;
extern PFNGLVERTEXATTRIB1FVARBPROC              pglVertexAttrib1fvARB;
extern PFNGLVERTEXATTRIB1SARBPROC               pglVertexAttrib1sARB;
extern PFNGLVERTEXATTRIB1SVARBPROC              pglVertexAttrib1svARB;
extern PFNGLVERTEXATTRIB2DARBPROC               pglVertexAttrib2dARB;
extern PFNGLVERTEXATTRIB2DVARBPROC              pglVertexAttrib2dvARB;
extern PFNGLVERTEXATTRIB2FARBPROC               pglVertexAttrib2fARB;
extern PFNGLVERTEXATTRIB2FVARBPROC              pglVertexAttrib2fvARB;
extern PFNGLVERTEXATTRIB2SARBPROC               pglVertexAttrib2sARB;
extern PFNGLVERTEXATTRIB2SVARBPROC              pglVertexAttrib2svARB;
extern PFNGLVERTEXATTRIB3DARBPROC               pglVertexAttrib3dARB;
extern PFNGLVERTEXATTRIB3DVARBPROC              pglVertexAttrib3dvARB;
extern PFNGLVERTEXATTRIB3FARBPROC               pglVertexAttrib3fARB;
extern PFNGLVERTEXATTRIB3FVARBPROC              pglVertexAttrib3fvARB;
extern PFNGLVERTEXATTRIB3SARBPROC               pglVertexAttrib3sARB;
extern PFNGLVERTEXATTRIB3SVARBPROC              pglVertexAttrib3svARB;
extern PFNGLVERTEXATTRIB4NBVARBPROC             pglVertexAttrib4NbvARB;
extern PFNGLVERTEXATTRIB4NIVARBPROC             pglVertexAttrib4NivARB;
extern PFNGLVERTEXATTRIB4NSVARBPROC             pglVertexAttrib4NsvARB;
extern PFNGLVERTEXATTRIB4NUBARBPROC             pglVertexAttrib4NubARB;
extern PFNGLVERTEXATTRIB4NUBVARBPROC            pglVertexAttrib4NubvARB;
extern PFNGLVERTEXATTRIB4NUIVARBPROC            pglVertexAttrib4NuivARB;
extern PFNGLVERTEXATTRIB4NUSVARBPROC            pglVertexAttrib4NusvARB;
extern PFNGLVERTEXATTRIB4BVARBPROC              pglVertexAttrib4bvARB;
extern PFNGLVERTEXATTRIB4DARBPROC               pglVertexAttrib4dARB;
extern PFNGLVERTEXATTRIB4DVARBPROC              pglVertexAttrib4dvARB;
extern PFNGLVERTEXATTRIB4FARBPROC               pglVertexAttrib4fARB;
extern PFNGLVERTEXATTRIB4FVARBPROC              pglVertexAttrib4fvARB;
extern PFNGLVERTEXATTRIB4IVARBPROC              pglVertexAttrib4ivARB;
extern PFNGLVERTEXATTRIB4SARBPROC               pglVertexAttrib4sARB;
extern PFNGLVERTEXATTRIB4SVARBPROC              pglVertexAttrib4svARB;
extern PFNGLVERTEXATTRIB4UBVARBPROC             pglVertexAttrib4ubvARB;
extern PFNGLVERTEXATTRIB4UIVARBPROC             pglVertexAttrib4uivARB;
extern PFNGLVERTEXATTRIB4USVARBPROC             pglVertexAttrib4usvARB;
extern PFNGLVERTEXATTRIBPOINTERARBPROC          pglVertexAttribPointerARB;
extern PFNGLENABLEVERTEXATTRIBARRAYARBPROC      pglEnableVertexAttribArrayARB;
extern PFNGLDISABLEVERTEXATTRIBARRAYARBPROC     pglDisableVertexAttribArrayARB;
extern PFNGLPROGRAMSTRINGARBPROC                pglProgramStringARB;
extern PFNGLBINDPROGRAMARBPROC                  pglBindProgramARB;
extern PFNGLDELETEPROGRAMSARBPROC               pglDeleteProgramsARB;
extern PFNGLGENPROGRAMSARBPROC                  pglGenProgramsARB;
extern PFNGLPROGRAMENVPARAMETER4DARBPROC        pglProgramEnvParameter4dARB;
extern PFNGLPROGRAMENVPARAMETER4DVARBPROC       pglProgramEnvParameter4dvARB;
extern PFNGLPROGRAMENVPARAMETER4FARBPROC        pglProgramEnvParameter4fARB;
extern PFNGLPROGRAMENVPARAMETER4FVARBPROC       pglProgramEnvParameter4fvARB;
extern PFNGLPROGRAMLOCALPARAMETER4DARBPROC      pglProgramLocalParameter4dARB;
extern PFNGLPROGRAMLOCALPARAMETER4DVARBPROC     pglProgramLocalParameter4dvARB;
extern PFNGLPROGRAMLOCALPARAMETER4FARBPROC      pglProgramLocalParameter4fARB;
extern PFNGLPROGRAMLOCALPARAMETER4FVARBPROC     pglProgramLocalParameter4fvARB;
extern PFNGLGETPROGRAMENVPARAMETERDVARBPROC     pglGetProgramEnvParameterdvARB;
extern PFNGLGETPROGRAMENVPARAMETERFVARBPROC     pglGetProgramEnvParameterfvARB;
extern PFNGLGETPROGRAMLOCALPARAMETERDVARBPROC   pglGetProgramLocalParameterdvARB;
extern PFNGLGETPROGRAMLOCALPARAMETERFVARBPROC   pglGetProgramLocalParameterfvARB;
extern PFNGLGETPROGRAMIVARBPROC                 pglGetProgramivARB;
extern PFNGLGETPROGRAMSTRINGARBPROC             pglGetProgramStringARB;
extern PFNGLGETVERTEXATTRIBDVARBPROC            pglGetVertexAttribdvARB;
extern PFNGLGETVERTEXATTRIBFVARBPROC            pglGetVertexAttribfvARB;
extern PFNGLGETVERTEXATTRIBIVARBPROC            pglGetVertexAttribivARB;
extern PFNGLGETVERTEXATTRIBPOINTERVARBPROC      pglGetVertexAttribPointervARB;
extern PFNGLISPROGRAMARBPROC                    pglIsProgramARB;
#define glVertexAttrib1dARB                     pglVertexAttrib1dARB
#define glVertexAttrib1dvARB                    pglVertexAttrib1dvARB
#define glVertexAttrib1fARB                     pglVertexAttrib1fARB
#define glVertexAttrib1fvARB                    pglVertexAttrib1fvARB
#define glVertexAttrib1sARB                     pglVertexAttrib1sARB
#define glVertexAttrib1svARB                    pglVertexAttrib1svARB
#define glVertexAttrib2dARB                     pglVertexAttrib2dARB
#define glVertexAttrib2dvARB                    pglVertexAttrib2dvARB
#define glVertexAttrib2fARB                     pglVertexAttrib2fARB
#define glVertexAttrib2fvARB                    pglVertexAttrib2fvARB
#define glVertexAttrib2sARB                     pglVertexAttrib2sARB
#define glVertexAttrib2svARB                    pglVertexAttrib2svARB
#define glVertexAttrib3dARB                     pglVertexAttrib3dARB
#define glVertexAttrib3dvARB                    pglVertexAttrib3dvARB
#define glVertexAttrib3fARB                     pglVertexAttrib3fARB
#define glVertexAttrib3fvARB                    pglVertexAttrib3fvARB
#define glVertexAttrib3sARB                     pglVertexAttrib3sARB
#define glVertexAttrib3svARB                    pglVertexAttrib3svARB
#define glVertexAttrib4NbvARB                   pglVertexAttrib4NbvARB
#define glVertexAttrib4NivARB                   pglVertexAttrib4NivARB
#define glVertexAttrib4NsvARB                   pglVertexAttrib4NsvARB
#define glVertexAttrib4NubARB                   pglVertexAttrib4NubARB
#define glVertexAttrib4NubvARB                  pglVertexAttrib4NubvARB
#define glVertexAttrib4NuivARB                  pglVertexAttrib4NuivARB
#define glVertexAttrib4NusvARB                  pglVertexAttrib4NusvARB
#define glVertexAttrib4bvARB                    pglVertexAttrib4bvARB
#define glVertexAttrib4dARB                     pglVertexAttrib4dARB
#define glVertexAttrib4dvARB                    pglVertexAttrib4dvARB
#define glVertexAttrib4fARB                     pglVertexAttrib4fARB
#define glVertexAttrib4fvARB                    pglVertexAttrib4fvARB
#define glVertexAttrib4ivARB                    pglVertexAttrib4ivARB
#define glVertexAttrib4sARB                     pglVertexAttrib4sARB
#define glVertexAttrib4svARB                    pglVertexAttrib4svARB
#define glVertexAttrib4ubvARB                   pglVertexAttrib4ubvARB
#define glVertexAttrib4uivARB                   pglVertexAttrib4uivARB
#define glVertexAttrib4usvARB                   pglVertexAttrib4usvARB
#define glVertexAttribPointerARB                pglVertexAttribPointerARB
#define glEnableVertexAttribArrayARB            pglEnableVertexAttribArrayARB
#define glDisableVertexAttribArrayARB           pglDisableVertexAttribArrayARB
#define glProgramStringARB                      pglProgramStringARB
#define glBindProgramARB                        pglBindProgramARB
#define glDeleteProgramsARB                     pglDeleteProgramsARB
#define glGenProgramsARB                        pglGenProgramsARB
#define glProgramEnvParameter4dARB              pglProgramEnvParameter4dARB
#define glProgramEnvParameter4dvARB             pglProgramEnvParameter4dvARB
#define glProgramEnvParameter4fARB              pglProgramEnvParameter4fARB
#define glProgramEnvParameter4fvARB             pglProgramEnvParameter4fvARB
#define glProgramLocalParameter4dARB            pglProgramLocalParameter4dARB
#define glProgramLocalParameter4dvARB           pglProgramLocalParameter4dvARB
#define glProgramLocalParameter4fARB            pglProgramLocalParameter4fARB
#define glProgramLocalParameter4fvARB           pglProgramLocalParameter4fvARB
#define glGetProgramEnvParameterdvARB           pglGetProgramEnvParameterdvARB
#define glGetProgramEnvParameterfvARB           pglGetProgramEnvParameterfvARB
#define glGetProgramLocalParameterdvARB         pglGetProgramLocalParameterdvARB
#define glGetProgramLocalParameterfvARB         pglGetProgramLocalParameterfvARB
#define glGetProgramivARB                       pglGetProgramivARB
#define glGetProgramStringARB                   pglGetProgramStringARB
#define glGetVertexAttribdvARB                  pglGetVertexAttribdvARB
#define glGetVertexAttribfvARB                  pglGetVertexAttribfvARB
#define glGetVertexAttribivARB                  pglGetVertexAttribivARB
#define glGetVertexAttribPointervARB            pglGetVertexAttribPointervARB
#define glIsProgramARB                          pglIsProgramARB

// WGL_ARB_extensions_string
typedef const char* (WINAPI * PFNWGLGETEXTENSIONSSTRINGARBPROC)(HDC hdc);
extern PFNWGLGETEXTENSIONSSTRINGARBPROC pwglGetExtensionsStringARB;
#define wglGetExtensionsStringARB       pwglGetExtensionsStringARB

// WGL_ARB_pixel_format
typedef BOOL (WINAPI * PFNWGLGETPIXELFORMATATTRIBIVARBPROC) (HDC hdc, int iPixelFormat, int iLayerPlane, UINT nAttributes, const int *piAttributes, int *piValues);
typedef BOOL (WINAPI * PFNWGLGETPIXELFORMATATTRIBFVARBPROC) (HDC hdc, int iPixelFormat, int iLayerPlane, UINT nAttributes, const int *piAttributes, FLOAT *pfValues);
typedef BOOL (WINAPI * PFNWGLCHOOSEPIXELFORMATARBPROC) (HDC hdc, const int *piAttribIList, const FLOAT *pfAttribFList, UINT nMaxFormats, int *piFormats, UINT *nNumFormats);
extern PFNWGLGETPIXELFORMATATTRIBIVARBPROC  pwglGetPixelFormatAttribivARB;
extern PFNWGLGETPIXELFORMATATTRIBFVARBPROC  pwglGetPixelFormatAttribfvARB;
extern PFNWGLCHOOSEPIXELFORMATARBPROC       pwglChoosePixelFormatARB;
#define wglGetPixelFormatAttribivARB        pwglGetPixelFormatAttribivARB
#define wglGetPixelFormatAttribfvARB        pwglGetPixelFormatAttribfvARB
#define wglChoosePixelFormatARB             pwglChoosePixelFormatARB

// WGL_ARB_create_context
typedef HGLRC (WINAPI * PFNWGLCREATECONTEXTATTRIBSARBPROC) (HDC hDC, HGLRC hShareContext, const int *attribList);
extern PFNWGLCREATECONTEXTATTRIBSARBPROC    pwglCreateContextAttribsARB;
#define wglCreateContextAttribsARB          pwglCreateContextAttribsARB


#endif //======================================================================

#endif // end of #ifndef
