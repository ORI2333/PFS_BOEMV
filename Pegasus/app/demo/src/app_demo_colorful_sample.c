
// APP控制相关文件

#include <app_demo_multi_sample.h>
#include <app_demo_colorful_sample.h>
#include <hi_i2c.h>
hi_s32 Flage_1 = 5;
hi_s32 Flage_2 = 5;
hi_s32 Flage_3 = 0;
hi_s32 Flag_temp_1 = 0;
hi_s32 Flag_temp_2 = 0;

// APP控制函数
hi_void colorful_light_func(hi_void)
{
    // 再次初始化，确保状态
     hi_io_set_func(HI_IO_NAME_GPIO_9, HI_IO_FUNC_GPIO_9_GPIO);
     hi_gpio_set_dir(HI_GPIO_IDX_9, HI_GPIO_DIR_OUT);
     hi_io_set_func(HI_IO_NAME_GPIO_10, HI_IO_FUNC_GPIO_10_GPIO);
     hi_gpio_set_dir(HI_GPIO_IDX_10, HI_GPIO_DIR_OUT);

    /*Key interrupt initialization*/
    extern hi_void test_gpio_init(hi_void);
    test_gpio_init();
    //clear screen 
    oled_fill_screen(OLED_CLEAN_SCREEN);
    // oled_show_str(0,0, "WiFi-AP  ON  U:1",1);
    // oled_show_str(15,1, "Colorful Light",1);
    // oled_show_str(0,2, "                ",1);
    // oled_show_str(0,3, "Mode:           ",1);
    // oled_show_str(0,4, "1.Control Mode  ",1);
    // oled_show_str(0,6, "Type:           ",1);

    g_current_type = KEY_UP; 
    
    while(1){
        switch (g_current_mode) {
            case CONTROL_MODE:
            //   oled_show_str(0,3, "Mode:           ",1);
            //   oled_show_str(0,4, "1.Control Mode",1);
            //   oled_show_str(0,6, "Type:           ",1);
            //   oled_show_str(0,, "Weight: ",1);
            //   hx711_weight_1
                 switch (g_current_type) {
                     case FORWARD:      //向下移动
                         // oled_show_str(0,7, "1.Move down     ",1);                                        
                          break;
                     case BACK:    //向上移动
                         // oled_show_str(0,7, "2.Move Up       ",1);    
                          break;
                     case STOP:   // 复位
                         // oled_show_str(0,7, "3.Stop       ",1);
                          break;
                    case LEFT:   // 向左移动
                        //  oled_show_str(0,7, "4.Move Lift    ",1);
                          break;
                    case RIGHT:   // 向右移动
                        //  oled_show_str(0,7, "5.Move Right    ",1);
                          break;
                     default:
                          break;

                          
                 }
                 hi_sleep(SLEEP_1_MS);

               
               if(Flag_temp_1 != Flage_1){
                  if(Flage_1 <= 10){
                         hi_gpio_set_ouput_val(HI_GPIO_IDX_10,HI_GPIO_VALUE1);
                         hi_udelay(600+Flage_1*100);  
                         hi_gpio_set_ouput_val(HI_GPIO_IDX_10,HI_GPIO_VALUE0);
                         hi_udelay(20000-(600+Flage_1*100));   
                  }  

                  if(Flage_1 > 10) Flage_1 = 10;
                  if(Flage_1 < 0)  Flage_1 = 0;
               }
               Flag_temp_1 = Flage_1;


                if(Flag_temp_2 != Flage_2){
                  if(Flage_2 <= 10){
                         hi_gpio_set_ouput_val(HI_GPIO_IDX_9,HI_GPIO_VALUE1);
                         hi_udelay(300+Flage_2*100);  
                         hi_gpio_set_ouput_val(HI_GPIO_IDX_9,HI_GPIO_VALUE0);
                         hi_udelay(20000-(300+Flage_2*100));   
                  }  

                  if(Flage_2 > 10) Flage_2 = 10;
                  if(Flage_2 < 0)  Flage_2 = 0;
               }
               Flag_temp_2 = Flage_2;


                 break;
            default:
                break;
        }
    }
    return;
}