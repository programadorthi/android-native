/**
 * @file render.h
 *
 */

#ifndef NATIVE_APPLICATION_RENDER_H
#define NATIVE_APPLICATION_RENDER_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "clay.h"
#include "lvgl.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * The layout entrypoint called by the main
 */
void render(lv_obj_t *canvas);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif //NATIVE_APPLICATION_RENDER_H
