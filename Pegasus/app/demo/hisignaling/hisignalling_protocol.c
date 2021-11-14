#include <hi_types_base.h>
#include "hisignalling_protocol.h"
#include <hi_early_debug.h>
#include <hi_stdlib.h>
#include <hi_uart.h>
#include <hi_task.h>
#include <motor_control.h>
#include <steering_engine.h>
#include <app_demo_uart.h>
#include <smart_can_driver_log.h>

#include <app_demo_io_gpio.h>
#include <hi_adc.h>
#include <hi_time.h>

hi_u32 hisignalling_meg_handle_id;
hi_bool engine_stop_flag = HI_FALSE;
extern hi_u8    g_receive_uart_buff[UART_BUFF_SIZE];
extern hi_bool  g_uart_receive_flag;
extern hi_s32   g_uart_len;
hi_u8    g_send_uart_buff[UART_BUFF_SIZE];
hi_u32 Flag_Receive = 1;

extern hi_s32 Flage_1;  //垂直
extern hi_s32 Flage_2;  //水平

hi_u32 Flag_Follow = 0;

/*
 *crc32 Verification implementation
*/
static const hi_u32 crc32table[] = {
 0x00000000L, 0x77073096L, 0xee0e612cL, 0x990951baL,
 0x076dc419L, 0x706af48fL, 0xe963a535L, 0x9e6495a3L,
 0x0edb8832L, 0x79dcb8a4L, 0xe0d5e91eL, 0x97d2d988L,
 0x09b64c2bL, 0x7eb17cbdL, 0xe7b82d07L, 0x90bf1d91L,
 0x1db71064L, 0x6ab020f2L, 0xf3b97148L, 0x84be41deL,
 0x1adad47dL, 0x6ddde4ebL, 0xf4d4b551L, 0x83d385c7L,
 0x136c9856L, 0x646ba8c0L, 0xfd62f97aL, 0x8a65c9ecL,
 0x14015c4fL, 0x63066cd9L, 0xfa0f3d63L, 0x8d080df5L,
 0x3b6e20c8L, 0x4c69105eL, 0xd56041e4L, 0xa2677172L,
 0x3c03e4d1L, 0x4b04d447L, 0xd20d85fdL, 0xa50ab56bL,
 0x35b5a8faL, 0x42b2986cL, 0xdbbbc9d6L, 0xacbcf940L,
 0x32d86ce3L, 0x45df5c75L, 0xdcd60dcfL, 0xabd13d59L,
 0x26d930acL, 0x51de003aL, 0xc8d75180L, 0xbfd06116L,
 0x21b4f4b5L, 0x56b3c423L, 0xcfba9599L, 0xb8bda50fL,
 0x2802b89eL, 0x5f058808L, 0xc60cd9b2L, 0xb10be924L,
 0x2f6f7c87L, 0x58684c11L, 0xc1611dabL, 0xb6662d3dL,
 0x76dc4190L, 0x01db7106L, 0x98d220bcL, 0xefd5102aL,
 0x71b18589L, 0x06b6b51fL, 0x9fbfe4a5L, 0xe8b8d433L,
 0x7807c9a2L, 0x0f00f934L, 0x9609a88eL, 0xe10e9818L,
 0x7f6a0dbbL, 0x086d3d2dL, 0x91646c97L, 0xe6635c01L,
 0x6b6b51f4L, 0x1c6c6162L, 0x856530d8L, 0xf262004eL,
 0x6c0695edL, 0x1b01a57bL, 0x8208f4c1L, 0xf50fc457L,
 0x65b0d9c6L, 0x12b7e950L, 0x8bbeb8eaL, 0xfcb9887cL,
 0x62dd1ddfL, 0x15da2d49L, 0x8cd37cf3L, 0xfbd44c65L,
 0x4db26158L, 0x3ab551ceL, 0xa3bc0074L, 0xd4bb30e2L,
 0x4adfa541L, 0x3dd895d7L, 0xa4d1c46dL, 0xd3d6f4fbL,
 0x4369e96aL, 0x346ed9fcL, 0xad678846L, 0xda60b8d0L,
 0x44042d73L, 0x33031de5L, 0xaa0a4c5fL, 0xdd0d7cc9L,
 0x5005713cL, 0x270241aaL, 0xbe0b1010L, 0xc90c2086L,
 0x5768b525L, 0x206f85b3L, 0xb966d409L, 0xce61e49fL,
 0x5edef90eL, 0x29d9c998L, 0xb0d09822L, 0xc7d7a8b4L,
 0x59b33d17L, 0x2eb40d81L, 0xb7bd5c3bL, 0xc0ba6cadL,
 0xedb88320L, 0x9abfb3b6L, 0x03b6e20cL, 0x74b1d29aL,
 0xead54739L, 0x9dd277afL, 0x04db2615L, 0x73dc1683L,
 0xe3630b12L, 0x94643b84L, 0x0d6d6a3eL, 0x7a6a5aa8L,
 0xe40ecf0bL, 0x9309ff9dL, 0x0a00ae27L, 0x7d079eb1L,
 0xf00f9344L, 0x8708a3d2L, 0x1e01f268L, 0x6906c2feL,
 0xf762575dL, 0x806567cbL, 0x196c3671L, 0x6e6b06e7L,
 0xfed41b76L, 0x89d32be0L, 0x10da7a5aL, 0x67dd4accL,
 0xf9b9df6fL, 0x8ebeeff9L, 0x17b7be43L, 0x60b08ed5L,
 0xd6d6a3e8L, 0xa1d1937eL, 0x38d8c2c4L, 0x4fdff252L,
 0xd1bb67f1L, 0xa6bc5767L, 0x3fb506ddL, 0x48b2364bL,
 0xd80d2bdaL, 0xaf0a1b4cL, 0x36034af6L, 0x41047a60L,
 0xdf60efc3L, 0xa867df55L, 0x316e8eefL, 0x4669be79L,
 0xcb61b38cL, 0xbc66831aL, 0x256fd2a0L, 0x5268e236L,
 0xcc0c7795L, 0xbb0b4703L, 0x220216b9L, 0x5505262fL,
 0xc5ba3bbeL, 0xb2bd0b28L, 0x2bb45a92L, 0x5cb36a04L,
 0xc2d7ffa7L, 0xb5d0cf31L, 0x2cd99e8bL, 0x5bdeae1dL,
 0x9b64c2b0L, 0xec63f226L, 0x756aa39cL, 0x026d930aL,
 0x9c0906a9L, 0xeb0e363fL, 0x72076785L, 0x05005713L,
 0x95bf4a82L, 0xe2b87a14L, 0x7bb12baeL, 0x0cb61b38L,
 0x92d28e9bL, 0xe5d5be0dL, 0x7cdcefb7L, 0x0bdbdf21L,
 0x86d3d2d4L, 0xf1d4e242L, 0x68ddb3f8L, 0x1fda836eL,
 0x81be16cdL, 0xf6b9265bL, 0x6fb077e1L, 0x18b74777L,
 0x88085ae6L, 0xff0f6a70L, 0x66063bcaL, 0x11010b5cL,
 0x8f659effL, 0xf862ae69L, 0x616bffd3L, 0x166ccf45L,
 0xa00ae278L, 0xd70dd2eeL, 0x4e048354L, 0x3903b3c2L,
 0xa7672661L, 0xd06016f7L, 0x4969474dL, 0x3e6e77dbL,
 0xaed16a4aL, 0xd9d65adcL, 0x40df0b66L, 0x37d83bf0L,
 0xa9bcae53L, 0xdebb9ec5L, 0x47b2cf7fL, 0x30b5ffe9L,
 0xbdbdf21cL, 0xcabac28aL, 0x53b39330L, 0x24b4a3a6L,
 0xbad03605L, 0xcdd70693L, 0x54de5729L, 0x23d967bfL,
 0xb3667a2eL, 0xc4614ab8L, 0x5d681b02L, 0x2a6f2b94L,
 0xb40bbe37L, 0xc30c8ea1L, 0x5a05df1bL, 0x2d02ef8dL
};
 
static hi_u32 crc32(const hi_u8 *buf, hi_u32 len)
{
    hi_u32 crc = 0xffffffff;
 
    for (hi_u32 i = 0; i < len; i++) {
        crc = crc32table[(crc ^ buf[i]) & 0xff] ^ (crc >> 8);
    }
    return (crc^0xffffffff);
}
/*Log level look up table*/
static const char *hisignalling_level_names[] = 
{
    "TRACE",
    "DEBUG",
    "INFO",
    "WARN",
    "ERROR",
    "FATAL"
};
/*get hisignaling log level*/
const char *hisignalling_level_num (hisignalling_log_type hisignalling_level)
{
    if (hisignalling_level >= HISIGNALLING_LEVEL_MAX) {
        return "NULL";
    } else {
        return hisignalling_level_names[hisignalling_level];
    }
}

/*hisignal Hi3861 message package*/
static hi_u32 hisignalling_data_package(hisignalling_protocal_type *buf, hi_u32 len, hi_u8 *hisignalling_data_buf)
{
    hi_u32 crc_check_send;
    hi_u32 package_len = 0;

    memcpy_s(hisignalling_data_buf, HISGNALLING_MSG_FRAME_HEADER_LEN, buf->frame_header, HISGNALLING_MSG_FRAME_HEADER_LEN);
    memcpy_s(&hisignalling_data_buf[HISGNALLING_MSG_FRAME_HEADER_LEN], len, buf->hisignalling_msg_buf, len);
    memcpy_s(&hisignalling_data_buf[HISGNALLING_MSG_FRAME_HEADER_LEN + len], HISIGNALLING_MSG_HEADER_LEN, &(buf->end_of_frame), HISIGNALLING_MSG_HEADER_LEN);

    crc_check_send = crc32(hisignalling_data_buf, (len + HISIGNALLING_MSG_HEADER_TAIL_LEN));

    hisignalling_data_buf[len + HISIGNALLING_MSG_HEADER_TAIL_LEN] =     (hi_u8) ((crc_check_send & 0xff000000)>>24);
    hisignalling_data_buf[len + HISIGNALLING_MSG_HEADER_TAIL_LEN + 1] = (hi_u8) ((crc_check_send & 0x00ff0000)>>16);
    hisignalling_data_buf[len + HISIGNALLING_MSG_HEADER_TAIL_LEN + 2] = (hi_u8) ((crc_check_send & 0x0000ff00)>>8);
    hisignalling_data_buf[len + HISIGNALLING_MSG_HEADER_TAIL_LEN + 3] = (hi_u8)crc_check_send;
    package_len = len + HISIGNALLING_MSG_HEADER_TAIL_LEN + HISGNALLING_MSG_CRC32_LEN;
    return package_len;
}

/*hisignal Hi3861 message recevice*/
hisignalling_error_type hisignalling_msg_receive(hi_u8 *buf, hi_u32 len)
{
    hi_u32 crc_check_received;

    if(buf == HI_NULL && len > 0) {
        HISIGNALLING_LOG_FATAL("received buf is null");
        return HISGNALLING_RET_VAL_MAX;
    }
    /*校验crc*/
    if (len > HISGNALLING_MSG_CRC32_LEN) {
        crc_check_received = crc32(buf, len - HISGNALLING_MSG_CRC32_LEN);
        if (((hi_u8)((crc_check_received & 0xff000000)>>24) != buf[len-4]) && \
            ((hi_u8)((crc_check_received & 0x00ff0000)>>16) != buf[len-3]) && \
            ((hi_u8)((crc_check_received & 0x0000ff00)>>8)  != buf[len-2]) && \
            ((hi_u8)crc_check_received != buf[len-1])) {
            HISIGNALLING_LOG_ERROR("<Hi3861>: crc32 Verification failed!");
            HISIGNALLING_LOG_ERROR("<Hi3861>:crc_4=0x%x, crc_3=0x%x, crc_2=0x%x, crc_1=0x%x", buf[len-4], buf[len-3], buf[len-2], buf[len-1]);
            return HISIGNALLING_RET_VAL_ERROR;
        }
    }
    switch (buf[HISIGNALLING_MSG_PAYLOAD_INDEX]) {
        case STEERING_ENGINE_AND_MOTOR_STOP :

        if(Flag_Receive == 1){
        hi_gpio_set_ouput_val(HI_GPIO_IDX_2,HI_GPIO_VALUE1);
        hi_udelay(500);  
        hi_gpio_set_ouput_val(HI_GPIO_IDX_2,HI_GPIO_VALUE0);
        hi_udelay(20000-500); 
         hi_sleep(100);
        }
        break;
        
        case STEERING_ENGINE_AND_MOTOR_MOVEMENT:
       //  hi_gpio_set_ouput_val(HI_GPIO_IDX_2,HI_GPIO_VALUE0);
        if(Flag_Receive == 1){
        hi_gpio_set_ouput_val(HI_GPIO_IDX_8,HI_GPIO_VALUE1);
        hi_udelay(500);  
        hi_gpio_set_ouput_val(HI_GPIO_IDX_8,HI_GPIO_VALUE0);
        hi_udelay(20000-500);  
        hi_sleep(100);
        }
        break;
    }

    if(Flag_Follow){
    switch (buf[4]) {
        case 0x04 :
        Flage_2++;
        if(Flage_2 > 10) Flage_2 = 10;
        hi_sleep(1000);
        break;
        case 0x06 :
        Flage_2--;
        if(Flage_2 < 0) Flage_2 = 0;
        hi_sleep(1000);
        break;
        case 0x02 :
        Flage_1--;
        if(Flage_1 < 0) Flage_1 = 0;
        hi_sleep(1000);
        break;
        case 0x08 :
        Flage_1++;
        if(Flage_1 > 10) Flage_1 = 10;
        hi_sleep(1000);
        break;
    }
    }

    /*输出回显收到的数据*/ 
    // if ((buf[0] == HISIGNALLING_MSG_FRAME_HEADER_1) && (buf[1] == HISIGNALLING_MSG_FRAME_HEADER_2)) {
    //     for (int i = 0; i < len; i++) {
    //         HISIGNALLING_LOG_INFO("0x%x", buf[i]);
    //     }
    // }
     return HISIGNALLING_RET_VAL_CORRECT;
}
/*hisignal Hi3861 message send*/
hi_u32 hisignalling_msg_send(hi_void *buf, hi_u32 data_len)
{
    hisignalling_protocal_type hisignalling_msg;
    hi_u8 hisignalling_send_buf[HISIGNALLING_MSG_BUFF_LEN] = {0};
    hi_u32 hisignalling_package_len = 0;
    hi_u32 write_data_len = 0;

    hisignalling_msg.frame_header[0]= HISIGNALLING_MSG_FRAME_HEADER_1;
    hisignalling_msg.frame_header[1]= HISIGNALLING_MSG_FRAME_HEADER_2; 
    memcpy_s(hisignalling_msg.hisignalling_msg_buf, data_len, buf, data_len);
    hisignalling_msg.end_of_frame = HISIGNALLING_MSG_FRAME_TAIL;

    hisignalling_package_len = hisignalling_data_package(&hisignalling_msg, data_len, hisignalling_send_buf);
    if (!hisignalling_package_len) {
        HISIGNALLING_LOG_ERROR("hisignaling_data_package failed = %d\r\n", hisignalling_package_len);
        return HI_ERR_FAILURE;
    }
    if (*hisignalling_send_buf == NULL) {
        HISIGNALLING_LOG_ERROR("hisignal send buf is null!\r\n");
        return HI_ERR_FAILURE;
    }
    write_data_len = hi_uart_write(HI_UART_IDX_1, hisignalling_send_buf, hisignalling_package_len);
    if (!write_data_len) {
        HISIGNALLING_LOG_ERROR("hi_uart_write failed, write data len=%d\r\n", write_data_len);
        return HI_ERR_FAILURE;
    }
    return HI_ERR_SUCCESS;
}

hi_void *hisignalling_msg_handle(hi_void *param)
{
    hi_u32 ret;

    while(1) {

        memset_s(g_send_uart_buff, sizeof(g_send_uart_buff)/sizeof(g_send_uart_buff[0]), 0x0, sizeof(g_send_uart_buff)/sizeof(g_send_uart_buff[0]));
        if (g_uart_receive_flag) {
            /*接收数据*/
            hisignalling_msg_receive(g_receive_uart_buff, g_uart_len);
            /*回显数据组包*/
            if (g_uart_len > (HISGNALLING_MSG_CRC32_LEN + HISIGNALLING_MSG_HEADER_TAIL_LEN)) {
                memcpy_s(g_send_uart_buff, (g_uart_len - HISGNALLING_MSG_CRC32_LEN - HISIGNALLING_MSG_HEADER_TAIL_LEN), \
                &g_receive_uart_buff[HISGNALLING_MSG_FRAME_HEADER_LEN], \
                (g_uart_len - HISGNALLING_MSG_CRC32_LEN - HISIGNALLING_MSG_HEADER_TAIL_LEN));
                /*接收到Hi3516DV300数据后，发送回显*/
                hisignalling_msg_send(g_send_uart_buff, (g_uart_len - HISGNALLING_MSG_CRC32_LEN - HISIGNALLING_MSG_HEADER_TAIL_LEN));
            }
            g_uart_receive_flag = HI_FALSE;
            memset_s(g_receive_uart_buff, UART_BUFF_SIZE, 0x0, UART_BUFF_SIZE);
        }
        hi_sleep(HISGNALLING_FREE_TASK_TIME);
    }
    ret = hi_task_delete(hisignalling_meg_handle_id);
    if (ret != HI_ERR_SUCCESS) {
        HISIGNALLING_LOG_ERROR("Failed to delete hisignal msg task %d\r\n", ret);
    }
    hisignalling_meg_handle_id = 0;
}

hi_u32 hisignaling_msg_task(hi_void)
{
    hi_u32 ret; 
    
    hi_task_attr hisignalling_attr;

    hisignalling_attr.stack_size = HISIGNALLING_MSG_TASK_STACK_SIZE;
    hisignalling_attr.task_prio = HISIGNALLING_MSG_TASK_PRIO;
    hisignalling_attr.task_name = (hi_char*)"hisignal msg task";

    ret = hi_task_create(&hisignalling_meg_handle_id, &hisignalling_attr, hisignalling_msg_handle, HI_NULL);
    if (ret != HI_ERR_SUCCESS) {
        HISIGNALLING_LOG_ERROR("Failed to create hisignaling msg task\r\n");
        return HI_ERR_FAILURE;
    }
    return HI_ERR_SUCCESS;
}
