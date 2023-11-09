/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
unsigned short timeCount = 0;	       //发送间隔变量
unsigned char *dataPtr = NULL;       //onenet需要用到
extern uint8_t aRxBuffer;            //串口数据缓冲区

uint32_t adc_value[30];              //adc数据缓冲区
uint32_t value1=0,value2=0,value3=0; //DMA数据处理的三个中间变量，分别提取光照，音量，烟雾的数据     
uint32_t Lighting=0,Voice=0,Gas=0;   //光照，音量，烟雾的数据 
uint16_t high_time=0;                //记录超声波ECHO引脚高电平时间      
uint8_t temperature=0;               //温度
uint8_t humidity=0;                  //湿度
uint8_t LED_value=1;                   //LED灯的值，0或1
uint8_t fan;                         //风扇开关
uint8_t i=0;                         //for循环用到的变量而已

float Distance;                      //超声波测得距离
char buf1[40];
char buf2[40];
char buf3[40];
char buf4[40];
char buf5[40];
char buf6[40];
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)//定时器输入捕获中断服务函数入口
{
	HC_SR04_value(htim);//捕获超声波的高电平时间
}
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_ADC1_Init();
  MX_I2C1_Init();
  MX_TIM1_Init();
  MX_TIM2_Init();
  MX_TIM4_Init();
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */
	
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
//==================================片内外设初始化====================================================
	HAL_ADC_Start_DMA(&hadc1, (uint32_t *)adc_value, 30); //初始化ADC，用于采集音量，光照，烟雾浓度
//  OLED_Init();//初始化oled屏幕
//	DHT11_Init();//初始化温湿度模块
	
//===================================网络协议初始化===================================================	
  ESP8266_Init();					    //初始化ESP8266
  while(OneNet_DevLink())			//接入OneNET
  HAL_Delay(500);
	
	show();                     //OLED显示主界面
	
  while (1)
  {
//=================================超声波测距==========================================================
		Read_diatance();
		Distance=17.6*high_time/1000;//根据高电平时间算出距离
		sprintf(buf6,"%.1fCM",Distance);
		OLED_ShowString(64,6,(uint8_t *)buf6,16);//显示距离
		
//===============================温湿度传感器==========================================================	
    if(DHT11_Read_Data(&temperature,&humidity) == 0)
		{
			sprintf((char *)buf1,":%d",temperature);
			sprintf((char *)buf2,":%d%%",humidity);
			OLED_ShowString(32,0,(uint8_t *)buf1,16);
			OLED_ShowString(96,0,(uint8_t *)buf2,16);
		}
//=====================（光照，声音，有害气体）三个传感器，通过ADC规则通道查询获取======================
		
    for(i=0,value1=0,value2=0,value3=0;i<30;)
		{
			value1+=adc_value[i++];	
			value2+=adc_value[i++];
			value3+=adc_value[i++];
		}			
		Lighting=(4096-value1/10)*100/4096; //亮度
		Voice=(4096-value2/10)*100/4096;    //声音
		Gas=(value3/10)*100/4096;           //烟雾
		value1=value2=value3=0;
		
		sprintf((char *)buf3,":%d%%",Lighting);
		sprintf((char *)buf4,":%d ",Voice);
		sprintf((char *)buf5,":%d%%",Gas);
		OLED_ShowString(32,2,(uint8_t *)buf3,16);//OLED显示亮度
		OLED_ShowString(96,2,(uint8_t *)buf4,16);//OLED显示声音
		OLED_ShowString(64,4,(uint8_t *)buf5,16);//OLED显示烟雾浓度
		
//=============================================上传数据到OneNet服务器（可以不用管）=================================				
     if(++timeCount >= 500)									//发送间隔5s
		{
			printf("OneNet_SendData\r\n");
			OneNet_SendData();									//发送数据
			timeCount = 0;
			ESP8266_Clear();                    //清空缓存
		}
//==============================================接受OneNet服务器发来的数据（可以不用管）===============================			
		dataPtr = ESP8266_GetIPD(0);
		if(dataPtr != NULL)
		OneNet_RevPro(dataPtr);
		HAL_Delay(10);
		
		
	
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
  /** Enables the Clock Security System
  */
  HAL_RCC_EnableCSS();
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
