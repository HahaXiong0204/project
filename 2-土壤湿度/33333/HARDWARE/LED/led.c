#include "led.h"

//��ʼ��PC13��PA5Ϊ�����.��ʹ���������ڵ�ʱ��		    
//LED IO��ʼ��
void LED_Init(void)
{    	 
	GPIO_InitTypeDef GPIO_InitStructure;                     
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);    

	GPIO_InitStructure.GPIO_Pin = LED_PIN;   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;     
	GPIO_Init(GPIOC, &GPIO_InitStructure); 
	GPIO_SetBits(GPIOC, LED_PIN); 						 //PC13 ����� 
}

/*-------------------------------------------------*/
/*��������LED����                                  */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void LED_On(void)
{			
	GPIO_ResetBits(GPIOC, LED_PIN); 						 //PC13 �����
} 


/*-------------------------------------------------*/
/*��������LED�ر�                                  */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void LED_Off(void)
{		
	GPIO_SetBits(GPIOC, LED_PIN); 						 //PC13 �����
}


void Buzzer_Init(void)//��������ʼ��
{

	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //ʹ�ܶ˿�ʱ��

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;	    		 //�˿�����, �������
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);	  				 //������� ��IO���ٶ�Ϊ50MHz
	
	GPIO_SetBits(GPIOB,GPIO_Pin_12);//����
}
void Buzzer_on(void)
{
  GPIO_ResetBits(GPIOB,GPIO_Pin_12);
}

void Buzzer_off(void)
{ 
 GPIO_SetBits(GPIOB,GPIO_Pin_12);
}


void relay_init(void)
{
  GPIO_InitTypeDef     GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOA,GPIO_Pin_10);
}          

void relay_on(void)
{
  GPIO_SetBits(GPIOA,GPIO_Pin_10);
}

void relay_off(void)
{ 
  GPIO_ResetBits(GPIOA,GPIO_Pin_10);
}
