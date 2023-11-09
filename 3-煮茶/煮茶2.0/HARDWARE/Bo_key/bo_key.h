#ifndef _key_H
#define _key_H

#include "sys.h"
 
#define KEY_LEFT_Pin    GPIO_Pin_13    //����K_LEFT�ܽ�
#define KEY_DOWN_Pin    GPIO_Pin_14    //����K_DOWN�ܽ�
#define KEY_RIGHT_Pin   GPIO_Pin_15   //����K_RIGHT�ܽ�
#define KEY_UP_Pin      GPIO_Pin_11  //����KEY_UP�ܽ�

#define KEY_Port (GPIOC) //����˿�
#define KEY_UP_Port (GPIOB) //����˿�


//ʹ��λ��������
#define K_UP PBin(11)
#define K_DOWN PCin(14)
#define K_LEFT PCin(13)
#define K_RIGHT PCin(15)



//�����������ֵ  
#define KEY_UP 2
#define KEY_DOWN 4
#define KEY_LEFT 3
#define KEY_RIGHT 1  



void KEY_Init(void);
u8 KEY_Scan(u8 mode);
#endif
