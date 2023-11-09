/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2023 STMicroelectronics.
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
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "dht11.h"
#include "wifi.h"
#include "onenet.h"
#include "oled.h"
#include "string.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define GPIO_Pin_PB6 GPIO_PIN_6
#define GPIO_Port_PB6 GPIOB
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

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
float temperature;               //温度
float humidity;                  //湿度

GPIO_PinState pinState;            //有人
uint32_t Gas;
uint8_t net_connect = 0;
char date_show_buff[128];
char buf1[40];
char buf2[40];
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
    uint16_t timeCount;
    unsigned char *dataPtr = NULL;
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
  MX_ADC1_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */
	OLED_Init();
	show();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  while (1)
  {
		  if(net_connect == 1)   // 连接成功
			{

					if(timeCount%50 == 0)
					{
							//烟雾传感器
							HAL_ADC_Start(&hadc1);  // 启动ADC1转换
							HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);  // 等待转换完成
							uint32_t adcValue = HAL_ADC_GetValue(&hadc1);  // 读取ADC值
							uint32_t n,Gas;
							float percentage;
							percentage=(float)(adcValue/10)*(3.3/4096); //mq135
							if(percentage<0.9)
							{
									n=percentage/0.001;
									Gas = n*0.012;
							}
							else
							{
									Gas = ((percentage-0.9)/0.1)*100;
									Gas =Gas+9;
							}
							memset(date_show_buff,0,sizeof(date_show_buff));
							sprintf(date_show_buff,":%d%%",Gas);
							OLED_ShowString(64,2,(uint8_t*)date_show_buff,16);
							

//===============================温湿度传感器==========================================================	
							if(DHT11(&temperature,&humidity)==0)
							{
									memset(date_show_buff,0,sizeof(date_show_buff));
									sprintf(date_show_buff,":%.0f",temperature);
									OLED_ShowString(32,0,(uint8_t*)date_show_buff,16);
									
									memset(date_show_buff,0,sizeof(date_show_buff));
									sprintf(date_show_buff,":%.0f",humidity);
									OLED_ShowString(96,0,(uint8_t*)date_show_buff,16);
							}
//=================================人体传感器====================================
							// 读取PB6引脚的状态
							pinState = HAL_GPIO_ReadPin(GPIO_Port_PB6, GPIO_Pin_PB6);

							// 根据引脚状态执行相应的操作
							if (pinState == GPIO_PIN_SET)
							{
									OLED_ShowCHinese(72,4,35);
									OLED_ShowCHinese(88,4,36);
							}
							else
							{
									OLED_ShowCHinese(72,4,37);
									OLED_ShowCHinese(88,4,36);
							} 
					
					}
					if(++timeCount >= 101)		//时间间隔5s 发送
					{
							HAL_Delay(100);
							printf( "开始发送数据\r\n");//通过串口1发送提示信息（要开始发送数据了）
							OneNet_SendData();//发送数据给onenet
							printf("发送数据结束\n");
							timeCount = 0;
							ESP8266_Clear();
					}
//            ESP8266_GetIPD(0);//获取平台返回的数据
					dataPtr = ESP8266_GetIPD(0);//获取平台返回的数据
					if(dataPtr != NULL)//如果返回数据不为空
							OneNet_RevPro(dataPtr);//平台返回数据检测

					HAL_Delay(40);
			}
    
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
			 else
        {
            printf("开始连接服务器\r\n");
            ESP8266_Init();					//初始化ESP8266

            printf("8266初始化成功\n");


            while(OneNet_DevLink())			//接入OneNET
                
            HAL_Delay(500);
            net_connect = 1;

            printf("接入onenet成功");
        }
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
