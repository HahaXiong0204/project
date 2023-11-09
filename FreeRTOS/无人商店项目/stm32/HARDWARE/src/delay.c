#include "delay.h"

#include "main.h"

#define USE_HAL_LEGACY
#include "stm32_hal_legacy.h"

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





//#define Timebase_Source_is_SysTick 1	//��Timebase SourceΪSysTickʱ��Ϊ1
#define Timebase_Source_is_SysTick 0	//��ʹ��FreeRTOS��Timebase SourceΪ������ʱ��ʱ��Ϊ0

#if	(!Timebase_Source_is_SysTick)
	extern TIM_HandleTypeDef htim14;		//��ʹ��FreeRTOS��Timebase SourceΪ������ʱ��ʱ���޸�Ϊ��Ӧ�Ķ�ʱ��
	#define Timebase_htim htim14

	#define Delay_GetCounter()		__HAL_TIM_GetCounter(&Timebase_htim)
	#define Delay_GetAutoreload()	__HAL_TIM_GetAutoreload(&Timebase_htim)
#else
	#define Delay_GetCounter()		(SysTick->VAL)
	#define Delay_GetAutoreload()	(SysTick->LOAD)
#endif


static uint16_t fac_us = 0;
static uint32_t fac_ms = 0;

/*��ʼ��*/
void delay_init(void)
{
#if	(!Timebase_Source_is_SysTick)
	fac_ms = 1000000;				//��Ϊʱ���ļ�����ʱ��Ƶ����HAL_InitTick()�б���Ϊ��1MHz
	fac_us = fac_ms / 1000;
#else
	fac_ms = SystemCoreClock / 1000;
	fac_us = fac_ms / 1000;
#endif
}

/*΢�뼶��ʱ*/
void delay_us(uint32_t nus)
{
	uint32_t ticks = 0;
	uint32_t told = 0;
	uint32_t tnow = 0;
	uint32_t tcnt = 0;
	uint32_t reload = 0;

	reload = Delay_GetAutoreload();

	ticks = nus * fac_us;

	told = Delay_GetCounter();

	while (1)
	{
		tnow = Delay_GetCounter();

		if (tnow != told)
		{
			if (tnow < told)
			{
				tcnt += told - tnow;
			}
			else
			{
				tcnt += reload - tnow + told;
			}
			told = tnow;
			if (tcnt >= ticks)
			{
				break;
			}
		}
	}
}

/*���뼶��ʱ*/
void delay_ms(uint32_t nms)
{
	uint32_t ticks = 0;
	uint32_t told = 0;
	uint32_t tnow = 0;
	uint32_t tcnt = 0;
	uint32_t reload = 0;

	reload = Delay_GetAutoreload();

	ticks = nms * fac_ms;

	told = Delay_GetCounter();

	while (1)
	{
		tnow = Delay_GetCounter();

		if (tnow != told)
		{
			if (tnow < told)
			{
				tcnt += told - tnow;
			}
			else
			{
				tcnt += reload - tnow + told;
			}
			told = tnow;
			if (tcnt >= ticks)
			{
				break;
			}
		}
	}
}

/*��дHAL_Delay*/
void HAL_Delay(uint32_t Delay)
{
  uint32_t tickstart = HAL_GetTick();
  uint32_t wait = Delay;

  /*��̫���׹ٷ�Դ��Ϊɶ��ôд�������ʱ1ms��ע�͵����׼*/
//  /* Add a freq to guarantee minimum wait */
//  if (wait < HAL_MAX_DELAY)
//  {
//    wait += (uint32_t)(uwTickFreq);
//  }

  while ((HAL_GetTick() - tickstart) < wait)
  {
  }
}



