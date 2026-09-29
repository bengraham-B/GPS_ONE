/**
 * @file screen1_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "screen1_gen.h"
#include "../lvgl_ui_project.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/***********************
 *  STATIC VARIABLES
 **********************/

/***********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t * screen1_create(void)
{
    LV_TRACE_OBJ_CREATE("begin");

    static lv_style_t btn;

    static bool style_inited = false;

    if (!style_inited) {
        /*Init all styles*/
        lv_style_init(&btn);

        lv_style_set_radius(&btn, 5);
        lv_style_set_pad_all(&btn, 4);
        lv_style_set_bg_color(&btn, lv_color_hex(0xf59e0b));
        lv_style_set_text_color(&btn, lv_color_hex(0x000000));

        style_inited = true;
    }


    lv_obj_t * the_root = NULL;

    #if LVGL_UI_PROJECT_CHECK_COMPILE_TARGET(LVGL_UI_PROJECT_TARGET_ALL)
    if (lvgl_ui_project_check_target(LVGL_UI_PROJECT_TARGET_ALL)) {
        lv_obj_t * lv_obj_0 = lv_obj_create(NULL);
        lv_obj_set_name_static(lv_obj_0, "screen1_#");

        lv_obj_t * lv_button_0 = lv_button_create(lv_obj_0);
        lv_obj_set_style_text_align(lv_button_0, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_set_x(lv_button_0, 113);
        lv_obj_set_y(lv_button_0, 102);
        lv_obj_set_width(lv_button_0, 89);
        lv_obj_set_height(lv_button_0, 29);
        lv_obj_add_style(lv_button_0, &btn, 0);
        lv_obj_t * lv_label_0 = lv_label_create(lv_button_0);
        lv_label_set_text(lv_label_0, "Press");
        lv_obj_set_align(lv_label_0, LV_ALIGN_CENTER);
        lv_obj_set_width(lv_label_0, 80);
        lv_obj_set_height(lv_label_0, 16);

        the_root = lv_obj_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

