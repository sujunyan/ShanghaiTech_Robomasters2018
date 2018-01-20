#ifndef _SERIAL_DEBUG_H
#define _SERIAL_DEBUG_H

#include "test_uart.h"
#include "test_can.h"
#include "test_app.h"
#include "usart.h"
#include "stdio.h"
typedef enum
{
   CM_MSG,     	
}DEBUG_msg;

void Send_Debug_Msg(uint8_t* Msg,uint16_t Size);
int fputc(int ch, FILE *f);  //  redirect the printf function
int fgetc(FILE *f);  //  redirect the scanf function
int fputs(const char * s /*s*/, FILE * f/*stream*/);//  redirect the printf function
void test_serial();
#endif
