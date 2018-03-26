#ifndef __SERIAL_DEBUG_H__
#define __SERIAL_DEBUG_H__
//#define DEBUGSWD


#define SERIAL_DEBUG

#include "usart.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#define LED_R_ON        HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7, GPIO_PIN_RESET)
#define LED_R_OFF       HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7, GPIO_PIN_SET)
#define LED_R_TOGGLE   HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_7)
#define LED_G_ON      HAL_GPIO_WritePin(GPIOF, GPIO_PIN_14, GPIO_PIN_RESET)
#define LED_G_OFF     HAL_GPIO_WritePin(GPIOF, GPIO_PIN_14, GPIO_PIN_SET)
#define LED_G_TOGGLE  HAL_GPIO_TogglePin(GPIOF, GPIO_PIN_14)
#define KEY_PRESS          (HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_10))?0x00:0x01

#define LED_INIT \
{\
  HAL_GPIO_WritePin(GPIOF, GPIO_PIN_14, GPIO_PIN_SET);\
  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7, GPIO_PIN_SET);\
}\


#define SERIAL_DEBUG_PERIOD 100

void testSerialDebug(void);
void checkUART(void);
void write_uart_noblocking(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size);
void write_uart_blocking(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size);
void serial_debug_task(void const *argu);
void send_serial_debug_msg(void);
#endif
