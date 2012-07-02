#ifndef _INCLUDE_CONTEXT_
#define _INCLUDE_CONTEXT_

#include <GLES2/gl2.h>
#include <EGL/egl.h>

#include "common.h"

class Context
{
public:
    Context(int width, int height);
    ~Context();

    void resize(int width, int height);
    bool swap_buffers();
    bool process_events();

private:
    static const EGLint config_attributes[];
    static const EGLint context_attributes[];

    EGLDisplay display;
    EGLContext context;
    EGLSurface surface;
    EGLConfig config;
    int width, height;
};

#endif // _INCLUDE_CONTEXT_
