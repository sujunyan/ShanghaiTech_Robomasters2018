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

#define SERIAL_DEBUG_PERIOD 20 

uint8_t serial_debug_buffer[1000];
uint8_t serial_debug_size=0;
UBaseType_t serial_debug_stack_surplus;
void serial_debug_task(void const *argu)
{
  
  uint32_t wake_time = osKernelSysTick();
  while(1)
  {
    if(serial_debug_size>0)
			write_uart_blocking(&COMPUTER_HUART,serial_debug_buffer,serial_debug_size);
    
    serial_debug_stack_surplus = uxTaskGetStackHighWaterMark(NULL);
    
    osDelayUntil(&wake_time, SERIAL_DEBUG_PERIOD);  
  }

}

int fputc(int ch, FILE *f) {
	serial_debug_buffer[serial_debug_size++]=ch;
  return(ch);
}
