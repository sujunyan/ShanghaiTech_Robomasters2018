#ifndef __GIMBAL_TASK_H__
#define __GIMBAL_TASK_H__

#include "stm32f4xx_hal.h"
#include "chassis_task.h"
/* gimbal control period time (ms) */
#define GIMBAL_PERIOD 5
#define CALI_DONE
#define PIT_ECD_CENTER_OFFSET 2434
#define YAW_ECD_CENTER_OFFSET 3816


typedef enum
{
  GIMBAL_RELAX         = 0,
  GIMBAL_INIT          = 1,
  GIMBAL_NO_ARTI_INPUT = 2,
  GIMBAL_FOLLOW_ZGYRO  = 3,
  GIMBAL_TRACK_ARMOR   = 4,
  GIMBAL_PATROL_MODE   = 5,
  GIMBAL_SHOOT_BUFF    = 6,
  GIMBAL_POSITION_MODE = 7,
} gimbal_mode_e;

typedef enum
{
  NO_ACTION = 0,
  IS_ACTION,
} action_mode_e;

typedef enum
{
  CMD_CALI_FIVE = 0,
  CMD_CALI_NINE,
  CMD_TARGET_NUM
} gimbal_cmd_e;



typedef struct
{
  /* position loop */
  float yaw_angle_ref;
  float pit_angle_ref;
  float yaw_angle_fdb;
  float pit_angle_fdb;
  /* speed loop */
  float yaw_speed_ref;
  float pit_speed_ref;
  float yaw_speed_fdb;
  float pit_speed_fdb;
} gim_pid_t;

typedef struct
{
  /* unit: degree */
  float pit_relative_angle_ecd;  // relative_angle to the center
  float yaw_relative_angle_ecd;
  float pit_relative_angle_imu;
	float yaw_relative_angle_imu;
	float pit_offset_angle_imu;
  float yaw_offset_angle_imu;
  /* uint: degree/s */
  float yaw_palstance;
  float pit_palstance;
} gim_sensor_t;


typedef struct
{
  /* ctrl mode */
  gimbal_mode_e ctrl_mode;
  gimbal_mode_e last_ctrl_mode;
  
  /* gimbal information */
  gim_sensor_t  sensor;
	
  
  
  /* gimbal ctrl parameter */
  gim_pid_t     pid;
	
	

	/*current sent to the gimbal*/
	/* yaw  pitch*/
	int16_t 			current[2];
	
  moto_measure_t pit_motor;
	moto_measure_t yaw_motor;
} gimbal_t;

extern gimbal_t gim;


static void init_mode_handle(void);
static void close_loop_handle(void);



static void cascade_pid_ctrl(void);
float remote_ctrl_map(float offset,float step);
void gimbal_param_init(void);
void gimbal_back_param(void);
uint8_t gimbal_is_controllable(void);
void gimbal_task(void const *argu);
void gimbal_self_check(void);
void cascade_pid_ctrl(void);
void update_gimbal_sensor(void);
int16_t get_relative_pos(int16_t raw_ecd, int16_t center_offset);
#endif
