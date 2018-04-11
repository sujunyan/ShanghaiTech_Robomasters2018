#include "test.h"
#include "sys_config.h"
#include "RemoteTask.h"
#include "imu_task.h"
#include "gimbal_task.h"
#include "chassis_task.h"
#include "shoot_task.h"
#include "PC_communication_task.h"
#define TEST_TASK_PERIOD 100
UBaseType_t test_task_stack_surplus;
uint32_t test_wake_time;
extern float can2_buf[3];
extern TaskHandle_t shoot_task_t;
extern TaskHandle_t can_msg_send_task_t;
void test_task( const void* argu){
	
	//osDelay(1000); // wait for imu and ecd to be stable
	  test_wake_time= osKernelSysTick();
	
  while(1)
  {
		//used in calibrate
		#ifndef CALI_DONE
		printf_cali_info();		
		#endif
		
		//LED_G_TOGGLE;		
		#if 0
		print_chassis_info();
		#endif
	
		#ifdef CHASSIS_ONLY
		err_detector_hook(GIMBAL_YAW_OFFLINE);
		err_detector_hook(GIMBAL_PIT_OFFLINE);
		err_detector_hook(TRIGGER_MOTO_OFFLINE);
		#endif
	//	test_shoot_task();
	
		// we do not have these module, try to get rid of it
		 
		err_detector_hook(GIMBAL_GYRO_OFFLINE);
    test_task_stack_surplus = uxTaskGetStackHighWaterMark(NULL);
    
    osDelayUntil(&test_wake_time, TEST_TASK_PERIOD);  
  }
}
void printf_cali_info(void){
	update_gimbal_sensor();
		printf("pit_relative ecd %f imu %f , yaw_relative ecd %f imu %f \r\n",
		gim.sensor.pit_relative_angle_ecd,
		gim.sensor.pit_relative_angle_imu,
		gim.sensor.yaw_relative_angle_ecd,
		gim.sensor.yaw_relative_angle_imu);
		printf("imu offset pit %f yaw %f \r\n",gim.sensor.pit_offset_angle_imu,gim.sensor.yaw_offset_angle_imu);
		
		printf("encoder: yaw %d pitch %d  \r\n",gim.yaw_motor.ecd,gim.pit_motor.ecd);
		printf("atti is pitch: %f yaw: %f roll: %f\r\n",atti.pitch,atti.yaw,atti.roll);
		printf("mpu  gx %d gy %d gz:%d \r\n",mpu_data.gx,mpu_data.gy,mpu_data.gz);		
}

void print_chassis_info(void){
	printf("chassis speed %d %d %d %d \r\n",chassis.motor[0].speed_rpm,
	chassis.motor[1].speed_rpm,chassis.motor[2].speed_rpm,chassis.motor[3].speed_rpm);
	
	printf("chassis delta %d %d %d %d \r\n",g_err.list[CHASSIS_M1_OFFLINE].dev->delta_time,
		g_err.list[CHASSIS_M2_OFFLINE].dev->delta_time,g_err.list[CHASSIS_M3_OFFLINE].dev->delta_time,g_err.list[CHASSIS_M4_OFFLINE].dev->delta_time);
	
}
