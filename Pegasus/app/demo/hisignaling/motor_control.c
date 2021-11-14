#include <hi_types_base.h>
#include <hi_early_debug.h>
#include <hi_io.h>
#include <hi_gpio.h>
#include <hi_pwm.h>

#define PWM_FREQ    (60000)
#define MOTOR_SPEED (1000)

hi_void gpio_config(hi_io_name gpio, hi_gpio_idx id, hi_gpio_dir dir, hi_gpio_value gpio_val, hi_u8 val)
{
    hi_io_set_func(gpio, val);
    hi_gpio_set_dir(id, dir);
    hi_gpio_set_ouput_val(id, gpio_val);
}

hi_void pwm_control(hi_io_name id, hi_u8 val, hi_pwm_port port, hi_u16 duty)
{
    hi_io_set_func(id, val);
    hi_pwm_init(port);
    hi_pwm_set_clock(PWM_CLK_160M);
    hi_pwm_start(port, duty, PWM_FREQ);
}
/*后退*/
hi_void motor_go_back(hi_void)
{
    pwm_control(HI_IO_NAME_GPIO_0, HI_IO_FUNC_GPIO_0_PWM3_OUT, HI_PWM_PORT_PWM3, MOTOR_SPEED);
    gpio_config(HI_IO_NAME_GPIO_1, HI_GPIO_IDX_1, HI_GPIO_DIR_OUT, HI_GPIO_VALUE1, HI_IO_FUNC_GPIO_1_GPIO);
    pwm_control(HI_IO_NAME_GPIO_9, HI_IO_FUNC_GPIO_9_PWM0_OUT, HI_PWM_PORT_PWM0, MOTOR_SPEED);
    gpio_config(HI_IO_NAME_GPIO_10, HI_GPIO_IDX_10, HI_GPIO_DIR_OUT, HI_GPIO_VALUE1, HI_IO_FUNC_GPIO_10_GPIO);
}
/*前进*/

hi_void motor_go_forward(hi_void)
{
    gpio_config(HI_IO_NAME_GPIO_0, HI_GPIO_IDX_0, HI_GPIO_DIR_OUT, HI_GPIO_VALUE1, HI_IO_FUNC_GPIO_0_GPIO);
    pwm_control(HI_IO_NAME_GPIO_1, HI_IO_FUNC_GPIO_1_PWM4_OUT, HI_PWM_PORT_PWM4, MOTOR_SPEED);
    gpio_config(HI_IO_NAME_GPIO_9, HI_GPIO_IDX_9, HI_GPIO_DIR_OUT, HI_GPIO_VALUE1, HI_IO_FUNC_GPIO_9_GPIO);
    pwm_control(HI_IO_NAME_GPIO_10, HI_IO_FUNC_GPIO_10_PWM1_OUT, HI_PWM_PORT_PWM1, MOTOR_SPEED);
}
/*停止*/
hi_void motor_stop(hi_void)
{
    gpio_config(HI_IO_NAME_GPIO_0, HI_GPIO_IDX_0, HI_GPIO_DIR_OUT, HI_GPIO_VALUE0, HI_IO_FUNC_GPIO_0_GPIO);
    gpio_config(HI_IO_NAME_GPIO_1, HI_GPIO_IDX_1, HI_GPIO_DIR_OUT, HI_GPIO_VALUE0, HI_IO_FUNC_GPIO_1_GPIO);
    gpio_config(HI_IO_NAME_GPIO_9, HI_GPIO_IDX_9, HI_GPIO_DIR_OUT, HI_GPIO_VALUE0, HI_IO_FUNC_GPIO_9_GPIO);
    gpio_config(HI_IO_NAME_GPIO_10, HI_GPIO_IDX_10, HI_GPIO_DIR_OUT, HI_GPIO_VALUE0, HI_IO_FUNC_GPIO_10_GPIO);
}