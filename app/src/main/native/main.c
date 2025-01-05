#include <android/log.h>
#include <android_native_app_glue.h>

#include "lvgl.h"

#define LOG_TAG "NativeApplication"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

#define BYTE_PER_PIXEL (LV_COLOR_FORMAT_GET_SIZE(LV_COLOR_FORMAT_ARGB8888))

static ANativeWindow *nativeWindow;
static pthread_t lvglTickThread;
static bool ready = false;

static void handleCmd(struct android_app *app, int32_t cmd);

static void initLvgl(struct android_app *app);

__attribute__((unused))
void android_main(struct android_app *app) {
    lv_init();

    app->onAppCmd = handleCmd;

    int *events = NULL;
    struct android_poll_source *source;
    while (!app->destroyRequested) {
        int timeout = ready ? 0 : -1;
        int result = ALooper_pollOnce(timeout, NULL, events, (void **) &source);
        if (result == ALOOPER_POLL_ERROR) {
            LOGE("ALooper_pollOnce returned error %d\n", result);
            break;
        }
        if (source) {
            source->process(app, source);
        }
    }

    lv_deinit();
}

static void handleCmd(struct android_app *app, int32_t cmd) {
    switch (cmd) {
        case APP_CMD_INIT_WINDOW:
            ready = true;
            initLvgl(app);
            break;
        case APP_CMD_TERM_WINDOW:
            ready = false;
            break;
        default:
            break;

    }
}

static uint32_t currentTimeInMillis(void) {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    uint64_t timeMs = t.tv_sec * 1000 + (t.tv_nsec / 1000000);
    return timeMs;
}

static void *timerHandler(__attribute__((unused)) void *data) {
    while (true) {
        if (!ready) {
            break;
        }
        uint32_t timeUntilNext = lv_timer_handler();
        lv_delay_ms(timeUntilNext);
    }
    return 0;
}

static void pixelFlush(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map) {
    // map to window buffer
}

static void initLvgl(struct android_app *app) {
    nativeWindow = app->window;

    lv_tick_set_cb(currentTimeInMillis);

    AConfiguration *config = AConfiguration_new();
    AConfiguration_fromAssetManager(config, app->activity->assetManager);
    int32_t dpi = AConfiguration_getDensity(config);
    AConfiguration_delete(config);

    int32_t windowWidth = ANativeWindow_getWidth(nativeWindow);
    int32_t windowHeight = ANativeWindow_getHeight(nativeWindow);

    lv_display_t *display = lv_display_create(windowWidth, windowHeight);
    lv_display_set_dpi(display, dpi);
    lv_display_set_flush_cb(display, pixelFlush);

    uint32_t buf1[windowWidth * windowHeight * BYTE_PER_PIXEL];
    uint32_t buf2[windowWidth * windowHeight * BYTE_PER_PIXEL];
    lv_display_set_buffers(display, buf1, buf2, sizeof(buf1), LV_DISPLAY_RENDER_MODE_FULL);

    ANativeWindow_setBuffersGeometry(nativeWindow, windowWidth, windowHeight, WINDOW_FORMAT_RGBA_8888);

    ANativeWindow_Buffer buffer;
    ANativeWindow_lock(nativeWindow, &buffer, 0);
    lv_memset(buffer.bits, 0xFF, buffer.stride * buffer.height * 4);
    ANativeWindow_unlockAndPost(nativeWindow);

    pthread_create(&lvglTickThread, 0, timerHandler, 0);
}