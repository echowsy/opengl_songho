///////////////////////////////////////////////////////////////////////////////
// BitmapFont.h
// ============
//
//  AUTHOR: Song Ho Ahn (song.ahn@gmail.com)
// CREATED: 2009-04-15
// UPDATED: 2012-03-16
///////////////////////////////////////////////////////////////////////////////

#ifndef BITMAP_FONT_H
#define BITMAP_FONT_H

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include <string>
#include <vector>
#include <map>
#include <utility>
#include "Vertices.h"
#include "Vectors.h"
#include "Tokenizer.h"



///////////////////////////////////////////////////////////////////////////////
// character descriptor
struct BitmapCharacter
{
    short x;
    short y;
    short width;
    short height;
    short xOffset;
    short yOffset;
    short xAdvance;
    short page;
    Vector4 uvs;    // s1, t1, s2, t2

    BitmapCharacter() : x(0), y(0), width(0), height(0), xOffset(0), yOffset(0),
                        xAdvance(0), page(0) {};
};



///////////////////////////////////////////////////////////////////////////////
class BitmapFont
{
public:
    BitmapFont();
    ~BitmapFont();

    bool loadFont(const std::string& fileName);
    int drawText(float x, float y, const char* str);
    int getTextWidth(const char* str);

    void setColor(float r, float g, float b, float a);
    void setColor(const float color[4]);
    void setColor(const Vector4& color);
    void setScale(float x, float y);
    void setScale(const Vector2& scale);
    void setAngle(float z);
    //void setAngle(float x, float y, float z);

    short getHeight() const                 { return size; }
    short getBaseline() const               { return base; }

protected:


private:
    void parse(const char* src);
    //void parseInfo(Tokenizer& str);
    void parseCommon(Tokenizer& str);
    void parsePage(Tokenizer& str);
    void parseCharacterCount(Tokenizer& str);
    void parseCharacter(Tokenizer& str);
    void parseKerning(Tokenizer& str);
    void initUVs();

    GLuint loadBitmap(const std::string& name);

    static void getFieldAndValue(const std::string& str, std::string& field, std::string& value);
    static void getFieldAndValue(const std::string& str, std::string& field, short& value);
    static void trimQuotes(std::string& str);

    //std::string name;
    short size;
    short base;
    short bitmapWidth;
    short bitmapHeight;
    float bitmapWidthInv;
    float bitmapHeightInv;
    short characterCount;

    //short pageCount;
    std::vector<GLuint> pages;
    std::map<short, BitmapCharacter> characters;
    std::map<std::pair<short, short>, short> kernings;
    std::string path;
    
    static Vertex2 quadVertices[4];
    float color[4];
    Vector2 scale;
    Vector3 angle;
};

#endif
