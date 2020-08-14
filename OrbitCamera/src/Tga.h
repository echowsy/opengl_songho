// Tga.h
// =====
// Targa image loader and writer
// It reads uncompressed and RLE compressed color (24-bit and 32-bit) and 
// grayscale image.
// And, it saves as only uncompressed color or grayscale image.
//
//  AUTHOR: Song Ho Ahn (song.ahn@gmail.com)
// CREATED: 2006-07-17
// UPDATED: 2013-01-23
///////////////////////////////////////////////////////////////////////////////

#ifndef IMAGE_TGA_H
#define IMAGE_TGA_H

#include <string>

namespace Image
{
    class Tga
    {
    public:
        // ctor/dtor
        Tga();
        Tga(const Tga &rhs);
        ~Tga();

        Tga& operator=(const Tga &rhs);             // assignment operator

        // load image header and data from a TGA file
        bool read(const char* fileName);

        // save an image as TGA format
        // It assumes the color order of input image is RGB, so it will convert to BGR order before save
        static bool save(const char* fileName, int width, int height, int channelCount, const unsigned char* data);

        // getters
        int getWidth() const;                       // return width of image in pixel
        int getHeight() const;                      // return height of image in pixel
        int getBitCount() const;                    // return the number of bits per pixel (8, 24, or 32)
        std::size_t getDataSize() const;            // return data size in bytes
        const unsigned char* getData() const;       // return the pointer to image data
        const unsigned char* getDataRGB() const;    // return image data as RGB/RGBA order

        void printSelf() const;                     // print itself for debug purpose
        const char* getError() const;               // return last error message


    protected:


    private:
        // member functions
        void init();                                // clear the existing values

        // shared functions (only 1 copy of the function, even if there are multiple instances of this class)
        static bool decodeRLE(const unsigned char *encData, unsigned char *data, std::size_t encSize, int channelCount); // decode TGA RLE to uncompressed
        static void flipImage(unsigned char *data, int width, int height, int channelCount);    // flip the vertical orientation
        static void swapRedBlue(unsigned char *data, int dataSize, int channelCount);           // swap the position of red and blue components

        // member variables
        int width;
        int height;
        int bitCount;
        std::size_t dataSize;
        unsigned char *data;                        // data with default BGR order
        unsigned char *dataRGB;                     // extra copy of image data with RGB order
        std::string errorMessage;
    };



    ///////////////////////////////////////////////////////////////////////////
    // inline functions
    ///////////////////////////////////////////////////////////////////////////
    inline int Tga::getWidth() const { return width; }
    inline int Tga::getHeight() const { return height; }

    // return bits per pixel, 8 means grayscale, 24 means RGB color, 32 means RGBA
    inline int Tga::getBitCount() const { return bitCount; }

    inline std::size_t Tga::getDataSize() const { return dataSize; }
    inline const unsigned char* Tga::getData() const { return data; }
    inline const unsigned char* Tga::getDataRGB() const { return dataRGB; }

    inline const char* Tga::getError() const { return errorMessage.c_str(); }
}

#endif // IMAGE_TGA_H
