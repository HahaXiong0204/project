#include "duoji.h"

void SG90_Rotate(uint16_t ch,uint16_t Degrees)
{
	switch(ch)
	{
		case 1:
			__HAL_TIM_SET_COMPARE(&htim5,TIM_CHANNEL_1,Degrees);   //һ�������ڣ�20ms����0.5ms�ߵ�ƽ
			break;
		case 2:
			__HAL_TIM_SET_COMPARE(&htim5,TIM_CHANNEL_2,Degrees);   //һ�������ڣ�20ms����0.5ms�ߵ�ƽ
			break;
		default:
			break;
	}
    delay_ms(330);
    TIM5->CCR1=150;
}


