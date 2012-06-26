#ifndef _COMMON_H_
#define _COMMON_H_

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define FUXI_FATAL(msg) \
    printf("Fatal error in line %d of %s : %s\n", __LINE__, __FILE__, msg); \
    printf("Exit.\n"); \
    exit(-1);

#define FUXI_CHECK(condition, msg) \
    if (!(condition)) { \
        FUXI_FATAL(msg); \
    }

typedef struct {
	float x, y, z;
} vec3;

typedef struct 
{
    unsigned short int bfType; //magic identifier
    unsigned int size; //size of the file
    unsigned short int reserved1; //
    unsigned short int reserved2; //
    unsigned int bfOffBits; //offset to image data
} BITMAPFILEHEADER;

typedef struct {
    unsigned int imagesize; //specifies the number of bytes required by the struct
    int biWidth , biHeight; //species height and width in pixels of the image
    unsigned short int planes; //the number of color planes
    unsigned short int bits; //specifies the number of bits per pixel
    unsigned int compression; //spcifies the type of compression
    unsigned int biSizeImage; //size of image in bytes
    unsigned int xresolution, yresolution; //pixels in x and y axis
    unsigned int nColours; //number of colors used by the bitmap
    unsigned int importantColours; //number of colors that are important in the image
} BITMAPINFOHEADER;

#define GL_CHECK(x) \
        x; \
        { \
          GLenum glError = glGetError(); \
          if(glError != GL_NO_ERROR) { \
            fprintf(stderr, "glGetError() = %i (0x%.8x) at line %i\n", glError, glError, __LINE__); \
            exit(1); \
          } \
        }

#define EGL_CHECK(x) \
    x; \
    { \
        EGLint eglError = eglGetError(); \
        if(eglError != EGL_SUCCESS) { \
            fprintf(stderr, "eglGetError() = %i (0x%.8x) at line %i\n", eglError, eglError, __LINE__); \
            exit(1); \
        } \
    }


// Load a bitmap from disk
unsigned char *LoadBitmapFile(char *filename, BITMAPINFOHEADER *bitmapInfoHeader);

#endif // _COMMON_H_