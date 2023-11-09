/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "sys.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

void water_level_machiery(void);

void lcd_show_water(void);

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
extern uint16_t pwm_mach;     // 电机pwm

extern uint8_t water_flag;   // 0 复位  1 暂停   2 正常
extern uint8_t Wh;	// 高水位
extern uint8_t user_start_end;
extern uint8_t user_start_end;
extern uint16_t ID_Cart;

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
extern uint16_t pwm_mach;     // 电机pwm

extern uint8_t water_flag;   // 0 复位  1 暂停   2 正常
extern uint8_t Wh;	// 高水位
extern uint16_t MAX_Water_flow_all;
extern uint16_t temp_water;

extern uint8_t beep_flag;    // 1 warning1   2 Error1
extern uint8_t beep_num;
/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

#define DERVICE_ID		2001

extern char show_buff[124];

extern uint16_t pwm_mach;     // 电机pwm

extern uint8_t water_flag;   // 0 复位  1 暂停   2 正常
extern uint8_t Wh;	// 高水位
extern uint8_t Wl;	// 低水位/* USER CODE END EC */
extern uint8_t Wc;    // 当前水位
extern uint8_t machiery_Join_W_flag;   // 0 不进水 1进水

extern uint8_t TH ;   // 高水温
extern double Tc;    // 当前水温
extern uint8_t temp_water_flag;  // 是否烧水 1 烧

extern uint8_t water_key_flag;   //出水 0 关闭  1 热水 2 温水

extern uint16_t water_intake;  // 饮水量
extern uint8_t water_flow;    // 流量


/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define RED_SCAN_Pin GPIO_PIN_2
#define RED_SCAN_GPIO_Port GPIOE
#define bom_red_Pin GPIO_PIN_4
#define bom_red_GPIO_Port GPIOE
#define bom_red_EXTI_IRQn EXTI4_IRQn
#define bom_yellow_Pin GPIO_PIN_5
#define bom_yellow_GPIO_Port GPIOE
#define bom_yellow_EXTI_IRQn EXTI9_5_IRQn
#define bom_green_Pin GPIO_PIN_6
#define bom_green_GPIO_Port GPIOE
#define bom_green_EXTI_IRQn EXTI9_5_IRQn
#define KEY2_Pin GPIO_PIN_13
#define KEY2_GPIO_Port GPIOC
#define KEY2_EXTI_IRQn EXTI15_10_IRQn
#define WK_UP_Pin GPIO_PIN_0
#define WK_UP_GPIO_Port GPIOA
#define WK_UP_EXTI_IRQn EXTI0_IRQn
#define KEY1_Pin GPIO_PIN_2
#define KEY1_GPIO_Port GPIOH
#define KEY1_EXTI_IRQn EXTI2_IRQn
#define KEY0_Pin GPIO_PIN_3
#define KEY0_GPIO_Port GPIOH
#define KEY0_EXTI_IRQn EXTI3_IRQn
#define input_machi_Pin GPIO_PIN_3
#define input_machi_GPIO_Port GPIOA
#define LED0_Pin GPIO_PIN_0
#define LED0_GPIO_Port GPIOB
#define LED1_Pin GPIO_PIN_1
#define LED1_GPIO_Port GPIOB
#define PWM_machinery_Pin GPIO_PIN_10
#define PWM_machinery_GPIO_Port GPIOH
#define DHT11_Pin GPIO_PIN_14
#define DHT11_GPIO_Port GPIOG
#define RC522_SDA_Pin GPIO_PIN_4
#define RC522_SDA_GPIO_Port GPIOI
#define RC522_RST_Pin GPIO_PIN_5
#define RC522_RST_GPIO_Port GPIOI

/* USER CODE BEGIN Private defines */
    extern IWDG_HandleTypeDef hiwdg;
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
