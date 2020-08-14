///////////////////////////////////////////////////////////////////////////////
// glExtension.cpp
// ===============
// OpenGL extension helper
// NOTE: The size of HDC (void*) in 64bit Windows is 8 bytes.
//
//  AUTHOR: Song Ho Ahn (song.ahn@gmail.com)
// CREATED: 2013-03-05
// UPDATED: 2017-11-07
///////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <algorithm>
#include "glExtension.h"


#ifdef _WIN32 //===============================================================
// GL_ARB_framebuffer_object
PFNGLGENFRAMEBUFFERSPROC                        pglGenFramebuffers = 0;                     // FBO name generation procedure
PFNGLDELETEFRAMEBUFFERSPROC                     pglDeleteFramebuffers = 0;                  // FBO deletion procedure
PFNGLBINDFRAMEBUFFERPROC                        pglBindFramebuffer = 0;                     // FBO bind procedure
PFNGLCHECKFRAMEBUFFERSTATUSPROC                 pglCheckFramebufferStatus = 0;              // FBO completeness test procedure
PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC    pglGetFramebufferAttachmentParameteriv = 0; // return various FBO parameters
PFNGLGENERATEMIPMAPPROC                         pglGenerateMipmap = 0;                      // FBO automatic mipmap generation procedure
PFNGLFRAMEBUFFERTEXTURE1DPROC                   pglFramebufferTexture1D = 0;                // FBO 1D texture attachement procedure
PFNGLFRAMEBUFFERTEXTURE2DPROC                   pglFramebufferTexture2D = 0;                // FBO 2D texture attachement procedure
PFNGLFRAMEBUFFERTEXTURE3DPROC                   pglFramebufferTexture3D = 0;                // FBO 3D texture attachement procedure
PFNGLFRAMEBUFFERTEXTURELAYERPROC                pglFramebufferTextureLayer = 0;             // FBO 3D texture layer attachement procedure
PFNGLFRAMEBUFFERRENDERBUFFERPROC                pglFramebufferRenderbuffer = 0;             // FBO renderbuffer attachement procedure
PFNGLISFRAMEBUFFERPROC                          pglIsFramebuffer = 0;                       // FBO state = true/false
PFNGLBLITFRAMEBUFFERPROC                        pglBlitFramebuffer = 0;                     // FBO copy
PFNGLGENRENDERBUFFERSPROC                       pglGenRenderbuffers = 0;                    // renderbuffer generation procedure
PFNGLDELETERENDERBUFFERSPROC                    pglDeleteRenderbuffers = 0;                 // renderbuffer deletion procedure
PFNGLBINDRENDERBUFFERPROC                       pglBindRenderbuffer = 0;                    // renderbuffer bind procedure
PFNGLRENDERBUFFERSTORAGEPROC                    pglRenderbufferStorage = 0;                 // renderbuffer memory allocation procedure
PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC         pglRenderbufferStorageMultisample = 0;      // renderbuffer memory allocation with multisample
PFNGLGETRENDERBUFFERPARAMETERIVPROC             pglGetRenderbufferParameteriv = 0;          // return various renderbuffer parameters
PFNGLISRENDERBUFFERPROC                         pglIsRenderbuffer = 0;                      // determine renderbuffer object type

// GL_ARB_multisample
PFNGLSAMPLECOVERAGEARBPROC  pglSampleCoverageARB = 0;

// GL_ARB_multitexture
PFNGLACTIVETEXTUREARBPROC   pglActiveTextureARB = 0;

// GL_ARB_pixel_buffer_objects & GL_ARB_vertex_buffer_object
PFNGLGENBUFFERSARBPROC              pglGenBuffersARB = 0;           // VBO Name Generation Procedure
PFNGLBINDBUFFERARBPROC              pglBindBufferARB = 0;           // VBO Bind Procedure
PFNGLBUFFERDATAARBPROC              pglBufferDataARB = 0;           // VBO Data Loading Procedure
PFNGLBUFFERSUBDATAARBPROC           pglBufferSubDataARB = 0;        // VBO Sub Data Loading Procedure
PFNGLDELETEBUFFERSARBPROC           pglDeleteBuffersARB = 0;        // VBO Deletion Procedure
PFNGLGETBUFFERPARAMETERIVARBPROC    pglGetBufferParameterivARB = 0; // return various parameters of VBO
PFNGLMAPBUFFERARBPROC               pglMapBufferARB = 0;            // map VBO procedure
PFNGLUNMAPBUFFERARBPROC             pglUnmapBufferARB = 0;          // unmap VBO procedure

// GL_ARB_shader_objects
PFNGLDELETEOBJECTARBPROC            pglDeleteObjectARB = 0;         // delete shader object
PFNGLGETHANDLEARBPROC               pglGetHandleARB = 0;            // return handle of program
PFNGLDETACHOBJECTARBPROC            pglDetachObjectARB = 0;         // detatch a shader from a program
PFNGLCREATESHADEROBJECTARBPROC      pglCreateShaderObjectARB = 0;   // create a shader
PFNGLSHADERSOURCEARBPROC            pglShaderSourceARB = 0;         // set a shader source(codes)
PFNGLCOMPILESHADERARBPROC           pglCompileShaderARB = 0;        // compile shader source
PFNGLCREATEPROGRAMOBJECTARBPROC     pglCreateProgramObjectARB = 0;  // create a program
PFNGLATTACHOBJECTARBPROC            pglAttachObjectARB = 0;         // attach a shader to a program
PFNGLLINKPROGRAMARBPROC             pglLinkProgramARB = 0;          // link a program
PFNGLUSEPROGRAMOBJECTARBPROC        pglUseProgramObjectARB = 0;     // use a program
PFNGLVALIDATEPROGRAMARBPROC         pglValidateProgramARB = 0;      // validate a program
PFNGLUNIFORM1FARBPROC               pglUniform1fARB = 0;            //
PFNGLUNIFORM2FARBPROC               pglUniform2fARB = 0;            //
PFNGLUNIFORM3FARBPROC               pglUniform3fARB = 0;            //
PFNGLUNIFORM4FARBPROC               pglUniform4fARB = 0;            //
PFNGLUNIFORM1IARBPROC               pglUniform1iARB = 0;            //
PFNGLUNIFORM2IARBPROC               pglUniform2iARB = 0;            //
PFNGLUNIFORM3IARBPROC               pglUniform3iARB = 0;            //
PFNGLUNIFORM4IARBPROC               pglUniform4iARB = 0;            //
PFNGLUNIFORM1FVARBPROC              pglUniform1fvARB = 0;           //
PFNGLUNIFORM2FVARBPROC              pglUniform2fvARB = 0;           //
PFNGLUNIFORM3FVARBPROC              pglUniform3fvARB = 0;           //
PFNGLUNIFORM4FVARBPROC              pglUniform4fvARB = 0;           //
PFNGLUNIFORM1FVARBPROC              pglUniform1ivARB = 0;           //
PFNGLUNIFORM2FVARBPROC              pglUniform2ivARB = 0;           //
PFNGLUNIFORM3FVARBPROC              pglUniform3ivARB = 0;           //
PFNGLUNIFORM4FVARBPROC              pglUniform4ivARB = 0;           //
PFNGLUNIFORMMATRIX2FVARBPROC        pglUniformMatrix2fvARB = 0;     //
PFNGLUNIFORMMATRIX3FVARBPROC        pglUniformMatrix3fvARB = 0;     //
PFNGLUNIFORMMATRIX4FVARBPROC        pglUniformMatrix4fvARB = 0;     //
PFNGLGETOBJECTPARAMETERFVARBPROC    pglGetObjectParameterfvARB = 0; // get shader/program param
PFNGLGETOBJECTPARAMETERIVARBPROC    pglGetObjectParameterivARB = 0; //
PFNGLGETINFOLOGARBPROC              pglGetInfoLogARB = 0;           // get log
PFNGLGETATTACHEDOBJECTSARBPROC      pglGetAttachedObjectsARB = 0;   // get attached shader to a program
PFNGLGETUNIFORMLOCATIONARBPROC      pglGetUniformLocationARB = 0;   // get index of uniform var
PFNGLGETACTIVEUNIFORMARBPROC        pglGetActiveUniformARB = 0;     // get info of uniform var
PFNGLGETUNIFORMFVARBPROC            pglGetUniformfvARB = 0;         // get value of uniform var
PFNGLGETUNIFORMIVARBPROC            pglGetUniformivARB = 0;         //
PFNGLGETSHADERSOURCEARBPROC         pglGetShaderSourceARB = 0;      // get shader source codes

// GL_ARB_sync extension
PFNGLFENCESYNCPROC          pglFenceSync = 0;
PFNGLISSYNCPROC             pglIsSync = 0;
PFNGLDELETESYNCPROC         pglDeleteSync = 0;
PFNGLCLIENTWAITSYNCPROC     pglClientWaitSync = 0;
PFNGLWAITSYNCPROC           pglWaitSync = 0;
PFNGLGETINTEGER64VPROC      pglGetInteger64v = 0;
PFNGLGETSYNCIVPROC          pglGetSynciv = 0;

// GL_ARB_vertex_shader and GL_ARB_fragment_shader extensions
PFNGLBINDATTRIBLOCATIONARBPROC  pglBindAttribLocationARB = 0;       // bind vertex attrib var with index
PFNGLGETACTIVEATTRIBARBPROC     pglGetActiveAttribARB = 0;          // get attrib value
PFNGLGETATTRIBLOCATIONARBPROC   pglGetAttribLocationARB = 0;        // get lndex of attrib var

// GL_ARB_vertex_program
PFNGLVERTEXATTRIB1DARBPROC              pglVertexAttrib1dARB = 0;
PFNGLVERTEXATTRIB1DVARBPROC             pglVertexAttrib1dvARB = 0;
PFNGLVERTEXATTRIB1FARBPROC              pglVertexAttrib1fARB = 0;
PFNGLVERTEXATTRIB1FVARBPROC             pglVertexAttrib1fvARB = 0;
PFNGLVERTEXATTRIB1SARBPROC              pglVertexAttrib1sARB = 0;
PFNGLVERTEXATTRIB1SVARBPROC             pglVertexAttrib1svARB = 0;
PFNGLVERTEXATTRIB2DARBPROC              pglVertexAttrib2dARB = 0;
PFNGLVERTEXATTRIB2DVARBPROC             pglVertexAttrib2dvARB = 0;
PFNGLVERTEXATTRIB2FARBPROC              pglVertexAttrib2fARB = 0;
PFNGLVERTEXATTRIB2FVARBPROC             pglVertexAttrib2fvARB = 0;
PFNGLVERTEXATTRIB2SARBPROC              pglVertexAttrib2sARB = 0;
PFNGLVERTEXATTRIB2SVARBPROC             pglVertexAttrib2svARB = 0;
PFNGLVERTEXATTRIB3DARBPROC              pglVertexAttrib3dARB = 0;
PFNGLVERTEXATTRIB3DVARBPROC             pglVertexAttrib3dvARB = 0;
PFNGLVERTEXATTRIB3FARBPROC              pglVertexAttrib3fARB = 0;
PFNGLVERTEXATTRIB3FVARBPROC             pglVertexAttrib3fvARB = 0;
PFNGLVERTEXATTRIB3SARBPROC              pglVertexAttrib3sARB = 0;
PFNGLVERTEXATTRIB3SVARBPROC             pglVertexAttrib3svARB = 0;
PFNGLVERTEXATTRIB4NBVARBPROC            pglVertexAttrib4NbvARB = 0;
PFNGLVERTEXATTRIB4NIVARBPROC            pglVertexAttrib4NivARB = 0;
PFNGLVERTEXATTRIB4NSVARBPROC            pglVertexAttrib4NsvARB = 0;
PFNGLVERTEXATTRIB4NUBARBPROC            pglVertexAttrib4NubARB = 0;
PFNGLVERTEXATTRIB4NUBVARBPROC           pglVertexAttrib4NubvARB = 0;
PFNGLVERTEXATTRIB4NUIVARBPROC           pglVertexAttrib4NuivARB = 0;
PFNGLVERTEXATTRIB4NUSVARBPROC           pglVertexAttrib4NusvARB = 0;
PFNGLVERTEXATTRIB4BVARBPROC             pglVertexAttrib4bvARB = 0;
PFNGLVERTEXATTRIB4DARBPROC              pglVertexAttrib4dARB = 0;
PFNGLVERTEXATTRIB4DVARBPROC             pglVertexAttrib4dvARB = 0;
PFNGLVERTEXATTRIB4FARBPROC              pglVertexAttrib4fARB = 0;
PFNGLVERTEXATTRIB4FVARBPROC             pglVertexAttrib4fvARB = 0;
PFNGLVERTEXATTRIB4IVARBPROC             pglVertexAttrib4ivARB = 0;
PFNGLVERTEXATTRIB4SARBPROC              pglVertexAttrib4sARB = 0;
PFNGLVERTEXATTRIB4SVARBPROC             pglVertexAttrib4svARB = 0;
PFNGLVERTEXATTRIB4UBVARBPROC            pglVertexAttrib4ubvARB = 0;
PFNGLVERTEXATTRIB4UIVARBPROC            pglVertexAttrib4uivARB = 0;
PFNGLVERTEXATTRIB4USVARBPROC            pglVertexAttrib4usvARB = 0;
PFNGLVERTEXATTRIBPOINTERARBPROC         pglVertexAttribPointerARB = 0;
PFNGLENABLEVERTEXATTRIBARRAYARBPROC     pglEnableVertexAttribArrayARB = 0;
PFNGLDISABLEVERTEXATTRIBARRAYARBPROC    pglDisableVertexAttribArrayARB = 0;
PFNGLPROGRAMSTRINGARBPROC               pglProgramStringARB = 0;
PFNGLBINDPROGRAMARBPROC                 pglBindProgramARB = 0;
PFNGLDELETEPROGRAMSARBPROC              pglDeleteProgramsARB = 0;
PFNGLGENPROGRAMSARBPROC                 pglGenProgramsARB = 0;
PFNGLPROGRAMENVPARAMETER4DARBPROC       pglProgramEnvParameter4dARB = 0;
PFNGLPROGRAMENVPARAMETER4DVARBPROC      pglProgramEnvParameter4dvARB = 0;
PFNGLPROGRAMENVPARAMETER4FARBPROC       pglProgramEnvParameter4fARB = 0;
PFNGLPROGRAMENVPARAMETER4FVARBPROC      pglProgramEnvParameter4fvARB = 0;
PFNGLPROGRAMLOCALPARAMETER4DARBPROC     pglProgramLocalParameter4dARB = 0;
PFNGLPROGRAMLOCALPARAMETER4DVARBPROC    pglProgramLocalParameter4dvARB = 0;
PFNGLPROGRAMLOCALPARAMETER4FARBPROC     pglProgramLocalParameter4fARB = 0;
PFNGLPROGRAMLOCALPARAMETER4FVARBPROC    pglProgramLocalParameter4fvARB = 0;
PFNGLGETPROGRAMENVPARAMETERDVARBPROC    pglGetProgramEnvParameterdvARB = 0;
PFNGLGETPROGRAMENVPARAMETERFVARBPROC    pglGetProgramEnvParameterfvARB = 0;
PFNGLGETPROGRAMLOCALPARAMETERDVARBPROC  pglGetProgramLocalParameterdvARB = 0;
PFNGLGETPROGRAMLOCALPARAMETERFVARBPROC  pglGetProgramLocalParameterfvARB = 0;
PFNGLGETPROGRAMIVARBPROC                pglGetProgramivARB = 0;
PFNGLGETPROGRAMSTRINGARBPROC            pglGetProgramStringARB = 0;
PFNGLGETVERTEXATTRIBDVARBPROC           pglGetVertexAttribdvARB = 0;
PFNGLGETVERTEXATTRIBFVARBPROC           pglGetVertexAttribfvARB = 0;
PFNGLGETVERTEXATTRIBIVARBPROC           pglGetVertexAttribivARB = 0;
PFNGLGETVERTEXATTRIBPOINTERVARBPROC     pglGetVertexAttribPointervARB = 0;
PFNGLISPROGRAMARBPROC                   pglIsProgramARB = 0;

// WGL_ARB_extensions_string
PFNWGLGETEXTENSIONSSTRINGARBPROC    pwglGetExtensionsStringARB = 0;

// WGL_ARB_pixel_format
PFNWGLGETPIXELFORMATATTRIBIVARBPROC  pwglGetPixelFormatAttribivARB = 0;
PFNWGLGETPIXELFORMATATTRIBFVARBPROC  pwglGetPixelFormatAttribfvARB = 0;
PFNWGLCHOOSEPIXELFORMATARBPROC       pwglChoosePixelFormatARB = 0;

// WGL_ARB_create_context
PFNWGLCREATECONTEXTATTRIBSARBPROC   pwglCreateContextAttribsARB = 0;

#endif //======================================================================



///////////////////////////////////////////////////////////////////////////////
// ctor / dtor
///////////////////////////////////////////////////////////////////////////////
glExtension::glExtension(void* param)
{
    // must be called after OpenGL RC is open
    hdc = param;    // HDC == void*
    getExtensionStrings();

#ifdef _WIN32
    getFunctionPointers();
#endif
}
glExtension::~glExtension()
{
}



///////////////////////////////////////////////////////////////////////////////
// instantiate a singleton instance if not exist
///////////////////////////////////////////////////////////////////////////////
glExtension& glExtension::getInstance(void* param)
{
    static glExtension self(param);
    return self;
}



///////////////////////////////////////////////////////////////////////////////
// check if opengl extension is available
///////////////////////////////////////////////////////////////////////////////
bool glExtension::isSupported(const std::string& ext)
{
    // search corresponding extension
    std::vector<std::string>::const_iterator iter = this->extensions.begin();
    std::vector<std::string>::const_iterator endIter = this->extensions.end();
    while(iter != endIter)
    {
        if(toLower(ext) == toLower(*iter))
            return true;
        else
            ++iter;
    }
    return false;
}



///////////////////////////////////////////////////////////////////////////////
// return array of OpenGL extension strings
///////////////////////////////////////////////////////////////////////////////
const std::vector<std::string>& glExtension::getExtensions()
{
    // re-try to get extensions if it is empty
    if(extensions.size() == 0)
        getExtensionStrings();

    return extensions;
}



///////////////////////////////////////////////////////////////////////////////
// get supported extensions
///////////////////////////////////////////////////////////////////////////////
void glExtension::getExtensionStrings()
{
    const char* cstr = (const char*)glGetString(GL_EXTENSIONS);
    if(!cstr) // check null ptr
        return;

    std::string str(cstr);
    std::string token;
    std::string::const_iterator cursor = str.begin();
    while(cursor != str.end())
    {
        if(*cursor != ' ')
        {
            token += *cursor;
        }
        else
        {
            extensions.push_back(token);
            token.clear();
        }
        ++cursor;
    }

#ifdef _WIN32 //===========================================
    // get WGL specific extensions for v3.0+
    wglGetExtensionsStringARB = (PFNWGLGETEXTENSIONSSTRINGARBPROC)wglGetProcAddress("wglGetExtensionsStringARB");
    if(wglGetExtensionsStringARB && hdc)
    {
        str = (const char*)wglGetExtensionsStringARB((HDC)hdc);
        std::string token;
        std::string::const_iterator cursor = str.begin();
        while(cursor != str.end())
        {
            if(*cursor != ' ')
            {
                token += *cursor;
            }
            else
            {
                extensions.push_back(token);
                token.clear();
            }
            ++cursor;
        }
    }
#endif //==================================================

    // sort extension by alphabetical order
    std::sort(this->extensions.begin(), this->extensions.end());
}



///////////////////////////////////////////////////////////////////////////////
// string utility
///////////////////////////////////////////////////////////////////////////////
std::string glExtension::toLower(const std::string& str)
{
    std::string newStr = str;
    std::transform(newStr.begin(), newStr.end(), newStr.begin(), ::tolower);
    return newStr;
}


///////////////////////////////////////////////////////////////////////////////
// get function pointers from OpenGL ICD driver
///////////////////////////////////////////////////////////////////////////////
void glExtension::getFunctionPointers()
{
#ifdef _WIN32
    std::vector<std::string>::const_iterator iter = this->extensions.begin();
    std::vector<std::string>::const_iterator endIter = this->extensions.end();
    for(int i = 0; i < (int)extensions.size(); ++i)
    {
        if(extensions[i] == "GL_ARB_framebuffer_object")
        {
            glGenFramebuffers                     = (PFNGLGENFRAMEBUFFERSPROC)wglGetProcAddress("glGenFramebuffers");
            glDeleteFramebuffers                  = (PFNGLDELETEFRAMEBUFFERSPROC)wglGetProcAddress("glDeleteFramebuffers");
            glBindFramebuffer                     = (PFNGLBINDFRAMEBUFFERPROC)wglGetProcAddress("glBindFramebuffer");
            glCheckFramebufferStatus              = (PFNGLCHECKFRAMEBUFFERSTATUSPROC)wglGetProcAddress("glCheckFramebufferStatus");
            glGetFramebufferAttachmentParameteriv = (PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC)wglGetProcAddress("glGetFramebufferAttachmentParameteriv");
            glGenerateMipmap                      = (PFNGLGENERATEMIPMAPPROC)wglGetProcAddress("glGenerateMipmap");
            glFramebufferTexture1D                = (PFNGLFRAMEBUFFERTEXTURE1DPROC)wglGetProcAddress("glFramebufferTexture1D");
            glFramebufferTexture2D                = (PFNGLFRAMEBUFFERTEXTURE2DPROC)wglGetProcAddress("glFramebufferTexture2D");
            glFramebufferTexture3D                = (PFNGLFRAMEBUFFERTEXTURE3DPROC)wglGetProcAddress("glFramebufferTexture3D");
            glFramebufferTextureLayer             = (PFNGLFRAMEBUFFERTEXTURELAYERPROC)wglGetProcAddress("glFramebufferTextureLayer");
            glFramebufferRenderbuffer             = (PFNGLFRAMEBUFFERRENDERBUFFERPROC)wglGetProcAddress("glFramebufferRenderbuffer");
            glIsFramebuffer                       = (PFNGLISFRAMEBUFFERPROC)wglGetProcAddress("glIsFramebuffer");
            glBlitFramebuffer                     = (PFNGLBLITFRAMEBUFFERPROC)wglGetProcAddress("glBlitFramebuffer");
            glGenRenderbuffers                    = (PFNGLGENRENDERBUFFERSPROC)wglGetProcAddress("glGenRenderbuffers");
            glDeleteRenderbuffers                 = (PFNGLDELETERENDERBUFFERSPROC)wglGetProcAddress("glDeleteRenderbuffers");
            glBindRenderbuffer                    = (PFNGLBINDRENDERBUFFERPROC)wglGetProcAddress("glBindRenderbuffer");
            glRenderbufferStorage                 = (PFNGLRENDERBUFFERSTORAGEPROC)wglGetProcAddress("glRenderbufferStorage");
            glRenderbufferStorageMultisample      = (PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC)wglGetProcAddress("glRenderbufferStorageMultisample");
            glGetRenderbufferParameteriv          = (PFNGLGETRENDERBUFFERPARAMETERIVPROC)wglGetProcAddress("glGetRenderbufferParameteriv");
            glIsRenderbuffer                      = (PFNGLISRENDERBUFFERPROC)wglGetProcAddress("glIsRenderbuffer");
        }
        else if(extensions[i] == "GL_ARB_multisample")
        {
            glSampleCoverageARB = (PFNGLSAMPLECOVERAGEARBPROC)wglGetProcAddress("glSampleCoverageARB");
        }
        else if(extensions[i] == "GL_ARB_multitexture")
        {
            glActiveTextureARB = (PFNGLACTIVETEXTUREARBPROC)wglGetProcAddress("glActiveTextureARB");
        }
        else if(extensions[i] == "GL_ARB_vertex_buffer_object") // same as PBO
        {
            glGenBuffersARB             = (PFNGLGENBUFFERSARBPROC)wglGetProcAddress("glGenBuffersARB");
            glBindBufferARB             = (PFNGLBINDBUFFERARBPROC)wglGetProcAddress("glBindBufferARB");
            glBufferDataARB             = (PFNGLBUFFERDATAARBPROC)wglGetProcAddress("glBufferDataARB");
            glBufferSubDataARB          = (PFNGLBUFFERSUBDATAARBPROC)wglGetProcAddress("glBufferSubDataARB");
            glDeleteBuffersARB          = (PFNGLDELETEBUFFERSARBPROC)wglGetProcAddress("glDeleteBuffersARB");
            glGetBufferParameterivARB   = (PFNGLGETBUFFERPARAMETERIVARBPROC)wglGetProcAddress("glGetBufferParameterivARB");
            glMapBufferARB              = (PFNGLMAPBUFFERARBPROC)wglGetProcAddress("glMapBufferARB");
            glUnmapBufferARB            = (PFNGLUNMAPBUFFERARBPROC)wglGetProcAddress("glUnmapBufferARB");
        }
        else if(extensions[i] == "GL_ARB_shader_objects")
        {
            glDeleteObjectARB           = (PFNGLDELETEOBJECTARBPROC)wglGetProcAddress("glDeleteObjectARB");
            glGetHandleARB              = (PFNGLGETHANDLEARBPROC)wglGetProcAddress("glGetHandleARB");
            glDetachObjectARB           = (PFNGLDETACHOBJECTARBPROC)wglGetProcAddress("glDetachObjectARB");
            glCreateShaderObjectARB     = (PFNGLCREATESHADEROBJECTARBPROC)wglGetProcAddress("glCreateShaderObjectARB");
            glShaderSourceARB           = (PFNGLSHADERSOURCEARBPROC)wglGetProcAddress("glShaderSourceARB");
            glCompileShaderARB          = (PFNGLCOMPILESHADERARBPROC)wglGetProcAddress("glCompileShaderARB");
            glCreateProgramObjectARB    = (PFNGLCREATEPROGRAMOBJECTARBPROC)wglGetProcAddress("glCreateProgramObjectARB");
            glAttachObjectARB           = (PFNGLATTACHOBJECTARBPROC)wglGetProcAddress("glAttachObjectARB");
            glLinkProgramARB            = (PFNGLLINKPROGRAMARBPROC)wglGetProcAddress("glLinkProgramARB");
            glUseProgramObjectARB       = (PFNGLUSEPROGRAMOBJECTARBPROC)wglGetProcAddress("glUseProgramObjectARB");
            glValidateProgramARB        = (PFNGLVALIDATEPROGRAMARBPROC)wglGetProcAddress("glValidateProgramARB");
            glUniform1fARB              = (PFNGLUNIFORM1FARBPROC)wglGetProcAddress("glUniform1fARB");
            glUniform2fARB              = (PFNGLUNIFORM2FARBPROC)wglGetProcAddress("glUniform2fARB");
            glUniform3fARB              = (PFNGLUNIFORM3FARBPROC)wglGetProcAddress("glUniform3fARB");
            glUniform4fARB              = (PFNGLUNIFORM4FARBPROC)wglGetProcAddress("glUniform4fARB");
            glUniform1iARB              = (PFNGLUNIFORM1IARBPROC)wglGetProcAddress("glUniform1iARB");
            glUniform2iARB              = (PFNGLUNIFORM2IARBPROC)wglGetProcAddress("glUniform2iARB");
            glUniform3iARB              = (PFNGLUNIFORM3IARBPROC)wglGetProcAddress("glUniform3iARB");
            glUniform4iARB              = (PFNGLUNIFORM4IARBPROC)wglGetProcAddress("glUniform4iARB");
            glUniform1fvARB             = (PFNGLUNIFORM1FVARBPROC)wglGetProcAddress("glUniform1fvARB");
            glUniform2fvARB             = (PFNGLUNIFORM2FVARBPROC)wglGetProcAddress("glUniform2fvARB");
            glUniform3fvARB             = (PFNGLUNIFORM3FVARBPROC)wglGetProcAddress("glUniform3fvARB");
            glUniform4fvARB             = (PFNGLUNIFORM4FVARBPROC)wglGetProcAddress("glUniform4fvARB");
            glUniform1ivARB             = (PFNGLUNIFORM1FVARBPROC)wglGetProcAddress("glUniform1ivARB");
            glUniform2ivARB             = (PFNGLUNIFORM2FVARBPROC)wglGetProcAddress("glUniform2ivARB");
            glUniform3ivARB             = (PFNGLUNIFORM3FVARBPROC)wglGetProcAddress("glUniform3ivARB");
            glUniform4ivARB             = (PFNGLUNIFORM4FVARBPROC)wglGetProcAddress("glUniform4ivARB");
            glUniformMatrix2fvARB       = (PFNGLUNIFORMMATRIX2FVARBPROC)wglGetProcAddress("glUniformMatrix2fvARB");
            glUniformMatrix3fvARB       = (PFNGLUNIFORMMATRIX3FVARBPROC)wglGetProcAddress("glUniformMatrix3fvARB");
            glUniformMatrix4fvARB       = (PFNGLUNIFORMMATRIX4FVARBPROC)wglGetProcAddress("glUniformMatrix4fvARB");
            glGetObjectParameterfvARB   = (PFNGLGETOBJECTPARAMETERFVARBPROC)wglGetProcAddress("glGetObjectParameterfvARB");
            glGetObjectParameterivARB   = (PFNGLGETOBJECTPARAMETERIVARBPROC)wglGetProcAddress("glGetObjectParameterivARB");
            glGetInfoLogARB             = (PFNGLGETINFOLOGARBPROC)wglGetProcAddress("glGetInfoLogARB");
            glGetAttachedObjectsARB     = (PFNGLGETATTACHEDOBJECTSARBPROC)wglGetProcAddress("glGetAttachedObjectsARB");
            glGetUniformLocationARB     = (PFNGLGETUNIFORMLOCATIONARBPROC)wglGetProcAddress("glGetUniformLocationARB");
            glGetActiveUniformARB       = (PFNGLGETACTIVEUNIFORMARBPROC)wglGetProcAddress("glGetActiveUniformARB");
            glGetUniformfvARB           = (PFNGLGETUNIFORMFVARBPROC)wglGetProcAddress("glGetUniformfvARB");
            glGetUniformivARB           = (PFNGLGETUNIFORMIVARBPROC)wglGetProcAddress("glGetUniformivARB");
            glGetShaderSourceARB        = (PFNGLGETSHADERSOURCEARBPROC)wglGetProcAddress("glGetShaderSourceARB");
        }
        else if(extensions[i] == "GL_ARB_sync")
        {
            glFenceSync         = (PFNGLFENCESYNCPROC)wglGetProcAddress("glFenceSync");
            glIsSync            = (PFNGLISSYNCPROC)wglGetProcAddress("glIsSync");
            glDeleteSync        = (PFNGLDELETESYNCPROC)wglGetProcAddress("glDeleteSync");
            glClientWaitSync    = (PFNGLCLIENTWAITSYNCPROC)wglGetProcAddress("glClientWaitSync");
            glWaitSync          = (PFNGLWAITSYNCPROC)wglGetProcAddress("glWaitSync");
            glGetInteger64v     = (PFNGLGETINTEGER64VPROC)wglGetProcAddress("glGetInteger64v");
            glGetSynciv         = (PFNGLGETSYNCIVPROC)wglGetProcAddress("glGetSynciv");
        }
        else if(extensions[i] == "GL_ARB_vertex_shader") // also GL_ARB_fragment_shader
        {
            glBindAttribLocationARB = (PFNGLBINDATTRIBLOCATIONARBPROC)wglGetProcAddress("glBindAttribLocationARB");
            glGetActiveAttribARB    = (PFNGLGETACTIVEATTRIBARBPROC)wglGetProcAddress("glGetActiveAttribARB");
            glGetAttribLocationARB  = (PFNGLGETATTRIBLOCATIONARBPROC)wglGetProcAddress("glGetAttribLocationARB");
        }
        else if(extensions[i] == "GL_ARB_vertex_program")
        {
            glVertexAttrib1dARB             = (PFNGLVERTEXATTRIB1DARBPROC)wglGetProcAddress("glVertexAttrib1dARB");
            glVertexAttrib1dvARB            = (PFNGLVERTEXATTRIB1DVARBPROC)wglGetProcAddress("glVertexAttrib1dvARB");
            glVertexAttrib1fARB             = (PFNGLVERTEXATTRIB1FARBPROC)wglGetProcAddress("glVertexAttrib1fARB");
            glVertexAttrib1fvARB            = (PFNGLVERTEXATTRIB1FVARBPROC)wglGetProcAddress("glVertexAttrib1fvARB");
            glVertexAttrib1sARB             = (PFNGLVERTEXATTRIB1SARBPROC)wglGetProcAddress("glVertexAttrib1sARB");
            glVertexAttrib1svARB            = (PFNGLVERTEXATTRIB1SVARBPROC)wglGetProcAddress("glVertexAttrib1svARB");
            glVertexAttrib2dARB             = (PFNGLVERTEXATTRIB2DARBPROC)wglGetProcAddress("glVertexAttrib2dARB");
            glVertexAttrib2dvARB            = (PFNGLVERTEXATTRIB2DVARBPROC)wglGetProcAddress("glVertexAttrib2dvARB");
            glVertexAttrib2fARB             = (PFNGLVERTEXATTRIB2FARBPROC)wglGetProcAddress("glVertexAttrib2fARB");
            glVertexAttrib2fvARB            = (PFNGLVERTEXATTRIB2FVARBPROC)wglGetProcAddress("glVertexAttrib2fvARB");
            glVertexAttrib2sARB             = (PFNGLVERTEXATTRIB2SARBPROC)wglGetProcAddress("glVertexAttrib2sARB");
            glVertexAttrib2svARB            = (PFNGLVERTEXATTRIB2SVARBPROC)wglGetProcAddress("glVertexAttrib2svARB");
            glVertexAttrib3dARB             = (PFNGLVERTEXATTRIB3DARBPROC)wglGetProcAddress("glVertexAttrib3dARB");
            glVertexAttrib3dvARB            = (PFNGLVERTEXATTRIB3DVARBPROC)wglGetProcAddress("glVertexAttrib3dvARB");
            glVertexAttrib3fARB             = (PFNGLVERTEXATTRIB3FARBPROC)wglGetProcAddress("glVertexAttrib3fARB");
            glVertexAttrib3fvARB            = (PFNGLVERTEXATTRIB3FVARBPROC)wglGetProcAddress("glVertexAttrib3fvARB");
            glVertexAttrib3sARB             = (PFNGLVERTEXATTRIB3SARBPROC)wglGetProcAddress("glVertexAttrib3sARB");
            glVertexAttrib3svARB            = (PFNGLVERTEXATTRIB3SVARBPROC)wglGetProcAddress("glVertexAttrib3svARB");
            glVertexAttrib4NbvARB           = (PFNGLVERTEXATTRIB4NBVARBPROC)wglGetProcAddress("glVertexAttrib4NbvARB");
            glVertexAttrib4NivARB           = (PFNGLVERTEXATTRIB4NIVARBPROC)wglGetProcAddress("glVertexAttrib4NivARB");
            glVertexAttrib4NsvARB           = (PFNGLVERTEXATTRIB4NSVARBPROC)wglGetProcAddress("glVertexAttrib4NsvARB");
            glVertexAttrib4NubARB           = (PFNGLVERTEXATTRIB4NUBARBPROC)wglGetProcAddress("glVertexAttrib4NubARB");
            glVertexAttrib4NubvARB          = (PFNGLVERTEXATTRIB4NUBVARBPROC)wglGetProcAddress("glVertexAttrib4NubvARB");
            glVertexAttrib4NuivARB          = (PFNGLVERTEXATTRIB4NUIVARBPROC)wglGetProcAddress("glVertexAttrib4NuivARB");
            glVertexAttrib4NusvARB          = (PFNGLVERTEXATTRIB4NUSVARBPROC)wglGetProcAddress("glVertexAttrib4NusvARB");
            glVertexAttrib4bvARB            = (PFNGLVERTEXATTRIB4BVARBPROC)wglGetProcAddress("glVertexAttrib4bvARB");
            glVertexAttrib4dARB             = (PFNGLVERTEXATTRIB4DARBPROC)wglGetProcAddress("glVertexAttrib4dARB");
            glVertexAttrib4dvARB            = (PFNGLVERTEXATTRIB4DVARBPROC)wglGetProcAddress("glVertexAttrib4dvARB");
            glVertexAttrib4fARB             = (PFNGLVERTEXATTRIB4FARBPROC)wglGetProcAddress("glVertexAttrib4fARB");
            glVertexAttrib4fvARB            = (PFNGLVERTEXATTRIB4FVARBPROC)wglGetProcAddress("glVertexAttrib4fvARB");
            glVertexAttrib4ivARB            = (PFNGLVERTEXATTRIB4IVARBPROC)wglGetProcAddress("glVertexAttrib4ivARB");
            glVertexAttrib4sARB             = (PFNGLVERTEXATTRIB4SARBPROC)wglGetProcAddress("glVertexAttrib4sARB");
            glVertexAttrib4svARB            = (PFNGLVERTEXATTRIB4SVARBPROC)wglGetProcAddress("glVertexAttrib4svARB");
            glVertexAttrib4ubvARB           = (PFNGLVERTEXATTRIB4UBVARBPROC)wglGetProcAddress("glVertexAttrib4ubvARB");
            glVertexAttrib4uivARB           = (PFNGLVERTEXATTRIB4UIVARBPROC)wglGetProcAddress("glVertexAttrib4uivARB");
            glVertexAttrib4usvARB           = (PFNGLVERTEXATTRIB4USVARBPROC)wglGetProcAddress("glVertexAttrib4usvARB");
            glVertexAttribPointerARB        = (PFNGLVERTEXATTRIBPOINTERARBPROC)wglGetProcAddress("glVertexAttribPointerARB");
            glEnableVertexAttribArrayARB    = (PFNGLENABLEVERTEXATTRIBARRAYARBPROC)wglGetProcAddress("glEnableVertexAttribArrayARB");
            glDisableVertexAttribArrayARB   = (PFNGLDISABLEVERTEXATTRIBARRAYARBPROC)wglGetProcAddress("glDisableVertexAttribArrayARB");
            glProgramStringARB              = (PFNGLPROGRAMSTRINGARBPROC)wglGetProcAddress("glProgramStringARB");
            glBindProgramARB                = (PFNGLBINDPROGRAMARBPROC)wglGetProcAddress("glBindProgramARB");
            glDeleteProgramsARB             = (PFNGLDELETEPROGRAMSARBPROC)wglGetProcAddress("glDeleteProgramsARB");
            glGenProgramsARB                = (PFNGLGENPROGRAMSARBPROC)wglGetProcAddress("glGenProgramsARB");
            glProgramEnvParameter4dARB      = (PFNGLPROGRAMENVPARAMETER4DARBPROC)wglGetProcAddress("glProgramEnvParameter4dARB");
            glProgramEnvParameter4dvARB     = (PFNGLPROGRAMENVPARAMETER4DVARBPROC)wglGetProcAddress("glProgramEnvParameter4dvARB");
            glProgramEnvParameter4fARB      = (PFNGLPROGRAMENVPARAMETER4FARBPROC)wglGetProcAddress("glProgramEnvParameter4fARB");
            glProgramEnvParameter4fvARB     = (PFNGLPROGRAMENVPARAMETER4FVARBPROC)wglGetProcAddress("glProgramEnvParameter4fvARB");
            glProgramLocalParameter4dARB    = (PFNGLPROGRAMLOCALPARAMETER4DARBPROC)wglGetProcAddress("glProgramLocalParameter4dARB");
            glProgramLocalParameter4dvARB   = (PFNGLPROGRAMLOCALPARAMETER4DVARBPROC)wglGetProcAddress("glProgramLocalParameter4dvARB");
            glProgramLocalParameter4fARB    = (PFNGLPROGRAMLOCALPARAMETER4FARBPROC)wglGetProcAddress("glProgramLocalParameter4fARB");
            glProgramLocalParameter4fvARB   = (PFNGLPROGRAMLOCALPARAMETER4FVARBPROC)wglGetProcAddress("glProgramLocalParameter4fvARB");
            glGetProgramEnvParameterdvARB   = (PFNGLGETPROGRAMENVPARAMETERDVARBPROC)wglGetProcAddress("glGetProgramEnvParameterdvARB");
            glGetProgramEnvParameterfvARB   = (PFNGLGETPROGRAMENVPARAMETERFVARBPROC)wglGetProcAddress("glGetProgramEnvParameterfvARB");
            glGetProgramLocalParameterdvARB = (PFNGLGETPROGRAMLOCALPARAMETERDVARBPROC)wglGetProcAddress("glGetProgramLocalParameterdvARB");
            glGetProgramLocalParameterfvARB = (PFNGLGETPROGRAMLOCALPARAMETERFVARBPROC)wglGetProcAddress("glGetProgramLocalParameterfvARB");
            glGetProgramivARB               = (PFNGLGETPROGRAMIVARBPROC)wglGetProcAddress("glGetProgramivARB");
            glGetProgramStringARB           = (PFNGLGETPROGRAMSTRINGARBPROC)wglGetProcAddress("glGetProgramStringARB");
            glGetVertexAttribdvARB          = (PFNGLGETVERTEXATTRIBDVARBPROC)wglGetProcAddress("glGetVertexAttribdvARB");
            glGetVertexAttribfvARB          = (PFNGLGETVERTEXATTRIBFVARBPROC)wglGetProcAddress("glGetVertexAttribfvARB");
            glGetVertexAttribivARB          = (PFNGLGETVERTEXATTRIBIVARBPROC)wglGetProcAddress("glGetVertexAttribivARB");
            glGetVertexAttribPointervARB    = (PFNGLGETVERTEXATTRIBPOINTERVARBPROC)wglGetProcAddress("glGetVertexAttribPointervARB");
            glIsProgramARB                  = (PFNGLISPROGRAMARBPROC)wglGetProcAddress("glIsProgramARB");
        }
        else if(extensions[i] == "WGL_ARB_pixel_format")
        {
            wglGetPixelFormatAttribivARB = (PFNWGLGETPIXELFORMATATTRIBIVARBPROC)wglGetProcAddress("wglGetPixelFormatAttribivARB");
            wglGetPixelFormatAttribfvARB = (PFNWGLGETPIXELFORMATATTRIBFVARBPROC)wglGetProcAddress("wglGetPixelFormatAttribfvARB");
            wglChoosePixelFormatARB      = (PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB");
        }
        else if(extensions[i] == "WGL_ARB_create_context")
        {
            wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
        }
    }
#endif
}

