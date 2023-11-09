
/*-------------------------------------------------*/
/*                                                 */
/*          	 WIFI��ESP8266��Դ�ļ�             */
/*                                                 */
/*-------------------------------------------------*/

// Ӳ�����ӣ�
// PA2 RX
// PA3 TX
// PG7 PD/EN
// PE6 ��λ

#include "stm32f10x.h"  //������Ҫ��ͷ�ļ�
#include "wifi.h"	    //������Ҫ��ͷ�ļ�
#include "delay.h"	    //������Ҫ��ͷ�ļ�
#include "usart1.h"	    //������Ҫ��ͷ�ļ�
#include "oled.h"	    //������Ҫ��ͷ�ļ�

char wifi_mode = 0;     //����ģʽ 0��SSID������д�ڳ�����   1��Smartconfig��ʽ��APP����
extern u16 i;
/*-------------------------------------------------*/
/*����������ʼ��ESP8266��PD/EN���ţ���ʹ��                       */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void WiFi_EnableIO_Init()
{
  GPIO_InitTypeDef GPIO_InitStructure;                    //����һ������IO�˿ڲ����Ľṹ��
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB , ENABLE); //ʹ��PG�˿�ʱ��
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;               //׼������PG7
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;       //����50Mhz
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   	    //���������ʽ
	GPIO_Init(GPIOB, &GPIO_InitStructure);            	    //����PE6
	
	GPIO_SetBits(GPIOB,GPIO_Pin_12);													//ʹ��PD/EN,�ߵ�ƽ
}	
/*-------------------------------------------------*/
/*����������ʼ��ESP8266�ĸ�λIO                       */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void WiFi_ResetIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;                    //����һ������IO�˿ڲ����Ľṹ��
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB , ENABLE); //ʹ��PE�˿�ʱ��
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;               //׼������PE6
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;       //����50Mhz
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   	    //���������ʽ
	GPIO_Init(GPIOB, &GPIO_InitStructure);            	    //����PE6
	RESET_IO(1);                                            //��λIO���ߵ�ƽ
}
/*-------------------------------------------------*/
/*��������WiFi��������ָ��                          */
/*��  ����cmd��ָ��                                */
/*��  ����timeout����ʱʱ�䣨100ms�ı�����          */
/*����ֵ��0����ȷ   ����������                      */
/*-------------------------------------------------*/
u8 text[20];
char WiFi_SendCmd(char *cmd, int timeout)
{
	WiFi_RxCounter = 0;                           			//WiFi������������������                        
	memset(WiFi_RX_BUF, 0, WiFi_RXBUFF_SIZE);     			//���WiFi���ջ����� 
	WiFi_printf("%s\r\n", cmd);                  			//����ָ��
    
	while(timeout--)										//�ȴ���ʱʱ�䵽0
	{                           			
		DelayMs(100);                             		    //��ʱ100ms
		if(strstr(WiFi_RX_BUF, "OK"))              			//������յ�OK��ʾָ��ɹ�
			break;       									//��������whileѭ��
		u1_printf("%d ", timeout);                 			//����������ڵĳ�ʱʱ��
        printf("%s\r\n",WiFi_RX_BUF);
	}			
	u1_printf("\r\n");                          			//���������Ϣ
	if(timeout <= 0)return 1;                    				//���timeout<=0��˵����ʱʱ�䵽�ˣ�Ҳû���յ�OK������1
	else return 0;		         							//��֮����ʾ��ȷ��˵���յ�OK��ͨ��break��������while
}
/*-------------------------------------------------*/
/*��������WiFi��λ                                 */
/*��  ����timeout����ʱʱ�䣨100ms�ı�����          */
/*����ֵ��0����ȷ   ����������                      */
/*-------------------------------------------------*/
char WiFi_Reset(int timeout)
{
	RESET_IO(0);                                    	  //��λIO���͵�ƽ
	DelayMs(500);                                  		  //��ʱ500ms
	RESET_IO(1);                                   		  //��λIO���ߵ�ƽ	
    DelayMs(500);
//    if(WiFi_SendCmd("AT+RST\r\n",50))//����STAģʽ��100ms��ʱ��λ���ܼ�5s��ʱʱ��
//	{             
//		u1_printf("����ʧ�ܣ�׼������\r\n");   //���ط�0ֵ������if��������ʾ����
//		return 2;                                   //����2
//	}else u1_printf("���óɹ�\r\n");          //������ʾ����
//	while(timeout--)									  //�ȴ���ʱʱ�䵽0 
//	{                              		  
//		DelayMs(100);                                 	  //��ʱ100ms
//		if(strstr(WiFi_RX_BUF, "ready"))               	  //������յ�ready��ʾ��λ�ɹ�
//			break;       						   		  //��������whileѭ��
//		u1_printf("%d\r\n", timeout);                     	  //����������ڵĳ�ʱʱ��
//        u1_printf("%s\r\n",WiFi_RX_BUF);
//	}
	u1_printf("\r\n");                              	  //���������Ϣ
	if(timeout <= 0)return 1;                        		  //���timeout<=0��˵����ʱʱ�䵽�ˣ�Ҳû���յ�ready������1
	else return 0;		         				   		  //��֮����ʾ��ȷ��˵���յ�ready��ͨ��break��������while
}
/*-------------------------------------------------*/
/*��������WiFi����·����ָ��                       */
/*��  ����timeout����ʱʱ�䣨1s�ı�����            */
/*����ֵ��0����ȷ   ����������                     */
/*-------------------------------------------------*/


char WiFi_JoinAP(int timeout)
{		
	WiFi_RxCounter = 0;                                    //WiFi������������������                        
	memset(WiFi_RX_BUF, 0, WiFi_RXBUFF_SIZE);              //���WiFi���ջ����� 
	WiFi_printf("AT+CWJAP=\"%s\",\"%s\"\r\n", SSID, PASS); //����ָ��	
	while(timeout--)									   //�ȴ���ʱʱ�䵽0
	{                                   
		DelayMs(1000);                             		   //��ʱ1s
		if(strstr(WiFi_RX_BUF, "GOT IP"))   //������յ�WIFI GOT IP��ʾ�ɹ�
			break;       						           //��������whileѭ��
		u1_printf("%d ", timeout);                         //����������ڵĳ�ʱʱ��
	}
	u1_printf("\r\n%s\r\n", WiFi_RX_BUF);
	u1_printf("\r\n");                             	       //���������Ϣ
	if(timeout <= 0)return 1;                              //���timeout<=0��˵����ʱʱ�䵽�ˣ�Ҳû���յ�WIFI GOT IP������1
	return 0;                                              //��ȷ������0
}
/*-------------------------------------------------*/
/*������������TCP��������������͸��ģʽ            */
/*��  ����timeout�� ��ʱʱ�䣨100ms�ı�����        */
/*����ֵ��0����ȷ  ����������                      */
/*-------------------------------------------------*/
char WiFi_Connect_Server(int timeout)
{	
	WiFi_RxCounter=0;                              	//WiFi������������������                        
	memset(WiFi_RX_BUF,0,WiFi_RXBUFF_SIZE);         //���WiFi���ջ�����   
	WiFi_printf("AT+CIPSTART=\"TCP\",\"%s\",%d\r\n", ServerIP, ServerPort);//�������ӷ�����ָ��
	while(timeout--)								//�ȴ���ʱ���
	{                           
		DelayMs(100);                             	//��ʱ100ms	
		if(strstr(WiFi_RX_BUF, "CONNECT"))          //������ܵ�CONNECT��ʾ���ӳɹ�
			break;                                  //����whileѭ��
		if(strstr(WiFi_RX_BUF, "CLOSED"))           //������ܵ�CLOSED��ʾ������δ����
			return 1;                               //������δ��������1
		if(strstr(WiFi_RX_BUF, "ALREADY CONNECTED"))//������ܵ�ALREADY CONNECTED�Ѿ���������
			return 2;                               //�Ѿ��������ӷ���2
		u1_printf("%d ", timeout);                   //����������ڵĳ�ʱʱ��  
	}
	u1_printf("=========================\r\n");                              //���������Ϣ
	if(timeout <= 0)return 3;                       //��ʱ���󣬷���3
	else                                            //���ӳɹ���׼������͸��
	{
		u1_printf("���ӷ������ɹ���׼������͸��\r\n"); //������ʾ��Ϣ
		WiFi_RxCounter = 0;                          //WiFi������������������                        
		memset(WiFi_RX_BUF, 0, WiFi_RXBUFF_SIZE);    //���WiFi���ջ�����     
		WiFi_printf("AT+CIPSEND\r\n");               //���ͽ���͸��ָ��
		while(timeout--)							 //�ȴ���ʱ���
		{                            
			DelayMs(100);                            //��ʱ100ms	
//			if(strstr(WiFi_RX_BUF, "\r\nOK\r\n\r\n>"))//���������ʾ����͸���ɹ�
            if(strstr(WiFi_RX_BUF, "OK"))//���������ʾ����͸���ɹ�
				break;                          	 //����whileѭ��
            u1_printf("buf =%s\r\n",WiFi_RX_BUF);
			u1_printf("%d ", timeout);                //����������ڵĳ�ʱʱ��  
		}
		if(timeout <= 0)return 4;                      //͸����ʱ���󣬷���4	
	}
	return 0;	                                     //�ɹ�����0	
}
/*-------------------------------------------------*/
/*��������WiFi_Smartconfig                         */
/*��  ����timeout����ʱʱ�䣨1s�ı�����            */
/*����ֵ��0����ȷ   ����������                     */
/*-------------------------------------------------*/
char WiFi_Smartconfig(int timeout)
{
	
	WiFi_RxCounter=0;                           		//WiFi������������������                        
	memset(WiFi_RX_BUF,0,WiFi_RXBUFF_SIZE);     		//���WiFi���ջ�����     
	while(timeout--)									//�ȴ���ʱʱ�䵽0
	{                           		
		DelayMs(1000);                         				//��ʱ1s
		if(strstr(WiFi_RX_BUF, "connected"))    	 		  //������ڽ��ܵ�connected��ʾ�ɹ�
			break;                                  		//����whileѭ��  
		u1_printf("%d ", timeout);                 		//����������ڵĳ�ʱʱ��  
	}	
	u1_printf("\r\n");                          		//���������Ϣ
	if(timeout <= 0)return 1;                     		//��ʱ���󣬷���1
	return 0;                                   		//��ȷ����0
}
/*-------------------------------------------------*/
/*���������ȴ�����·����                           */
/*��  ����timeout����ʱʱ�䣨1s�ı�����            */
/*����ֵ��0����ȷ   ����������                     */
/*-------------------------------------------------*/
char WiFi_WaitAP(int timeout)
{		
	while(timeout--){                               //�ȴ���ʱʱ�䵽0
		DelayMs(1000);                             		//��ʱ1s
		if(strstr(WiFi_RX_BUF, "WIFI GOT IP"))         //������յ�WIFI GOT IP��ʾ�ɹ�
			break;       						   								  //��������whileѭ��
		u1_printf("%d ", timeout);                     //����������ڵĳ�ʱʱ��
	}
	u1_printf("\r\n");                              //���������Ϣ
	if(timeout <= 0)return 1;                         //���timeout<=0��˵����ʱʱ�䵽�ˣ�Ҳû���յ�WIFI GOT IP������1
	return 0;                                       //��ȷ������0
}
/*-------------------------------------------------*/
/*��������WiFiģ��esp8266���ӷ�����                */
/*��  ������                                       */
/*����ֵ��0����ȷ   ����������                     */
/*-------------------------------------------------*/
char WiFi_Connect_IoTServer(void)
{	

    
    
    
	u1_printf("׼����λģ��\r\n");                   //������ʾ����
	if(WiFi_Reset(50))								//��λ��100ms��ʱ��λ���ܼ�5s��ʱʱ��
	{                             
		u1_printf("��λʧ�ܣ�׼������\r\n");	      //���ط�0ֵ������if��������ʾ����
		return 1;                                   //����1
	}else u1_printf("��λ�ɹ�\r\n");                 //������ʾ����
	   OLED_CLS();//����
		OLED_ShowStr(35,1,(uint8_t *)"ESP8266",2);
    OLED_ShowCN(33,3,7);OLED_ShowCN(49,3,8);OLED_ShowCN(65,3,9);OLED_ShowCN(81,3,10);//oled��ʾ��ESP8266��ʼ���С�
	for(i=0;i<10;i++){OLED_progress_bar(6,i);}//��ʾ������
	u1_printf("׼������STAģʽ\r\n");                //������ʾ����
	DelayMs(500);
	if(WiFi_SendCmd("AT+CWMODE=1",50))//����STAģʽ��100ms��ʱ��λ���ܼ�5s��ʱʱ��
	{             
		u1_printf("����STAģʽʧ�ܣ�׼������\r\n");   //���ط�0ֵ������if��������ʾ����
		return 2;                                   //����2
	}else u1_printf("����STAģʽ�ɹ�\r\n");          //������ʾ����
	for(i=10;i<20;i++){OLED_progress_bar(6,i);}//��ʾ������
	DelayMs(500);
	if(wifi_mode==0) //�������ģʽ=0��SSID������д�ڳ����� 
	{                              
		u1_printf("׼��ȡ���Զ�����\r\n");            //������ʾ����
		if(WiFi_SendCmd("AT+CWAUTOCONN=0",50))		 //ȡ���Զ����ӣ�100ms��ʱ��λ���ܼ�5s��ʱʱ��
		{       
			u1_printf("ȡ���Զ�����ʧ�ܣ�׼������\r\n"); //���ط�0ֵ������if��������ʾ����
			return 3;                                  //����3
		}else u1_printf("ȡ���Զ����ӳɹ�\r\n");        //������ʾ����
				
		u1_printf("׼������·����\r\n");                //������ʾ����	
		if(WiFi_JoinAP(30))//����·����,1s��ʱ��λ���ܼ�30s��ʱʱ��
		{                          
			u1_printf("����·����ʧ�ܣ�׼������\r\n");  //���ط�0ֵ������if��������ʾ����
			return 4;                                 //����4	
		}else u1_printf("����·�����ɹ�\r\n");         //������ʾ����	
   for(i=20;i<30;i++){OLED_progress_bar(6,i);}//��ʾ������	
    DelayMs(500);	 
	}
	
	u1_printf("׼������͸��\r\n");                    //������ʾ����
	if(WiFi_SendCmd("AT+CIPMODE=1",50)) 			 //����͸����100ms��ʱ��λ���ܼ�5s��ʱʱ��
	{           
		u1_printf("����͸��ʧ�ܣ�׼������\r\n");       //���ط�0ֵ������if��������ʾ����
		return 8;                                    //����8
	}else u1_printf("����͸���ɹ�\r\n");              //������ʾ����
	for(i=30;i<51;i++){OLED_progress_bar(6,i);}//��ʾ������
	  DelayMs(500);
	u1_printf("׼���رն�·����\r\n");                //������ʾ����
	if(WiFi_SendCmd("AT+CIPMUX=0",50)) 				 //�رն�·���ӣ�100ms��ʱ��λ���ܼ�5s��ʱʱ��
	{            
		u1_printf("�رն�·����ʧ�ܣ�׼������\r\n");   //���ط�0ֵ������if��������ʾ����
		return 9;                                    //����9
	}else u1_printf("�رն�·���ӳɹ�\r\n");          //������ʾ����
	 
	 OLED_ShowCN(24,3,7);OLED_ShowCN(40,3,8);OLED_ShowCN(56,3,9);OLED_ShowCN(72,3,11);OLED_ShowCN(88,3,12);//oled��ʾ��ESP8266��ʼ����ɡ�
	  DelayMs(500);
	u1_printf("׼�����ӷ�����\r\n");                  //������ʾ����
	OLED_CLS();//OLED����
  OLED_ShowCN(15,2,15);OLED_ShowCN(31,2,16);OLED_ShowCN(47,2,17);OLED_ShowCN(63,2,13);OLED_ShowCN(79,2,14);OLED_ShowCN(95,2,10);//oled��ʾ�������������С�
	if(WiFi_Connect_Server(100))      				 //���ӷ�������100ms��ʱ��λ���ܼ�10s��ʱʱ��
	{            
		u1_printf("���ӷ�����ʧ�ܣ�׼������\r\n");     //���ط�0ֵ������if��������ʾ����
		OLED_ShowCN(47,4,26);OLED_ShowCN(63,4,27);OLED_ShowCN(47,6,28);OLED_ShowCN(63,6,29);//����ʧ�ܣ�����
    DelayMs(500);		
		return 10;                                   //����10
	}else u1_printf("���ӷ������ɹ�\r\n");            //������ʾ����	
	 OLED_CLS();//OLED����
	 OLED_ShowCN(31,4,13);OLED_ShowCN(47,4,14);OLED_ShowCN(63,4,12);OLED_ShowCN(79,4,18);//oled��ʾ�����ӳɹ���
	 OLED_CLS();//OLED����
	return 0;                                        //��ȷ����0
}
	




