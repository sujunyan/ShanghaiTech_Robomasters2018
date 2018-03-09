
#include "test.h"
#include "sys_config.h"
#include "RemoteTask.h"
UBaseType_t test_task_stack_surplus;
uint32_t test_wake_time;
void test_task( const void* argu){
	
	 int flag=0;
	  test_wake_time= osKernelSysTick();
  while(1)
  {
    //imu_time_ms = HAL_GetTick() - imu_time_last;
    //imu_time_last = HAL_GetTick();
    if(flag==0)
		{
			LED_G_ON;
			flag=1;
		}
		else 
		{
			LED_G_OFF;
			flag=0;
		}
		
		
		
      //IMU_Get_Data();
		//test_motor();
		//testRemoteTask();
		//printf("Hello World\n\r");
		
    test_task_stack_surplus = uxTaskGetStackHighWaterMark(NULL);
    
    osDelayUntil(&test_wake_time, TEST_TASK_PERIOD);  
  }
}
