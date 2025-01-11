/**
 * @file main.c
 *
 */
#define CLAY_IMPLEMENTATION
/*********************
 *      INCLUDES
 *********************/

#include <android/log.h>
#include <android_native_app_glue.h>

#include "lvgl.h"
#include "types.h"
#include "render.h"

/*********************
 *      DEFINES
 *********************/
#define LOG_TAG "NativeApplication"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   STATIC FUNCTIONS
 **********************/

static uint32_t currentTimeInMillis(void) {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    uint64_t timeMs = t.tv_sec * 1000 + (t.tv_nsec / 1000000);
    return timeMs;
}

static void *refresh_routine(void *data) {
    struct app_data_t *app_data = data;
    LV_ASSERT_NULL(app_data)

    while (app_data->running) {
        render();
        uint32_t timeUntilNext = lv_timer_handler();
        lv_delay_ms(timeUntilNext);
    }

    return NULL;
}

static void flush_cb(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map) {
    struct app_data_t *data = lv_display_get_driver_data(disp);
    LV_ASSERT_NULL(data)

    uint32_t areaWidth = lv_area_get_width(area);
    uint32_t areaHeight = lv_area_get_height(area);

    int32_t left = LV_MAX(area->x1, 0);
    int32_t top = LV_MAX(area->y1, 0);
    int32_t right = LV_MIN(area->x2 + 1, areaWidth);
    int32_t bottom = LV_MIN(area->y2 + 1, areaHeight);

    ANativeWindow_Buffer buffer;
    ANativeWindow_lock(data->window, &buffer, 0);

    int32_t width = LV_MIN(buffer.width, (right - left));
    int32_t height = LV_MIN(buffer.height, (bottom - top));

    uint32_t *out = (uint32_t *) buffer.bits;

    // TODO: Are there a faster way to copy pixels? I don't know yet.
    for (int32_t y = 0; y < height; y++) {
        for (int32_t x = 0; x < width; x++) {
            uint8_t b = *(px_map++); // px_map[0] has blue color
            uint8_t g = *(px_map++); // px_map[1] has green color
            uint8_t r = *(px_map++); // px_map[2] has red color
            uint8_t a = *(px_map++); // px_map[3] has alpha opacity

            uint32_t pixel = (a << 24) | (b << 16) | (g << 8) | r;
            out[x] = pixel;
        }
        out += buffer.stride;
    }

    ANativeWindow_unlockAndPost(data->window);
    lv_display_flush_ready(disp);
}

static void input_read_cb(lv_indev_t *indev, lv_indev_data_t *data) {
    struct app_data_t *appData = lv_indev_get_driver_data(indev);
    LV_ASSERT_NULL(appData)

    data->point.x = appData->touchData.x;
    data->point.y = appData->touchData.y;

    data->state = appData->touchData.pressed ? LV_INDEV_STATE_PRESSED : LV_INDEV_STATE_RELEASED;
}

static void destroy_display(struct app_data_t *data, int32_t format) {
    data->running = false;
    void *ret = NULL;
    pthread_join(data->tickThread, &ret);
    ANativeWindow_setBuffersGeometry(
            data->window,
            ANativeWindow_getWidth(data->window),
            ANativeWindow_getHeight(data->window),
            format
    );
    lv_deinit();
}

static void create_input(struct app_data_t *data) {
    lv_indev_t *input = lv_indev_create();
    lv_indev_set_driver_data(input, data);
    lv_indev_set_type(input, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(input, input_read_cb);
}

static void setup_window(struct app_data_t *data, struct android_app *app) {
    AConfiguration *config = AConfiguration_new();
    AConfiguration_fromAssetManager(config, app->activity->assetManager);
    int32_t dpi = AConfiguration_getDensity(config);
    AConfiguration_delete(config);

    ANativeWindow_setBuffersGeometry(
            data->window,
            ANativeWindow_getWidth(data->window),
            ANativeWindow_getHeight(data->window),
            WINDOW_FORMAT_RGBA_8888
    );

    lv_display_set_dpi(lv_display_get_default(), dpi);
}

static void create_display(struct android_app *app) {
    struct app_data_t *data = (struct app_data_t *) app->userData;
    LV_ASSERT_NULL(data)

    int32_t hor_res = ANativeWindow_getWidth(app->window);
    int32_t ver_res = ANativeWindow_getHeight(app->window);

    data->window = app->window;

    lv_init();
    lv_tick_set_cb(currentTimeInMillis);

    //lv_log_register_print_cb(lvgl_log_cb);

    lv_display_t *display = lv_display_create(hor_res, ver_res);
    lv_display_set_driver_data(display, data);
    lv_display_set_flush_cb(display, flush_cb);

    setup_window(data, app);

    int32_t buf_size = hor_res * ver_res * (LV_COLOR_DEPTH + 7) / 8;
    data->buffer[0] = lv_malloc(buf_size);
    data->buffer[1] = lv_malloc(buf_size);
    lv_display_set_buffers(
            display,
            data->buffer[0],
            data->buffer[1],
            buf_size,
            LV_DISPLAY_RENDER_MODE_FULL
    );

    lv_theme_t *theme = lv_theme_default_init(
            display,
            lv_palette_main(LV_PALETTE_BLUE),
            lv_palette_main(LV_PALETTE_CYAN),
            false,
            LV_FONT_DEFAULT
    );
    lv_display_set_theme(display, theme);

    data->running = true;
    pthread_create(&data->tickThread, NULL, refresh_routine, data);
}

/*static void lvgl_log_cb(lv_log_level_t level, const char *buf) {
    if (level == LV_LOG_LEVEL_ERROR) {
        LOGE(">> LVGL: %s", buf);
    } else {
        LOGI(">> LVGL: %s", buf);
    }
}*/

static void clay_error_handler(Clay_ErrorData errorText) {
    LOGE(">> CLAY: %s", errorText.errorText.chars);
}

static inline Clay_Dimensions clay_measure_text(Clay_String *text, Clay_TextElementConfig *config) {
    // Clay_TextElementConfig contains members such as fontId, fontSize, letterSpacing etc
    // Note: Clay_String->chars is not guaranteed to be null terminated
    return (Clay_Dimensions) {
            .width = 0.0f,
            .height = 0.0f
    };
}

static void clay_initialize(struct app_data_t *data) {
    // Note: malloc is only used here as an example, any allocator that provides
    // a pointer to addressable memory of at least totalMemorySize will work
    uint64_t totalMemorySize = Clay_MinMemorySize();
    Clay_Arena arena = Clay_CreateArenaWithCapacityAndMemory(
            totalMemorySize,
            lv_malloc(totalMemorySize)
    );
    Clay_Initialize(arena, (Clay_Dimensions) {
            .width = (float) ANativeWindow_getWidth(data->window),
            .height = (float) ANativeWindow_getHeight(data->window)
    }, (Clay_ErrorHandler) {
            .errorHandlerFunction = clay_error_handler
    });

    // Tell clay how to measure text
    Clay_SetMeasureTextFunction(clay_measure_text);
}

/**********************
 *   ANDROID FUNCTIONS
 **********************/
static void handle_cmd(struct android_app *app, int32_t cmd) {
    static int32_t format = WINDOW_FORMAT_RGB_565;
    struct app_data_t *data = (struct app_data_t *) app->userData;

    switch (cmd) {
        case APP_CMD_INIT_WINDOW:
            format = ANativeWindow_getFormat(app->window);
            create_display(app);
            create_input(data);
            clay_initialize(data);
            break;
        case APP_CMD_TERM_WINDOW:
            destroy_display(data, format);
            break;
        case APP_CMD_WINDOW_RESIZED:
            destroy_display(data, format);
            create_display(app);
            create_input(data);
            break;
        default:
            break;

    }
}

int32_t handle_input(struct android_app *app, AInputEvent *event) {
    size_t ptrCount = AMotionEvent_getPointerCount(event);
    // Checking for multi-touch that is not supported
    if (ptrCount != 1) {
        return false;
    }

    struct app_data_t *data = (struct app_data_t *) app->userData;
    LV_ASSERT_NULL(data)

    int32_t source = AInputEvent_getSource(event);
    int32_t action = AMotionEvent_getAction(event);
    int32_t actionMasked = action & AMOTION_EVENT_ACTION_MASK;
    int32_t ptrIndex = (action & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK)
            >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
    bool isOnScreen = (source & AINPUT_SOURCE_TOUCHSCREEN) != 0;
    float x = AMotionEvent_getX(event, ptrIndex);
    float y = AMotionEvent_getY(event, ptrIndex);

    data->touchData.x = (int32_t) x;
    data->touchData.y = (int32_t) y;
    data->touchData.pressed = actionMasked == AMOTION_EVENT_ACTION_DOWN ||
                              actionMasked == AMOTION_EVENT_ACTION_POINTER_DOWN;

    return isOnScreen;
}

__attribute__((unused))
void android_main(struct android_app *app) {
    struct app_data_t app_data;
    lv_memzero(&app_data, sizeof(app_data));

    app->userData = &app_data;
    app->onAppCmd = handle_cmd;
    app->onInputEvent = handle_input;

    int *events = NULL;
    struct android_poll_source *source = NULL;
    while (!app->destroyRequested) {
        int timeout = app_data.running ? 0 : -1;
        int result = ALooper_pollOnce(timeout, NULL, events, (void **) &source);
        if (result == ALOOPER_POLL_ERROR) {
            LOGE("ALooper_pollOnce returned error %d\n", result);
            break;
        }
        if (source) {
            source->process(app, source);
        }
    }
}
