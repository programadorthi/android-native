#include <android/log.h>
#include <android_native_app_glue.h>

#include "lvgl.h"

#define LOG_TAG "NativeApplication"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

bool hasWindow = false;

__attribute__((unused))
void android_main(struct android_app* app)
{
    lv_init();

    int *events = NULL;
    struct android_poll_source *source;
    while (!app->destroyRequested)
    {
        int timeout = hasWindow ? 0 : -1;
        int result = ALooper_pollOnce(timeout, NULL, events, (void **) &source);
        if (result == ALOOPER_POLL_ERROR)
        {
            LOGE("ALooper_pollOnce returned error %d\n", result);
            break;
        }
        if (source)
        {
            source->process(app, source);
        }
        if (hasWindow)
        {
            uint32_t delay = lv_timer_handler();
            lv_delay_ms(delay);
        }
    }

    lv_deinit();
}