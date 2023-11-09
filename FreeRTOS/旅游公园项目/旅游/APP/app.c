#include "app.h"

#include "adc.h"
#include "tim.h"
//#include "pic.h"
#include "FreeRTOS.h"
#include "semphr.h"








void Mx_Init(void)
{
    delay_init();
    LCD_Init();
    DHT11_Init();
    while(AT24C02_Check()) {}
    while(pcf8574_Check()) {}
    SDRAM_Init();
	
#if RFID_FLAG
    RC522_Init();
#endif

	
    my_mem_init(SRAMIN);		    //��ʼ���ڲ��ڴ��
    my_mem_init(SRAMEX);		    //��ʼ���ⲿ�ڴ��
    my_mem_init(SRAMCCM);		    //��ʼ��CCM�ڴ��
    HAL_ADC_Start_DMA(&hadc1, adc_value, ADC_MAX);
		
			
    HAL_TIM_Base_Start_IT(&htim1);
	
		
    HAL_TIM_Base_Start(&htim4);
    __HAL_TIM_SET_COUNTER(&htim4, 0);
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);
		
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_4);   // pwm��
	
	HAL_TIM_PWM_Start(&htim5,TIM_CHANNEL_1);//�����ʱ����ʼ��
	TIM5->CCR1=150;   //ֹͣ
	
	Hcsr04Init(&htim12, TIM_CHANNEL_1);  //  ������ģ���ʼ��
	Hcsr04Start();  //  ����������ģ����

    while(W25QXX_ReadID() != W25Q256)								//��ⲻ��W25Q256
    {
        LCD_ShowString(30, 150, 200, 16, 16, "W25Q256 Check Failed!");
        delay_ms(500);
        LCD_ShowString(30, 150, 200, 16, 16, "Please Check!        ");
        delay_ms(500);
    }
    while(font_init()) 		//����ֿ�
    {
        LCD_ShowString(30, 50, 200, 16, 16, "Font Error!");
        delay_ms(200);
        LCD_Fill(30, 50, 240, 66, WHITE); //�����ʾ
        delay_ms(200);
    }


    //	show_picture(0,0,320,320,(uint16_t *)gImage_pic);

    at_sava_th();

    printf("����ֵǰ  %d-%d-%d-%d\r\n", TH_Temp, TH_Hum, TH_Gas, TH_light);
    // ��ȡ��ֵ����
    AT24C02_read(0, &at_th_size, 1);
    AT24C02_read(1, (uint8_t *)at_th_buff, at_th_size);

    sscanf(at_th_buff, "%d-%d-%d-%d", &TH_Temp, &TH_Hum, &TH_Gas, &TH_light);
    printf("����ֵ��  %d-%d-%d-%d\r\n", TH_Temp, TH_Hum, TH_Gas, TH_light);

	SG90_Rotate(1,Degrees_45);
}





/*  ͨ������2 ���͸�zigbee */
static char zigbee_buff[128];
void Send_Zigbee(void)
{
	memset(zigbee_buff,0,sizeof(zigbee_buff));
	sprintf(zigbee_buff,"te:%.f   Gas:%d  zhu:%d",temperature,Gas,Turbidity);
	HAL_UART_Transmit_DMA(&huart2,(uint8_t *)zigbee_buff,strlen(zigbee_buff));
	osDelay(2);
	huart2.gState = HAL_UART_STATE_READY;
	printf("zigbee��������:%s\r\n",zigbee_buff);
}

/*  ͨ������6 ���͸����� */
static char x_buff[8];
void Send_tianwen(uint8_t mode)
{
	memset(x_buff,0,sizeof(x_buff));
	
	switch(mode)
	{
		case 1:{    /*��ӭ����*/
			x_buff[0] = 0x01;
			break;
		}
		case 2:{    /*��ӭ�´ι���*/
			x_buff[0] = 0x02;
			break;
		}
		case 3:{    /*����ʱ��*/
			x_buff[0] = 0x03;
			x_buff[1] = year_time;
			x_buff[2] = moon_time;
			x_buff[3] = sun_time;
			x_buff[4] = hour_time;
			x_buff[5] = minute_time;
			x_buff[6] = second_time;
			break;
		}
		case 4:{    /*������ǰ�¶�*/
			x_buff[0] = 0x04;
			x_buff[1] = (int)temperature;
			x_buff[2] = (int)(temperature*100)%100;
			break;
		}
		case 5:{    /*������ǰʪ��*/
			x_buff[0] = 0x05;
			x_buff[1] = (int)humidity;
			x_buff[2] = (int)(humidity*100)%100;
			break;
		}
		case 6:{    /*�¶ȳ�����ֵ*/
			x_buff[0] = 0x06;
			break;
		}
		case 7:{    /*ʪ�ȳ�����ֵ*/
			x_buff[0] = 0x07;
			break;
		}
		case 8:{    /*��������������ֵ*/
			x_buff[0] = 0x08;
			break;
		}
		case 9:{    /*ˮ�ʳ�����ֵ*/
			x_buff[0] = 0x09;
			break;
		}		
		case 10:{    /*��ѹ������ֵ*/
			x_buff[0] = 0x0a;
			break;
		}	
		case 11:{    /*������Ϊ*/
			x_buff[0] = 0x0b;
			x_buff[1] = ;
			x_buff[2] = 0x0b;
			break;
		}	
		case 9:{    /*ˮ�ʳ�����ֵ*/
			x_buff[0] = 0x09;
			break;
		}	
		case 9:{    /*ˮ�ʳ�����ֵ*/
			x_buff[0] = 0x09;
			break;
		}	
		
	}
	
	if(mode==1)   // ������
	{
		
	}
	if(mode==2)
	{
		x_buff[0] = 0x02;
	}
	
	if(mode==3)
	{
		x_buff[0] = 0x03;
	}
	
	if(mode==4)
	{
		x_buff[0] = 0x04;
	}
	
	if(mode==5)
	{
		x_buff[0] = 0xF1;
	}
	if(mode==6)
	{
		x_buff[0] = 0xF2;
	}
	if(mode==7)
	{
		x_buff[0] = 0xF4;
	}
	
	
	
//	memset(tianwen_buff,0,sizeof(tianwen_buff));
//	sprintf(tianwen_buff,"%s",buff);
	HAL_UART_Transmit_DMA(&huart6,(uint8_t *)x_buff,strlen(x_buff));
	osDelay(2);
	huart6.gState = HAL_UART_STATE_READY;
	printf("���ʷ�������:%s\r\n",x_buff);
}



// ������ֵ���ݵ�at24c02
char at_th_buff[64];
uint8_t at_th_size = 0;   // ��ֵ�����ݴ�С
void at_sava_th(void)
{
    uint8_t temp = 0;
    memset(at_th_buff, 0, sizeof(at_th_buff));
    sprintf(at_th_buff, "%d-%d-%d-%d", TH_Temp, TH_Hum, TH_Gas, TH_light);
    printf("%s\r\n", at_th_buff);
    at_th_size = strlen(at_th_buff);
    AT24C02_write(0, &at_th_size, 1);
    if(at_th_size == AT24C02_read(1, &temp, 1))
    {
        printf("���ݳ��ȱ���ɹ� = %d \r\n", at_th_size);
    }
    AT24C02_write(1, (uint8_t *)at_th_buff, at_th_size);

    memset(at_th_buff, 0, sizeof(at_th_buff));
    AT24C02_read(1, (uint8_t *)at_th_buff, at_th_size);
    printf("�������� = %s\r\n", at_th_buff);
    if(net_connect == 0 || net_connect == 1)
        at_data_struct.addr = at_th_size;
}






static uint8_t	net_cone_num = 0 ;
static uint8_t at24_addr = 4;
// �������� wifi
void resest_wifi(void)
{
    net_cone_num = 0;
    if(ESP8266_SendCmd("AT\r\n", "OK") == 0)
    {
		if(ESP8266_SendCmd("AT+RST\r\n", "ready")==0)
		{
			osDelay(20);
			if(ESP8266_SendCmd("AT+CWMODE=1\r\n", "OK") == 0)
			{
				if(ESP8266_SendCmd("AT+CWDHCP=1,1\r\n", "OK") == 0)
				{
					osDelay(20);
					while(ESP8266_SendCmd(ESP8266_WIFI_INFO, "GOT IP")) {
						net_cone_num++;
						osDelay(10);
						if(net_cone_num>200)
						{
							net_cone_num = 0;
							break;
						}
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
    osDelay(200);
}

// ��������������
void resest_server(void)
{
    net_cone_num = 5;
    printf("����net_connet = %d\r\n", net_connect);
    while(ESP8266_SendCmd(ESP8266_ONENET_INFO, "CONNECT") == 0)
    {
        while(OneNet_DevLink())			//����OneNET
        {
            if(net_connect == 2)
                break;
        }
        if(net_connect != 2)
        {
            osDelay(1000);
            printf("����onenet�ɹ�");
            at24_addr = AT_24_Init_ADRR;
            again_num = 0;
            net_connect  = 1;
			xQueueReset(hsend_queue);
            for(i = 0; i < at_data_struct.len; i++)
            {
                memset(at_data_arr, 0, sizeof(at_data_arr));
                AT24C02_read(at24_addr, (uint8_t *)at_data_arr, at_data_struct.arr[i]);
                sscanf(at_data_arr, "%.2f-%.2f-%d-%.2f-%d-%d", &temperature, &humidity, &Lighting, &voltage, &Turbidity, &Gas);
                printf("ƴ������  =%.2f-%.2f-%d-%.2f-%d-%d\r\n", temperature, humidity, Lighting, voltage, Turbidity, Gas);
                OneNet_SendData(1);
                at24_addr += at_data_struct.arr[i];
                printf("�ط����� %d = %s\r\n", i, at_data_arr);
            }
            at_data_struct.addr = AT_24_Init_ADRR;
            memset(at_data_struct.arr, 0, sizeof(at_data_struct.arr));
            at_data_struct.len = 0;
            ESP8266_Clear();									//��ջ���
			reconnect_one_num = 0;
            break;
        }


    }
    osDelay(200);

}


// ���͵����ͻ�����
void Send_queue(char *msg)
{
	send_buffer = msg;

	if( xQueueSendToBackFromISR( hsend_queue, &send_buffer, 0 ) != pdPASS )
	{
		printf( "Could not send to the queue.\r\n" );
	}
	

}



// ���õ����λ
void machinery_pwm_mode(uint8_t mode)
{
    machinery_position = mode;
    if(mode == 0)	TIM3->CCR3 = 499;
    if(mode == 1)   TIM3->CCR3 = 200;
    if(mode == 2)	TIM3->CCR3 = 0;
}


// ����LED��λ
void Led_pwm_mode(uint8_t mode)
{
    Led_position = mode;
    if(mode == 0)	TIM3->CCR4 = 500;
    if(mode == 1)   TIM3->CCR4 = 300;
    if(mode == 2)	TIM3->CCR4 = 0;
}


void Wait_send_mutex(void)
{
	Wait_reve_mutex();
	/*�ȴ�������*/
	xSemaphoreTake(send_mutex,portMAX_DELAY);
}

void Release_send_mutex(void)
{
	Release_reve_mutex();
	/*�ͷŷ�����*/
	xSemaphoreGive(send_mutex);
}

void Wait_reve_mutex(void)
{
	/*�ȴ�������*/
	xSemaphoreTake(reve_mutex,portMAX_DELAY);
}

void Release_reve_mutex(void)
{
	/*�ͷŽ�����*/
	xSemaphoreGive(reve_mutex);
}

/*�ȴ�iic��spi��*/
void Wait_iic_spi_mutex(void)
{
	/*�ȴ�iic_spi��*/
	xSemaphoreGive(iic_spi_mutex);
}
/*�ͷ�iic��spi��*/
void Release_iic_spi_mutex(void)
{
	/*�ͷŷ�����*/
	xSemaphoreGive(iic_spi_mutex);

}

