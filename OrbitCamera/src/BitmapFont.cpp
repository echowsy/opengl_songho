///////////////////////////////////////////////////////////////////////////////
// BitmapFont.cpp
// ==============
//
//  AUTHOR: Song Ho Ahn (song.ahn@gmail.com)
// CREATED: 2009-04-15
// UPDATED: 2017-09-06
///////////////////////////////////////////////////////////////////////////////

#ifdef _WIN32
#include <windows.h>    // include windows.h to avoid thousands of compile errors even though this class is not depending on Windows
#endif

#include <iostream>
#include <sstream>
#include <fstream>
#include "BitmapFont.h"
#include "Tga.h"

// static member definition
Vertex2 BitmapFont::quadVertices[4];

///////////////////////////////////////////////////////////////////////////////
// ctor
///////////////////////////////////////////////////////////////////////////////
BitmapFont::BitmapFont() : size(0), base(0), bitmapWidth(0), bitmapHeight(0),
                           bitmapWidthInv(1), bitmapHeightInv(1)
{
    color[0] = color[1] = color[2] = color[3] = 1.0f;
    scale.x = scale.y = 1.0f;
}



///////////////////////////////////////////////////////////////////////////////
// dtor
///////////////////////////////////////////////////////////////////////////////
BitmapFont::~BitmapFont()
{
    for(size_t i = 0; i < pages.size(); ++i)
    {
        glDeleteTextures(1, &pages[i]);
    }
    pages.clear();
    kernings.clear();
    characters.clear();
}



///////////////////////////////////////////////////////////////////////////////
// read *.fnt file
///////////////////////////////////////////////////////////////////////////////
bool BitmapFont::loadFont(const std::string& fileName)
{
    // remember directory
    size_t found = fileName.find_last_of("/\\");
    if(found != std::string::npos)
        path = fileName.substr(0, found+1);
    else
        path = "";
    
    // open the file
    std::ifstream inFile;
    inFile.open(fileName.c_str(), std::ios::in);
    if(!inFile.good())
    {
        std::cout << "[ERROR] Failed to open file, \"" << fileName << "\"." << std::endl;
        return false;
    }

    // compute buffer size
    inFile.seekg(0, std::ios::end);
    size_t bufferSize = inFile.tellg();
    inFile.seekg(0, std::ios::beg);

    // get memory space based on image data size
    char* buffer = new char[bufferSize + 1];
    buffer[bufferSize] = '\0';

    // read data
    inFile.read(buffer, bufferSize);

    // close the image file
    inFile.close();

    // parse font file
    parse(buffer);

    delete [] buffer;
    return true;
}



///////////////////////////////////////////////////////////////////////////////
// parse font file
///////////////////////////////////////////////////////////////////////////////
void BitmapFont::parse(const char* src)
{
    if(!src)
        return;

    // reset members
    for(size_t i = 0; i < pages.size(); ++i)
    {
        glDeleteTextures(1, &pages[i]);
    }
    pages.clear();
    kernings.clear();
    characters.clear();

    Tokenizer lines(src, "\n");
    Tokenizer fields;
    std::string line;
    std::string field;

    while((line = lines.next()) != "")
    {
        fields.set(line);
        field = fields.next();

        if(field == "info")
        {
        //    parseInfo(fields);
        }
        else if(field == "common")
        {
            parseCommon(fields);
        }
        else if(field == "page")
        {
            parsePage(fields);
        }
        else if(field == "chars")
        {
            parseCharacterCount(fields);
        }
        else if(field == "char")
        {
            parseCharacter(fields);
        }
        else if(field == "kerning")
        {
            parseKerning(fields);
        }
    }

    // set uvs per character
    initUVs();
}



///////////////////////////////////////////////////////////////////////////////
// print text and return width of the text
///////////////////////////////////////////////////////////////////////////////
int BitmapFont::drawText(float x, float y, const char* str)
{
    if(!str)
        return 0;

    char chr;
    char prevChr = 0;
    BitmapCharacter character;
    std::pair<short, short> kerning;

    // snap to pixel
    int shiftX = (int)(x + 0.5f);
    int shiftY = (int)(y + 0.5f);

    // OpenGL calls
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex2), &quadVertices[0].s);
    glVertexPointer(2, GL_FLOAT, sizeof(Vertex2), &quadVertices[0].x);

    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    //glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);
    glColor4fv(this->color);

    glPushMatrix();
    glTranslatef(shiftX, shiftY, 0);
    glScalef(scale.x, scale.y, 1);
    //glRotatef(angle.x, 1,0,0);
    //glRotatef(angle.y, 0,1,0);
    glRotatef(angle.z, 0,0,1);

    int cursor = 0;
    while((chr = *str++) != '\0')
    {
        character = characters[(short)chr];

        // kerning
        kerning.first = prevChr;
        kerning.second = chr;
        cursor += kernings[kerning];

        quadVertices[0].s = quadVertices[1].s = character.uvs[0];
        quadVertices[0].t = quadVertices[2].t = character.uvs[1];
        quadVertices[0].x = quadVertices[1].x = cursor + character.xOffset;
        quadVertices[0].y = quadVertices[2].y = base - character.yOffset;

        quadVertices[3].s = quadVertices[2].s = character.uvs[2];
        quadVertices[3].t = quadVertices[1].t = character.uvs[3];
        quadVertices[3].x = quadVertices[2].x = quadVertices[0].x + character.width;
        quadVertices[3].y = quadVertices[1].y = quadVertices[0].y - character.height;

        glBindTexture(GL_TEXTURE_2D, pages[character.page]);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        // next
        cursor += character.xAdvance;
        prevChr = chr;
    }

    glPopMatrix();

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    glDisable(GL_BLEND);
    glDisable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHTING);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    return (int)(cursor * scale.x + 0.5f);  // return width of the current string
}


///////////////////////////////////////////////////////////////////////////////
// compute the width of the text
///////////////////////////////////////////////////////////////////////////////
int BitmapFont::getTextWidth(const char* str)
{
    if(!str)
        return 0;
    
    char chr;
    char prevChr = 0;
    std::pair<short, short> kerning;

    int cursor = 0;
    while((chr = *str++) != '\0')
    {
        // kerning
        kerning.first = prevChr;
        kerning.second = chr;
        cursor += kernings[kerning];
        
        // next
        cursor += characters[(short)chr].xAdvance;
        prevChr = chr;
    }
    return (int)(cursor * scale.x + 0.5f);
}



/*
///////////////////////////////////////////////////////////////////////////////
void BitmapFont::parseInfo(Tokenizer& str)
{
    std::string subStr;
    std::string field;
    std::string value;
    std::string faceName;

    while((subStr = str.next()) != "")
    {
        getFieldAndValue(subStr, field, value);
        if(field == "face")
        {
            faceName = value;
            if(value.at(value.size()-1) != '\"')
            {
                std::string tmpStr;
                do
                {
                    tmpStr += " " + str.next();
                }
                while(tmpStr.find("\"") == std::string::npos);
                faceName += tmpStr;
            }
            trimQuotes(faceName);
            this->name = faceName;
        }
    }
}
*/



///////////////////////////////////////////////////////////////////////////////
void BitmapFont::parseCommon(Tokenizer& str)
{
    std::string subStr;
    std::string field;
    short value;

    while((subStr = str.next()) != "")
    {
        getFieldAndValue(subStr, field, value);
        if(field == "lineHeight")
        {
            this->size = value;
        }
        else if(field == "base")
        {
            this->base = value;
        }
        else if(field == "scaleW")
        {
            this->bitmapWidth = value;
            this->bitmapWidthInv = 1.0f / this->bitmapWidth;
        }
        else if(field == "scaleH")
        {
            this->bitmapHeight = value;
            this->bitmapHeightInv = 1.0f / this->bitmapHeight;
        }
        //else if(field == "pages")
        //{
        //    this->pageCount = value;
        //}
    }
}



///////////////////////////////////////////////////////////////////////////////
void BitmapFont::parsePage(Tokenizer& str)
{
    std::string subStr;
    std::string field;
    std::string value;
    std::string fileName;

    while((subStr = str.next()) != "")
    {
        getFieldAndValue(subStr, field, value);
        if(field == "file")
        {
            fileName = value;
            if(value.at(value.size()-1) != '\"')
            {
                std::string tmpStr;
                do
                {
                    tmpStr += " " + str.next();
                }
                while(tmpStr.find("\"") == std::string::npos);
                fileName += tmpStr;
            }
            trimQuotes(fileName);
            GLuint texId = this->loadBitmap(path + fileName);
            pages.push_back(texId);
        }
    }
}



///////////////////////////////////////////////////////////////////////////////
void BitmapFont::parseCharacterCount(Tokenizer& str)
{
    std::string subStr;
    std::string field;
    short value;
    
    while((subStr = str.next()) != "")
    {
        getFieldAndValue(subStr, field, value);
        if(field == "count")
        {
            characterCount = value;
        }
    }
}



///////////////////////////////////////////////////////////////////////////////
void BitmapFont::parseCharacter(Tokenizer& str)
{
    std::string subStr;
    std::string field;
    short value;
    short index = 0;

    while((subStr = str.next()) != "")
    {
        getFieldAndValue(subStr, field, value);
        if(field == "id")
        {
            index = value;
            BitmapCharacter character;
            characters[index] = character;
        }
        else if(field == "x")
        {
            characters[index].x = value;
        }
        else if(field == "y")
        {
            characters[index].y = value;
        }
        else if(field == "width")
        {
            characters[index].width = value;
        }
        else if(field == "height")
        {
            characters[index].height = value;
        }
        else if(field == "xoffset")
        {
            characters[index].xOffset = value;
        }
        else if(field == "yoffset")
        {
            characters[index].yOffset = value;
        }
        else if(field == "xadvance")
        {
            characters[index].xAdvance = value;
        }
        else if(field == "page")
        {
            characters[index].page = value;
        }
    }
}



///////////////////////////////////////////////////////////////////////////////
void BitmapFont::parseKerning(Tokenizer& str)
{
    std::string subStr;
    std::string field;
    short value;
    std::pair <short, short> kerning;

    while((subStr = str.next()) != "")
    {
        getFieldAndValue(subStr, field, value);
        if(field == "first")
        {
            kerning.first = value;
        }
        else if(field == "second")
        {
            kerning.second = value;
        }
        else if(field == "amount")
        {
            kernings[kerning] = value;
        }
    }
}



///////////////////////////////////////////////////////////////////////////////
void BitmapFont::getFieldAndValue(const std::string& str, std::string& field, std::string& value)
{
    Tokenizer tokenizer(str, "=");
    field = tokenizer.next();
    value = tokenizer.next();
}

void BitmapFont::getFieldAndValue(const std::string& str, std::string& field, short& value)
{
    std::string valueStr;
    Tokenizer tokenizer(str, "=");
    field = tokenizer.next();
    valueStr = tokenizer.next();
    std::stringstream ss(valueStr);
    ss >> value;
}



///////////////////////////////////////////////////////////////////////////////
// trim leading and tailing quote char
///////////////////////////////////////////////////////////////////////////////
void BitmapFont::trimQuotes(std::string& str)
{
    str = str.substr(1, str.size()-2);
}



///////////////////////////////////////////////////////////////////////////////
// set font colour
///////////////////////////////////////////////////////////////////////////////
void BitmapFont::setColor(float r, float g, float b, float a)
{
    this->color[0] = r;
    this->color[1] = g;
    this->color[2] = b;
    this->color[3] = a;
}

void BitmapFont::setColor(const float color[4])
{
    if(!color)
        return;

    setColor(color[0], color[1], color[2], color[3]);
}

void BitmapFont::setColor(const Vector4& color)
{
    setColor(color[0], color[1], color[2], color[3]);
}



///////////////////////////////////////////////////////////////////////////////
// set scale
///////////////////////////////////////////////////////////////////////////////
void BitmapFont::setScale(float x, float y)
{
    scale.x = x;
    scale.y = y;
}

void BitmapFont::setScale(const Vector2& scale)
{
    this->scale = scale;
}



///////////////////////////////////////////////////////////////////////////////
// set angle
///////////////////////////////////////////////////////////////////////////////
void BitmapFont::setAngle(float z)
{
    angle.z = z;
}
/*
void BitmapFont::setAngle(float x, float y, float z)
{
    angle.x = x;
    angle.y = y;
    angle.z = z;
}
*/



///////////////////////////////////////////////////////////////////////////////
// load texture
///////////////////////////////////////////////////////////////////////////////
GLuint BitmapFont::loadBitmap(const std::string& name)
{
    Image::Tga tga;
    tga.read(name.c_str());
    GLint format;
    switch(tga.getBitCount())
    {
        case 8:
            format = GL_ALPHA;
            break;
        case 24:
            format = GL_RGB;
            break;
        case 32:
            format = GL_RGBA;
            break;
    }

    // copy the texture to OpenGL
    GLuint texture;
    glGenTextures(1, &texture);

    // set active texture and configure it
    glBindTexture(GL_TEXTURE_2D, texture);

    // select modulate to mix texture with color for shading
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    glTexImage2D(GL_TEXTURE_2D, 0, format, tga.getWidth(), tga.getHeight(), 0, format, GL_UNSIGNED_BYTE, tga.getDataRGB());

    glBindTexture(GL_TEXTURE_2D, 0);

    return texture;
}



///////////////////////////////////////////////////////////////////////////////
void BitmapFont::initUVs()
{
    std::map<short, BitmapCharacter>::iterator iter = characters.begin();
    while(iter != characters.end())
    {
        iter->second.uvs[0] = iter->second.x * bitmapWidthInv;
        iter->second.uvs[1] = iter->second.y * bitmapHeightInv;
        iter->second.uvs[2] = (iter->second.x + iter->second.width) * bitmapWidthInv;
        iter->second.uvs[3] = (iter->second.y + iter->second.height) * bitmapHeightInv;

        ++iter;
    }
}
