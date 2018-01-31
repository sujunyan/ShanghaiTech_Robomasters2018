#ifndef _TEST__MOTOR_H
#define _TEST__MOTOR_H


#include "test_can.h"
#include "Serial_Debug.h"

#define CAN_3510_M1_ID 0x201
#define CAN_3510_M2_ID 0x202
#define CAN_3510_M3_ID 0x203
#define CAN_3510_M4_ID 0x204

void Set_CM_Speed(CAN_HandleTypeDef* hcan, int16_t cm1_iq, int16_t cm2_iq, int16_t cm3_iq, int16_t cm4_iq);
void test_motor(void);
void encoder_data_handle(CAN_HandleTypeDef* hcan,uint8_t motor_ID);
#endif
