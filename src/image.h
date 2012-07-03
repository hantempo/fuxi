#ifndef _INCLUDE_IMAGE_
#define _INCLUDE_IMAGE_

#include "common.h"

class Image
{
public:
    int width, height, channels;
    unsigned char *pixels;

    Image();
    Image(int w, int h, int c = 4);
    virtual ~Image();

    void set_size(int w, int h, int c);

    float overdraw_ratio() const;

    static void LoadBMP(const char *filepath, Image &image);
};

#endif // _INCLUDE_IMAGE_
