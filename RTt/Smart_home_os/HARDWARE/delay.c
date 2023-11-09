#include "delay.h"



#ifdef  time
int year_time = 0;
int moon_time = 0;
int sun_time = 0;
int hour_time = 0;
int minute_time = 0;
int second_time = 0;

uint8_t	time_flag = 0;


void time_init(int year, int moon , int sun, int hour, int minute, int second)
{

    year_time = year;  // ��
    moon_time = moon;  // ��
    sun_time = sun;    // ��
    hour_time = hour;  // Сʱ
    minute_time = minute;  // ����
    second_time = second;  // ��
    time_flag = 1;
    printf("ʱ�����ݳ�ʼ���ɹ�\r\n");
}


void time_set(void)
{
    second_time++;
    if(second_time > 60)
    {
        minute_time++;
        second_time = 0;
        if(minute_time >= 60)  // ����
        {
            hour_time++;
            minute_time = 0;
            if(hour_time > 23)
            {
                sun_time++;
                hour_time = 0;
                if(sun_time > 31)
                {
                    moon_time++;
                    sun_time = 1;
                    year_time++;
                }
            }
        }
    }
    printf("%d��%d��%d��%dʱ%d��%d��\r\n", year_time, moon_time, sun_time, hour_time, minute_time, second_time);
    //	lcd_show_string()
}

#endif

static u32 fac_us = 0;							//us��ʱ������
void delay_init(u8 SYSCLK)
{
#if SYSTEM_SUPPORT_OS 						//�����Ҫ֧��OS.
    u32 reload;
#endif
    HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);//SysTickƵ��ΪHCLK
    fac_us = SYSCLK;						//�����Ƿ�ʹ��OS,fac_us����Ҫʹ��
#if SYSTEM_SUPPORT_OS 						//�����Ҫ֧��OS.
    reload = SYSCLK;					  //ÿ���ӵļ������� ��λΪK
    reload *= 1000000 / delay_ostickspersec;	//����delay_ostickspersec�趨���ʱ��
    //reloadΪ24λ�Ĵ���,���ֵ:16777216,��180M��,Լ��0.745s����
    fac_ms = 1000 / delay_ostickspersec;		//����OS������ʱ�����ٵ�λ
    SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk; //����SYSTICK�ж�
    SysTick->LOAD = reload; 					//ÿ1/OS_TICKS_PER_SEC���ж�һ��
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk; //����SYSTICK
#else
#endif
}

void delay_us(u32 nus)
{
    u32 ticks;
    u32 told, tnow, tcnt = 0;
    u32 reload = SysTick->LOAD;				//LOAD��ֵ
    ticks = nus * fac_us; 						//��Ҫ�Ľ�����
    told = SysTick->VAL;        				//�ս���ʱ�ļ�����ֵ
    while(1)
    {
        tnow = SysTick->VAL;
        if(tnow != told)
        {
            if(tnow < told)tcnt += told - tnow;	//����ע��һ��SYSTICK��һ���ݼ��ļ������Ϳ�����.
            else tcnt += reload - tnow + told;
            told = tnow;
            if(tcnt >= ticks)break;			//ʱ�䳬��/����Ҫ�ӳٵ�ʱ��,���˳�.
        }
    };
}

//��ʱnms
//nms:Ҫ��ʱ��ms��
void delay_ms(u16 nms)
{
	u32 i;
	for(i=0;i<nms;i++) delay_us(1000);
}

//void delay_us(__IO uint32_t delay) {
//    uint32_t us_delay = delay * (HAL_RCC_GetHCLKFreq() / 1000000);
//
//    // ʹ��DWT�Ĵ���������΢�뼶�����ʱ
//    uint32_t start = DWT->CYCCNT;
//    while ((DWT->CYCCNT - start) < us_delay);
//}

//void delay_us(__IO uint32_t delay)
//{
//    int last, curr, val;
//    int temp;

//    while (delay != 0)
//    {
//        temp = delay > 900 ? 900 : delay;
//        last = SysTick->VAL;
//        curr = last - CPU_FREQUENCY_MHZ * temp;
//        if (curr >= 0)
//        {
//            do
//            {
//                val = SysTick->VAL;
//            }
//            while ((val < last) && (val >= curr));
//        }
//        else
//        {
//            curr += CPU_FREQUENCY_MHZ * 1000;
//            do
//            {
//                val = SysTick->VAL;
//            }
//            while ((val <= last) || (val > curr));
//        }
//        delay -= temp;
//    }
//}



