#include "test_motor.h"
#include "can.h"
#include "RemoteTask.h"



void encoder_data_handle(CAN_HandleTypeDef* hcan,uint8_t motor_ID)
{
	static int cnt=0;
	
	if(cnt++%10==0)
	{
		uint16_t degree=(hcan->pRxMsg->Data[0]<<8)+hcan->pRxMsg->Data[1];
		uint16_t rpm_speed=(hcan->pRxMsg->Data[2]<<8)+hcan->pRxMsg->Data[3];
		uint16_t Torque_current=(hcan->pRxMsg->Data[4]<<8)+hcan->pRxMsg->Data[5];
		uint16_t temperature=(hcan->pRxMsg->Data[4]<<8)+hcan->pRxMsg->Data[5];
		printf("Motor %d Msg recieved in time %d \n",motor_ID,HAL_GetTick());
		printf("degree： %d rpm_speed %d Torque_current %d temperature %d \n",
						degree,rpm_speed,Torque_current,temperature);
		cnt=0;
	}
	
}
	
void Set_CM_Speed(CAN_HandleTypeDef* hcan, int16_t cm1_iq, int16_t cm2_iq, int16_t cm3_iq, int16_t cm4_iq)
{
		hcan->pTxMsg->StdId = 0x200;
		hcan->pTxMsg->IDE = CAN_ID_STD;
		hcan->pTxMsg->RTR = CAN_RTR_DATA;
		hcan->pTxMsg->DLC = 8;
	
		//机械部分反接
    cm2_iq=-cm2_iq;
		cm3_iq=-cm3_iq;
	
    hcan->pTxMsg->Data[0] = (uint8_t)(cm1_iq >> 8);
    hcan->pTxMsg->Data[1] = (uint8_t)cm1_iq;
    hcan->pTxMsg->Data[2] = (uint8_t)(cm2_iq >> 8);
    hcan->pTxMsg->Data[3] = (uint8_t)cm2_iq;
    hcan->pTxMsg->Data[4] = (uint8_t)(cm3_iq >> 8);
    hcan->pTxMsg->Data[5] = (uint8_t)cm3_iq;
    hcan->pTxMsg->Data[6] = (uint8_t)(cm4_iq >> 8);
    hcan->pTxMsg->Data[7] = (uint8_t)cm4_iq;
    HAL_CAN_Transmit(hcan, 10);
		
		
		//debug
		printf("Try to set the speed 0x%x 0x%x 0x%x 0x%x at time %d \n\r",cm1_iq,cm2_iq,cm3_iq,cm4_iq,HAL_GetTick());
	/*printf("Data Set to CM is:");
	
	
		for (int i=0;i<8;i++)
				printf("%0x,",hcan->pTxMsg->Data[i]);
		printf("\n\r");
	*/

}

/*协议定义：
* ch0 右摇杆 364-1024-1684 左->右
* ch1 右摇杆 364-1024-1684 下->上
*	ch2 左摇杆 364-1024-1684 左->右
* ch3 左摇杆 364-1024-1684 下->上
*  s1 left  switch  上-中-下  1-3-2
*  s2 right ... 
*/
extern RC_Ctl_t RC_CtrlData;   //remote control data
void test_motor(void)
{
	static int cnt10=0;
	static int A=1;
	
	/*
	cnt10++;
	if(RC_CtrlData.rc.ch1!=0 &&cnt10>20)
	{	
		int16_t speed=(RC_CtrlData.rc.ch1-1024)*A;
		Set_CM_Speed(&hcan1,speed,speed,speed,speed);
	}
	else 
		Set_CM_Speed(&hcan1,0,0,0,0);
	*/
	Set_CM_Speed(&hcan1,0x90,0x90,0x90,0x090);
	
	//HAL_CAN_Receive(&hcan1,CAN_FIFO0,10);
}

