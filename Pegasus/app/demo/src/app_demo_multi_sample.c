/*
 * Copyright (c) 2020 HiHope Community.
 * Description: colorful light demo
 * Author: HiSpark Product Team.
 * Create: 2020-5-20
 */
#include <app_demo_multi_sample.h>


hi_u8 g_menu_type =0;
hi_u8 g_menu_select = COLORFUL_LIGHT_MENU;
hi_u8 g_current_mode = CONTROL_MODE;
hi_u8 g_current_type = 0;
hi_u8 g_someone_walking_flag = 0;
hi_u8 g_with_light_flag = 0;

hi_u8 g_menu_mode      = MAIN_FUNCTION_SELECT_MODE;  
hi_u8 g_key_down_flag  = KEY_UP;

hi_u32 g_gpio5_tick = KEY_UP;
hi_u32 g_gpio7_tick = KEY_UP;
hi_u32 g_gpio8_tick = KEY_UP;
hi_u8  g_gpio9_tick = 0;
hi_u32 g_gpio7_first_key_dwon = HI_TRUE;
hi_u8 g_gpio8_current_type = 0;
hi_u8 g_beep_control_type =0;

hi_u8 oc_beep_status = BEEP_OFF;

extern hi_u8 get_light_status(hi_void);
extern hi_u8 get_human_status(hi_void);
extern hi_void get_gpio5_voltage(hi_void *param);
/*factory test HiSpark board*/
hi_void hispark_board_test(hi_gpio_value value)
{
    hi_io_set_func(HI_IO_NAME_GPIO_9, HI_IO_FUNC_GPIO_9_GPIO);
    hi_gpio_set_dir(HI_GPIO_IDX_9, HI_GPIO_DIR_OUT);
    hi_gpio_set_ouput_val(HI_GPIO_IDX_9, value);
}

/*gpio init*/
hi_void gpio_control(hi_io_name gpio, hi_gpio_idx id, hi_gpio_dir  dir,  hi_gpio_value gpio_val, hi_u8 val)
{
    hi_io_set_func(gpio, val);
    hi_gpio_set_dir(id, dir);
    hi_gpio_set_ouput_val(id, gpio_val);
}

/*pwm init*/
hi_void pwm_init(hi_io_name id, hi_u8 val, hi_pwm_port port)
{
    hi_io_set_func(id, val); /* PWM0 OUT */
    hi_pwm_init(port);
    hi_pwm_set_clock(PWM_CLK_160M);
}
/* gpio key init*/
hi_void hi_switch_init(hi_io_name id, hi_u8 val,hi_gpio_idx idx, hi_gpio_dir dir, hi_io_pull pval)
{
    hi_io_set_func(id, val);
    hi_gpio_set_dir(idx, dir);
    hi_io_set_pull(id,pval);
}

/*
VCC：5V
blue:  gpio12 yellow
green: gpio11 green 
red:   gpio10 red
switch ：
gpio7 switch1
gpio5 switch2
*/
hi_void test_gpio_init(hi_void)
{
    hi_switch_init(HI_IO_NAME_GPIO_5,HI_IO_FUNC_GPIO_5_GPIO,HI_GPIO_IDX_5,HI_GPIO_DIR_IN,HI_IO_PULL_UP);//switch2
    //hi_switch_init(HI_IO_NAME_GPIO_8,HI_IO_FUNC_GPIO_8_GPIO,HI_GPIO_IDX_8,HI_GPIO_DIR_IN,HI_IO_PULL_UP);//switch2

    //pwm_init(HI_IO_NAME_GPIO_10,HI_IO_FUNC_GPIO_10_PWM1_OUT,HI_PWM_PORT_PWM1);
    // pwm_init(HI_IO_NAME_GPIO_11,HI_IO_FUNC_GPIO_11_PWM2_OUT,HI_PWM_PORT_PWM2);
    // pwm_init(HI_IO_NAME_GPIO_12,HI_IO_FUNC_GPIO_12_PWM3_OUT,HI_PWM_PORT_PWM3);
}

/*
Three color light control mode: each time you press type S1 button, you can switch between the three states of green light, yellow light and red light
mode:
1.control mode

type:
1.red on
2.yellow on
3.green on
RED_ON = 1,
    YELLOW_ON,
    GREEN_ON,
*/
hi_void control_mode_sample(hi_void)
{
    hi_u8 current_type = g_current_type; 
    hi_u8 current_mode = g_current_mode;
    while (1) {
        switch (g_current_type) {
             case RED_ON:
             
                hi_pwm_start(HI_PWM_PORT_PWM1, PWM_SMALL_DUTY, PWM_MIDDLE_DUTY);
                hi_pwm_start(HI_PWM_PORT_PWM2, PWM_LOW_DUTY, PWM_MIDDLE_DUTY);
                hi_pwm_start(HI_PWM_PORT_PWM3, PWM_LOW_DUTY, PWM_MIDDLE_DUTY);
                break;
            case YELLOW_ON:
                 hi_gpio_set_ouput_val(HI_GPIO_IDX_12,HI_GPIO_VALUE1);
                // hi_udelay(1000);  
                // hi_gpio_set_ouput_val(HI_GPIO_IDX_12,HI_GPIO_VALUE0);
                // hi_udelay(20000-1000);  
                hi_pwm_start(HI_PWM_PORT_PWM1, PWM_LOW_DUTY, PWM_MIDDLE_DUTY);
                hi_pwm_start(HI_PWM_PORT_PWM2, PWM_SMALL_DUTY, PWM_MIDDLE_DUTY);
                hi_pwm_start(HI_PWM_PORT_PWM3, PWM_LOW_DUTY, PWM_MIDDLE_DUTY);
                
                break;
            case GREEN_ON:
                hi_gpio_set_ouput_val(HI_GPIO_IDX_12,HI_GPIO_VALUE1);
                hi_udelay(500);  
                hi_gpio_set_ouput_val(HI_GPIO_IDX_12,HI_GPIO_VALUE0);
                hi_udelay(20000-500);  
                hi_pwm_start(HI_PWM_PORT_PWM1, PWM_LOW_DUTY, PWM_MIDDLE_DUTY);
                hi_pwm_start(HI_PWM_PORT_PWM2, PWM_LOW_DUTY, PWM_MIDDLE_DUTY);
                hi_pwm_start(HI_PWM_PORT_PWM3, PWM_SMALL_DUTY, PWM_MIDDLE_DUTY);
                break;
            default:
                break;
        }
        
        if ((current_mode != g_current_mode) || (current_type != g_current_type)) {
            all_light_out();
            break;
        }
        hi_sleep(SLEEP_1_MS);
    }
}

/*Interrupt detection function, once every 10ms*/
hi_u8 delay_and_check_key_interrupt(hi_u32 delay_time)
{
    hi_u32 i = 0;
    hi_u32 cycle_count =0;
    hi_u8 current_mode =0;
    hi_u8 current_type =0;

    cycle_count = delay_time/DELAY_10_MS;
    current_mode = g_current_mode;
    current_type = g_current_type;

    for (i=0; i<cycle_count; i++) {
        if ((current_mode != g_current_mode) || (current_type != g_current_type)) {
            return KEY_DOWN_INTERRUPT;  
        }
        hi_udelay(DELAY_5_MS);   //10ms
        hi_sleep(SLEEP_1_MS);

     } 
     return HI_NULL;
}

/*Red, yellow and green light up in turn every one second*/
hi_void cycle_for_one_second(hi_void)
{
   while (1) {
        hi_pwm_start(HI_PWM_PORT_PWM1, PWM_SMALL_DUTY, PWM_MIDDLE_DUTY);

        if (delay_and_check_key_interrupt(DELAY_1_s) == KEY_DOWN_INTERRUPT) {
            hi_pwm_start(HI_PWM_PORT_PWM1, PWM_LOW_DUTY, PWM_MIDDLE_DUTY);
            break;
        }

        hi_pwm_start(HI_PWM_PORT_PWM1, PWM_LOW_DUTY, PWM_MIDDLE_DUTY);
        hi_pwm_start(HI_PWM_PORT_PWM2, PWM_SMALL_DUTY, PWM_MIDDLE_DUTY);

        if (delay_and_check_key_interrupt(DELAY_1_s) == KEY_DOWN_INTERRUPT) {
            hi_pwm_start(HI_PWM_PORT_PWM2, PWM_LOW_DUTY, PWM_MIDDLE_DUTY);
            break;
        }

        hi_pwm_start(HI_PWM_PORT_PWM2, PWM_LOW_DUTY, PWM_MIDDLE_DUTY);        
        hi_pwm_start(HI_PWM_PORT_PWM3, PWM_SMALL_DUTY, PWM_MIDDLE_DUTY);

        if (delay_and_check_key_interrupt(DELAY_1_s) == KEY_DOWN_INTERRUPT) {
            hi_pwm_start(HI_PWM_PORT_PWM3, PWM_LOW_DUTY, PWM_MIDDLE_DUTY);
            break;
        }
        hi_pwm_start(HI_PWM_PORT_PWM3, PWM_LOW_DUTY, PWM_MIDDLE_DUTY);
        hi_sleep(SLEEP_1_MS);
   }
}

/*Red, yellow and green light up in turn every 0.5 seconds*/
hi_void cycle_for_half_second(hi_void)
{
    while (1) {
        hi_pwm_start(HI_PWM_PORT_PWM1, PWM_SMALL_DUTY, PWM_MIDDLE_DUTY);

        if (delay_and_check_key_interrupt(DELAY_500_MS) == KEY_DOWN_INTERRUPT) {
            hi_pwm_start(HI_PWM_PORT_PWM1, PWM_LOW_DUTY, PWM_MIDDLE_DUTY);
            break;
        }

        hi_pwm_start(HI_PWM_PORT_PWM1, PWM_LOW_DUTY, PWM_MIDDLE_DUTY);       
        hi_pwm_start(HI_PWM_PORT_PWM2, PWM_SMALL_DUTY, PWM_MIDDLE_DUTY);

        if (delay_and_check_key_interrupt(DELAY_500_MS) == KEY_DOWN_INTERRUPT) {
            hi_pwm_start(HI_PWM_PORT_PWM2, PWM_LOW_DUTY, PWM_MIDDLE_DUTY);
            break;
        }

        hi_pwm_start(HI_PWM_PORT_PWM2, PWM_LOW_DUTY, PWM_MIDDLE_DUTY);      
        hi_pwm_start(HI_PWM_PORT_PWM3, PWM_SMALL_DUTY, PWM_MIDDLE_DUTY);

        if (delay_and_check_key_interrupt(DELAY_500_MS) == KEY_DOWN_INTERRUPT) {
            hi_pwm_start(HI_PWM_PORT_PWM3, PWM_LOW_DUTY, PWM_MIDDLE_DUTY);
            break;
        }
        hi_pwm_start(HI_PWM_PORT_PWM3, PWM_LOW_DUTY, PWM_MIDDLE_DUTY);
        hi_sleep(SLEEP_1_MS);
    }
}

/*Red, yellow and green lights up in turn every 0.25 seconds.*/
hi_void cycle_for_quarter_second(hi_void)
{
    while (1) {
        hi_pwm_start(HI_PWM_PORT_PWM1, PWM_SMALL_DUTY, PWM_MIDDLE_DUTY);

        if (delay_and_check_key_interrupt(DELAY_250_MS) == KEY_DOWN_INTERRUPT) {
            hi_pwm_start(HI_PWM_PORT_PWM1, PWM_LOW_DUTY, PWM_MIDDLE_DUTY);
            break;
        }

        hi_pwm_start(HI_PWM_PORT_PWM1, PWM_LOW_DUTY, PWM_MIDDLE_DUTY);       
        hi_pwm_start(HI_PWM_PORT_PWM2, PWM_SMALL_DUTY, PWM_MIDDLE_DUTY);

        if (delay_and_check_key_interrupt(DELAY_250_MS) == KEY_DOWN_INTERRUPT) {
            hi_pwm_start(HI_PWM_PORT_PWM2, PWM_LOW_DUTY, PWM_MIDDLE_DUTY);
            break;
        }

        hi_pwm_start(HI_PWM_PORT_PWM2, PWM_LOW_DUTY, PWM_MIDDLE_DUTY);    
        hi_pwm_start(HI_PWM_PORT_PWM3, PWM_SMALL_DUTY, PWM_MIDDLE_DUTY);

        if (delay_and_check_key_interrupt(DELAY_250_MS) == KEY_DOWN_INTERRUPT) {
            hi_pwm_start(HI_PWM_PORT_PWM3, PWM_LOW_DUTY, PWM_MIDDLE_DUTY);
            break;
        }
        hi_pwm_start(HI_PWM_PORT_PWM3, PWM_LOW_DUTY, PWM_MIDDLE_DUTY);
        hi_sleep(SLEEP_1_MS);
    }   
}

/*
mode:
2.Colorful light

type:
1.period by 1s
2.period by 0.5s
3.period by 0.25s
Mode 2: dazzle color mode, each click S2, different dazzle color types,
	Type 1: red, yellow and green lights turn on every 1 second
	Type 2: red, yellow and green light alternately every 0.5 seconds
	Type 3: red, yellow and green light alternately every 0.25 seconds.
*/
hi_void colorful_light_sample(hi_void)
{
    hi_u8 current_type = g_current_type;
    hi_u8 current_mode = g_current_mode;

    while (1) {
        switch (g_current_type) {
            case CYCLE_FOR_ONE_SECOND:
                 cycle_for_one_second();
                 break;
            case CYCLE_FOR_HALF_SECOND:
                 cycle_for_half_second();
                 break;
            case CYCLE_FOR_QUARATER_SECOND:
                 cycle_for_quarter_second();
                 break;  
            default:
                 break;       
        }

        if ((current_mode != g_current_mode) || (current_type != g_current_type)) {
            break;
        }
        hi_sleep(SLEEP_1_MS);        
    }
}

/*The duty cycle of all PWM is 1 and all lights are off*/
hi_void all_light_out(hi_void)
{
    hi_pwm_start(HI_PWM_PORT_PWM1, PWM_LOW_DUTY, PWM_MIDDLE_DUTY);
    hi_pwm_start(HI_PWM_PORT_PWM2, PWM_LOW_DUTY, PWM_MIDDLE_DUTY);
    hi_pwm_start(HI_PWM_PORT_PWM3, PWM_LOW_DUTY, PWM_MIDDLE_DUTY);
}

/*Red from dark to brightest*/
hi_void red_light_dark_to_bright(hi_void)
{
    printf("red_light_dark_to_bright 334 g_current_type = %d\n",g_current_type);
    hi_u8 current_type = g_current_type;
    hi_u8 current_mode = g_current_mode;
    all_light_out();
    while (1) {
        for (hi_s32 i = 1; i < 3000; i=i+i) {
            hi_pwm_start(HI_PWM_PORT_PWM1, i, PWM_MIDDLE_DUTY);
            if ((current_mode != g_current_mode) || (current_type != g_current_type)) {
                return HI_NULL;
            }
            hi_sleep((SLEEP_6_S-i+1)/SLEEP_30_MS);
        }
    }
}

/*Green light from dark to brightest*/
hi_void green_light_dark_to_bright(hi_void)
{
    hi_u8 current_type = g_current_type;
    hi_u8 current_mode = g_current_mode;
    all_light_out();
    while (1) {
        for (hi_s32 i =1; i <3000; i =i+i) {
            hi_pwm_start(HI_PWM_PORT_PWM2, i, PWM_MIDDLE_DUTY);
            if ((current_mode != g_current_mode) || (current_type != g_current_type)) {
                return HI_NULL;
            }
            hi_sleep((SLEEP_6_S-i+1)/SLEEP_30_MS);
        }
    }
}

/*Blue light from dark to brightest*/
hi_void blue_light_dark_to_bright(hi_void)
{
    hi_u8 current_type = g_current_type;
    hi_u8 current_mode = g_current_mode;
    all_light_out();
    while (1) {
        for (hi_s32 i =1; i <3000; i =i+i) {
            if ((current_mode != g_current_mode) || (current_type != g_current_type)) {
                return HI_NULL;
            }
            hi_pwm_start(HI_PWM_PORT_PWM3, i, PWM_MIDDLE_DUTY);

            hi_sleep((SLEEP_6_S-i+1)/SLEEP_30_MS);
        }
    }
}

/*Purple light from dark to brightest*/
hi_void purple_light_dark_to_bright(hi_void)
{
    hi_u8 current_type = g_current_type;
    hi_u8 current_mode = g_current_mode;
    all_light_out();
    while (1) {
        for (hi_s32 i = 0; i < 36; i++){
            hi_pwm_start(HI_PWM_PORT_PWM1, 2000+(i*500), PWM_MIDDLE_DUTY);
            hi_pwm_start(HI_PWM_PORT_PWM2, i, PWM_MIDDLE_DUTY);
            hi_pwm_start(HI_PWM_PORT_PWM3, 2000+(i*500), PWM_MIDDLE_DUTY);
            if ((current_mode != g_current_mode) || (current_type != g_current_type)) {
                return HI_NULL;
            }
            hi_sleep(SLEEP_50_MS);
        }
    }
}

/*The white light changes from dark to full light.*/
hi_void all_light_dark_to_bright(hi_void)
{
    hi_u8 current_type = g_current_type;
    hi_u8 current_mode = g_current_mode;
    all_light_out();
    while (1) {
        for(hi_s32 i = 1; i < 3000; i=i+i){
            hi_pwm_start(HI_PWM_PORT_PWM1, i, PWM_MIDDLE_DUTY);
            hi_pwm_start(HI_PWM_PORT_PWM2, i, PWM_MIDDLE_DUTY);
            hi_pwm_start(HI_PWM_PORT_PWM3, i, PWM_MIDDLE_DUTY);
            if ((current_mode != g_current_mode) || (current_type != g_current_type)) {
                return HI_NULL;
            }
            hi_sleep((SLEEP_6_S-i+1)/SLEEP_30_MS);
        }
    }
}

/*
mode:
3.pwm control
type:
1.red
2.yellow
3.green
4.purple
5.all
Mode 3: PWM stepless dimming mode, each press S2, different dimming types
	Type 1: red from dark to brightest
	Type 2: yellow light from dark to brightest
	Type 3: green light from dark to brightest
	Type 4: purple from dark to brightest
	Type 5: from dark to full light.
*/

hi_void pwm_control_sample(hi_void)
{
    hi_u8 current_type = g_current_type; 
    hi_u8 current_mode = g_current_mode;
    printf("pwm_control_sample\n");
    all_light_out();
    while (1) {
        switch(g_current_type) {
            case PWM_CONTROL_RED:
                red_light_dark_to_bright();
                break;
            case PWM_CONTROL_YELLOW:
                green_light_dark_to_bright();
                break;
            case PWM_CONTROL_GREEN:
                blue_light_dark_to_bright();         
                break;
            case PWM_CONTROL_PURPLE:
                purple_light_dark_to_bright();
                break;
            case PWM_CONTROL_ALL:
                all_light_dark_to_bright();
                break;
            default:
                break;
        }

        if ((current_mode != g_current_mode) || (current_type != g_current_type)) {
            break;
        }
        hi_sleep(SLEEP_20_MS);
    }
}

/*
Use the key to control the brightness of the white light
*/
hi_void brightness_control_sample(hi_void)
{
    hi_u8 current_type = g_current_type; 
    hi_u8 current_mode = g_current_mode;
    all_light_out();
    while (1) {
        switch (g_current_type) {
            case BRIGHTNESS_LOW:
                hi_pwm_start(HI_PWM_PORT_PWM1, PWM_SLOW_DUTY, PWM_MIDDLE_DUTY);
                hi_pwm_start(HI_PWM_PORT_PWM2, PWM_SLOW_DUTY, PWM_MIDDLE_DUTY);
                hi_pwm_start(HI_PWM_PORT_PWM3, PWM_SLOW_DUTY, PWM_MIDDLE_DUTY);
                break;
            case BRIGHTNESS_MIDDLE:
                hi_pwm_start(HI_PWM_PORT_PWM1, PWM_LITTLE_DUTY, PWM_MIDDLE_DUTY);
                hi_pwm_start(HI_PWM_PORT_PWM2, PWM_LITTLE_DUTY, PWM_MIDDLE_DUTY);
                hi_pwm_start(HI_PWM_PORT_PWM3, PWM_LITTLE_DUTY, PWM_MIDDLE_DUTY);
                break;
            case BRIGHTNESS_HIGH:
                hi_pwm_start(HI_PWM_PORT_PWM1, PWM_MIDDLE_DUTY, PWM_MIDDLE_DUTY);
                hi_pwm_start(HI_PWM_PORT_PWM2, PWM_MIDDLE_DUTY, PWM_MIDDLE_DUTY);
                hi_pwm_start(HI_PWM_PORT_PWM3, PWM_MIDDLE_DUTY, PWM_MIDDLE_DUTY);
                break;
            default:
                break;
        }

        if ((current_mode != g_current_mode) || (current_type != g_current_type)) {
            break;
        }
        hi_sleep(SLEEP_10_MS);
    }
}
/*
mode:5.Human detect
Mode 5: human body infrared detection mode (set a nice light brightness)
Someone is near, the light is on, no one is in the detection area, the light is off
*/
hi_void human_detect_sample(hi_void)
{
    hi_u8 current_mode = g_current_mode;
    
    printf("human_detect_sample start\n");

    hi_io_set_func(HI_IO_NAME_GPIO_7, HI_IO_FUNC_GPIO_7_GPIO);
    hi_gpio_set_dir(HI_GPIO_IDX_7,HI_GPIO_DIR_IN);
    hi_gpio_value gpio_val = HI_GPIO_VALUE1;

    while (1) {
        /*use adc channal_0*/
        // g_someone_walking_flag = get_human_status();
        /*use gpio7*/
        hi_gpio_get_input_val(HI_GPIO_IDX_7, &gpio_val);
        if (gpio_val == HI_GPIO_VALUE1) {
            g_someone_walking_flag =  OLED_FALG_ON;
        } else {
            g_someone_walking_flag =  OLED_FALG_OFF;
        }

        if (g_someone_walking_flag == OLED_FALG_ON) {
            
            hi_pwm_start(HI_PWM_PORT_PWM1, PWM_SMALL_DUTY, PWM_MIDDLE_DUTY);
            hi_pwm_start(HI_PWM_PORT_PWM2, PWM_SMALL_DUTY, PWM_MIDDLE_DUTY);
            hi_pwm_start(HI_PWM_PORT_PWM3, PWM_SMALL_DUTY, PWM_MIDDLE_DUTY);
        } else if (g_someone_walking_flag == OLED_FALG_OFF) {
           
            hi_pwm_start(HI_PWM_PORT_PWM1, PWM_LOW_DUTY, PWM_MIDDLE_DUTY);
            hi_pwm_start(HI_PWM_PORT_PWM2, PWM_LOW_DUTY, PWM_MIDDLE_DUTY);
            hi_pwm_start(HI_PWM_PORT_PWM3, PWM_LOW_DUTY, PWM_MIDDLE_DUTY);
        }

        if (current_mode != g_current_mode) {
            hi_pwm_start(HI_PWM_PORT_PWM1, PWM_LOW_DUTY, PWM_MIDDLE_DUTY);
            hi_pwm_start(HI_PWM_PORT_PWM2, PWM_LOW_DUTY, PWM_MIDDLE_DUTY);
            hi_pwm_start(HI_PWM_PORT_PWM3, PWM_LOW_DUTY, PWM_MIDDLE_DUTY);
            break;
        }
        hi_sleep(SLEEP_1_MS);
    }
}

/*
mode:6.Light detect
Mode 6: photosensitive detection mode
No light on, light off
*/
hi_void light_detect_sample(hi_void)
{
    hi_u8 current_mode = g_current_mode;
    while (1) {
        g_with_light_flag = get_light_status();

        if (g_with_light_flag == OLED_FALG_ON) {

            hi_pwm_start(HI_PWM_PORT_PWM1, PWM_SMALL_DUTY, PWM_MIDDLE_DUTY);
            hi_pwm_start(HI_PWM_PORT_PWM2, PWM_SMALL_DUTY, PWM_MIDDLE_DUTY);
            hi_pwm_start(HI_PWM_PORT_PWM3, PWM_SMALL_DUTY, PWM_MIDDLE_DUTY);
        } else if (g_with_light_flag == OLED_FALG_OFF) {
 
            hi_pwm_start(HI_PWM_PORT_PWM1, PWM_LOW_DUTY, PWM_MIDDLE_DUTY);
            hi_pwm_start(HI_PWM_PORT_PWM2, PWM_LOW_DUTY, PWM_MIDDLE_DUTY);
            hi_pwm_start(HI_PWM_PORT_PWM3, PWM_LOW_DUTY, PWM_MIDDLE_DUTY);
        }

        if (current_mode != g_current_mode) {
            hi_pwm_start(HI_PWM_PORT_PWM1, PWM_LOW_DUTY, PWM_MIDDLE_DUTY);
            hi_pwm_start(HI_PWM_PORT_PWM2, PWM_LOW_DUTY, PWM_MIDDLE_DUTY);
            hi_pwm_start(HI_PWM_PORT_PWM3, PWM_LOW_DUTY, PWM_MIDDLE_DUTY);
            break;
        }
        hi_sleep(SLEEP_1_MS);
    }
}

/*
mode:7.Union detect
Mode 7: human infrared + photosensitive joint detection mode.
No one has light = > the light doesn't work
No one without light = > the light doesn't work
Someone has light = > the light doesn't work
Someone without light = > light on
g_ with_ light_ flag = OLED_ FALG_ On: no light (night), OLED_ FALG_ Off with light (day)
g_ someone_ walking_ flag = OLED_ FALG_ On, OLED_ FALG_ Off (unmanned)
*/
hi_void union_detect_sample(hi_void)
{
    hi_u8 current_mode = g_current_mode;

    hi_io_set_func(HI_IO_NAME_GPIO_7, HI_IO_FUNC_GPIO_7_GPIO);
    hi_gpio_set_dir(HI_GPIO_IDX_7,HI_GPIO_DIR_IN);
    hi_gpio_value gpio_val = HI_GPIO_VALUE1;

    while (1) {
        hi_gpio_get_input_val(HI_GPIO_IDX_7, &gpio_val);

        if(gpio_val == HI_GPIO_VALUE1) {
            g_someone_walking_flag =  OLED_FALG_ON;
        } else {
            g_someone_walking_flag =  OLED_FALG_OFF;
        }
        
        g_with_light_flag = get_light_status();

        if (((g_someone_walking_flag == OLED_FALG_ON)&&(g_with_light_flag == OLED_FALG_OFF)) ||(g_someone_walking_flag == OLED_FALG_OFF)) {
            hi_pwm_start(HI_PWM_PORT_PWM1, PWM_LOW_DUTY, PWM_MIDDLE_DUTY);
            hi_pwm_start(HI_PWM_PORT_PWM2, PWM_LOW_DUTY, PWM_MIDDLE_DUTY);
            hi_pwm_start(HI_PWM_PORT_PWM3, PWM_LOW_DUTY, PWM_MIDDLE_DUTY);
        } else if ((g_with_light_flag == OLED_FALG_ON) && (g_someone_walking_flag == OLED_FALG_ON)) {
            hi_pwm_start(HI_PWM_PORT_PWM1, PWM_SMALL_DUTY, PWM_MIDDLE_DUTY);
            hi_pwm_start(HI_PWM_PORT_PWM2, PWM_SMALL_DUTY, PWM_MIDDLE_DUTY);
            hi_pwm_start(HI_PWM_PORT_PWM3, PWM_SMALL_DUTY, PWM_MIDDLE_DUTY);
        }

        if (current_mode != g_current_mode) {
            hi_pwm_start(HI_PWM_PORT_PWM1, PWM_LOW_DUTY, PWM_MIDDLE_DUTY);
            break; 
        }
        hi_sleep(SLEEP_1_MS);
    }
}

/*
mode:8.Creturn
Mode 8: return to the main interface
*/
hi_void return_main_enum_sample(hi_void)
{
    hi_u8 current_type = g_current_type; 
    hi_u8 current_mode = g_current_mode;
    while (1) {       
        switch (g_current_type) {
            case KEY_UP:
                 break;
            case KEY_DOWN:
                 return;
            default:
                 break;
        }

        if (current_mode != g_current_mode) {
            break;
        }
        hi_sleep(SLEEP_1_MS);
    }
}
/*LED light control on gpio9*/
hi_void gpio9_led_light_func(hi_viod) 
{
    g_gpio9_tick++;
    if (g_gpio9_tick%2 == 0) {
         printf("led off\r\n");
        hispark_board_test(HI_GPIO_VALUE1);
    } else {
        printf("led on\r\n");
        hispark_board_test(HI_GPIO_VALUE0);
    } 
}
/*
Each time key 5 is pressed, an interrupt is generated and the function is called
Used to adjust the mode
*/
hi_void gpio5_isr_func_mode(hi_void)
{
    hi_u32 tick_interval =0;
    hi_u32 current_gpio5_tick =0; 

    current_gpio5_tick = hi_get_tick();
    tick_interval = current_gpio5_tick - g_gpio5_tick;

    if (tick_interval < KEY_INTERRUPT_PROTECT_TIME) {  
        return HI_NULL;
    }

    g_gpio5_tick = current_gpio5_tick;
    g_key_down_flag = KEY_GPIO_5;

    printf("gpio5 key down; g_menu_mode = %d, g_current_mode = %d\n",g_menu_mode, g_current_mode);

    // if (g_menu_mode == MAIN_FUNCTION_SELECT_MODE) {
    //     g_menu_select ++;
    //     if (g_menu_select >= MAX_FUNCTION_DEMO_NUM) {
    //         g_menu_select = COLORFUL_LIGHT_MENU;
    //     }      
    //     return HI_ERR_SUCCESS;
    // }

   if (g_menu_mode == SUB_MODE_SELECT_MODE) {
        g_current_mode ++;
        g_current_type = CONTROL_MODE;         
        switch (g_menu_select) {
            case COLORFUL_LIGHT_MENU:
                 if (g_current_mode >= MAX_COLORFUL_LIGHT_MODE) {
                     g_current_mode = CONTROL_MODE;
                 }
                 break;
            case TRAFFIC_LIGHT_MENU:
                 if (g_current_mode >= MAX_TRAFFIC_LIGHT_MODE) {
                     g_current_mode = CONTROL_MODE;
                 }
                 break;
            case ENVIRONMENT_MENU:
                 if (g_current_mode >= MAX_ENVIRONMENT_MODE) {
                    g_current_mode =CONTROL_MODE;
                 }
                 break;
            case NFC_TEST_MENU:
                 if (g_current_mode >= MAX_NFC_TAG_MODE) {
                     g_current_mode =CONTROL_MODE;
                 }
                 break;     
            default:
                 break;
        }
    }
}

/*
Each time key 7 is pressed, an interrupt is generated and the function is called
Used to adjust the type
*/
hi_void gpio7_isr_func_type(hi_void)
{
    hi_u32 tick_interval;
    hi_u32 current_gpio7_tick; 

    /*Key anti shake*/
    current_gpio7_tick = hi_get_tick();
    tick_interval = current_gpio7_tick - g_gpio7_tick;
    printf("gpio7 start = %d, end = %d end-start = %d\n",g_gpio7_tick,current_gpio7_tick,tick_interval);
    if (tick_interval < KEY_INTERRUPT_PROTECT_TIME) {  
        return HI_NULL;
    }

    g_gpio7_tick = current_gpio7_tick;
    g_key_down_flag = KEY_GPIO_7;


#ifdef CONFIG_COLORFUL_LIGHT
    g_current_type++;
#elif  CONFIG_TRAFFIC_LIGHT
    g_current_type++;
#else
    if (g_gpio7_first_key_dwon != HI_TRUE) {
        g_current_type++;
    }
    g_gpio7_first_key_dwon = HI_FALSE;
#endif

    if (g_menu_select == COLORFUL_LIGHT_MENU) {
        switch (g_current_mode) {
            case CONTROL_MODE:
                 if (g_current_type > MAX_CONTROL_MODE_TYPE) {
                     g_current_type = RED_ON;
                 }
                 break;
            case COLORFUL_LIGHT_MODE:
                 if (g_current_type > MAX_COLORFUL_LIGHT_TYPE) {
                     g_current_type = CYCLE_FOR_ONE_SECOND;
                 }
                 break;
            case PWM_CONTROL_MODE:
                 if (g_current_type > MAX_PWM_CONTROL_TYPE) {
                     g_current_type = PWM_CONTROL_RED;
                 }
                 break;
            case BIRGHTNESS_MODE:
                 if (g_current_type > MAX_BIRGHTNESS_TYPE) {
                     g_current_type = BRIGHTNESS_LOW;
                 }            
                 break;
            case HUMAN_DETECT_MODE:
            case LIGHT_DETECT_MODE:
            case UNION_DETECT_MODE:
            case RETURN_MODE:
                 if (g_current_type > RETURN_TYPE_MODE) {
                     g_current_type = KEY_UP;
                 } 
                 break;
            default:
                 break;
        }
    }

    if (g_menu_select == TRAFFIC_LIGHT_MENU) {
        switch (g_current_mode) {
            case TRAFFIC_CONTROL_MODE:
                 if (g_current_type >= MAX_TRAFFIC_CONTROL_TYPE) {
                     g_current_type = TRAFFIC_NORMAL_TYPE;
                 }
                 break;
            case TRAFFIC_AUTO_MODE:
                 if (g_current_type > MAX_TRAFFIC_AUTO_TYPE) {
                     g_current_type = TRAFFIC_NORMAL_TYPE;
                 }
                 break;
            case TRAFFIC_HUMAN_MODE:
                 if (g_current_type > MAX_TRAFFIC_HUMAN_TYPE) {
                     g_current_type = TRAFFIC_NORMAL_TYPE;
                 }
                 break;
            case TRAFFIC_RETURN_MODE:
                 if (g_current_type > RETURN_TYPE_MODE) {
                     g_current_type = KEY_UP;
                 } 
                 break;
            default:
                 break;
        }
    }

    printf("gpio7 current mode is %d, type is %d\n",g_current_mode, g_current_type);
}
/*	@berf gpio 8 External interrupt*/
hi_void gpio8_interrupt(hi_void *param)
{

    hi_u32 tick_interval =0;
    hi_u32 current_gpio8_tick =0; 

    hi_unref_param(param);
    current_gpio8_tick = hi_get_tick();
    tick_interval = current_gpio8_tick - g_gpio8_tick;
    printf("gpio8 interrupt \r\n");

    if (tick_interval < KEY_INTERRUPT_PROTECT_TIME) {  
        printf("gpio8 interrupt return  \r\n");
        return HI_NULL;
    }
    g_gpio8_current_type++;

    if (g_gpio8_current_type %2 == 0) {
        printf("beep off\r\n", g_gpio8_current_type);
        oc_beep_status = BEEP_OFF;
    } else {
        printf("beep on %d\r\n", g_gpio8_current_type);
        oc_beep_status = BEEP_ON;
    }

    if (g_gpio8_current_type >= 255)  {
        g_gpio8_current_type=0;
    }
}
/*
S2 (gpio5) is the mode key and S1 (gpio7) is the type key.
1. Three color light control mode: each time you press type S1 button, you can switch between the three states of green light, yellow light and red light
2. Dazzle mode, each click S2, different dazzle type
3. PWM stepless dimming mode, each press S2, different dimming types
4. Human body infrared detection mode (set a nice light brightness)
5. Photosensitive detection mode
6. Human body infrared + photosensitive joint detection mode.
7. Return mode
Create two key interrupts and call the corresponding callback function after the key response
*/
hi_void app_multi_sample_demo(hi_void)
{
    hi_u32 ret =HI_ERR_SUCCESS;

    (hi_void)hi_gpio_init();
    g_gpio5_tick = hi_get_tick(); 
    ret = hi_gpio_register_isr_function(HI_GPIO_IDX_5, HI_INT_TYPE_EDGE,HI_GPIO_EDGE_FALL_LEVEL_LOW, get_gpio5_voltage, HI_NULL);
    if (ret == HI_ERR_SUCCESS) {
        printf(" register gpio 5\r\n");
    }

    g_gpio8_tick = hi_get_tick();
    ret = hi_gpio_register_isr_function(HI_GPIO_IDX_8, HI_INT_TYPE_EDGE,HI_GPIO_EDGE_FALL_LEVEL_LOW, gpio8_interrupt, HI_NULL);
    if (ret == HI_ERR_SUCCESS) {
        printf(" register gpio8\r\n");
    }
}