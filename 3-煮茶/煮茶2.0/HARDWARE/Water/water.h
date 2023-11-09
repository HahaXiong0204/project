#ifndef _WATER_H
#define _WATER_H

#include "sys.h"



// 光敏传感器
void Water_Init(void);
u16 Water_Get(void);
u16 Get_Water_Adc_Average(u8 times);//求平均值



#endif
