#include "test_motor.h"

void Set_CM_Speed(CAN_HandleTypeDef* hcan, int16_t cm1_iq, int16_t cm2_iq, int16_t cm3_iq, int16_t cm4_iq)
{
		hcan->pTxMsg->StdId = 0x200;
		hcan->pTxMsg->IDE = CAN_ID_STD;
		hcan->pTxMsg->RTR = CAN_RTR_DATA;
		hcan->pTxMsg->DLC = 8;
    
    hcan->pTxMsg->Data[1] = (uint8_t)(cm1_iq >> 8);
    hcan->pTxMsg->Data[0] = (uint8_t)cm1_iq;
    hcan->pTxMsg->Data[3] = (uint8_t)(cm2_iq >> 8);
    hcan->pTxMsg->Data[2] = (uint8_t)cm2_iq;
    hcan->pTxMsg->Data[5] = (uint8_t)(cm3_iq >> 8);
    hcan->pTxMsg->Data[4] = (uint8_t)cm3_iq;
    hcan->pTxMsg->Data[7] = (uint8_t)(cm4_iq >> 8);
    hcan->pTxMsg->Data[6] = (uint8_t)cm4_iq;
    HAL_CAN_Transmit(hcan, 10);
	
		//debug
		uint8_t Msg[9];
		Msg[0]=CM_MSG;
		for (int i=0;i<8;i++)
				Msg[i+1]=hcan->pTxMsg->Data[i];
		
	  Send_Debug_Msg(Msg,sizeof(Msg));

}

