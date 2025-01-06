#include <android/log.h>
#include <android_native_app_glue.h>

#include "lvgl.h"

/*********************
 *      DEFINES
 *********************/
#define LOG_TAG "NativeApplication"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

// TODO: Dynamic buffer size based on W x H is not working on Andorid 25+
// TODO: Implement dynamic buffer size based on device resolution to support res > 2048
#define BUF_SIZE (2048 * 2048 * (LV_COLOR_DEPTH + 7) / 8)

/**********************
 *      TYPEDEFS
 **********************/
struct touch_data_t {
    bool pressed;
    int32_t x;
    int32_t y;
};

struct app_data_t {
    bool running; // Flag to stop tick thread
    ANativeWindow *window;  // Android window object
    pthread_t tickThread; // Tick thread
    struct touch_data_t touchData; // The touch state
};

/**********************
 *  STATIC VARIABLES
 **********************/
// Double buffering
static uint8_t buf1[BUF_SIZE];
static uint8_t buf2[BUF_SIZE];

/**********************
 *      MACROS
 **********************/

/**********************
 *   STATIC FUNCTIONS
 **********************/

// TODO: maybe an extern to decouple from main.c file
static void render();

static uint32_t currentTimeInMillis(void) {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    uint64_t timeMs = t.tv_sec * 1000 + (t.tv_nsec / 1000000);
    return timeMs;
}

static void *refresh_routine(void *data) {
    struct app_data_t *app_data = data;
    LV_ASSERT_NULL(app_data);

    while (app_data->running) {
        uint32_t timeUntilNext = lv_timer_handler();
        lv_delay_ms(timeUntilNext);
    }

    return NULL;
}

static void flush_cb(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map) {
    struct app_data_t *data = lv_display_get_driver_data(disp);
    LV_ASSERT_NULL(data);

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

    // TODO: There is a faster way to copy pixels? I don't know yet.
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
    LV_ASSERT_NULL(appData);

    data->point.x = appData->touchData.x;
    data->point.y = appData->touchData.y;

    data->state = appData->touchData.pressed ? LV_INDEV_STATE_PRESSED : LV_INDEV_STATE_RELEASED;
}

static void close_display(struct app_data_t *data) {
    data->running = false;
    void *ret = NULL;
    pthread_join(data->tickThread, &ret);
}

static void open_display(struct app_data_t *data, int32_t dpi) {
    int32_t hor_res = ANativeWindow_getWidth(data->window);
    int32_t ver_res = ANativeWindow_getHeight(data->window);

    lv_display_t *display = lv_display_create(hor_res, ver_res);
    lv_display_set_driver_data(display, data);
    lv_display_set_dpi(display, dpi);
    lv_display_set_flush_cb(display, flush_cb);
    lv_display_set_buffers(display, buf1, buf2, BUF_SIZE, LV_DISPLAY_RENDER_MODE_FULL);

    lv_theme_t *theme = lv_theme_default_init(
            display,
            lv_palette_main(LV_PALETTE_BLUE),
            lv_palette_main(LV_PALETTE_CYAN),
            false,
            LV_FONT_DEFAULT
    );
    lv_display_set_theme(display, theme);

    lv_indev_t *input = lv_indev_create();
    lv_indev_set_driver_data(input, data);
    lv_indev_set_type(input, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(input, input_read_cb);

    render();

    data->running = true;
    pthread_create(&data->tickThread, NULL, refresh_routine, data);
}

static void render() {
    lv_obj_t *display = lv_screen_active();

    struct app_data_t *data = lv_display_get_driver_data(lv_display_get_default());
    LV_ASSERT_NULL(data);

    //*Change the active screen's background color*//*
    lv_obj_set_style_bg_color(display, lv_color_hex(0x003a57), LV_PART_MAIN);

    //*Create a white label, set its text and align it to the center*//*
    lv_obj_t *label = lv_label_create(display);

    lv_label_set_long_mode(label, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_set_width(label, ANativeWindow_getWidth(data->window));
    lv_label_set_text(label,
                      "Lorem Ipsum is simply dummy text of the printing and typesetting industry");
    lv_obj_set_style_text_color(display, lv_color_hex(0xffffff), LV_PART_MAIN);
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

    lv_obj_t * btn = lv_button_create(lv_screen_active());     /*Add a button the current screen*/
    lv_obj_align(btn, LV_ALIGN_CENTER, 0, -220);
    lv_obj_set_size(btn, 240, 100);                          /*Set its size*/
    //lv_obj_add_event_cb(btn, btn_event_cb, LV_EVENT_ALL, NULL);           /*Assign a callback to the button*/

    lv_obj_t * btnLabel = lv_label_create(btn);          /*Add a label to the button*/
    lv_label_set_text(btnLabel, "Button");                     /*Set the labels text*/
    lv_obj_center(btnLabel);
}

/*static void lvgl_log_cb(lv_log_level_t level, const char *buf) {
    if (level == LV_LOG_LEVEL_ERROR) {
        LOGE(">> LVGL: %s", buf);
    } else {
        LOGI(">> LVGL: %s", buf);
    }
}*/

/**********************
 *   ANDROID FUNCTIONS
 **********************/
static void handle_cmd(struct android_app *app, int32_t cmd) {
    static int32_t format = WINDOW_FORMAT_RGB_565;
    struct app_data_t *data = (struct app_data_t *) app->userData;

    switch (cmd) {
        case APP_CMD_INIT_WINDOW:
            data->window = app->window;

            AConfiguration *config = AConfiguration_new();
            AConfiguration_fromAssetManager(config, app->activity->assetManager);
            int32_t dpi = AConfiguration_getDensity(config);
            AConfiguration_delete(config);

            format = ANativeWindow_getFormat(data->window);
            ANativeWindow_setBuffersGeometry(
                    data->window,
                    ANativeWindow_getWidth(data->window),
                    ANativeWindow_getHeight(data->window),
                    WINDOW_FORMAT_RGBA_8888
            );

            open_display(data, dpi);
            break;
        case APP_CMD_TERM_WINDOW:
            // FIXME: ANDROID <=26 call APP_CMD_TERM_WINDOW during screen rotation :/
            close_display(data);
            ANativeWindow_setBuffersGeometry(
                    data->window,
                    ANativeWindow_getWidth(data->window),
                    ANativeWindow_getHeight(data->window),
                    format
            );
            break;
        default:
            // TODO: screen rotation font size not good
            break;

    }
}

int32_t handle_input(struct android_app *app, AInputEvent *event) {
    struct app_data_t *data = (struct app_data_t *) app->userData;
    LV_ASSERT_NULL(data);

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

    lv_init();

    //lv_log_register_print_cb(lvgl_log_cb);

    lv_tick_set_cb(currentTimeInMillis);

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

    lv_deinit();
}
