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
/** @file shoot_task.c
 *  @version 1.1
 *  @date June 2017
 *
 *  @brief shoot bullet task
 *
 *  @copyright 2017 DJI RoboMaster. All rights reserved.
 *
 */

#include "shoot_task.h"
#include "gimbal_task.h"
#include "detect_task.h"
#include "comm_task.h"
#include "modeswitch_task.h"
#include "remote_ctrl.h"
#include "bsp_io.h"
#include "bsp_can.h"
#include "bsp_uart.h"
#include "bsp_io.h"
#include "keyboard.h"
#include "pid.h"
#include "sys_config.h"
#include "cmsis_os.h"
#include "string.h"
#include "info_interactive.h"
#include "communicate.h"
#include "serial_debug.h"



UBaseType_t serial_debug_stack_surplus;

#define MAX_SERIAL_BUFFER_SIZE 1000
uint8_t serial_debug_buffer[MAX_SERIAL_BUFFER_SIZE];
uint16_t serial_debug_buffer_size=0;

void serial_debug_task(void const *argu)
{
  static uint32_t cnt=0;
  uint32_t wake_time = osKernelSysTick();
  while(1)
  {
		// TODO
		printf("test %d\n\r",cnt++);
		LED_G_ON;
		//if(serial_debug_buffer_size>0)
		{
			
			//write_uart_noblocking(&COMPUTER_HUART, buffer, sizeof(buffer));
			write_uart_noblocking(&COMPUTER_HUART, serial_debug_buffer, serial_debug_buffer_size);
			serial_debug_buffer_size=0;
		}
    
    serial_debug_stack_surplus = uxTaskGetStackHighWaterMark(NULL);
    
    osDelayUntil(&wake_time, SERIAL_DEBUG_PERIOD);  
  }

}

int fputc(int ch, FILE *f) {
	LED_G_ON;
	if(serial_debug_buffer_size<MAX_SERIAL_BUFFER_SIZE-1)
		serial_debug_buffer[serial_debug_buffer_size++]=ch;
	LED_G_OFF;
  return(ch);
	
}
/*
int fputs(const char * s , FILE * f)//redirect the printf function
{
	HAL_UART_Transmit(&COMPUTER_HUART,(uint8_t *)&s,sizeof(s),10);
	return 1;
}
*/

