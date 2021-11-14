/*
 * Copyright (c) 2020 HiHope Community.
 * Description: mq2 demo
 * Author: HiSpark Product Team.
 * Create: 2020-5-20
 */
#include <math.h>
#include <hi_early_debug.h>
#include <hi_task.h>
#include <hi_time.h>
#include <hi_adc.h>
#include <hi_stdlib.h>
#include <hi_watchdog.h>
#include <ssd1306_oled.h>
#include <hi_pwm.h>
#include <hi_io.h>
#include <hi_gpio.h>
#define MQ2_DEMO_TASK_STAK_SIZE     (1024*8)
#define MQ2_DEMO_TASK_PRIORITY      (25)
#define ADC_TEST_LENGTH             (20)
#define VLT_MIN                     (100)
#define RS_R0_PROPORTION_300        ((hi_float)0.27)   
#define RS_R0_PROPORTION_400        ((hi_float)0.23)  
#define RS_R0_PROPORTION_500        ((hi_float)0.195)       
#define RS_R0_PROPORTION_600        ((hi_float)0.175)
#define RS_R0_PROPORTION_700        ((hi_float)0.16)
#define RS_R0_PROPORTION_800        ((hi_float)0.15)
#define RS_R0_PROPORTION_900        ((hi_float)0.14)     
#define RS_R0_PROPORTION_1000       ((hi_float)0.13)
#define RS_R0_PROPORTION_2000       ((hi_float)0.078)
#define RS_R0_PROPORTION_3000       ((hi_float)0.058)
#define RS_R0_PROPORTION_4000       ((hi_float)0.049)
#define RS_R0_PROPORTION_5000       ((hi_float)0.042)
#define RS_R0_PROPORTION_6000       ((hi_float)0.037)
#define RS_R0_PROPORTION_7000       ((hi_float)0.032)
#define RS_R0_PROPORTION_8000       ((hi_float)0.029)
#define RS_R0_PROPORTION_9000       ((hi_float)0.027)
#define RS_R0_PROPORTION_10000      ((hi_float)0.025)

#define CAL_PPM                     (25)          // ppm value in calibration environment
#define RL			                (1)		      // RL resistance
#define MQ2_RATIO                   (1111)

static float R0 = 22;
hi_u16 g_mq2_adc_buf[ADC_TEST_LENGTH] = { 0 };
hi_u8 g_mq2_demo_task_id =0;
hi_u8 boot_time_ms =0;
hi_double g_combustible_gas_value = 0;
hi_u8 g_ahu20_gas_buff[6] = {0};
extern hi_u8  *flaot_to_string(hi_double d, hi_u8 *str );
/*-----------------------------------
ppm: the concentration of combustible gas.
VRL: voltage output value.
Rs: resistance value of the device under different gases and concentrations.
R0: resistance value of the device in clean air.
RL: load resistance.
------------------------------------*/
hi_void mq2_ppm_calibration(hi_float RS)
{
    R0 = RS / pow(CAL_PPM / 613.9f, 1 / -2.074f);
    printf("R0:%f\r\n", R0);
}
 
/**
	@berf MQ2 sensor data processing
	@param hi_float voltage: Incoming voltage value parameter
	@return Return ppm combustible gas concentration after conversion
*/ 
hi_float mq2_get_ppm(hi_float voltage)
{
    hi_double ppm =0;
    static hi_u8 flag = 1;
    static hi_u8 count = 0;

    hi_float RS = (5 - voltage) / voltage * RL; 
    // printf("RS/R0:%03f\r\t", RS/R0);
    memset(&ppm, 0x0, sizeof(ppm));
    // memset(g_ahu20_gas_buff, 0x0, sizeof(g_ahu20_gas_buff));
    if (flag == 1) {
        flag = 0;
        hi_pwm_init(HI_PWM_PORT_PWM0);
        hi_pwm_set_clock(PWM_CLK_160M);
        hi_io_set_func(HI_IO_NAME_GPIO_9, HI_IO_FUNC_GPIO_9_PWM0_OUT);
        hi_gpio_set_dir(HI_IO_NAME_GPIO_9, HI_GPIO_DIR_OUT);
    }
    ppm = pow(11.5428*22*voltage/(5-voltage), 1.0/0.6549);
    if(ppm < 300) {
        ppm = 0;
    }
    // printf("ppm:%03f\r\n",ppm);
    if (ppm > 3000) {
        if (count < 1) {
            count++;
            hi_pwm_start(HI_PWM_PORT_PWM0, PWM_DUTY, PWM_FULL_DUTY);
        } else {
            count = 0;
            hi_pwm_start(HI_PWM_PORT_PWM0, PWM_LOW_DUTY, PWM_FULL_DUTY);
        }   
    } else {
        count = 0;
        hi_pwm_start(HI_PWM_PORT_PWM0, PWM_LOW_DUTY, PWM_FULL_DUTY);
    }
    flaot_to_string(ppm, g_ahu20_gas_buff);
    return  ppm;
}

/** 
	@berf mq2 sesor get data from adc change
	@berf ADC reads analog value of combustible gas sensor and converts it into voltage value
*/
hi_void mq2_get_data(hi_void)
{
    hi_u32 ret =0;
    hi_u8 i =0;
    hi_u16 data =0;  /* 10 */
    hi_float voltage;

    ret = hi_adc_read(HI_ADC_CHANNEL_5, &data, HI_ADC_EQU_MODEL_4, HI_ADC_CUR_BAIS_DEFAULT, 0xFF);//ADC_Channal_2 (gpio5) automatic recognition pattern cn comment:4 Sub average algorithm mode CNend */    
    if (ret != HI_ERR_SUCCESS) {
        printf("ADC Read Fail\n");
        return HI_NULL;
    }
    voltage = (hi_float)(data * 1.8 * 4 / 4096.0);  /* vlt * 1.8* 4 / 4096.0 is to convert codeword to voltage */
    // printf("ADC data:%d\n", data);     
    g_combustible_gas_value = mq2_get_ppm(voltage);
}

/* mq2 demo :get adc change data*/
hi_void *app_demo_mq2(hi_void *param)
{
    hi_u32 ret =0;
    hi_u16 data =0;  /* 10 */
    float voltage;
    float vlt_max = 0;
    float vlt_min = VLT_MIN;
    hi_double ppm =0;
    hi_float aht_ratio =0;
    static hi_float ratio_d = 0.26;
    static hi_float ratio_d_s =0.28;
    printf("ADC Test Start\n");
    while (1) {
        ret = hi_adc_read(HI_ADC_CHANNEL_5, &data, HI_ADC_EQU_MODEL_4, HI_ADC_CUR_BAIS_DEFAULT, 0xFF);//ADC_Channal_2 (gpio5) automatic recognition pattern cn comment:4 Sub average algorithm mode CNend */    
        if (ret != HI_ERR_SUCCESS) {
            printf("ADC Read Fail\n");
            return HI_NULL;
        }
        voltage = (float)(data * 1.8 * 4 / 4096.0);  /* vlt * 1.8* 4 / 4096.0 is to convert codeword to voltage */
        vlt_max = (voltage > vlt_max) ? voltage : vlt_max;
        vlt_min = (voltage < vlt_min) ? voltage : vlt_min;
        hi_float RS = (5 - voltage) / voltage * RL; 
        printf("RS:%03f\r\n", RS);
        printf("R0/RS:%03f\r\n", RS/R0);
        aht_ratio = (hi_float) (RS/R0);
        if (aht_ratio >= 0.025 && aht_ratio <= 0.27) {
            if (aht_ratio > 0.195 && aht_ratio <= 0.27) {
                ppm = 100*pow(11.5428*35.904*voltage/(25.5-1*voltage), 0.6549);              
                printf("ppm_1:%03f\r\n", ppm); 
            } else if(aht_ratio >= 0.175 && aht_ratio <= 0.195) {
                ppm =100+100*pow(11.5428*35.904*voltage/(25.5-1*voltage), 0.6549);              
                printf("ppm_2:%03f\r\n", ppm); 
            } else if (aht_ratio >= 0.160 && aht_ratio <= 0.175) {
                ppm =200+100*pow(11.5428*35.904*voltage/(25.5-1*voltage), 0.6549);              
                printf("ppm_3:%03f\r\n", ppm); 
            } else if(aht_ratio >= 0.150 && aht_ratio < 0.160) {
                ppm =300+100*pow(11.5428*35.904*voltage/(25.5-1*voltage), 0.6549);              
                printf("ppm_4:%03f\r\n", ppm); 
            }else if (aht_ratio >= 0.140 && aht_ratio < 0.150) {
                ppm =400+100*pow(11.5428*35.904*voltage/(25.5-1*voltage), 0.6549);              
                printf("ppm_5:%03f\r\n", ppm); 
            } else if (aht_ratio >= 0.130 && aht_ratio < 0.140) {
                ppm =400+100*pow(11.5428*35.904*voltage/(25.5-1*voltage), 0.6549);              
                printf("ppm_6:%03f\r\n", ppm); 
            }else if (aht_ratio >= 0.078 && aht_ratio < 0.130) {
                ppm =700+100*pow(11.5428*35.904*voltage/(25.5-1*voltage), 0.6549);              
                printf("ppm_7:%03f\r\n", ppm); 
            }
        }
        printf("=======================================\r\n");
        printf("=======================================\r\n");

    }     
              
}
/* create mq2 task */
hi_void app_demo_mq2_task(hi_void)
{
    hi_u32 ret =0;
    hi_task_attr attr ={0};
    attr.stack_size = MQ2_DEMO_TASK_STAK_SIZE;
    attr.task_prio = MQ2_DEMO_TASK_PRIORITY;
    attr.task_name = (hi_char*)"app_demo_mq2";

    hi_task_lock();
    ret = hi_task_create(&g_mq2_demo_task_id, &attr, app_demo_mq2, HI_NULL);
    if (ret != HI_ERR_SUCCESS) {
        printf("Falied to create i2c sht3x demo task!\n");
    }
    hi_task_unlock();
}