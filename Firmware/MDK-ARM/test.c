
#include "test.h"
#include "sys_config.h"

UBaseType_t test_task_stack_surplus;

void test_task(const void* argu){
	
	 uint32_t wake_time = osKernelSysTick();
  while(1)
  {
    //imu_time_ms = HAL_GetTick() - imu_time_last;
    //imu_time_last = HAL_GetTick();
    
		
      //IMU_Get_Data();
		test_motor();
		testRemoteTask();
		
		
    test_task_stack_surplus = uxTaskGetStackHighWaterMark(NULL);
    
    osDelayUntil(&wake_time, TEST_TASK_PERIOD);  
  }
}
