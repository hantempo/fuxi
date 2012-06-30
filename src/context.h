#ifndef _INCLUDE_CONTEXT_
#define _INCLUDE_CONTEXT_

#include <GLES2/gl2.h>
#include <EGL/egl.h>

class FBDevContext
{
public:
   FBDevContext(unsigned int width, unsigned int height);
   ~FBDevContext();
   bool swap_buffer();

private:
    EGLDisplay display;
    EGLContext context;
    EGLSurface surface;
};

#endif // _INCLUDE_CONTEXT_
