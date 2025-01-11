/**
 * @file layout.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "layout.h"

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

void layout() {
    CLAY(
            CLAY_ID("OuterContainer"),
            CLAY_RECTANGLE(
                    {
                            .color = {255, 0, .0, 255},
                            .cornerRadius = (Clay_CornerRadius) {
                                    .topLeft = 5,
                                    .topRight = 6,
                                    .bottomLeft = 7,
                                    .bottomRight = 8
                            }
                    }
            ), CLAY_LAYOUT((Clay_LayoutConfig) {
            .sizing = {
                    .width = CLAY_SIZING_GROW(),
                    .height = CLAY_SIZING_GROW()
            }
    })) {}
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
