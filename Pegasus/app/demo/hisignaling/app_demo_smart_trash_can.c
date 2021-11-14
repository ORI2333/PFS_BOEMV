
/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: app led demo 
 * Author: Hisilicon
 * Create: 2020-10-26
 */
#include <app_demo_io_gpio.h>
#include <hi_adc.h>
#include <hi_time.h>
#include <hi_wifi_api.h>
#include <smart_can_driver_log.h>
#include <app_demo_uart.h>

#define  TRASH_DEMO_TASK_STAK_SIZE    (1024*4)
#define  TRASH_DEMO_TASK_PRIORITY     (26)
#define  ADC_TEST_LENGTH              (20)
#define  VLT_MIN                      (100)

hi_u32  g_trash_demo_task_id =0;
hi_u32  g_num = 0;
hi_bool g_trash_can_is_open = HI_FALSE;
hi_u16  g_adc_buf_trash[ADC_TEST_LENGTH] = { 0 };
hi_u8   transfer_buf[1] = {0};
hi_u8   trash_can_communicate = HI_FALSE;
/*
步骤：
    1、GPIO复用关系
    2、设置GPIO的方向
    3、设置GPIO的输出值
*/
hi_void gpio_control_trash(hi_io_name name, hi_u8 val, hi_gpio_idx idx, hi_gpio_dir dir, hi_gpio_value value)
{
    hi_io_set_func(name, val);
    hi_gpio_set_dir(idx, dir);
    if (HI_GPIO_DIR_OUT == dir) {  
        hi_gpio_set_ouput_val(idx, value);
    }
}
// 正转
hi_void set_engine_angle_forward_rotation(hi_s32 duty)
{
    hi_u32 count = duty/20;

    for(int i=1; i <= count; i++) {
        hi_gpio_set_ouput_val(HI_GPIO_IDX_10,HI_GPIO_VALUE1);
        hi_udelay(duty - i*20);  
        hi_gpio_set_ouput_val(HI_GPIO_IDX_10,HI_GPIO_VALUE0);
        hi_udelay(20000-(duty - i*20));  
    }  
}
//反转
hi_void set_engine_angle_transfer( hi_s32 duty)
{
    hi_u32 count = duty/20;

    for(int i=1; i <= count; i++) {
        hi_gpio_set_ouput_val(HI_GPIO_IDX_10,HI_GPIO_VALUE1);
        hi_udelay(i*20);  
        hi_gpio_set_ouput_val(HI_GPIO_IDX_10,HI_GPIO_VALUE0);
        hi_udelay(20000 - i*20);  
    } 
}

// 垃圾桶开
hi_void trash_can_open(hi_void)
{   
    SMART_CAN_LOG_INFO("trash_can_open");
    set_engine_angle(1000); 
    g_trash_can_is_open = HI_TRUE;
}

// 垃圾箱初始化
hi_void smart_trash_can_init(hi_void)
{
    // //舵机初始化 GPIO6
    // gpio_control_trash(HI_IO_NAME_GPIO_10,HI_IO_FUNC_GPIO_10_GPIO,HI_GPIO_IDX_10,HI_GPIO_DIR_OUT,HI_GPIO_VALUE0);
    // //绿色LED灯初始化 GPIO8
    // gpio_control_trash(HI_IO_NAME_GPIO_8,HI_IO_FUNC_GPIO_8_GPIO,HI_GPIO_IDX_8,HI_GPIO_DIR_OUT,HI_GPIO_VALUE0);
}

hi_void smart_can_and_led_control(hi_void)
{
    /*3516DV300控制*/
    SMART_CAN_LOG_INFO("3516dv300 control Hi3861 Trash can open");
    hi_gpio_set_ouput_val(HI_GPIO_IDX_8, HI_GPIO_VALUE1); // 开灯
    /*开盖*/
    set_engine_angle_forward_rotation(SMART_CAN_STEERING_ENGINE_ROTATION_ANGLE);
    /*关盖*/
    set_engine_angle_transfer(SMART_CAN_STEERING_ENGINE_ROTATION_ANGLE);
    hi_gpio_set_ouput_val(HI_GPIO_IDX_8, HI_GPIO_VALUE0);/*关灯*/
}

hi_void *app_demo_smart_trash_can(hi_void *param)
{
    hi_gpio_init();
    smart_trash_can_init();
    while (1) {
        /*led控制*/
        smart_can_and_led_control();
        hi_sleep(SMART_CAN_TASK_FREE_TIME);
    }
}

hi_void app_smart_trash_can_task(hi_void)
{
    hi_u32 ret = 0;
    hi_task_attr attr = {0};

    attr.stack_size = TRASH_DEMO_TASK_STAK_SIZE;
    attr.task_prio  = TRASH_DEMO_TASK_PRIORITY;
    attr.task_name  = (hi_char*)"app_smart_trash_can_task";
    ret = hi_task_create(&g_trash_demo_task_id, &attr, app_demo_smart_trash_can, HI_NULL);
    if (ret != HI_ERR_SUCCESS) {
        SMART_CAN_LOG_ERROR("Failed to create app_demo_smart_trash_can");
    }
}