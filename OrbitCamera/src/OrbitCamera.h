///////////////////////////////////////////////////////////////////////////////
// OrbitCamera.h
// =============
// Orbital camera class for OpenGL
// Use lookAt() for initial positioning the camera, then call rotateTo() for
// orbital rotation, moveTo()/moveForward() to move camera position only and
// shiftTo() to move position and target together (panning)
//
// Dependencies: Vector2, Vector3, Matrix4, Quaternion, animUtils.h
//
//  AUTHOR: Song Ho Ahn (song.ahn@gmail.com)
// CREATED: 2011-12-02
// UPDATED: 2016-10-24
///////////////////////////////////////////////////////////////////////////////

#ifndef GIL_ORBIT_CAMERA_H
#define GIL_ORBIT_CAMERA_H

#include "Vectors.h"
#include "Matrices.h"
#include "animUtils.h"
#include "Quaternion.h"

class OrbitCamera
{
public:
    OrbitCamera();
    OrbitCamera(const Vector3& position, const Vector3& target);
    ~OrbitCamera();

    void update(float frameTime=0);     // update position, target and matrix during given sec
    void printSelf();

    // set position, target and transform matrix so camera looks at the target
    void lookAt(const Vector3& pos, const Vector3& target);
    void lookAt(const Vector3& pos, const Vector3& target, const Vector3& up);
    void lookAt(float px, float py, float pz, float tx, float ty, float tz);
    void lookAt(float px, float py, float pz, float tx, float ty, float tz, float ux, float uy, float uz);

    // move the camera position to the destination
    // if duration(sec) is greater than 0, it will animate for the given duration
    // otherwise, it will set the position immediately
    // use moveForward() to move the camera forward/backward
    // NOTE: you must call update() before getting the delta movement per frame
    void moveTo(const Vector3& to, float duration=0, Gil::AnimationMode mode=Gil::EASE_OUT);
    void moveForward(float delta, float duration=0, Gil::AnimationMode mode=Gil::EASE_OUT);
    void startForward(float maxSpeed=1.0f, float accel=1.0f);
    void stopForward();

    // pan the camera, shift both position and target point in same direction; left/right/up/down
    // use this function to offset the camera's rotation pivot
    void shiftTo(const Vector3& to, float duration=0, Gil::AnimationMode mode=Gil::EASE_OUT);
    void shift(const Vector2& delta, float duration=0, Gil::AnimationMode mode=Gil::EASE_OUT);
    void startShift(const Vector2& shiftVector, float accel=1.0f);
    void stopShift();

    // rotate the camera around the target point
    // You can use either quaternion or Euler anagles
    void rotateTo(const Vector3& angle, float duration=0.0f, Gil::AnimationMode mode=Gil::EASE_OUT);
    void rotateTo(const Quaternion& quat, float duration=0.0f, Gil::AnimationMode mode=Gil::EASE_OUT);
    void rotate(const Vector3& deltaAngle, float duration=0.0f, Gil::AnimationMode mode=Gil::EASE_OUT);

    // setters
    void setPosition(const Vector3& v);
    void setPosition(float x, float y, float z)     { setPosition(Vector3(x,y,z)); }
    void setTarget(const Vector3& v);
    void setTarget(float x, float y, float z)       { setTarget(Vector3(x,y,z)); }
    void setDistance(float distance);
    void setRotation(const Vector3& v); // angles in degree
    void setRotation(float ax, float ay, float az)  { setRotation(Vector3(ax,ay,az)); }
    void setRotation(const Quaternion& q);

    // getters
    const Vector3& getPosition() const      { return position; }
    const Vector3& getTarget() const        { return target; }
    const Vector3& getAngle() const         { return angle; }
    const Matrix4& getMatrix() const        { return matrix; }
    const float getDistance() const         { return distance; }
    const Quaternion& getQuaternion() const { return quaternion; }

    // return camera's 3 axis vectors
    Vector3 getLeftAxis() const;
    Vector3 getUpAxis() const;
    Vector3 getForwardAxis() const;

protected:

private:
    void updateMove(float frameTime);
    void updateShift(float frameTime);
    void updateForward(float frameTime);
    void updateTurn(float frameTime);
    void computeMatrix();

    // static functions
    static Matrix4 angleToMatrix(const Vector3& angles);
    static Vector3 matrixToAngle(const Matrix4& matrix);
    //static Vector3 lookAtToAngle(const Vector3& position, const Vector3& target);

    // member variables
    Vector3 position;                   // camera position at world space
    Vector3 target;                     // camera focal(lookat) position at world space
    float   distance;                   // distance between position and target
    Vector3 angle;                      // angle in degree around the target (pitch, heading, roll)
    Matrix4 matrix;                     // 4x4 matrix combined rotation and translation
    Matrix4 matrixRotation;             // rotation only
    Quaternion quaternion;              // quaternion for rotations

    // for position movement
    Vector3 movingFrom;                 // camera starting position
    Vector3 movingTo;                   // camera destination position 
    Vector3 movingVector;               // normalized direction vector
    float   movingTime;                 // animation elapsed time (sec)
    float   movingDuration;             // animation duration (sec)
    bool    moving;                     // flag to start/stop animation
    Gil::AnimationMode movingMode;      // interpolation mode

    // for target movement (shift)
    Vector3 shiftingFrom;               // camera starting target
    Vector3 shiftingTo;                 // camera destination target
    Vector3 shiftingVector;             // normalized direction vector
    float   shiftingTime;               // animation elapsed time (sec)
    float   shiftingDuration;           // animation duration (sec)
    float   shiftingSpeed;              // current velocity of shift vector
    float   shiftingAccel;              // acceleration, units per second squared
    float   shiftingMaxSpeed;           // max velocity of shift vector
    bool    shifting;                   // flag to start/stop animation
    Gil::AnimationMode shiftingMode;    // interpolation mode

    // for forwarding using distance between position and target
    float forwardingFrom;               // starting distance
    float forwardingTo;                 // ending distance
    float forwardingTime;               // animation elapsed time (sec)
    float forwardingDuration;           // animation duration (sec)
    float forwardingSpeed;              // current velocity of moving forward
    float forwardingAccel;              // acceleration, units per second squared
    float forwardingMaxSpeed;           // max velocity of moving forward
    bool  forwarding;                   // flag to start/stop forwarding
    Gil::AnimationMode forwardingMode;  // interpolation mode

    // for rotation
    Vector3 turningAngleFrom;           // starting angles
    Vector3 turningAngleTo;             // ending angles
    Quaternion turningQuaternionFrom;   // starting quaternion
    Quaternion turningQuaternionTo;     // ending quaternion
    float   turningTime;                // animation elapsed time (sec)
    float   turningDuration;            // animation duration (sec)
    bool    turning;                    // flag to start/stop rotation
    bool    quaternionUsed;             // flag to use quaternion
    Gil::AnimationMode turningMode;     // interpolation mode
};

#endif
