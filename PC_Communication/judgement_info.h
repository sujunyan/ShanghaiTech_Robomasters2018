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
  GAME_INFO_ID       = 0x0001,  //10Hz
  REAL_BLOOD_DATA_ID = 0x0002,
  REAL_SHOOT_DATA_ID = 0x0003,
  STU_CUSTOM_DATA_ID = 0x0005,
  //REAL_FIELD_DATA_ID = 0x0005,  //10hZ
  //GAME_RESULT_ID     = 0x0006,
  //GAIN_BUFF_ID       = 0x0007,

  //STU_CUSTOM_DATA_ID = 0x0100,
  //ROBOT_TO_CLIENT_ID = 0x0101,
  //CLIENT_TO_ROBOT_ID = 0x0102,
} judge_data_id_e;


/**
  * @brief  GPS state structures definition
  */
typedef struct
{
  uint8_t flag;
  float x;
  float y;
  float z;
  float compass;
} __attribute__((packed))tLocData;

/**
  * @brief  game information structures definition(0x0001)
  *         this package send frequency is 50Hz
  */
typedef struct
{
  uint32_t remainTime;
  uint16_t remainLifeValue;
  float realChassisOutV;
  float realChassisOutA;
  tLocData locData;
  float remainPower;
} __attribute__((packed))tGameInfo;

/**
  * @brief  real time blood volume change data(0x0002)
  */
typedef struct
{
  uint8_t weakId:4;
 /* 0-3bits: the attacked armor id:
    0x00: 0 front
    0x01：1 left
    0x02：2 behind
    0x03：3 right
    others reserved*/
  uint8_t way:4;
 /* 4-7bits: blood volume change type
    0x00: armor attacked
    0x01：module offline
    0x02: bullet over speed
    0x03: bullet over frequency */
  uint16_t value;
} __attribute__((packed))tRealBloodChangedData;

/**
  * @brief  real time shooting data(0x0003)
  */
typedef struct
{
  float realBulletShootSpeed;
  float realBulletShootFreq;
  float realGolfShootSpeed;
  float realGolfShootFreq;
} __attribute__((packed))tRealShootData;

/**
  * @brief  User-defined data(0x0005)
  *        200Hz max frequency
  *        send to server, display in student client
  */

typedef struct
{
  float data1;
  float data2;
  float data3;
} __attribute__((packed))tUserDefined;

///**
//  * @brief  rfid detect data(0x0005)
//  */
//typedef struct
//{
//  uint8_t card_type;
//  uint8_t card_idx;
//} __attribute__((packed))rfid_detect_t;

///**
//  * @brief  game result data(0x0006)
//  */
//typedef struct
//{
//  uint8_t winner;
//} __attribute__((packed))game_result_t;
//
///**
//  * @brief  the data of get field buff(0x0007)
//  */
//typedef struct
//{
//  uint8_t buff_type;
//  uint8_t buff_addition;
//} __attribute__((packed))get_buff_t;
//
///**
//  * @brief  student custom data
//  */
//typedef struct
//{
//  float data1;
//  float data2;
//  float data3;
//} __attribute__((packed))client_show_data_t;
//
//typedef struct
//{
//  uint8_t  data[64];
//} __attribute__((packed))user_to_server_t;
//
//typedef struct
//{
//  uint8_t  data[32];
//} __attribute__((packed))server_to_user_t;

/**
  * @brief  the data structure receive from judgement
  */
typedef struct
{
  tGameInfo               game_information;
  tRealBloodChangedData   blood_changed_data;
  tRealShootData          real_shoot_data;
  //rfid_detect_t      rfid_data;
  //game_result_t      game_result_data;
  //get_buff_t         get_buff_data;
  tUserDefined            student_download_data;
} __attribute__((packed))receive_judge_t;
#endif //PC_COMMUNICATION_JUDGEMENT_INFO_H
