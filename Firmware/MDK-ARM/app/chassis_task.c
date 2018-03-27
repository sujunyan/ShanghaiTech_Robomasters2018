#include "chassis_task.h"
#include "can.h"
#include "RemoteTask.h"
#include "pid.h"
#include "sys_config.h"
#include "bsp_can.h"
#include "gimbal_task.h"
#include "infantry_info.h"
chassis_t chassis;
UBaseType_t chasis_task_stack_surplus;
extern TaskHandle_t can_msg_send_task_t;

void chassis_task(const void* argu){ // timer

	
	// TODO swich the mode to handle data from PC 
	chassis_mode_switch();
	//chassis.ctrl_mode=MANUAL_FOLLOW_GIMBAL;
	
	// chassis follow the gimbal
	if (chassis_is_follow()){
	pid_calc(&pid_chassis_angle, - gim.sensor.yaw_relative_angle_ecd , 0);
	chassis.vw = pid_chassis_angle.out;
	}
	else{
		chassis.vw = pc_rece_mesg.chassis_control_data.w_info.w_speed;
	}
	
	if(!chassis_is_auto())chasis_remote_handle();
	
	
  mecanum_calc(chassis.vx, chassis.vy, chassis.vw, chassis.wheel_speed_ref);
  
  if (!chassis_is_controllable())
  {
    memset(chassis.current, 0, sizeof(chassis.current));
  }
	else
	{
		for (int i = 0; i < 4; i++)
		{
			chassis.wheel_speed_fdb[i] = chassis.motor[i].speed_rpm;		
			chassis.current[i] = pid_calc(&pid_spd[i], chassis.wheel_speed_fdb[i], chassis.wheel_speed_ref[i]);
		}
	}
	//TODO ban chassis for test 
	
	//memset(chassis.current, 0, sizeof(chassis.current));
	
		osSignalSet(can_msg_send_task_t, CHASSIS_MOTOR_MSG_SEND);
		chasis_task_stack_surplus = uxTaskGetStackHighWaterMark(NULL);
}






void encoder_data_handle(CAN_HandleTypeDef* hcan,moto_measure_t* ptr){
	
	ptr->last_ecd = ptr->ecd;
  ptr->ecd      = (uint16_t)(hcan->pRxMsg->Data[0] << 8 | hcan->pRxMsg->Data[1]);
  
  if (ptr->ecd - ptr->last_ecd > 4096)
  {
    ptr->round_cnt--;
    ptr->ecd_raw_rate = ptr->ecd - ptr->last_ecd - 8192;
  }
  else if (ptr->ecd - ptr->last_ecd < -4096)
  {
    ptr->round_cnt++;
    ptr->ecd_raw_rate = ptr->ecd - ptr->last_ecd + 8192;
  }
  else
  {
    ptr->ecd_raw_rate = ptr->ecd - ptr->last_ecd;
  }

  ptr->total_ecd = ptr->round_cnt * 8192 + ptr->ecd - ptr->offset_ecd;
  /* total angle, unit is degree */
  ptr->total_angle = ptr->total_ecd * ENCODER_ANGLE_RATIO;
  

  ptr->speed_rpm     = (int16_t)(hcan->pRxMsg->Data[2] << 8 | hcan->pRxMsg->Data[3]);
  ptr->given_current = (int16_t)(hcan->pRxMsg->Data[4] << 8 | hcan->pRxMsg->Data[5]);
	
	
}



/*协议定义：
* ch0 右摇杆 364-1024-1684 左->右
* ch1 右摇杆 364-1024-1684 下->上
*	ch2 左摇杆 364-1024-1684 左->右
* ch3 左摇杆 364-1024-1684 下->上
*  s1 left  switch  上-中-下  1-3-2
*  s2 right ... 
*/




uint8_t chassis_is_controllable(void){
  if (chassis.ctrl_mode == CHASSIS_RELAX 
	 ||	gim.ctrl_mode ==  GIMBAL_INIT
   || g_err.list[REMOTE_CTRL_OFFLINE].err_exist
	|| g_err.list[CHASSIS_M1_OFFLINE].err_exist
	|| g_err.list[CHASSIS_M2_OFFLINE].err_exist
	|| g_err.list[CHASSIS_M3_OFFLINE].err_exist
	|| g_err.list[CHASSIS_M4_OFFLINE].err_exist)
	{
		//printf("chssis not controllable!\n\r");
    return 0;
	}
  else
    return 1;
}

/**
  * @brief mecanum chassis velocity decomposition
  * @param input : forward=+vy(mm/s)  leftward =+vx(mm/s)  couter-clockwise=+vw(deg/s)
  *        output: every wheel speed(rpm)
  * @note  1=FR 2=BR 3=BL 4=FL
  */
void mecanum_calc(float vx, float vy, float vw, int16_t speed[]){
  static float rotate_ratio_fr=((WHEELBASE+WHEELTRACK)/2.0f)/RADIAN_COEF;
  static float rotate_ratio_fl=((WHEELBASE+WHEELTRACK)/2.0f)/RADIAN_COEF;
  static float rotate_ratio_bl=((WHEELBASE+WHEELTRACK)/2.0f)/RADIAN_COEF;
  static float rotate_ratio_br=((WHEELBASE+WHEELTRACK)/2.0f)/RADIAN_COEF;
  static float wheel_rpm_ratio = 60.0f/(PERIMETER*CHASSIS_DECELE_RATIO);
  
  
  VAL_LIMIT(vx, -MAX_CHASSIS_VX_SPEED, MAX_CHASSIS_VX_SPEED);  //mm/s
  VAL_LIMIT(vy, -MAX_CHASSIS_VY_SPEED, MAX_CHASSIS_VY_SPEED);  //mm/s
  VAL_LIMIT(vw, -MAX_CHASSIS_VR_SPEED, MAX_CHASSIS_VR_SPEED);  //deg/s
  
  int16_t wheel_rpm[4];
  float   max = 0;
  
  wheel_rpm[2] = ( vx + vy + vw * rotate_ratio_fr) * wheel_rpm_ratio;   //  back- left
  wheel_rpm[3] = ( -vx + vy + vw * rotate_ratio_fl) * wheel_rpm_ratio;	 // forward- left
	// these wheels are reversed due to sysmetry
  wheel_rpm[0] = ( -vx - vy + vw * rotate_ratio_bl) * wheel_rpm_ratio;  // forward right
  wheel_rpm[1] = ( vx - vy + vw * rotate_ratio_br) * wheel_rpm_ratio;		// back -right

  //find max item 
  for (uint8_t i = 0; i < 4; i++)
  {
    if (abs(wheel_rpm[i]) > max)
      max = abs(wheel_rpm[i]);
  }
  //equal proportion
  if (max > MAX_WHEEL_RPM)
  {
    float rate = MAX_WHEEL_RPM / max;
    for (uint8_t i = 0; i < 4; i++)
      wheel_rpm[i] *= rate;
  }
  memcpy(speed, wheel_rpm, 4*sizeof(int16_t));
}

/*协议定义：
* ch0 右摇杆 364-1024-1684 左->右
* ch1 右摇杆 364-1024-1684 下->上
*	ch2 左摇杆 364-1024-1684 左->右
* ch3 左摇杆 364-1024-1684 下->上
*  s1 left  switch  上-中-下  1-3-2
*  s2 right ... 
*/
 
void chassis_mode_switch(void){
	if(remote_info.rc.s1 == RC_DN && remote_info.rc.s2 == RC_DN )chassis.ctrl_mode=AUTO_SEPARATE_GIMBAL;
	else chassis.ctrl_mode=MANUAL_FOLLOW_GIMBAL;
}

	

void chasis_remote_handle(void){
	if(is_keyboard_mode())
	{
		float ratio;
		if (KEY_SHIFT)ratio=0.7 ;
		else if (KEY_CTRL) ratio=1;
		else ratio=0.5;
		
		if(KEY_A)chassis.vx =  CHASSIS_KB_MAX_SPEED_X * ratio; // left-right
		else if(KEY_D)chassis.vx = - CHASSIS_KB_MAX_SPEED_X * ratio;
		else chassis.vx=0;
		
		if(KEY_W)chassis.vy =  CHASSIS_KB_MAX_SPEED_Y * ratio; //   forward-backward 
		else if (KEY_S)chassis.vy =  -CHASSIS_KB_MAX_SPEED_Y * ratio;
		else chassis.vy=0;
		
		//chassis.vw  = remote_info.mouse.x* CHASSIS_KB_MAX_SPEED_R *ratio; // rotate 
	}
	else
		{
			chassis.vx = (- remote_info.rc.ch0 / RC_RESOLUTION * CHASSIS_RC_MAX_SPEED_X) 
										+ pc_rece_mesg.chassis_control_data.x_speed; // left-right
			chassis.vy =  (remote_info.rc.ch1 / RC_RESOLUTION * CHASSIS_RC_MAX_SPEED_Y)
										+ pc_rece_mesg.chassis_control_data.y_speed; //  forward-backward 		
			//chassis.vw  =   remote_info.rc.ch2 / RC_RESOLUTION * CHASSIS_RC_MAX_SPEED_R; // rotate 
		}
		
}
uint8_t chassis_is_auto(void){
	//return (remote_info.rc.s1==RC_DN);
	return (chassis.ctrl_mode==AUTO_SEPARATE_GIMBAL || chassis.ctrl_mode==AUTO_SEPARATE_GIMBAL); //TODO
}

uint8_t chassis_is_follow(void){
	return (chassis.ctrl_mode==MANUAL_FOLLOW_GIMBAL || chassis.ctrl_mode==AUTO_FOLLOW_GIMBAL);
}
