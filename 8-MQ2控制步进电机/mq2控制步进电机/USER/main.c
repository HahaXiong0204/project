//=============================================================================
//�ļ����ƣ�main.h
//���ܸ�Ҫ��STM32F103C8���ļ��
//��Ȩ���У�Դ�ع�����www.vcc-gnd.com
//��Ȩ���£�2013-02-20
//���Է�ʽ��J-Link OB ARM SW��ʽ 5MHz
//=============================================================================

//ͷ�ļ�
#include "stm32f10x.h"
#include "GPIOLIKE51.h"

#include "delay.h"
#include "sys.h"
#include "usart.h"

#include "oled.h"

//����Э���
#include "onenet.h"
//�����豸
#include "esp8266.h"

#include "stdio.h"
#include "string.h"
#include "adc.h"
//��������
void GPIO_Configuration(void);


u16 ADC_Value[10];
extern uint8_t net_connect;

u8 buf1[10]={0};

u16 i;   //forѭ���õ��ı�������
u16 fire_value;
u16 timeCount = 0;	//���ͼ������
float value1 = 0,Gas;
float temp1;
u16 n=0;
u8 flag = 2;    // 2 Ϊ����  1 Ϊ����
u8 mode = 1;   // 1 Ϊ�Զ�  2 Ϊ�ֶ�
u8 th_Gas = 0 ;
u8 num = 0;
//=============================================================================
//�ļ����ƣ�Delay
//���ܸ�Ҫ����ʱ
//����˵����nCount����ʱ����
//�������أ���
//=============================================================================

void Delay(uint32_t nCount)
{
  for(; nCount != 0; nCount--);
}


//========================================
u8 phasecw[8] ={0x08,0x0c,0x04,0x06,0x02,0x03,0x01,0x09};// ��ʱ��
//1000,1100,0100,0110,0010,0011,0001,1001
u8 phaseccw[8]={0x09,0x01,0x03,0x02,0x06,0x04,0x0c,0x08};// ˳ʱ��
//1001,0001,0011,0010,0110,0100,1100,1000

//���ų�ʼ��
void Step_Motor_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
    GPIO_Init(GPIOA,&GPIO_InitStructure);   
}
//����ӳ��
void SetMotor(unsigned char InputData)//���ݴ��͵�����Inputdata���������������
{//���ݴ��͵�����Inputdata����PB5-PB8�������
    if(InputData&0x01)
    {
        GPIO_SetBits(GPIOA,GPIO_Pin_5);
    }
    else
    {
        GPIO_ResetBits(GPIOA,GPIO_Pin_5);
    }
    if(InputData&0x02)
    {
        GPIO_SetBits(GPIOA,GPIO_Pin_6);
    }
    else
    {
        GPIO_ResetBits(GPIOA,GPIO_Pin_6);

    }
    if(InputData&0x04)
    {
        GPIO_SetBits(GPIOA,GPIO_Pin_7);
    }
    else
    {
        GPIO_ResetBits(GPIOA,GPIO_Pin_7);
    }
    if(InputData&0x08)
    {
        GPIO_SetBits(GPIOA,GPIO_Pin_4);
    }
    else
    {
        GPIO_ResetBits(GPIOA,GPIO_Pin_4);
    }
}
//�����5.625 360/5.625=64 ���ٱ�1/64 
//��64*64������תһȦ
//nȦ�� 
//position ����
void motorNcircle(int n,u8 position)//nΪ�������ת����Ȧ����positionΪ��ת���߷�ת
{
    int i,j,k=0;
    for(j=0;j<n;j++)
    {
       for(i=0;i<64*8;i++)
       // for(i=0;i<8;i++)
        {//4�൥˫8��
					for(k=0;k<8;k++)
            {
                if(1 == position)
                {
                    SetMotor(phasecw[k]);
                }
                else
                {
                    SetMotor(phaseccw[k]);
                }
                delay_ms(1);//��ʱ���ٶȿ��ƣ���ֵԽ���ٶ�Խ��������0.65A
            }
        }
    }
}






//=================================
//=============================================================================
//�ļ����ƣ�main
//���ܸ�Ҫ��������
//����˵������
//�������أ�int
//=============================================================================
int main(void)
{
    unsigned char *dataPtr = NULL;

    const char *topics[] = {"two","$creq"};
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�����ж����ȼ�����
	delay_init();			          //��ʱ������ʼ��
    Usart1_Init(115200);							//����1����ӡ��Ϣ��
	Usart2_Init(115200);
    ADCx_Init();

	OLED_Init(); 	              //OLED��ʼ��  
    show();
    Step_Motor_GPIO_Init();     // ���������ʼ��

    th_Gas = 15;
	                     //OLED��ʾ������
	GPIO_Configuration();
//    PBout(9)=1;
//    delay_ms(10);
//    PBout(9)=0;
//    
//    
    
//    delay_ms(10);
    PCout(13)=1;
    while(1)
    {
        if(net_connect == 1)
        {
            timeCount ++;
            
            if(mode == 1)   // �Զ�
            {
                OLED_ShowStr(32,6,":",2);OLED_ShowCN(42,6,73); OLED_ShowCN(58,6,74); 
            }
            else
            {
                OLED_ShowStr(32,6,":",2);OLED_ShowCN(42,6,75); OLED_ShowCN(58,6,74);
            }
            
            if(flag == 1)
            {
                OLED_ShowCN(88,4,76); OLED_ShowCN(104,4,78); // ����
            }
            else
            {
                 OLED_ShowCN(88,4,77); OLED_ShowCN(104,4,78); // ����
            }
//=======================�����գ��������к����壩�������������ݵĻ�ȡ��ͨ��ADC����ͨ����ѯ��ȡ========	
            if(timeCount%200 == 0)
            {   
                for(i=0,value1=0;i<40;)
                {
                    value1+=ADC_Value[i++];	
                }	
                
                Gas = ((((float)value1/10) / 4096.0f * 5.0f) - 0.4f)/ 0.4f+5;
                memset(buf1,0,sizeof(buf1));
                sprintf((char *)buf1,":%.2f%%",Gas);
                OLED_ShowStr(32,0,buf1,2);//OLED��ʾ����
                
                memset(buf1,0,sizeof(buf1));
                sprintf((char *)buf1,":%d",th_Gas);
                OLED_ShowStr(64,2,buf1,2);//OLED��ʾ����

                value1=0;
                      // ��ֵ�ж�
                if(mode==1)
                {
                    if(th_Gas<Gas&&num != 1)
                    {
                        PCout(13)=0;    // ����
                        PBout(9)=0;
                        OLED_ShowStr(32,4,":",2); OLED_ShowCN(42,4,66); OLED_ShowCN(58,4,68); // �����ת
                        motorNcircle(1,1);//nΪ�������ת����Ȧ����positionΪ��ת���߷�ת
                        flag = 1;    // ����
                        OLED_ShowStr(32,4,":",2); OLED_ShowCN(42,4,79); OLED_ShowCN(58,4,80); // ���ֹͣ
                        num = 1;   // ��תһ��
                        
                    }else if(th_Gas>=Gas&&num !=2)
                    {
                        
                        PCout(13) =  1;  // �ص�
                        PBout(9)=1;
                        OLED_ShowStr(32,4,":",2); OLED_ShowCN(42,4,67); OLED_ShowCN(58,4,68); // �����ת
                        motorNcircle(1,0);//nΪ�������ת����Ȧ����positionΪ��ת���߷�ת
                        OLED_ShowStr(32,4,":",2); OLED_ShowCN(42,4,79); OLED_ShowCN(58,4,80); // ���ֹͣ
                        flag = 2;   // ����
                        num = 2;   // ��һ��
                    }
                }


            }

            
            
            
            if(timeCount > 400)	//���ͼ��3S
            {	
                timeCount = 0;
                OneNet_SendData();	//��������
                ESP8266_Clear();    //��ջ���
//                printf("mode = %d   th_temp = %d  th_hum = %d  th_Mq135= %d   th_mq2 = %d   th_fire= %d  th_Lighting = %d \r\n",mode,th_temp,th_humidity,th_MQ135,th_MQ2,th_fire,th_Lighting);
            }
//            
////==============================================����OneNet���������������ݣ����Բ��ùܣ�===============================			
            dataPtr = ESP8266_GetIPD(0);
            if(dataPtr != NULL)
            {
                OneNet_RevPro(dataPtr);
            }
        }
        else
        {
            printf("��ʼ���ӣ�����\r\n");
            ESP8266_Init();					//��ʼ��ESP8266
	
            while(OneNet_DevLink())			//����OneNET
                delay_ms(500);
            
            OneNet_Subscribe(topics, 1);
            net_connect = 1;
        
        }
    
    
    }
    

}

//=============================================================================
//�ļ����ƣ�GPIO_Configuration
//���ܸ�Ҫ��GPIO��ʼ��
//����˵������
//�������أ���
//=============================================================================
void GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC , ENABLE); 
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB , ENABLE);     
//=============================================================================
//LED -> PC13    BEEP ->PB9
//=============================================================================			 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 

    GPIO_Init(GPIOC, &GPIO_InitStructure);
}




