/*
 * This proprietary software may be used only as
 * authorised by a licensing agreement from ARM Limited
 * (C) COPYRIGHT 2009 - 2010 ARM Limited
 * ALL RIGHTS RESERVED
 * The entire notice above must be reproduced on all authorised
 * copies and copies may only be made to the extent permitted
 * by a licensing agreement from ARM Limited.
 */

#include "common.h"
#include "shader.h"
#include "math3d.h"
#include "geometry.h"

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

int main(int argc, char **argv) {

    const unsigned int WIDTH  = 1280;
    const unsigned int HEIGHT = 720;
    FBDevContext context(WIDTH, HEIGHT);
    
    int iYangle = 0;
    
    float aLightPos[] = { 0.0f, 0.0f, -1.0f }; // Light is nearest camera.
    
    unsigned char *myPixels = (unsigned char*)calloc(1, 128*128*4); // Holds texture data.
    unsigned char *myPixels2 = (unsigned char*)calloc(1, 128*128*4); // Holds texture data.
    
    const char* data_path_ptr = getenv("FUXI_DATA_PATH");
    FUXI_DEBUG_ASSERT_POINTER(data_path_ptr);
    const std::string data_path(data_path_ptr);

    GLuint uiProgram, uiFragShader, uiVertShader;
    process_shader(&uiVertShader, (data_path + "/shader/shader.vert").c_str(), GL_VERTEX_SHADER);
    process_shader(&uiFragShader, (data_path + "/shader/shader.frag").c_str(), GL_FRAGMENT_SHADER);

    uiProgram = GL_CHECK(glCreateProgram());
    GL_CHECK(glAttachShader(uiProgram, uiVertShader));
    GL_CHECK(glAttachShader(uiProgram, uiFragShader));
    GL_CHECK(glLinkProgram(uiProgram));

    /* Get attribute locations of non-fixed attributes like colour and texture coordinates. */
    GLint iLocPosition = GL_CHECK(glGetAttribLocation(uiProgram, "in_position"));

    /* Get uniform locations */
    GLint iLocMVP = GL_CHECK(glGetUniformLocation(uiProgram, "mvp"));

    GL_CHECK(glUseProgram(uiProgram));

    /* Enable attributes for position, colour and texture coordinates etc. */
    GL_CHECK(glEnableVertexAttribArray(iLocPosition));

    // Load the model from obj file
    const char * obj_filename = argv[1];
    const std::string obj_filepath = data_path + "/model/" + obj_filename;
    const Geometry geometry(obj_filepath.c_str());
    FUXI_DEBUG_ASSERT(geometry.triangle_count(), "No faces in model.");

    geometry.enable_position_attribute(iLocPosition);
    
    GL_CHECK(glEnable(GL_CULL_FACE));
    GL_CHECK(glEnable(GL_DEPTH_TEST));
    
    /* Enter event loop */
    int count = 0;
    while (count < 200) {

        Matrix4x4 rotateY = Matrix4x4::Rotate(Vector3(0, 1, 0), iYangle);
        Matrix4x4 translate = Matrix4x4::Translate(Vector3(0, -5, -15));
        Matrix4x4 pers = Matrix4x4::Perspective(60.0f, (float)WIDTH/HEIGHT, 0.01, 100.0);
        Matrix4x4 mvp = rotateY * translate * pers;
        GL_CHECK(glUniformMatrix4fv(iLocMVP, 1, GL_FALSE, mvp));

        iYangle += 2;
        if(iYangle >= 360) iYangle -= 360;
        if(iYangle < 0) iYangle += 360;

        ///////////////////////////////////////////////////////////////////
        glViewport(0, 0, WIDTH, HEIGHT);
        GL_CHECK(glClearColor(1.0f,1.0f,1.0f,1.0f));
        GL_CHECK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));
        
        geometry.draw();

        if (!context.swap_buffer()) 
        {
            printf("Failed to swap buffers.\n");
        }

        count++;
        usleep(20000);
    }
    
    /* Cleanup shaders */
    GL_CHECK(glUseProgram(0));
    GL_CHECK(glDeleteShader(uiVertShader));
    GL_CHECK(glDeleteShader(uiFragShader));
    GL_CHECK(glDeleteProgram(uiProgram));
    
    return 0;
}
