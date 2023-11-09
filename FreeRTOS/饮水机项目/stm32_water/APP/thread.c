#include "thread.h"
#include "app.h"
#include "pic.h"

float Distance;                      //��������þ���
struct AT24CDATA at_data_struct = {0};    // ����Ľṹ��
char at_data_arr[256];

/*
	�ɼ�������������
	mode = 0 ����ģʽ
	mode = 1 ���浽at24c02
*/
uint32_t value1 = 0, value2 = 0, value3 = 0, value4 = 0;
uint8_t sensor_again_num = 0;
void collect_data_thread_entry(void *pargem)
{
//	uint8_t c_num = 0;
    while(1)
    {
        //		printf("net_connect = %d\r\n",net_connect);
		
		osDelay(2000);
        if(net_connect)
        {
			
			printf("approch = %d",HAL_GPIO_ReadPin(approach_switch_GPIO_Port,approach_switch_Pin));
			printf("red_input = %d",HAL_GPIO_ReadPin(red_input_GPIO_Port,red_input_Pin));
			
            // =============================================== �ٶ� ==================================================
            speed = TIM4->CNT;
            printf("speed = %d\r\n", TIM4->CNT);
            TIM4->CNT = 0;
            // =============================================================================================================

            // =============================================== ���崫���� ==================================================
//			
			Hcsr04Start();
			printf("dis = %.1f\r\n",Hcsr04Read());
//			
			
//            if(HAL_GPIO_ReadPin(People_GPIO_Port, People_Pin))
//            {
//                printf("����\r\n");
//				people = 1;
//				
//            }
//            else
//            {
//                printf("����\r\n");
//				people = 0;
//				
//            }
            // ==============================================================================================================


            //===============================================dht11��ȡ======================================================
//            DHT11_Read_Data(&humidity, &temperature);
//            printf("temperature = %.2f		humidity = %.2f\r\n", temperature, humidity);
//			osDelay(50);
			
			temperature = ((float)DS18B20_Get_Temp()/10);
			Water_Hot = ((float)DS18B20_2_Get_Temp()/10);   // ��ˮ
			
			printf("===========%.2f\r\n",temperature);
			printf("===========%.2f\r\n",Water_Hot);
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
			voltage = (float)value2 / 10 * (3.3 / 4096);
			Water_TDS1 = 110*((5-voltage)/voltage);
            printf("���Water_TDS1ֵΪ��%d%%\r\n", Water_TDS1);

            // PA6 ��ѹ
//            voltage  = (float)value3 / 10 * (3.3 / 4096);
//			Water_TDS1 = 110*((5-voltage)/voltage);
//            printf("���ˮԴˮ��ֵΪ1��%dV\r\n", Water_TDS1);

            // PA5
            voltage = (float)value2 / 10 * (3.3 / 4096);
			Water_TDS2 = 110*((5-voltage)/voltage);
            printf("�����ˮˮ��ֵΪ��%d\r\n", Water_TDS2);

            // PA7
//            Gas = (value4 / 10) * 100 / 4096; //����
//            printf("�������ֵΪ��%d%%\r\n", Gas);
//            value1 = value2 = value3 = value4 = 0;

            // ���浽at24c02
            if(net_connect == 2 || net_connect == 3)
            {
                sensor_again_num++;
                if(sensor_again_num > 3)
                {
                    sensor_again_num = 0;
                    memset(at_data_arr, 0, sizeof(at_data_arr));
                    sprintf(at_data_arr, "%.2f-%.2f-%d-%.2f-%d-%d", temperature, humidity, Lighting, voltage, Turbidity, Gas);
                    printf("��������:%s    len = %d\r\n", at_data_arr, strlen(at_data_arr));

                    AT24C02_write(at_data_struct.addr, (uint8_t *)at_data_arr, strlen(at_data_arr));
                    at_data_struct.arr[at_data_struct.len++] = strlen(at_data_arr);
                    memset(at_data_arr, 0, sizeof(at_data_arr));
                    AT24C02_read(at_data_struct.addr, (uint8_t *)at_data_arr, at_data_struct.arr[at_data_struct.len - 1]);
                    at_data_struct.addr += strlen(at_data_arr);
                    printf("��������%d �� %d   %s\r\n", at_data_struct.len, at_data_struct.addr, at_data_arr);
                }
				osDelay(200);

            }

			Send_Zigbee();   // ���͵�zigbee��
        }
		

    }


}

/*
	���������߳�
*/
uint8_t key = 0;
void key_thread_entry(void *pargem)
{
    while(1)
    {
        key = KEY_Scan(0);
        switch(key)
        {
        case KEY0_PRES:
        {
			/*��ˮ*/
			if(state==2&&user_uing==1)
			{
				if(Water_switch==0)
					Water_switch = 2;
				else if(Water_switch == 2)
					Water_switch = 0;
				else
					Water_switch = 2;
			}
			
            printf("KEY0_PRES\r\n");
            break;
        }
        case KEY1_PRES:
        {
			Water_flow -= 2;
            printf("KEY1_PRES\r\n");
            break;
        }
        case KEY2_PRES:
        {
			/*��ˮ*/
			if(state==2)
			{
				if(Water_switch==0&&user_uing ==1){
					Water_switch = 1;
				}
				else if(Water_switch == 1){
					Water_switch = 0;
				}
				else{
					Water_switch = 1;
				}
			}
			
            printf("KEY2_PRES\r\n");
            break;
        }
        case WKUP_PRES:
        {
			Water_flow += 2;
            printf("WKUP_PRES\r\n");
            break;
        }
        case RED_PRES:
        {
			if(Hot_flag==0)
				Hot_flag = 1;
            else
				Hot_flag = 0;
			printf("RED_PRES\r\n");
            break;
        }
        case YELLOW_PRES:
        {
//			Send_tianwen("2");
            printf("YELLOW_PRES\r\n");
            break;
        }
        case GRREN_PRES:
        {
			user_uing = 0;
			OneNet_Send_one_Data(1);   // ����
			beep_one();
            printf("grean_pres\r\n");
            break;
        }

        }
        osDelay(10);

    }


}

/*
	��rfid��ֵ
*/
#if RFID_FLAG
// ��rfid
void read_rfid_thread_entry(void *pargem)
{
    while(1)
    {
        //		printf("\r\nid");

		/*  ɨ�赽rfid �����߳�    ���Ҫ���¿����߳���Ҫ�� vTaskResume(read_rfid_id); */
        if(user_uing == 0)
        {
            while(ReaderCard() == 0)
            {
                printf("wait id\r\n");
            }
            user_uing = 1;
			vTaskSuspend(read_rfid_id);
        }
        osDelay(2000);
    }
}
#endif




/* 
	�߼��ж��߳�  
*/
uint8_t Error_num = 0;
uint8_t Warning_num = 0;
uint8_t hot_auto_flag = 0;
void logical_thread_entry(void *pargem)
{
	while(1)
	{
		osDelay(500);
		
		/* ˮλ�� */
		if(HAL_GPIO_ReadPin(approach_switch_GPIO_Port,approach_switch_Pin)==0)   
		{
			Water_c = Water_l;
			Water_leve_flag = 1;
		}
		/*��ǰˮλ�������ˮλ*/
		if(Water_c<=Water_l)
		{
			machinery_pwm_mode(2);
			Water_leve_flag = 1;
		}
		

		/* ˮλ�� */
		if(HAL_GPIO_ReadPin(red_input_GPIO_Port,red_input_Pin)==1)   
		{
			Water_c = Water_h;
			machinery_pwm_mode(0);
			Water_leve_flag = 0;
		}
		/*��ǰˮλ�������ˮλ*/
		if(Water_c>=Water_h)
		{
			machinery_pwm_mode(0);
			Water_leve_flag = 0;
		}
		
		// ����
		if((HAL_GPIO_ReadPin(red_input_GPIO_Port,red_input_Pin)==1)&&(HAL_GPIO_ReadPin(approach_switch_GPIO_Port,approach_switch_Pin)==0))
		{
			
			if(Error_num == 0){
				Error_flag = 1;
				printf("11111111111111111111111111\r\n");
				OneNet_Send_String("statue","Error1");
				HAL_TIM_Base_Start_IT(&htim10);
				Error_num = 1;
			}
		}
		else{
			Error_num = 0;
			Error_flag = 0;
		}
		
		/* ˮԴˮ�ʾ��� */ 
		if(Water_TDS2>=100)
		{
			
			if(Warning_num == 0){
				Warning_flag = 1;
				OneNet_Send_String("statue","Warning1");
				HAL_TIM_Base_Start_IT(&htim10);
				Warning_num = 1;
			}
		}
		else{
			Warning_num = 0;
			Warning_flag = 0;
		}
		
		/* ���� */
		if(Water_Hot<=TL_Temp)
		{
//			if(Hot_flag==0)
//				Water_Hot_temp = 0;
			Hot_flag = 1;
			
			
			Led_pwm_mode(2);
			Water_Hot_temp = Water_Hot;
		}
		else if(Water_Hot>=TH_Temp)
		{
//			if(Hot_flag==1)
////				Water_Hot_temp = 0;
			
			Hot_flag = 0;
			
			Led_pwm_mode(0);
		}
		
		
		/* ��ˮ���� */
//		if(Water_switch==1)   // ��ˮ
//		{
//			HAL_GPIO_WritePin(led_out_GPIO_Port,led_out_Pin,GPIO_PIN_RESET);
//			HAL_GPIO_WritePin(led1_out_GPIO_Port,led1_out_Pin,GPIO_PIN_SET);
//		}
//		else if(Water_switch == 2)
//		{
//			HAL_GPIO_WritePin(led_out_GPIO_Port,led_out_Pin,GPIO_PIN_SET);
//			HAL_GPIO_WritePin(led1_out_GPIO_Port,led1_out_Pin,GPIO_PIN_RESET);
//		}
//		else{
//			HAL_GPIO_WritePin(led_out_GPIO_Port,led_out_Pin,GPIO_PIN_SET);
//			HAL_GPIO_WritePin(led1_out_GPIO_Port,led1_out_Pin,GPIO_PIN_SET);
//			
//		}
		
		
		/*������ˮ��*/
		if(Water_one_all>=WATER_ONE_MAX)
		{
			Water_switch = 0;
			Water_one_all = 0;
		}
		
	}

}


/*
	��ʾlcd��
*/
char show_buff[124];
void lcd_show_thread_entry(void *pargem)
{
	// һ�����ִ��ռ40
    while(1)
    {
		osDelay(900);
        if(net_connect)
        {
			if(user_uing != 1){
				show_picture(50,400,320,320,(uint16_t *)gImage_pic);
			}
			
//            LCD_Clear(WHITE);
            // ��ʾʱ��
            memset(show_buff, 0, sizeof(show_buff));
			Show_Str(10,40,200,32,"ʱ�䣺",32,0);
            sprintf(show_buff, "%d-%d-%d-%d-%d-%d", year_time, moon_time, sun_time, hour_time, minute_time, second_time);
            LCD_ShowString(90, 40, 460, 32, 32, (uint8_t *)show_buff);
			
			

            // ��ʾ�豸��
            memset(show_buff, 0, sizeof(show_buff));
            sprintf(show_buff, "%s     ", PROID);
			Show_Str(10,80,200,32,"�豸��:",32,0);
            LCD_ShowString(130, 80, 560, 32, 32, (uint8_t *)show_buff);

            // ��ʾ��ֵˮ��
            memset(show_buff, 0, sizeof(show_buff));
            sprintf(show_buff, "��ˮ��ߣ�%d    ��ˮ��ͣ�%d", TH_Temp, TL_Temp);
			Show_Str(10,120,560,32,(uint8_t*)show_buff,32,0);
			
            // ��ʾ�¶� ʪ��
            memset(show_buff, 0, sizeof(show_buff));
            sprintf(show_buff, "��ˮ��%.2f     ��ˮ��%.2f",temperature,Water_Hot);
			Show_Str(10,160,560,32,(uint8_t*)show_buff,32,0);
			
			memset(show_buff, 0, sizeof(show_buff));
            sprintf(show_buff, "ˮԴˮ��:%d    ��ˮˮ��:%d", Water_TDS1,Water_TDS2);
			Show_Str(10,200,560,32,(uint8_t*)show_buff,32,0);
			
			memset(show_buff, 0, sizeof(show_buff));
            sprintf(show_buff, "���ˮλ:%d    ���ˮλ��%d", Water_h, Water_l);
			Show_Str(10,240,560,32,(uint8_t*)show_buff,32,0);
			
			memset(show_buff, 0, sizeof(show_buff));
            sprintf(show_buff, "��ǰˮλ��%3d  ˮ������%3d", Water_c,Water_flow);
			Show_Str(10,280,560,32,(uint8_t*)show_buff,32,0);
			
			
			memset(show_buff, 0, sizeof(show_buff));
            sprintf(show_buff, "��ˮ��:%4d   ����ʣ��:%4d", Water_all,WATER_ONE_MAX-Water_one_all);
			Show_Str(10,320,560,32,(uint8_t*)show_buff,32,0);
			
			memset(show_buff, 0, sizeof(show_buff));
			if(Hot_flag)
				sprintf(show_buff, "������");
			else
				sprintf(show_buff, "������");
			Show_Str(10,360,560,32,(uint8_t*)show_buff,32,0);
			
			memset(show_buff, 0, sizeof(show_buff));
			if(user_uing)
				sprintf(show_buff, "����ˮ��������");
			else
				sprintf(show_buff, "��ɨ��ʹ�ã�������");
			Show_Str(150,360,560,32,(uint8_t*)show_buff,32,0);
			
			

			
			
			
		

			memset(show_buff, 0, sizeof(show_buff));
			if(state==1)
				sprintf(show_buff, "��ˮ��״̬����ͣ");
			else if(state == 2)
				sprintf(show_buff, "��ˮ��״̬������");
			else
				sprintf(show_buff, "��λ");
			Show_Str(10,720,320,32,(uint8_t*)show_buff,32,0);
			
			
			memset(show_buff, 0, sizeof(show_buff));
			if(Water_switch==1)
				sprintf(show_buff, "����ˮ");
			else if(Water_switch == 2)
				sprintf(show_buff, "����ˮ");
			else
				sprintf(show_buff, "�ر�");
			Show_Str(280,720,380,32,(uint8_t*)show_buff,32,0);


            //���
            if(net_connect == 1)
				Show_Str(10, 760, 200, 32, "���ӳɹ�!!!", 32, 0);
//                sprintf(show_buff, "connect success !!! %d bp = %d", user_mode, beep_flag);
            else if(net_connect == 0 )
				Show_Str(10, 760, 200, 32, "�ȴ�����!!!", 32, 0);
//                sprintf(show_buff, "wait connect!!! %d  bp = %d", user_mode, beep_flag);
            else
                Show_Str(10, 760, 200, 32, "������������!!!", 32, 0);
			
			// ��������
			Show_Str(260, 760, 200, 32, "��������:", 32, 0);
			memset(show_buff, 0, sizeof(show_buff));
			sprintf(show_buff,"%d",reconnect_num);
            LCD_ShowString(400, 760, 560, 32, 32, (uint8_t *)show_buff);
        }

    }


}
/* 
	���������߳� 
*/
void Send_data_Entry(void *pargem)
{
	char *Send_rece_Str = NULL;    // ���ͻ���������ָ��
	cJSON *cjson = NULL;
	while(1)
	{
			if((net_connect==1)&&(again_mode==0)&&(xQueueReceive( hsend_queue, &Send_rece_Str, portMAX_DELAY))) /* �õ�buffer��ַ��ֻ��4�ֽ� */
			{
				/* ���������� */
				if(Send_rece_Str!=NULL)
				{
					if(again_mode == 1){
						again_mode = 0;
					}
					printf("����ʣ��������� = %d\r\n",uxQueueMessagesWaiting(hsend_queue));
					
					Wait_send_mutex();
					printf("Get***************************: %s", Send_rece_Str);
					cjson = cJSON_Parse(Send_rece_Str); 
					if(cjson!=NULL){
						if(cJSON_GetObjectItem(cjson,"clientId")!=NULL)
						{
							OneNet_Publish("deviceOnlineStatus",Send_rece_Str);
						}
						else{
							OneNet_Publish("one",Send_rece_Str);
						}
						Send_rece_Str = NULL;
					}
					Release_send_mutex(); 
					
				}
				
			}
			osDelay(300);
		
	
	}

}





