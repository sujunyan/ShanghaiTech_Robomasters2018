//
// Created by su on 2/4/18.
//

#ifndef PC_COMMUNICATION_JUDGEMENT_INFO_H
#define PC_COMMUNICATION_JUDGEMENT_INFO_H


#define JUDGE_FRAME_BUFLEN 200

/**
  * @brief  judgement data command id
  */
typedef enum
{
  GAME_INFO_ID       = 0x0001,  // 10Hz
  REAL_BLOOD_DATA_ID = 0x0002,  // Damage Taken
  REAL_SHOOT_DATA_ID = 0x0003,  // Bullet Shooting
  POWER_HEAT_DATA_ID = 0x0004,  // 50Hz
  REAL_FIELD_DATA_ID = 0x0005,  // 10Hz if RFID exists
  GAME_RESULT_ID     = 0x0006,  // End of the game
  GAIN_BUFF_ID       = 0x0007,  // Buff gained
  ROBOT_POSITION_ID  = 0x0008,  // 50Hz

  STU_CUSTOM_DATA_ID = 0x0100,
  UPLOAD_DATA_ID     = 0x0101,
  DOWNLOAD_DATA_ID   = 0x0102,
} judge_data_id_e;

/**
  * @brief  game information structures definition(0x0001)
  *         this package send frequency is 10Hz
  */
typedef struct
{
  uint16_t stageRemainTime;
  uint8_t gameProgress;
  uint8_t  robotLevel;
  uint16_t remainHP;
  uint16_t maxHP;
} __attribute__((packed))extGameRobotState_t;

/**
  * @brief  real time blood volume change data(0x0002)
  */
typedef struct
{
  uint8_t armorType:4;
 /* 0-3bits: the attacked armor id:
    0x00: 0 front
    0x01: 1 left
    0x02: 2 behind
    0x03: 3 right
    0x04: 4 above1
    0x05: 5 above2
    others reserved */
  uint8_t hurtType:4;
 /* 4-7bits: blood volume change type
    0x00: armor attacked
    0x01: module offline */
} __attribute__((packed))extRobotHurt_t;

/**
  * @brief  real time shooting data(0x0003)
  */
typedef struct
{
  uint8_t bulletType;
  uint8_t bulletFreq;
  float   bulletSpeed;
} __attribute__((packed))extShootData_t;

/**
  * @brief  real time power heat data(0x0004)
  */
typedef struct
{
  float chassisVolt;
  float chassisCurrent;
  float chassisPower;
  float chassisPowerBuffer;
  uint16_t shooterHeat0;
  uint16_t shooterHeat1;
} __attribute__((packed))extPowerHeatData_t;

/**
 * @brief  rfid detect data(0x0005)
 */
typedef struct
{
 uint8_t cardType;
 /* 1bit: card type:
    0: ATK card
    1: DEF card
    2: red HEAL card
    3: blue HEAL card
    4: red ENERGY card
    5: blue ENERGY card
    others reserved */
 uint8_t cardIdx;
} __attribute__((packed))extRfidDetect_t;

/**
 * @brief  game result data(0x0006)
 */
typedef struct
{
 uint8_t winner;
 /* 1bit: winner:
    0: draw-game
    1: red win
    2: blue win
    others reserved */
} __attribute__((packed))extGameResult_t;

/**
 * @brief  the data of get field buff(0x0007)
 */
typedef struct
{
 uint8_t buffType;
 /* 1bit: buff type:
    0: ATK buff
    1: DEF buff
    2: ENERGY buff
    others reserved */
 uint8_t buffAddition;
 /* 1bit: percentage:
    10 == 10% */
} __attribute__((packed))extGetBuff_t;

/**
  * @brief  GPS state structures definition(0x0008)
  *         this package send frequency is 50Hz
  */
typedef struct
{
  float x;
  float y;
  float z;
  float yaw;
} __attribute__((packed))extGameRobotPos_t;

/**
 * @brief  student custom data(0x0100)
 */
typedef struct
{
 float data1;
 float data2;
 float data3;
uint8_t mask;
} __attribute__((packed))extShowData_t;

/**
 * @brief  UPLOAD (0x0101)
 */
typedef struct
{
 uint8_t data[64];
} __attribute__((packed))extUpStreamData_t;

/**
 * @brief  DOWNLOAD (0x0101)
 */
typedef struct
{
  uint8_t data[32];
} __attribute__((packed))extDownStreamData_t;

/**
  * @brief  the data structure receive from judgement
  */
typedef struct
{
  extGameRobotState_t     game_information;
  extRobotHurt_t          blood_changed_data;
  extShootData_t          real_shoot_data;
  extPowerHeatData_t      power_heat_data;
  extRfidDetect_t         rfid_data;
  extGameResult_t         game_result_data;
  extGetBuff_t            get_buff_data;
  extGameRobotPos_t       robot_position_data;
  extShowData_t           clinet_show_data;
  extDownStreamData_t     clinet_download_data;
} __attribute__((packed))receive_judge_t;
#endif
//PC_COMMUNICATION_JUDGEMENT_INFO_H
