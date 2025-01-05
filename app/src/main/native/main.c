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

static void render();

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
            // TODO: screen rotation font size not good
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

static void pixelFlush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map) {
    uint32_t areaWidth = lv_area_get_width(area);
    uint32_t areaHeight = lv_area_get_height(area);

    int32_t left = LV_MAX(area->x1, 0);
    int32_t top = LV_MAX(area->y1, 0);
    int32_t right = LV_MIN(area->x2 + 1, areaWidth);
    int32_t bottom = LV_MIN(area->y2 + 1, areaHeight);

    ANativeWindow_Buffer buffer;
    ANativeWindow_lock(nativeWindow, &buffer, 0);

    int32_t width = LV_MIN(buffer.width, (right - left));
    int32_t height = LV_MIN(buffer.height, (bottom - top));

    uint32_t *out = (uint32_t *) buffer.bits;

    for (int32_t y = 0; y < height; y++) {
        for (int32_t x = 0; x < width; x++) {
            uint8_t b = *(px_map++); // px_map[0] has blue color
            uint8_t g = *(px_map++); // px_map[0] has green color
            uint8_t r = *(px_map++); // px_map[0] has red color
            uint8_t a = *(px_map++); // px_map[0] has alpha opacity

            uint32_t pixel = (a << 24) | (b << 16) | (g << 8) | r;
            out[x] = pixel;
        }
        out += buffer.stride;
    }

    ANativeWindow_unlockAndPost(nativeWindow);
    lv_display_flush_ready(disp);
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

    ANativeWindow_setBuffersGeometry(nativeWindow, windowWidth, windowHeight,
                                     WINDOW_FORMAT_RGBA_8888);

    ANativeWindow_Buffer buffer;
    ANativeWindow_lock(nativeWindow, &buffer, 0);
    lv_memset(buffer.bits, 0xFF, buffer.stride * buffer.height * 4);
    ANativeWindow_unlockAndPost(nativeWindow);

    lv_theme_t *theme = lv_theme_default_init(
            display,
            lv_palette_main(LV_PALETTE_BLUE),
            lv_palette_main(LV_PALETTE_CYAN),
            false,
            LV_FONT_DEFAULT
    );
    lv_display_set_theme(display, theme);

    render();

    pthread_create(&lvglTickThread, 0, timerHandler, 0);
}

static void render() {
    /*Change the active screen's background color*/
    lv_obj_set_style_bg_color(lv_screen_active(), lv_color_hex(0x003a57), LV_PART_MAIN);

    /*Create a white label, set its text and align it to the center*/
    lv_obj_t *label = lv_label_create(lv_screen_active());

    // TODO: label circular animation is crashing after a while
    lv_label_set_long_mode(label, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_set_width(label, ANativeWindow_getWidth(nativeWindow));
    lv_label_set_text(label, "Lorem Ipsum is simply dummy text of the printing and typesetting industry");
    lv_obj_set_style_text_color(lv_screen_active(), lv_color_hex(0xffffff), LV_PART_MAIN);
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
}