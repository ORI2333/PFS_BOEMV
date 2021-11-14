#ifndef __APP_DEMO_COLORFUL_SAMPLE_H__
#define __APP_DEMO_COLORFUL_SAMPLE_H__

#include <hi_early_debug.h>
#include <hi_types_base.h>
#include <app_demo_multi_sample.h>

extern hi_u8  g_current_mode;
extern hi_u8  g_current_type;
extern hi_u8 g_menu_mode;
extern hi_u8 g_menu_select;

extern hi_void oled_show_str(hi_u8 x, hi_u8 y, hi_u8 *chr, hi_u8 char_size);
extern hi_void oled_fill_screen(hi_u8 fii_data);
extern hi_u32 oled_init(hi_void);
hi_void colorful_light_func(hi_void);

#endif