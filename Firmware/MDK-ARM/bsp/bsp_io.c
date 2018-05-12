
#include "bsp_io.h"
#include "sys_config.h"
#include "tim.h"


void turn_on_laser(void)
{
  HAL_GPIO_WritePin(GPIOG, GPIO_PIN_13, GPIO_PIN_SET);
}

void turn_off_laser(void)
{
  HAL_GPIO_WritePin(GPIOG, GPIO_PIN_13, GPIO_PIN_RESET);
}

void turn_on_friction_wheel(uint16_t spd)
{
  LEFT_FRICTION  = spd;
  RIGHT_FIRCTION = spd;
}

void turn_off_friction_wheel(void)
{
  LEFT_FRICTION  = 1000;
  RIGHT_FIRCTION = 1000;
}

void mpu_heat_ctrl(uint16_t pwm_pulse)
{
  IMU_PWM_PULSE = pwm_pulse;
}


void pwm_device_init(void)
{
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2); // ctrl imu temperature
  HAL_TIM_PWM_Start(&htim12, TIM_CHANNEL_1); // beep
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4); // friction wheel //TODO
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
}

uint8_t sd_insert(void)
{
  return !HAL_GPIO_ReadPin(SD_EXTI_GPIO_Port, SD_EXTI_Pin);
}
