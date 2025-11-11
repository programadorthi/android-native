#include <android/log.h>
#include <android_native_app_glue.h>
#include "raylib.h"

#define LOG_TAG "NativeApplication"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

__attribute__((unused))
void android_main(struct android_app* app)
{
    LOGI( ">>>> Starting...\n" );
}