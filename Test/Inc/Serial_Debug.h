#ifndef _SERIAL_DEBUG_H
#define _SERIAL_DEBUG_H

#include "test_uart.h"
#include "test_can.h"
#include "usart.h"
typedef enum
{
   CM_MSG,     	
}DEBUG_msg;

void Send_Debug_Msg(uint8_t* Msg,uint16_t Size);
#endif
