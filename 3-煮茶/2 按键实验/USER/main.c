

/* /  
��µ���	  ELH    enlighten  sunny
STM32 ������ �⺯������	  */

//��������LED��ʵ��  
//�����ϵİ��� ��S2 
//��ʵ�� ͨ��S2�������� led���� �����ɿ� LED��



#include  "delay.h"
#include  "led.h"
#include  "key.h"


int main(void)
   {

   	delay_init();	     //��ʱ������ʼ��	  
	LED_Init();		  	//��ʼ����LED���ӵ�Ӳ���ӿ�
	KEY_Init();         //��ʼ������
   while(1)
   {
 
        if(KEY0==0)
		  {
		    GPIO_ResetBits(GPIOA,GPIO_Pin_1);		  //��PA1��0  ����LED
		  }

		   GPIO_SetBits(GPIOA,GPIO_Pin_1);		      //��PA1��1  �ر�LED

          }
   }

