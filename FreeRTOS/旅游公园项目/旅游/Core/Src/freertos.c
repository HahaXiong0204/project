/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
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
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for mainTask */
osThreadId_t mainTaskHandle;
const osThreadAttr_t mainTask_attributes = {
  .name = "mainTask",
  .stack_size = 2048 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void mainTask_entry(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
	Mx_Init();
  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
	
	send_mutex = xSemaphoreCreateMutex();
	reve_mutex = xSemaphoreCreateMutex();
	
	iic_spi_mutex = xSemaphoreCreateMutex();   
	
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */

  hreve_queue = xQueueCreate(REVE_MAX_QUEUE,sizeof(char *));
  
  hsend_queue = xQueueCreate(SEND_MAX_QUEUE,sizeof(char *));

  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of mainTask */
  mainTaskHandle = osThreadNew(mainTask_entry, NULL, &mainTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */


#if RFID_FLAG
		xTaskCreate(read_rfid_thread_entry,"rfid_id",256,NULL,4,&read_rfid_id);	
		vTaskSuspend(read_rfid_id);
#endif
		
//			/*  ���������߳� */
//		xTaskCreate(Ping_thread_entry,"ping_id",128,NULL,9,&ping_id);
//		vTaskSuspend(ping_id);
		
			/*  ���������߳� */
		xTaskCreate(Send_data_Entry,"send_data_id",1024,NULL,7,&send_data_id);
			
		
//		/*  ���������߳� */
//		xTaskCreate(Reve_thread_Entry,"reve_data_id",1024,NULL,7,&reve_data_id);
		
		/*  �����߼��߳� */
		xTaskCreate(logical_thread_entry,"logical_id",128,NULL,9,&Logical_code_id);
		vTaskSuspend(Logical_code_id);
		
		/*  �����ɼ������߳� */
		xTaskCreate(collect_data_thread_entry,"collect_id",256,NULL,9,&collect_data_id);	
		vTaskSuspend(collect_data_id);
		
		/*  ��������ɨ���߳� */
		xTaskCreate(key_thread_entry,"key_id",128,NULL,8,&key_scan_id);
		vTaskSuspend(key_scan_id);
		
		/*  ����lcd��ʾ�߳� */
		xTaskCreate(lcd_show_thread_entry,"lcd_id",256,NULL,osPriorityNormal+1,&show_lcd_id);
		vTaskSuspend(show_lcd_id);
		
		


  
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_mainTask_entry */
/**
  * @brief  Function implementing the mainTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_mainTask_entry */
void mainTask_entry(void *argument)
{
  /* USER CODE BEGIN mainTask_entry */
	 uint16_t timeCount = 0;
	char *receivedStr = NULL;    // ���ջ���������ָ��

	
  /* Infinite loop */
  for(;;)
  {	  
	  if(net_connect == 1)
	  {
// ======================================== ������Ҫ��timeCount��ֵ ���� ��ʱʱ�� ��ʱ����Ҫͬ��ʱ�䲻Ȼn���Ӻ������ =====================================
//			++timeCount;
//			if((timeCount%470==0))
//			{
//#if IWDG_FLAG
//				HAL_IWDG_Refresh(&hiwdg);               // ι��
//#endif
//			}
//			// �ж��Ƿ���ʱ��ͬ��
//			if((timeCount%270==0)&&(time_flag==0))
//			{
//				OneNet_Send_Device_ID_Data();
//			}
			osDelay(5);
//            if(timeCount >= 1000)		//ʱ����5s ����
//            {

//				
//                printf( "��ʼ��������\r\n");//ͨ������1������ʾ��Ϣ��Ҫ��ʼ���������ˣ�
//                OneNet_SendData(0);//�������ݸ�onenet
//                printf("�������ݽ���\n");
//                timeCount = 0;
//                ESP8266_Clear();
//            }
//			
			if ((xQueueReceive(hreve_queue, &receivedStr, 0))) {
					// ������յ����ַ���ָ��
					if(receivedStr != NULL)//����������ݲ�Ϊ��
					{
						Wait_reve_mutex();
						OneNet_RevPro((uint8_t*)receivedStr);//ƽ̨�������ݼ��
						Release_reve_mutex();
					}	
					
			}
			
		
	  }
	  else if(net_connect == 2||net_connect==3)
	  {
		  if(net_connect==2)
		  {
			resest_wifi();
		  }
		  if(net_connect==3)
		  {
			resest_server();
		  }
	  }
	  else
	  {
			printf("��ʼ���ӷ�����\r\n");
			Show_Str(10, 760, 200, 32, "�ȴ�����!!!", 32, 0);
            ESP8266_Init();					//��ʼ��ESP8266

            printf("8266��ʼ���ɹ�\n");


            while(OneNet_DevLink())			//����OneNET
			
			
			HAL_Delay(50);
			
            net_connect = 1;
//            HAL_ADC_Start_DMA(&hadc1, (uint32_t *)adc_value, 40);

//			vTaskResume(reve_id);
			vTaskResume(show_lcd_id);
			vTaskResume(collect_data_id);
#if RFID_FLAG
//			vTaskResume(read_rfid_id);
#endif
			vTaskResume(Logical_code_id);
			vTaskResume(key_scan_id);
            printf("����onenet�ɹ�");
			
			

	
	  
	  }
	  
	  
  }
  /* USER CODE END mainTask_entry */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

