// This file was generated by SquareLine Studio
// SquareLine Studio version: SquareLine Studio 1.3.1
// LVGL version: 8.2.0
// Project name: SquareLine_Project

#ifndef _SQUARELINE_PROJECT_UI_H
#define _SQUARELINE_PROJECT_UI_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined __has_include
#if __has_include("lvgl.h")
#include "lvgl.h"
#elif __has_include("lvgl/lvgl.h")
#include "lvgl/lvgl.h"
#else
#include "lvgl.h"
#endif
#else
#include "lvgl.h"
#endif

#include "ui_helpers.h"
#include "ui_events.h"
// SCREEN: ui_Screen1
void ui_Screen1_screen_init(void);
extern lv_obj_t * ui_Screen1;
extern lv_obj_t * ui_Panel2;
extern lv_obj_t * ui_CoolDown;
extern lv_obj_t * ui_eXPOSURE;
extern lv_obj_t * ui_radiationDanger;
extern lv_obj_t * ui_battery0;
extern lv_obj_t * ui_battery25;
extern lv_obj_t * ui_battery50;
extern lv_obj_t * ui_Image75;
extern lv_obj_t * ui_battery100;
extern lv_obj_t * ui_Panel1;
extern lv_obj_t * ui_Labelstatus;
extern lv_obj_t * ui_labelKv;
extern lv_obj_t * ui_labelCurrent;
extern lv_obj_t * ui_Panel4;
extern lv_obj_t * ui_labelmS;
extern lv_obj_t * ui_labelTimer;
extern lv_obj_t * ui____initial_actions0;

LV_IMG_DECLARE(ui_img_thermometer_png);    // assets\thermometer.png
LV_IMG_DECLARE(ui_img_radiation_warning_png);    // assets\Radiation_warning.png
LV_IMG_DECLARE(ui_img_exposure_png);    // assets\Exposure.png
LV_IMG_DECLARE(ui_img_battery_0_png);    // assets\battery_0.png
LV_IMG_DECLARE(ui_img_battery_25_png);    // assets\battery_25.png
LV_IMG_DECLARE(ui_img_battery_50_png);    // assets\battery_50.png
LV_IMG_DECLARE(ui_img_battery_75_png);    // assets\battery_75.png
LV_IMG_DECLARE(ui_img_battery_full_png);    // assets\battery_full.png

void ui_init(void);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
