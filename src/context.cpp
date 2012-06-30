#include "context.h"

struct fbdev_window
{
    unsigned short width;
    unsigned short height;
};

FBDevContext::FBDevContext(unsigned int width, unsigned int height)
: display(0), context(0), surface(0)
{
    const EGLint attributes[] = {
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_DEPTH_SIZE, 16,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL_NONE
    };

    const EGLint context_attributes[] = {
        EGL_CONTEXT_CLIENT_VERSION, 2,
        EGL_NONE
    };
    
    display = EGL_CHECK(eglGetDisplay((EGLNativeDisplayType)EGL_DEFAULT_DISPLAY));    
    EGL_CHECK(eglInitialize(display, NULL, NULL));

    EGLConfig configs[1];
    EGLint config_num;
    EGL_CHECK(eglChooseConfig(display, attributes, configs, 1, &config_num));
    FUXI_DEBUG_ASSERT(config_num, "No EGL configurations were returned.");

    fbdev_window window;
    window.width = static_cast<unsigned short>(width);
    window.height = static_cast<unsigned short>(height);
    surface = EGL_CHECK(eglCreateWindowSurface(display, configs[0], (EGLNativeWindowType)&window, NULL));
    FUXI_DEBUG_ASSERT(surface != EGL_NO_SURFACE, "Failed to create EGL surface.");

    context = EGL_CHECK(eglCreateContext(display, configs[0], EGL_NO_CONTEXT, context_attributes));
    FUXI_DEBUG_ASSERT(context != EGL_NO_CONTEXT, "Failed to create EGL context.");

    EGL_CHECK(eglMakeCurrent(display, surface, surface, context));
}

FBDevContext::~FBDevContext()
{
    EGL_CHECK(eglDestroySurface(display, surface));
    EGL_CHECK(eglDestroyContext(display, context));
    EGL_CHECK(eglTerminate(display));
}

bool FBDevContext::swap_buffer()
{
    return eglSwapBuffers(display, surface);
}
