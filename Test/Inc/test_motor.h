#ifndef _TEST__MOTOR_H
#define _TEST__MOTOR_H


#include "test_can.h"
#include "Serial_Debug.h"

#define CAN_3510_M1_ID 0x201
#define CAN_3510_M2_ID 0x202
#define CAN_3510_M3_ID 0x203
#define CAN_3510_M4_ID 0x204

#define ENCODER_ANGLE_RATIO    (8192.0f/360.0f)
/* can receive motor parameter structure */
#define FILTER_BUF 5
#define CM_CAN hcan1
typedef struct
{
  uint16_t ecd;
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

typedef struct
{
  float           vx; // forward/back
  float           vy; // left/right
  float           vw; // 
  
  //chassis_mode_e  ctrl_mode;
  //chassis_mode_e  last_ctrl_mode;

  float           gyro_angle;
  float           gyro_palstance;

  int16_t         wheel_speed_fdb[4];
  int16_t         wheel_speed_ref[4];
  int16_t         current[4];
  
  int16_t         position_ref;
  uint8_t         follow_gimbal;
} chassis_t;


void Set_CM_Speed(void);
void test_motor(void);
void encoder_data_handle(CAN_HandleTypeDef* hcan,moto_measure_t* ptr);
void get_moto_offset(moto_measure_t* ptr, CAN_HandleTypeDef* hcan);
void print_encoder(moto_measure_t*);
int is_Motor_Reversed(int i);

extern moto_measure_t moto_chassis[4];
extern moto_measure_t moto_pit;
extern moto_measure_t moto_yaw;
extern chassis_t chassis;
#endif
