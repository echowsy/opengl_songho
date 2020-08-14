///////////////////////////////////////////////////////////////////////////////
// animUtils.h
// ===========
//
//  AUTHOR: Song Ho Ahn (song.ahn@gmail.com)
// CREATED: 2009-04-12
// UPDATED: 2016-06-01
//
// Copyright 2009 Song Ho Ahn. All rights reserved.
///////////////////////////////////////////////////////////////////////////////

#ifndef GIL_ANIM_UTILS_H
#define GIL_ANIM_UTILS_H

#include <cmath>
#include "Vectors.h"
#include "Quaternion.h"

namespace Gil
{

// enums
//namespace AnimationMode
//{
    enum AnimationMode  // enum
    {
        LINEAR = 0,
        EASE_IN,
        EASE_OUT,
        EASE_IN_OUT,
        BOUNCE,
        ELASTIC
    };
//}



// get current frame at given time (sec)
// - return the current frame number
int getFrame(int frameStart, int frameEnd, float elapsedTime, float frameRate=30, bool loop=true);



// spherical linear interpolation between 3D vectors
// alpha should be 0 ~ 1
Vector3 slerp(const Vector3& from, const Vector3& to, float alpha, AnimationMode mode=LINEAR);



// spherical linear interpolation between 2 quaternions
// the alpha value should be 0 ~ 1
Quaternion slerp(const Quaternion& from, const Quaternion& to, float alpha, AnimationMode mode=LINEAR);



// accelerate / deaccelerate speed
// === PARAMS ===
//  isMoving: accelerate if true, deaccelerate if false
// currSpeed: the current speed
//  maxSpeed: maximum speed (positive or negative)
//     accel: acceleration (always positive)
// deltaTime: frame time in second
float accelerate(bool isMoving, float currSpeed, float maxSpeed, float accel, float deltaTime);



// move from one point to the other
// - the result will be stored in "vec"
// - "speed" param is units/sec
// - return "true" when it reaches "to" point
template <class T>
bool move(T& vec, const T& from, const T& to, float elapsedTime, float speed)
{
    // check if it is moving to itself
    if(from == to)
    {
        vec = to;
        return true;
    }

    bool done = false;

    vec = to - from;
    float length1 = vec.length();

    vec.normalize();
    float distance = elapsedTime * speed;
    vec *= distance;
    float length2 = vec.length();

    vec = from + vec;
    if(length2 > length1)
    {
        vec = to;
        done = true;
    }

    return done;
}



// linear interpolation: P = (1-t)*P1 + t* P2
// use interpolate() for non-linear interpolations
template <class T>
T lerp(const T& from, const T& to, float alpha)
{
    return from + alpha * (to - from);
}



// interpolate from one point to the other
// - "alpha" param is interpolation value (0 ~ 1)
// - "mode" param is animation mode
// - return new vector after interpolation
template <class T>
T interpolate(const T& from, const T& to, float alpha, AnimationMode mode)
{
    //const float PI = 3.141593f;
    //const float HALF_PI = 3.141593f * 0.5f;

    // recompute alpha based on animation mode
    if(mode == EASE_IN)
    {
        //@@alpha = 1 - cosf(HALF_PI * alpha);
        // with cubic function
        alpha = alpha * alpha * alpha;
    }
    else if(mode == EASE_OUT)
    {
        //@@alpha = sinf(HALF_PI * alpha);
        // with cubic function
        float beta = 1 - alpha;
        alpha = 1 - beta * beta * beta;
    }
    else if(mode == EASE_IN_OUT)
    {
        //@@alpha = 0.5f * (1 - cosf(PI * alpha));
        // with cubic function
        float beta = 1 - alpha;
        float scale = 4.0f;     // 0.5 / (0.5^3)
        if(alpha < 0.5f)
            alpha = alpha * alpha * alpha * scale;
        else
            alpha = 1 - (beta * beta * beta * scale);
    }
    else if(mode == BOUNCE)
    {
    }
    else if(mode == ELASTIC)
    {
    }

    return from + alpha * (to - from);
}



/*
// interpolate from one point to the other
// - "dir" param is the normalized direction vector
// - "distance" param is total length of movement
// - "alpha" param is interpolation value (0 ~ 1)
// - "mode" param is animation mode
// - return new vector after interpolation
template <class T>
T interpolate(const T& from, const T& dir, float distance, float alpha, AnimationMode mode)
{
    const float PI = 3.141592f;
    const float HALF_PI = 3.141592f * 0.5f;

    // recompute alpha based on animation mode
    if(mode == EASE_IN)
    {
        alpha = 1 - cosf(HALF_PI * alpha);
    }
    else if(mode == EASE_OUT)
    {
        alpha = sinf(HALF_PI * alpha);
    }
    else if(mode == EASE_IN_OUT)
    {
        alpha = 0.5f * (1 - cosf(PI * alpha));
    }
    else if(mode == BOUNCE)
    {
    }
    else if(mode == ELASTIC)
    {
    }

    return from + ((distance * alpha) * dir);
}
*/



} // namespace

#endif
