/**
  *@file test_can.c
  *@date 2016-12-12
  *@author Albert.D
  *@brief 
  */

#include "test_can.h"
#include "can.h"
#include "test_motor.h"
  
uint8_t can1_rx_data[8];
uint8_t can2_rx_data[8];

//can filter must be initialized before use
void CanFilter_Init(CAN_HandleTypeDef* hcan)
{
  CAN_FilterConfTypeDef canfilter;
  
  //create memory to save the message, if not will raise error
  static CanTxMsgTypeDef  Tx1Message;
  static CanRxMsgTypeDef  Rx1Message;
  static CanTxMsgTypeDef  Tx2Message;
  static CanRxMsgTypeDef  Rx2Message;
  
  canfilter.FilterMode = CAN_FILTERMODE_IDMASK;
  canfilter.FilterScale = CAN_FILTERSCALE_32BIT;
  
  //filtrate any ID you want here
  canfilter.FilterIdHigh = 0x0000;
  canfilter.FilterIdLow = 0x0000;
  canfilter.FilterMaskIdHigh = 0x0000;
  canfilter.FilterMaskIdLow = 0x0000;
  
  canfilter.FilterFIFOAssignment = CAN_FilterFIFO0;
  canfilter.FilterActivation = ENABLE;
  canfilter.BankNumber = 14;
  
  //use different filter for can1&can2
  if(hcan == &hcan1)
  {
    canfilter.FilterNumber = 14;
    hcan->pTxMsg = &Tx1Message;
    hcan->pRxMsg = &Rx1Message;
  }
  if(hcan == &hcan2)
  {
    canfilter.FilterNumber = 14;
    hcan->pTxMsg = &Tx2Message;
    hcan->pRxMsg = &Rx2Message;
  }
  
  HAL_CAN_ConfigFilter(hcan, &canfilter);
  
}


extern moto_measure_t moto_chassis[4];
extern moto_measure_t moto_pit;
extern moto_measure_t moto_yaw;
//it will be auto callback when can receive msg completely
// 
void HAL_CAN_RxCpltCallback(CAN_HandleTypeDef* hcan)
{
	//printf("Can data Recieved with ID: 0x%x at time %d\r\n",hcan->pRxMsg->StdId,HAL_GetTick());
  switch(hcan->pRxMsg->StdId)
  {
		case CAN_3510_M1_ID:
    case CAN_3510_M2_ID:
    case CAN_3510_M3_ID:
    case CAN_3510_M4_ID:
		{
			static uint8_t i;
      i = hcan->pRxMsg->StdId - CAN_3510_M1_ID;  // motor ID
			moto_chassis[i].msg_cnt++ <= 50? get_moto_offset(&moto_chassis[i],hcan):encoder_data_handle(hcan,&moto_chassis[i]);
		}
		break;
		
  }
  
	__HAL_CAN_ENABLE_IT(&hcan1, CAN_IT_FMP0);
  __HAL_CAN_ENABLE_IT(&hcan2, CAN_IT_FMP0);
  //HAL_CAN_Receive_IT(&hcan1, CAN_FIFO0);
  //HAL_CAN_Receive_IT(&hcan2, CAN_FIFO0);
}

void get_moto_offset(moto_measure_t* ptr, CAN_HandleTypeDef* hcan)
{
    ptr->ecd        = (uint16_t)(hcan->pRxMsg->Data[0] << 8 | hcan->pRxMsg->Data[1]);
    ptr->offset_ecd = ptr->ecd;
}

