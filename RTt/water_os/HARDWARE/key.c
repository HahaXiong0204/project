#include "key.h"
#include "delay.h"


//按键处理函数
//返回按键值
//mode:0,不支持连续按;1,支持连续按;
//0，没有任何按键按下
//1，WKUP按下 WK_UP
//注意此函数有响应优先级,KEY0>KEY1>KEY2>WK_UP!!
u8 KEY_Scan(uint8_t mode)
{
    static u8 key_up = 1;   //按键松开标志
    if(mode == 1)key_up = 1; //支持连按
    if(key_up && (KEY0 == 0 || KEY1 == 0 || KEY2 == 0 || WK_UP == 1 || bom_red == 0 ||bom_yellow ==0 || bom_green ==0))
    {
        rt_thread_mdelay(10);
        key_up = 0;
        if(KEY0 == 0)       return KEY0_PRES;
        else if(KEY1 == 0)  return KEY1_PRES;
        else if(KEY2 == 0)  return KEY2_PRES;
        else if(WK_UP == 1) return WKUP_PRES;
		else if(bom_red == 0) return RED_PRES;
		else if(bom_yellow == 0)return YELLOW_PRES;
		else if(bom_green == 0)return GRREN_PRES;
    }
    else if(KEY0 == 1 && KEY1 == 1 && KEY2 == 1 && WK_UP == 0 && bom_red == 1 && bom_yellow == 1 && bom_green == 1 )key_up = 1;
    return 0;   //无按键按下
}
