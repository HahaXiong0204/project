#include "led.h"

//////////////////////////////////////////////////////////////////////////////////
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEKս��STM32������
//LED��������
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/2
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved
//////////////////////////////////////////////////////////////////////////////////


void LED_Init(void)
{

    GPIO_InitTypeDef  GPIO_InitStructure;					 //����GPIO�ṹ��

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��PA�˿�ʱ��

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;			 //LED-->PA1�˿�����
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
    GPIO_Init(GPIOA, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOA.1
    GPIO_SetBits(GPIOA, GPIO_Pin_1);						 //PA.A �����

}




/*-------------------------------------------------*/
/*����������������ʼ������,PB12,�͵�ƽ���� 	    */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void Buzzer_Init(void)//��������ʼ��
{

    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	 //ʹ�ܶ˿�ʱ��

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;	    		 //�˿�����, �������
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
    GPIO_Init(GPIOC, &GPIO_InitStructure);	  				 //������� ��IO���ٶ�Ϊ50MHz

    GPIO_ResetBits(GPIOC, GPIO_Pin_13);
}
void Buzzer_on(void)
{
    GPIO_ResetBits(GPIOC, GPIO_Pin_13);
}

void Buzzer_off(void)
{
    GPIO_SetBits(GPIOC, GPIO_Pin_13);
}

/*-------------------------------------------------*/
/*���������̵�����ʼ������,PA10,               	    */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void relay_init(void)
{
    GPIO_InitTypeDef     GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOA, GPIO_Pin_10);
}

void relay_on(void)
{
    GPIO_ResetBits(GPIOA, GPIO_Pin_10);

}

void relay_off(void)

{
    GPIO_SetBits(GPIOA, GPIO_Pin_10);
}

/*-------------------------------------------------*/
/*���������̵�����ʼ������,PB5,               	    */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void relay1_init(void)
{
    GPIO_InitTypeDef     GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOA, GPIO_Pin_5);
}

void relay1_on(void)
{
    GPIO_ResetBits(GPIOB, GPIO_Pin_5);

}

void relay1_off(void)

{
    GPIO_SetBits(GPIOB, GPIO_Pin_5);
}
/*-------------------------------------------------*/
/*���������̵�����ʼ������,PB14,               	    */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void relay2_init(void)
{
    GPIO_InitTypeDef     GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOB, GPIO_Pin_14);
}

void relay2_on(void)
{
    GPIO_ResetBits(GPIOB, GPIO_Pin_14);

}

void relay2_off(void)

{
    GPIO_SetBits(GPIOB, GPIO_Pin_14);
}

/*-------------------------------------------------*/
/*���������̵�����ʼ������,PB15,               	    */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void relay3_init(void)
{
    GPIO_InitTypeDef     GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOB, GPIO_Pin_15);
}

void relay3_on(void)
{
    GPIO_ResetBits(GPIOB, GPIO_Pin_15);

}

void relay3_off(void)

{
    GPIO_SetBits(GPIOB, GPIO_Pin_15);
}
