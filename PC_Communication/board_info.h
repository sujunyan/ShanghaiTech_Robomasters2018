//
// Created by su on 2/4/18.
//

#ifndef PC_COMMUNICATION_BOARD_INFO_H
#define PC_COMMUNICATION_BOARD_INFO_H

#include <stdlib.h>
#include "judgement_info.h"
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

typedef enum
{
  DEVICE_NORMAL = 0,
  ERROR_EXIST   = 1,
  UNKNOWN_STATE = 2,
} bottom_err_e;

typedef enum
{
  GLOBAL_NORMAL        = 0,
  SOFTWARE_WARNING     = 1,
  SOFTWARE_ERROR       = 2,
  SOFTWARE_FATAL_ERROR = 3,
  GIMBAL_ERROR         = 4,
  CHASSIS_ERROR        = 5,
  HARAWARE_ERROR       = 6,
} err_level_e;

typedef enum
{
  NO_CONFIG      = 0,
  DEFAULT_CONFIG = 1,
  CUSTOM_CONFIG  = 3,
} struct_config_e;

/********** the information get from the board ***********/

/**
  * @brief  chassis information
  */
typedef struct
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
} __attribute__((packed)) chassis_info_t;

/**
  * @brief  gimbal information
  */
typedef struct
{
  uint8_t ctrl_mode;          /* gimbal control mode */
  float   pit_relative_angle; /* pitch angle(degree) relative to the gimbal center */
  float   yaw_relative_angle; /* yaw angle(degree) relative to the gimbal center */
  float   pit_absolute_angle; /* pitch angle(degree) relative to ground */
  float   yaw_absolute_angle; /* yaw angle(degree) relative to ground */
  float   pit_palstance;      /* pitch axis palstance(degree/s) */
  float   yaw_palstance;      /* yaw axis palstance(degree/s) */
} __attribute__((packed))gimbal_info_t;

/**
  * @brief  shoot information
  */
typedef struct
{
  int16_t remain_bullets;  /* the member of remain bullets */
  int16_t shot_bullets;    /* the member of bullets that have been shot */
  uint8_t fric_wheel_run;  /* friction run or not */
} __attribute__((packed))shoot_info_t;

/**
  * @brief  infantry error information
  */
typedef struct
{
  bottom_err_e err_sta;                 /* bottom error state */
  bottom_err_e err[ERROR_LIST_LENGTH];  /* device error list */
} __attribute__((packed))infantry_err_t;

/**
  * @brief  infantry structure config response
  */
typedef  struct
{
  struct_config_e chassis_config;
  struct_config_e gimbal_config;
} __attribute__((packed))config_response_t;

/**
  * @brief  gimbal calibrate response
  */
typedef struct
{
  uint8_t type;     //0x01 success 0x00 fault
  int16_t yaw_offset;
  int16_t pitch_offset;
} __attribute__((packed))cali_response_t;

/**
  * @brief  remote control information
  */
typedef struct
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
  struct
  {
    int16_t x;
    int16_t y;
    int16_t z;

    uint8_t l;
    uint8_t r;
  } __attribute__((packed))mouse;
  /* keyboard key information */
  union
  {
    uint16_t key_code;
    struct
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
    } __attribute__((packed))bit;
  } __attribute__((packed))kb;
} __attribute__((packed))rc_info_t;

/**
  * @brief  bottom software version information
  */
typedef struct
{
  uint8_t num[4];
} __attribute__((packed))version_info_t;

/********** the information from computer **********/

typedef struct
{
  int16_t x_offset;   /* offset(mm) relative to the x-axis of the chassis center */
  int16_t y_offset;   /* offset(mm) relative to the y-axis of the chassis center */
  float   w_speed;    /* rotation speed(degree/s) of chassis */
} __attribute__((packed))chassis_rotate_t;

/**
  * @brief  chassis control information
  */
typedef struct
{
  uint8_t          ctrl_mode; /* chassis control mode */
  int16_t          x_speed;   /* x-axis move speed(mm/s) of chassis */
  int16_t          y_speed;   /* y-axis move speed(mm/s) of chassis */
  chassis_rotate_t w_info;    /* rotation control of chassis */
} __attribute__((packed))chassis_ctrl_t;

/**
  * @brief  gimbal control information
  */
typedef struct
{
  uint8_t ctrl_mode;    /* gimbal control mode */
  float   pit_ref;      /* gimbal pitch reference angle(degree) */
  float   yaw_ref;      /* gimbal yaw reference angle(degree) */
  uint8_t visual_valid; /* visual information valid or not */
}__attribute__((packed)) gimbal_ctrl_t;

/**
  * @brief  shoot control information
  */
typedef struct
{
  uint8_t shoot_cmd;      /* single shoot command */
  uint8_t c_shoot_cmd;    /* continuous shoot command */
  uint8_t fric_wheel_run; /* friction run or not */
  uint8_t fric_wheel_spd; /* fricrion wheel speed */
} __attribute__((packed))shoot_ctrl_t;

/**
  * @brief  robot system error level
  */
typedef struct
{
  err_level_e err_level; /* the error level is included in err_level_e enumeration */
} __attribute__((packed))global_err_level_t;

/**
  * @brief  infantry structure configuration information
  */
typedef struct
{
  struct_config_e  chassis_config;  /* chassis structure config state */
  uint16_t         wheel_perimeter; /* the perimeter(mm) of wheel */
  uint16_t         wheel_track;     /* wheel track distance(mm) */
  uint16_t         wheel_base;      /* wheelbase distance(mm) */
  struct_config_e  gimbal_config;   /* gimbal structure config state */
  int16_t          gimbal_x_offset; /* gimbal offset(mm) relative to the x-axis of the chassis center */
  int16_t          gimbal_y_offset; /* gimbal offset(mm) relative to the y-axis of the chassis center */
} __attribute__((packed))infantry_structure_t;

/**
  * @brief  gimbal calibrate command
  */
typedef struct
{
  uint8_t type;        /* 0x01 calibrate gimbal center, 0x02 calibrate camera */
} __attribute__((packed))cali_cmd_t;

/********* variables **********/
/**
  * @brief  the data structure send to pc
  */
typedef struct
{
  /* data send */
  chassis_info_t    chassis_information;
  gimbal_info_t     gimbal_information;
  shoot_info_t      shoot_task_information;
  infantry_err_t    bottom_error_data;
  config_response_t structure_config_data;
  cali_response_t   cali_response_data;
  rc_info_t         remote_ctrl_data;
  version_info_t    version_info_data;
} __attribute__((packed))receive_board_t;
/**
  * @brief  the data structure receive from pc
  */
typedef struct
{
  /* data receive */
  chassis_ctrl_t       chassis_control_data;
  gimbal_ctrl_t        gimbal_control_data;
  shoot_ctrl_t         shoot_control_data;
  global_err_level_t   global_error_level;
  infantry_structure_t structure_data;
  cali_cmd_t           cali_cmd_data;
  /* receive to forward */
//  client_show_data_t   show_in_client_data;
//  user_to_server_t     pc_to_server_data;
} __attribute__((packed))send_board_t;

#endif //PC_COMMUNICATION_BOARD_INFO_H
