#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "lcd.h"
#include "sdram.h"
#include "timer.h"
/************************************************
 ALIENTEK ������STM32F429������ʵ��14
 LTDC LCDʵ��-HAL�⺯����
 ����֧�֣�www.openedv.com
 �Ա����̣�http://eboard.taobao.com  
 ��ע΢�Ź���ƽ̨΢�źţ�"����ԭ��"����ѻ�ȡSTM32���ϡ�
 ������������ӿƼ����޹�˾  
 ���ߣ�����ԭ�� @ALIENTEK
************************************************/

uint8_t mode = 0,wei=0;
uint8_t buff[124];
u8 a = 1,b = 1,c = 1;
void show()
{
    if(mode == 1){
        LCD_ShowString(180,50,200,16,16,"square wave"); 
    }
    else{
        LCD_ShowString(180,50,200,16,16,"triangular wave");
    }
    if(wei == 0){
        LCD_ShowString(180,90,200,16,16,"a");
    }else if(wei == 1){
        LCD_ShowString(180,90,200,16,16,"b");
    }else{
        LCD_ShowString(180,90,200,16,16,"c");
    }
    
    


}


void LCD_cl()
{
    LCD_Fill(0,300,480,800,WHITE);

}
int main(void)
{

    u8 dir=1;
    u8 fan_bo = 0;
    u16 pw,x_temp;
    uint8_t key = 0;

    u8 i=1;
    u16 x = 0,y=400;
    u16 tempx;
    u16 led0pwmval=0;  
    HAL_Init();                     //��ʼ��HAL��   
    Stm32_Clock_Init(360,25,2,8);   //����ʱ��,180Mhz
    delay_init(180);                //��ʼ����ʱ����
    uart_init(115200);              //��ʼ��USART
    LED_Init();  
    KEY_Init();
    SDRAM_Init();                   //SDRAM��ʼ��
    TIM3_PWM_Init(1000-1,45-1);      //90M/90=1M�ļ���Ƶ�ʣ��Զ���װ��Ϊ500����ôPWMƵ��Ϊ1M/500=2kHZ
    LCD_Init();                     //LCD��ʼ��
    mode = 0;
    while(1)
    {
        show();
        pw = a*100+b*10+c;
        sprintf(buff,"frequency:%d%d%d",a,b,c);

        LCD_ShowString(180,70,200,16,16,buff);
        

        
        key = KEY_Scan(0);
        if(key==KEY0_PRES)
        {
            wei++;
            if(wei>2)wei =0;

        }else if(key == KEY1_PRES)
        {
            if(wei == 0){
                if(a==0) a=9;
                else a--;
                
            }else if(wei == 1){
                if(b==0) b=9;
                else b--;
            }else{
                if(c==0) c=9;
                else c--;
            }
        }
        else if(key == WKUP_PRES)
        {
            if(wei == 0){
                if(a==9) a=0;
                else a++;
                
            }else if(wei == 1){
                if(b==9) b=0;
                else b++;
            }else{
                if(c==9) c=0;
                else c++;
            }
        }
        else if(key == KEY2_PRES)
        {
            LCD_cl();
            if(mode == 0)mode =1;
            else mode = 0;
        
        }
        
//        LCD_Fast_DrawPoint(x++,300+led0pwmval,RED);
        if(x>480){
            x = 0;
//            LCD_Clear(WHITE);
            LCD_cl();
            tempx = 0;
        }


		delay_ms(10);	 
        if(mode == 0)
        {
            
            
            if(dir)
            {
                led0pwmval+=i;				//dir==1 led0pwmval����
 
                if(led0pwmval>pw/10)
                {
                    
                    x_temp = (1000 - pw)/10;
                    LCD_DrawLine(x,y,x+x_temp,y+100);
                    dir=0;
                    x += x_temp;
                    delay_ms(200);
                }
                
            }
            else{
                led0pwmval -=i;
                if(led0pwmval<=0)
                {
                    x_temp = (1000 - pw)/10;
                    LCD_DrawLine(x,y+100,x+x_temp,y);
                    x+=x_temp;
                    delay_ms(200);
                    dir = 1;
                }
            }
            TIM_SetTIM3Compare4(led0pwmval);	//�޸ıȽ�ֵ���޸�ռ�ձ�

        }
        else if(mode == 1)
        {
            if(fan_bo==0)
            {
                TIM_SetTIM3Compare4(pw);	//�޸ıȽ�ֵ���޸�ռ�ձ�
                x_temp =  110 - pw/10;
                LCD_DrawLine(x,y,x+x_temp,y);
                LCD_DrawLine(x+x_temp,y,x+x_temp,y+100);
                x += x_temp;
                delay_ms(200);
                fan_bo = 1;
            }
            else{
                TIM_SetTIM3Compare4(0);	//�޸ıȽ�ֵ���޸�ռ�ձ�
                x_temp = 110 - pw/10;
                LCD_DrawLine(x,y+100,x+x_temp,y+100);
                LCD_DrawLine(x+x_temp,y+100,x+x_temp,y);
                x += x_temp;
                delay_ms(200);
                fan_bo = 0;
            }
            

        }
		
    }
}
