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
#include "gimbal_task.h"
#include "stdlib.h"
#include "shoot_task.h"
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
	gimbal_param_init();
	gimbal_back_param();
	shoot_param_init();
}

void chassis_param_init(void)
{
	memset(&chassis, 0, sizeof(chassis_t));

  for (int k = 0; k < 4; k++)
  {
    PID_struct_init(&pid_spd[k], POSITION_PID, 10000, 1000, 3.0f, 0, 0);
  }
  
}

/**
  * @brief initialize gimbal pid parameter
  *
  */
void gimbal_param_init(void)
{
  memset(&gim, 0, sizeof(gimbal_t));
  //gim.pit_offset_angle = PIT_ECD_CENTER_OFFSET*ENCODER_ANGLE_RATIO;
	//gim.yaw_offset_angle = YAW_ECD_CENTER_OFFSET*ENCODER_ANGLE_RATIO;
	
	
  gim.ctrl_mode      = GIMBAL_INIT;
  gim.last_ctrl_mode = GIMBAL_RELAX;
 
	
	PID_struct_init(&pid_chassis_angle, POSITION_PID, 300, 10,
                  10 , 0.0, 0);  // TODO
	/*gimbal offset */
  
  /* pitch axis motor pid parameter */
  PID_struct_init(&pid_pit, POSITION_PID, 300, 10,
                  30, 0, 0); //
  PID_struct_init(&pid_pit_speed, POSITION_PID, 7000, 1000,
                  15, 0.1, 0);

  /* yaw axis motor pid parameter */
  PID_struct_init(&pid_yaw, POSITION_PID, 1000, 50, // TODO MAX=1000 previously
                  20, 0.00, 0); //
  PID_struct_init(&pid_yaw_speed, POSITION_PID, 7000, 2000,
                  13, 0, 0 );
  //pid_yaw_speed.min_out= 200;
	//pid_yaw.min_out=100;
	
  /* bullet trigger motor pid parameter */
  
}


void shoot_param_init(void){
	
	
  memset(&shoot, 0, sizeof(shoot_t));
  
  shoot.ctrl_mode      = SHOT_DISABLE;
  shoot.fric_wheel_spd = 2000;
  //shot.remain_bullets = 0;
  
  memset(&trig, 0, sizeof(trigger_t));
 
  trig.dir             = 1;
  trig.feed_bullet_spd = 2000;
  trig.c_shot_spd      = 4000;
  trig.state         = TRIG_INIT;
	
	PID_struct_init(&pid_trigger, POSITION_PID, 10000, 2000,
                  15, 0, 10);
  PID_struct_init(&pid_trigger_speed, POSITION_PID, 7000, 3000,
                  1.5, 0.1, 5);
  
}

