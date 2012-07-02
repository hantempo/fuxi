#include "common.h"

// Load a bitmap from disk
unsigned char *LoadBitmapFile(const char *filename, BITMAPINFOHEADER *bitmapInfoHeader)
{
    unsigned char *bitmapImage = NULL;  //store image data
    FILE *filePtr; //our file pointer
    FILE *filePtr1;
    BITMAPFILEHEADER bitmapFileHeader; //our bitmap file header

    int imageIdx=0;  //image index counter
    unsigned char tempRGB;  //our swap variable

    //open filename in read binary mode
    filePtr = fopen(filename,"rb");

    if (filePtr == NULL)
    {
        printf("Can't read file provide;\n");
    }
    //read the bitmap file header
    ////////////////////////////////////////////////////////////////////////////////////////////
    fread(&bitmapFileHeader.bfType, 2,1,filePtr);
    fread(&bitmapFileHeader.size, 4,1,filePtr);
    fread(&bitmapFileHeader.reserved1, 2,1,filePtr);
    fread(&bitmapFileHeader.reserved2, 2,1,filePtr);
    fread(&bitmapFileHeader.bfOffBits, 4,1,filePtr);
    ////////////////////////////////////////////////////////////////////////////////////////////
    //verify that this is a bmp file by check bitmap id
    if (bitmapFileHeader.bfType !=0x4D42)
    {
        fclose(filePtr);
        printf("Its not a bitmap file;\n");
        return NULL;
    }

    //read the bitmap info header
    /////////////////////////////////////////////////////////////////////////////////////////

    fread(&bitmapInfoHeader->imagesize, 4,1,filePtr);
    fread(&bitmapInfoHeader->biWidth, 4,1,filePtr);
    fread(&bitmapInfoHeader->biHeight, 4,1,filePtr);
    fread(&bitmapInfoHeader->planes, 2,1,filePtr);
    fread(&bitmapInfoHeader->bits, 2,1,filePtr);
    fread(&bitmapInfoHeader->compression, 4,1,filePtr);
    fread(&bitmapInfoHeader->biSizeImage, 4,1,filePtr);
    fread(&bitmapInfoHeader->xresolution, 4,1,filePtr);
    fread(&bitmapInfoHeader->yresolution, 4,1,filePtr);
    fread(&bitmapInfoHeader->nColours, 4,1,filePtr);
    fread(&bitmapInfoHeader->importantColours, 4,1,filePtr);
    /////////////////////////////////////////////////////////////////////////////////////////

    //move file point to the begging of bitmap data
    fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);

    //allocate enough memory for the bitmap image data
    bitmapImage = (unsigned char*)malloc(bitmapInfoHeader->biSizeImage);

    //verify memory allocation
    if (!bitmapImage)
    {
        free(bitmapImage);
        fclose(filePtr);
        printf("Not enough memory;\n");
        return NULL;
    }

    //read in the bitmap image data
    fread(bitmapImage,bitmapInfoHeader->biSizeImage,1,filePtr);

    //make sure bitmap image data was read
    if (bitmapImage == NULL)
    {
        fclose(filePtr);
        printf("Didn't read anything off the image;\n");
        return NULL;
    }

    //swap the r and b values to get RGB (bitmap is BGR)
    for (imageIdx = 0; imageIdx < bitmapInfoHeader->biSizeImage; imageIdx += 3)
    {
        tempRGB = bitmapImage[imageIdx];
        bitmapImage[imageIdx] = bitmapImage[imageIdx + 2];
        bitmapImage[imageIdx + 2] = tempRGB;
    }

    //filePtr1 = fopen("filename.raw","wb");
    //fwrite(bitmapImage,bitmapInfoHeader->biSizeImage,1,filePtr1);
    //fclose(filePtr1);

    //close file and return bitmap image data
    fclose(filePtr);
    return bitmapImage;
}


