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

uint32_t Timetick1ms = 0;
uint32_t Startup_Success_music_index = 0;
Test_ResultTypedef sTestResult = {0, 0, 0, 1, 0, 0};

uint8_t can1_tx_data[8] = {0};
uint8_t can2_tx_data[8] = {0};

/*total task used to check all,it will be used in timer 6 interrupt*/
void Test_task(void)
{
  Timetick1ms++;
  
  //can&imu can't init in 4s,led show error
  if(Timetick1ms > 4000)
  {
    if(sTestResult.imuTest == 0)
    {
      MPU_id = MPU6500_Read_Reg(MPU6500_WHO_AM_I);
      if(MPU_id == 0)
      {
        LED_Red_On();
        LED_Green_On();
      }
    }
   if(sTestResult.canTest == 0)
    {
      LED_Red_Toggle();
    }
  }
  
  //can&usrt send mag in evry 500ms
  if(Timetick1ms % 500 == 0)
  {
    if(sTestResult.imuTest == 0x01)
    {
      LED_Green_Toggle();
      LED_Red_Off();
    }
    
    can1_tx_data[0]++;
    can2_tx_data[0]++;
    CAN_Send_Msg(&hcan1, can1_tx_data, TEST_CAN1_ID, 8);
    CAN_Send_Msg(&hcan2, can2_tx_data, TEST_CAN2_ID, 8);
    
    HAL_UART_Transmit(&huart3, can1_tx_data, 1, 100);
    
  }
  
  //check the key 
  if(KEY_PRESS)
  {
    LED_Green_On();
    LED_Red_Off();
    sTestResult.keyTest = 0x01;
  }
  
  //if can&usrt receive msg,save they are well
  if(can1_rx_data[0]&&can2_rx_data[0])sTestResult.canTest = 0x01;
  if(uart3_rx_buff[0]&&uart6_rx_buff[0])sTestResult.uartTest = 0x01;
  
  if(sTestResult.canTest == 0x01 && sTestResult.imuTest == 0x01 && sTestResult.uartTest == 0x01
    && sTestResult.keyTest == 0x01 && sTestResult.usbTest == 0x01)
  sTestResult.allTest = 0x01;
  
  //if all the peripheral have checked
  if(sTestResult.allTest)
  {
    LED_Green_On();
    if(Timetick1ms % 80 == 0)
    {
      if(Startup_Success_music_index < Startup_Success_music_len)
        Sing_Startup_music(Startup_Success_music_index);
      Startup_Success_music_index++;
    }
  }
}
