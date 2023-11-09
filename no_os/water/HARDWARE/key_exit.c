#include "key_exit.h"
#include "PCF8574.h"
#include "delay.h"
#include "onenet.h"

extern uint8_t beep_flag;
uint8_t state_flag = 0;  //
uint8_t water_key_flag = 0;   //出水 0 关闭  1 热水 2 温水

extern uint8_t water_end;    // 饮水结束

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	delay_us(100000);
	printf("key_exit\r\n");
	
	
	
	if(GPIO_Pin == bom_red_Pin)
	{
		if(HAL_GPIO_ReadPin(bom_red_GPIO_Port,bom_red_Pin)==0)
		{
			
			OneNet_Send_One_String_Data("STATE_CODE","Error1");
			water_flag = 1;
			printf("red user end\r\n");
		}
	}
	
	if(GPIO_Pin == bom_green_Pin)
	{
		if(HAL_GPIO_ReadPin(bom_green_GPIO_Port,bom_green_Pin)==1)
		{
			
			water_end = 1;
			printf("green\r\n");
		}
	}
	
	if(GPIO_Pin == bom_yellow_Pin)
	{
		if(HAL_GPIO_ReadPin(bom_yellow_GPIO_Port,bom_yellow_Pin)==1)
		{
			beep_flag = 2;
//			OneNet_Send_One_String_Data("STATE_CODE","Warning1");
			water_flag = 1;
			printf("yellow\r\n");
		}
	}
	
	if(water_flag==2)
	{
		// 出热水
		if(GPIO_Pin == GPIO_PIN_0)    // WK_UP
		{
			if(HAL_GPIO_ReadPin(WK_UP_GPIO_Port,WK_UP_Pin)==1)
			{
				printf("热水控制\r\n");
				if(water_key_flag==1)
				{
					// LED1关
					HAL_GPIO_WritePin(LED1_GPIO_Port,LED1_Pin,1);
					// LED0关
					HAL_GPIO_WritePin(LED0_GPIO_Port,LED0_Pin,1);
					water_key_flag = 0;
					temp_water = 0;
				}
				else
				{
					// LED1开
					HAL_GPIO_WritePin(LED1_GPIO_Port,LED1_Pin,0);
					// LED0关
					HAL_GPIO_WritePin(LED0_GPIO_Port,LED0_Pin,1);
					
					water_key_flag = 1;
				}
				
			}

		}
		
		// key0出温水 
		if(GPIO_Pin == KEY0_Pin)   // KEY0
		{
			if(HAL_GPIO_ReadPin(KEY0_GPIO_Port,KEY0_Pin)==0)
			{
				printf("出温水\r\n");
				if(water_key_flag==2)
				{
					// LED1关
					HAL_GPIO_WritePin(LED1_GPIO_Port,LED1_Pin,1);
					// LED0关
					HAL_GPIO_WritePin(LED0_GPIO_Port,LED0_Pin,1);
					water_key_flag = 0;
					temp_water = 0;
				}
				else{
					// LED1关
					HAL_GPIO_WritePin(LED1_GPIO_Port,LED1_Pin,1);
					// LED0开
					HAL_GPIO_WritePin(LED0_GPIO_Port,LED0_Pin,0);
					
					water_key_flag = 2;
				}
				
			}
			

		}
		
		
		// key 1设置水位低于最低水位
		if(GPIO_Pin == KEY1_Pin)
		{
			if(HAL_GPIO_ReadPin(KEY1_GPIO_Port,KEY1_Pin)==0)
			{
				water_flow += 2;
			}
			
		}

		// key2 烧水
		if(GPIO_Pin == KEY2_Pin)   // KEY2
		{
			if(HAL_GPIO_ReadPin(KEY2_GPIO_Port,KEY2_Pin)==0)
			{
				temp_water_flag = 1;
			}
			
		}
		
	}
	
	


//printf("pwm_mach = %d\r\n",pwm_mach);

}
