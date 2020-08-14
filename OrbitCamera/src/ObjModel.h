///////////////////////////////////////////////////////////////////////////////
// ObjModel.h
// ==========
// Wavefront 3D object (.obj and .mtl) loader
//
//  AUTHOR: Song Ho Ahn (ssong.ahn@gmail.com)
// CREATED: 2003-05-16
// UPDATED: 2014-04-01
///////////////////////////////////////////////////////////////////////////////

#ifndef OBJ_MODEL_H
#define OBJ_MODEL_H

#include <vector>
#include <string>
#include <map>
#include <sstream>
#include "BoundingBox.h"
#include "Vectors.h"


// constants //////////////////////////////////////////////////////////////////
const float SMOOTH_ANGLE = 90.0f;   // degree



///////////////////////////////////////////////////////////////////////////////
struct ObjMaterial
{
    std::string name;
    std::string textureName;
    float ambient[4];
    float diffuse[4];
    float specular[4];
    float shininess;

    // ctors
    ObjMaterial()
    {
        ambient[0] = ambient[1] = ambient[2] = 0;
        ambient[3] = 1;

        diffuse[0] = diffuse[1] = diffuse[2] = diffuse[3] = 1;

        specular[0] = specular[1] = specular[2] = specular[3] = 1;

        shininess = 128;
    }

    // setter
    void setAmbient(float r, float g, float b, float a)
    {
        ambient[0] = r; ambient[1] = g; ambient[2] = b; ambient[3] = a;
    }
    void setDiffuse(float r, float g, float b, float a)
    {
        diffuse[0] = r; diffuse[1] = g; diffuse[2] = b; diffuse[3] = a;
    }
    void setSpecular(float r, float g, float b, float a)
    {
        specular[0] = r; specular[1] = g; specular[2] = b; specular[3] = a;
    }
};



///////////////////////////////////////////////////////////////////////////////
struct ObjGroup
{
    std::string name;
    std::string materialName;           // "usemtl"
    unsigned int indexOffset;           // starting position of indices for this group
    unsigned int indexCount;            // number of indices for this group

    ObjGroup() : indexOffset(0), indexCount(0) {}
};



///////////////////////////////////////////////////////////////////////////////
class ObjModel
{
public:
    ObjModel();
    ~ObjModel();

    // load obj file
    bool read(const char* file);
    bool save(const char* file, bool textured=true, const float* matrix=NULL);

    // re-generate and soften normals
    void smoothNormals(float smoothAngle = SMOOTH_ANGLE);

    // remove duplicated vertices
    void removeDuplicates();

    // vertex attributes
    unsigned int getVertexCount() const         { return (unsigned int)vertices.size() / 3; }
    unsigned int getNormalCount() const         { return (unsigned int)normals.size() / 3; }
    unsigned int getTexCoordCount() const       { return (unsigned int)texCoords.size() / 2; }
    unsigned int getIndexCount() const          { return (unsigned int)indices.size(); }  // total
    unsigned int getTriangleCount() const       { return (unsigned int)indices.size() / 3; }
    const BoundingBox& getBoundingBox() const   { return bound; }

    // return data as 1D array
    const float* getVertices() const            { return &vertices[0]; }
    const float* getNormals() const             { return &normals[0]; }
    const float* getTexCoords() const           { return &texCoords[0]; }

    // group attributes
    int getGroupCount() const                   { return (int)groups.size(); }
    int getMaterialCount() const                { return (int)materials.size(); }
    std::string getGroupName(int index) const;
    std::string getMaterialName(int index) const;
    void setMaterial(int index, const ObjMaterial& mtl);
    const ObjMaterial& getMaterial(int index);
    unsigned int getIndexOffset(int groupId) const;
    unsigned int getIndexCount(int groupId) const;          // per group
    const unsigned int* getIndices(int groupId=0) const;    // if groupIdx omitted, return the beginning of array

    // for interleaved vertices: V/N or V/N/T
    // NOTE: interleaved vertex array will be built automatically
    //       if getInterleavedVertices() is first called. And, stride, interleaved
    //       vertex count and size are only valid after getInterleavedVertices() called
    const float* getInterleavedVertices();          // return interleaved data
    int getInterleavedStride() const                { return stride; }
    unsigned int getInterleavedVertexCount() const  { return getVertexCount(); }
    unsigned int getInterleavedVertexSize() const   { return (unsigned int)interleavedVertices.size() * sizeof(float); } // # of bytes

    // for path & file names
    const std::string& getObjFileName() const   { return objFileName; }
    const std::string& getMtlFileName() const   { return mtlFileName; }
    const std::string& getObjDirectory() const  { return objDirectory; }

    const std::string& getErrorMessage() const  { return errorMessage; }
    void printSelf() const;

protected:


private:
    void init();
    void parseVertexLookup(const std::vector<std::string>& lines);      // parse "v" lines
    void parseNormalLookup(const std::vector<std::string>& lines);      // parse "vn" lines
    void parseTexCoordLookup(const std::vector<std::string>& lines);    // parse "vt" lines
    void parseFaces(const std::vector<std::string>& lines);             // parse "f" lines and other tags
    void parseMesh(const std::vector<std::string>& lines);              // old parser
    bool parseMaterial(const std::string& mtlFile);
    void convertToTriangles(std::vector<std::string>& faceIndices);
    void createGroup(const std::string& groupName);
    void addFace(const std::vector<std::string>& faceIndices);
    void computeBoundingBox();
    Vector3 computeFaceNormal(const Vector3& v1, const Vector3& v2, const Vector3& v3);
    void splitFaces();
    void findDuplicates();
    void joinFaces();
    void averageNormals(float smoothAngle = SMOOTH_ANGLE);
    int  findMaterial(const std::string& name);
    int  findGroup(const std::string& name);

    // for interleaved vertices: V/N or V/N/T
    int buildInterleavedVertices();             // create interleaved data and return stride
    void buildInterleavedVerticesVN();
    void buildInterleavedVerticesVNT();

    // transform a vertex with 4x4 matrix: M*v
    Vector3 transform(const float* mat, const Vector3& vec);

    int currentGroup;                           // index of current group
    int currentMaterial;                        // index of current material
    bool currentMaterialAssigned;

    std::vector<ObjGroup> groups;               // obj model can have multiple proups
    std::vector<ObjMaterial> materials;         // obj model can have multiple materials

    std::vector<float> vertices;                // vertex position array for opengl
    std::vector<float> normals;                 // vertex normal array for opengl
    std::vector<float> texCoords;               // tex-ccord array for opengl
    std::vector<unsigned int> indices;          // index array for opengl
    std::vector<Vector3> faceNormals;           // normals per face 
    std::vector<float> interleavedVertices;     // for opengl interleaved vertex

    // split vertex data without sharing vertices for smoothing normals
    std::vector<Vector3> splitVertices;
    std::vector<Vector3> splitNormals;
    std::vector<Vector2> splitTexCoords;
    std::multimap<Vector3, unsigned int> splitVertexMap;
    std::map<unsigned int, unsigned int> sharedVertexLookup;

    BoundingBox bound;

    int stride;                                 // # of bytes to hop to the next vertex

    // temporary lookup buffers
    std::vector<float> vertexLookup;            // for "v" lines
    std::vector<float> normalLookup;            // for "vn" lines
    std::vector<float> texCoordLookup;          // for "vt" lines
    std::map<std::string, unsigned int> faces;  // for "f" lines, map list

    ObjMaterial defaultMaterial;                // dummy material for default

    std::string objDirectory;                   // obj/mtl file location with trailing /
    std::string objFileName;                    // file name without path
    std::string mtlFileName;

    std::string errorMessage;
};

#endif // OBJ_MODEL_H
