#include "HC_SR04.h"


extern uint16_t high_time;

void Read_diatance(void)//���������
{
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15,GPIO_PIN_SET);
  HAL_Delay(1);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15,GPIO_PIN_RESET);		
  HAL_TIM_IC_CaptureCallback(&htim2);
}

void HC_SR04_value(TIM_HandleTypeDef *htim)
{
	static uint32_t capture_buf[2]={0,0};  //����������
  static uint8_t capture_cnt=0;          //ѭ������
  if(capture_cnt==0)
	{
		capture_cnt++;
		__HAL_TIM_SET_CAPTUREPOLARITY(&htim2,TIM_CHANNEL_2,TIM_INPUTCHANNELPOLARITY_RISING);  //���������ز���
		//TIM_RESET_CAPTUREPOLARITY
		HAL_TIM_IC_Start_IT(&htim2,TIM_CHANNEL_2);  //��������
	}
	if(capture_cnt==3)
	{ 
 		high_time = capture_buf[1]- capture_buf[0];
 		capture_cnt=0;
	}
	if(TIM2== htim->Instance)
	{
		if(HAL_TIM_ACTIVE_CHANNEL_2 == htim->Channel)
		{
		  switch(capture_cnt)
			{
			  case 1:
					capture_buf[0]= __HAL_TIM_GET_COMPARE(htim,TIM_CHANNEL_2);
			    __HAL_TIM_SET_CAPTUREPOLARITY(htim,TIM_CHANNEL_2,TIM_INPUTCHANNELPOLARITY_FALLING); //���������½���
				  capture_cnt++;
				  break;
			  case 2:
					capture_buf[1]= __HAL_TIM_GET_COMPARE(htim,TIM_CHANNEL_2); 
				  capture_cnt++;
				  HAL_TIM_IC_Stop_IT(htim,TIM_CHANNEL_2); //ֹͣ���� 
				  break;				 
				 default: break;
			}		
		}	
	}	
}


