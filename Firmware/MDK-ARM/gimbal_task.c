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
/** @file gimbal_task.c
 *  @version 1.1
 *  @date Oct 2017
 *
 *  @brief gimbal control task
 *
 *  @copyright 2017 DJI RoboMaster. All rights reserved.
 *
 */

#include "gimbal_task.h"
#include "chassis_task.h"
#include "bsp_uart.h"
#include "keyboard.h"
#include "pid.h"
#include "sys_config.h"
#include "ramp.h"
#include "cmsis_os.h"
#include "math.h"
#include "stdlib.h"
#include "string.h"
#include "imu_task.h"
#include "detect_task.h"

//#define OLD_TRIGGER
/* gimbal patrol angle (degree)*/
#define PATROL_ANGLE     40
/* patrol period time (ms) */
#define PATROL_PERIOD    1500
/* gimbal back center time (ms) */
#define BACK_CENTER_TIME 2500
#define PIT_ECD_DIR  (- 1.0f) 
#define YAW_ECD_DIR  (- 1.0f)
#define YAW_IMU_DIR  (- 1.0f)
/* keyboard mode gimbal speed limit */

#define GIMBAL_PC_MOVE_RATIO_PIT 0.1f 
#define GIMBAL_PC_MOVE_RATIO_YAW -1.0f 


/* stack usage monitor */
UBaseType_t gimbal_stack_surplus;

/* for debug */
//int yaw_a_fdb;
//int yaw_a_ref;
//int yaw_s_fdb;
//int yaw_s_ref;

//int pit_a_fdb;
//int pit_a_ref;
//int pit_s_fdb;
//int pit_s_ref;

/* gimbal task global parameter */
gimbal_t gim;

/* gimbal task static parameter */
/* shot buff parameter */

/* control ramp parameter */
static ramp_t     yaw_ramp = RAMP_GEN_DAFAULT;
static ramp_t     pit_ramp = RAMP_GEN_DAFAULT;

uint32_t gimbal_time_last;
int gimbal_time_ms;
uint32_t patrol_count;


extern TaskHandle_t can_msg_send_task_t;
extern TaskHandle_t shoot_task_t;
void gimbal_task(void const *argu)
{
 
	//printf("gimbal mode is %d \r\n",gim.ctrl_mode);
	
	//get sensor info
	update_gimbal_sensor();
	// calculate the pid refence
  switch (gim.ctrl_mode)
  {
    case GIMBAL_INIT:
      init_mode_handle();
    break;
    
    case GIMBAL_FOLLOW_ZGYRO:
      close_loop_handle();
    break;
		
    default:
			close_loop_handle();
    break;
  }

  
  
  pid_calc(&pid_yaw, gim.pid.yaw_angle_fdb, gim.pid.yaw_angle_ref);
  pid_calc(&pid_pit, gim.pid.pit_angle_fdb, gim.pid.pit_angle_ref);
  
	cascade_pid_ctrl();
  
  pid_calc(&pid_yaw_speed, gim.pid.yaw_speed_fdb, gim.pid.yaw_speed_ref + remote_info.mouse.x * GIMBAL_PC_MOVE_RATIO_YAW);
  pid_calc(&pid_pit_speed, gim.pid.pit_speed_fdb, gim.pid.pit_speed_ref);

  /* safe protect */
  if (gimbal_is_controllable())
  {
    gim.current[0] = pid_yaw_speed.out;
    gim.current[1] = pid_pit_speed.out;
  }
  else
  {
    memset(gim.current, 0, sizeof(gim.current));
    //gim.ctrl_mode = GIMBAL_RELAX;
    //pid_trigger.iout = 0;
  }
  
  //memset(gim.current, 0, sizeof(gim.current));
	
  osSignalSet(can_msg_send_task_t, GIMBAL_MOTOR_MSG_SEND);
  osSignalSet(shoot_task_t, SHOT_TASK_EXE_SIGNAL);
	//printf("gimbal_task done T %d\r\n",osKernelSysTick());
  gimbal_stack_surplus = uxTaskGetStackHighWaterMark(NULL);

}



float remote_ctrl_map(float offset,float step){
	static float out=0;
	out+=offset;
	if(out>step){
		out-=step;
		return step;
	}
	else{
		float tmp=out;
		out=0;
		return tmp;
	}

}



/*Ð­Òé¶¨Òå£º
* ch0 ÓÒÒ¡¸Ë 364-1024-1684 ×ó->ÓÒ
* ch1 ÓÒÒ¡¸Ë 364-1024-1684 ÏÂ->ÉÏ
*	ch2 ×óÒ¡¸Ë 364-1024-1684 ×ó->ÓÒ
* ch3 ×óÒ¡¸Ë 364-1024-1684 ÏÂ->ÉÏ
*  s1 left  switch  ÉÏ-ÖÐ-ÏÂ  1-3-2
*  s2 right ... 
*/
void close_loop_handle(void){
  static float chassis_angle_tmp=0;
  static float limit_angle_range = 3;
  static float step= GIMBAL_RC_MOVE_RATIO_YAW*660;
	
  gim.pid.pit_angle_fdb = gim.sensor.pit_relative_angle_ecd;
  gim.pid.yaw_angle_fdb = gim.sensor.yaw_relative_angle_imu; // 
  //TODO
	//gim.pid.yaw_angle_fdb = gim.sensor.yaw_relative_angle;
	
  /* chassis angle relative to gim.pid.yaw_angle_fdb */
  chassis_angle_tmp =  gim.pid.yaw_angle_ref ;
  /* limit gimbal yaw axis angle */
	
  if ((gim.pid.yaw_angle_fdb >= chassis_angle_tmp+ YAW_ANGLE_MIN - limit_angle_range) && \
      (gim.pid.yaw_angle_fdb <= chassis_angle_tmp+ YAW_ANGLE_MAX + limit_angle_range))
  {
    gim.pid.yaw_angle_ref += YAW_ECD_DIR*remote_ctrl_map( 
											 (KEY_Q)?(step):0 -  (KEY_E)?(-step):0 +
												-remote_info.rc.ch2 * GIMBAL_RC_MOVE_RATIO_YAW
                    //   + remote_info.mouse.x * GIMBAL_PC_MOVE_RATIO_YAW 
			,step);
    VAL_LIMIT(gim.pid.yaw_angle_ref, chassis_angle_tmp + YAW_ANGLE_MIN, chassis_angle_tmp + YAW_ANGLE_MAX);
  }
  /* limit gimbal pitch axis angle */
  if ((gim.sensor.pit_relative_angle_ecd >= PIT_ANGLE_MIN - limit_angle_range) && \
      (gim.sensor.pit_relative_angle_ecd <= PIT_ANGLE_MAX + limit_angle_range))
  {
    gim.pid.pit_angle_ref += - remote_info.rc.ch3 * GIMBAL_RC_MOVE_RATIO_PIT
                       + remote_info.mouse.y * GIMBAL_PC_MOVE_RATIO_PIT;
    VAL_LIMIT(gim.pid.pit_angle_ref, PIT_ANGLE_MIN, PIT_ANGLE_MAX);
  }
}






//ramp used in init model 
void gimbal_back_param(void){ 
  ramp_init(&pit_ramp, BACK_CENTER_TIME/GIMBAL_PERIOD);
  ramp_init(&yaw_ramp, BACK_CENTER_TIME/GIMBAL_PERIOD);
}


uint8_t gimbal_is_controllable(void){
	
  if (gim.ctrl_mode == GIMBAL_RELAX||
		g_err.list[REMOTE_CTRL_OFFLINE].err_exist ||
    g_err.list[GIMBAL_YAW_OFFLINE].err_exist ||
    g_err.list[GIMBAL_PIT_OFFLINE].err_exist)
	{
	#if 0
	printf("In gimbal_is_controllable %d %d %d %d\r\n",gim.ctrl_mode == GIMBAL_RELAX,
	g_err.list[REMOTE_CTRL_OFFLINE].err_exist,
	g_err.list[GIMBAL_YAW_OFFLINE].err_exist,
	g_err.list[GIMBAL_PIT_OFFLINE].err_exist);
	#endif	
    return 0;
	}
  else
    return 1;
}



void init_mode_handle(void){
	//printf("gimbal init mode handle\r\n");
  /* lift gimbal pitch */
	
	
  gim.pid.pit_angle_fdb = gim.sensor.pit_relative_angle_ecd;
  gim.pid.pit_angle_ref = gim.sensor.pit_relative_angle_ecd * (1 - ramp_calc(&pit_ramp)); // desired ref is 0
  
	/* keep yaw unmove this time */
  gim.pid.yaw_angle_fdb = gim.sensor.yaw_relative_angle_imu;
  gim.pid.yaw_angle_ref = gim.pid.yaw_angle_fdb;

  if(gim.pid.pit_angle_fdb >= -2.0f && gim.pid.pit_angle_fdb < 2.0f)
  {
    /* yaw back center after pitch arrive */
    gim.pid.yaw_angle_ref = gim.sensor.yaw_relative_angle_ecd * ( 1 - ramp_calc(&yaw_ramp));
    gim.pid.yaw_angle_fdb = gim.sensor.yaw_relative_angle_ecd;
		
    if (gim.pid.yaw_angle_fdb >= -1.5f && gim.pid.yaw_angle_fdb <= 1.5f)
    {
      /* yaw arrive and switch gimbal state */
      gim.ctrl_mode = GIMBAL_FOLLOW_ZGYRO;
      
			gim.sensor.yaw_offset_angle_imu =atti.yaw;
      //gim.yaw_offset_angle = gim.sensor.gyro_angle;
      gim.pid.pit_angle_ref = 0;
      gim.pid.yaw_angle_ref = 0;
    }
  }
}
void cascade_pid_ctrl(void){

  gim.pid.yaw_speed_ref = pid_yaw.out;
  gim.pid.pit_speed_ref = pid_pit.out;
 
  gim.pid.yaw_speed_fdb = gim.sensor.yaw_palstance;
  gim.pid.pit_speed_fdb = gim.sensor.pit_palstance;
}

/**
  * @brief     get relative position angle to center
  * @param[in] raw_ecd: gimbal motor encoder raw angle
  * @param[in] center_offset: read gim_cali_data from chip flash
  * @retval    relative angle, unit is degree.
  */
int16_t get_relative_pos(int16_t raw_ecd, int16_t center_offset){
  int16_t tmp = 0;
  if (center_offset >= 4096)
  {
    if (raw_ecd > center_offset - 4096)
      tmp = raw_ecd - center_offset;
    else
      tmp = raw_ecd + 8192 - center_offset;
  }
  else
  {
    if (raw_ecd > center_offset + 4096)
      tmp = raw_ecd - 8192 - center_offset;
    else
      tmp = raw_ecd - center_offset;
  }
  return tmp;
}


void update_gimbal_sensor(void){
	
	static float yaw_ecd_ratio = YAW_ECD_DIR*YAW_DECELE_RATIO*ENCODER_ANGLE_RATIO;
  static float pit_ecd_ratio = PIT_ECD_DIR*PIT_DECELE_RATIO*ENCODER_ANGLE_RATIO;
  gim.sensor.yaw_relative_angle_ecd =  yaw_ecd_ratio*get_relative_pos(gim.yaw_motor.ecd, YAW_ECD_CENTER_OFFSET);
  gim.sensor.pit_relative_angle_ecd =  pit_ecd_ratio*get_relative_pos(gim.pit_motor.ecd, PIT_ECD_CENTER_OFFSET);
	
	 //gim.sensor.yaw_relative_angle_imu updated in imu task
	
 // ecd offset update in bsp_can
	gim.sensor.yaw_relative_angle_imu = YAW_IMU_DIR*(atti.yaw - gim.sensor.yaw_offset_angle_imu);
	gim.sensor.pit_relative_angle_imu = (atti.roll- gim.sensor.pit_offset_angle_imu);
	
	 /* get gimbal relative palstance */
  gim.sensor.yaw_palstance = YAW_IMU_DIR* (-mpu_data.gz) / 16.384f; //unit: dps
  gim.sensor.pit_palstance = - mpu_data.gx / 16.384f; //unit: dps
}








#if 0
static void gimbal_patrol_handle(void){
  static int16_t patrol_period = PATROL_PERIOD/GIMBAL_PERIOD;
  static int16_t patrol_angle  = PATROL_ANGLE;
  
  gim.pid.pit_angle_fdb = gim.sensor.pit_relative_angle;
  gim.pid.yaw_angle_fdb = gim.sensor.yaw_relative_angle;
  
  patrol_count++;
  gim.pid.yaw_angle_ref = patrol_angle*sin(2*PI/patrol_period*patrol_count);
  gim.pid.pit_angle_ref = 0;
}

void pc_position_ctrl_handle(void){
  gim.pid.pit_angle_fdb = gim.sensor.pit_relative_angle;
  gim.pid.yaw_angle_fdb = gim.sensor.yaw_relative_angle;
  
  taskENTER_CRITICAL();
  gim.pid.pit_angle_ref = pc_rece_mesg.gimbal_control_data.pit_ref;
  gim.pid.yaw_angle_ref = pc_rece_mesg.gimbal_control_data.yaw_ref;
  taskEXIT_CRITICAL();
}


void no_action_handle(void){
  if (gim.input.no_action_flag == 1)
  {
    if ((HAL_GetTick() - gim.input.no_action_time) < 1500)
    {
      close_loop_handle();
    }
    else
    {
      gim.input.no_action_flag = 2;
      gim.pid.yaw_angle_ref = 0;
    }
  }
  
  if (gim.input.no_action_flag == 2)
  {
    gim.pid.pit_angle_fdb = gim.sensor.pit_relative_angle;
    gim.pid.yaw_angle_fdb = gim.sensor.yaw_relative_angle;
  }
}




#endif 

