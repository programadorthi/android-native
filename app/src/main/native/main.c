#include <android/log.h>
#include <android_native_app_glue.h>

#include "lvgl.h"

#define LOG_TAG "NativeApplication"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

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

static void initLvgl(struct android_app *app) {
    lv_tick_set_cb(currentTimeInMillis);

    pthread_create(&lvglTickThread, 0, timerHandler, 0);
}