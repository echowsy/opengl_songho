///////////////////////////////////////////////////////////////////////////////
// Vertices.h
// ==========
// Interleaved vertex data structure has 
//
//  AUTHOR: Song Ho Ahn
// CREATED: 2009-04-04
// UPDATED: 2009-04-04
//
// Copyright 2009 Song Ho Ahn. All rights reserved.
///////////////////////////////////////////////////////////////////////////////

#ifndef VERTICES_H
#define VERTICES_H

// 2D vertex, no need normal //////////////////////////////////////////////////
struct Vertex2
{
    float x, y;
    float s, t;

    Vertex2() : x(0), y(0), s(0), t(0) {}
    Vertex2(float x, float y, float s, float t) : x(x), y(y), s(s), t(t) {}
};



struct Vertex2t2
{
    float x, y;
    float s0, t0, s1, t1;
    
    Vertex2t2() : x(0), y(0), s0(0), t0(0), s1(0), t1(0) {}
};



// 3D vertex //////////////////////////////////////////////////////////////////
struct Vertex3
{
    float x, y, z;
    float nx, ny, nz;
    float s, t;
    
    Vertex3() : x(0), y(0), z(0), nx(0), ny(0), nz(0), s(0), t(0) {}
    Vertex3(float x, float y, float z, float nx, float ny, float nz, float s, float t) :
            x(x), y(y), z(z), nx(nx), ny(ny), nz(nz), s(s), t(t) {}
};



struct Vertex3t2
{
    float x, y, z;
    float nx, ny, nz;
    float s0, t0, s1, t1;
    
    Vertex3t2() : x(0), y(0), z(0), nx(0), ny(0), nz(0), s0(0), t0(0), s1(0), t1(0) {}
};


#endif
