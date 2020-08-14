///////////////////////////////////////////////////////////////////////////////
// BoundingBox.h
// =============
// struct of 3D bounding box
//
//  AUTHOR: Song Ho Ahn (song.ahn@gmail.com)
// CREATED: 2011-03-07
// UPDATED: 2011-12-20
///////////////////////////////////////////////////////////////////////////////

#ifndef BOUNDING_BOX_H
#define BOUNDING_BOX_H

#include <string>
#include <sstream>
#include <iomanip>
#include <cmath>

struct BoundingBox
{
    float minX;
    float maxX;
    float minY;
    float maxY;
    float minZ;
    float maxZ;

    // ctor
    BoundingBox() : minX(0), maxX(0), minY(0), maxY(0), minZ(0), maxZ(0) {}

    void set(float x1, float x2, float y1, float y2, float z1, float z2)
    {
        minX = x1;  maxX = x2;
        minY = y1;  maxY = y2;
        minZ = z1;  maxZ = z2;
    }

    // return center values
    float getCenterX()      { return (maxX + minX) / 2.0f; }
    float getCenterY()      { return (maxY + minY) / 2.0f; }
    float getCenterZ()      { return (maxZ + minZ) / 2.0f; }

    // return the smallest radius to fit
    float getRadiusX()      { return (maxX - minX) / 2.0f; }
    float getRadiusY()      { return (maxY - minY) / 2.0f; }
    float getRadiusZ()      { return (maxZ - minZ) / 2.0f; }
    float getRadius()
    {
        float x = getRadiusX();
        float y = getRadiusY();
        float z = getRadiusZ();
        return sqrtf(x*x+ y*y + z*z);
    }

    // return (minX, minY, minZ) - (maxX, maxY, maxZ)
    const std::string toString() const
    {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(3);

        ss << "(" << minX << ", " << minY << ", " << minZ << ") - "
           << "(" << maxX << ", " << maxY << ", " << maxZ << ")";

        // unset floating format
        ss << std::resetiosflags(std::ios_base::fixed | std::ios_base::floatfield);

        return ss.str();
    }
};

#endif
