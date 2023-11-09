#ifndef _key_H
#define _key_H

#include "sys.h"
 
#define KEY_LEFT_Pin    GPIO_Pin_13    //定义K_LEFT管脚
#define KEY_DOWN_Pin    GPIO_Pin_14    //定义K_DOWN管脚
#define KEY_RIGHT_Pin   GPIO_Pin_15   //定义K_RIGHT管脚
#define KEY_UP_Pin      GPIO_Pin_11  //定义KEY_UP管脚

#define KEY_Port (GPIOC) //定义端口
#define KEY_UP_Port (GPIOB) //定义端口


//使用位操作定义
#define K_UP PBin(11)
#define K_DOWN PCin(14)
#define K_LEFT PCin(13)
#define K_RIGHT PCin(15)



//定义各个按键值  
#define KEY_UP 2
#define KEY_DOWN 4
#define KEY_LEFT 3
#define KEY_RIGHT 1  



void KEY_Init(void);
u8 KEY_Scan(u8 mode);
#endif
