/*
 * This proprietary software may be used only as
 * authorised by a licensing agreement from ARM Limited
 * (C) COPYRIGHT 2009 - 2010 ARM Limited
 * ALL RIGHTS RESERVED
 * The entire notice above must be reproduced on all authorised
 * copies and copies may only be made to the extent permitted
 * by a licensing agreement from ARM Limited.
 */

#include <unistd.h>
#include <string>
#include "common.h"
#include "shader.h"
//#include "window.h"
#include "matrix.h"

#include "GLES2/gl2.h"
#include "EGL/egl.h"

#include "objLoader/objLoader.h"

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

struct fbdev_window
{
    unsigned short width;
    unsigned short height;
};

int main(int argc, char **argv) {
	EGLDisplay	sEGLDisplay;
	EGLContext	sEGLContext;
	EGLSurface	sEGLSurface;
	
	/* EGL Configuration */
	
	EGLint aEGLAttributes[] = {
		EGL_RED_SIZE, 8,
		EGL_GREEN_SIZE, 8,
		EGL_BLUE_SIZE, 8,
		EGL_DEPTH_SIZE, 16,
		EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
		EGL_NONE
	};

	EGLint aEGLContextAttributes[] = {
		EGL_CONTEXT_CLIENT_VERSION, 2,
		EGL_NONE
	};
	
	EGLConfig	aEGLConfigs[1];
	EGLint		cEGLConfigs;

	const unsigned int WIDTH  = 1280;
	const unsigned int HEIGHT = 720;
	
	int iXangle = 0, iYangle = 0, iZangle = 0;
	
	float aLightPos[] = { 0.0f, 0.0f, -1.0f }; // Light is nearest camera.
	
	unsigned char *myPixels = (unsigned char*)calloc(1, 128*128*4); // Holds texture data.
	unsigned char *myPixels2 = (unsigned char*)calloc(1, 128*128*4); // Holds texture data.
	
	float aRotate[16], aModelView[16], aPerspective[16], aMVP[16];
	
	sEGLDisplay = EGL_CHECK(eglGetDisplay((EGLNativeDisplayType)EGL_DEFAULT_DISPLAY));	
	EGL_CHECK(eglInitialize(sEGLDisplay, NULL, NULL));
    EGL_CHECK(eglChooseConfig(sEGLDisplay, aEGLAttributes, aEGLConfigs, 1, &cEGLConfigs));
    if (cEGLConfigs == 0) {
        printf("No EGL configurations were returned.\n");
        exit(-1);
    }

    fbdev_window hWindow;
    hWindow.width = static_cast<unsigned short>(WIDTH);
    hWindow.height = static_cast<unsigned short>(HEIGHT);
	sEGLSurface = EGL_CHECK(eglCreateWindowSurface(sEGLDisplay, aEGLConfigs[0], (EGLNativeWindowType)&hWindow, NULL));
    if (sEGLSurface == EGL_NO_SURFACE) {
        printf("Failed to create EGL surface.\n");
		exit(-1);
    }

    sEGLContext = EGL_CHECK(eglCreateContext(sEGLDisplay, aEGLConfigs[0], EGL_NO_CONTEXT, aEGLContextAttributes));
    if (sEGLContext == EGL_NO_CONTEXT) {
        printf("Failed to create EGL context.\n");
        exit(-1);
    }

    GLuint error;
    
	EGL_CHECK(eglMakeCurrent(sEGLDisplay, sEGLSurface, sEGLSurface, sEGLContext));

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
    objLoader *objData = new objLoader();
    FUXI_DEBUG_ASSERT(objData->load(obj_filepath.c_str()), "Failed to load obj file");

#ifndef NDEBUG
    printf("Loaded OBJ file: %s\n", obj_filepath.c_str());
    printf("Number of vertices: %i\n", objData->vertexCount);
    printf("Number of vertex normals: %i\n", objData->normalCount);
    printf("Number of texture coordinates: %i\n", objData->textureCount);

    printf("Number of faces: %i\n", objData->faceCount);
#endif

    // Load vertex attributes into VBO
    FUXI_DEBUG_ASSERT(objData->vertexCount, "No vertex position data.");
    float *position = new float[objData->vertexCount * 3];
    for (int i = 0; i < objData->vertexCount; ++i)
    {
        const obj_vector *pos = objData->vertexList[i];
        memcpy(position + i * 3, pos->e, sizeof(float) * 3);
    }
    unsigned short *index = new unsigned short[objData->faceCount * 3];
    for (int i = 0; i < objData->faceCount; ++i)
    {
        obj_face *f = objData->faceList[i];
        FUXI_DEBUG_ASSERT(f->vertex_count == 3, "Only accept triangle face");
        const unsigned short vi0 = static_cast<unsigned short>(f->vertex_index[0]);
        const unsigned short vi1 = static_cast<unsigned short>(f->vertex_index[1]);
        const unsigned short vi2 = static_cast<unsigned short>(f->vertex_index[2]);
        index[i * 3 + 0] = vi0;
        index[i * 3 + 1] = vi1;
        index[i * 3 + 2] = vi2;
    }

    /* Populate attributes for position, colour and texture coordinates etc. */
    GL_CHECK(glVertexAttribPointer(iLocPosition, 3, GL_FLOAT, GL_FALSE, 0, position));
    
    GL_CHECK(glEnable(GL_CULL_FACE));
    GL_CHECK(glEnable(GL_DEPTH_TEST));
    
	/* Enter event loop */
    int count = 0;
	while (count < 200) {
		/* 
		 * Do some rotation with Euler angles. It is not a fixed axis as
         * quaterions would be, but the effect is cool. 
		 */
        rotate_matrix(iXangle, 1.0, 0.0, 0.0, aModelView);
        rotate_matrix(iYangle, 0.0, 1.0, 0.0, aRotate);
        multiply_matrix(aRotate, aModelView, aModelView);
        rotate_matrix(iZangle, 0.0, 1.0, 0.0, aRotate);
        multiply_matrix(aRotate, aModelView, aModelView);

		/* Pull the camera back from the cube */
        aModelView[14] -= 15;
        aModelView[13] -= 5;

        perspective_matrix(45.0, (double)WIDTH/(double)HEIGHT, 0.01, 100.0, aPerspective);
        multiply_matrix(aPerspective, aModelView, aMVP);

        //iXangle += 3;
        iYangle += 2;
        //iZangle += 1;

        if(iXangle >= 360) iXangle -= 360;
        if(iXangle < 0) iXangle += 360;
        if(iYangle >= 360) iYangle -= 360;
        if(iYangle < 0) iYangle += 360;
        if(iZangle >= 360) iZangle -= 360;
        if(iZangle < 0) iZangle += 360;

        ///////////////////////////////////////////////////////////////////
        glViewport(0, 0, WIDTH, HEIGHT);
        GL_CHECK(glClearColor(1.0f,1.0f,1.0f,1.0f));
        GL_CHECK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));
        
        GL_CHECK(glUniformMatrix4fv(iLocMVP, 1, GL_FALSE, aMVP));
        GL_CHECK(glDrawElements(GL_TRIANGLES, objData->faceCount * 3, GL_UNSIGNED_SHORT, index));

        if (!eglSwapBuffers(sEGLDisplay, sEGLSurface)) 
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
    
    /* EGL clean up */
    EGL_CHECK(eglDestroySurface(sEGLDisplay, sEGLSurface));
    EGL_CHECK(eglDestroyContext(sEGLDisplay, sEGLContext));
    EGL_CHECK(eglTerminate(sEGLDisplay));

    delete objData;
	
	return 0;
}
