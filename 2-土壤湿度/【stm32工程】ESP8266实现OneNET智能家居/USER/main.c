 /*-----------------------------------------------------*/
/*                                                     */
/*            ����main��������ں���Դ�ļ�              */
/*                                                     */
/*-----------------------------------------------------*/

#include "stm32f10x.h"   //������Ҫ��ͷ�ļ�
#include "control.h"     //������Ҫ��ͷ�ļ�
#include "delay.h"       //������Ҫ��ͷ�ļ�
#include "usart1.h"      //������Ҫ��ͷ�ļ�
#include "usart2.h"      //������Ҫ��ͷ�ļ�
#include "timer2.h"      //������Ҫ��ͷ�ļ�
#include "timer3.h"      //������Ҫ��ͷ�ļ�
#include "timer4.h"      //������Ҫ��ͷ�ļ�
#include "wifi.h"	     //������Ҫ��ͷ�ļ�
#include "led.h"	     //������Ҫ��ͷ�ļ� LED                                                  
#include "mqtt.h"        //������Ҫ��ͷ�ļ�
 //#include "ds18b20.h"   //������Ҫ��ͷ�ļ�  �¶�
#include "oled.h"
#include "adc.h"
#include <string.h>
#include "sys.h"
 #include "dht11.h"
 




u8 temperature;            //�¶�
u8 fan;                    //���ȿ���
u8 humidity;               //ʪ��
u16 value1,value2,value3;  //DMA���ݴ���������м�������ֱ���ȡ���գ����������������
u16 n=0;
u8 Lighting,Voice,Gas;     //���գ����������������
float temp1;
u8 LED_value;              //LED�Ƶ�ֵ��0��1
u16 ADC_Value[30];         //adc���ݻ�����
u8 buf1[10]={0};
u8 buf2[10]={0};
u8 buf3[10]={0};
u8 buf4[10]={0};
u8 buf5[10]={0};
u8 buf6[10]={0};
u16 i;   //forѭ���õ��ı�������

u8 temperature1[10]={0};
u8 hum[10]={0};
u8 Light[10]={0};
u8 Ga[10]={0};
u16 tu_si;
u8 fan;

u8 shui;

char *cmdLED_On = "LEDON";      //LED��
char *cmdLED_Off = "LEDOFF";    //LED�ر�
char *ledFlag = "LEDOFF";       //LED״̬




char *relayFlag = "relayOFF";       //�̵����ر�
char *relay_Off = "relayON";       //�̵�����
char *relay_On = "relayOFF";         //�̵����ر�



char *relay1Flag = "relay1OFF";       //�̵����ر�
char *relay1_Off = "relay1ON";       //�̵�����
char *relay1_On = "relay1OFF";         //�̵����ر�

char *relay2Flag = "relay2OFF";       //�̵����ر�
char *relay2_Off = "relay2ON";       //�̵�����
char *relay2_On = "relay2OFF";         //�̵����ر�

char *relay3Flag = "relay3OFF";       //�̵����ر�
char *relay3_Off = "relay3ON";       //�̵�����
char *relay3_On = "relay3OFF";         //�̵����ر�

int main(void) 
{	
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�����ж����ȼ�����
	Delay_Init();          //systick��ʱ���ܳ�ʼ��              
	Usart1_Init(115200);   //����1���ܳ�ʼ����������115200
	Usart2_Init(115200);   //����2���ܳ�ʼ����������115200	
	TIM4_Init(500,7200);   //TIM4��ʼ������ʱʱ�� 500*7200*1000/72000000 = 50ms
	LED_Init();			   //LED��ʼ��
	relay_init();
//	relay1_init();
//	relay2_init();
//	relay3_init();
	Buzzer_Init();
    ADCx_Init();                //��ʼ��ADC�����ڲɼ����������գ�����Ũ��
	WiFi_EnableIO_Init();  //��ʼ��WiFiģ��ESP8266��ʹ������PD/EN
	WiFi_ResetIO_Init();   //��ʼ��WiFi�ĸ�λIO
	IoT_Parameter_Init();  //��ʼ��MQTT�������Ĳ���	
    OLED_Init();

	
	while(1)
	{		
		
		
		show();                     //OLED��ʾ������
//=====================================��ʪ��ģ��===================================================	
	  if(DHT11_Read_Data(&temperature,&humidity) == 0)
		{
			sprintf((char *)buf1,":%d",temperature);
			sprintf((char *)buf2,":%d%%RH",humidity);
			OLED_ShowStr(32,0,buf1,2);
			OLED_ShowStr(32,2,buf2,2);
		}
		
//=======================�����գ��������к����壩�������������ݵĻ�ȡ��ͨ��ADC����ͨ����ѯ��ȡ========		
    for(i=0,value1=0,value2=0,value3=0;i<30;)
		{
			value1+=ADC_Value[i++];	
			value2+=ADC_Value[i++];
			value3+=ADC_Value[i++];
		}			
		Lighting=(4096-value1/10)*100/4096; //����
		temp1=(float)(value2/10)*(3.3/4096);//��������
        tu_si=(4096-value3/10); //shidu
//        printf("val1 = %d   val2 = %d  val3= %d \r\n",value1,value2,value3);
//        printf("LIGHRT = %d    Tu = %d",Lighting,tu_si);

		
		if(temp1<0.9)
        {
            n=temp1/0.001;
            Gas = n*0.012;
//				printf("һ����̼Ũ��:%dppm\n",Gas);
        }
        else
        {
            Gas = ((temp1-0.9)/0.1)*100;
            Gas =Gas+9;
//				printf("һ����̼Ũ��:%dppm\n",Gas);
        }
		sprintf((char *)buf3,":%d%%",Lighting);
        sprintf((char *)buf4,":%d%%",(4096-value3/10));
		sprintf((char *)buf5,":%d   ",Gas);
//        Gas = value2/10;
//        printf("  GAS =%d \r\n",Gas);
		OLED_ShowStr(32,4,buf3,2);//OLED��ʾ����
        OLED_ShowStr(96,4,buf4,2);//OLED��ʾʪ��
		OLED_ShowStr(64,6,buf5,2);//OLED��ʾ��������
        
        
        value1=0;
		value2=0;
		value3=0;
				
//===========================================================����������===================================
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
	
		// connectFlag=1ͬ���������������� 
		if(connectFlag == 1) 
		{     
			/*-------------------------------------------------------------*/
			/*                     �����ͻ���������						 */
			/*-------------------------------------------------------------*/
			if(MQTT_TxDataOutPtr != MQTT_TxDataInPtr) //if�����Ļ���˵�����ͻ�������������
																							  //��������ָ�벻���ڳ�ʼֵ�����ƶ�����
			{                
				//3������ɽ���if
				//��1�֣�0x10 ���ӱ��ġ�0x10��0x82Ϊ�̶�ͷ
				//��2�֣�0x82 ���ı��������ӱ��ĳɹ�����connectPackFlag��λ����ʾ���ӱ��ĳɹ�
				//			 0x10��0x82Ϊ�̶�ͷ�������ķ��뷢�ͻ������ǣ��Ǵӵ�3���ֽڿ�ʼ�ŵģ�indexΪ2���ο�mqtt.c��TxDataBuf_Deal��������
				//��3�֣��������ġ���subcribePackFlag��λ��˵�����ӺͶ��ľ��ɹ����������Ŀɷ���
				if((MQTT_TxDataOutPtr[2] == 0x10)||((MQTT_TxDataOutPtr[2] == 0x82)&&(connectPackFlag == 1))||(subcribePackFlag == 1)) 
				{    
					if (MQTT_TxDataOutPtr[2] == 0x30) 
					{	
						u1_printf("��������:0x%x����Ƭ������������������\r\n", MQTT_TxDataOutPtr[2]);	
						u1_printf("\r\n");
					}
					else 
					{
						u1_printf("��������:0x%x\r\n", MQTT_TxDataOutPtr[2]);//������ʾ��Ϣ
					}
					
					MQTT_TxData(MQTT_TxDataOutPtr);							//��������
					MQTT_TxDataOutPtr += TBUFF_UNIT;						//ָ������
					if(MQTT_TxDataOutPtr == MQTT_TxDataEndPtr)				//���ָ�뵽������β����
					{ 
						MQTT_TxDataOutPtr = MQTT_TxDataBuf[0];				//ָ���λ����������ͷ
					}	
				} 				
			}
			/*-------------------------------------------------------------*/
			/*                     ������ջ���������                      */
			/*-------------------------------------------------------------*/
			if(MQTT_RxDataOutPtr != MQTT_RxDataInPtr) //if�����Ļ���˵�����ջ�������������	
			{												
				u1_printf("���յ�����:");
				/*-----------------------------------------------------*/
				/*                    ����CONNACK����                  */
				/*-----------------------------------------------------*/				
				//if�жϣ������һ���ֽ���0x20����ʾ�յ�����CONNACK����
				//��������Ҫ�жϵ�4���ֽڣ�����CONNECT�����Ƿ�ɹ�
				if(MQTT_RxDataOutPtr[2] == 0x20)
				{             			
					switch(MQTT_RxDataOutPtr[5])
					{					   
						case 0x00 : u1_printf("CONNECT���ĳɹ�\r\n");							//���������Ϣ	
										connectPackFlag = 1;									//CONNECT���ĳɹ������ı��Ŀɷ�
									break;														//������֧case 0x00                                              
						case 0x01 : u1_printf("�����Ѿܾ�����֧�ֵ�Э��汾��׼������\r\n");    //���������Ϣ
									connectFlag = 0;											//connectFlag���㣬��������
									break;														//������֧case 0x01   
						case 0x02 : u1_printf("�����Ѿܾ������ϸ�Ŀͻ��˱�ʶ����׼������\r\n");//���������Ϣ
									connectFlag = 0;                                            //connectFlag���㣬��������
									break; 														//������֧case 0x02 
						case 0x03 : u1_printf("�����Ѿܾ�������˲����ã�׼������\r\n");	    //���������Ϣ
									connectFlag = 0;											//connectFlag���㣬��������
									break;														//������֧case 0x03
						case 0x04 : u1_printf("�����Ѿܾ�����Ч���û��������룬׼������\r\n");	//���������Ϣ
									connectFlag = 0;											//connectFlag���㣬��������						
									break;														//������֧case 0x04
						case 0x05 : u1_printf("�����Ѿܾ���δ��Ȩ��׼������\r\n");				//���������Ϣ
									connectFlag = 0;											//connectFlag���㣬��������						
									break;														//������֧case 0x05 		
						default   : u1_printf("�����Ѿܾ���δ֪״̬��׼������\r\n");		    //���������Ϣ 
									connectFlag = 0;											//connectFlag���㣬��������					
									break;														//������֧case default 								
					}				
			    }			
				//if�жϣ���һ���ֽ���0x90����ʾ�յ�����SUBACK����
				//��������Ҫ�ж϶��Ļظ��������ǲ��ǳɹ�
				else if(MQTT_RxDataOutPtr[2] == 0x90)
				{ 
					switch(MQTT_RxDataOutPtr[6])
					{					
						case 0x00 :
						case 0x01 : 
									u1_printf("���ĳɹ�\r\n");			//���������Ϣ
									subcribePackFlag = 1;				//subcribePackFlag��1����ʾ���ı��ĳɹ����������Ŀɷ���
									pingFlag = 0;						//pingFlag����
									TIM3_ENABLE_30S();					//����30s��PING��ʱ��	
									Send_Data();
									TIM2_ENABLE_10S();
									break;								//������֧                                             
						default: 
									u1_printf("����ʧ�ܣ�׼������\r\n");//���������Ϣ 
									connectFlag = 0;					//connectFlag���㣬��������
									break;								//������֧ 								
						}					
			    }
				//if�жϣ���һ���ֽ���0xD0����ʾ�յ�����PINGRESP����
				else if(MQTT_RxDataOutPtr[2] == 0xD0)
				{ 
					u1_printf("PING���Ļظ�\r\n");                      //���������Ϣ 
					if(pingFlag == 1)
					{                   						        //���pingFlag=1����ʾ��һ�η���
						 pingFlag = 0;    				       			//Ҫ���pingFlag��־
					}
					else if(pingFlag > 1)	
					{ 				 									//���pingFlag>1����ʾ�Ƕ�η����ˣ�������2s����Ŀ��ٷ���
						pingFlag = 0;     				      			//Ҫ���pingFlag��־
						TIM3_ENABLE_30S(); 				      			//PING��ʱ���ػ�30s��ʱ��
					}				
				}	
				//if�жϣ������һ���ֽ���0x30����ʾ�յ����Ƿ�������������������
				//����Ҫ��ȡ��������
				else if(MQTT_RxDataOutPtr[2] == 0x30)
			    { 
					u1_printf("�������ȼ�0����\r\n"); 		   	//���������Ϣ 
					MQTT_DealPushdata_Qs0(MQTT_RxDataOutPtr);   //����ȼ�0��������
				}									
				MQTT_RxDataOutPtr += RBUFF_UNIT;                //ָ������
				if(MQTT_RxDataOutPtr == MQTT_RxDataEndPtr)      //���ָ�뵽������β����
				{
					MQTT_RxDataOutPtr = MQTT_RxDataBuf[0];      //ָ���λ����������ͷ              
				}		          
			}//������ջ��������ݵ�else if��֧��β
						
			/*---------------------------------------------------------------------------------------------------------------------*/
			/*                    							 ���������������                   		          			         */
			/*---------------------------------------------------------------------------------------------------------------------*/
			if(MQTT_CMDOutPtr != MQTT_CMDInPtr)							  //if�����Ļ���˵�����������������	
			{                             		       
				u1_printf("����:%s\r\n",&MQTT_CMDOutPtr[2]);              //���������Ϣ
				
				if(!memcmp(&MQTT_CMDOutPtr[2],cmdLED_On,strlen(cmdLED_On)))//�ж�ָ������CMD1 
				{                                            
					ledFlag = "LEDON";  								             
					LED_On();									  		   //LED����
				}
				else if(!memcmp(&MQTT_CMDOutPtr[2],cmdLED_Off,strlen(cmdLED_Off))) //�ж�ָ������CMD1
				{ 
					ledFlag = "LEDOFF";                                              
					LED_Off(); 								                //LED�ر�
				}
				else if(!memcmp(&MQTT_CMDOutPtr[2],relay_On,strlen(relay_On)))//�ж�ָ������CMD
				{                                            
					relayFlag = "relayON";  								             
					relay_on();				  		   //�̵�������
					
				}
				else if(!memcmp(&MQTT_CMDOutPtr[2],relay_Off,strlen(relay_Off))) //�ж�ָ������CMD
				{ 
					relayFlag = "relayOFF";                                              
					relay_off();								                //�̵����ر�
				}
//					else if(!memcmp(&MQTT_CMDOutPtr[2],relay1_On,strlen(relay1_On))) //�ж�ָ������CMD
//				{                                            
//					relay1Flag = "relay1ON";  								             
//					relay1_on();				  		   //�̵�������
//					
//				}
//				else if(!memcmp(&MQTT_CMDOutPtr[2],relay1_Off,strlen(relay1_Off))) //�ж�ָ������CMD
//				{ 
//					relay1Flag = "relay1OFF";                                              
//					relay1_off();								                //�̵����ر�
//				}						
//				
//				else if(!memcmp(&MQTT_CMDOutPtr[2],relay2_On,strlen(relay2_On)))  //�ж�ָ������CMD
//				{                                            
//					relay2Flag = "relay2ON";  								             
//					relay2_on();				  		   //�̵�������
//					
//				}
//				else if(!memcmp(&MQTT_CMDOutPtr[2],relay2_Off,strlen(relay2_Off)))  //�ж�ָ������CMD
//				{ 
//					relay2Flag = "relay2OFF";                                              
//					relay2_off();								                //�̵����ر�
//				}
//				
//				else if(!memcmp(&MQTT_CMDOutPtr[2],relay3_On,strlen(relay3_On)))   //�ж�ָ������CMD
//				{                                            
//					relay3Flag = "relay3ON";  								             
//					relay3_on();				  		   //�̵�������
//					
//				}
//				else if(!memcmp(&MQTT_CMDOutPtr[2],relay3_Off,strlen(relay3_Off)))  //�ж�ָ������CMD
//				{ 
//					relay3Flag = "relay3OFF";                                              
//					relay3_off();								                //�̵����ر�
//				}
				//������������ᷢ�Ϳ�����״̬
				else u1_printf("δָ֪��\r\n");				//���������Ϣ	
			
				MQTT_CMDOutPtr += CBUFF_UNIT;				//ָ������
				if(MQTT_CMDOutPtr == MQTT_CMDEndPtr)	    //���ָ�뵽������β����
				MQTT_CMDOutPtr = MQTT_CMDBuf[0];			//ָ���λ����������ͷ	
				
				Send_Data();//���Ϳ����豸��״̬����
				//��������������ݵ�else if��֧��β					
			}	//connectFlag=1��if��֧�Ľ�β
		}
	/*--------------------------------------------------------------------*/
	/*      connectFlag=0ͬ�������Ͽ�������,Ҫ�������ӷ�����            */
	/*--------------------------------------------------------------------*/
		else
		{ 
			u1_printf("��Ҫ���ӷ�����\r\n");               //���������Ϣ
			TIM_Cmd(TIM4, DISABLE);                        //�ر�TIM4 
			TIM_Cmd(TIM3, DISABLE);                        //�ر�TIM3  
			WiFi_RxCounter = 0;                            //WiFi������������������                        
			memset(WiFi_RX_BUF, 0, WiFi_RXBUFF_SIZE);      //���WiFi���ջ����� 
			
			/**WiFi_Connect_IoTServer()����ESP8266�������ȵ�**/
			if(WiFi_Connect_IoTServer() == 0)			   //���WiFi�����Ʒ�������������0����ʾ��ȷ������if
			{   			     
				u1_printf("����TCP���ӳɹ�\r\n");          //���������Ϣ
				connectFlag = 1;                           //connectFlag��1����ʾ���ӳɹ�	
				WiFi_RxCounter = 0;                        //WiFi������������������                        
				memset(WiFi_RX_BUF, 0, WiFi_RXBUFF_SIZE);  //���WiFi���ջ����� 
				MQTT_Buff_Init();                          //��ʼ�����ͻ�����                    
			}				
		}
	}
}
