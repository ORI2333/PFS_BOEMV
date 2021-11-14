// #include <hi_early_debug.h>
// #include <hi_io.h>
// #include <hi_gpio.h>
// #include <hi_types_base.h>
// #include <hi_time.h>
// #include <hi_task.h>

// #define ENGINE_MOVEMENT_TIME   (1000*1000)
// #define ENGINE_TASK_FREE_TIME  (20)
// hi_u32 engine_id;
// extern hi_bool engine_stop_flag;
// hi_void engine_io_init(hi_void)
// {
//     hi_io_set_func(HI_IO_NAME_GPIO_2, HI_IO_FUNC_GPIO_2_GPIO);
//     hi_gpio_set_dir(HI_GPIO_IDX_2, HI_GPIO_DIR_OUT);
// }
// /*正转*/
// hi_void set_engine_angle(hi_s32 duty)
// {
//     hi_s32 count = duty/100;
//     for (int i = 1; i <= count; i++) {
//         hi_gpio_set_ouput_val(HI_GPIO_IDX_2, HI_GPIO_VALUE1);
//         hi_udelay(i*100);
//         hi_gpio_set_ouput_val(HI_GPIO_IDX_2, HI_GPIO_VALUE0);
//         hi_udelay(20000 - i*100);
//     }
// }
// /*反转*/
// hi_void set_engine_angle_reversal(hi_s32 duty)
// {
//     hi_s32 count = duty/100;
//     for (int i = 1; i <= count; i++) {
//         hi_gpio_set_ouput_val(HI_GPIO_IDX_2, HI_GPIO_VALUE1);
//         hi_udelay(duty- i*100);
//         hi_gpio_set_ouput_val(HI_GPIO_IDX_2, HI_GPIO_VALUE0);
//         hi_udelay(20000 - (duty-i*100));
//     }
// }
// /*engine task handle*/
// hi_void *engine_movement(hi_void* param)
// {
//     hi_u32 ret;
//     engine_io_init();
//     while(1) {
//         if (engine_stop_flag == HI_FALSE) {
//             set_engine_angle(1000);
//             set_engine_angle_reversal(1000);
//         }
//         hi_sleep(ENGINE_TASK_FREE_TIME);
//     }
//     ret = hi_task_delete(engine_id);
//     if (ret != HI_ERR_SUCCESS) {
//         printf("engine task delete failed\r\n");
//     }
//     engine_id = 0;
// }
// /*engine stop*/
// hi_void engine_stop(hi_void)
// {
//     engine_io_init();
//     set_engine_angle(0);
//     set_engine_angle_reversal(0);
// }
// /*create task*/
// hi_u32 engine_task(hi_void)
// {
//     hi_u32 ret; 
    
//     hi_task_attr engine_attr;

//     engine_attr.stack_size = 1024;
//     engine_attr.task_prio = 29;
//     engine_attr.task_name = (hi_char*)"engine task";

//     ret = hi_task_create(&engine_id, &engine_attr, engine_movement, HI_NULL);
//     if (ret != HI_ERR_SUCCESS) {
//         printf("Failed to create engine task\r\n");
//         return HI_ERR_FAILURE;
//     }
//     return HI_ERR_SUCCESS;
// }