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
    lv_obj_t *display = lv_screen_active();

    struct app_data_t *data = lv_display_get_driver_data(lv_display_get_default());
    LV_ASSERT_NULL(data);

    //*Change the active screen's background color*//*
    lv_obj_set_style_bg_color(display, lv_color_hex(0x003a57), LV_PART_MAIN);

    //*Create a white label, set its text and align it to the center*//*
    lv_obj_t *label = lv_label_create(display);

    lv_label_set_long_mode(label, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_set_width(label, ANativeWindow_getWidth(data->window));
    lv_label_set_text(label,
                      "Lorem Ipsum is simply dummy text of the printing and typesetting industry");
    lv_obj_set_style_text_color(display, lv_color_hex(0xffffff), LV_PART_MAIN);
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

    lv_obj_t * btn = lv_button_create(lv_screen_active());     /*Add a button the current screen*/
    lv_obj_align(btn, LV_ALIGN_CENTER, 0, -220);
    lv_obj_set_size(btn, 240, 100);                          /*Set its size*/
    //lv_obj_add_event_cb(btn, btn_event_cb, LV_EVENT_ALL, NULL);           /*Assign a callback to the button*/

    lv_obj_t * btnLabel = lv_label_create(btn);          /*Add a label to the button*/
    lv_label_set_text(btnLabel, "Button");                     /*Set the labels text*/
    lv_obj_center(btnLabel);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

