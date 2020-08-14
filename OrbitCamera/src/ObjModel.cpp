///////////////////////////////////////////////////////////////////////////////
// ObjModel.cpp
// ============
// Wavefront 3D object (.obj) loader
//
//  AUTHOR: Song Ho Ahn (ssong.ahn@gmail.com)
// CREATED: 2003-05-16
// UPDATED: 2017-12-13
///////////////////////////////////////////////////////////////////////////////

#include "ObjModel.h"
#include "Tokenizer.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <map>
#include <algorithm>
#include <limits>
#include <ctime>


// constants
const char* DEFAULT_GROUP_NAME = "ObjModel_default_group";
const char* DEFAULT_MATERIAL_NAME = "ObjModel_default_material";
const float EPSILON = 0.00001f;



///////////////////////////////////////////////////////////////////////////////
// ctor
///////////////////////////////////////////////////////////////////////////////
ObjModel::ObjModel() : currentGroup(-1), currentMaterial(-1),
                       errorMessage("No Error.")
{
    defaultMaterial.name = DEFAULT_MATERIAL_NAME;
}



///////////////////////////////////////////////////////////////////////////////
// dtor
///////////////////////////////////////////////////////////////////////////////
ObjModel::~ObjModel()
{
}



///////////////////////////////////////////////////////////////////////////////
// print itself
///////////////////////////////////////////////////////////////////////////////
void ObjModel::printSelf() const
{
    std::cout << "===== ObjModel =====\n"
              << "Triangle Count: " << getTriangleCount() << "\n"
              << "   Index Count: " << getIndexCount() << "\n"
              << "  Bounding Box: " << bound.toString() << "\n"
              << "   Group Count: " << getGroupCount() << "\n"
              << "Material Count: " << materials.size() << "\n"
              << "  Vertex Count: " << getVertexCount() << "\n"
              << "  Normal Count: " << getNormalCount() << "\n"
              << "TexCoord Count: " << getTexCoordCount() << std::endl;

    // info about each group
    for(unsigned int i = 0; i < groups.size(); ++i)
    {
        std::cout << "\n[Group " << i << "]\n"
                  << "    Group Name: " << groups[i].name << "\n"
                  << " Material Name: " << groups[i].materialName << "\n"
                  << "   Index Count: " << groups[i].indexCount << "\n"
                  << "  Index Offset: " << groups[i].indexOffset << std::endl;
    }

    // info about each mtl
    for(unsigned int i = 0; i < materials.size(); ++i)
    {
        std::cout << "\n[Material " << i << "]\n"
                  << "     Name: " << materials[i].name << "\n"
                  << "  Texture: " << materials[i].textureName << "\n"
                  << "  Ambient: " << "(" << materials[i].ambient[0] << ", "
                                          << materials[i].ambient[1] << ", "
                                          << materials[i].ambient[2] << ", "
                                          << materials[i].ambient[3] << ")\n"
                  << "  Diffuse: " << "(" << materials[i].diffuse[0] << ", "
                                          << materials[i].diffuse[1] << ", "
                                          << materials[i].diffuse[2] << ", "
                                          << materials[i].diffuse[3] << ")\n"
                  << " Specular: " << "(" << materials[i].specular[0] << ", "
                                          << materials[i].specular[1] << ", "
                                          << materials[i].specular[2] << ", "
                                          << materials[i].specular[3] << ")\n"
                  << "Shininess: " << materials[i].shininess << std::endl;
    }

    std::cout << std::endl;
}



///////////////////////////////////////////////////////////////////////////////
// init containers and vars
///////////////////////////////////////////////////////////////////////////////
void ObjModel::init()
{
    currentGroup = currentMaterial = -1;
    currentMaterialAssigned = false;
    stride = 0;

    // reset bounding box
    bound.set(0,0, 0,0, 0,0);

    // flush(deallocate) the previous memory
    std::vector<ObjGroup>().swap(groups);
    std::vector<ObjMaterial>().swap(materials);

    std::vector<float>().swap(vertices);
    std::vector<float>().swap(normals);
    std::vector<float>().swap(texCoords);

    std::vector<unsigned int>().swap(indices);
    std::vector<Vector3>().swap(faceNormals);
    std::vector<float>().swap(interleavedVertices);

    std::vector<float>().swap(vertexLookup);    // for "v"
    std::vector<float>().swap(normalLookup);    // for "vn"
    std::vector<float>().swap(texCoordLookup);  // for "vt"
    faces.clear();                              // for "f"
}



///////////////////////////////////////////////////////////////////////////////
// load obj file from file
///////////////////////////////////////////////////////////////////////////////
bool ObjModel::read(const char* fileName)
{
    // validate file name
    if(!fileName)
    {
        errorMessage = "File name is not defined.";
        return false;
    }

    // remember path and file name (assume fileName has absolute path)
    std::string path = fileName;
    std::size_t index = path.find_last_of("/\\");
    if(index != std::string::npos)
    {
        objDirectory = path.substr(0, index+1);
        objFileName = path.substr(index+1);
        mtlFileName = "";   // start with blank
    }
    else
    {
        objDirectory = "";
        objFileName = fileName;
        mtlFileName = "";   // start with blank
    }
    path = objDirectory + objFileName;  // full path (dir + file)

    // open an OBJ file
    std::ifstream inFile;
    inFile.open(path.c_str(), std::ios::in);
    if(!inFile.good())
    {
        errorMessage = "Failed to open a OBJ file to read: ";
        errorMessage += path;
        inFile.close();
        return false;
    }

    // get lines of obj file
    std::vector<std::string> vLines;    // "v" lines from obj file
    std::vector<std::string> vnLines;   // "vn" lines from obj file
    std::vector<std::string> vtLines;   // "vt" lines from obj file
    std::vector<std::string> fLines;    // "f" lines from obj file, other lines as well
    for(std::string line; std::getline(inFile, line);)
    {
        if(line.size() < 2) // skip invalid lines (must have 2 chars per line)
            continue;

        if(line[0] == '#') // skip comment lines begin with #
            continue;

        if(line[0] == 'v')
        {
            if(line[1] == 'n')          // vn
                vnLines.push_back(line);
            else if(line[1] == 't')     // vt
                vtLines.push_back(line);
            else if(line[1] == ' ')     // v
                vLines.push_back(line);
        }
        else
        {
            fLines.push_back(line); // store "f", "g", "usemtl", "mtllib"
        }
    }

    // close opened file
    inFile.close();

    // init arrays for opengl drawing
    std::vector<unsigned int>().swap(indices);
    std::vector<Vector3>().swap(faceNormals);
    std::vector<float>().swap(interleavedVertices);
    std::vector<float>().swap(vertices);            // dealloc arrays
    std::vector<float>().swap(normals);
    std::vector<float>().swap(texCoords);
    indices.reserve(fLines.size() * 3);             // assume they are triangles
    faceNormals.reserve(fLines.size());             // normals per face
    vertices.reserve(vLines.size() * 3);
    normals.reserve(vLines.size() * 3);
    if(vtLines.size() > 0)
        texCoords.reserve(vLines.size() * 2);

    // parse "v" lines to vertexLookup
    std::vector<float>().swap(vertexLookup);
    vertexLookup.reserve(vLines.size() * 3);        // x,y,z
    parseVertexLookup(vLines);
    std::vector<std::string>().swap(vLines);        // dealloc memory

    // parse "vn" lines to normalLookup
    std::vector<float>().swap(normalLookup);
    normalLookup.reserve(vnLines.size() * 3);       // nx,ny,nz
    parseNormalLookup(vnLines);
    std::vector<std::string>().swap(vnLines);       // dealloc memory

    // parse "vt" lines to texCoordLookup
    if(vtLines.size() > 0)
    {
        std::vector<float>().swap(texCoordLookup);
        texCoordLookup.reserve(vtLines.size() * 2); // u,v
        parseTexCoordLookup(vtLines);
        std::vector<std::string>().swap(vtLines);   // dealloc memory
    }

    // parse "f" lines with other lines as well: "g", "usemtl", "mtllib"
    parseFaces(fLines);
    std::vector<std::string>().swap(fLines);        // dealloc memory

    // clear lookups
    std::vector<float>().swap(vertexLookup);
    std::vector<float>().swap(normalLookup);
    std::vector<float>().swap(texCoordLookup);
    faces.clear();

    /*@@ old parser ===========================================================
    // get lines of obj file
    unsigned int fCount = 0;            // # of "f" lines
    unsigned int vCount = 0;            // # of "v" lines
    unsigned int vnCount = 0;           // # of "vn" lines
    unsigned int vtCount = 0;           // # of "vt" lines
    std::vector<std::string> objLines;  // lines from obj file
    for(std::string line; std::getline(inFile, line);)
    {
        if(line.size() < 2) // skip invalid lines (must have 2 chars per line)
            continue;

        if(line[0] == '#') // skip comment lines begin with #
            continue;

        objLines.push_back(line);

        if(line[0] == 'v')
        {
            if(line[1] == 'n')          // vn
                ++vnCount;
            else if(line[1] == 't')     // vt
                ++vtCount;
            else if(line[1] == ' ')     // v
                ++vCount;
        }
        else if(line[0] == 'f')
        {
            ++fCount;
        }
    }
    //DEBUG
    //std::cout << "v  count: " << vCount << std::endl;
    //std::cout << "vn count: " << vnCount << std::endl;
    //std::cout << "vt count: " << vtCount << std::endl;

    // close opened file
    inFile.close();

    // flush the prev memory and reset other values
    init();

    // allocate(estimate) initial container size
    vertexLookup.reserve(vCount * 3);    // x,y,z
    normalLookup.reserve(vnCount * 3);   // nx,ny,nz
    texCoordLookup.reserve(vtCount * 2); // s,t
    indices.reserve(fCount * 3);         // indices (assume they are triangles)
    faceNormals.reserve(fCount);         // normals per face
    vertices.reserve(vCount * 3);        // vertex array for opengl
    normals.reserve(vCount * 3);         // normal array for opengl
    if(vtCount > 0)
        texCoords.reserve(vCount * 2);   // tex coord array for opengl (optional)

    // read obj mesh data from the lines
    parseMesh(objLines);

    // clear temp arrays
    std::vector<std::string>().swap(objLines);
    std::vector<float>().swap(vertexLookup);
    std::vector<float>().swap(normalLookup);
    std::vector<float>().swap(texCoordLookup);
    faces.clear();
    =========================================================================*/

    // compute bounding box
    computeBoundingBox();

    return true;
}



///////////////////////////////////////////////////////////////////////////////
// parse only "v" lines from obj file
// The vertex positions will be stored in vertexLookup array.
///////////////////////////////////////////////////////////////////////////////
void ObjModel::parseVertexLookup(const std::vector<std::string>& lines)
{
    // tokenizer for each line
    Tokenizer tokenizer;

    // convert as float then store to vertexLookup
    unsigned int count = (unsigned int)lines.size();
    for(unsigned int i = 0; i < count; ++i)
    {
        tokenizer.set(lines[i], " ");
        tokenizer.next(); // skip first "v" token

        vertexLookup.push_back((float)atof(tokenizer.next().c_str())); // x
        vertexLookup.push_back((float)atof(tokenizer.next().c_str())); // y
        vertexLookup.push_back((float)atof(tokenizer.next().c_str())); // z
    }
}



///////////////////////////////////////////////////////////////////////////////
// parse only "vn" lines from obj file
// The vertex normals will be stored in normalLookup array.
///////////////////////////////////////////////////////////////////////////////
void ObjModel::parseNormalLookup(const std::vector<std::string>& lines)
{
    Tokenizer tokenizer;
    Vector3 vec;

    // convert as float then store to vertexLookup
    unsigned int count = (unsigned int)lines.size();
    for(unsigned int i = 0; i < count; ++i)
    {
        tokenizer.set(lines[i], " ");
        tokenizer.next(); // skip first "vn" token

        vec.x = (float)atof(tokenizer.next().c_str());  // nx
        vec.y = (float)atof(tokenizer.next().c_str());  // ny
        vec.z = (float)atof(tokenizer.next().c_str());  // nz
        vec.normalize();                // make sure normal vector is normalized
        normalLookup.push_back(vec.x);  // nx
        normalLookup.push_back(vec.y);  // ny
        normalLookup.push_back(vec.z);  // nz
        /*
        normalLookup.push_back((float)atof(tokenizer.next().c_str()));  // nx
        normalLookup.push_back((float)atof(tokenizer.next().c_str()));  // ny
        normalLookup.push_back((float)atof(tokenizer.next().c_str()));  // nz
        */
    }
}



///////////////////////////////////////////////////////////////////////////////
// parse only "vt" lines from obj file
// The texture coordinates will be stored in texCoordLookup array.
///////////////////////////////////////////////////////////////////////////////
void ObjModel::parseTexCoordLookup(const std::vector<std::string>& lines)
{
    Tokenizer tokenizer;

    // convert as float then store to vertexLookup
    unsigned int count = (unsigned int)lines.size();
    for(unsigned int i = 0; i < count; ++i)
    {
        tokenizer.set(lines[i], " ");
        tokenizer.next(); // skip first "vt" token

        // OpenGL uses bottom-left origin, and OBJ is top-left origin
        // invert v coord to OpenGL orientation
        texCoordLookup.push_back((float)atof(tokenizer.next().c_str()));        // u
        texCoordLookup.push_back(1.0f - (float)atof(tokenizer.next().c_str())); // v
    }
}



///////////////////////////////////////////////////////////////////////////////
// parse faces from obj file, also read "g", "usemtl", "mtllib" lines as well
// NOTE: "f" elements should be listed inside a group(g). If face elements come
// before a "g" tag, create a default group and assign the faces into the
// default group.
///////////////////////////////////////////////////////////////////////////////
void ObjModel::parseFaces(const std::vector<std::string>& lines)
{
    // reset the previous values
    currentGroup = currentMaterial = -1;
    currentMaterialAssigned = false;
    stride = 0;
    groups.clear();
    materials.clear();
    faces.clear();

    // tokenizer for each line
    Tokenizer tokenizer;
    std::string token;

    // parse each line
    unsigned int count = (unsigned int)lines.size();
    for(unsigned int i = 0; i < count; ++i)
    {
        // start tokenizing
        tokenizer.set(lines[i], " ");
        token = tokenizer.next();

        // parse face (triangle)
        if(token == "f")
        {
            // if not shown "g" before "f" yet, then create a default group
            if(currentGroup == -1)
            {
                createGroup(DEFAULT_GROUP_NAME);
                // if "usemtl" shown before f, then use this mtl for this group
                if(currentMaterial >= 0)
                {
                    groups[currentGroup].materialName = materials[currentMaterial].name;
                    currentMaterialAssigned = true;
                }
            }

            // get all face index list in a line
            std::vector<std::string> faceIndices = tokenizer.split();

            // convert to triangles if the face has more than 3 indices
            if(faceIndices.size() > 3)
                convertToTriangles(faceIndices);

            // add the triangles to index list
            addFace(faceIndices);
        }

        // parse group
        else if(token == "g")
        {
            std::string groupName = tokenizer.next();
            createGroup(groupName); // create new group, mtl name will be set when "usemtl" called

            // if "usemtl"->"g" (if a material is not assigned to a group yet),
            // then assign the current material to this group
            if(currentMaterial >= 0 && !currentMaterialAssigned)
            {
                groups[currentGroup].materialName = materials[currentMaterial].name;
                currentMaterialAssigned = true;
            }
        }

        // parse material file
        else if(token == "mtllib")
        {
            parseMaterial(tokenizer.rest()); // pass the rest of tokens
            currentMaterial = -1; // reset current ID after read mtl file
        }

        // parse material name(ID)
        else if(token == "usemtl")
        {
            std::string materialName = tokenizer.next();
            currentMaterial = findMaterial(materialName); // remember in case "usemtl" comes before "g"
            currentMaterialAssigned = false;

            // if "g"->"usemtl ("g" comes before "usemtl"), assign the material name to the group
            if(currentMaterial >= 0 && currentGroup >= 0)
            {
                // if material name is not set on the current group, assign it
                if(groups[currentGroup].materialName == "")
                {
                    groups[currentGroup].materialName = materialName;
                    currentMaterialAssigned = true;
                }
                // if material name is different, then create new group with mtl name
                else if(groups[currentGroup].materialName != materialName)
                {
                    // create a temp group here
                    // if "g" will appear next line, use that group,
                    // but no "g" follows, use this temp group
                    createGroup(materialName);
                    groups[currentGroup].materialName = materialName;
                }
            }
        }
    }

    // compute index count of the last group, before return
    if(currentGroup >= 0)
        groups[currentGroup].indexCount = (unsigned int)indices.size() - groups[currentGroup].indexOffset;

    // delete empty groups which have no indices assigned
    std::vector<ObjGroup>::iterator iter = groups.begin();
    while(iter != groups.end())
    {
        if(iter->indexCount == 0)
            iter = groups.erase(iter);  // erase() returns the next iterator after delete the current
        else
            ++iter;
    }

    // clear temp map
    faces.clear();
}



///////////////////////////////////////////////////////////////////////////////
// parse OBJ string
// Any OBJ elements (v, vt, vn, f) should be listed inside a group(g).
// If elements come before "g" tag, create a default group.
///////////////////////////////////////////////////////////////////////////////
void ObjModel::parseMesh(const std::vector<std::string>& lines)
{
    // reset the previous values
    currentGroup = currentMaterial = -1;
    currentMaterialAssigned = false;
    stride = 0;

    // tokenizer for each line
    Tokenizer tokenizer;
    std::string token;

    // parse each line
    unsigned int count = (unsigned int)lines.size();
    for(unsigned int i = 0; i < count; ++i)
    {
        // start tokenizing
        tokenizer.set(lines[i], " ");
        token = tokenizer.next();

        // parse vertex
        if(token == "v")
        {
            vertexLookup.push_back((float)atof(tokenizer.next().c_str())); // x
            vertexLookup.push_back((float)atof(tokenizer.next().c_str())); // y
            vertexLookup.push_back((float)atof(tokenizer.next().c_str())); // z
        }

        // parse normal
        else if(token == "vn")
        {
            Vector3 vec;
            vec.x = (float)atof(tokenizer.next().c_str());  // nx
            vec.y = (float)atof(tokenizer.next().c_str());  // ny
            vec.z = (float)atof(tokenizer.next().c_str());  // nz
            vec.normalize();                // make sure normal vector is normalized
            normalLookup.push_back(vec.x);  // nx
            normalLookup.push_back(vec.y);  // ny
            normalLookup.push_back(vec.z);  // nz
        }

        // parse texture coords
        else if(token == "vt")
        {
            // OpenGL uses bottom-left origin, and OBJ is top-left origin
            // invert v coord to OpenGL orientation
            texCoordLookup.push_back((float)atof(tokenizer.next().c_str()));        // u
            texCoordLookup.push_back(1.0f - (float)atof(tokenizer.next().c_str())); // v
        }

        // parse face (triangle)
        else if(token == "f")
        {
            // if not shown "g" before "f" yet, then create a default group
            if(currentGroup == -1)
            {
                createGroup(DEFAULT_GROUP_NAME);
                // if "usemtl" shown before f, then use this mtl for this group
                if(currentMaterial >= 0)
                {
                    groups[currentGroup].materialName = materials[currentMaterial].name;
                    currentMaterialAssigned = true;
                }
            }

            // get all face index list in a line
            std::vector<std::string> faceIndices = tokenizer.split();

            // convert to triangles if the face has more than 3 indices
            if(faceIndices.size() > 3)
                convertToTriangles(faceIndices);

            // add the triangles to index list
            addFace(faceIndices);
        }

        // parse group
        else if(token == "g")
        {
            std::string groupName = tokenizer.next();
            createGroup(groupName); // create new group, mtl name will be set when "usemtl" called

            // if "usemtl"->"g" (if a material is not assigned to a group yet),
            // then assign the current material to this group
            if(currentMaterial >= 0 && !currentMaterialAssigned)
            {
                groups[currentGroup].materialName = materials[currentMaterial].name;
                currentMaterialAssigned = true;
            }
        }

        // parse material file
        else if(token == "mtllib")
        {
            parseMaterial(tokenizer.rest()); // pass the rest of tokens
            currentMaterial = -1; // reset current ID after read mtl file
        }

        // parse material name(ID)
        else if(token == "usemtl")
        {
            std::string materialName = tokenizer.next();
            currentMaterial = findMaterial(materialName); // remember in case "usemtl" comes before "g"
            currentMaterialAssigned = false;

            // if "g"->"usemtl ("g" comes before "usemtl"), assign the material name to the group
            if(currentMaterial >= 0 && currentGroup >= 0)
            {
                // if material name is not set on the current group, assign it
                if(groups[currentGroup].materialName == "")
                {
                    groups[currentGroup].materialName = materialName;
                    currentMaterialAssigned = true;
                }
                // if material name is different, then create new group with mtl name
                else if(groups[currentGroup].materialName != materialName)
                {
                    // create a temp group here
                    // if "g" will appear next line, use that group,
                    // but no "g" follows, use this temp group
                    createGroup(materialName);
                    groups[currentGroup].materialName = materialName;
                }
            }
        }
    }

    // compute index count of the last group, before return
    if(currentGroup >= 0)
        groups[currentGroup].indexCount = (unsigned int)indices.size() - groups[currentGroup].indexOffset;

    // delete empty groups which have no indices assigned
    std::vector<ObjGroup>::iterator iter = groups.begin();
    while(iter != groups.end())
    {
        if(iter->indexCount == 0)
            iter = groups.erase(iter);  // erase() returns the next iterator after delete the current
        else
            ++iter;
    }
}



///////////////////////////////////////////////////////////////////////////////
// parse mtl file
// input param contains only mtl file name, ignoring dir
// (obj and mtl files should be located in the same directory)
///////////////////////////////////////////////////////////////////////////////
bool ObjModel::parseMaterial(const std::string& mtlName)
{
    // remove directory from the input string if it has
    std::size_t index = mtlName.find_last_of("/\\");
    if(index != std::string::npos)
        mtlFileName = mtlName.substr(index+1);
    else
        mtlFileName = mtlName;

    std::string path = objDirectory + mtlFileName; // full path (dir + file)

    // open an MTL file
    std::ifstream inFile;
    inFile.open(path.c_str());
    if(!inFile.good())
    {
        errorMessage = "Failed to open a MTL file to read: "  + path;
        inFile.close();
        return false;
    }

    // copy all data from file to stringstream
    std::stringstream ss;
    ss << inFile.rdbuf();

    // close opened file
    inFile.close();

    Tokenizer lines = Tokenizer(ss.str(), "\n\r\f");
    std::string line;
    Tokenizer tokenizer;                    // tokenizer for each line
    std::string token;

    // parse each line
    while((line = lines.next()) != "")
    {
        // skip comment line
        if(line.at(0) == '#')
            continue;

        // start tokenizing
        tokenizer.set(line);
        token = tokenizer.next();

        // parse material name
        if(token == "newmtl")
        {
            ObjMaterial material;
            material.name = tokenizer.next();

            // add to material list
            materials.push_back(material);
            currentMaterial = (int)materials.size() - 1;
        }

        // parse ambient
        else if(token == "Ka")
        {
            materials[currentMaterial].ambient[0] = (float)atof(tokenizer.next().c_str());
            materials[currentMaterial].ambient[1] = (float)atof(tokenizer.next().c_str());
            materials[currentMaterial].ambient[2] = (float)atof(tokenizer.next().c_str());
        }

        // parse diffuse
        else if(token == "Kd")
        {
            materials[currentMaterial].diffuse[0] = (float)atof(tokenizer.next().c_str());
            materials[currentMaterial].diffuse[1] = (float)atof(tokenizer.next().c_str());
            materials[currentMaterial].diffuse[2] = (float)atof(tokenizer.next().c_str());
        }

        // parse specular
        else if(token == "Ks")
        {
            materials[currentMaterial].specular[0] = (float)atof(tokenizer.next().c_str());
            materials[currentMaterial].specular[1] = (float)atof(tokenizer.next().c_str());
            materials[currentMaterial].specular[2] = (float)atof(tokenizer.next().c_str());
        }

        // parse specular exponent
        else if(token == "Ns")
        {
            materials[currentMaterial].shininess = (float)atof(tokenizer.next().c_str());
        }

        // parse transparency
        else if(token == "d")
        {
            // override alpha value
            float alpha = (float)atof(tokenizer.next().c_str());
            materials[currentMaterial].ambient[3] = alpha;
            materials[currentMaterial].diffuse[3] = alpha;
            materials[currentMaterial].specular[3] = alpha;
        }

        // parse texture map name
        else if(token == "map_Kd")
        {
            materials[currentMaterial].textureName = tokenizer.next();
        }
    }

    return true;
}



///////////////////////////////////////////////////////////////////////////////
// convert a polygon to multiple triangles index list
///////////////////////////////////////////////////////////////////////////////
void ObjModel::convertToTriangles(std::vector<std::string>& faceIndices)
{
    // make a copy
    std::vector<std::string> copyIndices = faceIndices;
    unsigned int count = (unsigned int)faceIndices.size();

    // rebuild face index list with multiple triangles
    faceIndices.clear();                    // clean up first
    faceIndices.push_back(copyIndices[0]);  // first 3 indices are not changed
    faceIndices.push_back(copyIndices[1]);
    faceIndices.push_back(copyIndices[2]);

    // start from 4th index, insert 2 more between the target element
    for(unsigned int i = 3; i < count; ++i)
    {
        faceIndices.push_back(copyIndices[i-1]); // insert the previous
        faceIndices.push_back(copyIndices[i]);   // insert the target
        faceIndices.push_back(copyIndices[0]);   // insert the first
    }
}



///////////////////////////////////////////////////////////////////////////////
// add faces to the face list
// The input string is index list of vertex attributes separated by "/".
// OBJ file has 4 different sets of vertex attributes:
// 1. v    : position only
// 2. v/t  : position and texCoord
// 3. v//n : position and normal
// 4. v/t/n: position, texCoord and normal
//
// If OBJ file does not provide normals, then generate a face normal per
// a triangle, and assign it to the vertices of the triangle.
///////////////////////////////////////////////////////////////////////////////
void ObjModel::addFace(const std::vector<std::string>& faceIndices)
{
    // tmp array to store 3 vertex positions to compute face normal when normal
    // is not provided
    Vector3 positions[3];

    float vx, vy, vz;
    volatile bool normalNeeded = false;
    int newVertexCount = 0;

    int lookupIndex;                   // index of lookup arrays (can be negative)
    unsigned int count = (unsigned int)faceIndices.size();
    for(unsigned int i = 0; i < count; ++i)
    {
        // get index from face list
        std::map<std::string, unsigned int>::iterator iter = faces.find(faceIndices[i]);
        if(iter == faces.end())     // not in the "f" list
        {
            // vertex attributes are separated by "/": vertex/texCoords/normal
            Tokenizer indexString = Tokenizer(faceIndices[i], "/");
            std::vector<std::string> indexStrings = indexString.split();

            // indexString can be negative, if so, make it positive
            lookupIndex = stoi(indexStrings[0]);
            if(lookupIndex >= 0)
                lookupIndex = (lookupIndex - 1) * 3;    // obj uses 1-based index
            else
                lookupIndex = (int)vertexLookup.size() + lookupIndex * 3;

            // vertex position is common for all cases, so, add it here
            vx = vertexLookup[lookupIndex];
            vy = vertexLookup[lookupIndex + 1];
            vz = vertexLookup[lookupIndex + 2];
            vertices.push_back(vx);
            vertices.push_back(vy);
            vertices.push_back(vz);
            ++newVertexCount;   // remember how many vertices are added per face

            unsigned int stringCount = (unsigned int)indexStrings.size();
            // 1 token means it is (vertex) only
            if(stringCount == 1)
            {
                // normal should be computed for this case,
                // so, remember the vertex position to compute face normal
                normalNeeded = true;                // mark we need add normal computation later
                positions[i % 3].set(vx, vy, vz);
            }
            // 2 tokens means it is (vertex, texCoord), OR (vertex, normal)
            else if(stringCount == 2)
            {
                // (vertex, texCoord)
                if(faceIndices[i].find("//") == std::string::npos)
                {
                    normalNeeded = true;                // mark we need add normal later
                    positions[i % 3].set(vx, vy, vz);   // also, remember vertex to compute face normal

                    lookupIndex = stoi(indexStrings[1]);
                    if(lookupIndex >= 0)
                        lookupIndex = (lookupIndex - 1) * 2;
                    else
                        lookupIndex = (int)texCoordLookup.size() + lookupIndex * 2;

                    texCoords.push_back(texCoordLookup[lookupIndex]);
                    texCoords.push_back(texCoordLookup[lookupIndex + 1]);
                }
                // (vertex, normal)
                else
                {
                    positions[i % 3].set(vx, vy, vz);   // remember vertex to compute face normal

                    lookupIndex = stoi(indexStrings[1]);
                    if(lookupIndex >= 0)
                        lookupIndex = (lookupIndex - 1) * 3;
                    else
                        lookupIndex = (int)normalLookup.size() + lookupIndex * 3;

                    normals.push_back(normalLookup[lookupIndex]);
                    normals.push_back(normalLookup[lookupIndex + 1]);
                    normals.push_back(normalLookup[lookupIndex + 2]);
                }
            }
            // 3 tokens means it is (vertex, texCoord, normal)
            else if(stringCount == 3)
            {
                positions[i % 3].set(vx, vy, vz);   // remember vertex to compute face normal

                lookupIndex = stoi(indexStrings[1]);
                if(lookupIndex >= 0)
                    lookupIndex = (lookupIndex - 1) * 2;
                else
                        lookupIndex = (int)texCoordLookup.size() + lookupIndex * 2;

                texCoords.push_back(texCoordLookup[lookupIndex]);
                texCoords.push_back(texCoordLookup[lookupIndex + 1]);

                lookupIndex = stoi(indexStrings[2]);
                if(lookupIndex >= 0)
                    lookupIndex = (lookupIndex - 1) * 3;
                else
                    lookupIndex = (int)normalLookup.size() + lookupIndex * 3;

                normals.push_back(normalLookup[lookupIndex]);
                normals.push_back(normalLookup[lookupIndex + 1]);
                normals.push_back(normalLookup[lookupIndex + 2]);
            }

            // add new index to the list
            unsigned int vertexIndex = (unsigned int)vertices.size() / 3 - 1;
            faces[faceIndices[i]] = vertexIndex;
            indices.push_back(vertexIndex);
        }
        // it is already in list, get the index from the list
        else
        {
            // add it to only the index list
            indices.push_back(iter->second);

            // for face normal generation
            lookupIndex = iter->second * 3;
            positions[i % 3].set(vertices[lookupIndex],
                                 vertices[lookupIndex + 1],
                                 vertices[lookupIndex + 2]);
        }

        // finally, compute face normal per triangle
        if(i % 3 == 2)
        {
            Vector3 normal = computeFaceNormal(positions[0], positions[1], positions[2]);
            faceNormals.push_back(normal);  // store face normal per face

            // assign face normal as vertex normal to new vertices only
            if(normalNeeded)
            {
                for(int j = 0; j < newVertexCount; ++j)
                {
                    normals.push_back(normal.x);
                    normals.push_back(normal.y);
                    normals.push_back(normal.z);
                }
            }

            newVertexCount = 0; // reset
        }
    }
}



///////////////////////////////////////////////////////////////////////////////
// start new group and assign currentGroup index to it
///////////////////////////////////////////////////////////////////////////////
void ObjModel::createGroup(const std::string& groupName)
{
    // add new group to container
    ObjGroup group;
    group.name = groupName;
    group.materialName = "";
    group.indexOffset = (unsigned int)indices.size(); // starting point
    groups.push_back(group);

    // remember the current group index
    int groupCount = (int)groups.size();
    currentGroup = groupCount - 1;

    // index count for previous group
    int prevGroup = groupCount - 2;
    if(prevGroup >= 0)
        groups[prevGroup].indexCount = (unsigned int)indices.size() - groups[prevGroup].indexOffset;
}



///////////////////////////////////////////////////////////////////////////////
// return the name of group at given group
///////////////////////////////////////////////////////////////////////////////
std::string ObjModel::getGroupName(int index) const
{
    if(index >=0 && index < (int)groups.size())
        return groups[index].name;
    else
        return "";
}



///////////////////////////////////////////////////////////////////////////////
// return the name of material at given group
///////////////////////////////////////////////////////////////////////////////
std::string ObjModel::getMaterialName(int index) const
{
    if(index >=0 && index < (int)groups.size())
        return groups[index].materialName;
    else
        return "";
}



///////////////////////////////////////////////////////////////////////////////
// return material object for given group index
///////////////////////////////////////////////////////////////////////////////
const ObjMaterial& ObjModel::getMaterial(int index)
{
    std::string materialName = getMaterialName(index);
    if(materialName == "")
        return defaultMaterial;

    int mtlId = findMaterial(materialName);
    if(mtlId >= 0)
        return materials[mtlId];
    else
        return defaultMaterial;
}



///////////////////////////////////////////////////////////////////////////////
// set material with given group index
///////////////////////////////////////////////////////////////////////////////
void ObjModel::setMaterial(int index, const ObjMaterial& material)
{
    std::string materialName = getMaterialName(index);
    int mtlId = findMaterial(materialName);
    if(mtlId >= 0)
    {
        materials[mtlId] = material;
    }
}



///////////////////////////////////////////////////////////////////////////////
// return the index buffer offset for given group
///////////////////////////////////////////////////////////////////////////////
unsigned int ObjModel::getIndexOffset(int index) const
{
    if(index >=0 && index < (int)groups.size())
        return groups[index].indexOffset;
    else
        return 0;
}



///////////////////////////////////////////////////////////////////////////////
// return the number of indices at given group
///////////////////////////////////////////////////////////////////////////////
unsigned int ObjModel::getIndexCount(int index) const
{
    if(index >=0 && index < (int)groups.size())
        return groups[index].indexCount;
    else
        return 0;
}



///////////////////////////////////////////////////////////////////////////////
// return the pointer to the index buffer at given group
///////////////////////////////////////////////////////////////////////////////
const unsigned int* ObjModel::getIndices(int index) const
{
    if(index >=0 && index < (int)groups.size())
        return &indices[groups[index].indexOffset];
    else
        return 0;
}



///////////////////////////////////////////////////////////////////////////////
// compute bounding box of the object
///////////////////////////////////////////////////////////////////////////////
void ObjModel::computeBoundingBox()
{
    // prepare default bound with opposite values
    bound.minX = bound.minY = bound.minZ = std::numeric_limits<float>::max();
    bound.maxX = bound.maxY = bound.maxZ = -std::numeric_limits<float>::max();

    float x, y, z;
    unsigned int count = (unsigned int)vertices.size();
    for(unsigned int i = 0; i < count; i += 3)
    {
        x = vertices[i];
        y = vertices[i+1];
        z = vertices[i+2];

        if(x < bound.minX) bound.minX = x;
        if(x > bound.maxX) bound.maxX = x;

        if(y < bound.minY) bound.minY = y;
        if(y > bound.maxY) bound.maxY = y;

        if(z < bound.minZ) bound.minZ = z;
        if(z > bound.maxZ) bound.maxZ = z;
    }
}



///////////////////////////////////////////////////////////////////////////////
// First, make all faces are split (not shared) and assign face normal to
// vertex. Next find shared faces and average the face normals to soften/harden.
// Finally, join (weld) shared vertices into one.
// Note that this process increase the number of vertices because it
// will convert shared vertices to independent.
///////////////////////////////////////////////////////////////////////////////
void ObjModel::smoothNormals(float angle)
{
    // clean up the previous
    std::vector<Vector3>().swap(splitVertices);
    std::vector<Vector3>().swap(splitNormals);
    std::vector<Vector2>().swap(splitTexCoords);
    splitVertexMap.clear();
    sharedVertexLookup.clear();

    // a vertex on the hard edge should have separate vertex attributes at the
    // shared vertex in OpenGL because position and normal must be paired in
    // order to be drawn, so, we force to split faces for the hard edge faces.
    splitFaces();

    // copy face normal to the vertex normals, flatten(harden) the surface normals
    std::size_t indexCount = indices.size();
    for(std::size_t i = 0; i < indexCount; i += 3)
    {
        Vector3 faceNormal;
        faceNormal = faceNormals[i / 3];
        for(int j = 0; j < 3; ++j)
            splitNormals[i+j] = faceNormal;
    }

    // soften vertex normals at shared vertices
    averageNormals(angle);

    // join shared vertices only if all vertex attributes are same far each
    // vertex. It will reduce the number of vertex attributes.
    joinFaces();

    // return memory to system after smoothing
    std::vector<Vector3>().swap(splitVertices);
    std::vector<Vector3>().swap(splitNormals);
    std::vector<Vector2>().swap(splitTexCoords);
    splitVertexMap.clear();
    sharedVertexLookup.clear();
}



///////////////////////////////////////////////////////////////////////////////
// remove the duplicated vertices
///////////////////////////////////////////////////////////////////////////////
void ObjModel::removeDuplicates()
{
    // clean up the previous
    std::vector<Vector3>().swap(splitVertices);
    std::vector<Vector3>().swap(splitNormals);
    std::vector<Vector2>().swap(splitTexCoords);
    splitVertexMap.clear();
    sharedVertexLookup.clear();

    // make each face independent (not shared)
    splitFaces();

    // find same vertex data then store the shared vertices to sharedVertexLookup
    findDuplicates();

    // weld same vertex together
    joinFaces();

    // return memory to system after merging
    std::vector<Vector3>().swap(splitVertices);
    std::vector<Vector3>().swap(splitNormals);
    std::vector<Vector2>().swap(splitTexCoords);
    splitVertexMap.clear();
    sharedVertexLookup.clear();
}



///////////////////////////////////////////////////////////////////////////////
// split faces before regenerating normals
///////////////////////////////////////////////////////////////////////////////
void ObjModel::splitFaces()
{
    // check if texCoords need to be processed
    bool hasTexCoords = false;
    if(texCoords.size() > 0)
        hasTexCoords = true;

    Vector3 vertex;
    Vector3 normal;
    Vector2 texCoord;
    unsigned int vertexIndex;

    // reserve container size
    unsigned int indexCount = (unsigned int)indices.size();
    splitVertices.reserve(indexCount);
    splitNormals.reserve(indexCount);
    if(hasTexCoords)
        splitTexCoords.reserve(indexCount);

    // copy vertex attributes, but split them as not shared
    for(unsigned int i = 0; i < indexCount; i += 3)
    {
        // get 3 vertices and normals of a face
        vertexIndex = indices[i] * 3;
        vertex.set(vertices[vertexIndex], vertices[vertexIndex+1], vertices[vertexIndex+2]);
        splitVertices.push_back(vertex);
        splitVertexMap.insert(std::pair<Vector3, unsigned int>(vertex, i));

        normal.set(normals[vertexIndex], normals[vertexIndex+1], normals[vertexIndex+2]);
        splitNormals.push_back(normal);

        vertexIndex = indices[i+1] * 3;
        vertex.set(vertices[vertexIndex], vertices[vertexIndex+1], vertices[vertexIndex+2]);
        splitVertices.push_back(vertex);
        splitVertexMap.insert(std::pair<Vector3, unsigned int>(vertex, i+1));

        normal.set(normals[vertexIndex], normals[vertexIndex+1], normals[vertexIndex+2]);
        splitNormals.push_back(normal);

        vertexIndex = indices[i+2] * 3;
        vertex.set(vertices[vertexIndex], vertices[vertexIndex+1], vertices[vertexIndex+2]);
        splitVertices.push_back(vertex);
        splitVertexMap.insert(std::pair<Vector3, unsigned int>(vertex, i+2));

        normal.set(normals[vertexIndex], normals[vertexIndex+1], normals[vertexIndex+2]);
        splitNormals.push_back(normal);

        // copy tex coords only if the original has it
        if(hasTexCoords)
        {
            vertexIndex = indices[i] * 2;
            texCoord.set(texCoords[vertexIndex], texCoords[vertexIndex+1]);
            splitTexCoords.push_back(texCoord);

            vertexIndex = indices[i+1] * 2;
            texCoord.set(texCoords[vertexIndex], texCoords[vertexIndex+1]);
            splitTexCoords.push_back(texCoord);

            vertexIndex = indices[i+2] * 2;
            texCoord.set(texCoords[vertexIndex], texCoords[vertexIndex+1]);
            splitTexCoords.push_back(texCoord);
        }
    }
}



///////////////////////////////////////////////////////////////////////////////
// find shared vertices that their normals are less than smooth angle, then
// soften the vertex normals by averaging their face normals
// This routine uses the multimap prepared in splitFaces() in order to speed up
// the averaging algorithm.
///////////////////////////////////////////////////////////////////////////////
void ObjModel::averageNormals(float angle)
{
    typedef std::multimap<Vector3, unsigned int>::iterator MapIter;

    const float DEG2RAD = 3.141592f / 180.0f;
    float cosAngle = cosf(DEG2RAD * angle);

    // first, find unique vertex keys
    std::vector<Vector3> vertexKeys;
    MapIter mapIter = splitVertexMap.begin();

    // add the first one into vertex key list
    Vector3 vertexKey = mapIter->first;
    vertexKeys.push_back(vertexKey);
    ++mapIter;

    // find different keys from the map
    while(mapIter != splitVertexMap.end())
    {
        // add new key to the list if found different key
        if(vertexKey != mapIter->first)
        {
            vertexKey = mapIter->first;     // remember new key for next search
            vertexKeys.push_back(vertexKey);
        }
        ++mapIter;
    }

    // init shared vertex index map with itself
    unsigned int vertexCount = (unsigned int)splitVertices.size();
    sharedVertexLookup.clear();
    for(unsigned int i = 0; i < vertexCount; ++i)
        sharedVertexLookup[i] = i;

    // do average
    Vector3 sharedNormal, normal1, normal2;
    unsigned int faceIndex1, faceIndex2;
    unsigned int vertexIndex1, vertexIndex2;

    // loop through each vertex
    std::vector<Vector3>::iterator iter;
    for(iter = vertexKeys.begin(); iter != vertexKeys.end(); ++iter)
    {
        // get all indices with same vertex position
        std::pair<MapIter, MapIter> iterPair = splitVertexMap.equal_range(*iter);

        //@@ FIXME: It only checks the first normal. Must compare all normals
        // in the range

        // get normal of the first element
        mapIter = iterPair.first;
        vertexIndex1 = mapIter->second;
        faceIndex1 = vertexIndex1 / 3;
        sharedNormal = normal1 = faceNormals[faceIndex1];

        // compare with other vertex normals
        ++mapIter;
        while(mapIter != iterPair.second)
        {
            vertexIndex2 = mapIter->second;
            faceIndex2 = vertexIndex2 / 3;
            normal2 = faceNormals[faceIndex2];

            // check if 2 normals are less than smooth angle
            if(normal1.dot(normal2) > cosAngle)
            {
                // remember the shared vertex index, so we can join the vertex together
                sharedVertexLookup[vertexIndex2] = vertexIndex1;

                sharedNormal += normal2;    // sum normal
            }
            ++mapIter;
        }
        sharedNormal.normalize();   // make it unit length

        splitNormals[vertexIndex1] = sharedNormal;   // update only the first one
    }
}



///////////////////////////////////////////////////////////////////////////////
// find same vertices where position, normal and texCoord are same
///////////////////////////////////////////////////////////////////////////////
void ObjModel::findDuplicates()
{
    typedef std::multimap<Vector3, unsigned int>::iterator MapIter;

    // first, find unique vertex keys
    MapIter mapIter = splitVertexMap.begin();

    // add the first one into vertex key list
    std::vector<Vector3> vertexKeys;
    Vector3 vertexKey = mapIter->first;
    vertexKeys.push_back(vertexKey);
    ++mapIter;

    // find all different keys from the map
    while(mapIter != splitVertexMap.end())
    {
        // add new key to the list if the key is different
        if(vertexKey != mapIter->first)
        {
            vertexKey = mapIter->first;     // remember new key for next search
            vertexKeys.push_back(vertexKey);
        }
        ++mapIter;
    }

    // init shared vertex index map with itself
    sharedVertexLookup.clear();
    unsigned int vertexCount = (unsigned int)splitVertices.size();
    for(unsigned int i = 0; i < vertexCount; ++i)
        sharedVertexLookup[i] = i;

    // determine there is texCoords
    bool texCoordNeeded = false;
    if(splitVertices.size() == splitTexCoords.size())
        texCoordNeeded = true;

    Vector3 normal1, normal2;
    Vector2 texCoord1, texCoord2;
    unsigned int index1, index2;

    // loop through each vertex to compare other vertices
    std::vector<Vector3>::iterator iter;
    for(iter = vertexKeys.begin(); iter != vertexKeys.end(); ++iter)
    {
        // get all vertex indices with same vertex position
        std::pair<MapIter, MapIter> iterRange = splitVertexMap.equal_range(*iter);

        // remember index range that has same vertex position
        std::vector<unsigned int> indexRange;
        for(mapIter = iterRange.first; mapIter != iterRange.second; ++mapIter)
            indexRange.push_back(mapIter->second);

        unsigned int rangeCount = (unsigned int)indexRange.size();
        for(unsigned int i = 0; i < rangeCount; ++i)
        {
            // get the normal and texcoord of the first element
            index1 = indexRange[i];
            normal1 = splitNormals[index1];
            if(texCoordNeeded)
                texCoord1 = splitTexCoords[index1];

            // compare with other vertex normals and texCoords
            for(unsigned int j = i+1; j < rangeCount; ++j)
            {
                index2 = indexRange[j];
                normal2 = splitNormals[index2];
                if(texCoordNeeded)
                    texCoord2 = splitTexCoords[index2];

                // check if 2 normals are same and 2 texCoords are same
                if(normal1 == normal2 && texCoord1 == texCoord2)
                {
                    // remember this shared vertex index, so we can join the vertex together
                    // If the index and value of lookup are different, then it is
                    // already set as duplicate. So, we update only where index and
                    // value are same.
                    if(sharedVertexLookup[index2] == index2)
                        sharedVertexLookup[index2] = index1;
                }
            }
        }
    }
}



///////////////////////////////////////////////////////////////////////////////
// weld shared vertices together
///////////////////////////////////////////////////////////////////////////////
void ObjModel::joinFaces()
{
    Vector3 vec3;
    Vector2 vec2;
    unsigned int index;
    unsigned int vertexCount = (unsigned int)splitVertices.size();
    bool texCoordNeeded = false;
    if(splitVertices.size() == splitTexCoords.size())
        texCoordNeeded = true;

    std::map<unsigned int, unsigned int> newIndexLookup;

    // clear previous lists
    vertices.clear();
    normals.clear();
    texCoords.clear();
    indices.clear();

    // loop through all vertices
    for(unsigned int i = 0; i < vertexCount; ++i)
    {
        // if index is same, then not shared, add as new vertex
        if(sharedVertexLookup[i] == i)
        {
            vec3 = splitVertices[i];            // add position
            vertices.push_back(vec3.x);
            vertices.push_back(vec3.y);
            vertices.push_back(vec3.z);

            vec3 = splitNormals[i];             // add normal
            normals.push_back(vec3.x);
            normals.push_back(vec3.y);
            normals.push_back(vec3.z);

            index = (unsigned int)vertices.size() / 3 - 1; // add index
            indices.push_back(index);

            newIndexLookup[i] = index;          // remember new index for other shared vertex

            if(texCoordNeeded)                  // add tex coord
            {
                vec2 = splitTexCoords[i];
                texCoords.push_back(vec2.x);
                texCoords.push_back(vec2.y);
            }
        }
        // shared vertex, add only index to the index list
        else
        {
            indices.push_back(newIndexLookup[sharedVertexLookup[i]]);
        }
    }
}



///////////////////////////////////////////////////////////////////////////////
// compute normal with 3 face vertices
///////////////////////////////////////////////////////////////////////////////
Vector3 ObjModel::computeFaceNormal(const Vector3& v1,
                                    const Vector3& v2,
                                    const Vector3& v3)
{
    Vector3 v12 = v2 - v1;
    Vector3 v13 = v3 - v1;
    Vector3 normal = v12.cross(v13);
    normal.normalize();

    return normal;
}



///////////////////////////////////////////////////////////////////////////////
// return the material index associated with material name
// return -1 if not found
///////////////////////////////////////////////////////////////////////////////
int ObjModel::findMaterial(const std::string& name)
{
    for(int i = 0; i < (int)materials.size(); ++i)
    {
        if(materials[i].name == name)
            return i;
    }
    return -1;  // not found
}



///////////////////////////////////////////////////////////////////////////////
// return the group index associated with group name
// return -1 if not found
///////////////////////////////////////////////////////////////////////////////
int ObjModel::findGroup(const std::string& name)
{
    for(int i = 0; i < (int)groups.size(); ++i)
    {
        if(groups[i].name == name)
            return i;
    }
    return -1;  // not found
}



///////////////////////////////////////////////////////////////////////////////
// return interleaved data
///////////////////////////////////////////////////////////////////////////////
const float* ObjModel::getInterleavedVertices()
{
    // create one if not built yet
    if(interleavedVertices.size() <= 0)
        buildInterleavedVertices();

    return &interleavedVertices[0];
}



///////////////////////////////////////////////////////////////////////////////
// create interleaved vertices: v-n, or v-n-t
// It assume the number of vertices is same as normals and tex coords.
///////////////////////////////////////////////////////////////////////////////
int ObjModel::buildInterleavedVertices()
{
    // compute stride
    stride = 0;
    if(getNormalCount() == getVertexCount())
    {
        stride = 24;
        if(getTexCoordCount() == getVertexCount())
            stride += 8;
    }

    //std::vector<float>().swap(interleavedVertices); // flush the previous
    unsigned int count = (3 * getVertexCount()) + (3 * getNormalCount()) + (2 * getTexCoordCount());
    interleavedVertices.reserve(count);
    switch(stride)
    {
    case 24:
        buildInterleavedVerticesVN();   // vn,vn,vn...
        break;
    case 32:
        buildInterleavedVerticesVNT();  // vnt,vnt,vnt...
        break;
    default:
        interleavedVertices = vertices; // copy only vertices
    }

    return stride;
}



///////////////////////////////////////////////////////////////////////////////
// build interleaved vertices: vn,vn,vn... OR vnt,vnt,vnt...
///////////////////////////////////////////////////////////////////////////////
void ObjModel::buildInterleavedVerticesVN()
{
    unsigned int count = (unsigned int)vertices.size();
    for(unsigned int i = 0; i < count; i += 3)
    {
        interleavedVertices.push_back(vertices[i]);
        interleavedVertices.push_back(vertices[i+1]);
        interleavedVertices.push_back(vertices[i+2]);

        interleavedVertices.push_back(normals[i]);
        interleavedVertices.push_back(normals[i+1]);
        interleavedVertices.push_back(normals[i+2]);
    }
}
void ObjModel::buildInterleavedVerticesVNT()
{
    unsigned int count = (unsigned int)vertices.size();
    for(unsigned int i = 0, j = 0; i < count; i += 3, j += 2)
    {
        interleavedVertices.push_back(vertices[i]);
        interleavedVertices.push_back(vertices[i+1]);
        interleavedVertices.push_back(vertices[i+2]);

        interleavedVertices.push_back(normals[i]);
        interleavedVertices.push_back(normals[i+1]);
        interleavedVertices.push_back(normals[i+2]);

        interleavedVertices.push_back(texCoords[j]);
        interleavedVertices.push_back(texCoords[j+1]);
    }
}



///////////////////////////////////////////////////////////////////////////////
// save to OBJ file
// If the texture coords are not needed, set "textured" flag to false.
// If the 4x4 transform matrix is provided, thr vertex data will be transform
// before saving. But, the original vertex data are not changed.
///////////////////////////////////////////////////////////////////////////////
bool ObjModel::save(const char* fileName, bool textured, const float* matrix)
{
    // validate file name
    if(!fileName)
    {
        errorMessage = "File name is not defined.";
        return false;
    }

    float rotMat[16] = {1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1};
    if(matrix)
    {
        //@@ use only rotation part for normal transform
        rotMat[0] = matrix[0]; rotMat[1] = matrix[1]; rotMat[2] = matrix[2];
        rotMat[4] = matrix[4]; rotMat[5] = matrix[5]; rotMat[6] = matrix[6];
        rotMat[8] = matrix[8]; rotMat[9] = matrix[9]; rotMat[10]= matrix[10];
    }

    // build mtl file name
    std::string mtlFile = fileName;
    mtlFile = mtlFile.substr(0, mtlFile.find_last_of("."));
    mtlFile += ".mtl";
    //std::cout << "MTL: " << mtlFile << std::endl;

    // open a file
    std::ofstream outFile;
    outFile.open(fileName);
    if(!outFile.good())
    {
        errorMessage = "Failed to open a file to save: ";
        errorMessage += fileName;
        outFile.close();
        return false;
    }

    // get the current time
    time_t t  = time(0);

    // print comment
    outFile << "# Generated by ObjModel. -- " << ctime(&t)
            << "# Triangle Count: " << getTriangleCount() << "\n"
            << "#   Vertex Count: " << getVertexCount() << "\n"
            << "#    Group Count: " << getGroupCount() << "\n"
            << "\n";

    // print mtl file name
    outFile << "mtllib " << mtlFile << "\n\n";

    // tmp containers to store unique values
    std::map<std::string,std::size_t> vertexMap;
    std::map<std::string,std::size_t> normalMap;
    std::map<std::string,std::size_t> texCoordMap;
    std::pair<std::map<std::string,std::size_t>::iterator,bool> result;
    std::size_t* vertexIds = new std::size_t[vertices.size()];
    std::size_t* normalIds = new std::size_t[normals.size()];
    std::size_t* texCoordIds = new std::size_t[texCoords.size()];
    std::stringstream ss;
    std::size_t index;

    // print vertices
    Vector3 v;
    std::size_t i;
    std::size_t count = vertices.size();
    index = 1; // 1-base index
    for(i = 0; i < count; i += 3)
    {
        v.set(vertices[i], vertices[i+1], vertices[i+2]);

        // add unique vertex position with index to the tmp map
        ss.str("");
        ss << v.x << "," << v.y << "," << v.z;
        result = vertexMap.insert(std::pair<std::string,std::size_t>(ss.str(), index));
        if(result.second) // success to added new one
        {
            // transform if necessary
            if(matrix)
                v = this->transform(matrix, v);

            // print it to the file
            outFile << "v " << v.x << " " << v.y << " " << v.z << "\n";
            ++index; // for next
        }
        vertexIds[i/3] = result.first->second; // store unique index
    }
    outFile << "\n";

    // print normals
    index = 1; // 1-base in OBJ
    for(i = 0; i < count; i += 3)
    {
        v.set(normals[i], normals[i+1], normals[i+2]);

        // add unique normal with index to the tmp map
        ss.str("");
        ss << v.x << "," << v.y << "," << v.z;
        result = normalMap.insert(std::pair<std::string,std::size_t>(ss.str(),index));
        if(result.second) // success to added new one
        {
            if(matrix)
                v = this->transform(rotMat, v);

            outFile << "vn " << v.x << " " << v.y << " " << v.z << "\n";
            ++index;
        }
        normalIds[i/3] = result.first->second; // store unique index
    }
    outFile << "\n";

    // print texcoords
    count = texCoords.size();
    if(textured && count > 0)
    {
        index = 1;
        for(i = 0; i < count; i += 2)
        {
            // add unique uv with index to the tmp map
            ss.str("");
            ss << texCoords[i] << "," << texCoords[i+1];
            result = texCoordMap.insert(std::pair<std::string,std::size_t>(ss.str(), index));
            if(result.second) // success to added new one
            {
                // t-ccords are reversed (top-to-bottom)
                outFile << "vt " << texCoords[i] << " " << 1 - texCoords[i+1] << "\n";
                ++index;
            }
            texCoordIds[i/2] = result.first->second; // store unique index
        }
    }
    outFile << "\n";

    // print groups
    count = getGroupCount();
    for(i = 0; i < count; ++i)
    {
        outFile << "g " << groups[i].name << "\n";
        if(groups[i].materialName.size() > 0)
            outFile << "usemtl " << groups[i].materialName << "\n";

        // print faces
        const unsigned int* indices = getIndices((int)i);
        for(unsigned int j = 0; j < groups[i].indexCount; j += 3)
        {
            if(textured && texCoords.size() > 0)
            {
                outFile << "f " << vertexIds[indices[j]]   << "/" << texCoordIds[indices[j]]   << "/" << normalIds[indices[j]]   << " "
                                << vertexIds[indices[j+1]] << "/" << texCoordIds[indices[j+1]] << "/" << normalIds[indices[j+1]] << " "
                                << vertexIds[indices[j+2]] << "/" << texCoordIds[indices[j+2]] << "/" << normalIds[indices[j+2]] << "\n";
                //outFile << "f " << indices[j]+1   << "/" << indices[j]+1   << "/" << indices[j]+1   << " "
                //                << indices[j+1]+1 << "/" << indices[j+1]+1 << "/" << indices[j+1]+1 << " "
                //                << indices[j+2]+1 << "/" << indices[j+2]+1 << "/" << indices[j+2]+1 << "\n";
            }
            else
            {
                outFile << "f " << vertexIds[indices[j]]   << "//" << normalIds[indices[j]]   << " "
                                << vertexIds[indices[j+1]] << "//" << normalIds[indices[j+1]] << " "
                                << vertexIds[indices[j+2]] << "//" << normalIds[indices[j+2]] << "\n";
                //outFile << "f " << indices[j]+1   << "//" << indices[j]+1   << " "
                //                << indices[j+1]+1 << "//" << indices[j+1]+1 << " "
                //                << indices[j+2]+1 << "//" << indices[j+2]+1 << "\n";
            }
        }
        outFile << "\n";
    }

    // close opened file
    outFile.close();

    // write mtl file
    outFile.open(mtlFile.c_str());
    if(!outFile.good())
    {
        errorMessage = "Failed to open a mtl file to save: ";
        errorMessage += mtlFile;
        outFile.close();
        return false;
    }

    // print comment
    outFile << "# Generated by ObjModel. -- " << ctime(&t)
            << "# Material Count: " << getMaterialCount() << "\n"
            << "\n";

    // print newmtl
    count = getMaterialCount();
    for(i = 0; i < count; ++i)
    {
        const ObjMaterial& material = getMaterial((int)i);
        outFile << "newmtl " << material.name << "\n"
                << "Ka " << material.ambient[0] << " " << material.ambient[1] << " " << material.ambient[2] << "\n"
                << "Kd " << material.diffuse[0] << " " << material.diffuse[1] << " " << material.diffuse[2] << "\n"
                << "Ks " << material.specular[0] << " " << material.specular[1] << " " << material.specular[2] << "\n"
                << "Ns " << material.shininess <<"\n";
        if(material.textureName.size() > 0)
        {
            outFile << "map_Kd " << material.textureName << "\n";
        }
        outFile << "\n";
    }

    // close opened file
    outFile.close();

    // clean memory
    delete [] vertexIds;
    delete [] normalIds;
    delete [] texCoordIds;

    return true;
}



///////////////////////////////////////////////////////////////////////////////
// transform a vertex data by multiplying by a 4x4 matrix
///////////////////////////////////////////////////////////////////////////////
Vector3 ObjModel::transform(const float* mat, const Vector3& vec)
{
    Vector3 result;
    result.x = mat[0] * vec.x + mat[1] * vec.y + mat[2] * vec.z + mat[3];
    result.y = mat[4] * vec.x + mat[5] * vec.y + mat[6] * vec.z + mat[7];
    result.z = mat[8] * vec.x + mat[9] * vec.y + mat[10]* vec.z + mat[11];
    return result;
}

