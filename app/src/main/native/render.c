/**
 * @file render.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include <android/native_window.h>
#include "lvgl.h"
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

static void render_rectangle(lv_obj_t *canvas, Clay_RenderCommand *renderCommand);

static void render_text(lv_obj_t *canvas, Clay_RenderCommand *renderCommand);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void render(lv_obj_t *canvas) {
    Clay_BeginLayout();

    layout();

    Clay_RenderCommandArray renderCommands = Clay_EndLayout();

    for (int j = 0; j < renderCommands.length; j++) {
        Clay_RenderCommand *renderCommand = Clay_RenderCommandArray_Get(&renderCommands, j);
        switch (renderCommand->commandType) {
            case CLAY_RENDER_COMMAND_TYPE_TEXT: {
                render_text(canvas, renderCommand);
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
                render_rectangle(canvas, renderCommand);
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

static void render_rectangle(lv_obj_t *canvas, Clay_RenderCommand *renderCommand) {
    Clay_BoundingBox boundingBox = renderCommand->boundingBox;
    Clay_RectangleElementConfig *config = renderCommand->config.rectangleElementConfig;

    lv_layer_t layer;
    lv_canvas_init_layer(canvas, &layer);

    lv_draw_rect_dsc_t rect;
    lv_draw_rect_dsc_init(&rect);
    rect.bg_color = clay_color_to_lv_color(config->color);
    rect.bg_opa = (uint8_t) config->color.a;
    rect.radius = (int32_t) config->cornerRadius.topLeft;

    lv_area_t coords = {
            .x1 = (int32_t) boundingBox.x,
            .y1 = (int32_t) boundingBox.y,
            // FIXME: Clay is removing double padding from width and height
            .x2 = (int32_t) (boundingBox.width + boundingBox.x),
            .y2 = (int32_t) (boundingBox.height + boundingBox.y)
    };

    lv_draw_rect(&layer, &rect, &coords);

    lv_canvas_finish_layer(canvas, &layer);
}

static void render_text(lv_obj_t *canvas, Clay_RenderCommand *renderCommand) {
    Clay_BoundingBox boundingBox = renderCommand->boundingBox;
    Clay_TextElementConfig *config = renderCommand->config.textElementConfig;

    lv_layer_t layer;
    lv_canvas_init_layer(canvas, &layer);

    lv_draw_label_dsc_t label;
    lv_draw_label_dsc_init(&label);

    label.color = clay_color_to_lv_color(config->textColor);
    label.text = renderCommand->text.chars;

    lv_area_t coords = {
            .x1 = (int32_t) boundingBox.x,
            .y1 = (int32_t) boundingBox.y,
            // FIXME: Clay is removing double padding from width and height
            .x2 = (int32_t) (boundingBox.width + boundingBox.x),
            .y2 = (int32_t) (boundingBox.height + boundingBox.y)
    };

    lv_draw_label(&layer, &label, &coords);

    lv_canvas_finish_layer(canvas, &layer);
}