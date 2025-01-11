/**
 * @file render.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include <android/native_window.h>
#include "lvgl.h"
#include "types.h"
#include "render.h"
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

static lv_color_t clay_color_to_lv_color(Clay_Color color);

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
    Clay_BeginLayout();

    layout();

    Clay_RenderCommandArray renderCommands = Clay_EndLayout();

    for (int j = 0; j < renderCommands.length; j++) {
        Clay_RenderCommand *renderCommand = Clay_RenderCommandArray_Get(&renderCommands, j);
        Clay_BoundingBox boundingBox = renderCommand->boundingBox;
        switch (renderCommand->commandType) {
            case CLAY_RENDER_COMMAND_TYPE_TEXT: {
                // TODO: render a text
                break;
            }
            case CLAY_RENDER_COMMAND_TYPE_IMAGE: {
                // TODO: render a image
                break;
            }
            case CLAY_RENDER_COMMAND_TYPE_SCISSOR_START: {
                // TODO: apply scissor start
                break;
            }
            case CLAY_RENDER_COMMAND_TYPE_SCISSOR_END: {
                // TODO: apply scissor end
                break;
            }
            case CLAY_RENDER_COMMAND_TYPE_RECTANGLE: {
                Clay_RectangleElementConfig *config = renderCommand->config.rectangleElementConfig;

                static lv_style_t style;
                lv_style_init(&style);
                lv_style_set_x(&style, (int32_t) boundingBox.x);
                lv_style_set_y(&style, (int32_t) boundingBox.y);
                lv_style_set_width(&style, (int32_t) boundingBox.width);
                lv_style_set_height(&style, (int32_t) boundingBox.height);
                lv_style_set_bg_color(&style, clay_color_to_lv_color(config->color));
                lv_style_set_bg_opa(&style, (uint8_t) config->color.a);

                // TODO: add support to each border radius
                lv_style_set_radius(&style, (int32_t) config->cornerRadius.topLeft);

                lv_obj_t *obj = lv_obj_create(lv_screen_active());
                lv_obj_add_style(obj, &style, 0);
                break;
            }
            case CLAY_RENDER_COMMAND_TYPE_BORDER: {
                // TODO: render a border
                break;
            }
            case CLAY_RENDER_COMMAND_TYPE_CUSTOM: {
                // TODO: render a custom layout
                break;
            }
            default: {
                LV_LOG_ERROR("Error: unhandled render command.");
            }
        }
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static lv_color_t clay_color_to_lv_color(Clay_Color color) {
    return (lv_color_t) {
            .blue = (uint8_t) color.b,
            .green = (uint8_t) color.g,
            .red = (uint8_t) color.r
    };
}