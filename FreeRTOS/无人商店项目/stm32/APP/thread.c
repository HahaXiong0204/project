#include "thread.h"
#include "app.h"

float Distance;                      //超声波测得距离
struct AT24CDATA at_data_struct = {0};    // 掉电的结构体
char at_data_arr[256];


// 采集传感器的数据
// mode = 0 正常模式
// mode = 1 保存到at24c02
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

            if(HAL_GPIO_ReadPin(People_GPIO_Port, People_Pin))
            {
                printf("有人\r\n");
				people = 1;
				
            }
            else
            {
                printf("无人\r\n");
				people = 0;
				
            }
            // ==============================================================================================================


            //===============================================dht11读取======================================================
            DHT11_Read_Data(&humidity, &temperature);
            printf("temperature = %.2f		humidity = %.2f\r\n", temperature, humidity);
			osDelay(50);
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
            Lighting = (4096 - value1 / 10) * 100 / 4096; //亮度
            printf("检测光亮值为：%d%%\r\n", Lighting);

            // PA5
            voltage = (float)value2 / 10 * (3.3 / 4096);
            printf("检测电压值为1：%.3fV\r\n", voltage);

            // PA6
            Turbidity = (value3 / 10) * 100 / 4096; //浊度
            printf("检测浊度值为：%d\r\n", Turbidity);

            // PA7
            Gas = (value4 / 10) * 100 / 4096; //烟雾
            printf("检测烟雾值为：%d%%\r\n", Gas);
            value1 = value2 = value3 = value4 = 0;

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



// 按键处理线程
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
			if(user_uing==0)
			{
				switch(stor_flag)
				{
					case 1:{
						if(Water_g!=0){
							Water_g--;
							Water_num++;
						}
						break;
					}
					case 2:{
						if(bingan_g!=0){
							bingan_num++;
							bingan_g--;
						}
						break;
					}
					case 3:{
						if(yinliao_g!=0)
						{
							yinliao_g--;
							yinlian_num++;
						}
						break;
					}
					default:
						break;
					
				
				}
			
			}

			
			
			
			
            printf("KEY0_PRES\r\n");
            break;
        }
        case KEY1_PRES:
        {
			if(user_uing==0)
			{
				if(stor_flag>=MAX_NUM)
					stor_flag=MAX_NUM;
				else
					stor_flag++;
			}
            printf("KEY1_PRES\r\n");
            break;
        }
        case KEY2_PRES:
        {
			if(user_uing==0)
			{
				switch(stor_flag)
				{
					case 1:{
						Water_g++;
						Water_num--;
						break;
					}
					case 2:{
						bingan_g++;
						bingan_num--;
						break;
					}
					case 3:{
						yinliao_g++;
						yinlian_num--;
						break;
					}
					default:
						break;
					
				
				}
			}
            printf("KEY2_PRES\r\n");
            break;
        }
        case WKUP_PRES:
        {
			if(user_uing==0)
			{
				if(stor_flag<=1)
					stor_flag = 1;
				else
					stor_flag --;
			}
			
			
            printf("WKUP_PRES\r\n");
            break;
        }
        case RED_PRES:
        {
			Send_tianwen(1);
			vTaskResume(read_rfid_id);
			user_uing = 1;
            printf("RED_PRES\r\n");
            break;
        }
        case YELLOW_PRES:
        {
			vTaskSuspend(read_rfid_id);
			user_uing = 0;
            printf("YELLOW_PRES\r\n");
            break;
        }
        case GRREN_PRES:
        {
			Send_tianwen(1);
            printf("grean_pres\r\n");
            break;
        }

        }
        osDelay(10);

    }


}


#if RFID_FLAG
// 读rfid
void read_rfid_thread_entry(void *pargem)
{
    while(1)
    {
        //		printf("\r\nid");

		/*  扫描到rfid 挂起线程    如果要重新开启线程需要用 vTaskResume(read_rfid_id); */
        if(user_uing == 1)
        {
            while(ReaderCard() == 0)
            {
                printf("wait id\r\n");
            }
            user_uing = 3;
			Send_data_uing();
			Send_tianwen(1);
			osDelay(2000);
			Water_g =0;
			bingan_g = 0;
			yinliao_g = 0;
			calu_num =0;
			user_uing = 0;
			
			vTaskSuspend(read_rfid_id);
        }
        osDelay(2000);
    }
}
#endif




/* 逻辑判断线程  */
void logical_thread_entry(void *pargem)
{
	while(1)
	{
		osDelay(500);
		if(net_connect == 1)
        {
            if(user_mode == 0) // 自动模式
            {
				if(nua==1)
				{
					Send_data_kc();
					nua = 0;
				}
				
				
                if(temperature > TH_Temp)
                {
                    if(tem_flag != 1){
                        OneNet_Send_Int("tem_flag", 1);
						Send_tianwen(2);
					}
                    tem_flag = 1;
                    machinery_pwm_mode(1);
                    HAL_TIM_Base_Start_IT(&htim10);
                }
                else
                {
                    if(tem_flag != 0)
                    {
                        OneNet_Send_Int("tem_flag", 0);
                        beep_On_Off(1);
                    }
                    tem_flag = 0;
                    machinery_pwm_mode(0);
                }


                if(Gas > TH_Gas)
                {
                    if(Gas_flag != 1){
                        OneNet_Send_Int("Gas_flag", 1);
						Send_tianwen(3);
					}
                    Gas_flag = 1;
                    HAL_TIM_Base_Start_IT(&htim10);
                }
                else
                {
                    if(Gas_flag != 0)
                    {
                        OneNet_Send_Int("Gas_flag", 0);
                        beep_On_Off(1);
                    }
                    Gas_flag = 0;

                }
				
				
				if(people==1)
				{
					Led_pwm_mode(2);
				}
				else
				{
					Led_pwm_mode(0);
				}
				
				if(people_flag == 3)
				{
					Led_pwm_mode(0);
					people_flag = 0;
					
				}
/*
                if(Lighting < TH_light)
                {
                    Led_pwm_mode(1);
                    if(light_flag != 1)
                        OneNet_Send_Int("light_flag", 1);
                    light_flag = 1;
                }
                else
                {
                    Led_pwm_mode(0);
                    if(light_flag != 0)
                    {
                        OneNet_Send_Int("light_flag", 0);
                        beep_On_Off(1);
                    }
                    light_flag = 0;

                }

*/
            }



            
        }
		
		
		
	}

}



/*显示lcd上*/
char show_buff[124];
uint8_t peo_num = 0;
void lcd_show_thread_entry(void *pargem)
{
	// 一个汉字大概占40
    while(1)
    {
		osDelay(1000);
        if(net_connect)
        {
            LCD_Clear(WHITE);
            // 显示时间
            memset(show_buff, 0, sizeof(show_buff));
			Show_Str(10,40,200,32,"时间：",32,0);
            sprintf(show_buff, "%d-%2d-%2d-%2d-%2d-%2d", year_time, moon_time, sun_time, hour_time, minute_time, second_time);
            LCD_ShowString(90, 40, 460, 32, 32, (uint8_t *)show_buff);
			
			
			
			// 人体传感器
			if(people == 1)
			{
				if(peo_num==0)
				{
					Send_tianwen(4);
					peo_num = 1;
				}
				
				Show_Str(400, 40, 200, 32, "有人", 32, 0);
			}
			else
			{
				peo_num = 0;
				Show_Str(400, 40, 200, 32, "无人", 32, 0);
			}

            // 显示设备号
            memset(show_buff, 0, sizeof(show_buff));
            sprintf(show_buff, "%s     RFID:%3d", PROID,ID_Cart);
			Show_Str(10,80,200,32,"设备号:",32,0);
            LCD_ShowString(130, 80, 560, 32, 32, (uint8_t *)show_buff);

            // 显示阈值水温
            memset(show_buff, 0, sizeof(show_buff));
            sprintf(show_buff, "温度:%.2f   温度阈值:%3d", temperature, TH_Temp);
			Show_Str(10,120,560,32,(uint8_t*)show_buff,32,0);

            // 显示
            memset(show_buff, 0, sizeof(show_buff));
            sprintf(show_buff, "空气质量%2d    空气阈值%2d", Gas, TH_Gas);
			Show_Str(10,160,560,32,(uint8_t*)show_buff,32,0);
			
			memset(show_buff, 0, sizeof(show_buff));
            sprintf(show_buff, "商品  价格  库存  购买  选择");
			Show_Str(10,200,560,32,(uint8_t*)show_buff,32,0);
			
			// ============================================================  240
			memset(show_buff, 0, sizeof(show_buff));
            sprintf(show_buff, "矿泉水  %.1f   %2d    %2d",Water_p,Water_num,Water_g);
			Show_Str(10,240,560,32,(uint8_t*)show_buff,32,0);
			
			
			memset(show_buff, 0, sizeof(show_buff));
            sprintf(show_buff, "饼干    %.1f   %2d    %2d",bingan_p,bingan_num,bingan_g);
			Show_Str(10,280,560,32,(uint8_t*)show_buff,32,0);
			
			memset(show_buff, 0, sizeof(show_buff));
            sprintf(show_buff, "饮料    %.1f   %2d    %2d",yinliao_p,yinlian_num,yinliao_g);
			Show_Str(10,320,560,32,(uint8_t*)show_buff,32,0);
			
			
			memset(show_buff, 0, sizeof(show_buff));
			sprintf(show_buff, "<-");
			Show_Str(400,240+(40*(stor_flag-1)),560,32,(uint8_t*)show_buff,32,0);



            //最后
			calu_num = (Water_g*Water_p)+(bingan_g*bingan_p)+(yinliao_g*yinliao_p);
			
			printf("总金额：%.2f\r\n",calu_num);
			memset(show_buff, 0, sizeof(show_buff));
			if(user_uing==1)
				sprintf(show_buff, "总金额：%.2f   请刷卡!!!",calu_num);
			else if(user_uing==0)
				sprintf(show_buff, "总金额：%.2f   请选择商品!!!",calu_num);
			else
				sprintf(show_buff, "总金额：%.2f   刷卡成功!!!",calu_num);
			Show_Str(10, 720, 560, 32, (uint8_t*)show_buff, 32, 0);
			
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

/* 发送数据线程 */
void Send_data_Entry(void *pargem)
{
	char *Send_rece_Str = NULL;    // 发送缓存区接收指针
	cJSON *cjson = NULL;
	while(1)
	{
			if((net_connect==1)&&(xQueueReceive( hsend_queue, &Send_rece_Str, portMAX_DELAY))) /* 得到buffer地址，只是4字节 */
			{
				/* 读到了数据 */
				if(Send_rece_Str!=NULL)
				{
					Wait_send_mutex();
					printf("Get***************************: %s", Send_rece_Str);
					cjson = cJSON_Parse(Send_rece_Str); 
					if(cjson!=NULL)
						OneNet_Publish("one",Send_rece_Str);
					Release_send_mutex(); 
					
				}
				
			}
			osDelay(300);
		
	
	}

}



///*  接收处理数据线程  */
//void Reve_thread_Entry(void *pargem)
//{
//	char *receivedStr = NULL;
//	while(1)
//	{
//		if ((xQueueReceive(hreve_queue, &receivedStr, portMAX_DELAY))) {
//				// 处理接收到的字符串指针
//				if(receivedStr != NULL)//如果返回数据不为空
//				{
//					Wait_reve_mutex();
//					OneNet_RevPro((uint8_t*)receivedStr);//平台返回数据检测
//					Release_reve_mutex();
//				}	
//				
//		}
//		osDelay(100);
//	}


//}

