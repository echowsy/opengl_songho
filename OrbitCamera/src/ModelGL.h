///////////////////////////////////////////////////////////////////////////////
// ModelGL.h
// =========
// Model component of OpenGL
//
//  AUTHOR: Song Ho Ahn (song.ahn@gmail.com)
// CREATED: 2016-05-29
// UPDATED: 2016-07-21
///////////////////////////////////////////////////////////////////////////////

#ifndef MODEL_GL_H
#define MODEL_GL_H

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#ifdef _WIN32
#include "glExtension.h"
#endif

#include <vector>
#include <string>
#include "Matrices.h"
#include "Vectors.h"
#include "Quaternion.h"
#include "ObjModel.h"
#include "BoundingBox.h"
#include "BitmapFont.h"
#include "OrbitCamera.h"
#include "Vertices.h"

class ModelGL
{
public:
    ModelGL();
    ~ModelGL();

    void init();                            // initialize OpenGL states
    bool initShaders();                     // init shader programs
    void quit();                            // clean up OpenGL objects
    void draw(int screenId=1);
    bool loadObjs();

    void setMouseLeft(bool flag)            { mouseLeftDown = flag; };
    void setMouseRight(bool flag)           { mouseRightDown = flag; };
    void setMousePosition(int x, int y)     { mouseX = x; mouseY = y; };
    void setWindowSize(int width, int height);
    void setFov(float fov);

    void setCameraAngleX(float x);
    void setCameraAngleY(float y);
    void setCameraAngleZ(float z);
    void setCameraPositionX(float x);
    void setCameraPositionY(float y);
    void setCameraPositionZ(float z);
    void setCameraTargetX(float x);
    void setCameraTargetY(float y);
    void setCameraTargetZ(float z);

    Vector3 getCameraAngle()                { return cameraAngle; }
    float getCameraAngleX()                 { return cameraAngle.x; }
    float getCameraAngleY()                 { return cameraAngle.y; }
    float getCameraAngleZ()                 { return cameraAngle.z; }
    Vector3 getCameraPosition()             { return cameraPosition; }
    float getCameraPositionX()              { return cameraPosition.x; }
    float getCameraPositionY()              { return cameraPosition.y; }
    float getCameraPositionZ()              { return cameraPosition.z; }
    Vector3 getCameraTarget()               { return cameraTarget; }
    float getCameraTargetX()                { return cameraTarget.x; }
    float getCameraTargetY()                { return cameraTarget.y; }
    float getCameraTargetZ()                { return cameraTarget.z; }
    const Quaternion& getCameraQuaternion() { return cameraQuaternion; }
    const Matrix4& getCameraMatrix()        { return cameraMatrix; }

    void rotateCamera(int x, int y);
    void zoomCamera(int dist);
    void zoomCameraDelta(float delta);        // for mousewheel
    void resetCamera();

    bool isShaderSupported();
    bool isVboSupported();

    // for grid
    void setGridSize(float radius);

    // for FOV
    void enableFov()                        { fovEnabled = true; }
    void disableFov()                       { fovEnabled = false; }

    // for grid
    void enableGrid()                       { gridEnabled = true; }
    void disableGrid()                      { gridEnabled = false; }

protected:

private:
    // member functions
    void initLights();                              // add a white light ti scene
    void initFont();
    void setViewport(int x, int y, int width, int height);
    void preFrame();
    void postFrame();
    void drawObj();
    void drawObjWithVbo();
    void drawCamera();                              // draw camera in world space
    void drawCameraWithVbo();
    void drawGridXZ(float size, float step);        // draw a grid on XZ plane
    void drawGridXY(float size, float step);        // draw a grid on XY plane
    void drawFocalLine();
    void drawFocalPoint();
    void drawFov();
    void draw2D(int screenId);
    void setFrustum(float l, float r, float b, float t, float n, float f);
    void setFrustum(float fovy, float ratio, float n, float f);
    void setOrthoFrustum(float l, float r, float b, float t, float n=-1, float f=1);
    bool createShaderPrograms();
    void createVertexBufferObjects();
    void logShaders();
    void computeFovVertices(float fov);

    // members
    int windowWidth;
    int windowHeight;
    bool windowSizeChanged;
    bool mouseLeftDown;
    bool mouseRightDown;
    int mouseX;
    int mouseY;
    float nearPlane;
    float farPlane;
    float fov;
    Vector4 bgColor;
    bool gridEnabled;

    float gridSize;         // half length of grid
    float gridStep;         // step for next grid line

    // obj
    ObjModel objModel;
    ObjModel objCam;
    bool objLoaded;

    // cameras
    OrbitCamera cam1;       // for view1
    OrbitCamera cam2;       // for view2
    Vector3 cameraAngle;
    Vector3 cameraPosition;
    Vector3 cameraTarget;
    Quaternion cameraQuaternion;
    Matrix4 cameraMatrix;

    // 4x4 transform matrices
    Matrix4 matrixProjection;

    // vbo extension
    bool vboSupported;
    bool vboReady;
    GLuint vboModel;                // vbo for OBJ vertices
    GLuint vboCam;                  // vbo for camera OBJ
    std::vector<GLuint> iboModel;   // vbo for OBJ indices
    std::vector<GLuint> iboCam;     // vbo for OBJ indices

    // glsl extension
    bool glslSupported;
    bool glslReady;
    GLhandleARB progId1;            // shader program with color
    GLhandleARB progId2;            // shader program with color + lighting

    // bitmap font
    BitmapFont font;

    // material
    float defaultAmbient[4];
    float defaultDiffuse[4];
    float defaultSpecular[4];
    float defaultShininess;
    float camAmbient[4];
    float camDiffuse[4];
    float camSpecular[4];
    float camShininess;

    // vertices for FOV
    Vector3 fovVertices[5];
    Vector3 fovNormals[4];
    bool    fovEnabled;

};
#endif
