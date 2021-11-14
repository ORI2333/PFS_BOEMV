#ifndef __APP_AHT20_H__
#define __APP_AHT20_H__

#define AHT_SLEEP_20MS              (20)//20ms
#define AHT_SLEEP_50MS              (50)//5ms
#define AHT_SLEEP_1S                (1000)//1s
#define AHT_DELAY_10MS              (10000)//10ms
#define AHT_DELAY_40MS              (40000) //40ms
#define AHT_DELAY_100MS             (100000) //100ms
#define AHT_DEVICE_ADDR             (0x38) //device addr
#define AHT_DEVICE_READ_STATUS      (0x71) //befor read tem&humi data need to send cmd to comfir the              
#define AHT_DEVICE_INIT_CMD         (0xBE) //aht init cmd
#define AHT_DEVICE_TEST_CMD         (0xAC) // test cmd
#define AHT_DEVICE_PARAM_HIGH_BYTE  (0x33)
#define AHT_DEVICE_PARAM_LOW_BYTE   (0x00)
#define AHT_DEVICE_PARAM_INIT_HIGH  (0x08)
#define AHT_DEVICE_CALIBRATION      (0x80)
#define AHT_DEVICE_CALIBRATION_ERR  (0x1C)
#define AHT_DEVICE_CALIBRATION_ERR_R (0x18)
#define AHT_TASK_SLEEP_TIME         (20) //thread sleep 20ms
#define BAUDRATE_INIT               (400000) 


typedef enum{
    AHT_TEMPERATURE =1,
    AHT_HUMIDITY    =2,
}aht_serson_type;
#endif