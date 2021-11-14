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

#define HX711_TASK_SIZE       (7168)
#define HX711_TASK_PRIO       (30)
hi_u32 hi_hx711_task_1_id = 22;


#define GapValue_1 460.5
#define GapValue_2 460.5

unsigned long Sensor_Read_1(void);
double Get_Sensor_Read_1(void);
unsigned long Sensor_Read_2(void);
double Get_Sensor_Read_2(void);

hi_u32 W_Flag_1;
hi_u32 Weigth_1_Flag = 100;
hi_u32 temp = 0;;


hi_void hi_hx711_task_1(hi_void)
{	
	
	// 1号秤
	hi_io_set_func(HI_IO_NAME_GPIO_11, HI_IO_FUNC_GPIO_11_GPIO);
    hi_gpio_set_dir(HI_GPIO_IDX_11,HI_GPIO_DIR_IN);
    hi_io_set_func(HI_IO_NAME_GPIO_12, HI_IO_FUNC_GPIO_12_GPIO);
    hi_gpio_set_dir(HI_GPIO_IDX_12,HI_GPIO_DIR_OUT);
    hi_gpio_set_ouput_val(HI_IO_NAME_GPIO_12,0);

	// 2号秤
	hi_io_set_func(HI_IO_NAME_GPIO_3, HI_IO_FUNC_GPIO_3_GPIO);
    hi_gpio_set_dir(HI_GPIO_IDX_3,HI_GPIO_DIR_IN);
   	hi_io_set_func(HI_IO_NAME_GPIO_4, HI_IO_FUNC_GPIO_4_GPIO);
  	hi_gpio_set_dir(HI_GPIO_IDX_4,HI_GPIO_DIR_OUT);
    hi_gpio_set_ouput_val(HI_IO_NAME_GPIO_4,0);


    oled_show_str(0,1, "Weigth1 : ",1);
 	oled_show_str(0,5, "Weigth2 : ",1);
 
 	double base_data_1 = 0;
  	double hx711_weight_1 = 0;
	char buf[21];
  	base_data_1 = Get_Sensor_Read_1(); //Get benchmark value
	double base_data_2 = 0,hx711_weight_2 = 0;
    base_data_2 = Get_Sensor_Read_2(); 
	W_Flag_1 = 1;
    while (1)
     {
	
     	hx711_weight_1 = ((Get_Sensor_Read_1() - base_data_1) / GapValue_1) ; //Gain weight
		hx711_weight_2 = ((Get_Sensor_Read_2() - base_data_2) / GapValue_2) ; 
        if(hx711_weight_1 > 0) 
        {
			sprintf_s(buf,20,"%.2f g   ",hx711_weight_1);
			oled_show_str(20,3, buf,1);
        }else
		{
			hx711_weight_1 = -hx711_weight_1;
            sprintf_s(buf,10,"%.2f g   ",hx711_weight_1);
         	oled_show_str(20,3, buf,1);
		}

		if(W_Flag_1 && (hx711_weight_1 > Weigth_1_Flag - 30) ){
		
		W_Flag_1 = 0;
		// 关闭舵机
		hi_gpio_set_ouput_val(HI_GPIO_IDX_2,HI_GPIO_VALUE1);
        hi_udelay(1500); 
        hi_gpio_set_ouput_val(HI_GPIO_IDX_2,HI_GPIO_VALUE0);
        hi_udelay(20000-1500); 
        hi_sleep(100);

		}else if (hx711_weight_1 <= Weigth_1_Flag - 30 )
		{
			W_Flag_1 = 1;
		}
		
		

		 if(hx711_weight_2 > 0) 
        {
           sprintf_s(buf,10,"%.2f g ",hx711_weight_2);
           oled_show_str(20,7, buf,1);

        }else
		{
			hx711_weight_2 = -hx711_weight_2;
			//printf("weight2 :   %.2f\r\n" ,hx711_weight_2);  
          	sprintf_s(buf,10,"%.2f g ",hx711_weight_2);
          	oled_show_str(20,7, buf,1);
		}

		if(hx711_weight_2 > 70.0){
		
		// 关闭舵机
		hi_gpio_set_ouput_val(HI_GPIO_IDX_8,HI_GPIO_VALUE1);
        hi_udelay(1500);  
        hi_gpio_set_ouput_val(HI_GPIO_IDX_8,HI_GPIO_VALUE0);
        hi_udelay(20000-1500); 
        hi_sleep(100);
		}
        hi_sleep(100);  
     }
}



unsigned long Sensor_Read_1(void)
{

	unsigned long value = 0;
	unsigned char i = 0;
	hi_gpio_value input = 0;
	hi_udelay(2);
	//When the clock line is pulled down and idle, the clock line keeps low potential
	hi_gpio_set_ouput_val(HI_IO_NAME_GPIO_12,0);
	hi_udelay(2);	
	hi_gpio_get_input_val(HI_IO_NAME_GPIO_11,&input);
	//Wait for AD conversion to finish
	while(input)
    {
        hi_gpio_get_input_val(HI_IO_NAME_GPIO_11,&input);
    }
	for(i=0;i<24;i++)
	{
		//The clock line is pulled up to send the clock pulse
		hi_gpio_set_ouput_val(HI_IO_NAME_GPIO_12,1);
		// hi_udelay(2);
		
		//Shift left and fill zero right to wait for receiving data
		value = value << 1;
		//Clock line down
		hi_gpio_set_ouput_val(HI_IO_NAME_GPIO_12,0);
		// hi_udelay(2);
		//Read one bit data
        hi_gpio_get_input_val(HI_IO_NAME_GPIO_11,&input);
		if(input){
			value ++;
        }
		
	}
	//The 25th pulse
	hi_gpio_set_ouput_val(HI_IO_NAME_GPIO_12,1);
	hi_udelay(2);
	value = value^0x800000;	
	//25th pulse end
	hi_gpio_set_ouput_val(HI_IO_NAME_GPIO_12,0);	
	hi_udelay(2);	
	return value;
}

unsigned long Sensor_Read_2(void)
{

	unsigned long value = 0;
	unsigned char i = 0;
	hi_gpio_value input = 0;
	hi_udelay(2);
	//When the clock line is pulled down and idle, the clock line keeps low potential
	hi_gpio_set_ouput_val(HI_IO_NAME_GPIO_4,0);
	hi_udelay(2);	
	hi_gpio_get_input_val(HI_IO_NAME_GPIO_3,&input);
	//Wait for AD conversion to finish
	while(input)
    {
        hi_gpio_get_input_val(HI_IO_NAME_GPIO_3,&input);
    }
	for(i=0;i<24;i++)
	{
		//The clock line is pulled up to send the clock pulse
		hi_gpio_set_ouput_val(HI_IO_NAME_GPIO_4,1);
		// hi_udelay(2);
		
		//Shift left and fill zero right to wait for receiving data
		value = value << 1;
		//Clock line down
		hi_gpio_set_ouput_val(HI_IO_NAME_GPIO_4,0);
		// hi_udelay(2);
		//Read one bit data
        hi_gpio_get_input_val(HI_IO_NAME_GPIO_3,&input);
		if(input){
			value ++;
        }
		
	}
	//The 25th pulse
	hi_gpio_set_ouput_val(HI_IO_NAME_GPIO_4,1);
	hi_udelay(2);
	value = value^0x800000;	
	//25th pulse end
	hi_gpio_set_ouput_val(HI_IO_NAME_GPIO_4,0);	
	hi_udelay(2);	
	return value;
}


/*get serson data*/ 
double Get_Sensor_Read_1(void)
{
  	double sum = 0;    // In order to reduce the error, take out 10 values at a time and calculate the average value.
  	for (int i = 0; i < 8; i++) // The more loops, the higher the accuracy, and of course, the more time it takes
    	sum += Sensor_Read_1();  // accumulation
  	return (sum/8); // Calculate the average value for average difference

}

double Get_Sensor_Read_2(void)
{
  	double sum = 0;    // In order to reduce the error, take out 10 values at a time and calculate the average value.
  	for (int i = 0; i < 3; i++) // The more loops, the higher the accuracy, and of course, the more time it takes
    	sum += Sensor_Read_2();  // accumulation
  	return (sum/3); // Calculate the average value for average difference
}



hi_u32 app_demo_hi_hx711_task(hi_void)
{
    hi_u32 ret;
    hi_task_attr hx711_attr = {0};
	/*led_ control_ Parameter configuration of attr structure*/
     hx711_attr.stack_size = HX711_TASK_SIZE;//Task stack memory
     hx711_attr.task_prio = HX711_TASK_PRIO;//The task priority ranges from 0 to 31. Tasks 0 to 10 should not be used. The SDK has been used. The higher the value, the lower the priority
     hx711_attr.task_name = (hi_char*)"hi_hx711_task_1";//Task name
	//Create task
    ret = hi_task_create(&hi_hx711_task_1_id, &hx711_attr, hi_hx711_task_1, HI_NULL);
    if (ret != HI_ERR_SUCCESS) {
        printf("Failed to create hx711 demo\r\n");
        return HI_ERR_FAILURE;
    }
    return HI_ERR_SUCCESS;
}


