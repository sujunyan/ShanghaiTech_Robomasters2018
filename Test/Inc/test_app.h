/**
  *@file test_app.h
  *@date 2016-12-13
  *@author Albert.D
  *@brief 
  */
  
#ifndef _TEST__APP_H
#define _TEST__APP_H

#include "stm32f4xx_HAL.h"

typedef struct 
{
  uint8_t canTest;
  uint8_t uartTest;
  uint8_t imuTest;
  uint8_t usbTest;
  uint8_t keyTest;
  uint8_t allTest;
  
}Test_ResultTypedef;

extern Test_ResultTypedef sTestResult;

#define LED_Red_On()        HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7, GPIO_PIN_RESET)
#define LED_Red_Off()       HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7, GPIO_PIN_SET)
#define LED_Red_Toggle()    HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_7)
#define LED_Green_On()      HAL_GPIO_WritePin(GPIOF, GPIO_PIN_14, GPIO_PIN_RESET)
#define LED_Green_Off()     HAL_GPIO_WritePin(GPIOF, GPIO_PIN_14, GPIO_PIN_SET)
#define LED_Green_Toggle()  HAL_GPIO_TogglePin(GPIOF, GPIO_PIN_14)

#define KEY_PRESS          (HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_10))?0x00:0x01

void Test_task(void);

#endif

