#include "key.h"
#include "delay.h"


//����������
//���ذ���ֵ
//mode:0,��֧��������;1,֧��������;
//0��û���κΰ�������
//1��WKUP���� WK_UP
//ע��˺�������Ӧ���ȼ�,KEY0>KEY1>KEY2>WK_UP!!
u8 KEY_Scan(uint8_t mode)
{
    static u8 key_up = 1;   //�����ɿ���־
    if(mode == 1)key_up = 1; //֧������
    if(key_up && (KEY0 == 0 || KEY1 == 0 || KEY2 == 0 || WK_UP == 1 || bom_red == 0 ||bom_yellow ==0 || bom_green ==0))
    {
        HAL_Delay(10);
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
    return 0;   //�ް�������
}
