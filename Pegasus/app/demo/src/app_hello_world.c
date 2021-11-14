#include "app_hello_world.h"

extern hi_u32 app_demo_led_control_task(hi_void);
/*hello world entry*/
hi_void hispark_pegasus_hello_world(hi_void)
{

	/*led control task*/
    app_demo_led_control_task();
}