/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "i2c.h"
#include "iwdg.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "fmc.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "wifi.h"
#include "onenet.h"
#include "lcd.h"
#include "24c02.h"
#include "sdram.h"
#include "key.h"
#include "malloc.h"
#include "PCF8574.h"
#include "dht11.h"
#include "rc522.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

#define AT_24_DATA_MAX 100
struct AT24CDATA
{
    uint8_t 	arr[AT_24_DATA_MAX];
    uint16_t 	len;
    uint16_t	addr;
};


/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
void sensor_read_data(void);
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint8_t net_connect = 0;            // 网咯是否连接的标志位
uint8_t connet_send_flag = 0;
uint32_t adc_value[40];              //adc数据缓冲区

uint32_t value1 = 0, value2 = 0, value3 = 0, value4 = 0;
uint8_t i = 0;                       //for循环用到的变量而已

double temperature = 0;             //温度
double humidity = 0;                //湿度
uint32_t Lighting = 0, Turbidity = 0, Gas = 0; //光照，浊度，烟雾的数据
char buf1[128];
//char buf2[40];
uint8_t at_data_arr[256];
uint16_t at24_addr = 0;
float voltage = 0;  // 电压


uint8_t cmd_flag = 0;    // 命令打印标志位

//void sensor_read_data(void);


extern uint8_t net_connect;
struct AT24CDATA at_data_struct = {0};    // 掉电的结构体

char json_arr[256];

extern uint8_t Data[5];
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
    /* USER CODE BEGIN 1 */
    uint16_t timeCount = 0;
    //	uint16_t time_wwdg = 0;
    unsigned char *dataPtr = NULL;
    uint8_t net_cone_num = 0;
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
    MX_USART3_UART_Init();
    MX_FMC_Init();
    MX_ADC1_Init();
    MX_I2C2_Init();
    MX_TIM1_Init();
    MX_SPI2_Init();
    MX_IWDG_Init();
    /* USER CODE BEGIN 2 */
	HAL_TIM_Base_Start_IT(&htim1);
    delay_init(180);
    LCD_Init();
    while(AT24C02_Check()) {}
    while(pcf8574_Check()) {}


    RC522_Init();
    printf("等待管理员的卡\r\n");
	LCD_ShowString(100,100,240,32,32,"wait RFID...");
//    
//	while(ReaderCard() == 0)

    DHT11_Init();


    SDRAM_Init();

    my_mem_init(SRAMIN);		    //初始化内部内存池
    my_mem_init(SRAMEX);		    //初始化外部内存池
    my_mem_init(SRAMCCM);		    //初始化CCM内存池

    KEY_Init();

   

//    LCD_ShowString(10, 40, 240, 32, 32, "Apollo STM32F4/F7");
//    LCD_ShowString(10, 80, 240, 24, 24, "TFTLCD TEST");
//    LCD_ShowString(10, 110, 240, 16, 16, "ATOM@ALIENTEK");
//    // 		LCD_ShowString(10,130,240,16,16,lcd_id);		//显示LCD ID
//    LCD_ShowString(10, 150, 240, 12, 12, "2016/1/6");


    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */

    while (1)
    {
        //	  printf("sdjj\r\n");
        if(net_connect == 1)   // 连接成功
        {
            if(timeCount % 10 == 0)
            {
                // 发送数据
                sensor_read_data();


            }
            ++timeCount;
            //			if(timeCount==50)
            //			{
            //				HAL_IWDG_Refresh(&hiwdg);               // 喂狗
            //			}

            if(timeCount >= 20)		//时间间隔5s 发送
            {

                //				OneNet_SendData_State("EEROR");

                HAL_Delay(100);
                printf( "开始发送数据\r\n");//通过串口1发送提示信息（要开始发送数据了）
                OneNet_SendData();//发送数据给onenet
                printf("发送数据结束\n");
                //				while(OneNet_DevLink())			//接入OneNET
                timeCount = 0;
                ESP8266_Clear();
            }
            //            ESP8266_GetIPD(0);//获取平台返回的数据
            dataPtr = ESP8266_GetIPD(0);//获取平台返回的数据
            if(dataPtr != NULL)//如果返回数据不为空
                OneNet_RevPro(dataPtr);//平台返回数据检测

            HAL_Delay(20);
        }



        else if(net_connect == 2 || net_connect == 3)
        {
            ++timeCount;

            //==============================================================    断网数据采集 ===================================
            if(timeCount % 4 == 0)
            {

                //发送数据
                sensor_read_data();
                OneNet_FillBuf(json_arr, 1);
                // 掉电保存

                AT24C02_write(at_data_struct.addr, (uint8_t *)json_arr, strlen(json_arr));
                at_data_struct.arr[at_data_struct.len++] = strlen(json_arr);
                memset(at_data_arr, 0, sizeof(at_data_arr));
                AT24C02_read(at_data_struct.addr, at_data_arr, strlen(json_arr));
                printf("断网保存成功 = %s\r\n", at_data_arr);
                at_data_struct.addr += strlen(json_arr);
                timeCount = 0;
                //				HAL_IWDG_Refresh(&hiwdg);               // 喂狗
            }

            // ==========================================net_connect=2 尝试重连==================================
            if(net_connect == 2)
            {
                if(ESP8266_SendCmd("AT\r\n", "OK") == 0)
                {
                    if(ESP8266_SendCmd("AT+RST\r\n", "ready") == 0)
                    {
                        //					HAL_Delay(2000);
                        //					Usart_SendString(USART2, (unsigned char *)"AT+RST\r\n", strlen((const char *)"AT+RST\r\n"));
                        HAL_Delay(200);
                        if(ESP8266_SendCmd("AT+CWMODE=1\r\n", "OK") == 0)
                        {
                            if(ESP8266_SendCmd("AT+CWDHCP=1,1\r\n", "OK") == 0)
                            {
                                //								HAL_Delay(200);
                                while(ESP8266_SendCmd(ESP8266_WIFI_INFO, "GOT IP")) {}
                                while(ESP8266_SendCmd(ESP8266_WIFI_INFO, "GOT IP"))
                                {
                                    net_cone_num++;
                                    if(net_cone_num > 20)
                                    {

                                        break;
                                    }
                                    //									}
                                }
                                if(net_cone_num < 20)
                                {
                                    net_connect = 3;
                                    printf("net_connet = %d\r\n", net_connect);
                                }
                                net_cone_num = 0;
                            }
                        }
                    }
                }
            }
            if(net_connect == 3)
            {
                net_cone_num = 5;
                printf("进入net_connet = %d\r\n", net_connect);
                while(ESP8266_SendCmd(ESP8266_ONENET_INFO, "CONNECT") == 0)
                {
                    while(OneNet_DevLink())			//接入OneNET
                    {
                        if(net_connect == 2)
                            break;
                    }
                    if(net_connect != 2)
                    {
                        HAL_Delay(1000);
                        printf("接入onenet成功");
                        net_connect  = 1;
                        at24_addr = 0;

                        for(i = 0; i < at_data_struct.len; i++)
                        {
                            memset(at_data_arr, 0, sizeof(at_data_arr));
                            AT24C02_read(at24_addr, at_data_arr, at_data_struct.arr[i]);
                            if(OneNet_SendData_Buff((char *)at_data_arr))
                            {
                                connet_send_flag = 1;
                            }
                            at24_addr += at_data_struct.arr[i];
                            printf("重发数据 %d = %s\r\n", i, at_data_arr);
                        }
                        connet_send_flag = 0;
                        ESP8266_Clear();									//清空缓存
                        break;
                    }
                    //					while()


                }

            }
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
            HAL_ADC_Start_DMA(&hadc1, (uint32_t *)adc_value, 40);

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

    /** Configure the main internal regulator output voltage
    */
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /** Initializes the RCC Oscillators according to the specified parameters
    * in the RCC_OscInitTypeDef structure.
    */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI | RCC_OSCILLATORTYPE_LSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.LSIState = RCC_LSI_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PLLM = 8;
    RCC_OscInitStruct.PLL.PLLN = 180;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = 4;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    /** Activate the Over-Drive mode
    */
    if (HAL_PWREx_EnableOverDrive() != HAL_OK)
    {
        Error_Handler();
    }

    /** Initializes the CPU, AHB and APB buses clocks
    */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                                  | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
    {
        Error_Handler();
    }
}

/* USER CODE BEGIN 4 */
void sensor_read_data(void)
{
    //===============================================dht11读取======================================================
    DHT11_Read_Data(&temperature, &humidity);
    printf("temperature = %.2f		humidity = %.2f\r\n", temperature, humidity);
    //================================================================================================================

    //===============================================adc通道读取======================================================
    for(i = 0, value1 = 0, value2 = 0, value3 = 0, value4 = 0; i < 40;)
    {
        value1 += adc_value[i++];
        value2 += adc_value[i++];
        value3 += adc_value[i++];
        value4 += adc_value[i++];
    }
    printf("value1 = %d    value2 = %d     value3 = %d    value4 = %d\r\n", value1, value2, value3, value4);
    // PA4
    Lighting = (4096 - value1 / 10) * 100 / 4096; //亮度
    printf("检测光亮值为：%d%%\r\n", Lighting);

    voltage = (float)value2 / 10 * (3.3 / 4096);
    printf("检测电压值为1：%.3fV\r\n", voltage);

    Turbidity = (value3 / 10) * 100 / 4096; //浊度
    printf("检测浊度值为：%d\r\n", Turbidity);
    Gas = (4096 - value4 / 10) * 100 / 4096; //烟雾
    printf("检测烟雾值为：%d%%\r\n", Gas);

    HAL_Delay(2000);
    value1 = value2 = value3 = value4 = 0;
    //====================================================================================================================
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    __disable_irq();
    while (1)
    {
    }
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
       ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
