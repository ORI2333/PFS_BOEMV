#ifndef __APP_DEMO_ENVIRONMENT_H__
#define __APP_DEMO_ENVIRONMENT_H__
#include <app_demo_multi_sample.h>
#include <hi_types_base.h>
#include <ssd1306_oled.h>

#define ENVI_DISPLAY_INTERVAL_TIME  (1)

extern hi_u8 g_menu_select;
extern hi_u8 g_menu_mode;
extern hi_u8  g_current_mode;
extern hi_u8  g_current_type;
extern hi_float g_temperature;
extern hi_float g_humidity;
extern hi_double g_combustible_gas_value;

extern hi_void oled_fill_screen(hi_u8 fii_data);
extern hi_void oled_show_str(hi_u8 x, hi_u8 y, hi_u8 *chr, hi_u8 char_size);
extern hi_u32 oled_init(hi_void);
extern hi_u32 th06_get_data(hi_void);
extern hi_void mq2_get_data(hi_void);
extern hi_u8  *flaot_to_string(hi_double d, hi_u8 *str);

hi_void return_all_mode_enum_sample(hi_void);
hi_void show_all_environment_value(hi_void);
hi_void show_temperature_value(hi_void);
hi_void show_humidity_value(hi_void);
hi_void show_combustible_gas_value(hi_void);
hi_void environment_func(hi_void);
#endif