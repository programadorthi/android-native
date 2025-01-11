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
            CLAY_RECTANGLE({.color = {.r = 43, .g = 41, .b = 51, .a = 255}}),
            CLAY_LAYOUT((Clay_LayoutConfig) {
                    .layoutDirection = CLAY_TOP_TO_BOTTOM,
                    .sizing = {
                            .width = CLAY_SIZING_GROW(),
                            .height = CLAY_SIZING_GROW()
                    },
                    .padding = {16, 16}
            })
    ) {
        CLAY(
                CLAY_ID("HeaderBar"),
                CLAY_RECTANGLE({
                                       .color = {.r = 90, .g = 90, .b = 90, .a = 255},
                                       .cornerRadius = 40
                               }),
                CLAY_LAYOUT((Clay_LayoutConfig) {
                        .sizing = {
                                .width = CLAY_SIZING_GROW(),
                                .height = CLAY_SIZING_PERCENT(0.1)
                        }
                })
        ) {}
        CLAY(CLAY_ID("LowerContent")) {}
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
