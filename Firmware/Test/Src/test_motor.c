#include "test_motor.h"
#include "can.h"
#include "RemoteTask.h"
#include "pid.h"
moto_measure_t moto_chassis[4];
moto_measure_t moto_pit;
moto_measure_t moto_yaw;
chassis_t chassis;

void encoder_data_handle(CAN_HandleTypeDef* hcan,moto_measure_t* ptr)
{
	ptr->last_ecd = ptr->ecd;
  ptr->ecd      = (uint16_t)(hcan->pRxMsg->Data[0] << 8 | hcan->pRxMsg->Data[1]);
  
  if (ptr->ecd - ptr->last_ecd > 4096)
  {
    ptr->round_cnt--;
    ptr->ecd_raw_rate = ptr->ecd - ptr->last_ecd - 8192;
  }
  else if (ptr->ecd - ptr->last_ecd < -4096)
  {
    ptr->round_cnt++;
    ptr->ecd_raw_rate = ptr->ecd - ptr->last_ecd + 8192;
  }
  else
  {
    ptr->ecd_raw_rate = ptr->ecd - ptr->last_ecd;
  }

  ptr->total_ecd = ptr->round_cnt * 8192 + ptr->ecd - ptr->offset_ecd;
  /* total angle, unit is degree */
  ptr->total_angle = ptr->total_ecd / ENCODER_ANGLE_RATIO;
  
#ifdef CHASSIS_EC60
  int32_t temp_sum = 0;
  ptr->rate_buf[ptr->buf_cut++] = ptr->ecd_raw_rate;
  if (ptr->buf_cut >= FILTER_BUF)
    ptr->buf_cut = 0;
  for (uint8_t i = 0; i < FILTER_BUF; i++)
  {
    temp_sum += ptr->rate_buf[i];
  }
  ptr->filter_rate = (int32_t)(temp_sum/FILTER_BUF);
  ptr->speed_rpm   = (int16_t)(ptr->filter_rate * 7.324f);
#else
  ptr->speed_rpm     = (int16_t)(hcan->pRxMsg->Data[2] << 8 | hcan->pRxMsg->Data[3]);
  ptr->given_current = (int16_t)(hcan->pRxMsg->Data[4] << 8 | hcan->pRxMsg->Data[5]);
#endif
	
	#if 0
	static int cnt=1;
	cnt++;
	//printf("cnt=%d\n\r",cnt);
	if(cnt%1000==0)
	{
		int16_t degree=(hcan->pRxMsg->Data[0]<<8)+hcan->pRxMsg->Data[1];
		int16_t rpm_speed=(hcan->pRxMsg->Data[2]<<8)+hcan->pRxMsg->Data[3];
		int16_t Torque_current=(hcan->pRxMsg->Data[4]<<8)+hcan->pRxMsg->Data[5];
		int16_t temperature=(hcan->pRxMsg->Data[4]<<8)+hcan->pRxMsg->Data[5];
		printf("Motor %d Msg recieved in time %d cnt=%d",motor_ID,HAL_GetTick(),cnt);
		printf("degree: %d rpm_speed %d Torque_current %d temperature %d \r\n",
						degree,rpm_speed,Torque_current,temperature);
		
	}
	#endif
	
}


void Set_CM_Speed(void)
{
	//LED_Red_Off();
	//LED_Green_Off();
		CAN_HandleTypeDef* hcan=&CM_CAN;
		//LED_Green_On();
		hcan->pTxMsg->StdId = 0x200;
		hcan->pTxMsg->IDE = CAN_ID_STD;
		hcan->pTxMsg->RTR = CAN_RTR_DATA;
		hcan->pTxMsg->DLC = 8;
	
		//机械部分反接
    //cm2_iq=-cm2_iq;
		//cm3_iq=-cm3_iq;
	#if 0
	#define TOT_M_CURRENT 16384.0f
		printf("Try to set the speed %fA  %fA  %fA  %fA  at time %d \n\r",
	chassis.current[0]/TOT_M_CURRENT,
	chassis.current[1]/TOT_M_CURRENT,
	chassis.current[2]/TOT_M_CURRENT,
	chassis.current[3]/TOT_M_CURRENT,HAL_GetTick());
	#endif
    hcan->pTxMsg->Data[0] = (uint8_t)(chassis.current[0] >> 8);
    hcan->pTxMsg->Data[1] = (uint8_t)chassis.current[0];
    hcan->pTxMsg->Data[2] = (uint8_t)(chassis.current[1] >> 8);
    hcan->pTxMsg->Data[3] = (uint8_t)chassis.current[1];
    hcan->pTxMsg->Data[4] = (uint8_t)(chassis.current[2] >> 8);
    hcan->pTxMsg->Data[5] = (uint8_t)chassis.current[2];
    hcan->pTxMsg->Data[6] = (uint8_t)(chassis.current[3] >> 8);
    hcan->pTxMsg->Data[7] = (uint8_t)chassis.current[3];
    if(HAL_CAN_Transmit(hcan, 10)!=HAL_OK)
		{
			//LED_Green_On();
			printf("CM_CAN status is %d \r\n",hcan->State);
		}
		//LED_Green_Off();
		//debug	
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
	
	static uint16_t tmp=0;
	int A=2;
	tmp++;
	if(tmp<100)return;
	#if 1
  for(int i=0;i<4;i++)
	{
		chassis.wheel_speed_ref[i]= (RC_CtrlData.rc.ch3!=0&&
																RC_CtrlData.rc.s1==1&&
																RC_CtrlData.rc.s2==1)?(RC_CtrlData.rc.ch3-1024)*A:0;
		//chassis.wheel_speed_ref[i]=500;
		chassis.current[i]=pid_calc(&pid_spd[i],moto_chassis[i].speed_rpm,
											is_Motor_Reversed(i)*chassis.wheel_speed_ref[i]);
		//chassis.current[i]=0x90;
		if(tmp%100000 ==0)
		{
			printf("motor%d:",i+1);
			print_encoder(&moto_chassis[i]);
		}
	}
	//printf("\n");
	#endif
	#if 0  //test one motor
	int i=3;
	chassis.wheel_speed_ref[i]=600;
	chassis.current[i]=pid_calc(&pid_spd[i],moto_chassis[i].speed_rpm,
											is_Motor_Reversed(i)*chassis.wheel_speed_ref[i]);
	print_encoder(&moto_chassis[i]);
	#endif
	Set_CM_Speed();	
	
}

void print_encoder(moto_measure_t* ptr){
	printf("speed_rpm:%d  current:%d \r\n",ptr->speed_rpm,
  ptr->given_current);
}

int is_Motor_Reversed(int i){ // some of the motor is reversed due to symmetric
	switch (i)
	{
		case 0:
		case 3:
			return 1;
		case 1:
		case 2:
			return -1;
		default:
			return 0;
	}
}

