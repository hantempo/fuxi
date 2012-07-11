#include "image.h"

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

Image::Image()
: width(0), height(0), channels(0), pixels(NULL)
{
}

Image::Image(int w, int h, int c)
: width(w), height(h), channels(c),
  pixels(new unsigned char[w*h*c])
{

}

Image::~Image()
{
    delete [] pixels;
    pixels = NULL;
}

void Image::set_size(int w, int h, int c)
{
    width = w;
    height = h;
    channels = c;
    delete [] pixels;
    pixels = new unsigned char[w * h * c];
}

float Image::overdraw_ratio() const
{
    FUXI_DEBUG_ASSERT(channels == 1, "Only for stencil framebuffer");

    unsigned int total = 0;
    unsigned int visible = 0;
    unsigned char * patrol = pixels;
    for (int i = 0; i < width; ++i)
    {
        for (int j = 0; j < height; ++j)
        {
            unsigned char v = *(patrol++);
            if (v)
                ++visible;
            total += v;
        }
    }

    return (float)total / visible;
}

void Image::LoadBMP(const char *filepath, Image &image)
{
    FILE * f = fopen(filepath, "rb");
    FUXI_DEBUG_ASSERT(f, "Can't read file provided");

    BITMAPFILEHEADER bitmapFileHeader;
    fread(&bitmapFileHeader.bfType, 2, 1, f);
    FUXI_DEBUG_ASSERT(bitmapFileHeader.bfType == 0x4D42, "Not a bitmap file");
    fread(&bitmapFileHeader.size, 4, 1, f);
    fread(&bitmapFileHeader.reserved1, 2, 1, f);
    fread(&bitmapFileHeader.reserved2, 2, 1, f);
    fread(&bitmapFileHeader.bfOffBits, 4, 1, f);
    
    BITMAPINFOHEADER bitmapInfoHeader;
    fread(&bitmapInfoHeader.imagesize, 4, 1, f);
    fread(&bitmapInfoHeader.biWidth, 4, 1, f);
    fread(&bitmapInfoHeader.biHeight, 4, 1, f);
    fread(&bitmapInfoHeader.planes, 2, 1, f);
    fread(&bitmapInfoHeader.bits, 2, 1, f);
    fread(&bitmapInfoHeader.compression, 4, 1, f);
    fread(&bitmapInfoHeader.biSizeImage, 4, 1, f);
    fread(&bitmapInfoHeader.xresolution, 4, 1, f);
    fread(&bitmapInfoHeader.yresolution, 4, 1, f);
    fread(&bitmapInfoHeader.nColours, 4, 1, f);
    fread(&bitmapInfoHeader.importantColours, 4, 1, f);
    
    //move file point to the begging of bitmap data
    fseek(f, bitmapFileHeader.bfOffBits, SEEK_SET);

    image.set_size(bitmapInfoHeader.biWidth, bitmapInfoHeader.biHeight,
        bitmapInfoHeader.bits / 8);

    fread(image.pixels, bitmapInfoHeader.biSizeImage, 1, f);

    //swap the r and b values to get RGB (bitmap is BGR)
    for (int i = 0; i < bitmapInfoHeader.biSizeImage; i += 3)
    {
        const unsigned char tempRGB = image.pixels[i];
        image.pixels[i] = image.pixels[i + 2];
        image.pixels[i + 2] = tempRGB;
    }

    fclose(f);
}
