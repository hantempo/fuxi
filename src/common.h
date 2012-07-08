#ifndef _COMMON_H_
#define _COMMON_H_

#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <string>

typedef unsigned char       UInt8;
typedef char                SInt8;
typedef unsigned short      UInt16;
typedef short               SInt16;
typedef unsigned int        UInt32;
typedef int                 SInt32;

typedef float               Float32;
typedef double              Float64;

#define FUXI_PRINTF printf
#define FUXI_ASSERT_QUIT exit(1)

#define FUXI_DEBUG_TRACE \
    FUXI_PRINTF( "In file: "__FILE__ "  line:%4d\n" , __LINE__)

#define FUXI_DEBUG_ERROR_START printf("****************************************\n")
#define FUXI_DEBUG_ERROR_END printf("\n")

#define FUXI_DEBUG_ASSERT(expr, msg) \
    if (!(expr)) \
    { \
        FUXI_DEBUG_ERROR_START; \
        FUXI_PRINTF("ASSERT EXIT: "); \
        FUXI_DEBUG_TRACE; \
        FUXI_PRINTF(msg); \
        FUXI_DEBUG_ERROR_END; \
        FUXI_ASSERT_QUIT; \
    }

#define FUXI_DEBUG_ASSERT_POINTER(pointer) FUXI_DEBUG_ASSERT(pointer, ("Null pointer " #pointer) )

#define GL_CHECK(x) \
        x; \
        { \
          GLenum glError = glGetError(); \
          if(glError != GL_NO_ERROR) { \
            fprintf(stderr, "glGetError() = %i (0x%.8x) at line %i of %s\n", glError, glError, __LINE__, __FILE__); \
            exit(1); \
          } \
        }

#define EGL_CHECK(x) \
    x; \
    { \
        EGLint eglError = eglGetError(); \
        if(eglError != EGL_SUCCESS) { \
            fprintf(stderr, "eglGetError() = %i (0x%.8x) at line %i\n of %s", eglError, eglError, __LINE__, __FILE__); \
            exit(1); \
        } \
    }

#endif // _COMMON_H_
