#include "init.h"
#include "main.h"
#include "stm32f4xx_hal.h"
#include "can.h"
#include "dma.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "pid.h"
#include "chassis_task.h"
#include "sdio.h"
#include "rtc.h"
#include "imu_task.h"
void sys_init()
{
	 MX_GPIO_Init();
  MX_DMA_Init();
  MX_SDIO_SD_Init();
  MX_CAN1_Init();
  MX_RTC_Init();
  MX_SPI5_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_CAN2_Init();
  MX_TIM4_Init();
  MX_TIM5_Init();
  MX_TIM8_Init();
  MX_TIM12_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_USART3_UART_Init();
  MX_USART6_UART_Init();
  MX_TIM1_Init();
}

void pram_init(void)
{
	chassis_param_init();
	detector_param_init();
	imu_param_init();
}

void chassis_param_init(void)
{
	memset(&chassis, 0, sizeof(chassis_t));

  for (int k = 0; k < 4; k++)
  {
    PID_struct_init(&pid_spd[k], POSITION_PID, 8000, 1000, 3.0f, 0, 0);
  }
  
}

