#include <hi_gpio.h>
#include <hi_early_debug.h>
#include <hi_io.h>
#include <hi_time.h>
#include <hi_watchdog.h>
#include <hi_task.h>


#define LED_LIGHT_DELAY_1S       (1000*1000)
#define hi_unuse_param(p)           ((p) = (p))
#define LED_CONTROL_TASK_SLEEP_20MS (100)
#define LED_CONTROL_TASK_SIZE       (1024)
#define LED_CONTROL_TASK_PRIO       (29)
hi_u32 g_led_control_id;
hi_gpio_value gpio_val_1;
hi_gpio_value gpio_val_2;

hi_s32 Flage_Feed = 0;
hi_u32 Flage_Zigbee = 1;

extern hi_bool g_light_status;

hi_void Zigbee_Input_Gonfig(void);


hi_void Zigbee_Read(void)
{

    hi_io_set_func(HI_IO_NAME_GPIO_6, HI_IO_FUNC_GPIO_6_GPIO);
    hi_gpio_set_dir(HI_GPIO_IDX_6,HI_GPIO_DIR_IN);
    hi_io_set_func(HI_IO_NAME_GPIO_7, HI_IO_FUNC_GPIO_7_GPIO);
    hi_gpio_set_dir(HI_GPIO_IDX_7,HI_GPIO_DIR_IN);

     gpio_val_1 = HI_GPIO_VALUE1;
     gpio_val_2 = HI_GPIO_VALUE1;

}

/*Method 2*/
hi_void *led_control_demo(hi_void *param)
{
    hi_u32 ret;

    Zigbee_Read();
    hi_unuse_param(param);

    while (1)
     {
        hi_gpio_get_input_val(HI_GPIO_IDX_6, &gpio_val_1);
        
        if ((gpio_val_1 == HI_GPIO_VALUE0) || Flage_Feed ) {

        if( Flage_Zigbee == 1 || Flage_Feed){
         hi_gpio_set_ouput_val(HI_GPIO_IDX_2,HI_GPIO_VALUE1);
        hi_udelay(500);  
        hi_gpio_set_ouput_val(HI_GPIO_IDX_2,HI_GPIO_VALUE0);
        hi_udelay(20000-500); 
        }
        gpio_val_1 = HI_GPIO_VALUE1;
         Flage_Feed = 0;
        
        }

        hi_sleep(LED_CONTROL_TASK_SLEEP_20MS);

        hi_gpio_get_input_val(HI_GPIO_IDX_7, &gpio_val_2);
        if (gpio_val_2 == HI_GPIO_VALUE0) {
         if( Flage_Zigbee == 1 ){
        hi_gpio_set_ouput_val(HI_GPIO_IDX_8,HI_GPIO_VALUE1);
         hi_udelay(500);  
        hi_gpio_set_ouput_val(HI_GPIO_IDX_8,HI_GPIO_VALUE0);
        hi_udelay(20000-500); 
         }
        gpio_val_2 = HI_GPIO_VALUE1;
        }
        
    }
	/*Delete task*/
    ret = hi_task_delete(g_led_control_id);
    if (ret != HI_ERR_SUCCESS) {
        printf("Failed to delete led control demo task\r\n");
    }
}
/*
	@berf Create task：app_demo_led_control_task 
	@param hi_void 
	@return HI_ERR_FAILURE/HI_ERR_SUCCESS ,Task creation failed or succeeded 
*/
hi_u32 app_demo_led_control_task(hi_void)
{
    hi_u32 ret;
    hi_task_attr led_control_attr = {0};
	/*led_ control_ Parameter configuration of attr structure*/
    led_control_attr.stack_size = LED_CONTROL_TASK_SIZE;//Task stack memory
    led_control_attr.task_prio = LED_CONTROL_TASK_PRIO;//The task priority ranges from 0 to 31. Tasks 0 to 10 should not be used. The SDK has been used. The higher the value, the lower the priority
    led_control_attr.task_name = (hi_char*)"app demo led control";//Task name
	//Create task
    ret = hi_task_create(&g_led_control_id, &led_control_attr, led_control_demo, HI_NULL);
    if (ret != HI_ERR_SUCCESS) {
        printf("Failed to create led control demo\r\n");
        return HI_ERR_FAILURE;
    }
    return HI_ERR_SUCCESS;
}


