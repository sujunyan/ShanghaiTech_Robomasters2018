/**
  *@file test_app.c
  *@date 2016-12-13
  *@author Albert.D
  *@brief 
  */
  
#include "test_app.h"
#include "test_beep.h"
#include "test_can.h"
#include "test_uart.h"
#include "test_imu.h"
#include "mpu6500_reg.h"
#include "can.h"
#include "usart.h"
#include "test_motor.h"
#include "test_imu.h"
#include "stdio.h"
#include "RemoteTask.h"
extern IMUDataTypedef imu_data;
uint32_t Timetick1ms = 0;
uint32_t Startup_Success_music_index = 0;
Test_ResultTypedef sTestResult = {0, 0, 0, 1, 0, 0};

uint8_t can1_tx_data[8] = {0};
uint8_t can2_tx_data[8] = {0};
int16_t cnt=0x0200;
extern uint8_t uart2_rx_buff[50];
//uint8_t test_string[]="";
/*total task used to check all,it will be used in timer 6 interrupt*/
void Test_task(void)
{
  Timetick1ms++;
	
  //if(cnt++ >0x300)cnt=0x0200;
  //can&imu can't init in 4s,led show error
  if(Timetick1ms > 4000)
  {
    if(sTestResult.imuTest == 0)
    {
      MPU_id = MPU6500_Read_Reg(MPU6500_WHO_AM_I);
			//MPU_id=1;
      if(MPU_id == 0)
      {
        //LED_Red_On();
        //LED_Green_On();
      }
    }
		
   if(sTestResult.canTest == 0)
    {
      //LED_Red_Toggle();
    }
  }
  
  //can&usrt send mag in evry 500ms
  if(Timetick1ms % 500 == 0)
  {	
		//LED_Red_Off();
		//LED_Green_Off();
		
    if(sTestResult.imuTest == 0x01)
    {
      //LED_Green_Toggle();
      //LED_Red_Off();
    }
//    
		//testSerialDebug();
		//uint8_t imu_data_list[]={imu_data.ax,imu_data.ay,imu_data.az};
		//Send_Debug_Msg(imu_data_list,sizeof(imu_data_list));
		//Set_CM_Speed(&hcan1,cnt,cnt,cnt,cnt);
    can1_tx_data[0]++;
    can2_tx_data[0]++;
    //CAN_Send_Msg(&hcan1, can1_tx_data, TEST_CAN1_ID, 8);
    //CAN_Send_Msg(&hcan2, can2_tx_data, TEST_CAN2_ID, 8);
    
		//HAL_UART_Receive_IT(&huart2, uart2_rx_buff, sizeof(uart2_rx_buff));
		//HAL_UART_Transmit_IT(&huart2,test_string,sizeof(test_string));
		//HAL_UART_Transmit_IT(&huart2,uart2_rx_buff,sizeof(uart2_rx_buff));
    //HAL_UART_Transmit(&huart3, can1_tx_data, 1, 100);
    
  }
  
  //check the key 
  if(KEY_PRESS)
  {
    //LED_Green_On();
    //LED_Red_Off();
    sTestResult.keyTest = 0x01;
  }
  
  //if can&usrt receive msg,save they are well
  if(can1_rx_data[0]&&can2_rx_data[0])sTestResult.canTest = 0x01;
  if(uart3_rx_buff[0]&&uart6_rx_buff[0])sTestResult.uartTest = 0x01;
	
	//debug..
  sTestResult.uartTest = 0x01;
	
  if(sTestResult.canTest == 0x01 && sTestResult.imuTest == 0x01 && sTestResult.uartTest == 0x01
    && sTestResult.keyTest == 0x01 && sTestResult.usbTest == 0x01)
  sTestResult.allTest = 0x01;
  
  //if all the peripheral have checked
  if(sTestResult.allTest)
  {
    //LED_Green_On();
    if(Timetick1ms % 80 == 0)
    {
      if(Startup_Success_music_index < Startup_Success_music_len)
        Sing_Startup_music(Startup_Success_music_index);
      Startup_Success_music_index++;
    }
  }
}
