#ifndef _TEST__MOTOR_H
#define _TEST__MOTOR_H


#include "test_can.h"
#include "Serial_Debug.h"
void Set_CM_Speed(CAN_HandleTypeDef* hcan, int16_t cm1_iq, int16_t cm2_iq, int16_t cm3_iq, int16_t cm4_iq);

#endif
