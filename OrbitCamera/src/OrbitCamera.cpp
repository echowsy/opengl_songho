///////////////////////////////////////////////////////////////////////////////
// OrbitCamera.cpp
// ===============
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

#include "OrbitCamera.h"
#include <iostream>
#include <cmath>

// constants ==================================================================
const float DEG2RAD = 3.141593f / 180.0f;
const float RAD2DEG = 180.0f / 3.141593f;
const float EPSILON = 0.00001f;




///////////////////////////////////////////////////////////////////////////////
// ctors
///////////////////////////////////////////////////////////////////////////////
OrbitCamera::OrbitCamera() : movingTime(0), movingDuration(0), moving(false),
                             shiftingTime(0), shiftingDuration(0), shiftingSpeed(0),
                             shiftingAccel(0), shiftingMaxSpeed(0), shifting(false),
                             forwardingTime(0), forwardingDuration(0), forwardingSpeed(0),
                             forwardingAccel(0), forwardingMaxSpeed(0), forwarding(false),
                             turningTime(0), turningDuration(0), turning(false),
                             quaternionUsed(false)
{
    quaternion.set(1, 0, 0, 0);
}

OrbitCamera::OrbitCamera(const Vector3& position, const Vector3& target)
                           : movingTime(0), movingDuration(0), moving(false),
                             shiftingTime(0), shiftingDuration(0), shiftingSpeed(0),
                             shiftingAccel(0), shiftingMaxSpeed(0), shifting(false),
                             forwardingTime(0), forwardingDuration(0), forwardingSpeed(0),
                             forwardingAccel(0), forwardingMaxSpeed(0), forwarding(false),
                             turningTime(0), turningDuration(0), turning(false),
                             quaternionUsed(false)
{
    quaternion.set(1, 0, 0, 0);
    lookAt(position, target);
}






///////////////////////////////////////////////////////////////////////////////
// dtor
///////////////////////////////////////////////////////////////////////////////
OrbitCamera::~OrbitCamera()
{
}



///////////////////////////////////////////////////////////////////////////////
// update each frame, frame time is sec
///////////////////////////////////////////////////////////////////////////////
void OrbitCamera::update(float frameTime)
{
    if(moving)
        updateMove(frameTime);
    if(shifting || shiftingSpeed != 0)
        updateShift(frameTime);
    if(forwarding || forwardingSpeed != 0)
        updateForward(frameTime);
    if(turning)
        updateTurn(frameTime);
}



///////////////////////////////////////////////////////////////////////////////
// update position movement only
///////////////////////////////////////////////////////////////////////////////
void OrbitCamera::updateMove(float frameTime)
{
    movingTime += frameTime;
    if(movingTime >= movingDuration)
    {
        setPosition(movingTo);
        moving = false;
    }
    else
    {
        Vector3 p = Gil::interpolate(movingFrom, movingTo,
                                     movingTime/movingDuration, movingMode);
        setPosition(p);
    }
}



///////////////////////////////////////////////////////////////////////////////
// update target movement only
///////////////////////////////////////////////////////////////////////////////
void OrbitCamera::updateShift(float frameTime)
{
    shiftingTime += frameTime;

    // shift with duration
    if(shiftingDuration > 0)
    {
        if(shiftingTime >= shiftingDuration)
        {
            setTarget(shiftingTo);
            shifting = false;
        }
        else
        {
            Vector3 p = Gil::interpolate(shiftingFrom, shiftingTo,
                                         shiftingTime/shiftingDuration, shiftingMode);
            setTarget(p);
        }
    }
    // shift with acceleration
    else
    {
        shiftingSpeed = Gil::accelerate(shifting, shiftingSpeed,
                                        shiftingMaxSpeed, shiftingAccel, frameTime);
        setTarget(target + (shiftingVector * shiftingSpeed * frameTime));
    }
}



///////////////////////////////////////////////////////////////////////////////
// update forward movement only
///////////////////////////////////////////////////////////////////////////////
void OrbitCamera::updateForward(float frameTime)
{
    forwardingTime += frameTime;

    // move forward for duration
    if(forwardingDuration > 0)
    {
        if(forwardingTime >= forwardingDuration)
        {
            setDistance(forwardingTo);
            forwarding = false;
        }
        else
        {
            float d = Gil::interpolate(forwardingFrom, forwardingTo,
                                       forwardingTime/forwardingDuration, forwardingMode);
            setDistance(d);
        }
    }

    // move forward with acceleration
    else
    {
        forwardingSpeed = Gil::accelerate(forwarding, forwardingSpeed,
                                          forwardingMaxSpeed, forwardingAccel, frameTime);
        setDistance(distance - forwardingSpeed * frameTime);
    }
}



///////////////////////////////////////////////////////////////////////////////
// update rotation only
///////////////////////////////////////////////////////////////////////////////
void OrbitCamera::updateTurn(float frameTime)
{
    turningTime += frameTime;
    if(turningTime >= turningDuration)
    {
        if(quaternionUsed)
            setRotation(turningQuaternionTo);
        else
            setRotation(turningAngleTo);
        turning = false;
    }
    else
    {
        if(quaternionUsed)
        {
            Quaternion q = Gil::slerp(turningQuaternionFrom, turningQuaternionTo,
                                      turningTime/turningDuration, turningMode);
            setRotation(q);
        }
        else
        {
            Vector3 p = Gil::interpolate(turningAngleFrom, turningAngleTo,
                                         turningTime/turningDuration, turningMode);
            setRotation(p);
        }
    }
}



///////////////////////////////////////////////////////////////////////////////
// print itself
///////////////////////////////////////////////////////////////////////////////
void OrbitCamera::printSelf()
{
    std::cout << "===== OrbitCamera =====\n"
              << "  Position: " << position   << "\n"
              << "    Target: " << target     << "\n"
              << "    Matrix:\n"<< matrix     << "\n"
              << std::endl;
}



///////////////////////////////////////////////////////////////////////////////
// set position of camera, set transform matrix as well
///////////////////////////////////////////////////////////////////////////////
void OrbitCamera::setPosition(const Vector3& v)
{
    lookAt(v, target);
}



///////////////////////////////////////////////////////////////////////////////
// set target of camera, then rebuild matrix
// rotation parts are not changed, but translation part must be recalculated
// And, position is also shifted
///////////////////////////////////////////////////////////////////////////////
void OrbitCamera::setTarget(const Vector3& v)
{
    target = v;

    // forward vector of camera
    Vector3 forward(-matrix[2], -matrix[6], -matrix[10]);
    position = target - (distance * forward);
    computeMatrix();
}



///////////////////////////////////////////////////////////////////////////////
// set distance of camera, then recompute camera position
///////////////////////////////////////////////////////////////////////////////
void OrbitCamera::setDistance(float d)
{
    distance = d;
    computeMatrix();
}



///////////////////////////////////////////////////////////////////////////////
// set transform matrix equivalent to gluLookAt()
// 1. Mt: Translate scene to camera position inversely, (-x, -y, -z)
// 2. Mr: Rotate scene inversly so camera looks at the scene
// 3. Find matrix = Mr * Mt
//       Mr               Mt
// |r0  r4  r8  0|   |1  0  0 -x|   |r0  r4  r8  r0*-x + r4*-y + r8 *-z|
// |r1  r5  r9  0| * |0  1  0 -y| = |r1  r5  r9  r1*-x + r5*-y + r9 *-z|
// |r2  r6  r10 0|   |0  0  1 -z|   |r2  r6  r10 r2*-x + r6*-y + r10*-z|
// |0   0   0   1|   |0  0  0  1|   |0   0   0   1                     |
///////////////////////////////////////////////////////////////////////////////
void OrbitCamera::lookAt(const Vector3& position, const Vector3& target)
{
    // remeber the camera posision & target position
    this->position = position;
    this->target = target;

    // if pos and target are same, only translate camera to position without rotation
    if(position == target)
    {
        matrix.identity();
        matrix.setColumn(3, -position);
        // rotation stuff
        matrixRotation.identity();
        angle.set(0,0,0);
        quaternion.set(1,0,0,0);
        return;
    }

    Vector3 left, up, forward;  // 3 axis of matrix for scene

    // first, compute the forward vector of rotation matrix
    // NOTE: the direction is reversed (target to camera pos) because of camera transform
    forward = position - target;
    this->distance = forward.length();  // remember the distance
    // normalize
    forward /= this->distance;

    // compute temporal up vector based on the forward vector
    // watch out when look up/down at 90 degree
    // for example, forward vector is on the Y axis
    if(fabs(forward.x) < EPSILON && fabs(forward.z) < EPSILON)
    {
        // forward vector is pointing +Y axis
        if(forward.y > 0)
        {
            up = Vector3(0, 0, -1);
        }
        // forward vector is pointing -Y axis
        else
        {
            up = Vector3(0, 0, 1);
        }
    }
    // in general, up vector is straight up
    else
    {
        up = Vector3(0, 1, 0);
    }

    // compute the left vector of rotation matrix
    left = up.cross(forward);   // cross product
    left.normalize();

    // re-calculate the orthonormal up vector
    up = forward.cross(left);   // cross product

    // set inverse rotation matrix: M^-1 = M^T for Euclidean transform
    matrixRotation.identity();
    matrixRotation.setRow(0, left);
    matrixRotation.setRow(1, up);
    matrixRotation.setRow(2, forward);

    // copy it to matrix
    matrix.identity();
    matrix.setRow(0, left);
    matrix.setRow(1, up);
    matrix.setRow(2, forward);

    // set translation part
    Vector3 trans;
    trans.x = matrix[0]*-position.x + matrix[4]*-position.y + matrix[8]*-position.z;
    trans.y = matrix[1]*-position.x + matrix[5]*-position.y + matrix[9]*-position.z;
    trans.z = matrix[2]*-position.x + matrix[6]*-position.y + matrix[10]*-position.z;
    matrix.setColumn(3, trans);

    // set Euler angles
    angle = matrixToAngle(matrixRotation);

    // set quaternion from angle
    Vector3 reversedAngle(angle.x, -angle.y, angle.z);
    quaternion = Quaternion::getQuaternion(reversedAngle * DEG2RAD * 0.5f); // half angle

    //DEBUG
    //std::cout << matrixRotation << std::endl;
}



///////////////////////////////////////////////////////////////////////////////
// set transform matrix with target and camera's up vectors
///////////////////////////////////////////////////////////////////////////////
void OrbitCamera::lookAt(const Vector3& position, const Vector3& target, const Vector3& upDir)
{
    // remeber the camera posision & target position
    this->position = position;
    this->target = target;

    // if pos and target are same, only translate camera to position without rotation
    if(position == target)
    {
        matrix.identity();
        matrix.translate(-position.x, -position.y, -position.z);
        // rotation stuff
        matrixRotation.identity();
        angle.set(0,0,0);
        quaternion.set(1,0,0,0);
        return;
    }

    Vector3 left, up, forward;          // 3 axis vectors for scene

    // compute the forward vector
    // NOTE: the direction is reversed (target to camera pos) because of camera transform
    forward = position - target;
    this->distance = forward.length();  // remember the distance
    // normalize
    forward /= this->distance;

    // compute the left vector
    left = upDir.cross(forward);        // cross product
    left.normalize();

    // recompute the orthonormal up vector
    up = forward.cross(left);           // cross product
    //up.normalize();

    // set inverse rotation matrix: M^-1 = M^T for Euclidean transform
    matrixRotation.identity();
    matrixRotation.setRow(0, left);
    matrixRotation.setRow(1, up);
    matrixRotation.setRow(2, forward);

    // copy it to matrix
    matrix.identity();
    matrix.setRow(0, left);
    matrix.setRow(1, up);
    matrix.setRow(2, forward);

    // set translation
    Vector3 trans;
    trans.x = matrix[0]*-position.x + matrix[4]*-position.y + matrix[8]*-position.z;
    trans.y = matrix[1]*-position.x + matrix[5]*-position.y + matrix[9]*-position.z;
    trans.z = matrix[2]*-position.x + matrix[6]*-position.y + matrix[10]*-position.z;
    matrix.setColumn(3, trans);

    // set Euler angles
    angle = matrixToAngle(matrixRotation);

    // set quaternion from angle
    Vector3 reversedAngle(angle.x, -angle.y, angle.z);
    quaternion = Quaternion::getQuaternion(reversedAngle * DEG2RAD * 0.5f); // half angle

    //DEBUG
    //std::cout << matrixRotation << std::endl;
}



void OrbitCamera::lookAt(float px, float py, float pz,
                         float tx, float ty, float tz)
{
    lookAt(Vector3(px,py,pz), Vector3(tx,ty,tz));
}

void OrbitCamera::lookAt(float px, float py, float pz,
                         float tx, float ty, float tz,
                         float ux, float uy, float uz)
{
    lookAt(Vector3(px,py,pz), Vector3(tx,ty,tz), Vector3(ux,uy,uz));
}



///////////////////////////////////////////////////////////////////////////////
// set transform matrix with rotation angles (degree)
// NOTE: the angle is for camera, so yaw value must be negated for computation.
//
// The order of rotation is Roll->Yaw->Pitch (Rx*Ry*Rz)
// Rx: rotation about X-axis, pitch
// Ry: rotation about Y-axis, yaw(heading)
// Rz: rotation about Z-axis, roll
//    Rx           Ry          Rz
// |1  0   0| | Cy  0 Sy| |Cz -Sz 0|   | CyCz        -CySz         Sy  |
// |0 Cx -Sx|*|  0  1  0|*|Sz  Cz 0| = | SxSyCz+CxSz -SxSySz+CxCz -SxCy|
// |0 Sx  Cx| |-Sy  0 Cy| | 0   0 1|   |-CxSyCz+SxSz  CxSySz+SxCz  CxCy|
///////////////////////////////////////////////////////////////////////////////
void OrbitCamera::setRotation(const Vector3& angle)
{
    // remember angles
    // NOTE: assume all angles are already reversed for camera
    this->angle = angle;

    // remember quaternion value
    // NOTE: yaw must be negated again for quaternion
    Vector3 reversedAngle(angle.x, -angle.y, angle.z);
    this->quaternion = Quaternion::getQuaternion(reversedAngle);

    // compute rotation matrix from angle
    matrixRotation = angleToMatrix(angle);

    // construct camera matrix
    computeMatrix();

    //DEBUG
    //std::cout << angle <<std::endl;
}



///////////////////////////////////////////////////////////////////////////////
// set rotation with new quaternion
// NOTE: quaternion value is for matrix, so matrixToAngle() will reverse yaw.
///////////////////////////////////////////////////////////////////////////////
void OrbitCamera::setRotation(const Quaternion& q)
{
    // remember the current quaternion
    quaternion = q;

    // quaternion to matrix
    matrixRotation = q.getMatrix();

    // construct matrix
    computeMatrix();

    // compute angle from matrix
    angle = matrixToAngle(matrixRotation);
}



///////////////////////////////////////////////////////////////////////////////
// construct camera matrix: M = Mt2 * Mr * Mt1
// where Mt1: move scene to target (-x,-y,-z)
//       Mr : rotate scene at the target point
//       Mt2: move scene away from target with distance -d
//
//     Mt2             Mr               Mt1
// |1  0  0  0|   |r0  r4  r8  0|   |1  0  0 -x|   |r0  r4  r8  r0*-x + r4*-y + r8*-z     |
// |0  1  0  0| * |r1  r5  r9  0| * |0  1  0 -y| = |r1  r5  r9  r1*-x + r5*-y + r9*-z     |
// |0  0  1 -d|   |r2  r6  r10 0|   |0  0  1 -z|   |r2  r6  r10 r2*-x + r6*-y + r10*-z - d|
// |0  0  0  1|   |0   0   0   1|   |0  0  0  1|   |0   0   0   1                         |
///////////////////////////////////////////////////////////////////////////////
void OrbitCamera::computeMatrix()
{
    // extract left/up/forward vectors from rotation matrix
    Vector3 left(matrixRotation[0], matrixRotation[1], matrixRotation[2]);
    Vector3 up(matrixRotation[4], matrixRotation[5], matrixRotation[6]);
    Vector3 forward(matrixRotation[8], matrixRotation[9], matrixRotation[10]);

    // compute translation vector
    Vector3 trans;
    trans.x = left.x * -target.x + up.x * -target.y + forward.x * -target.z;
    trans.y = left.y * -target.x + up.y * -target.y + forward.y * -target.z;
    trans.z = left.z * -target.x + up.z * -target.y + forward.z * -target.z - distance;

    // construct matrix
    matrix.identity();
    matrix.setColumn(0, left);
    matrix.setColumn(1, up);
    matrix.setColumn(2, forward);
    matrix.setColumn(3, trans);

    // re-compute camera position
    forward.set(-matrix[2], -matrix[6], -matrix[10]);
    position = target - (distance * forward);

    /*@@
    //DEBUG: equivalent to the above matrix computation
    matrix.identity();
    matrix.translate(-target.x, -target.y, -target.z); // Mt1: move scene to target point
    matrix = matrixRotation * matrix;                  // Mr : rotate scene at the target point
    matrix.translate(0, 0, -distance);                 // Mt2: move scene away from the target with distance

    // re-compute camera position
    // NOTE: camera's forward vector is the forward vector of inverse matrix
    Vector3 forward(-matrix[2], -matrix[6], -matrix[10]);
    position = target - (distance * forward);
    */
}



///////////////////////////////////////////////////////////////////////////////
// return left, up, forward axis
///////////////////////////////////////////////////////////////////////////////
Vector3 OrbitCamera::getLeftAxis() const
{
    return Vector3(-matrix[0], -matrix[4], -matrix[8]);
}

Vector3 OrbitCamera::getUpAxis() const
{
    return Vector3(matrix[1], matrix[5], matrix[9]);
}

Vector3 OrbitCamera::getForwardAxis() const
{
    return Vector3(-matrix[2], -matrix[6], -matrix[10]);
}



///////////////////////////////////////////////////////////////////////////////
// move the camera position with the given duration
///////////////////////////////////////////////////////////////////////////////
void OrbitCamera::moveTo(const Vector3& to, float duration, Gil::AnimationMode mode)
{
    if(duration <= 0.0f)
    {
        setPosition(to);
    }
    else
    {
        movingFrom = position;
        movingTo = to;
        movingVector = movingTo - movingFrom;
        movingVector.normalize();
        movingTime = 0;
        movingDuration = duration;
        movingMode = mode;
        moving = true;
    }
}



///////////////////////////////////////////////////////////////////////////////
// pan the camera target left/right/up/down with the given duration
// the camera position will be shifted after transform
///////////////////////////////////////////////////////////////////////////////
void OrbitCamera::shiftTo(const Vector3& to, float duration, Gil::AnimationMode mode)
{
    if(duration <= 0.0f)
    {
        setTarget(to);
    }
    else
    {
        shiftingFrom = target;
        shiftingTo = to;
        shiftingVector = shiftingTo - shiftingFrom;
        shiftingVector.normalize();
        shiftingTime = 0;
        shiftingDuration = duration;
        shiftingMode = mode;
        shifting = true;
    }
}



///////////////////////////////////////////////////////////////////////////////
// shift the camera position and target left/right/up/down
///////////////////////////////////////////////////////////////////////////////
void OrbitCamera::shift(const Vector2& delta, float duration, Gil::AnimationMode mode)
{
    // get left & up vectors of camera
    Vector3 cameraLeft(-matrix[0], -matrix[4], -matrix[8]);
    Vector3 cameraUp(-matrix[1], -matrix[5], -matrix[9]);

    // compute delta movement
    Vector3 deltaMovement = delta.x * cameraLeft;
    deltaMovement += -delta.y * cameraUp;   // reverse up direction

    // find new target position
    Vector3 newTarget = target + deltaMovement;

    shiftTo(newTarget, duration, mode);
}



///////////////////////////////////////////////////////////////////////////////
// start accelerating to shift camera
// It takes shift direction vector and acceleration per squared sec.
// acceleration should be always positive.
///////////////////////////////////////////////////////////////////////////////
void OrbitCamera::startShift(const Vector2& shiftVector, float accel)
{
    // get left & up vectors of camera
    Vector3 cameraLeft(-matrix[0], -matrix[4], -matrix[8]);
    Vector3 cameraUp(-matrix[1], -matrix[5], -matrix[9]);

    // compute new target vector
    Vector3 vector = shiftVector.x * cameraLeft;
    vector += -shiftVector.y * cameraUp;   // reverse up direction

    shiftingMaxSpeed = shiftVector.length();
    shiftingVector = vector;
    shiftingVector.normalize();
    shiftingSpeed = 0;
    shiftingAccel = accel;
    shiftingTime = 0;
    shiftingDuration = 0;
    shifting = true;
}

void OrbitCamera::stopShift()
{
    shifting = false;
}



///////////////////////////////////////////////////////////////////////////////
// zoom in/out the camera position with the given delta movement and duration
// it actually moves the camera forward or backward.
// positive delta means moving forward (decreasing distance)
///////////////////////////////////////////////////////////////////////////////
void OrbitCamera::moveForward(float delta, float duration, Gil::AnimationMode mode)
{
    if(duration <= 0.0f)
    {
        setDistance(distance - delta);
    }
    else
    {
        forwardingFrom = distance;
        forwardingTo = distance - delta;
        forwardingTime = 0;
        forwardingDuration = duration;
        forwardingMode = mode;
        forwarding = true;
    }
}



///////////////////////////////////////////////////////////////////////////////
// start accelerating to move forward
// It takes maximum speed per sec and acceleration per squared sec.
// positive speed means moving forward (decreasing distance).
// acceleration should be always positive.
///////////////////////////////////////////////////////////////////////////////
void OrbitCamera::startForward(float maxSpeed, float accel)
{
    forwardingSpeed = 0;
    forwardingMaxSpeed = maxSpeed;
    forwardingAccel = accel;
    forwardingTime = 0;
    forwardingDuration = 0;
    forwarding = true;
}

void OrbitCamera::stopForward()
{
    forwarding = false;
}



///////////////////////////////////////////////////////////////////////////////
// rotate camera to the given angle with duration
///////////////////////////////////////////////////////////////////////////////
void OrbitCamera::rotateTo(const Vector3& angle, float duration, Gil::AnimationMode mode)
{
    quaternionUsed = false;
    if(duration <= 0.0f)
    {
        setRotation(angle);
    }
    else
    {
        turningAngleFrom = this->angle;
        turningAngleTo = angle;
        turningTime = 0;
        turningDuration = duration;
        turningMode = mode;
        turning = true;
    }
}



///////////////////////////////////////////////////////////////////////////////
// rotate camera to the given quaternion with duration
///////////////////////////////////////////////////////////////////////////////
void OrbitCamera::rotateTo(const Quaternion& q, float duration, Gil::AnimationMode mode)
{
    quaternionUsed = true;
    if(duration <= 0.0f)
    {
        setRotation(q);
    }
    else
    {
        turningQuaternionFrom = this->quaternion;
        turningQuaternionTo = q;
        turningTime = 0;
        turningDuration = duration;
        turningMode = mode;
        turning = true;
    }
}



///////////////////////////////////////////////////////////////////////////////
// rotate camera with delta angle
// NOTE: delta angle must be negated already
///////////////////////////////////////////////////////////////////////////////
void OrbitCamera::rotate(const Vector3& delta, float duration, Gil::AnimationMode mode)
{
    rotateTo(angle + delta, duration, mode);
}



// static functions ===========================================================
///////////////////////////////////////////////////////////////////////////////
// convert rotation angles (degree) to 4x4 matrix
// NOTE: the angle is for orbit camera, so yaw angle must be reversed before
// matrix computation.
//
// The order of rotation is Roll->Yaw->Pitch (Rx*Ry*Rz)
// Rx: rotation about X-axis, pitch
// Ry: rotation about Y-axis, yaw(heading)
// Rz: rotation about Z-axis, roll
//    Rx           Ry          Rz
// |1  0   0| | Cy  0 Sy| |Cz -Sz 0|   | CyCz        -CySz         Sy  |
// |0 Cx -Sx|*|  0  1  0|*|Sz  Cz 0| = | SxSyCz+CxSz -SxSySz+CxCz -SxCy|
// |0 Sx  Cx| |-Sy  0 Cy| | 0   0 1|   |-CxSyCz+SxSz  CxSySz+SxCz  CxCy|
///////////////////////////////////////////////////////////////////////////////
Matrix4 OrbitCamera::angleToMatrix(const Vector3& angle)
{
    float sx, sy, sz, cx, cy, cz, theta;
    Vector3 left, up, forward;

    // rotation angle about X-axis (pitch)
    theta = angle.x * DEG2RAD;
    sx = sinf(theta);
    cx = cosf(theta);

    // rotation angle about Y-axis (yaw)
    theta = -angle.y * DEG2RAD;
    sy = sinf(theta);
    cy = cosf(theta);

    // rotation angle about Z-axis (roll)
    theta = angle.z * DEG2RAD;
    sz = sinf(theta);
    cz = cosf(theta);

    // determine left axis
    left.x = cy*cz;
    left.y = sx*sy*cz + cx*sz;
    left.z = -cx*sy*cz + sx*sz;

    // determine up axis
    up.x = -cy*sz;
    up.y = -sx*sy*sz + cx*cz;
    up.z = cx*sy*sz + sx*cz;

    // determine forward axis
    forward.x = sy;
    forward.y = -sx*cy;
    forward.z = cx*cy;

    // construct rotation matrix
    Matrix4 matrix;
    matrix.setColumn(0, left);
    matrix.setColumn(1, up);
    matrix.setColumn(2, forward);

    return matrix;
}



///////////////////////////////////////////////////////////////////////////////
// retrieve angles in degree from rotation matrix, M = Rx*Ry*Rz
// Rx: rotation about X-axis, pitch
// Ry: rotation about Y-axis, yaw(heading)
// Rz: rotation about Z-axis, roll
//    Rx           Ry          Rz
// |1  0   0| | Cy  0 Sy| |Cz -Sz 0|   | CyCz        -CySz         Sy  |
// |0 Cx -Sx|*|  0  1  0|*|Sz  Cz 0| = | SxSyCz+CxSz -SxSySz+CxCz -SxCy|
// |0 Sx  Cx| |-Sy  0 Cy| | 0   0 1|   |-CxSyCz+SxSz  CxSySz+SxCz  CxCy|
//
// Pitch: atan(-m[7] / m[8]) = atan(SxCy/CxCy)
// Yaw  : asin(m[6])         = asin(Sy)
// Roll : atan(-m[3] / m[0]) = atan(SzCy/CzCy)
///////////////////////////////////////////////////////////////////////////////
Vector3 OrbitCamera::matrixToAngle(const Matrix4& matrix)
{
    Vector3 angle = matrix.getAngle();

    // reverse yaw
    angle.y = -angle.y;
    angle.z = angle.z;
    return angle;
}



/*
///////////////////////////////////////////////////////////////////////////////
// compute Euler angles(degree) from given 2 points(position and target)
// NOTE: Roll angle cannot be computed with this function because of limitation
// of params. It assumes the roll angle is 0.
// Rx: rotation about X-axis, pitch
// Ry: rotation about Y-axis, yaw(heading)
///////////////////////////////////////////////////////////////////////////////
Vector3 OrbitCamera::lookAtToAngle(const Vector3& position, const Vector3& target)
{
    Vector3 vec;
    float  yaw, pitch;                          // roll is 0

    // compute the vector from position to target point
    vec = target - position;

    if(vec.x == 0.0f && vec.y == 0.0f)          // vector is on the Y-axis, therefore,
    {                                           // Yaw is 0, and Pitch will be +90 or -90.
        yaw = 0.0f;
        if(vec.y >= 0.0f)
            pitch = 90.0f;                      // facing along +Y
        else
            pitch = -90.0f;                     // facing along -Y
    }
    else
    {
        // yaw: angle on X-Z plane (heading)
        // yaw should be 0 if facing along +Z initially
        yaw = RAD2DEG * atan2f(-vec.x, vec.z);  // range -pi ~ +pi

        // length of vector projected on X-Z plane
        float dxz = sqrtf(vec.x*vec.x + vec.z*vec.z);
        pitch = RAD2DEG * atan2f(vec.y, dxz);   // range -pi ~ +pi
    }

    // return angles(degree)
    return Vector3(pitch, yaw, 0);
}
*/
