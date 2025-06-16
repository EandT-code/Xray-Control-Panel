SET(SOURCES screens/ui_Screen1.c
    ui.c
    components/ui_comp_hook.c
    ui_helpers.c
    images/ui_img_thermometer_png.c
    images/ui_img_radiation_warning_png.c
    images/ui_img_exposure_png.c
    images/ui_img_battery_0_png.c
    images/ui_img_battery_25_png.c
    images/ui_img_battery_50_png.c
    images/ui_img_battery_75_png.c
    images/ui_img_battery_full_png.c)

add_library(ui ${SOURCES})
