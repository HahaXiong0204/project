#include "iwdg.h"

//��ʼ���������Ź�
//prer:��Ƶ��:0~7(ֻ�е� 3 λ��Ч!)
//��Ƶ����=4*2^prer.�����ֵֻ���� 256!
//rlr:��װ�ؼĴ���ֵ:�� 11 λ��Ч.
//ʱ�����(���):Tout=((4*2^prer)*rlr)/40 (ms).
void IWDG_Init(u8 prer,u16 rlr)
{
	// 1��ȡ���Ĵ���д���� д0x5555
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
	// 2�����ö������Ź�Ԥ��Ƶϵ��
	IWDG_SetPrescaler(prer);
	// 3�����ö������Ź���װ��ֵ
	IWDG_SetReload(rlr);;
	// 4�����ؼ���ֵι�� д0xAAAA
	IWDG_ReloadCounter();
	// 5���������Ź�     д0xCCCC
	IWDG_Enable();
}
void IWDG_Feed(void)
{
	IWDG_ReloadCounter();
}