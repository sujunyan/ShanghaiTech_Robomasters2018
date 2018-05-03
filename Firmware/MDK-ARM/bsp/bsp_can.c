/****************************************************************************
 *  Copyright (C) 2018 RoboMaster.
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of 
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program. If not, see <http://www.gnu.org/licenses/>.
 ***************************************************************************/
/** @file bsp_can.c
 *  @version 1.0
 *  @date Apr 2017
 *
 *  @brief receive external can device message
 *
 *  @copyright 2017 DJI RoboMaster. All rights reserved.
 *
 */
#include "chassis_task.h"
#include "bsp_can.h"
#include "detect_task.h"
#include "bsp_uart.h"
#include "string.h"
#include "sys_config.h"
#include "gimbal_task.h"
#include "imu_task.h"
#include "shoot_task.h"
//float yaw_zgyro_angle;
void get_moto_offset(moto_measure_t* ptr, CAN_HandleTypeDef* hcan)
{
    ptr->ecd        = (uint16_t)(hcan->pRxMsg->Data[0] << 8 | hcan->pRxMsg->Data[1]);
    ptr->offset_ecd = ptr->ecd;
}

#if 1
float can2_buf[3];
void HAL_CAN_RxCpltCallback(CAN_HandleTypeDef* _hcan){
//	static uint32_t cnt=0;
	//printf("can recv ID %x T %d \r\n",_hcan->pRxMsg->StdId,osKernelSysTick());
  switch (_hcan->pRxMsg->StdId)
  {
    case CAN_3510_M1_ID:
    case CAN_3510_M2_ID:
    case CAN_3510_M3_ID:
    case CAN_3510_M4_ID:
    {
			//if(gim.ctrl_mode == GIMBAL_INIT)break; //TODO
      static uint8_t i;
      i = _hcan->pRxMsg->StdId - CAN_3510_M1_ID;
			if(chassis.motor[i].msg_cnt++ <= 50)
			{
				get_moto_offset(&chassis.motor[i], _hcan);
				chassis.ctrl_mode=CHASSIS_RELAX;
			}else encoder_data_handle( _hcan,&chassis.motor[i]);
      err_detector_hook(CHASSIS_M1_OFFLINE + i);
    }
    break;
		
    case CAN_YAW_MOTOR_ID:
    {
			//printf("yaw motor recv T %d\r\n",osKernelSysTick());
      encoder_data_handle( _hcan,&gim.yaw_motor);					
      err_detector_hook(GIMBAL_YAW_OFFLINE);
    }
    break;
    case CAN_PIT_MOTOR_ID:
    {
      encoder_data_handle(_hcan,&gim.pit_motor);
      err_detector_hook(GIMBAL_PIT_OFFLINE);
    }
    break;

    case CAN_TRIGGER_MOTOR_ID:
    {
      if (_hcan == &TRIGGER_CAN)
      {
        trig.motor.msg_cnt++;
        trig.motor.msg_cnt <= 10 ? get_moto_offset(&trig.motor, _hcan) : encoder_data_handle(_hcan,&trig.motor );
        err_detector_hook(TRIGGER_MOTO_OFFLINE);
      }
      else
      {
      }
    }
    break;

		case (CAN_CHASSIS_ZGYRO_ID): // gyro
     { 
           int  temp_yaw_angle = (int32_t)(_hcan->pRxMsg->Data[0]<<24)|(int32_t)(_hcan->pRxMsg->Data[1]<<16) 
            | (int32_t)(_hcan->pRxMsg->Data[2]<<8) | (int32_t)(_hcan->pRxMsg->Data[3]);
            
         
            float this_yaw_angle = -((float)temp_yaw_angle*0.01);
						//printf("CAN_CHASSIS_ZGYRO (yaw) data is %f \r\n",this_yaw_angle);
    }break;
		
		
		// The gyro in can2 
		case 0xA:
		{
			LED_G_TOGGLE;
			can2_buf[0]=*(float *)&(_hcan->pRxMsg->Data[0]);
			can2_buf[1]=*(float *)&(_hcan->pRxMsg->Data[4]);
		}break;
		case 0xB:
		{
			LED_G_TOGGLE;
			can2_buf[2]=*(float *)&(_hcan->pRxMsg->Data[0]);
		}break;

		
		
    default:
    {
    }
    break;
  }
  
  __HAL_CAN_ENABLE_IT(&hcan1, CAN_IT_FMP0);
  __HAL_CAN_ENABLE_IT(&hcan2, CAN_IT_FMP0);
}
#endif


/**
  * @brief   can filter initialization
  * @param   CAN_HandleTypeDef
  * @retval  None
  */
void can_device_init(void){
  //can1 &can2 use same filter config
  CAN_FilterConfTypeDef  can_filter;
  static CanTxMsgTypeDef Tx1Message;
  static CanRxMsgTypeDef Rx1Message;
  static CanTxMsgTypeDef Tx2Message;
  static CanRxMsgTypeDef Rx2Message;

  can_filter.FilterNumber         = 0;
  can_filter.FilterMode           = CAN_FILTERMODE_IDMASK;
  can_filter.FilterScale          = CAN_FILTERSCALE_32BIT;
  can_filter.FilterIdHigh         = 0x0000;
  can_filter.FilterIdLow          = 0x0000;
  can_filter.FilterMaskIdHigh     = 0x0000;
  can_filter.FilterMaskIdLow      = 0x0000;
  can_filter.FilterFIFOAssignment = CAN_FilterFIFO0;
  can_filter.BankNumber           = 14;
  can_filter.FilterActivation     = ENABLE;
  HAL_CAN_ConfigFilter(&hcan1, &can_filter);
  //while (HAL_CAN_ConfigFilter(&hcan1, &can_filter) != HAL_OK);
  
  can_filter.FilterNumber         = 14;
  HAL_CAN_ConfigFilter(&hcan2, &can_filter);
  //while (HAL_CAN_ConfigFilter(&hcan2, &can_filter) != HAL_OK);
    
  hcan1.pTxMsg = &Tx1Message;
  hcan1.pRxMsg = &Rx1Message;
  hcan2.pTxMsg = &Tx2Message;
  hcan2.pRxMsg = &Rx2Message;
}


void can_receive_start(void){
  HAL_CAN_Receive_IT(&hcan1, CAN_FIFO0);
  HAL_CAN_Receive_IT(&hcan2, CAN_FIFO0); 
}

/**
  * @brief     reset single axis gyroscope 
  * @attention gyro reset at least wait 2s  
  */
void gyro_device_init(void){
  while (ZGYRO_CAN.State == HAL_CAN_STATE_BUSY_TX);
  ZGYRO_CAN.pTxMsg->StdId   = CAN_ZGYRO_RST_ID;
  ZGYRO_CAN.pTxMsg->IDE     = CAN_ID_STD;
  ZGYRO_CAN.pTxMsg->RTR     = CAN_RTR_DATA;
  ZGYRO_CAN.pTxMsg->DLC     = 0x08;
  ZGYRO_CAN.pTxMsg->Data[0] = 0;
  ZGYRO_CAN.pTxMsg->Data[1] = 1;
  ZGYRO_CAN.pTxMsg->Data[2] = 2;
  ZGYRO_CAN.pTxMsg->Data[3] = 3;
  ZGYRO_CAN.pTxMsg->Data[4] = 4;
  ZGYRO_CAN.pTxMsg->Data[5] = 5;
  ZGYRO_CAN.pTxMsg->Data[6] = 6;
  ZGYRO_CAN.pTxMsg->Data[7] = 7;
  HAL_CAN_Transmit(&ZGYRO_CAN, 10);
}

/**
  * @brief  send current which pid calculate to esc. message to calibrate 6025 gimbal motor esc
  * @param  current value corresponding motor(yaw/pitch/trigger)
  */
void send_gimbal_cur(int16_t yaw_iq, int16_t pit_iq, int16_t trigger_iq){
	//printf("gim_cur_sent: yaw %d  pit %d  trigger %d  T %d\r\n",yaw_iq,pit_iq,trigger_iq,osKernelSysTick());
  GIMBAL_CAN.pTxMsg->StdId   = 0x1ff;
  GIMBAL_CAN.pTxMsg->IDE     = CAN_ID_STD;
  GIMBAL_CAN.pTxMsg->RTR     = CAN_RTR_DATA;
  GIMBAL_CAN.pTxMsg->DLC     = 8;
  GIMBAL_CAN.pTxMsg->Data[0] = yaw_iq >> 8;
  GIMBAL_CAN.pTxMsg->Data[1] = yaw_iq;
  GIMBAL_CAN.pTxMsg->Data[2] = pit_iq >> 8;
  GIMBAL_CAN.pTxMsg->Data[3] = pit_iq;
  GIMBAL_CAN.pTxMsg->Data[4] = trigger_iq >> 8;
  GIMBAL_CAN.pTxMsg->Data[5] = trigger_iq;
  GIMBAL_CAN.pTxMsg->Data[6] = 0;
  GIMBAL_CAN.pTxMsg->Data[7] = 0;
  HAL_CAN_Transmit(&GIMBAL_CAN, 10);
}

/**
  * @brief  send calculated current to motor
  * @param  3510 motor ESC id
  */
void send_chassis_cur(int16_t iq1, int16_t iq2, int16_t iq3, int16_t iq4){
	//printf("sending current to chassis %d %d %d %d T \n\r",iq1,iq2,iq3,iq4);
  CHASSIS_CAN.pTxMsg->StdId   = 0x200;
  CHASSIS_CAN.pTxMsg->IDE     = CAN_ID_STD;
  CHASSIS_CAN.pTxMsg->RTR     = CAN_RTR_DATA;
  CHASSIS_CAN.pTxMsg->DLC     = 0x08;
  CHASSIS_CAN.pTxMsg->Data[0] = iq1 >> 8;
  CHASSIS_CAN.pTxMsg->Data[1] = iq1;
  CHASSIS_CAN.pTxMsg->Data[2] = iq2 >> 8;
  CHASSIS_CAN.pTxMsg->Data[3] = iq2;
  CHASSIS_CAN.pTxMsg->Data[4] = iq3 >> 8;
  CHASSIS_CAN.pTxMsg->Data[5] = iq3;
  CHASSIS_CAN.pTxMsg->Data[6] = iq4 >> 8;
  CHASSIS_CAN.pTxMsg->Data[7] = iq4;
  HAL_CAN_Transmit(&CHASSIS_CAN, 10);
}

#if 0
void CAN1_Configuration(void)
{
    CAN_InitTypeDef        can;
    CAN_FilterInitTypeDef  can_filter;
    GPIO_InitTypeDef       gpio;
    NVIC_InitTypeDef       nvic;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);

    GPIO_PinAFConfig(GPIOA, GPIO_PinSource11, GPIO_AF_CAN1);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource12, GPIO_AF_CAN1);

    gpio.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12;
    gpio.GPIO_Mode = GPIO_Mode_AF;
    GPIO_Init(GPIOA, &gpio);
    
    nvic.NVIC_IRQChannel = CAN1_RX0_IRQn;
    nvic.NVIC_IRQChannelPreemptionPriority = 0;
    nvic.NVIC_IRQChannelSubPriority = 2;
    nvic.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic);
    
//    nvic.NVIC_IRQChannel = CAN1_TX_IRQn;
//    nvic.NVIC_IRQChannelPreemptionPriority = 1;
//    nvic.NVIC_IRQChannelSubPriority = 1;
//    nvic.NVIC_IRQChannelCmd = ENABLE;
//    NVIC_Init(&nvic); 
    
    CAN_DeInit(CAN1);
    CAN_StructInit(&can);
    
    can.CAN_TTCM = DISABLE;
    can.CAN_ABOM = DISABLE;
    can.CAN_AWUM = DISABLE;
    can.CAN_NART = DISABLE;
    can.CAN_RFLM = DISABLE;
    can.CAN_TXFP = ENABLE;
    can.CAN_Mode = CAN_Mode_Normal;
    can.CAN_SJW  = CAN_SJW_1tq;
    can.CAN_BS1 = CAN_BS1_9tq;
    can.CAN_BS2 = CAN_BS2_4tq;
    can.CAN_Prescaler = 3;   //CAN BaudRate 42/(1+9+4)/3=1Mbps
    CAN_Init(CAN1, &can);

    can_filter.CAN_FilterNumber=0;
    can_filter.CAN_FilterMode=CAN_FilterMode_IdMask;
    can_filter.CAN_FilterScale=CAN_FilterScale_32bit;
    can_filter.CAN_FilterIdHigh=0x0000;
    can_filter.CAN_FilterIdLow=0x0000;
    can_filter.CAN_FilterMaskIdHigh=0x0000;
    can_filter.CAN_FilterMaskIdLow=0x0000;
    can_filter.CAN_FilterFIFOAssignment=0;//the message which pass the filter save in fifo0
    can_filter.CAN_FilterActivation=ENABLE;
    CAN_FilterInit(&can_filter);
    
    CAN_ITConfig(CAN1,CAN_IT_FMP0,ENABLE);
}

void CAN1_RX0_IRQHandler(void)
{
    CanRxMsg rx_message;    
    
    if (CAN_GetITStatus(CAN1,CAN_IT_FMP0)!= RESET) 
	{

       if(rx_message.StdId == 0x401)
        { 
            gyro_ok_flag = 1;
            temp_yaw_angle = (int32_t)(rx_message.Data[0]<<24)|(int32_t)(rx_message.Data[1]<<16) 
            | (int32_t)(rx_message.Data[2]<<8) | (int32_t)(rx_message.Data[3]);
            
            last_yaw_angle = this_yaw_angle;
            this_yaw_angle = -((float)temp_yaw_angle*0.01);            
        }
    }
}

#endif

