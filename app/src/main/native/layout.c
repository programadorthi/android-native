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
    Clay_Sizing layoutExpand = {
            .width = CLAY_SIZING_GROW(),
            .height = CLAY_SIZING_GROW()
    };

    Clay_RectangleElementConfig contentConfig = {
            .color = {.r = 90, .g = 90, .b = 90, .a = 255},
            .cornerRadius = 16
    };

    CLAY(
            CLAY_ID("OuterContainer"),
            CLAY_RECTANGLE({.color = {.r = 43, .g = 41, .b = 51, .a = 255}}),
            CLAY_LAYOUT({
                                .layoutDirection = CLAY_TOP_TO_BOTTOM,
                                .sizing = layoutExpand,
                                .padding = {16, 16},
                                .childGap = 16
                        })
    ) {
        CLAY(
                CLAY_ID("HeaderBar"),
                CLAY_RECTANGLE(contentConfig),
                CLAY_LAYOUT({
                                    .sizing = {
                                            .width = CLAY_SIZING_GROW(),
                                            .height = CLAY_SIZING_PERCENT(0.1)
                                    }
                            })
        ) {}
        CLAY(
                CLAY_ID("LowerContent"),
                CLAY_LAYOUT({.sizing = layoutExpand, .childGap = 16})
        ) {
            CLAY(
                    CLAY_ID("Sidebar"),
                    CLAY_RECTANGLE(contentConfig),
                    CLAY_LAYOUT({
                                        .sizing = {
                                                .width = CLAY_SIZING_FIXED(250),
                                                .height = CLAY_SIZING_GROW()
                                        }
                                })
            ) {}
            CLAY(
                    CLAY_ID("MainContent"),
                    CLAY_RECTANGLE(contentConfig),
                    CLAY_LAYOUT({.sizing = layoutExpand})
            ) {}
        }
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
