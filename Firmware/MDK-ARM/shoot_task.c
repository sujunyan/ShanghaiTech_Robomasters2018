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
#include "RemoteTask.h"
#include "bsp_io.h"
#include "bsp_can.h"
#include "bsp_uart.h"
#include "bsp_io.h"
#include "keyboard.h"
#include "pid.h"
#include "sys_config.h"
#include "cmsis_os.h"
#include "string.h"
#include "pid.h"
/* stack usage monitor */
UBaseType_t shoot_stack_surplus;

/* shot task global parameter */
shoot_t   shoot;
trigger_t trig;

uint32_t shoot_time_last;
int shoot_time_ms;
void shoot_task(void const *argu)
{
  osEvent event;
  
  while (1)
  {
    event = osSignalWait(SHOT_TASK_EXE_SIGNAL, osWaitForever);
    
    if (event.status == osEventSignal)
    {
      if (event.value.signals & SHOT_TASK_EXE_SIGNAL)
      {
				#if 0
        shoot_time_ms = HAL_GetTick() - shoot_time_last;
        shoot_time_last = HAL_GetTick();
				printf("shoot time %d %d\r\n",shoot_time_ms,shoot_time_last);
				#endif
        
				shoot.fric_wheel_run = (remote_info.rc.s1 == RC_UP || pc_rece_mesg.shoot_control_data.fric_wheel_run);   
        //trig.key = get_trigger_key_state();
       
        if (shoot.fric_wheel_run && gimbal_is_controllable())
        {
					turn_on_friction_wheel(shoot.fric_wheel_spd);
					turn_on_laser();
					if(TRIG_RUN)pid_calc(&pid_trigger_speed,trig.motor.speed_rpm,trig.feed_bullet_spd);
					else pid_trigger_speed.out = 0;
          //shoot_bullet_handle();
        }
        else
        {
					turn_off_friction_wheel();
					turn_off_laser();
          pid_trigger_speed.out = 0;
        }
        
        trig.key_last = trig.key;

      }
    }
    
    shoot_stack_surplus = uxTaskGetStackHighWaterMark(NULL);
  }
}


void test_shoot_task(void){
	shoot.fric_wheel_run = (remote_info.rc.s1 == RC_UP && remote_info.rc.s2 == RC_UP);   
        //trig.key = get_trigger_key_state();
        
  if (shoot.fric_wheel_run)
  {
					turn_on_friction_wheel(1500);
					turn_on_laser();
					//printf("set fric speed %d \r\n",shoot.fric_wheel_spd);
          //shoot_bullet_handle();
  }
  else
  {
					turn_off_friction_wheel();
					turn_off_laser();
          pid_trigger_speed.out = 0;
 }
	
	
}
void block_bullet_handle(void){
  uint32_t stall_count = 0;
  uint32_t stall_inv_count = 0;
  uint8_t  stall_f = 0;
  
  if (pid_trigger_speed.out <= -4000)
  {
    if (stall_f == 0)
      stall_count ++;
  }
  else
    stall_count = 0;
  
  if (stall_count >= 50)         //0.25s
  {
    stall_f = 1;
    stall_count = 0;
  }
  
  if (stall_f == 1)
  {
    stall_inv_count++;
    
    if (stall_inv_count >= 100)  //0.5s
    {
      stall_f = 0;
      stall_inv_count = 0;
    }
    else
      trig.spd_ref = 2000;
  }
}








