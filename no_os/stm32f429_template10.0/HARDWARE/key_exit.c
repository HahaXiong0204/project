#include "key_exit.h"
#include "PCF8574.h"

uint8_t beep_flag = 1;

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(GPIO_Pin==GPIO_PIN_0)      // WK_UP
	{
		beep_On_Off(beep_flag);
		beep_flag =!beep_flag;
	}
	
	if(GPIO_Pin == GPIO_PIN_13)   // KEY2
	{
		beep_On_Off(beep_flag);
		beep_flag =!beep_flag;
	
	}


}
