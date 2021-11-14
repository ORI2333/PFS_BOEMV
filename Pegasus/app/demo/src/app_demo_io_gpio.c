/*
 * Copyright (c) 2020 HiSilicon (Shanghai) Technologies CO., LIMITED.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <app_demo_io_gpio.h>
#include <c081_nfc.h>
#include <hi_sem.h>
#include <hi_types_base.h>

#define     NFC_INTERRUPT_TASK_STAK_SIZE    (1024*4)
#define     NFC_INTERRUPT_TASK_PRIORITY     (25)
#define     SLEEP_1S                        (1000)     

hi_u32 g_nfc_sem_id =0;
hi_u8  g_nfc_isr_task_id;

/** 
	@berf gpio callback func
	@berf Interrupt callback function 	
*/
hi_void gpio_isr_func(hi_void *arg)
{
    hi_unref_param(arg);
    /*
	Because NFC uses I2C mode for communication, signals are used in I2C interface.
	When NFC uses interrupt mode for communication, it cannot directly operate the interface in interrupt
	Interrupt synchronization by semaphore
    */
    hi_sem_signal(g_nfc_sem_id);
    printf("----- gpio isr success -----\r\n");
}
/**
	@berf gpio Initialization configuration process
*/
hi_void io_gpio_demo(hi_void)
{
    /* Take gpio 0 as an example */
    hi_u32 ret;
    hi_gpio_value gpio_val = HI_GPIO_VALUE1;

    ret = hi_gpio_init();
    if (ret != HI_ERR_SUCCESS) {
        printf("===== ERROR ===== gpio -> hi_gpio_init ret:%d\r\n", ret);
        return;
    }
    printf("----- gpio init success-----\r\n");

    ret = hi_io_set_func(HI_IO_NAME_GPIO_0, HI_IO_FUNC_GPIO_0_GPIO);
    if (ret != HI_ERR_SUCCESS) {
        printf("===== ERROR ===== gpio -> hi_io_set_func ret:%d\r\n", ret);
        return;
    }
    printf("----- io set func success-----\r\n");

    ret = hi_gpio_set_dir(HI_GPIO_IDX_0, HI_GPIO_DIR_IN);
    if (ret != HI_ERR_SUCCESS) {
        printf("===== ERROR ===== gpio -> hi_gpio_set_dir1 ret:%d\r\n", ret);
        return;
    }
    printf("----- gpio set dir success! -----\r\n");

    ret = hi_gpio_get_input_val(HI_GPIO_IDX_0, &gpio_val);
    if (ret != HI_ERR_SUCCESS) {
        printf("===== ERROR ===== gpio -> hi_gpio_get_input_val ret:%d\r\n", ret);
        return;
    }
    printf("----- gpio input val is:%d. -----\r\n", gpio_val);

    ret = hi_gpio_deinit();
    if (ret != HI_ERR_SUCCESS) {
        printf("===== ERROR ===== gpio -> hi_gpio_deinit ret:%d\r\n", ret);
        return;
    }
    printf("----- gpio deinit success-----\r\n");
}

/** 
	@berf gpio callback demo 
	@berf gpio External interrupt example
*/
hi_void gpio_isr_demo(hi_void)
{
    hi_u32 ret;

    printf("----- gpio isr demo -----\r\n");

    (hi_void)hi_gpio_init();

    ret = hi_gpio_set_dir(HI_GPIO_IDX_10, HI_GPIO_DIR_IN);
    if (ret != HI_ERR_SUCCESS) {
        printf("===== ERROR ======gpio -> hi_gpio_set_dir1 ret:%d\r\n", ret);
        return;
    }

    ret = hi_gpio_register_isr_function(HI_GPIO_IDX_10, HI_INT_TYPE_EDGE,
                                        HI_GPIO_EDGE_FALL_LEVEL_LOW, gpio_isr_func, HI_NULL);
    if (ret != HI_ERR_SUCCESS) {
        printf("===== ERROR ======gpio -> hi_gpio_register_isr_function ret:%d\r\n", ret);
    }
}
/**
	@berf gpio Specific process of external interrupt implementation
*/
hi_void app_demo_custom_io_gpio(hi_void)
{
    io_gpio_demo();
    hi_sleep(SLEEP_1S);  /* sleep 1000ms */
    gpio_isr_demo();
}
/*NFC Interrupt handle*/
hi_void *gpio_isr_handle(hi_void* param)
{
    hi_u8 ret = 0;

    printf("----- gpio isr handle -----\r\n");
    while (1) {
        hi_sem_wait(g_nfc_sem_id, HI_SYS_WAIT_FOREVER);
        ret = fm11_read_reg(MAIN_IRQ_REG);
        if (ret & MAIN_IRQ_ACTIVE) {
            FlagFirstFrame = 1;
        }	        
        hi_sleep(1);
    }
    return (HI_NULL);
}
/*NFC Interrupt mode communication task*/
hi_void gpio_isr_task(hi_void)
{
    hi_u32 ret;
    hi_task_attr attr = {0};
    /*Create binary semaphore synchronous interrupt response*/
    hi_sem_bcreate(&g_nfc_sem_id, HI_SEM_ZERO);

    hi_task_lock();
    attr.stack_size = NFC_INTERRUPT_TASK_STAK_SIZE;
    attr.task_prio = NFC_INTERRUPT_TASK_PRIORITY;
    attr.task_name = (hi_char*)"gpio_isr_task";
    ret = hi_task_create(&g_nfc_isr_task_id, &attr, gpio_isr_handle, HI_NULL);
    if (ret != HI_ERR_SUCCESS) {
        printf("Falied to create gpio_isr task!\n");
    }
    hi_task_unlock();
}