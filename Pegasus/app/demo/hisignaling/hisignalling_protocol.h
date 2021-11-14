/**/
#ifndef __HISIGNALLING_PROTOCOL_H_
#define __HISIGNALLING_PROTOCOL_H_
#include <hi_types_base.h>
#include <stdarg.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
/**
* @brief : hisignalling Macro definition
*/
#define HISIGNALLING_MSG_HEADER_LEN         (1)
#define HISGNALLING_MSG_FRAME_HEADER_LEN    (2)
#define HISIGNALLING_MSG_HEADER_TAIL_LEN    (3)
#define HISGNALLING_MSG_CRC32_LEN           (4)
#define HISIGNALLING_MSG_PAYLOAD_INDEX      (5)
#define HISGNALLING_FREE_TASK_TIME          (20)
#define HISIGNALLING_MSG_TASK_PRIO          (28)
#define HISIGNALLING_UART_MSG_LEN           (32)
#define HISIGNALLING_MSG_BUFF_LEN           (512)
#define HISIGNALLING_MSG_TASK_STACK_SIZE    (4096)

/*hisgnalling protocol frame header and tail*/
#define HISIGNALLING_MSG_FRAME_HEADER_1     ((hi_u8)0xAA)
#define HISIGNALLING_MSG_FRAME_HEADER_2     ((hi_u8)0x55)
#define HISIGNALLING_MSG_FRAME_TAIL         ((hi_u8)0xFF)
/**
*@brief Adapter plate selection
*使用时选择打开宏，使用外设扩展板打开#define BOARD_SELECT_IS_EXPANSION_BOARD这个宏，使用Robot板打开#define BOARD_SELECT_IS_ROBOT_BOARD这个宏
**/
// #define BOARD_SELECT_IS_ROBOT_BOARD
#define BOARD_SELECT_IS_EXPANSION_BOARD
#ifdef BOARD_SELECT_IS_EXPANSION_BOARD
#define EXPANSION_BOARD
#else 
#define ROBOT_BOARD
#endif
/**
 * @brief:this defines for the log module, and  HISIGNALING_LOG_TRACE/HISIGNALING_LOG_DEBUG... will not participate the compile in the release version
 *
 *
 * */
typedef enum {
    HISIGNALLING_LEVEL_TRACE = 0,
    HISIGNALLING_LEVEL_DEBUG,
    HISIGNALLING_LEVEL_INFO,
    HISIGNALLING_LEVEL_WARN,
    HISIGNALLING_LEVEL_ERROR,
    HISIGNALLING_LEVEL_FATAL,
    HISIGNALLING_LEVEL_MAX
} hisignalling_log_type;
/**
 * @brief: use this function to get the hisignalling log level name
 *
 * @parameter[in]:level, the level to get
 *
 * @return: the mapped level name
 * */
const char *hisignalling_level_num (hisignalling_log_type hisignalling_level);
/**
 * @brief: this is a weak function ,and you could rewrite one
 *
 * @param fmt: same use as the fmt for printf
 *
 * @param unfixed: same use for printf
 *
 * @return: don't care about it
 *
 * @attention: and the components should not call this function directly, you'd better
 *
 *             call HISIGNALING_LOG groups
 *
 * */
//__attribute__((weak)) void HISIGNALLING_PRINT(const char *format, ...);
#define HISIGNALLING_PRINT(fmt, ...) \
    do \
    { \
        printf(fmt, ##__VA_ARGS__); \
    } while(0)

 #define HISIGNALLING_LOG(level, fmt, ...) \
    do \
    { \
        HISIGNALLING_PRINT("<%s>, <%s>, <%d> "fmt" \r\n", \
        hisignalling_level_num((level)), __FUNCTION__, __LINE__, ##__VA_ARGS__); \
    } while(0)

#define HISIGNALLING_LOG_TRACE(fmt, ...) \
    do \
    { \
        HISIGNALLING_LOG (HISIGNALLING_LEVEL_TRACE, fmt, ##__VA_ARGS__); \
    } while(0)
   
#define HISIGNALLING_LOG_DEBUG(fmt, ...)   \
    do \
    { \
        HISIGNALLING_LOG (HISIGNALLING_LEVEL_DEBUG, fmt, ##__VA_ARGS__); \
    } while(0)
    
#define HISIGNALLING_LOG_INFO(fmt, ...) \
    do \
    { \
       HISIGNALLING_LOG (HISIGNALLING_LEVEL_INFO, fmt, ##__VA_ARGS__); \
    } while(0) 

#define HISIGNALLING_LOG_WARN(fmt, ...) \
    do \
    { \
        HISIGNALLING_LOG (HISIGNALLING_LEVEL_WARN, fmt, ##__VA_ARGS__); \
    } while(0)
    
#define HISIGNALLING_LOG_ERROR(fmt, ...) \
    do \
    { \
        HISIGNALLING_LOG (HISIGNALLING_LEVEL_ERROR, fmt, ##__VA_ARGS__); \
    } while(0)
    
#define HISIGNALLING_LOG_FATAL(fmt, ...) \
    do \
    { \
        HISIGNALLING_LOG (HISIGNALLING_LEVEL_FATAL, fmt, ##__VA_ARGS__); \
    } while(0)
/**
 * @brief: use this hisignalling Transmission protocol frame format
 *
 * @param frame_header: Transmission protocol frame header
 *
 * @param hisignaling_msg_buf: Transmission protocol frame buffer
 *
 * @param hisigaling_msg_len: Transmission protocol frame buffer len
 * 
 * @param end_of_frame: Transmission protocol frame tail
 * 
 * @param hisignaling_crc32_check: Transmission protocol crc32 check
 *
 * */
typedef struct hisignalling_protocal {
    hi_u8 frame_header[HISGNALLING_MSG_FRAME_HEADER_LEN];
    hi_u8 hisignalling_msg_buf[HISIGNALLING_MSG_BUFF_LEN];
    hi_u32 hisigalling_msg_len;
    hi_u8 end_of_frame;
    hi_u32 hisignalling_crc32_check; 
}hisignalling_protocal_type;
/**
 * @brief: use this hisignalling return type
 *
 * @param HISIGNALLING_RET_VAL_CORRECT: return type is correct
 *
 * @param HISIGNALLING_RET_VAL_ERROR: return type is error 
 *
 * @param HISIGNALLING_RET_VAL_ERROR: return type is  unknown type
 * */
typedef enum hisignalling_return_val{
    HISIGNALLING_RET_VAL_CORRECT = 0,
    HISIGNALLING_RET_VAL_ERROR,
    HISGNALLING_RET_VAL_MAX
}hisignalling_error_type;
/**
 * @brief: use this hisignalling sample:Access peripheral enumeration type
 *
 * @param MOTOR_GO_FORWARD: peripheral motor go forward
 *
 * @param MOTOR_GO_BACK: peripheral motor go back 
 *
 * @param STEERING_ENGINE_MOVEMENT/STEERING_ENGINE_AND_MOTOR_MOVEMENT/STEERING_ENGINE_AND_MOTOR_STOP... the same use for peripheral
 * */
typedef enum {
    MOTOR_GO_FORWARD = 0x03,
    MOTOR_GO_BACK = 0x08,
    STEERING_ENGINE_MOVEMENT = 0x09,
    STEERING_ENGINE_AND_MOTOR_MOVEMENT = 0x01,
    STEERING_ENGINE_AND_MOTOR_STOP = 0x02,
    TRASH_CAN_LID_OPEN = 0x05,
    TRASH_CAN_LID_COLSE = 0x04
}hisignalling_data_type;
/**
* @brief:hisignalling protocal Function declaration
**/
hi_u32 hisignalling_msg_task(hi_void);
/**
*@brief:hisignalling uart message receive API
*@param buf: uart receive buffer
*@param len: uart receive buffer len
*/
hisignalling_error_type hisignalling_msg_receive(hi_u8 *buf, hi_u32 len);
/**
*@brief:hisignalling uart send API
*@param buf: uart message send buffer
*@param len: uart message send buffer len
*/
hi_u32 hisignalling_msg_send(hi_void *buf, hi_u32 data_len);

#endif /*HISIGNALING_PROTOCOL_H*/