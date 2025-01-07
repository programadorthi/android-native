/**
 * @file types.h
 *
 */

#ifndef NATIVE_APPLICATION_TYPES_H
#define NATIVE_APPLICATION_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include <android_native_app_glue.h>

/*********************
 *      DEFINES
 *********************/

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
    uint8_t *buffer[2]; // Double buffering
};

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif //NATIVE_APPLICATION_TYPES_H
