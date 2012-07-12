#include "context.h"

struct fbdev_window
{
    unsigned short width;
    unsigned short height;
};

Context::Context(int w, int h)
: display(EGL_NO_DISPLAY), context(EGL_NO_CONTEXT),
  surface(EGL_NO_SURFACE),
  width(w), height(h)
{
    // Initialize display
    display = EGL_CHECK(eglGetDisplay((EGLNativeDisplayType)EGL_DEFAULT_DISPLAY));
    FUXI_DEBUG_ASSERT(display != EGL_NO_DISPLAY, "Unable to get EGL display");
    
    EGLint major, minor;
    EGL_CHECK(eglInitialize(display, &major, &minor));

    // Get config
    EGLint config_num;
    EGL_CHECK(eglChooseConfig(display, config_attributes, &config, 1, &config_num));
    FUXI_DEBUG_ASSERT(config_num, "No EGL configurations were returned.");

    fbdev_window window;
    window.width = static_cast<unsigned short>(width);
    window.height = static_cast<unsigned short>(height);
    surface = EGL_CHECK(eglCreateWindowSurface(display, config, (EGLNativeWindowType)&window, NULL));
    FUXI_DEBUG_ASSERT(surface != EGL_NO_SURFACE, "Failed to create EGL surface.");

    context = EGL_CHECK(eglCreateContext(display, config, EGL_NO_CONTEXT, context_attributes));
    FUXI_DEBUG_ASSERT(context != EGL_NO_CONTEXT, "Failed to create EGL context.");

    EGL_CHECK(eglMakeCurrent(display, surface, surface, context));
}

Context::~Context()
{
    EGL_CHECK(eglDestroySurface(display, surface));
    EGL_CHECK(eglDestroyContext(display, context));
    EGL_CHECK(eglTerminate(display));
}

bool Context::process_events(void)
{
    return true;
}

bool Context::swap_buffers()
{
    return EGL_CHECK(eglSwapBuffers(display, surface));
}

void Context::resize(int w, int h)
{
    if (w == width && h == height) {
        return;
    }

    EGLContext current_context = EGL_CHECK(eglGetCurrentContext());
    EGL_CHECK(eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT));
    EGL_CHECK(eglDestroySurface(display, surface));

    fbdev_window window;
    window.width = static_cast<unsigned short>(width);
    window.height = static_cast<unsigned short>(height);
    surface = EGL_CHECK(eglCreateWindowSurface(display, config,
                                     (EGLNativeWindowType)&window, NULL));
    EGL_CHECK(eglMakeCurrent(display, surface, surface, current_context));
}

