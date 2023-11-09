#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "oled.h"
#include "usart.h"
#include "dht11.h"
#include "esp8266.h"
#include "AD.h"
#include "iwdg.h"


//����Э���
#include "onenet.h"

//�����豸
#include "esp8266.h"
//c��
#include "string.h"

u16 value0;
u16 value1;
u16 value2;
u16 value3;
u16 value4;
u16 value5;

u8 buf1[10] = {0};
u8 buf2[10] = {0};
u8 buf3[10] = {0};
u8 buf4[10] = {0};
u8 buf5[10] = {0};
u8 buf6[10] = {0};
int i;

extern u8  flag;
int net_connect = 0;


int main(void)
{

    unsigned char *dataPtr = NULL;

    unsigned short timeCount = 0;	//���ͼ������

    delay_init();	    	 //��ʱ������ʼ��
    NVIC_Configuration(); 	 //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
    AD_Init();            //��ʼ��ADC
    OLED_Init();			//��ʼ��OLED
    OLED_Clear();
    relay_init();    //�̵���
    LED_Init();
    uart_init(115200);//����1��ʼ��
    uart2_init(115200);//����2��ʼ��
    IWDG_Init(4, 625*5);	//���ʱ��Ϊ5s  (4*2^4)*625/40=1000ms

    
    



    OLED_Clear();//����



    while(1)
    {
        
        if(net_connect == 1)
        {
            
//            for(i = 0, value0 = 0, value1 = 0, value2 = 0, value3 = 0, value4 = 0, value5 = 0; i < 60;)
//            {
//                value0 += AD_Value[i++];
//                value1 += AD_Value[i++];
//                value2 += AD_Value[i++];
//                value3 += AD_Value[i++];
//                value4 += AD_Value[i++];
//                value5 += AD_Value[i++];
//            }

//            sprintf((char *)buf1, "%d", value0 / 10);
//            sprintf((char *)buf2, "%d", value1 / 10);
//            sprintf((char *)buf3, "%d", value2 / 10);


//            sprintf((char *)buf4, "%d", value3 / 10);
//            sprintf((char *)buf5, "%d", value4 / 10);
//            sprintf((char *)buf6, "%d", value5 / 10);

//            OLED_ShowString(28, 0, buf1);
//            OLED_ShowString(28, 2, buf2);
//            OLED_ShowString(28, 4, buf3);

//            OLED_ShowString(95, 0, buf4);
//            OLED_ShowString(95, 2, buf5);
//            OLED_ShowString(95, 4, buf6);

//            delay_ms(100);

//            value0 = 0, value1 = 0, value2 = 0, value3 = 0, value4 = 0, value5 = 0;
//            
            

                OLED_ShowString(28, 0, "sdwd");
            if(++timeCount >= 100)		//ʱ����5s ����
            {
                delay_ms(100);
                printf( "��ʼ��������\r\n");//ͨ������1������ʾ��Ϣ��Ҫ��ʼ���������ˣ�
                OneNet_SendData();//�������ݸ�onenet
                printf("�������ݽ���\n");
                timeCount = 0;
                ESP8266_Clear();
            }

            dataPtr = ESP8266_GetIPD(0);//��ȡƽ̨���ص�����
            if(dataPtr != NULL)//����������ݲ�Ϊ��
                OneNet_RevPro(dataPtr);//ƽ̨�������ݼ��

            delay_ms(10);
        }
        else
        {
            ESP8266_Init();					//��ʼ��ESP8266

            printf("8266��ʼ���ɹ�\n");


            while(OneNet_DevLink())			//����OneNET

            OLED_ShowCHinese(0, 3, 9); //������
            OLED_ShowCHinese(18, 3, 10);
            OLED_ShowCHinese(36, 3, 11);
            OLED_ShowCHinese(54, 3, 12);
            OLED_ShowCHinese(72, 3, 13);

            delay_ms(500);
            net_connect = 1;

            printf("����onenet�ɹ�");
        }
        
        

    }

}
