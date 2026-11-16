#include <android_native_app_glue.h>
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <cstdint>
struct Context {
    EGLDisplay display = EGL_NO_DISPLAY;
    EGLSurface surface = EGL_NO_SURFACE;
    EGLContext context = EGL_NO_CONTEXT;
    uint64_t seed = 0;
    int32_t width = 0;
    int32_t height = 0;
    int32_t half = 0;
    uint32_t target = 0;
};
__attribute__((always_inline)) inline uint32_t generate_target(uint64_t &state) {
    state += 0xa0761d6478bd642f;
    unsigned __int128 product = state;
    product *= (state ^ 0xe7037ed1a0b428db);
    return ((product >> 64) ^ product) & 1;
}
static void clear_graphics(Context &context) {
    if (context.display == EGL_NO_DISPLAY) {
        return;
    }
    eglMakeCurrent(context.display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    if (context.context != EGL_NO_CONTEXT) {
        eglDestroyContext(context.display, context.context);
        context.context = EGL_NO_CONTEXT;
    }
    if (context.surface != EGL_NO_SURFACE) {
        eglDestroySurface(context.display, context.surface);
        context.surface = EGL_NO_SURFACE;
    }
    eglTerminate(context.display);
    context.display = EGL_NO_DISPLAY;
}
static void shut_down(struct android_app *application, Context &context) {
    clear_graphics(context);
    ANativeActivity_finish(application->activity);
}
__attribute__((always_inline)) inline void draw_frame(Context &context) {
    glScissor(0, 0, context.width, context.height);
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    glScissor(static_cast<int32_t>(context.target) * context.half, 0, context.target ? (context.width - context.half) : context.half, context.height);
    glClearColor(1, 1, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    eglSwapBuffers(context.display, context.surface);
}
static int32_t handle_input(struct android_app *application, AInputEvent *event) {
    if (AInputEvent_getType(event) != AINPUT_EVENT_TYPE_MOTION) {
        return 0;
    }
    const int32_t action = AMotionEvent_getAction(event);
    const int32_t phase = action & AMOTION_EVENT_ACTION_MASK;
    if (phase != AMOTION_EVENT_ACTION_UP && phase != AMOTION_EVENT_ACTION_POINTER_UP) {
        return 1;
    }
    auto *context = static_cast<Context *>(application->userData);
    if (context->display == EGL_NO_DISPLAY) {
        return 1;
    }
    if ((static_cast<int32_t>(AMotionEvent_getX(event, (phase == AMOTION_EVENT_ACTION_POINTER_UP) ? ((action & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT) : 0)) >= context->half) != context->target) {
        shut_down(application, *context);
        return 1;
    }
    context->target = generate_target(context->seed);
    draw_frame(*context);
    return 1;
}
static void handle_command(struct android_app *application, int32_t command) {
    auto *context = static_cast<Context *>(application->userData);
    switch (command) {
        case APP_CMD_INIT_WINDOW: {
            if (!application->window || context->display != EGL_NO_DISPLAY) {
                break;
            }
            static constexpr EGLint configuration_attributes[] = {
                    EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
                    EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
                    EGL_RED_SIZE, 8,
                    EGL_GREEN_SIZE, 8,
                    EGL_BLUE_SIZE, 8,
                    EGL_ALPHA_SIZE, 8,
                    EGL_NONE
            };
            static constexpr EGLint context_attributes[] = {
                    EGL_CONTEXT_CLIENT_VERSION, 3,
                    EGL_NONE
            };
            context->display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
            EGLConfig configuration;
            EGLint count;
            if (context->display == EGL_NO_DISPLAY || !eglInitialize(context->display, nullptr, nullptr) || !eglChooseConfig(context->display, configuration_attributes, &configuration, 1, &count) || count <= 0 || (context->surface = eglCreateWindowSurface(context->display, configuration, application->window, nullptr)) == EGL_NO_SURFACE || (context->context = eglCreateContext(context->display, configuration, nullptr, context_attributes)) == EGL_NO_CONTEXT || !eglMakeCurrent(context->display, context->surface, context->surface, context->context)) {
                shut_down(application, *context);
                break;
            }
            eglSwapInterval(context->display, 0);
            glEnable(GL_SCISSOR_TEST);
            [[fallthrough]];
        }
        case APP_CMD_WINDOW_REDRAW_NEEDED:
        case APP_CMD_WINDOW_RESIZED:
        case APP_CMD_CONFIG_CHANGED:
            if (context->display != EGL_NO_DISPLAY) {
                eglQuerySurface(context->display, context->surface, EGL_WIDTH, &context->width);
                eglQuerySurface(context->display, context->surface, EGL_HEIGHT, &context->height);
                context->half = context->width / 2;
                draw_frame(*context);
            }
            break;
        case APP_CMD_TERM_WINDOW:
            clear_graphics(*context);
            break;
        default:
            break;
    }
}
[[maybe_unused]] void android_main(struct android_app *application) {
    Context context;
    context.seed = __builtin_readcyclecounter();
    context.target = generate_target(context.seed);
    application->userData = &context;
    application->onAppCmd = handle_command;
    application->onInputEvent = handle_input;
    while (!application->destroyRequested) {
        int count;
        struct android_poll_source *source;
        if (ALooper_pollOnce(-1, nullptr, &count, (void **) &source) >= 0 && source != nullptr) {
            source->process(application, source);
        }
    }
    clear_graphics(context);
}
