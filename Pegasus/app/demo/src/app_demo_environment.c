/*
 * Copyright (c) 2020 HiHope Community.
 * Description: oled demo
 * Author: HiSpark Product Team.
 * Create: 2020-5-20
 */
#include <hi_types_base.h>
#include <app_demo_multi_sample.h>
#include <hi_i2c.h>
#include <app_demo_environment.h>
#include <hi_time.h>
#include <hi_early_debug.h>
#include <hi_pwm.h>

extern hi_void get_aht20_sensor_datat(hi_void);
extern hi_u8  *flaot_to_string_gas(hi_double d, hi_u8 *str);
extern hi_u8 g_ahu20_humidity_buff[5];
extern hi_u8 g_ahu20_temperature_buff[5];
static hi_u8 hi3861_board_led_test =0;
extern hi_float aht_temper;
extern hi_float aht_humi;
/**
	@berf Return to the initial interface of environmental monitoring
*/
hi_void return_all_mode_enum_sample(hi_void)
{
    hi_u8 current_mode = g_current_mode;
    while (1) {      
        if (current_mode != g_current_mode) {
            break;
        }
        hi_sleep(SLEEP_1_MS);
    }
}
/**
	@berf environment menu display
	@berf The temperature and humidity data and smoke data are displayed on the OLED screen
*/
hi_void show_all_environment_value(hi_void)
{
    hi_u8 combustible_main_menu_gas_value_str[10]={0};
    hi_u8 temperature_str[6] ={0};
    hi_u8 humidity_str[6]={0};
    hi_u8 current_mode = g_current_mode;
    hi_i2c_init(HI_I2C_IDX_0, HI_I2C_IDX_BAUDRATE); /* baudrate: 400000 */
    hi_i2c_set_baudrate(HI_I2C_IDX_0, HI_I2C_IDX_BAUDRATE);
    
    while(1) {
        // hi_udelay(DELAY_10_MS);//delay 10ms
        get_aht20_sensor_datat();
        mq2_get_data();

        flaot_to_string(aht_temper, temperature_str);
        flaot_to_string(aht_humi, humidity_str);  
        flaot_to_string(g_combustible_gas_value, combustible_main_menu_gas_value_str); 

      //  oled_show_str(18,5,temperature_str,1);
       // oled_show_str(81,5,humidity_str,1);  
        if (!g_combustible_gas_value)   {
           //  oled_show_str(48,6,"0.00    ",1);
        } else {
          //  oled_show_str(48,6,combustible_main_menu_gas_value_str,1);
        }      
        if (current_mode != g_current_mode) {
            break;
        }

        hi_sleep(SLEEP_10_MS);//10ms
    }
}
/** 
	@berf temperature menu display
	@berf The temperature value is obtained and displayed on the OLED screen
*/
hi_void show_temperature_value(hi_void)
{
    hi_u8 current_mode = g_current_mode;
    hi_u8 temperature_str[6] ={0};
    hi_i2c_init(HI_I2C_IDX_0, HI_I2C_IDX_BAUDRATE); /* baudrate: 400000 */
    hi_i2c_set_baudrate(HI_I2C_IDX_0, HI_I2C_IDX_BAUDRATE);

    while(1) {
        hi_udelay(DELAY_10_MS);//delay 10ms
        get_aht20_sensor_datat();
        flaot_to_string(aht_temper, temperature_str); 
       // oled_show_str(40,5,temperature_str,1);
        if (current_mode != g_current_mode) {
            break;
        }

        hi_sleep(SLEEP_10_MS);//10ms
    }
}
/** 
	@berf humidity value display 
	@berfThe humidity value is obtained and displayed on the OLED screen
*/
hi_void show_humidity_value(hi_void)
{
    hi_u8 current_mode = g_current_mode;
    hi_u8 humidity_str[6]={0};
    hi_i2c_init(HI_I2C_IDX_0, HI_I2C_IDX_BAUDRATE); /* baudrate: 400000 */
    hi_i2c_set_baudrate(HI_I2C_IDX_0, HI_I2C_IDX_BAUDRATE);
    while(1) {
        hi_udelay(DELAY_10_MS);//delay 10ms
        get_aht20_sensor_datat();
        flaot_to_string(aht_humi, humidity_str);  
       // oled_show_str(56,5,humidity_str,1);  
        if (current_mode != g_current_mode) {
            break;
        }

        hi_sleep(SLEEP_10_MS);//10ms
    }
}
/** 
	@berf combustible gas value display 
	@berf The obtained combustible gas concentration value is displayed on the OLED screen
*/
hi_void show_combustible_gas_value(hi_void)
{
    hi_u8 combustible_gas_value_str[10]={0};
    hi_u8 current_mode = g_current_mode;
    hi_i2c_init(HI_I2C_IDX_0, HI_I2C_IDX_BAUDRATE); /* baudrate: 400000 */
    hi_i2c_set_baudrate(HI_I2C_IDX_0, HI_I2C_IDX_BAUDRATE);

    while(1) {
        g_combustible_gas_value = 0.0;
        mq2_get_data();
        flaot_to_string(g_combustible_gas_value, combustible_gas_value_str); 
        if (!g_combustible_gas_value)   {
           //  oled_show_str(60,5,"0.00    ",1);
        } else {
           // oled_show_str(60,5,combustible_gas_value_str,1);
        }  
        
        if (current_mode != g_current_mode) {
            break;
        }

        hi_sleep(SLEEP_10_MS);//10ms
    }
}
/* environment function hamdle and display */
hi_void environment_func(hi_void)
{
    /*Initialization time screen i2c baudrate setting*/
    hi_i2c_init(HI_I2C_IDX_0, HI_I2C_IDX_BAUDRATE); /* baudrate: 400kbps */
    hi_i2c_set_baudrate(HI_I2C_IDX_0, HI_I2C_IDX_BAUDRATE);
    /*ssd1306 config init*/
    // oled_init();
    while( HI_ERR_SUCCESS != oled_init()) {
         printf("Connecting oled board falied!Please access oled board\r\n");
         if (hi3861_board_led_test == 0) {
            hi3861_board_led_test =1;
             /*test HiSpark board*/
            FACTORY_HISPARK_BOARD_TEST("-----------HiSpark board test----------");  
         }
         hi_sleep(SLEEP_1S);
    }
    /*Key interrupt initialization*/
    extern hi_void test_gpio_init(hi_void);
    test_gpio_init();
    
    while (1) {
        switch (g_current_mode) {
            case ENV_ALL_MODE:
                //  oled_fill_screen(OLED_CLEAN_SCREEN);//clean screen
                //  oled_show_str(0,0, "WiFi-AP  ON  U:1",1);
                //  oled_show_str(0,1, "                ",1);
                //  oled_show_str(0,2, "  Environment   ",1);
                //  oled_show_str(0,3, "  Monitoring    ",1);
                //  oled_show_str(0,4, "                ",1);
                //  oled_show_str(0,5, "T:    C H:    % ",1);
                //  oled_show_str(0,6, "C_Gas:00        ",1);
                //  oled_show_str(0,7, "                ",1);
                 show_all_environment_value();
                 break;
            case ENV_TEMPERRATURE_MODE:
                //  oled_fill_screen(OLED_CLEAN_SCREEN);//clean screen 
                //  oled_show_str(0,0, "                ",1);
                //  oled_show_str(0,1, "                ",1);
                //  oled_show_str(0,2, "  Temperature   ",1);
                //  oled_show_str(0,3, "                ",1);
                //  oled_show_str(0,4, "                ",1);
                //  oled_show_str(0,5, "   T:    C      ",1);
                //  oled_show_str(0,6, "                ",1);
                //  oled_show_str(0,7, "                ",1);
                 show_temperature_value();
                 break;
            case ENV_HUMIDITY_MODE:
                //  oled_fill_screen(OLED_CLEAN_SCREEN);//clean screen
                //  oled_show_str(0,0, "                ",1);
                //  oled_show_str(0,1, "                ",1);
                //  oled_show_str(0,2, "    Humidity    ",1);
                //  oled_show_str(0,3, "                ",1);
                //  oled_show_str(0,4, "                ",1);
                //  oled_show_str(0,5, "     H:    %    ",1);
                //  oled_show_str(0,6, "                ",1);
                //  oled_show_str(0,7, "                ",1); 
                 show_humidity_value();
                 break;
            case COMBUSTIBLE_GAS_MODE:
                //  oled_fill_screen(OLED_CLEAN_SCREEN);//clean screen
                //  oled_show_str(0,0, "                ",1);
                //  oled_show_str(0,1, "                ",1);
                //  oled_show_str(0,2, "     C_Gas      ",1);
                //  oled_show_str(0,3, "                ",1);
                //  oled_show_str(0,4, "                ",1);
                //  oled_show_str(0,5, "  C_Gas:        ",1);
                //  oled_show_str(0,6, "                ",1);
                //  oled_show_str(0,7, "                ",1); 
                 show_combustible_gas_value();
                 break;               
            case ENV_RETURN_MODE:
                //  oled_fill_screen(OLED_CLEAN_SCREEN);//clean screen
                //  oled_show_str(0,0, "                ",1);
                //  oled_show_str(0,1, "                ",1);                               
                //  oled_show_str(0,2, "Return Menu     ",1);
                //  oled_show_str(0,3, "  Environment   ",1);
                //  oled_show_str(0,4, "                ",1);
                //  oled_show_str(0,5, "                ",1);
                //  oled_show_str(0,6, "                ",1);
#ifdef  CONFIG_ENVIRONMENTAL_MONITORING
                 oled_show_str(0,7, "Continue        ",1); 
                 return_all_mode_enum_sample();
#elif   CONFIG_ALL_DEMO
                 oled_show_str(0,7, "Continue   Exit ",1);                 
                 return_main_enum_sample();
                 if (g_current_type == KEY_DOWN) {
                     g_current_mode = ENV_ALL_MODE;
                     oled_main_menu_display();                    
                     return;
                 }
                 g_current_mode = ENV_ALL_MODE; 
#endif                 
                 break;               
        default:
            break;
        }
    }
}