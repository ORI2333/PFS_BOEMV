#include <hi_types_base.h>
#include <hi_early_debug.h>
#include <smart_can_driver_log.h>
/*log level names*/
const char *smart_can_level_names[] = 
{
    "TRACE",
    "DEBUG",
    "INFO",
    "WARN",
    "ERROR",
    "FATAL"
};
/*get log level number*/
const char *smart_can_level_num (smart_can_log_type smart_can_level)
{
    if (smart_can_level >= SMART_CAN_LEVEL_MAX) {
        return "NULL";
    } else {
        return smart_can_level_names[smart_can_level];
    }
}
