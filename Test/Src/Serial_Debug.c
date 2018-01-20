#include "Serial_Debug.h"

void Send_Debug_Msg(uint8_t* Msg,uint16_t Size)
{
	while(HAL_UART_Transmit_IT(&huart2,Msg,Size)==HAL_BUSY);
}
