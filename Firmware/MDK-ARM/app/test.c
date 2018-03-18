#include "test.h"
#include "sys_config.h"
#include "RemoteTask.h"
#include "imu_task.h"
#include "gimbal_task.h"
#include "chassis_task.h"
UBaseType_t test_task_stack_surplus;
uint32_t test_wake_time;
extern float can2_buf[3];
void test_task( const void* argu){
	
	
	  test_wake_time= osKernelSysTick();
  while(1)
  {
		//used in calibrate
		#ifndef CALI_DONE
		update_gimbal_sensor();
		printf("pit_relative ecd %f imu %f , yaw_relative ecd %f imu %f \r\n",
		gim.sensor.pit_relative_angle_ecd,
		gim.sensor.pit_relative_angle_imu,
		gim.sensor.yaw_relative_angle_ecd,
		gim.sensor.yaw_relative_angle_imu);
		printf("imu offset pit %f yaw %f \r\n",gim.sensor.pit_offset_angle_imu,gim.sensor.yaw_offset_angle_imu);
		
		printf("encoder: yaw %d pitch %d  \r\n",gim.yaw_motor.ecd,gim.pit_motor.ecd);
		printf("atti is pitch: %f yaw: %f roll: %f\r\n",atti.pitch,atti.yaw,atti.roll);
		//printf("mpu  gx %d gy %d gz:%d \r\n",mpu_data.gx,mpu_data.gy,mpu_data.gz);		
		#endif
		
		//LED_G_TOGGLE;
		
		
		
     //printf("can2_buf is %f %f %f \r\n",can2_buf[0],can2_buf[1],can2_buf[2]);
		
		
    test_task_stack_surplus = uxTaskGetStackHighWaterMark(NULL);
    
    osDelayUntil(&test_wake_time, TEST_TASK_PERIOD);  
  }
}
