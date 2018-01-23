/**
  *@file test_uart.c
  *@date 2016-12-12
  *@author Albert.D
  *@brief 
  */

#include "test_uart.h"
#include "usart.h"
#include "RemoteTask.h"
#include "Serial_Debug.h"
uint8_t uart3_rx_buff[50];
uint8_t uart2_rx_buff[50];
uint8_t uart6_rx_buff[50];
uint8_t RemoteData[RC_FRAME_LENGTH]={0};

//it will be auto callback when usart receive msg completely
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
//	uint8_t test_string[]="hello world";
  if(huart == &huart6)
  {
    //HAL_UART_Transmit(huart, uart6_rx_buff, 1, 100);
    __HAL_UART_CLEAR_PEFLAG(&huart6);
    HAL_UART_Receive_IT(&huart6, uart6_rx_buff, 1);
		//HAL_UART_Transmit_IT(&huart6,test_string,sizeof(test_string));
		//HAL_UART_Transmit_IT(&huart6,uart6_rx_buff,1);
  }
  if(huart == &huart2)
  {
    __HAL_UART_CLEAR_PEFLAG(&huart2);
    HAL_UART_Receive_IT(&huart2, uart2_rx_buff, sizeof(uart2_rx_buff));
		//HAL_UART_Transmit_IT(&huart2,test_string,sizeof(test_string));
		//HAL_UART_Transmit_IT(&huart3,uart3_rx_buff,sizeof(uart3_rx_buff));
  }
	if(huart == &huart1) //  Recieve data from 
  {
		SEGGER_RTT_printf(0,"USART1 Recieved\n");
		LED_Green_Toggle();
    __HAL_UART_CLEAR_PEFLAG(&huart1);
		
    HAL_UART_Receive(&huart1, RemoteData, sizeof(RemoteData),10);
		
  }
}



