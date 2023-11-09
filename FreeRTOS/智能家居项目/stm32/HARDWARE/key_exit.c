#include "key_exit.h"
#include "PCF8574.h"
#include "delay.h"
//uint8_t beep_flag = 1;


uint8_t fan_num = 0;
uint8_t led_num = 0;
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	
//	HAL_Delay(80);
//	delay_us(900000);
//	delay_us(900000);
//	HAL_Delay(30);
	delay_ms(50);
	// 手
	if(GPIO_Pin == GPIO_PIN_0)    // WK_UP
    {
		if(HAL_GPIO_ReadPin(WK_UP_GPIO_Port,WK_UP_Pin)==1)
		{
			if(mode_flag == 1)
			{
				mode_flag = 2;
			}
			else if(mode_flag==2)
			{
				mode_flag = 1;
			}
			printf("切换模式\r\n");
		
		}
		

    }
	// 风扇
    if(GPIO_Pin == GPIO_PIN_13)   // KEY2
    {
		if(HAL_GPIO_ReadPin(KEY2_GPIO_Port,KEY2_Pin)==0)
		{
			if(mode_flag==2)
			{
				fan_num++;
				if(fan_num>2){
					fan_num = 0;
				}
				
				fan_mode(fan_num);
			}
		}


    }
	
	// led灯
	if(GPIO_Pin ==KEY0_Pin)
	{
		if(HAL_GPIO_ReadPin(KEY0_GPIO_Port,KEY0_Pin)==0)
		{
			if(mode_flag == 2)
			{
				led_num++;
				if(fan_num>2){
					fan_num = 0;
				}
				
				light_mode(led_num);
			}
		}
	}
	
	if(GPIO_Pin == KEY1_Pin)
	{
        if(HAL_GPIO_ReadPin(KEY1_GPIO_Port,KEY1_Pin)==0)
		{
			
		}

	
	}
//	printf("pwm_mach = %d\r\n",pwm_mach);

}
