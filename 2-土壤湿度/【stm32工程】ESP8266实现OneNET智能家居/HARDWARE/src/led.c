 
/*-------------------------------------------------*/
/*         	                                       */
/*           	 LED������ƣ�����           	    */
/*                                                 */
/*-------------------------------------------------*/

// Ӳ�����ӣ�
// PC13  LED0

#include "stm32f10x.h"  //������Ҫ��ͷ�ļ�
#include "led.h"        //������Ҫ��ͷ�ļ�

extern u8 fan;

extern u8 shui;

/*-------------------------------------------------*/
/*����������ʼ��LED����,PC13,���������� 	    */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void LED_Init(void)
{    	 
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	 //ʹ�ܶ˿�ʱ��

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;	    		 //�˿�����, �������
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
	GPIO_Init(GPIOC, &GPIO_InitStructure);	  				 //������� ��IO���ٶ�Ϊ50MHz
	
	LED_Off();
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


/*-------------------------------------------------*/
/*����������������ʼ������,PB12,�͵�ƽ���� 	    */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void Buzzer_Init(void)//��������ʼ��
{

	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	 //ʹ�ܶ˿�ʱ��

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;	    		 //�˿�����, �������
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);	  				 //������� ��IO���ٶ�Ϊ50MHz

}
void Buzzer_on(void)
{
  GPIO_ResetBits(GPIOA,GPIO_Pin_5);
}

void Buzzer_off(void)
{ 
 GPIO_SetBits(GPIOA,GPIO_Pin_5);
}

/*-------------------------------------------------*/
/*���������̵�����ʼ������,PA10,               	    */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void relay_init(void)
{
  GPIO_InitTypeDef     GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15|GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_SetBits(GPIOB,GPIO_Pin_15);
    GPIO_SetBits(GPIOB,GPIO_Pin_9);
    fan = 0;
    shui =0;
}          


void relay_fan_off(void)
{
    printf("fan  ��\r\n");
	 GPIO_SetBits(GPIOB,GPIO_Pin_15);
     fan =0;
}

void relay_fan_on(void)
	
{ 
    printf("fan  ��\r\n");
    GPIO_ResetBits(GPIOB,GPIO_Pin_15);
     fan = 1;
}



void relay_shui_off(void)
{
    printf("shui  ��\r\n");
	GPIO_SetBits(GPIOB,GPIO_Pin_9);
    shui = 0;
 
}

void relay_shui_on(void)
{ 
     printf("shui  ��\r\n");
    GPIO_ResetBits(GPIOB,GPIO_Pin_9);
    shui =1;
}



void relay_on(void)
{
	 GPIO_ResetBits(GPIOA,GPIO_Pin_10);
 
}

void relay_off(void)
	
{ 
  GPIO_SetBits(GPIOA,GPIO_Pin_10);
}

/*-------------------------------------------------*/
/*���������̵�����ʼ������,PA9,               	    */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void relay1_init(void)
{
  GPIO_InitTypeDef     GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOA,GPIO_Pin_9);
}          

void relay1_on(void)
{
	 GPIO_ResetBits(GPIOA,GPIO_Pin_9);
 
}

void relay1_off(void)
	
{ 
  GPIO_SetBits(GPIOA,GPIO_Pin_9);
}
/*-------------------------------------------------*/
/*���������̵�����ʼ������,PA8,               	    */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void relay2_init(void)
{
  GPIO_InitTypeDef     GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOA,GPIO_Pin_8);
}          

void relay2_on(void)
{
	 GPIO_ResetBits(GPIOA,GPIO_Pin_8);
 
}

void relay2_off(void)
	
{ 
  GPIO_SetBits(GPIOA,GPIO_Pin_8);
}

/*-------------------------------------------------*/
/*���������̵�����ʼ������,PB15,               	    */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void relay3_init(void)
{
  GPIO_InitTypeDef     GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOB,GPIO_Pin_15);
}          

void relay3_on(void)
{
	 GPIO_ResetBits(GPIOB,GPIO_Pin_15);
 
}

void relay3_off(void)
	
{ 
  GPIO_SetBits(GPIOB,GPIO_Pin_15);
}
