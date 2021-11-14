
#include <hi_types_base.h>
#include <hi_i2c.h>
#include <app_demo_aht20.h>
#include <hi_early_debug.h>
#include <hi_task.h>
#include <hi_time.h>
#include <string.h>
#include <app_demo_multi_sample.h>
#include <hi_mux.h>

#define  AHT_REG_ARRAY_LEN          (6)
#define  AHT_OC_ARRAY_LEN           (6)
#define  AHT_SNED_CMD_LEN           (3)
#define  AHT20_DEMO_TASK_STAK_SIZE  (1024*4)
#define  AHT20_DEMO_TASK_PRIORITY   (25)
#define  AHT_REG_ARRAY_INIT_LEN     (1)
#define  AHT_CALCULATION            (1048576)

hi_u8 g_aht20_demo_task_id =0;
hi_u8 g_ahu20_temperature_buff[6] = {0};
hi_u8 g_ahu20_humidity_buff[6] = {0};
hi_float aht_temper =0;
hi_float aht_humi=0;
extern hi_u8 g_menu_select;
extern hi_u8 g_menu_mode ;
extern hi_u32 g_mux_id;
extern hi_u8  *flaot_to_string(hi_double d, hi_u8 *str );
/*
*Check whether the bit3 of the temperature and humidity sensor is initialized successfully, 
*otherwise send the setting of 0xbe to set the sensor initialization
*/
static hi_u32 ath20_check_and_init(hi_u8 init_cmd, hi_u8 init_high_byte, hi_u8 init_low_byte)
{
    hi_u32 status =0;
    hi_i2c_idx id = HI_I2C_IDX_0;
    hi_i2c_data aht20_i2c_write_cmd_addr_init ={0};
    hi_i2c_data aht20_i2c_data = { 0 };
     hi_u8 recv_data_init[AHT_REG_ARRAY_INIT_LEN] = { 0 };
    hi_u8 init_send_user_cmd[AHT_SNED_CMD_LEN] = {init_cmd, init_high_byte, init_low_byte};

    memset(&recv_data_init, 0x0, sizeof(recv_data_init));
    memset(&aht20_i2c_data, 0x0, sizeof(hi_i2c_data));

    aht20_i2c_data.receive_buf = recv_data_init;
    aht20_i2c_data.receive_len = AHT_REG_ARRAY_INIT_LEN;
    aht20_i2c_write_cmd_addr_init.send_buf = init_send_user_cmd;
    aht20_i2c_write_cmd_addr_init.send_len = AHT_SNED_CMD_LEN;

    status = hi_i2c_read(id, (AHT_DEVICE_ADDR<<1)|0x01, &aht20_i2c_data);
    // printf("recv_data_init[0] =0x%x\r\n", recv_data_init[0]);
    if (((recv_data_init[0] != AHT_DEVICE_CALIBRATION_ERR) && (recv_data_init[0] != AHT_DEVICE_CALIBRATION_ERR_R)) || (recv_data_init[0] == AHT_DEVICE_CALIBRATION)) {
        // printf("sensor need to calibration\r\n");
        status = hi_i2c_write(id, (AHT_DEVICE_ADDR<<1)|0x00, &aht20_i2c_write_cmd_addr_init);
        hi_sleep(AHT_SLEEP_1S);
        return HI_ERR_FAILURE;
    }

    return HI_ERR_SUCCESS;
}
/**
	@berf  Send trigger measurement command
	@param hi_u8 trigger_cmd: Trigger command
	@param hi_u8 high_byte_cmd:Measurement command high 8 bits
	@param hi_u8 low_byte_cmd:Measurement command low 8 bits
*/
hi_u32 aht20_write(hi_u8 trigger_cmd, hi_u8 high_byte_cmd, hi_u8 low_byte_cmd)
{
    hi_u32 status =0;
    hi_i2c_idx id = 0;
    hi_i2c_data aht20_i2c_write_cmd_addr ={0};
    hi_u8 _send_user_cmd[AHT_SNED_CMD_LEN] = {trigger_cmd, high_byte_cmd, low_byte_cmd};

    aht20_i2c_write_cmd_addr.send_buf = _send_user_cmd;
    aht20_i2c_write_cmd_addr.send_len = AHT_SNED_CMD_LEN;

    status = hi_i2c_write(id, (AHT_DEVICE_ADDR<<1)|0x00, &aht20_i2c_write_cmd_addr);

    return HI_ERR_SUCCESS;
}

/**
	@berf Read aht20 serson data, and save the read data in g_ahu20_temperature_buff,g_ahu20_humidity_buff
	@param hi_u32 recv_len: Read length
	@param hi_u8 type:Read type (temperature / humidity)
	@return Successed HI_ERR_SUCCESS
*/
hi_u32 aht20_read(hi_u32 recv_len, hi_u8 type)
{
    hi_u32 status;
    hi_i2c_idx id  = HI_I2C_IDX_0;
    hi_u8 recv_data[AHT_REG_ARRAY_LEN] = { 0 };
    hi_i2c_data aht20_i2c_data = { 0 };
    hi_float temper =0;
    hi_float temper_t =0;
    hi_float humi =0;
    hi_float humi_h =0;
    /* Request memory space */
    memset(&recv_data, 0x0, sizeof(recv_data));
    memset(&aht20_i2c_data, 0x0, sizeof(hi_i2c_data));
    aht20_i2c_data.receive_buf = recv_data;
    aht20_i2c_data.receive_len = recv_len;
    
    status = hi_i2c_read(id, (AHT_DEVICE_ADDR<<1)|0x01, &aht20_i2c_data);
    if (type == AHT_TEMPERATURE) {
        temper = (hi_float)((recv_data[3] &0x0f)<<16 | recv_data[4]<<8 |recv_data[5]);//Temperature splicing
        temper_t=(temper/AHT_CALCULATION)*200-50;  // T= (S_t/2^20)*200-50
        aht_temper = temper_t;
        flaot_to_string(aht_temper, g_ahu20_temperature_buff);
        return HI_ERR_SUCCESS; 

    } 
    if (type == AHT_HUMIDITY) {
        humi = (hi_float)((recv_data[1]<<12 | recv_data[2]<<4) | ((recv_data[3] & 0xf0)>>4));//Humidity splicing
        humi_h = humi/AHT_CALCULATION*100;
        aht_humi = humi_h;
        flaot_to_string(aht_humi, g_ahu20_humidity_buff);
        return HI_ERR_SUCCESS;
    }
}

hi_void *app_demo_aht20(hi_void *param)
{
    hi_i2c_idx id =0;
    hi_u32 status =0;
    hi_u32 ret;
    hi_unref_param(param);
    hi_i2c_init(id, BAUDRATE_INIT); /* baudrate: 400000 */
    hi_i2c_set_baudrate(id, BAUDRATE_INIT);
    /*上电等待40ms*/
    hi_udelay(AHT_DELAY_40MS);//40ms
    while(1) {
        ret = hi_mux_pend(g_mux_id, HI_SYS_WAIT_FOREVER);
        if (ret == HI_ERR_SUCCESS) {
        /*check whethe the sensor  calibration*/
            while ( HI_ERR_SUCCESS != ath20_check_and_init(AHT_DEVICE_INIT_CMD, AHT_DEVICE_PARAM_INIT_HIGH, AHT_DEVICE_PARAM_LOW_BYTE)) {
                // printf("aht20 sensor check init failed!\r\n");
                hi_sleep(AHT_SLEEP_50MS);
            }
            /* on hold master mode*/
            status = aht20_write(AHT_DEVICE_TEST_CMD, AHT_DEVICE_PARAM_HIGH_BYTE, AHT_DEVICE_PARAM_LOW_BYTE);//tempwerature
            hi_udelay(AHT_DELAY_100MS);//100ms, Wait for measurement to complete
            status = aht20_read(AHT_REG_ARRAY_LEN, AHT_TEMPERATURE);
            status = aht20_read(AHT_REG_ARRAY_LEN, AHT_HUMIDITY);
            if (status != HI_ERR_SUCCESS) {
                printf("get humidity data error!\r\n");
            }
        }
        hi_mux_post(g_mux_id);
        hi_sleep(AHT_TASK_SLEEP_TIME);//20ms
    }
}
/*get aht20 sensor data*/
hi_void get_aht20_sensor_datat(hi_void)
{
    hi_i2c_idx id =0;
    hi_u32 status =0;

    hi_i2c_init(id, BAUDRATE_INIT); /* baudrate: 400000 */
    hi_i2c_set_baudrate(id, BAUDRATE_INIT);

    /* on hold master mode*/
    status = aht20_write(AHT_DEVICE_TEST_CMD, AHT_DEVICE_PARAM_HIGH_BYTE, AHT_DEVICE_PARAM_LOW_BYTE);//tempwerature
    hi_udelay(AHT_DELAY_100MS);//100ms, Wait for measurement to complete
    status = aht20_read(AHT_REG_ARRAY_LEN, AHT_TEMPERATURE);
    status = aht20_read(AHT_REG_ARRAY_LEN, AHT_HUMIDITY);
}
/**
	@berf app demo th06 task
	@param hi_void
	@return Task created successfully ret = HI_ERR_SUCCESS,failed ret = 0x800xxxx
*/
hi_void app_demo_aht20_task(hi_void)
{
    hi_u32 ret =0;
    hi_task_attr attr ={0};
	/*attr Configuration of structural parameters*/
    attr.stack_size = AHT20_DEMO_TASK_STAK_SIZE;//Task stack memory
    attr.task_prio = AHT20_DEMO_TASK_PRIORITY;//The task priority ranges from 0 to 31. Tasks 0 to 10 should not be used. The SDK has been used. The higher the value, the lower the priority
    attr.task_name = (hi_char*)"app_demo_aht20_task";//task name
	/*create task*/
    ret = hi_task_create(&g_aht20_demo_task_id, &attr, app_demo_aht20, HI_NULL);
    if(ret != HI_ERR_SUCCESS){
        printf("Failed to create app_demo_aht20_task\r\n");
    }
}