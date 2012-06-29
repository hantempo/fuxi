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
    
    float aLightPos[] = { 0.0f, 0.0f, -1.0f }; // Light is nearest camera.
    
    unsigned char *myPixels = (unsigned char*)calloc(1, 128*128*4); // Holds texture data.
    unsigned char *myPixels2 = (unsigned char*)calloc(1, 128*128*4); // Holds texture data.
    
    const char* data_path_ptr = getenv("FUXI_DATA_PATH");
    FUXI_DEBUG_ASSERT_POINTER(data_path_ptr);
    const std::string data_path(data_path_ptr);

    GLuint fragShader, vertShader;
    process_shader(&vertShader, (data_path + "/shader/shader.vert").c_str(), GL_VERTEX_SHADER);
    process_shader(&fragShader, (data_path + "/shader/shader.frag").c_str(), GL_FRAGMENT_SHADER);

    GLuint program = GL_CHECK(glCreateProgram());
    GL_CHECK(glAttachShader(program, vertShader));
    GL_CHECK(glAttachShader(program, fragShader));
    GL_CHECK(glLinkProgram(program));

    /* Get attribute locations of non-fixed attributes like colour and texture coordinates. */
    GLint locPosition = GL_CHECK(glGetAttribLocation(program, "in_position"));
    GLint locNormal = GL_CHECK(glGetAttribLocation(program, "in_normal"));

    /* Get uniform locations */
    GLint locMVP = GL_CHECK(glGetUniformLocation(program, "mvp"));
    GLint locMV = GL_CHECK(glGetUniformLocation(program, "mv"));
    GLint locInvModel = GL_CHECK(glGetUniformLocation(program, "inv_model"));
    GLint locLightPos = GL_CHECK(glGetUniformLocation(program, "light_pos"));
    Vector3 light_pos(0.f, 0.f, 5.f);
    GL_CHECK(glUseProgram(program));
    
    GL_CHECK(glUniform3fv(locLightPos, 1, light_pos));

    // Load the model from obj file
    const char * obj_filename = argv[1];
    const std::string obj_filepath = data_path + "/model/" + obj_filename;
    const Geometry geometry(obj_filepath.c_str());
    FUXI_DEBUG_ASSERT(geometry.triangle_count(), "No faces in model.");

    geometry.enable_position_attribute(locPosition);
    geometry.enable_normal_attribute(locNormal);

    const Vector4 clear_color = Color::Black;
    GL_CHECK(glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w));
    GL_CHECK(glEnable(GL_CULL_FACE));
    GL_CHECK(glEnable(GL_DEPTH_TEST));
    
    /* Enter event loop */
    int iXangle = 0, iYangle = 0;
    int count = 0;
    while (count < 360) {

        const Matrix4x4 scale = Matrix4x4::Scale(Vector3(1, 1, 1));
        const Matrix4x4 rotateX = Matrix4x4::Rotate(Vector3(1, 0, 0), iXangle);
        const Matrix4x4 rotateY = Matrix4x4::Rotate(Vector3(0, 1, 0), iYangle);
        const Matrix4x4 translate = Matrix4x4::Translate(Vector3(0, 0, -3));
        const Matrix4x4 pers = Matrix4x4::Perspective(60.0f, (float)WIDTH/HEIGHT, 0.01, 100.0);
        const Matrix4x4 mv = scale * rotateX * rotateY * translate;
        const Matrix4x4 inv_model = Matrix4x4::Transpose(Matrix4x4::Invert4x3(mv));
        const Matrix4x4 mvp = mv * pers;
        GL_CHECK(glUniformMatrix4fv(locMVP, 1, GL_FALSE, mvp));
        GL_CHECK(glUniformMatrix4fv(locMV, 1, GL_FALSE, mv));
        GL_CHECK(glUniformMatrix4fv(locInvModel, 1, GL_FALSE, inv_model));

        if (count < 180)
        {
            iYangle += 2;
            if(iYangle >= 360) iYangle -= 360;
            if(iYangle < 0) iYangle += 360;
        }
        else
        {
            iXangle += 2;
            if(iXangle >= 360) iXangle -= 360;
            if(iXangle < 0) iXangle += 360;
        }

        ///////////////////////////////////////////////////////////////////
        glViewport(0, 0, WIDTH, HEIGHT);
        GL_CHECK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));
        
        geometry.draw();

        if (!context.swap_buffer()) 
        {
            printf("Failed to swap buffers.\n");
        }

        count++;
        usleep(20000);
    }

    GL_CHECK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));
    
    /* Cleanup shaders */
    GL_CHECK(glUseProgram(0));
    GL_CHECK(glDeleteShader(vertShader));
    GL_CHECK(glDeleteShader(fragShader));
    GL_CHECK(glDeleteProgram(program));
    
    return 0;
}
