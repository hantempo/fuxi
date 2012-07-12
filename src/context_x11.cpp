#include "context.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>

static Display * xdisplay = NULL;
static int xscreen = 0;
static XVisualInfo *xvisinfo = NULL;
static Window xwindow;
    
void waitForEvent(int type)
{
    XEvent event;
    do {
        XWindowEvent(xdisplay, xwindow, StructureNotifyMask, &event);
    } while (event.type != type);
}

Context::Context(int w, int h)
: display(EGL_NO_DISPLAY), context(EGL_NO_CONTEXT),
  surface(EGL_NO_SURFACE),
  width(w), height(h)
{
    // Initialize display
    xdisplay = XOpenDisplay(NULL);
    FUXI_DEBUG_ASSERT(xdisplay, "Unable to open display");

    xscreen = DefaultScreen(xdisplay);

    display = EGL_CHECK(eglGetDisplay((EGLNativeDisplayType)xdisplay));
    FUXI_DEBUG_ASSERT(display != EGL_NO_DISPLAY, "Unable to get EGL display");

    EGLint major, minor;
    EGL_CHECK(eglInitialize(display, &major, &minor));

    // Get config
    EGLint config_num;
    EGL_CHECK(eglChooseConfig(display, config_attributes, &config, 1, &config_num));
    FUXI_DEBUG_ASSERT(config_num, "No EGL configurations were returned.");

    EGLint vid;
    EGL_CHECK(eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &vid));
    {
        XVisualInfo templ;
        int num_visuals;
        templ.visualid = vid;
        xvisinfo = XGetVisualInfo(xdisplay, VisualIDMask, &templ, &num_visuals);
    }

    // Create window surface
    {
        Window root = RootWindow(xdisplay, xscreen);

        /* window attributes */
        XSetWindowAttributes attr;
        attr.background_pixel = 0;
        attr.border_pixel = 0;
        attr.colormap = XCreateColormap(xdisplay, root, xvisinfo->visual, AllocNone);
        attr.event_mask = StructureNotifyMask;

        unsigned long mask;
        mask = CWBackPixel | CWBorderPixel | CWColormap | CWEventMask;

        int x = 0, y = 0;

        xwindow = XCreateWindow(
            xdisplay, root,
            x, y, width, height,
            0,
            xvisinfo->depth,
            InputOutput,
            xvisinfo->visual,
            mask,
            &attr);

        XSizeHints sizehints;
        sizehints.x = x;
        sizehints.y = y;
        sizehints.width  = width;
        sizehints.height = height;
        sizehints.flags = USSize | USPosition;
        XSetNormalHints(xdisplay, xwindow, &sizehints);

        const char *name = "fuxi";
        XSetStandardProperties(
            xdisplay, xwindow, name, name,
            None, (char **)NULL, 0, &sizehints);

        eglWaitNative(EGL_CORE_NATIVE_ENGINE);

        surface = EGL_CHECK(eglCreateWindowSurface(display, config, (EGLNativeWindowType)xwindow, NULL));
    }
    
    context = EGL_CHECK(eglCreateContext(display, config, EGL_NO_CONTEXT, context_attributes));
    FUXI_DEBUG_ASSERT(context != EGL_NO_CONTEXT, "Failed to create EGL context.");

    EGL_CHECK(eglMakeCurrent(display, surface, surface, context));

    // display window
    eglWaitClient();
    XMapWindow(xdisplay, xwindow);
    waitForEvent(MapNotify);
    eglWaitNative(EGL_CORE_NATIVE_ENGINE);
}

Context::~Context()
{
    XFree(xvisinfo);
        
    eglDestroySurface(display, surface);
    eglWaitClient();
    XDestroyWindow(xdisplay, xwindow);
    eglWaitNative(EGL_CORE_NATIVE_ENGINE);
    
    EGL_CHECK(eglDestroyContext(display, context));

    EGL_CHECK(eglTerminate(display));
    XCloseDisplay(xdisplay);
    xdisplay == NULL;
}

bool Context::swap_buffers()
{
    return eglSwapBuffers(display, surface);
}

void Context::resize(int w, int h)
{
    if (w == width && h == height) {
        return;
    }

    eglWaitClient();
    XResizeWindow(xdisplay, xwindow, w, h);
    eglWaitNative(EGL_CORE_NATIVE_ENGINE);
}
