#ifndef _DELAY_H

#include "main.h"

#include "lcd.h"

#define time  1

#ifdef time
extern uint8_t	time_flag;
extern uint16_t year_time;
extern uint8_t moon_time;
extern uint8_t sun_time;
extern uint8_t hour_time;   // 小时
extern uint8_t minute_time;  // 分钟
extern uint8_t second_time;  // 秒

void time_set(void);
void time_init(uint16_t year, uint8_t moon , uint8_t sun, uint8_t hour, uint8_t minute, uint8_t second);
#endif

#define delay_ms    HAL_Delay

//#define CPU_FREQUENCY_MHZ    180		// STM32时钟主频
//void delay_us(uint32_t udelay);


void delay_init(u8 SYSCLK);
//void delay_ms(u16 nms);
void delay_us(u32 nus);

#endif
