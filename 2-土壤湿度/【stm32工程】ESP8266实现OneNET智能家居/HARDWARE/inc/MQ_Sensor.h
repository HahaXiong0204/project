#ifndef __MQ_SENSOR_H__
#define __MQ_SENSOR_H__

#include "stm32f10x.h"
#include "stm32f10x_conf.h"

/* ADC通道宏定义 */
#define MQ135   1
#define MQ2     2
#define GZ      3

void Adc_Init(void);
u16 Get_adcvalue(void);
void ADC_Val_Disp(u8 Page,u8 Colum);

#endif

