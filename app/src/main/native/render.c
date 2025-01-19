/**
 * @file render.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lvgl.h"
#include "render.h"

#if LV_USE_DEMO_MUSIC
#include "demos/music/lv_demo_music.h"
#endif

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void render() {
#if LV_USE_DEMO_MUSIC
    lv_demo_music();
#endif
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

