 /*-----------------------------------------------------*/
/*                                                     */
/*            程序main函数，入口函数源文件              */
/*                                                     */
/*-----------------------------------------------------*/

#include "stm32f10x.h"   //包含需要的头文件
#include "control.h"     //包含需要的头文件
#include "delay.h"       //包含需要的头文件
#include "usart1.h"      //包含需要的头文件
#include "usart2.h"      //包含需要的头文件
#include "timer2.h"      //包含需要的头文件
#include "timer3.h"      //包含需要的头文件
#include "timer4.h"      //包含需要的头文件
#include "wifi.h"	     //包含需要的头文件
#include "led.h"	     //包含需要的头文件 LED                                                  
#include "mqtt.h"        //包含需要的头文件
 //#include "ds18b20.h"   //包含需要的头文件  温度
#include "oled.h"
#include "adc.h"
#include <string.h>
#include "sys.h"
 #include "dht11.h"
 




u8 temperature;            //温度
u8 fan;                    //风扇开关
u8 humidity;               //湿度
u16 value1,value2,value3;  //DMA数据处理的三个中间变量，分别提取光照，音量，烟雾的数据
u16 n=0;
u8 Lighting,Voice,Gas;     //光照，音量，烟雾的数据
float temp1;
u8 LED_value;              //LED灯的值，0或1
u16 ADC_Value[30];         //adc数据缓冲区
u8 buf1[10]={0};
u8 buf2[10]={0};
u8 buf3[10]={0};
u8 buf4[10]={0};
u8 buf5[10]={0};
u8 buf6[10]={0};
u16 i;   //for循环用到的变量而已

u8 temperature1[10]={0};
u8 hum[10]={0};
u8 Light[10]={0};
u8 Ga[10]={0};
u16 tu_si;
u8 fan;

u8 shui;

char *cmdLED_On = "LEDON";      //LED打开
char *cmdLED_Off = "LEDOFF";    //LED关闭
char *ledFlag = "LEDOFF";       //LED状态




char *relayFlag = "relayOFF";       //继电器关闭
char *relay_Off = "relayON";       //继电器打开
char *relay_On = "relayOFF";         //继电器关闭



char *relay1Flag = "relay1OFF";       //继电器关闭
char *relay1_Off = "relay1ON";       //继电器打开
char *relay1_On = "relay1OFF";         //继电器关闭

char *relay2Flag = "relay2OFF";       //继电器关闭
char *relay2_Off = "relay2ON";       //继电器打开
char *relay2_On = "relay2OFF";         //继电器关闭

char *relay3Flag = "relay3OFF";       //继电器关闭
char *relay3_Off = "relay3ON";       //继电器打开
char *relay3_On = "relay3OFF";         //继电器关闭

int main(void) 
{	
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置中断优先级分组
	Delay_Init();          //systick延时功能初始化              
	Usart1_Init(115200);   //串口1功能初始化，波特率115200
	Usart2_Init(115200);   //串口2功能初始化，波特率115200	
	TIM4_Init(500,7200);   //TIM4初始化，定时时间 500*7200*1000/72000000 = 50ms
	LED_Init();			   //LED初始化
	relay_init();
//	relay1_init();
//	relay2_init();
//	relay3_init();
	Buzzer_Init();
    ADCx_Init();                //初始化ADC，用于采集音量，光照，烟雾浓度
	WiFi_EnableIO_Init();  //初始化WiFi模块ESP8266的使能引脚PD/EN
	WiFi_ResetIO_Init();   //初始化WiFi的复位IO
	IoT_Parameter_Init();  //初始化MQTT服务器的参数	
    OLED_Init();

	
	while(1)
	{		
		
		
		show();                     //OLED显示主界面
//=====================================温湿度模块===================================================	
	  if(DHT11_Read_Data(&temperature,&humidity) == 0)
		{
			sprintf((char *)buf1,":%d",temperature);
			sprintf((char *)buf2,":%d%%RH",humidity);
			OLED_ShowStr(32,0,buf1,2);
			OLED_ShowStr(32,2,buf2,2);
		}
		
//=======================（光照，声音，有害气体）三个传感器数据的获取，通过ADC规则通道查询获取========		
    for(i=0,value1=0,value2=0,value3=0;i<30;)
		{
			value1+=ADC_Value[i++];	
			value2+=ADC_Value[i++];
			value3+=ADC_Value[i++];
		}			
		Lighting=(4096-value1/10)*100/4096; //亮度
		temp1=(float)(value2/10)*(3.3/4096);//空气质量
        tu_si=(4096-value3/10); //shidu
//        printf("val1 = %d   val2 = %d  val3= %d \r\n",value1,value2,value3);
//        printf("LIGHRT = %d    Tu = %d",Lighting,tu_si);

		
		if(temp1<0.9)
        {
            n=temp1/0.001;
            Gas = n*0.012;
//				printf("一氧化碳浓度:%dppm\n",Gas);
        }
        else
        {
            Gas = ((temp1-0.9)/0.1)*100;
            Gas =Gas+9;
//				printf("一氧化碳浓度:%dppm\n",Gas);
        }
		sprintf((char *)buf3,":%d%%",Lighting);
        sprintf((char *)buf4,":%d%%",(4096-value3/10));
		sprintf((char *)buf5,":%d   ",Gas);
//        Gas = value2/10;
//        printf("  GAS =%d \r\n",Gas);
		OLED_ShowStr(32,4,buf3,2);//OLED显示亮度
        OLED_ShowStr(96,4,buf4,2);//OLED显示湿度
		OLED_ShowStr(64,6,buf5,2);//OLED显示空气质量
        
        
        value1=0;
		value2=0;
		value3=0;
				
//===========================================================蜂鸣器报警===================================
//		if(tu_si<40)
//		{
//			Buzzer=Buzzer_ON;
//            LED = LED_ON;
//		}		
//    else
//		{
//			Buzzer=Buzzer_ON;
//            LED = LED_OFF;
//		}			
	
		// connectFlag=1同服务器建立了连接 
		if(connectFlag == 1) 
		{     
			/*-------------------------------------------------------------*/
			/*                     处理发送缓冲区数据						 */
			/*-------------------------------------------------------------*/
			if(MQTT_TxDataOutPtr != MQTT_TxDataInPtr) //if成立的话，说明发送缓冲区有数据了
																							  //缓冲区的指针不等于初始值，被移动过。
			{                
				//3种情况可进入if
				//第1种：0x10 连接报文。0x10和0x82为固定头
				//第2种：0x82 订阅报文且连接报文成功。当connectPackFlag置位，表示连接报文成功
				//			 0x10和0x82为固定头。当报文放入发送缓冲区是，是从第3个字节开始放的，index为2。参考mqtt.c的TxDataBuf_Deal（）函数
				//第3种：其他报文。当subcribePackFlag置位，说明连接和订阅均成功，其他报文可发。
				if((MQTT_TxDataOutPtr[2] == 0x10)||((MQTT_TxDataOutPtr[2] == 0x82)&&(connectPackFlag == 1))||(subcribePackFlag == 1)) 
				{    
					if (MQTT_TxDataOutPtr[2] == 0x30) 
					{	
						u1_printf("发送数据:0x%x，单片机数据推送至服务器\r\n", MQTT_TxDataOutPtr[2]);	
						u1_printf("\r\n");
					}
					else 
					{
						u1_printf("发送数据:0x%x\r\n", MQTT_TxDataOutPtr[2]);//串口提示信息
					}
					
					MQTT_TxData(MQTT_TxDataOutPtr);							//发送数据
					MQTT_TxDataOutPtr += TBUFF_UNIT;						//指针下移
					if(MQTT_TxDataOutPtr == MQTT_TxDataEndPtr)				//如果指针到缓冲区尾部了
					{ 
						MQTT_TxDataOutPtr = MQTT_TxDataBuf[0];				//指针归位到缓冲区开头
					}	
				} 				
			}
			/*-------------------------------------------------------------*/
			/*                     处理接收缓冲区数据                      */
			/*-------------------------------------------------------------*/
			if(MQTT_RxDataOutPtr != MQTT_RxDataInPtr) //if成立的话，说明接收缓冲区有数据了	
			{												
				u1_printf("接收到数据:");
				/*-----------------------------------------------------*/
				/*                    处理CONNACK报文                  */
				/*-----------------------------------------------------*/				
				//if判断，如果第一个字节是0x20，表示收到的是CONNACK报文
				//接着我们要判断第4个字节，看看CONNECT报文是否成功
				if(MQTT_RxDataOutPtr[2] == 0x20)
				{             			
					switch(MQTT_RxDataOutPtr[5])
					{					   
						case 0x00 : u1_printf("CONNECT报文成功\r\n");							//串口输出信息	
										connectPackFlag = 1;									//CONNECT报文成功，订阅报文可发
									break;														//跳出分支case 0x00                                              
						case 0x01 : u1_printf("连接已拒绝，不支持的协议版本，准备重启\r\n");    //串口输出信息
									connectFlag = 0;											//connectFlag置零，重启连接
									break;														//跳出分支case 0x01   
						case 0x02 : u1_printf("连接已拒绝，不合格的客户端标识符，准备重启\r\n");//串口输出信息
									connectFlag = 0;                                            //connectFlag置零，重启连接
									break; 														//跳出分支case 0x02 
						case 0x03 : u1_printf("连接已拒绝，服务端不可用，准备重启\r\n");	    //串口输出信息
									connectFlag = 0;											//connectFlag置零，重启连接
									break;														//跳出分支case 0x03
						case 0x04 : u1_printf("连接已拒绝，无效的用户名或密码，准备重启\r\n");	//串口输出信息
									connectFlag = 0;											//connectFlag置零，重启连接						
									break;														//跳出分支case 0x04
						case 0x05 : u1_printf("连接已拒绝，未授权，准备重启\r\n");				//串口输出信息
									connectFlag = 0;											//connectFlag置零，重启连接						
									break;														//跳出分支case 0x05 		
						default   : u1_printf("连接已拒绝，未知状态，准备重启\r\n");		    //串口输出信息 
									connectFlag = 0;											//connectFlag置零，重启连接					
									break;														//跳出分支case default 								
					}				
			    }			
				//if判断，第一个字节是0x90，表示收到的是SUBACK报文
				//接着我们要判断订阅回复，看看是不是成功
				else if(MQTT_RxDataOutPtr[2] == 0x90)
				{ 
					switch(MQTT_RxDataOutPtr[6])
					{					
						case 0x00 :
						case 0x01 : 
									u1_printf("订阅成功\r\n");			//串口输出信息
									subcribePackFlag = 1;				//subcribePackFlag置1，表示订阅报文成功，其他报文可发送
									pingFlag = 0;						//pingFlag清零
									TIM3_ENABLE_30S();					//启动30s的PING定时器	
									Send_Data();
									TIM2_ENABLE_10S();
									break;								//跳出分支                                             
						default: 
									u1_printf("订阅失败，准备重启\r\n");//串口输出信息 
									connectFlag = 0;					//connectFlag置零，重启连接
									break;								//跳出分支 								
						}					
			    }
				//if判断，第一个字节是0xD0，表示收到的是PINGRESP报文
				else if(MQTT_RxDataOutPtr[2] == 0xD0)
				{ 
					u1_printf("PING报文回复\r\n");                      //串口输出信息 
					if(pingFlag == 1)
					{                   						        //如果pingFlag=1，表示第一次发送
						 pingFlag = 0;    				       			//要清除pingFlag标志
					}
					else if(pingFlag > 1)	
					{ 				 									//如果pingFlag>1，表示是多次发送了，而且是2s间隔的快速发送
						pingFlag = 0;     				      			//要清除pingFlag标志
						TIM3_ENABLE_30S(); 				      			//PING定时器重回30s的时间
					}				
				}	
				//if判断，如果第一个字节是0x30，表示收到的是服务器发来的推送数据
				//我们要提取控制命令
				else if(MQTT_RxDataOutPtr[2] == 0x30)
			    { 
					u1_printf("服务器等级0推送\r\n"); 		   	//串口输出信息 
					MQTT_DealPushdata_Qs0(MQTT_RxDataOutPtr);   //处理等级0推送数据
				}									
				MQTT_RxDataOutPtr += RBUFF_UNIT;                //指针下移
				if(MQTT_RxDataOutPtr == MQTT_RxDataEndPtr)      //如果指针到缓冲区尾部了
				{
					MQTT_RxDataOutPtr = MQTT_RxDataBuf[0];      //指针归位到缓冲区开头              
				}		          
			}//处理接收缓冲区数据的else if分支结尾
						
			/*---------------------------------------------------------------------------------------------------------------------*/
			/*                    							 处理命令缓冲区数据                   		          			         */
			/*---------------------------------------------------------------------------------------------------------------------*/
			if(MQTT_CMDOutPtr != MQTT_CMDInPtr)							  //if成立的话，说明命令缓冲区有数据了	
			{                             		       
				u1_printf("命令:%s\r\n",&MQTT_CMDOutPtr[2]);              //串口输出信息
				
				if(!memcmp(&MQTT_CMDOutPtr[2],cmdLED_On,strlen(cmdLED_On)))//判断指令，如果是CMD1 
				{                                            
					ledFlag = "LEDON";  								             
					LED_On();									  		   //LED开启
				}
				else if(!memcmp(&MQTT_CMDOutPtr[2],cmdLED_Off,strlen(cmdLED_Off))) //判断指令，如果是CMD1
				{ 
					ledFlag = "LEDOFF";                                              
					LED_Off(); 								                //LED关闭
				}
				else if(!memcmp(&MQTT_CMDOutPtr[2],relay_On,strlen(relay_On)))//判断指令，如果是CMD
				{                                            
					relayFlag = "relayON";  								             
					relay_on();				  		   //继电器开启
					
				}
				else if(!memcmp(&MQTT_CMDOutPtr[2],relay_Off,strlen(relay_Off))) //判断指令，如果是CMD
				{ 
					relayFlag = "relayOFF";                                              
					relay_off();								                //继电器关闭
				}
//					else if(!memcmp(&MQTT_CMDOutPtr[2],relay1_On,strlen(relay1_On))) //判断指令，如果是CMD
//				{                                            
//					relay1Flag = "relay1ON";  								             
//					relay1_on();				  		   //继电器开启
//					
//				}
//				else if(!memcmp(&MQTT_CMDOutPtr[2],relay1_Off,strlen(relay1_Off))) //判断指令，如果是CMD
//				{ 
//					relay1Flag = "relay1OFF";                                              
//					relay1_off();								                //继电器关闭
//				}						
//				
//				else if(!memcmp(&MQTT_CMDOutPtr[2],relay2_On,strlen(relay2_On)))  //判断指令，如果是CMD
//				{                                            
//					relay2Flag = "relay2ON";  								             
//					relay2_on();				  		   //继电器开启
//					
//				}
//				else if(!memcmp(&MQTT_CMDOutPtr[2],relay2_Off,strlen(relay2_Off)))  //判断指令，如果是CMD
//				{ 
//					relay2Flag = "relay2OFF";                                              
//					relay2_off();								                //继电器关闭
//				}
//				
//				else if(!memcmp(&MQTT_CMDOutPtr[2],relay3_On,strlen(relay3_On)))   //判断指令，如果是CMD
//				{                                            
//					relay3Flag = "relay3ON";  								             
//					relay3_on();				  		   //继电器开启
//					
//				}
//				else if(!memcmp(&MQTT_CMDOutPtr[2],relay3_Off,strlen(relay3_Off)))  //判断指令，如果是CMD
//				{ 
//					relay3Flag = "relay3OFF";                                              
//					relay3_off();								                //继电器关闭
//				}
				//不做处理，后面会发送控制器状态
				else u1_printf("未知指令\r\n");				//串口输出信息	
			
				MQTT_CMDOutPtr += CBUFF_UNIT;				//指针下移
				if(MQTT_CMDOutPtr == MQTT_CMDEndPtr)	    //如果指针到缓冲区尾部了
				MQTT_CMDOutPtr = MQTT_CMDBuf[0];			//指针归位到缓冲区开头	
				
				Send_Data();//发送控制设备的状态数据
				//处理命令缓冲区数据的else if分支结尾					
			}	//connectFlag=1的if分支的结尾
		}
	/*--------------------------------------------------------------------*/
	/*      connectFlag=0同服务器断开了连接,要重启连接服务器            */
	/*--------------------------------------------------------------------*/
		else
		{ 
			u1_printf("需要连接服务器\r\n");               //串口输出信息
			TIM_Cmd(TIM4, DISABLE);                        //关闭TIM4 
			TIM_Cmd(TIM3, DISABLE);                        //关闭TIM3  
			WiFi_RxCounter = 0;                            //WiFi接收数据量变量清零                        
			memset(WiFi_RX_BUF, 0, WiFi_RXBUFF_SIZE);      //清空WiFi接收缓冲区 
			
			/**WiFi_Connect_IoTServer()启动ESP8266，连接热点**/
			if(WiFi_Connect_IoTServer() == 0)			   //如果WiFi连接云服务器函数返回0，表示正确，进入if
			{   			     
				u1_printf("建立TCP连接成功\r\n");          //串口输出信息
				connectFlag = 1;                           //connectFlag置1，表示连接成功	
				WiFi_RxCounter = 0;                        //WiFi接收数据量变量清零                        
				memset(WiFi_RX_BUF, 0, WiFi_RXBUFF_SIZE);  //清空WiFi接收缓冲区 
				MQTT_Buff_Init();                          //初始化发送缓冲区                    
			}				
		}
	}
}
