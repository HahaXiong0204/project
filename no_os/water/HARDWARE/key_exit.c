#include "key_exit.h"
#include "PCF8574.h"
#include "delay.h"
#include "onenet.h"

extern uint8_t beep_flag;
uint8_t state_flag = 0;  //
uint8_t water_key_flag = 0;   //��ˮ 0 �ر�  1 ��ˮ 2 ��ˮ

extern uint8_t water_end;    // ��ˮ����

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
		// ����ˮ
		if(GPIO_Pin == GPIO_PIN_0)    // WK_UP
		{
			if(HAL_GPIO_ReadPin(WK_UP_GPIO_Port,WK_UP_Pin)==1)
			{
				printf("��ˮ����\r\n");
				if(water_key_flag==1)
				{
					// LED1��
					HAL_GPIO_WritePin(LED1_GPIO_Port,LED1_Pin,1);
					// LED0��
					HAL_GPIO_WritePin(LED0_GPIO_Port,LED0_Pin,1);
					water_key_flag = 0;
					temp_water = 0;
				}
				else
				{
					// LED1��
					HAL_GPIO_WritePin(LED1_GPIO_Port,LED1_Pin,0);
					// LED0��
					HAL_GPIO_WritePin(LED0_GPIO_Port,LED0_Pin,1);
					
					water_key_flag = 1;
				}
				
			}

		}
		
		// key0����ˮ 
		if(GPIO_Pin == KEY0_Pin)   // KEY0
		{
			if(HAL_GPIO_ReadPin(KEY0_GPIO_Port,KEY0_Pin)==0)
			{
				printf("����ˮ\r\n");
				if(water_key_flag==2)
				{
					// LED1��
					HAL_GPIO_WritePin(LED1_GPIO_Port,LED1_Pin,1);
					// LED0��
					HAL_GPIO_WritePin(LED0_GPIO_Port,LED0_Pin,1);
					water_key_flag = 0;
					temp_water = 0;
				}
				else{
					// LED1��
					HAL_GPIO_WritePin(LED1_GPIO_Port,LED1_Pin,1);
					// LED0��
					HAL_GPIO_WritePin(LED0_GPIO_Port,LED0_Pin,0);
					
					water_key_flag = 2;
				}
				
			}
			

		}
		
		
		// key 1����ˮλ�������ˮλ
		if(GPIO_Pin == KEY1_Pin)
		{
			if(HAL_GPIO_ReadPin(KEY1_GPIO_Port,KEY1_Pin)==0)
			{
				water_flow += 2;
			}
			
		}

		// key2 ��ˮ
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
