
#include "test.h"
#include "sys_config.h"
#include "RemoteTask.h"
#include "imu_task.h"
#include "chassis_task.h"
UBaseType_t test_task_stack_surplus;
uint32_t test_wake_time;
void test_task( const void* argu){
	
	
	  test_wake_time= osKernelSysTick();
  while(1)
  {
		//LED_G_TOGGLE;
		
		//printf("atti is pitch: %f yaw: %f roll: %f\r\n",atti.pitch,atti.yaw,atti.roll);
		
     
		//testRemoteTask();
		//test_chassis();
		
    test_task_stack_surplus = uxTaskGetStackHighWaterMark(NULL);
    
    osDelayUntil(&test_wake_time, TEST_TASK_PERIOD);  
  }
}
