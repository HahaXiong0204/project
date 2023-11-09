#include "thread.h"
#include "app.h"
#include "pic.h"

float Distance;                      //超声波测得距离
struct AT24CDATA at_data_struct = {0};    // 掉电的结构体
char at_data_arr[256];

/*
	采集传感器的数据
	mode = 0 正常模式
	mode = 1 保存到at24c02
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
			
            // =============================================== 速度 ==================================================
            speed = TIM4->CNT;
            printf("speed = %d\r\n", TIM4->CNT);
            TIM4->CNT = 0;
            // =============================================================================================================

            // =============================================== 人体传感器 ==================================================
//			
			Hcsr04Start();
			printf("dis = %.1f\r\n",Hcsr04Read());
//			
			
//            if(HAL_GPIO_ReadPin(People_GPIO_Port, People_Pin))
//            {
//                printf("有人\r\n");
//				people = 1;
//				
//            }
//            else
//            {
//                printf("无人\r\n");
//				people = 0;
//				
//            }
            // ==============================================================================================================


            //===============================================dht11读取======================================================
//            DHT11_Read_Data(&humidity, &temperature);
//            printf("temperature = %.2f		humidity = %.2f\r\n", temperature, humidity);
//			osDelay(50);
			
			temperature = ((float)DS18B20_Get_Temp()/10);
			Water_Hot = ((float)DS18B20_2_Get_Temp()/10);   // 热水
			
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
            printf("检测Water_TDS1值为：%d%%\r\n", Water_TDS1);

            // PA6 电压
//            voltage  = (float)value3 / 10 * (3.3 / 4096);
//			Water_TDS1 = 110*((5-voltage)/voltage);
//            printf("检测水源水质值为1：%dV\r\n", Water_TDS1);

            // PA5
            voltage = (float)value2 / 10 * (3.3 / 4096);
			Water_TDS2 = 110*((5-voltage)/voltage);
            printf("检测饮水水质值为：%d\r\n", Water_TDS2);

            // PA7
//            Gas = (value4 / 10) * 100 / 4096; //烟雾
//            printf("检测烟雾值为：%d%%\r\n", Gas);
//            value1 = value2 = value3 = value4 = 0;

            // 保存到at24c02
            if(net_connect == 2 || net_connect == 3)
            {
                sensor_again_num++;
                if(sensor_again_num > 3)
                {
                    sensor_again_num = 0;
                    memset(at_data_arr, 0, sizeof(at_data_arr));
                    sprintf(at_data_arr, "%.2f-%.2f-%d-%.2f-%d-%d", temperature, humidity, Lighting, voltage, Turbidity, Gas);
                    printf("保存数据:%s    len = %d\r\n", at_data_arr, strlen(at_data_arr));

                    AT24C02_write(at_data_struct.addr, (uint8_t *)at_data_arr, strlen(at_data_arr));
                    at_data_struct.arr[at_data_struct.len++] = strlen(at_data_arr);
                    memset(at_data_arr, 0, sizeof(at_data_arr));
                    AT24C02_read(at_data_struct.addr, (uint8_t *)at_data_arr, at_data_struct.arr[at_data_struct.len - 1]);
                    at_data_struct.addr += strlen(at_data_arr);
                    printf("保存数据%d 条 %d   %s\r\n", at_data_struct.len, at_data_struct.addr, at_data_arr);
                }
				osDelay(200);

            }

			Send_Zigbee();   // 发送到zigbee中
        }
		

    }


}

/*
	按键处理线程
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
			/*热水*/
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
			/*温水*/
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
			OneNet_Send_one_Data(1);   // 结算
			beep_one();
            printf("grean_pres\r\n");
            break;
        }

        }
        osDelay(10);

    }


}

/*
	读rfid的值
*/
#if RFID_FLAG
// 读rfid
void read_rfid_thread_entry(void *pargem)
{
    while(1)
    {
        //		printf("\r\nid");

		/*  扫描到rfid 挂起线程    如果要重新开启线程需要用 vTaskResume(read_rfid_id); */
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
	逻辑判断线程  
*/
uint8_t Error_num = 0;
uint8_t Warning_num = 0;
uint8_t hot_auto_flag = 0;
void logical_thread_entry(void *pargem)
{
	while(1)
	{
		osDelay(500);
		
		/* 水位低 */
		if(HAL_GPIO_ReadPin(approach_switch_GPIO_Port,approach_switch_Pin)==0)   
		{
			Water_c = Water_l;
			Water_leve_flag = 1;
		}
		/*当前水位低于最低水位*/
		if(Water_c<=Water_l)
		{
			machinery_pwm_mode(2);
			Water_leve_flag = 1;
		}
		

		/* 水位高 */
		if(HAL_GPIO_ReadPin(red_input_GPIO_Port,red_input_Pin)==1)   
		{
			Water_c = Water_h;
			machinery_pwm_mode(0);
			Water_leve_flag = 0;
		}
		/*当前水位高于最高水位*/
		if(Water_c>=Water_h)
		{
			machinery_pwm_mode(0);
			Water_leve_flag = 0;
		}
		
		// 警报
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
		
		/* 水源水质警告 */ 
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
		
		/* 加热 */
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
		
		
		/* 出水开关 */
//		if(Water_switch==1)   // 温水
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
		
		
		/*单次饮水量*/
		if(Water_one_all>=WATER_ONE_MAX)
		{
			Water_switch = 0;
			Water_one_all = 0;
		}
		
	}

}


/*
	显示lcd上
*/
char show_buff[124];
void lcd_show_thread_entry(void *pargem)
{
	// 一个汉字大概占40
    while(1)
    {
		osDelay(900);
        if(net_connect)
        {
			if(user_uing != 1){
				show_picture(50,400,320,320,(uint16_t *)gImage_pic);
			}
			
//            LCD_Clear(WHITE);
            // 显示时间
            memset(show_buff, 0, sizeof(show_buff));
			Show_Str(10,40,200,32,"时间：",32,0);
            sprintf(show_buff, "%d-%d-%d-%d-%d-%d", year_time, moon_time, sun_time, hour_time, minute_time, second_time);
            LCD_ShowString(90, 40, 460, 32, 32, (uint8_t *)show_buff);
			
			

            // 显示设备号
            memset(show_buff, 0, sizeof(show_buff));
            sprintf(show_buff, "%s     ", PROID);
			Show_Str(10,80,200,32,"设备号:",32,0);
            LCD_ShowString(130, 80, 560, 32, 32, (uint8_t *)show_buff);

            // 显示阈值水温
            memset(show_buff, 0, sizeof(show_buff));
            sprintf(show_buff, "热水最高：%d    热水最低：%d", TH_Temp, TL_Temp);
			Show_Str(10,120,560,32,(uint8_t*)show_buff,32,0);
			
            // 显示温度 湿度
            memset(show_buff, 0, sizeof(show_buff));
            sprintf(show_buff, "温水：%.2f     热水：%.2f",temperature,Water_Hot);
			Show_Str(10,160,560,32,(uint8_t*)show_buff,32,0);
			
			memset(show_buff, 0, sizeof(show_buff));
            sprintf(show_buff, "水源水质:%d    饮水水质:%d", Water_TDS1,Water_TDS2);
			Show_Str(10,200,560,32,(uint8_t*)show_buff,32,0);
			
			memset(show_buff, 0, sizeof(show_buff));
            sprintf(show_buff, "最高水位:%d    最低水位：%d", Water_h, Water_l);
			Show_Str(10,240,560,32,(uint8_t*)show_buff,32,0);
			
			memset(show_buff, 0, sizeof(show_buff));
            sprintf(show_buff, "当前水位：%3d  水流量：%3d", Water_c,Water_flow);
			Show_Str(10,280,560,32,(uint8_t*)show_buff,32,0);
			
			
			memset(show_buff, 0, sizeof(show_buff));
            sprintf(show_buff, "饮水量:%4d   单次剩余:%4d", Water_all,WATER_ONE_MAX-Water_one_all);
			Show_Str(10,320,560,32,(uint8_t*)show_buff,32,0);
			
			memset(show_buff, 0, sizeof(show_buff));
			if(Hot_flag)
				sprintf(show_buff, "加热中");
			else
				sprintf(show_buff, "不加热");
			Show_Str(10,360,560,32,(uint8_t*)show_buff,32,0);
			
			memset(show_buff, 0, sizeof(show_buff));
			if(user_uing)
				sprintf(show_buff, "请用水！！！！");
			else
				sprintf(show_buff, "请扫描使用！！！！");
			Show_Str(150,360,560,32,(uint8_t*)show_buff,32,0);
			
			

			
			
			
		

			memset(show_buff, 0, sizeof(show_buff));
			if(state==1)
				sprintf(show_buff, "饮水机状态：暂停");
			else if(state == 2)
				sprintf(show_buff, "饮水机状态：正常");
			else
				sprintf(show_buff, "复位");
			Show_Str(10,720,320,32,(uint8_t*)show_buff,32,0);
			
			
			memset(show_buff, 0, sizeof(show_buff));
			if(Water_switch==1)
				sprintf(show_buff, "出温水");
			else if(Water_switch == 2)
				sprintf(show_buff, "出热水");
			else
				sprintf(show_buff, "关闭");
			Show_Str(280,720,380,32,(uint8_t*)show_buff,32,0);


            //最后
            if(net_connect == 1)
				Show_Str(10, 760, 200, 32, "连接成功!!!", 32, 0);
//                sprintf(show_buff, "connect success !!! %d bp = %d", user_mode, beep_flag);
            else if(net_connect == 0 )
				Show_Str(10, 760, 200, 32, "等待连接!!!", 32, 0);
//                sprintf(show_buff, "wait connect!!! %d  bp = %d", user_mode, beep_flag);
            else
                Show_Str(10, 760, 200, 32, "正在重新连接!!!", 32, 0);
			
			// 重连次数
			Show_Str(260, 760, 200, 32, "重连次数:", 32, 0);
			memset(show_buff, 0, sizeof(show_buff));
			sprintf(show_buff,"%d",reconnect_num);
            LCD_ShowString(400, 760, 560, 32, 32, (uint8_t *)show_buff);
        }

    }


}
/* 
	发送数据线程 
*/
void Send_data_Entry(void *pargem)
{
	char *Send_rece_Str = NULL;    // 发送缓存区接收指针
	cJSON *cjson = NULL;
	while(1)
	{
			if((net_connect==1)&&(again_mode==0)&&(xQueueReceive( hsend_queue, &Send_rece_Str, portMAX_DELAY))) /* 得到buffer地址，只是4字节 */
			{
				/* 读到了数据 */
				if(Send_rece_Str!=NULL)
				{
					if(again_mode == 1){
						again_mode = 0;
					}
					printf("队列剩余可用数据 = %d\r\n",uxQueueMessagesWaiting(hsend_queue));
					
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





