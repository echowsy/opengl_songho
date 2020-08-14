///////////////////////////////////////////////////////////////////////////////
// animUtils.cpp
// =============
//
//  AUTHOR: Song Ho Ahn (song.ahn@gmail.com)
// CREATED: 2009-04-12
// UPDATED: 2016-06-01
//
// Copyright 2009 Song Ho Ahn. All rights reserved.
///////////////////////////////////////////////////////////////////////////////

#include "animUtils.h"

///////////////////////////////////////////////////////////////////////////////
// return the current keyframe at the given time (sec)
///////////////////////////////////////////////////////////////////////////////
int Gil::getFrame(int frameStart, int frameEnd, float time, float fps, bool loop)
{
    int frame = frameStart + (int)(fps * time + 0.5f);
    if(loop)
    {
        frame = frame % (frameEnd - frameStart + 1);
    }
    else
    {
        if(frame > frameEnd)
            frame = frameEnd;
    }
    return frame;
}



///////////////////////////////////////////////////////////////////////////////
// spherical linear interpolation between 2 3D vectors
// alpha value should be 0 ~ 1
// NOTE: If angle between 2 vectors are 180, the rotation axis cannot be
// determined.
///////////////////////////////////////////////////////////////////////////////
Vector3 Gil::slerp(const Vector3& from, const Vector3& to, float alpha, AnimationMode mode)
{
    // re-compute alpha
    float t = interpolate(0.0f, 1.0f, alpha, mode);

    // determine the angle between
    //@@ FIXME: handle if angle is ~180 degree
    //float dot = from.dot(to);
    float cosine = from.dot(to) / (from.length() * to.length());
    float angle = acosf(cosine);
    float invSine = 1.0f / sinf(angle);

    // compute the scale factors
    float scale1 = sinf((1-t)*angle) * invSine;
    float scale2 = sinf(t*angle) * invSine;

    // compute slerp-ed vector
    return scale1 * from + scale2 * to;
}



///////////////////////////////////////////////////////////////////////////////
// spherical linear interpolation between 2 quaternions
// the alpha should be 0 ~ 1
// assume the quaternions have unit length.
// NOTE: If angle between 2 vectors are 180, the rotation axis cannot be
// determined.
///////////////////////////////////////////////////////////////////////////////
Quaternion Gil::slerp(const Quaternion& from, const Quaternion& to, float alpha, AnimationMode mode)
{
    // re-compute alpha
    float t = interpolate(0.0f, 1.0f, alpha, mode);

    float dot = from.s*to.s + from.x*to.x + from.y*to.y + from.z*to.z;

    // if 2 quaternions are close (angle ~= 0), then use lerp
    if(1 - dot < 0.001f)
    {
        return Quaternion(from + (to - from) * t);
    }
    // if angle is ~180 degree, then the rotation axis is undefined
    // try to find any rotation axis in this case
    else if(fabs(1 + dot) < 0.001f) // dot ~= -1
    {
        Vector3 up, v1, v2;
        v1.set(from.x, from.y, from.z);
        v1.normalize();
        if(fabs(from.x) < 0.001f)
            up.set(1, 0, 0);
        else
            up.set(0, 1, 0);
        v2 = v1.cross(up); // orthonormal to v1
        v2.normalize();
        //std::cout << v2 << std::endl;

        // referenced from Jonathan Blow's Understanding Slerp
        float angle = acosf(dot) * t;
        Vector3 v3 = v1 * cosf(angle) + v2 * sinf(angle);
        return Quaternion(0, v3.x, v3.y, v3.z);
    }

    // determine the angle between
    float angle = acosf(dot);
    float invSine = 1.0f / sinf(angle);

    // compute the scale factors
    float scale1 = sinf((1-t)*angle) * invSine;
    float scale2 = sinf(t*angle) * invSine;

    return Quaternion(from * scale1 + to * scale2);
}



///////////////////////////////////////////////////////////////////////////////
// accelerate / deaccelerate speed
// === PARAMS ===
//  isMoving: accelerate if true, deaccelerate if false
//     speed: the current speed per sec
//  maxSpeed: maximum speed per sec (positive or negative)
//     accel: acceleration per sec squared (always positive)
// deltaTime: frame time in second
///////////////////////////////////////////////////////////////////////////////
float Gil::accelerate(bool isMoving, float speed, float maxSpeed, float accel, float deltaTime)
{
    // determine direction
    float sign;
    if(maxSpeed > 0)
        sign = 1;
    else
        sign = -1;

    // accelerating
    if(isMoving)
    {
        speed += sign * accel * deltaTime;
        if((sign * speed) > (sign * maxSpeed))
            speed = maxSpeed;
    }
    // deaccelerating
    else
    {
        speed -= sign * accel * deltaTime;
        if((sign * speed) < 0)
            speed = 0;
    }

    return speed;
}
