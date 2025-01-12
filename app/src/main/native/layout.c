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

static void RenderHeaderButton(Clay_String text);

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
                                    },
                                    .padding = {16},
                                    .childGap = 16,
                                    .childAlignment = {
                                            .y = CLAY_ALIGN_Y_CENTER
                                    }
                            })
        ) {
            RenderHeaderButton(CLAY_STRING("File"));
            RenderHeaderButton(CLAY_STRING("Edit"));
            RenderHeaderButton(CLAY_STRING("Upload"));
            RenderHeaderButton(CLAY_STRING("Media"));
            RenderHeaderButton(CLAY_STRING("Support"));
        }
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

static void RenderHeaderButton(Clay_String text) {
    CLAY(
            CLAY_LAYOUT({.padding = {16, 8}}),
            CLAY_RECTANGLE({
                                   .color = {.r = 140, .g = 140, .b = 140, .a = 255},
                                   .cornerRadius = 5
                           })
    ) {
        CLAY_TEXT(text, CLAY_TEXT_CONFIG({
                                                 .fontSize = 16,
                                                 .textColor = {.r = 255, .g = 255, .b = 255, .a = 255}
                                         }));
    }
}