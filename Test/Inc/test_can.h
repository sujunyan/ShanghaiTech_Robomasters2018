/**
  *@file test_can.h
  *@date 2016-12-12
  *@author Albert.D
  *@brief 
  */
  
#ifndef _TEST__CAN_H
#define _TEST__CAN_H

#include "stm32f4xx_HAL.h"

#define TEST_CAN1_ID    0x201
#define TEST_CAN2_ID    0x202

extern uint8_t can1_rx_data[8];
extern uint8_t can2_rx_data[8];

void CanFilter_Init(CAN_HandleTypeDef* hcan);
void CAN_Send_Msg(CAN_HandleTypeDef* hcan, uint8_t *msg, uint32_t id, uint8_t len);

#endif

