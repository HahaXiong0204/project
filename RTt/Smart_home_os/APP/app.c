#include "app.h"

#include "adc.h"
#include "tim.h"

#define AT_24_DATA_MAX  100   // �������
#define AT_24_Init_ADRR	 4     // ��ʼ�ĵ�ַ
struct AT24CDATA
{
    uint8_t 	arr[AT_24_DATA_MAX];
    uint16_t 	len;
    uint16_t	addr;
};
struct AT24CDATA at_data_struct = {0};    // ����Ľṹ��
char at_data_arr[256];
char at_th_buff[64];

void Mx_Init(void)
{
	delay_init(180);
    LCD_Init();
	DHT11_Init();
    while(AT24C02_Check()) {}
    while(pcf8574_Check()) {}
	SDRAM_Init();
	RC522_Init();

    my_mem_init(SRAMIN);		    //��ʼ���ڲ��ڴ��
    my_mem_init(SRAMEX);		    //��ʼ���ⲿ�ڴ��
    my_mem_init(SRAMCCM);		    //��ʼ��CCM�ڴ��
	HAL_ADC_Start_DMA(&hadc1,adc_value,ADC_MAX);
	HAL_TIM_Base_Start_IT(&htim1);
	HAL_TIM_Base_Start(&htim4);
	__HAL_TIM_SET_COUNTER(&htim4,0); 
	HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_3);
	HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_4);
//		at_sava_th();
		
		
	printf("����ֵǰ  %d-%d-%d-%d\r\n",TH_Temp,TH_Hum,TH_Gas,TH_light);
//	
//	// ��ȡ��ֵ����
	AT24C02_read(0,&at_th_size,1);
	printf("at_th_size = %d\r\n",at_th_size);
	AT24C02_read(2,(uint8_t *)at_th_buff,at_th_size);
	
	sscanf(at_th_buff,"%d-%d-%d-%d",&TH_Temp,&TH_Hum,&TH_Gas,&TH_light);
	printf("����ֵ��  %d-%d-%d-%d\r\n",TH_Temp,TH_Hum,TH_Gas,TH_light);

	
}


void Logical_code_entry(void *pargem)
{
	uint8_t temp = 0;
	while(1)
	{
		if(net_connect==1)
		{
			if(user_mode==0)  // �Զ�ģʽ
			{
//			printf("user_mode = %d\r\n",user_mode);
				if(temperature > TH_Temp)
				{
					if(tem_flag!=1)
						OneNet_Send_Int("tem_flag",1);
					tem_flag = 1;
					machinery_pwm_mode(1);
//					printf("beep_flag = %d\r\n",beep_flag);
					HAL_TIM_Base_Start_IT(&htim2);
				}
				else
				{
					if(tem_flag!=0){
						OneNet_Send_Int("tem_flag",0);
						beep_On_Off(1);
					}
					tem_flag = 0;
					machinery_pwm_mode(0);
				}
				

				if(humidity>TH_Hum)
				{
					if(Hum_flag!=1)
						OneNet_Send_Int("Hum_flag",1);
					Hum_flag = 1;
//					printf("beep_flag = %d\r\n",beep_flag);
					HAL_TIM_Base_Start_IT(&htim2);
				}
				else
				{
					if(Hum_flag!=0){
						OneNet_Send_Int("Hum_flag",0);
						beep_On_Off(1);
					}
					Hum_flag = 0;

				}
				
				if(Gas>TH_Gas)
				{
					if(Gas_flag!=1)
						OneNet_Send_Int("Gas_flag",1);
					Gas_flag = 1;
					HAL_TIM_Base_Start_IT(&htim2);
				}
				else{
					if(Gas_flag!=0){
						OneNet_Send_Int("Gas_flag",0);
						beep_On_Off(1);
					}
					Gas_flag = 0;
				}
				
				if(Lighting<TH_light)
				{
					Led_pwm_mode(1);
					if(light_flag!=1)
						OneNet_Send_Int("light_flag",1);
					light_flag = 1;
				}
				else
				{
					Led_pwm_mode(0);
					if(light_flag!=0){
						OneNet_Send_Int("light_flag",0);
						beep_On_Off(1);
					}
					light_flag = 0;
				
				}
				
			
			}
			
			
			
			rt_thread_mdelay(40);
		}
	}
	
}



// ������ֵ���ݵ�at24c02

uint8_t at_th_size = 0;   // ��ֵ�����ݴ�С
void at_sava_th(void)
{
	uint8_t temp = 0;
	memset(at_th_buff,0,sizeof(at_th_buff));
	sprintf(at_th_buff,"%d-%d-%d-%d",TH_Temp,TH_Hum,TH_Gas,TH_light);
	printf("%s    len =%d\r\n",at_th_buff,strlen(at_th_buff));
	at_th_size = strlen(at_th_buff);
	AT24C02_write(0,&at_th_size,1);
	if(at_th_size==AT24C02_read(1,&temp,1))
	{
		printf("���ݳ��ȱ���ɹ� = %d \r\n",at_th_size);
	}
	AT24C02_write(2,(uint8_t *)at_th_buff,at_th_size);
	
	memset(at_th_buff,0,sizeof(at_th_buff));
	AT24C02_read(1,(uint8_t *)at_th_buff,at_th_size);
	printf("�������� = %s\r\n",at_th_buff);
	if(net_connect==0||net_connect==1)
		at_data_struct.addr = at_th_size;
}

// ��rfid
void read_rfid(void *pargem)
{
	while(1)
	{
		printf("\r\n id");
		if(user_uing==0)
		{
			printf("read\r\n");
			while(ReaderCard()==0){
				printf("wait id\r\n");
			}
			user_uing = 1;
			sprintf(Publish_Topc_ID,"device/%d/%d/post",Device_ID,ID_Cart);
			sprintf(Submit_Topc_ID,"device/%d/%d/sub",Device_ID,ID_Cart);
			OneNet_Subscribe(topics_id, 1);   // ��������
			OneNet_Send_Device_ID_Data();
		}
		rt_thread_mdelay(4000);
	}
}

// ��ʾlcd��
char show_buff[124];
void lcd_show(void *pargem)
{
	while(1)
	{
		if(net_connect)
		{
			LCD_Clear(WHITE);
			// ��ʾʱ��
			memset(show_buff,0,sizeof(show_buff));
			sprintf(show_buff,"time:%d-%d-%d-%d-%d-%d",year_time,moon_time,sun_time,hour_time,minute_time,second_time);
			LCD_ShowString(10, 40, 460, 32, 32, (uint8_t *)show_buff);
			
				// ��ʾ�豸��
			memset(show_buff,0,sizeof(show_buff));
			sprintf(show_buff,"device:%d",Device_ID);
			LCD_ShowString(10,80,560,32,32,(uint8_t *)show_buff);
			
				// ��ʾ��ֵˮ��
			memset(show_buff,0,sizeof(show_buff));
			sprintf(show_buff,"TH_tem:%d  TH_Hum:%d",TH_Temp,TH_Hum);
			LCD_ShowString(10,120,560,32,32,(uint8_t *)show_buff);
			
			// ��ʾ�¶� ʪ��
			memset(show_buff,0,sizeof(show_buff));
			sprintf(show_buff,"temp:%.2f  Hum:%.2f",temperature,humidity);
			LCD_ShowString(10,160,560,32,32,(uint8_t *)show_buff);
			
				// ��ʾgas
			memset(show_buff,0,sizeof(show_buff));
			sprintf(show_buff,"Gas:%d   TH_Gas:%d ",Gas,TH_Gas);
			LCD_ShowString(10,200,560,32,32,(uint8_t *)show_buff);
			
				// ��ʾlight 
			memset(show_buff,0,sizeof(show_buff));
			sprintf(show_buff,"light:%d   TH_light:%d ",Lighting,TH_light);
			LCD_ShowString(10,240,560,32,32,(uint8_t *)show_buff);
			memset(show_buff,0,sizeof(show_buff));
			sprintf(show_buff,"tem:%d  hum:%d   gas:%d ",tem_flag,Hum_flag,Gas_flag);
			LCD_ShowString(10,280,560,32,32,(uint8_t *)show_buff);
			
			
			//���
			memset(show_buff,0,sizeof(show_buff));
			if(net_connect==1)
				sprintf(show_buff,"connect success !!! %d bp = %d",user_mode,beep_flag);
			else if(net_connect == 0 )
				sprintf(show_buff,"wait connect!!! %d  bp = %d",user_mode,beep_flag);
			else
				sprintf(show_buff,"Waiting for reconnection !!!   %d   bp = %d",user_mode,beep_flag);
			LCD_ShowString(10,760,560,32,32,(uint8_t *)show_buff);
			
			
			rt_thread_mdelay(1000);
		
		}
		
	}
	

}




static uint8_t	net_cone_num =0 ;
static uint8_t at24_addr = 4;
// �������� wifi
void resest_wifi(void)
{
	net_cone_num = 0;
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
	rt_thread_mdelay(200);
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
			HAL_Delay(1000);
			printf("����onenet�ɹ�");
			at24_addr = AT_24_Init_ADRR;
			again_num = 0;
			net_connect  = 1;
			for(i = 0; i < at_data_struct.len; i++)
			{
				memset(at_data_arr, 0, sizeof(at_data_arr));
				AT24C02_read(at24_addr, (uint8_t *)at_data_arr, at_data_struct.arr[i]);
				sscanf(at_data_arr,"%.2f-%.2f-%d-%.2f-%d-%d",&temperature,&humidity,&Lighting,&voltage,&Turbidity,&Gas);
				printf("ƴ������  =%.2f-%.2f-%d-%.2f-%d-%d\r\n",temperature,humidity,Lighting,voltage,Turbidity,Gas);
				OneNet_SendData(1);
				at24_addr += at_data_struct.arr[i];
				printf("�ط����� %d = %s\r\n", i, at_data_arr);
			}
			at_data_struct.addr = AT_24_Init_ADRR;
			memset(at_data_struct.arr,0,sizeof(at_data_struct.arr));
			at_data_struct.len = 0;
			ESP8266_Clear();									//��ջ���

			break;
		}


	}
	rt_thread_mdelay(200);

}


// �ɼ�������������
// mode = 0 ����ģʽ
// mode = 1 ���浽at24c02
uint32_t value1=0,value2=0,value3=0,value4=0;
uint8_t sensor_again_num = 0;
void collect_data(void *pargem)
{
	while(1)
	{
//		printf("net_connect = %d\r\n",net_connect);
		if(net_connect)
		{
			
//  ============================================================ ������� ====================================================
			speed = TIM4->CNT;
			printf("speed = %d\r\n",TIM4->CNT);
			TIM4->CNT = 0;
// ===========================================================================================================================
			// =============================================== ���崫���� ==================================================
			people = HAL_GPIO_ReadPin(People_GPIO_Port,People_Pin);
			if(people)
			{
				printf("����\r\n");
			}
			else{
				printf("����\r\n");
			
			}
			
			// ==============================================================================================================
			
			
			//===============================================dht11��ȡ======================================================
			DHT11_Read_Data(&humidity, &temperature);
			printf("temperature = %.2f		humidity = %.2f\r\n", temperature, humidity);
			//================================================================================================================
			for(i = 0, value1 = 0, value2 = 0, value3 = 0, value4 = 0; i < 40;)
			{
				value1 += adc_value[i++];
				value2 += adc_value[i++];
				value3 += adc_value[i++];
				value4 += adc_value[i++];
			}
			printf("value1 = %d    value2 = %d     value3 = %d    value4 = %d\r\n", value1, value2, value3, value4);
			// PA4
			Lighting = (4096 - value1 / 10) * 100 / 4096; //����
			printf("������ֵΪ��%d%%\r\n", Lighting);

			// PA5
			voltage = (float)value2 / 10 * (3.3 / 4096);
			printf("����ѹֵΪ1��%.3fV\r\n", voltage);

			// PA6
			Turbidity = (value3 / 10) * 100 / 4096; //�Ƕ�
			printf("����Ƕ�ֵΪ��%d\r\n", Turbidity);
			
			// PA7
			Gas = (value4 / 10) * 100 / 4096; //����
			printf("�������ֵΪ��%d%%\r\n", Gas);

			rt_thread_mdelay(2000);
			value1 = value2 = value3 = value4 = 0;
			
			// ���浽at24c02
			if(net_connect == 2|| net_connect == 3)
			{
				sensor_again_num++;
				if(sensor_again_num>3)
				{
					sensor_again_num = 0;
					memset(at_data_arr,0,sizeof(at_data_arr));
					sprintf(at_data_arr,"%.2f-%.2f-%d-%.2f-%d-%d",temperature,humidity,Lighting,voltage,Turbidity,Gas);
					printf("��������:%s    len = %d\r\n",at_data_arr,strlen(at_data_arr));
					
					AT24C02_write(at_data_struct.addr,(uint8_t *)at_data_arr,strlen(at_data_arr));
					at_data_struct.arr[at_data_struct.len++] = strlen(at_data_arr);
					memset(at_data_arr, 0, sizeof(at_data_arr));
					AT24C02_read(at_data_struct.addr, (uint8_t *)at_data_arr, at_data_struct.arr[at_data_struct.len-1]);
					at_data_struct.addr += strlen(at_data_arr);
					printf("��������%d �� %d   %s\r\n",at_data_struct.len,at_data_struct.addr,at_data_arr);
				}
			
			}
			
		
		}
	
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
