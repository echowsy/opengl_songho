// Tga.cpp
// =======
// Targa image loader and writer
// It reads uncompressed and RLE compressed color (24-bit and 32-bit) and 
// grayscale image.
// And, it saves as only uncompressed color or grayscale image.
//
//  AUTHOR: Song Ho Ahn (song.ahn@gmail.com)
// CREATED: 2006-07-17
// UPDATED: 2014-04-05
///////////////////////////////////////////////////////////////////////////////

#include <fstream>
#include <iostream>
#include <cstring>                      // for memcpy()
#include "Tga.h"
using std::ifstream;
using std::ofstream;
using std::ios;
using std::cout;
using std::endl;
using namespace Image;



///////////////////////////////////////////////////////////////////////////////
// default constructor
///////////////////////////////////////////////////////////////////////////////
Tga::Tga() : width(0), height(0), bitCount(0), dataSize(0), data(0), dataRGB(0),
             errorMessage("No error.")
{
}



///////////////////////////////////////////////////////////////////////////////
// copy constructor
// We need DEEP COPY for dynamic memory variables because the compiler inserts
// default copy constructor automatically for you, BUT it is only SHALLOW COPY
///////////////////////////////////////////////////////////////////////////////
Tga::Tga(const Tga &rhs)
{
    // copy member variables from right-hand-side object
    width = rhs.getWidth();
    height = rhs.getHeight();
    bitCount = rhs.getBitCount();
    dataSize = rhs.getDataSize();

    if(rhs.getData())       // allocate memory only if the pointer is not NULL
    {
        data = new unsigned char[dataSize];
        memcpy(data, rhs.getData(), dataSize); // deep copy
    }
    else
        data = 0;           // array is not allocated yet, set to 0

    if(rhs.getDataRGB())    // allocate memory only if the pointer is not NULL
    {
        dataRGB = new unsigned char[dataSize];
        memcpy(dataRGB, rhs.getDataRGB(), dataSize); // deep copy
    }
    else
        dataRGB = 0;        // array is not allocated yet, set to 0
}



///////////////////////////////////////////////////////////////////////////////
// default destructor
///////////////////////////////////////////////////////////////////////////////
Tga::~Tga()
{
    // deallocate data array
    delete [] data;
    data = 0;
    delete [] dataRGB;
    dataRGB = 0;
}



///////////////////////////////////////////////////////////////////////////////
// override assignment operator
///////////////////////////////////////////////////////////////////////////////
Tga& Tga::operator=(const Tga &rhs)
{
    if(this == &rhs)        // avoid self-assignment (A = A)
        return *this;

    // copy member variables
    width = rhs.getWidth();
    height = rhs.getHeight();
    bitCount = rhs.getBitCount();
    dataSize = rhs.getDataSize();

    if(rhs.getData())       // allocate memory only if the pointer is not NULL
    {
        data = new unsigned char[dataSize];
        memcpy(data, rhs.getData(), dataSize);
    }
    else
        data = 0;

    if(rhs.getDataRGB())   // allocate memory only if the pointer is not NULL
    {
        dataRGB = new unsigned char[dataSize];
        memcpy(dataRGB, rhs.getDataRGB(), dataSize);
    }
    else
        dataRGB = 0;

    return *this;
}



///////////////////////////////////////////////////////////////////////////////
// clear out the exsiting values
///////////////////////////////////////////////////////////////////////////////
void Tga::init()
{
    width = height = bitCount = 0;
    dataSize = 0;
    errorMessage = "No error.";
    delete [] data;
    data = 0;
    delete [] dataRGB;
    dataRGB = 0;
}



///////////////////////////////////////////////////////////////////////////////
// print itself for debug
///////////////////////////////////////////////////////////////////////////////
void Tga::printSelf() const
{
    cout << "===== Tga =====\n"
         << "Width: " << width << " pixels\n"
         << "Height: " << height << " pixels\n"
         << "Bit Count: " << bitCount << " bits\n"
         << "Data Size: " << dataSize  << " bytes\n"
         << endl;
}



///////////////////////////////////////////////////////////////////////////////
// read a Tga image header infos and datafile and load
///////////////////////////////////////////////////////////////////////////////
bool Tga::read(const char* fileName)
{
    this->init();   // clear out all values

    // check NULL pointer
    if(!fileName)
    {
        errorMessage = "File name is not defined (NULL pointer).";
        return false;
    }

    // check file extension
    if(strcmp(fileName + strlen(fileName) - 3, "tga") != 0)
    {
        errorMessage = "File extension is not tga.";
        return false;
    }

    // open a Tga file as binary mode
    ifstream inFile;
    inFile.open(fileName, ios::binary);         // binary mode
    if(!inFile.good())
    {
        errorMessage = "Failed to open a TGA file to read.";
        return false;            // exit if failed
    }

    // list of entries in TGA header (18 bytes)
    char idLength;          // length of image ID filed (1 bytes)
    char colormapType;      // colourmap type (1)
    char imageType;         // image type (1)
    short colormapOffset;   // colormap starting offset (2)
    short colormapCount;    // # of colors in colormap (2)
    char colormapDepth;     // bitCount per colormap (1)
    short originX;          // x origin of lower left corner of image (2)
    short originY;          // y origin of lower left corner of image (2)
    short width;            // image width (2)
    short height;           // image height (2)
    char bitCount;          // # of bits per pixel (1)
    char descriptor;        // image descriptor bits (1)

    // read Tga header infos
    inFile.read(&idLength, 1);                  // usually 0
    inFile.read(&colormapType, 1);              // 0 means no colormap, 1 means with colormap
    inFile.read(&imageType, 1);                 // 0=no image, 1=colormap image, 2=truecolor image, 3=gray image, 9,10,11=RLE compressed
    inFile.read((char*)&colormapOffset, 2);     // colormap starting offset
    inFile.read((char*)&colormapCount, 2);
    inFile.read(&colormapDepth, 1);             // should be 15, 16, 24, 32
    inFile.read((char*)&originX, 2);
    inFile.read((char*)&originY, 2);
    inFile.read((char*)&width, 2);
    inFile.read((char*)&height, 2);
    inFile.read(&bitCount, 1);                  // 8, 16, 24, or 32
    inFile.read(&descriptor, 1);                // use only vertical screen orientation (bit-5)

    // compute data size in bytes
    int dataSize = width * height * bitCount / 8;

    // it supports only true color image, no colormaped(palette) image
    if(colormapType != 0)
    {
        inFile.close();
        errorMessage = "Colormap (palette) type is not supported.";
        return false;
    }

    // it supports only 8-bit grayscale, 24-bit BGR or 32-bit BGRA
    if(bitCount != 8 && bitCount != 24 && bitCount != 32)
    {
        inFile.close();
        errorMessage = "Unsupported format.";
        cout << "bitCount: " << (int)bitCount << endl;
        return false;
    }

    // it supports only following image types
    // 2  : true color(bgr, bgra) image
    // 2+8: RLE compressed true color
    // 3  : grayscale image
    // 3+8: RLE compressed grayscale
    if(imageType != 2 && imageType != 3 && imageType != (2+8) && imageType != (3+8))
    {
        inFile.close();
        errorMessage = "Unsupported image type.";
        return false;
    }

    // allocate data array
    data = new unsigned char [dataSize];
    dataRGB = new unsigned char [dataSize];

    // now it is ready to store info and image data
    this->width = width;
    this->height = height;
    this->bitCount = bitCount;
    this->dataSize = dataSize;

    // compute data offset
    int dataOffset = 18;                    // 18 bytes for header
    dataOffset += idLength;                 // add length of id field

    // read data
    if(imageType == 2 || imageType == 3)    // uncompressed
    {
        inFile.seekg(dataOffset, ios::beg);     // move cursor to the starting position of data
        inFile.read((char*)data, dataSize);
    }
    // uncompressed
    else if(imageType == (2+8) || imageType == (3+8))
    {
        // get size of file
        inFile.seekg(0, ios::end);
        std::size_t size = inFile.tellg();

        // get length of encoded data
        size -= dataOffset;

        // allocate tmp array to store the encoded data
        unsigned char *encData = new unsigned char[size];

        // read data from file
        inFile.seekg(dataOffset, ios::beg);
        inFile.read((char*)encData, size);

        // decode RLE into image data buffer
        decodeRLE(encData, data, size, bitCount/8);

        // deallocate encoded data buffer after decoding
        delete [] encData;
    }

    // close it after reading
    inFile.close();

    // Tga is bottom-to-top orientation if bit-5 is 0. flip image vertically
    if((descriptor & 0x20) == 0x0)          // 20h = 100000b
        flipImage(data, width, height, bitCount/8);

    // the colour components order of Tga image is BGR
    // convert image data to RGB order for convenience
    memcpy(dataRGB, data, dataSize);    // copy data to dataRGB first
    if(bitCount == 24 || bitCount == 32)
        swapRedBlue(dataRGB, dataSize, bitCount/8);

    return true;
}



// static shared functions ****************************************************

///////////////////////////////////////////////////////////////////////////////
// save an image as an uncompressed Tga format
// We assume the source image is RGB order, so it must be converted BGR order.
///////////////////////////////////////////////////////////////////////////////
bool Tga::save(const char* fileName, int w, int h, int channelCount, const unsigned char* data)
{
    if(!fileName || !data) return false;

    // list of entries in TGA header (18 bytes)
    char idLength;          // length of image ID filed (1 bytes)
    char colormapType;      // colourmap type (1)
    char imageType;         // image type (1)
    short colormapOffset;   // colormap starting offset (2)
    short colormapCount;    // # of colors in colormap (2)
    char colormapDepth;     // bitCount per colormap (1)
    short originX;          // x origin of lower left corner of image (2)
    short originY;          // y origin of lower left corner of image (2)
    short width;            // image width (2)
    short height;           // image height (2)
    char bitCount;          // # of bits per pixel (1)
    char descriptor;        // image descriptor bits (1)

    idLength = (char)0;
    colormapType = (char)0;
    colormapOffset = (short)0;
    colormapCount = (short)0;
    colormapDepth = (char)0;
    originX = (short)0;
    originY = (short)0;
    width = (short)w;
    height = (short)h;
    bitCount = (char)(channelCount * 8);
    descriptor = (char)0;

    if(channelCount == 1)
        imageType = 3;      // grayscale
    else
        imageType = 2;      // color

    // allocate temp memory
    int dataSize = w * h * channelCount;
    unsigned char *tmpData = new unsigned char[dataSize];

    // copy image data
    memcpy(tmpData, data, dataSize);

    // flip the image upside down
    flipImage(tmpData, width, height, channelCount);

    // convert RGB to BGR order
    swapRedBlue(tmpData, dataSize, channelCount);

    // open output file
    ofstream outFile;
    outFile.open(fileName, ios::binary);
    if(!outFile.good()) return false;   // exit if failed

    // write header
    outFile.put(idLength);
    outFile.put(colormapType);
    outFile.put(imageType);
    outFile.write((char*)&colormapOffset, 2);
    outFile.write((char*)&colormapCount, 2);
    outFile.put(colormapDepth);
    outFile.write((char*)&originX, 2);
    outFile.write((char*)&originY, 2);
    outFile.write((char*)&width, 2);
    outFile.write((char*)&height, 2);
    outFile.put(bitCount);
    outFile.put(descriptor);

    // write image data
    outFile.write((char*)tmpData, dataSize);

    // close the opened file
    outFile.close();

    // deallocate tmp buffer
    delete [] tmpData;

    return true;
}



///////////////////////////////////////////////////////////////////////////////
// decode TGA RLE data into uncompressed data
// This routine needs 2 pointers; run-length encoded data as source and
// uncompressed output data. TGA RLE has 2 modes; one is run-length packet mode
// and the other is raw packet mode. Both modes has a 1-byte packet header 
// prior to colour values. The header consists of 2 parts. The bit-7 is a mode
// identifier. 1 means run-length mode and 0 means raw mode. The number of 
// counts are stored from bit-0 to bit-6, so the maximum value can be 127 in 
// this 7 digit field (from 0 to 127). However, the maximum run size is always
// 1 more than the value of this field because we count from 1, not 0.
// Therefore, the maximum run size is 128 (= 127+1).
// Header
// 7  6 5 4 3 2 1 0
// =  =============
// 1                : Run-Length packet mode
// 0                : Raw packet mode
//
// * Run-Length packet mode
// The following colour value repeats the number of time specified in the
// header, for example, if the header is 0x82 and the colour value is 0x01,
// 0x02, and 0x03 in BGR mode, then this colour will be repeated 3 times.
// Encoded      Decoded (BGR)
// ===========  ==========================
// 82 01 02 03  01 02 03 01 02 03 01 02 03
//
// * Raw packet mode
// In raw mode, the number of pixels specified in the header are decoded, for
// example, if the header is 0x01, then the following 2 pixels are copied to
// the output buffer, (A1,A2,A3) and (B4,B2,B3).
// Again, the count is always 1 more than the value in the header.
// Encoded               Decoded (BGR)
// ====================  =================
// 01 A1 A2 A3 B1 B2 B3  A1 A2 A3 B1 B2 B3
///////////////////////////////////////////////////////////////////////////////
bool Tga::decodeRLE(const unsigned char *encData, unsigned char *outData, std::size_t encDataSize, int channelCount)
{
    // check NULL pointer
    if(!encData || !outData)
        return false;

    const unsigned char* endPointer = encData + encDataSize - 1;

    unsigned char header;                   // RLE encode header (1-byte)
    int repeatCount;

    while(encData < endPointer)
    {
        // get header
        header = *encData;

        // run-length packet mode if bit-7 is 1
        if(header & 0x80)                   // 80h = 10000000b
        {
            // get # of colors to repeat from low 7 bits
            // NOTE: 7-bit can be 127 at max, but the # of repeats counts from 1, not 0.
            // Therefore, the possible repeat counts are from 0 to 128.
            repeatCount = (int)header - 0x80 + 1;
            ++encData;                      // move the pointer from header to data

            for(int i = 0; i < repeatCount; ++i)
            {
                memcpy(outData, encData, channelCount);
                outData += channelCount;   // next
            }

            // move to next header
            encData += channelCount;
        }

        // raw packet mode if bit-7 is 0
        else
        {
            // get the # of raw pixels
            // Again, the # of values are 1 more than actual value, because it always starts 1, not 0.
            repeatCount = header + 1;
            ++encData;                      // move from header to data

            for(int i = 0; i < repeatCount; ++i)
            {
                memcpy(outData, encData, channelCount);
                outData += channelCount;   // next
                encData += channelCount;   // next
            }
        }
    }

    return true;
}



///////////////////////////////////////////////////////////////////////////////
// Tga is bottom-to-top orientation. Flip the image vertically, so the image
// can be rendered from top to bottom orientation
///////////////////////////////////////////////////////////////////////////////
void Tga::flipImage(unsigned char *data, int width, int height, int channelCount)
{
    if(!data) return;

    int i;
    int lineSize = width * channelCount;        // the number of bytes per scanline
    unsigned char *line1 = data;                // the first scanline
    unsigned char *line2 = data + ((height-1)*lineSize); // the last scanline
    unsigned char tmp;

    // swap line #1 and line #2
    while(line1 < line2)
    {
        for(i=0; i < lineSize; ++i)
        {
            tmp = *line2;
            *line2++ = *line1;
            *line1++ = tmp;
        }

        // one scanline is done, move the pointer to scanline #2 1-line up
        line2 -= (lineSize * 2);
    }
}



///////////////////////////////////////////////////////////////////////////////
// swap the position of the 1st and 3rd color components (RGB <-> BGR)
///////////////////////////////////////////////////////////////////////////////
void Tga::swapRedBlue(unsigned char *data, int dataSize, int channelCount)
{
    if(!data) return;
    if(dataSize % channelCount) return;     // must be divisible by the number of channels

    unsigned char tmp;
    int i;

    // swap the position of red and blue components
    for(i=0; i < dataSize; i+=channelCount)
    {
        tmp = data[i];
        data[i] = data[i+2];
        data[i+2] = tmp;
    }
}
