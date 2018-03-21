/****************************************************************************
 *  Copyright (C) 2018 RoboMaster.
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of 
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program. If not, see <http://www.gnu.org/licenses/>.
 ***************************************************************************/
/** @file infantry_info.h
 *  @version 1.0
 *  @date Oct 2017
 *
 *  @brief the information from computer
 *
 *  @copyright 2017 DJI RoboMaster. All rights reserved.
 *
 */

#ifndef __INFANTRY_INFO_H__
#define __INFANTRY_INFO_H__

#include "stm32f4xx_hal.h"

/** 
  * @brief  infantry robot data command id
  */
typedef enum
{
  CHASSIS_DATA_ID     = 0x0010,
  GIMBAL_DATA_ID      = 0x0011,
  SHOOT_TASK_DATA_ID  = 0x0012,
  INFANTRY_ERR_ID     = 0x0013,
  CONFIG_RESPONSE_ID  = 0x0014,
  CALI_RESPONSE_ID    = 0x0015,
  REMOTE_CTRL_INFO_ID = 0x0016,
  BOTTOM_VERSION_ID   = 0x0017,
  
  CHASSIS_CTRL_ID     = 0x00A0,
  GIMBAL_CTRL_ID      = 0x00A1,
  SHOOT_CTRL_ID       = 0x00A2,
  ERROR_LEVEL_ID      = 0x00A3,
  INFANTRY_STRUCT_ID  = 0x00A4,
  CALI_GIMBAL_ID      = 0x00A5,
} infantry_data_id_e;

typedef enum
{
  BOTTOM_DEVICE        = 0,
  GIMBAL_GYRO_OFFLINE  = 1,
  CHASSIS_GYRO_OFFLINE = 2,
  CHASSIS_M1_OFFLINE   = 3,
  CHASSIS_M2_OFFLINE   = 4,
  CHASSIS_M3_OFFLINE   = 5,
  CHASSIS_M4_OFFLINE   = 6,
  REMOTE_CTRL_OFFLINE  = 7,
  JUDGE_SYS_OFFLINE    = 8,
  PC_SYS_OFFLINE       = 9,
  GIMBAL_YAW_OFFLINE   = 10,
  GIMBAL_PIT_OFFLINE   = 11,
  TRIGGER_MOTO_OFFLINE = 12,
  BULLET_JAM           = 13,
  CHASSIS_CONFIG_ERR   = 14,
  GIMBAL_CONFIG_ERR    = 15,
  ERROR_LIST_LENGTH    = 16,
} err_id_e;


/********** the information send to computer ***********/

/** 
  * @brief  chassis information
  */
typedef __packed struct
{
  uint8_t ctrl_mode;      /* chassis control mode */
  float   gyro_palstance; /* chassis palstance(degree/s) from gyroscope */
  float   gyro_angle;     /* chassis angle(degree) relative to ground from gyroscope */
  float   ecd_palstance;  /* chassis palstance(degree/s) from chassis motor encoder calculated */
  float   ecd_calc_angle; /* chassis angle(degree) relative to ground from chassis motor encoder calculated */
  int16_t x_speed;        /* chassis x-axis move speed(mm/s) from chassis motor encoder calculated */
  int16_t y_speed;        /* chassis y-axis move speed(mm/s) from chassis motor encoder calculated */
  int32_t x_position;     /* chassis x-axis position(mm) relative to the starting point */
  int32_t y_position;     /* chassis y-axis position(mm) relative to the starting point */
} chassis_info_t;

/** 
  * @brief  gimbal information
  */
typedef __packed struct
{
  uint8_t ctrl_mode;          /* gimbal control mode */
  float   pit_relative_angle; /* pitch angle(degree) relative to the gimbal center */
  float   yaw_relative_angle; /* yaw angle(degree) relative to the gimbal center */
  float   pit_absolute_angle; /* pitch angle(degree) relative to ground */
  float   yaw_absolute_angle; /* yaw angle(degree) relative to ground */
  float   pit_palstance;      /* pitch axis palstance(degree/s) */
  float   yaw_palstance;      /* yaw axis palstance(degree/s) */
} gimbal_info_t;

/** 
  * @brief  shoot information
  */
typedef __packed struct
{
  int16_t remain_bullets;  /* the member of remain bullets */
  int16_t shot_bullets;    /* the member of bullets that have been shot */
  uint8_t fric_wheel_run;  /* friction run or not */
} shoot_info_t;



typedef __packed struct
{
  /* rocker channel information */
  int16_t ch1;
  int16_t ch2;
  int16_t ch3;
  int16_t ch4;
  /* left and right lever information */
  uint8_t sw1;
  uint8_t sw2;
  /* mouse movement and button information */
  __packed struct
  {
    int16_t x;
    int16_t y;
    int16_t z;
  
    uint8_t l;
    uint8_t r;
  } mouse;
  /* keyboard key information */
  __packed union
  {
    uint16_t key_code;
    __packed struct 
    {
      uint16_t W:1;
      uint16_t S:1;
      uint16_t A:1;
      uint16_t D:1;
      uint16_t SHIFT:1;
      uint16_t CTRL:1;
      uint16_t Q:1;
      uint16_t E:1;
      uint16_t R:1;
      uint16_t F:1;
      uint16_t G:1;
      uint16_t Z:1;
      uint16_t X:1;
      uint16_t C:1;
      uint16_t V:1;
      uint16_t B:1;
    } bit;
  } kb;
} rc_info_t;

/********** the information from computer *************************************/



/** 
  * @brief  chassis control information
  */
typedef __packed struct
{
  int16_t x_offset;   /* offset(mm) relative to the x-axis of the chassis center */
  int16_t y_offset;   /* offset(mm) relative to the y-axis of the chassis center */
  float   w_speed;    /* rotation speed(degree/s) of chassis */
} chassis_rotate_t;

typedef __packed struct
{
  uint8_t          ctrl_mode; /* chassis control mode */
  int16_t          x_speed;   /* x-axis move speed(mm/s) of chassis */
  int16_t          y_speed;   /* y-axis move speed(mm/s) of chassis */
  chassis_rotate_t w_info;    /* rotation control of chassis */
} chassis_ctrl_t;


typedef __packed struct
{
  uint8_t ctrl_mode;    /* gimbal control mode */
  float   pit_ref;      /* gimbal pitch reference angle(degree) */
  float   yaw_ref;      /* gimbal yaw reference angle(degree) */
  uint8_t visual_valid; /* visual information valid or not */
} gimbal_ctrl_t;


typedef __packed struct
{
  uint8_t shoot_cmd;      /* single shoot command */
  uint8_t c_shoot_cmd;    /* continuous shoot command */
  uint8_t fric_wheel_run; /* friction run or not */
  uint8_t fric_wheel_spd; /* fricrion wheel speed */
} shoot_ctrl_t;

typedef struct
{
  /* data send */
  chassis_info_t    chassis_information;
  gimbal_info_t     gimbal_information;
  shoot_info_t      shoot_task_information;
  rc_info_t         remote_ctrl_data;
} send_pc_t;


typedef struct
{
  /* data receive */
  chassis_ctrl_t       chassis_control_data;
  gimbal_ctrl_t        gimbal_control_data;
  shoot_ctrl_t         shoot_control_data;
  
} receive_pc_t;

/* data send */
extern send_pc_t    pc_send_mesg;
/* data receive */
extern receive_pc_t pc_rece_mesg;

void pc_data_handle(uint8_t *p_frame);

#endif
