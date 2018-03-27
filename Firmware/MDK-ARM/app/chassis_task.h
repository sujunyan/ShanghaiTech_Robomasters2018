#ifndef __CHASIS_TASK__
#define __CHASIS_TASK__


#include "sys_config.h"
#include "bsp_can.h"


#define FILTER_BUF 5

#define CHASSIS_TASK_PERIOD 10

typedef struct
{
  uint16_t ecd;   // encoder value
  uint16_t last_ecd;
  
  int16_t  speed_rpm;
  int16_t  given_current;

  int32_t  round_cnt;
  int32_t  total_ecd;
  int32_t  total_angle;
  
  uint16_t offset_ecd;
  uint32_t msg_cnt;
  
  int32_t  ecd_raw_rate;
  int32_t  rate_buf[FILTER_BUF];
  uint8_t  buf_cut;
  int32_t  filter_rate;
} moto_measure_t;

typedef enum
{
  CHASSIS_RELAX          = 0,
  CHASSIS_STOP           = 1,
	
  MANUAL_SEPARATE_GIMBAL = 2,
  MANUAL_FOLLOW_GIMBAL   = 3,
	
  DODGE_MODE             = 4,
  AUTO_SEPARATE_GIMBAL   = 5,
  AUTO_FOLLOW_GIMBAL     = 6,
} chassis_mode_e;


typedef struct
{
  float           vx; // forward/back
  float           vy; // left/right
  float           vw; // 

	chassis_mode_e  ctrl_mode;
  chassis_mode_e  last_ctrl_mode;
	
	moto_measure_t 	motor[4];
  int16_t         wheel_speed_fdb[4];
  int16_t         wheel_speed_ref[4];
  int16_t         current[4];
  

} chassis_t;


void chassis_task(const void* argu);
void test_motor(void);
void encoder_data_handle(CAN_HandleTypeDef* hcan,moto_measure_t* ptr);
void get_moto_offset(moto_measure_t* ptr, CAN_HandleTypeDef* hcan);
void chasis_remote_handle(void);
void print_encoder(moto_measure_t*);
int is_Motor_Reversed(int i);
uint8_t chassis_is_controllable(void);
uint8_t chassis_is_auto(void);
uint8_t chassis_is_follow(void);
void mecanum_calc(float vx, float vy, float vw, int16_t speed[]);
void chassis_mode_switch(void);
extern chassis_t chassis;
#endif
/*
#define CAN_3510_M1_ID 0x201
#define CAN_3510_M2_ID 0x202
#define CAN_3510_M3_ID 0x203
#define CAN_3510_M4_ID 0x204
*/

