#ifndef __DUOJI_H
#define __DUOJI_H

#include "main.h"
#include "delay.h"

#define Degrees_OPEN    50  
#define Degrees_CLOSE	250 

#define Degrees_0    50  //��ת2��
#define Degrees_45	100 //��ת1��
#define Degrees_90   150 //ͣ
#define Degrees_135  200 //��ת1��
#define Degrees_180  250 //��ת2��
extern TIM_HandleTypeDef htim5;

void SG90_Rotate(uint16_t ch,uint16_t Degrees);

#endif


