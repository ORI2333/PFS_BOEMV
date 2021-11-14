#define CONFIG_HISTREAMING_SUPPORT 1
#ifdef CONFIG_HISTREAMING_SUPPORT
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <hi_pwm.h>
#include <hi_time.h>
/* Link Header Files */
#include <link_service.h>
#include <link_platform.h>
#include <histreaming.h>
#include <hi_io.h>
#include <hi_early_debug.h>
#include <hi_gpio.h>
#include <hi_task.h>
#include <hi_types_base.h>
#include <app_demo_multi_sample.h>

//#include <app_demo_colorful_sample.h>

#define HISTREAMING_TASK
#ifdef HISTREAMING_TASK
#define HISTREAMING_DEMO_TASK_STAK_SIZE (1024*8)
#define HISTREAMING_DEMO_TASK_PRIORITY  20
hi_u32 g_histreaming_demo_task_id = 0;
#endif

extern hi_u8 g_menu_mode;
extern hi_u8 g_current_type;
extern hi_u8 g_current_mode;
extern hi_u8 g_menu_select;
extern hi_u8 g_key_down_flag ;

extern hi_s32 Flage_1;
extern hi_s32 Flage_2;
extern hi_s32 Flage_3;


extern hi_s32 Flage_Feed;
extern hi_u32 Weigth_1_Flag;
extern hi_u32 Flage_Zigbee;
extern hi_u32 Flag_Receive;
extern hi_u32 Flag_Follow;
extern hi_u32 Weigth_1_Flag;

extern hi_void oled_main_menu_display(hi_void);

static hi_void histreaming_colorful_light_control(const char* property, char* value);
static hi_void histreaming_traffic_light_control(const char* property, char* value);
static hi_void histreaming_environment_control(const char* property, char* value);
static hi_void histreaming_nfc_control(const char* property, char* value);
static hi_u32 histreaming_colorful_light_return_main_menu(const char* property, char* value);
static hi_u32 histreaming_traffic_light_return_main_menu(const char* property, char* value);
extern hi_void gpio_control(hi_io_name gpio, hi_gpio_idx id, hi_gpio_dir  dir,  hi_gpio_value gpio_val, hi_u8 val);

/**
	@berf histreaming colorful light function control
	@param const char* property: Eigenvalues sent by app
	@param  char* value:Value from App
*/
static hi_void histreaming_colorful_light_control(const char* property, char* value)
{
    /*manual control module*/
    if (strcmp(property, "CarControl") == 0) {
        g_key_down_flag = KEY_GPIO_7;
        g_menu_mode = MAIN_FUNCTION_SELECT_MODE;
        g_menu_select = COLORFUL_LIGHT_MENU;
        g_current_mode = CONTROL_MODE;
        if (strcmp(value, "forward") == 0) {
            g_current_type = FORWARD;
            Flage_1++;
            printf("Move down, the gear is:%d\r\n",Flage_1);
        }  
        if (strcmp(value, "back") == 0) {
            g_current_type = BACK;
            Flage_1--;
            printf("Move up, the gear is:%d\r\n",Flage_1);               
        } 
        if (strcmp(value, "stop") == 0) {
            g_current_type = STOP;
             Flage_1 = 5;
             Flage_2 = 5;
            printf("Reset\r\n");  
        }
         if (strcmp(value, "left") == 0) {
            g_current_type = LEFT;
             Flage_2++;
             printf("Move Left, the gear is:%d\r\n",Flage_2);
        }
         if (strcmp(value, "right") == 0) {
            g_current_type = RIGHT;
             Flage_2--;
            printf("Move Right, the gear is:%d\r\n",Flage_2);
        }
         if (strcmp(value, "follow") == 0) {
            Flag_Follow = !Flag_Follow ;
        }
    }
     if (strcmp(property, "dogfood") == 0) {
        g_key_down_flag = KEY_GPIO_7;
        g_menu_mode = MAIN_FUNCTION_SELECT_MODE;
        g_menu_select = COLORFUL_LIGHT_MENU;
        g_current_mode = CONTROL_MODE;
        if (strcmp(value, "80") == 0) {
            Weigth_1_Flag = 80;
        }  
        if (strcmp(value, "30") == 0) {
            Flage_Zigbee = !Flage_Zigbee;
        }  
         if (strcmp(value, "50") == 0) {
            Flag_Receive = !Flag_Receive;
        }  
        if (strcmp(value, "feeding") == 0) {
            Flage_Feed = 1;    
        }
        if (isdigit(value)) {  
        Weigth_1_Flag = value;
        }
    }
    /*period led control*/
    if (strcmp(property, "pl_s") == 0) {
        g_key_down_flag = KEY_GPIO_7;
        g_menu_mode = MAIN_FUNCTION_SELECT_MODE;
        g_menu_select = COLORFUL_LIGHT_MENU;
        g_current_mode = COLORFUL_LIGHT_MODE;
        if (strcmp(value, "p1s") == 0) {
            g_menu_mode = SUB_MODE_SELECT_MODE;
            g_current_type = CYCLE_FOR_ONE_SECOND;
            printf("period_1s\r\n");
        } 
        if (strcmp(value, "p05s") == 0) {
            g_menu_mode = SUB_MODE_SELECT_MODE;
            g_current_type = CYCLE_FOR_HALF_SECOND;
            printf("period_05s\r\n");               
        }
         if (strcmp(value, "p025s") == 0) {
            g_menu_mode = SUB_MODE_SELECT_MODE;
            g_current_type = CYCLE_FOR_QUARATER_SECOND;
            printf("period_025s\r\n");  
        }
    }
    /*pwm led control*/
    if (strcmp(property, "pwm_s") == 0) {
        g_key_down_flag = KEY_GPIO_7;
        g_menu_mode = MAIN_FUNCTION_SELECT_MODE;
        g_menu_select = COLORFUL_LIGHT_MENU;
        g_current_mode = PWM_CONTROL_MODE;
        if (strcmp(value, "red_on") == 0) {
            g_menu_mode = SUB_MODE_SELECT_MODE;
            g_current_type = RED_ON;
            printf("red on\r\n");
        } 
        if (strcmp(value, "green_on") == 0) {
            g_menu_mode = SUB_MODE_SELECT_MODE;
            g_current_type = YELLOW_ON;
            printf("green on\r\n");               
        }
         if (strcmp(value, "blue_on") == 0) {
            g_menu_mode = SUB_MODE_SELECT_MODE;
            g_current_type = GREEN_ON;
            printf("blue on\r\n");  
        }
         if (strcmp(value, "purple_on") == 0) {
            g_menu_mode = SUB_MODE_SELECT_MODE;
            g_current_type = PWM_CONTROL_PURPLE;
            printf("purple on\r\n");  
        }
         if (strcmp(value, "white_on") == 0) {
            g_menu_mode = SUB_MODE_SELECT_MODE;
            g_current_type = PWM_CONTROL_ALL;
            printf("white on\r\n");  
        }
    }
    /*brightness led control*/
    if (strcmp(property, "br_s") == 0) {
        g_key_down_flag = KEY_GPIO_7;
        g_menu_mode = MAIN_FUNCTION_SELECT_MODE;
        g_menu_select = COLORFUL_LIGHT_MENU;
        g_current_mode = BIRGHTNESS_MODE;
        if (strcmp(value, "br_l") == 0) {
            g_menu_mode = SUB_MODE_SELECT_MODE;
            g_current_type = BRIGHTNESS_LOW;
            printf("birghtness low\r\n");
        }
         if (strcmp(value, "br_m") == 0) {
            g_menu_mode = SUB_MODE_SELECT_MODE;
            g_current_type = BRIGHTNESS_MIDDLE;
            printf("brightness middle\r\n");               
        }
         if (strcmp(value, "br_h") == 0) {
            g_menu_mode = SUB_MODE_SELECT_MODE;
            g_current_type = BRIGHTNESS_HIGH;
            printf("brightness high\r\n");  
        } 
    }
    /*human detect*/
    if (strcmp(property, "ht_s") == 0) {
        g_key_down_flag = KEY_GPIO_7;
        g_menu_mode = MAIN_FUNCTION_SELECT_MODE;
        g_menu_select = COLORFUL_LIGHT_MENU;
        if (strcmp(value, "ht") == 0) {
            g_menu_mode = SUB_MODE_SELECT_MODE;
            g_current_mode = HUMAN_DETECT_MODE;
            printf("human_detect_start\r\n");
        }
    }
    /*light detect*/
    if (strcmp(property, "lt_s") == 0) {
        g_key_down_flag = KEY_GPIO_7;
        g_menu_mode = MAIN_FUNCTION_SELECT_MODE;
        g_menu_select = COLORFUL_LIGHT_MENU;
        if (strcmp(value, "lt") == 0) {
            g_menu_mode = SUB_MODE_SELECT_MODE;
            g_current_mode = LIGHT_DETECT_MODE;
            printf("light_detect_start\r\n");
        }
    }
    /*union detect*/
    if (strcmp(property, "ut_s") == 0) {
        g_key_down_flag = KEY_GPIO_7;
        g_menu_mode = MAIN_FUNCTION_SELECT_MODE;
        g_menu_select = COLORFUL_LIGHT_MENU;
        if (strcmp(value, "ut") == 0) {
            g_menu_mode = SUB_MODE_SELECT_MODE;
            g_current_mode = UNION_DETECT_MODE;
            printf("union_detect_start\r\n");
        }
    }
}
/*
	@berf histreaming traffic light function control
	@param const char* property: Eigenvalues sent by app
	@param  char* value:Value from App
*/
static hi_void histreaming_traffic_light_control(const char* property, char* value)
{
    /*manual control module*/
    if (strcmp(property, "tl_s") == 0) {
        g_key_down_flag = KEY_GPIO_7;
        g_menu_mode = MAIN_FUNCTION_SELECT_MODE;
        g_menu_select = TRAFFIC_LIGHT_MENU;
        g_current_mode = TRAFFIC_CONTROL_MODE;
        if (strcmp(value, "red_on") == 0) {
            // g_menu_mode = SUB_MODE_SELECT_MODE;
            g_current_type = RED_ON;
            printf("traffic light red on\r\n");
        }
        if (strcmp(value, "yellow_on") == 0) {
            g_menu_mode = SUB_MODE_SELECT_MODE;
            g_current_type = YELLOW_ON;
            printf("traffic light yellow on\r\n");               
        }
        if (strcmp(value, "green_on") == 0) {
            g_menu_mode = SUB_MODE_SELECT_MODE;
            g_current_type = GREEN_ON;
            printf("traffic light green on\r\n");  
        }
    }
    /*auto module*/
    if (strcmp(property, "tla_s") == 0) {
        g_key_down_flag = KEY_GPIO_7;
        g_menu_mode = MAIN_FUNCTION_SELECT_MODE;
        g_menu_select = TRAFFIC_LIGHT_MENU;
        if (strcmp(value, "tla") == 0) {
            g_menu_mode = SUB_MODE_SELECT_MODE;
            g_current_mode = TRAFFIC_AUTO_MODE;
            printf("traffic light auto module start\r\n");
        } 
    }
    /*human module normal*/
    if (strcmp(property, "tlh_s") == 0) {
        g_key_down_flag = KEY_GPIO_7;
        g_menu_mode = MAIN_FUNCTION_SELECT_MODE;
        g_menu_select = TRAFFIC_LIGHT_MENU;
        if (strcmp(value, "tlh") == 0) {
            g_menu_mode = SUB_MODE_SELECT_MODE;
            g_current_mode = TRAFFIC_HUMAN_MODE;
            g_current_type =TRAFFIC_NORMAL_TYPE;
            printf("traffic light human module start\r\n");
        } 
    }
    /*human module human control*/
    if (strcmp(property, "tlm_s") == 0) {
        g_key_down_flag = KEY_GPIO_7;
        g_menu_mode = MAIN_FUNCTION_SELECT_MODE;
        g_menu_select = TRAFFIC_LIGHT_MENU;
        if (strcmp(value, "tlm") == 0) {
            g_menu_mode = SUB_MODE_SELECT_MODE;
            g_current_mode = TRAFFIC_HUMAN_MODE;
            g_current_type = TRAFFIC_HUMAN_TYPE;
            printf("traffic light human control start\r\n");
        } 
    }
}
/**
	@berf histreaming environment function control
	@param const char* property: Eigenvalues sent by app
	@param  char* value:Value from App
*/
static hi_void histreaming_environment_control(const char* property, char* value)
{

    /*get temperature/humidity/G_Gas value*/
    if (strcmp(property, "ena_s") == 0) {
        g_key_down_flag = KEY_GPIO_7;
        g_menu_mode = MAIN_FUNCTION_SELECT_MODE;
        g_menu_select = ENVIRONMENT_MENU;
        if (strcmp(value, "ena") == 0) {
            // g_menu_mode = SUB_MODE_SELECT_MODE;
            g_current_mode = ENV_ALL_MODE;
            printf("environment all control start\r\n");
        } 
    }
    /*get temperature value*/
    if (strcmp(property, "ent_s") == 0) {
        g_key_down_flag = KEY_GPIO_7;
        g_menu_mode = MAIN_FUNCTION_SELECT_MODE;
        g_menu_select = ENVIRONMENT_MENU;
        if (strcmp(value, "ent") == 0) {
            g_menu_mode = SUB_MODE_SELECT_MODE;
            g_current_mode = ENV_TEMPERRATURE_MODE;
            printf("environment temperature control start\r\n");
        } 
    }
    /*get humidity value*/
    if (strcmp(property, "enh_s") == 0) {
        g_key_down_flag = KEY_GPIO_7;
        g_menu_mode = MAIN_FUNCTION_SELECT_MODE;
        g_menu_select = ENVIRONMENT_MENU;
        if (strcmp(value, "enh") == 0) {
            g_menu_mode = SUB_MODE_SELECT_MODE;
            g_current_mode = ENV_HUMIDITY_MODE;
            printf("environment humiduty control start\r\n");
        } 
    }
    /*get G_Gas value*/
    if (strcmp(property, "eng_s") == 0) {
        g_key_down_flag = KEY_GPIO_7;
        g_menu_mode = MAIN_FUNCTION_SELECT_MODE;
        g_menu_select = ENVIRONMENT_MENU;
        if (strcmp(value, "eng") == 0) {
            g_menu_mode = SUB_MODE_SELECT_MODE;
            g_current_mode = COMBUSTIBLE_GAS_MODE;
            printf("environment g_gas control start\r\n");
        } 
    }
}
/**
	@berf histreaming nfc function control
	@param const char* property: Eigenvalues sent by app
	@param  char* value:Value from App
*/
static hi_void histreaming_nfc_control(const char* property, char* value)
{
    /*wechat*/
    if (strcmp(property, "nfw_s") == 0) {
        g_key_down_flag = KEY_GPIO_7;
        g_menu_mode = MAIN_FUNCTION_SELECT_MODE;
        g_menu_select = NFC_TEST_MENU;
        if (strcmp(value, "nfw") == 0) {
            g_current_mode = NFC_TAG_WECHAT_MODE;
            printf("nfc wechat control start\r\n");
        }     
    }
    /*today headline*/
    if (strcmp(property, "nfth_s") == 0) {
        g_key_down_flag = KEY_GPIO_7;
        g_menu_mode = MAIN_FUNCTION_SELECT_MODE;
        g_menu_select = NFC_TEST_MENU;
        if (strcmp(value, "nfth") == 0) {
            g_menu_mode = SUB_MODE_SELECT_MODE;
            g_current_mode = NFC_TAG_TODAY_HEADLINE_MODE;
            printf("nfc today headline control start\r\n");
        } 
    }
    /*taobao*/
    if (strcmp(property, "nft_s") == 0) {
        g_key_down_flag = KEY_GPIO_7;
        g_menu_mode = MAIN_FUNCTION_SELECT_MODE;
        g_menu_select = NFC_TEST_MENU;
        if (strcmp(value, "nft") == 0) {
            g_menu_mode = SUB_MODE_SELECT_MODE;
            g_current_mode = NFC_TAG_TAOBAO_MODE;
            printf("nfc taobao control start\r\n");
        } 
    } 
    /*huawei smart lift*/
    if (strcmp(property, "nfhs_s") == 0) {
        g_key_down_flag = KEY_GPIO_7;
        g_menu_mode = MAIN_FUNCTION_SELECT_MODE;
        g_menu_select = NFC_TEST_MENU;
        if (strcmp(value, "nfhs") == 0) {
            g_menu_mode = SUB_MODE_SELECT_MODE;
            g_current_mode = NFC_TAG_HUAWEI_LIFE_MODE;
            printf("nfc huawei smart lift control start\r\n");
        } 
    }
    /*histreaming*/
    if (strcmp(property, "nfh_s") == 0) {
        g_key_down_flag = KEY_GPIO_7;
        g_menu_mode = MAIN_FUNCTION_SELECT_MODE;
        g_menu_select = NFC_TEST_MENU;
        if (strcmp(value, "nfh") == 0) {
            g_menu_mode = SUB_MODE_SELECT_MODE;
            g_current_mode = NFC_TAG_HISTREAMING_MODE;
            printf("nfc histreaming control start\r\n");
        } 
    }
}
/**
	@berf histreaming colorful light return main menu
	@param const char* property:Eigenvalues sent by app
	@param  char* value:Value from App
*/
static hi_u32 histreaming_colorful_light_return_main_menu(const char* property, char* value)
{
    if (strcmp(property, "clr_s") == 0) {
        if (strcmp(value, "clr") == 0) {
         printf("colorful_light_return_start\r\n");    
        g_current_mode = RETURN_MODE;
        g_current_type = KEY_DOWN;
        gpio_control(HI_IO_NAME_GPIO_10, HI_GPIO_IDX_10, HI_GPIO_DIR_OUT, HI_GPIO_VALUE0,HI_IO_FUNC_GPIO_10_GPIO);
        gpio_control(HI_IO_NAME_GPIO_11, HI_GPIO_IDX_11, HI_GPIO_DIR_OUT, HI_GPIO_VALUE0,HI_IO_FUNC_GPIO_11_GPIO); 
        gpio_control(HI_IO_NAME_GPIO_12, HI_GPIO_IDX_12, HI_GPIO_DIR_OUT, HI_GPIO_VALUE0,HI_IO_FUNC_GPIO_12_GPIO);       
        oled_main_menu_display();                    
        return HI_ERR_SUCCESS;
        }
    }
}
/**
	@berf histreaming traffic light return main menu
	@param const char* property:Eigenvalues sent by app
	@param  char* value:Value from App
*/
static hi_u32 histreaming_traffic_light_return_main_menu(const char* property, char* value)
{
    if (strcmp(property, "tlr_s") == 0) {
        if (strcmp(value, "tlr") == 0) {
            printf("traffic_light_return_start\r\n");    
            g_current_mode = TRAFFIC_RETURN_MODE;
            g_current_type = KEY_DOWN;
            oled_main_menu_display();                    
            return HI_ERR_SUCCESS;
        }
    }
}
/**
	@berf histreaming environment return main menu
	@param const char* property:Eigenvalues sent by app
	@param  char* value:Value from App
*/
static hi_u32 histreaming_environment_return_main_menu(const char* property, char* value)
{
    if (strcmp(property, "enr_s") == 0) {
        if (strcmp(value, "enr") == 0) {
            printf("environment return start\r\n");  
            g_current_mode = ENV_RETURN_MODE;
            g_current_type = KEY_DOWN;
            oled_main_menu_display();                    
            return HI_ERR_SUCCESS;
        }
    }
}
/**
	@berf histreaming nfc return main menu
	@param const char* property: Eigenvalues sent by app
	@param  char* value:Value from App
*/
static hi_u32 histreaming_nfc_return_main_menu(const char* property, char* value)
{
    if (strcmp(property, "nfr_s") == 0) {
        if (strcmp(value, "nfr") == 0) {
            printf("nfc return start\r\n");  
            g_current_mode = NFC_RETURN_MODE;
            g_current_type = KEY_DOWN;
            oled_main_menu_display();                    
            return HI_ERR_SUCCESS;
        }
    }
}
/**
	@berf  The device side sends the characteristic value to the app side
	@param struct LinkService* ar: histreaming LinkServer structural morphology
	@param const char* property: characteristic value
	@param char* value: send value to app 
	@param int len: send value length

    设备端将特征值发送给应用端
@param struct LinkService* ar: histreaming LinkServer结构形态
@param const char*属性:特征值
@param char* value:发送值到app
@param int len:发送值长度
*/
static int GetStatusValue(struct LinkService* ar, const char* property, char* value, int len)
{
    (void)(ar);

    printf("Receive property: %s(value=%s[%d])\n", property, value, len);

    if (strcmp(property, "Status") == 0) {
        strcpy(value, "Opend");
    }

    /*
     * if Ok return 0,
     * Otherwise, any error, return StatusFailure
     */
    return 0;
}







/** 
	@berf recv from app cmd 
	@berf Receive the message sent by the app, and operate the hi3861 device side accordingly
	@param struct LinkService* ar: histreaming LinkServer structural morphology
	@param const char* property: Eigenvalues sent by app
	@param char* value: Value sent by app
	@param int len: Length of APP sent

    @berf recv from app cmd
@berf接收应用发送的消息，并相应操作hi3861设备侧
@param struct LinkService* ar: histreaming LinkServer结构形态
@param const char*属性:应用程序发送的特征值
@param char* value:应用发送的值
@param int len: APP发送的长度
*/
static int ModifyStatus(struct LinkService* ar, const char* property, char* value, int len)
{
    (void)(ar);

    if (property == NULL || value == NULL) {
        return -1;
    }
    /* modify status property*/
    /*colorful light module*/
    histreaming_colorful_light_control(property, value);
    /*traffic light module*/
    histreaming_traffic_light_control(property, value);
    /*environment*/
    histreaming_environment_control(property, value);
    /*nfc*/
    histreaming_nfc_control(property, value);
    /*colorful light return main menu*/
    histreaming_colorful_light_return_main_menu(property, value);
    /*traffic light return main menu*/
    histreaming_traffic_light_return_main_menu(property, value);
    /*environment return main menu*/
    histreaming_environment_return_main_menu(property, value);
    /*nfc return main menu*/
    histreaming_nfc_return_main_menu(property, value);
    printf("Receive property: %s(value=%s[%d])\n", property, value,len);
    /*
     * if Ok return 0,
     * Otherwise, any error, return StatusFailure
     */
    return 0;
}

/*
 * It is a Wifi IoT device
 */
static const char* g_wifista_type = "Light";
/**
	@berf The app side obtains the device name
	@param struct LinkService* ar: histreaming LinkServer structural morphology
*/
static const char* GetDeviceType(struct LinkService* ar)
{
    (void)(ar);

    return g_wifista_type;
}

static void *g_link_platform = NULL;

void* histreaming_open(void)
{
    hi_u32 ret = hi_gpio_init();
    if (ret != HI_ERR_SUCCESS) {
        printf("===== ERROR ===== gpio -> hi_gpio_init ret:%d\r\n", ret);
        return NULL;
    }
    else{
        /* code */
        printf("----- gpio init success-----\r\n");
    }
    
    LinkService* wifiIot = 0;
    LinkPlatform* link = 0;

    wifiIot = (LinkService*)malloc(sizeof(LinkService));
    if (!wifiIot){
        printf("malloc wifiIot failure\n");
        return NULL;
    }

    wifiIot->get    = GetStatusValue;
    wifiIot->modify = ModifyStatus;
    wifiIot->type = GetDeviceType;

    link = LinkPlatformGet();
    if (!link) {
        printf("get link failure\n");
        return NULL;
    }

    if (link->addLinkService(link, wifiIot, 1) != 0) {
        histreaming_close(link);
        return NULL;
    }

    if (link->open(link) != 0) {
        histreaming_close(link);
        return NULL;
    }

    /* cache link ptr*/
    g_link_platform = (void*)(link);
#ifdef HISTREAMING_TASK    
    hi_task_delete(g_histreaming_demo_task_id);
#endif
    return (void*)link;
}

void histreaming_close(void *link)
{
    LinkPlatform *linkPlatform = (LinkPlatform*)(link);
    if (!linkPlatform) {
        return;
    }

    linkPlatform->close(linkPlatform);

    if (linkPlatform != NULL) {
        LinkPlatformFree(linkPlatform);
    }
}
#ifdef HISTREAMING_TASK 
hi_void histreaming_demo(hi_void)
{
    hi_u32 ret;
    hi_task_attr histreaming ={0};
    histreaming.stack_size = HISTREAMING_DEMO_TASK_STAK_SIZE;
    histreaming.task_prio = HISTREAMING_DEMO_TASK_PRIORITY;
    histreaming.task_name = "histreaming_demo";
    ret = hi_task_create(&g_histreaming_demo_task_id, &histreaming, histreaming_open, HI_NULL);
    if (ret != HI_ERR_SUCCESS) {
        printf("Falied to create histreaming demo task!\n");
    }
}
#endif
#endif