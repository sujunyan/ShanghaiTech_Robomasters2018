/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2018 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"

/* USER CODE BEGIN Includes */     
#include "test.h"
#include "sys_config.h"
#include "serial_debug.h"
#include "detect_task.h"
#include "chasis_task.h"
/* USER CODE END Includes */

/* Variables -----------------------------------------------------------------*/
osThreadId defaultTaskHandle;

/* USER CODE BEGIN Variables */
TaskHandle_t shot_task_t;
TaskHandle_t can_msg_send_task_t;

TaskHandle_t detect_task_t;
TaskHandle_t imu_task_t;

TaskHandle_t serial_debug_task_t;
TaskHandle_t test_task_t;


osTimerId chassis_timer_id;
osTimerId gimbal_timer_id;
//osTimerId serial_debug_timer_id;

volatile int stack_over_flow_warning = 0;
/* USER CODE END Variables */

/* Function prototypes -------------------------------------------------------*/
//void StartDefaultTask(void * argument);

extern void MX_FATFS_Init(void);
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

/* Hook prototypes */
void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName);

/* USER CODE BEGIN 4 */
__weak void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName)
{
   /* Run time stack overflow checking is performed if
   configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2. This hook function is
   called if a stack overflow is detected. */
#if 0
    if (xTask == chasis_task_t)
      stack_over_flow_warning = 1;
    else if (xTask == gimbal_task_t)
      stack_over_flow_warning = 2;
    else if (xTask == detect_task_t)
      stack_over_flow_warning = 3;
    else if (xTask == record_task_t)
      stack_over_flow_warning = 4;
    else if (xTask == imu_task_t)
      stack_over_flow_warning = 5;
    else if (xTask == freq_info_task_t)
      stack_over_flow_warning = 6;
    else if (xTask == mode_sw_task_t)
      stack_over_flow_warning = 7;
    else if (xTask == info_get_task_t)
      stack_over_flow_warning = 8;
    else
      stack_over_flow_warning = 9;
#endif
    while (1)
    {
    }
}
/* USER CODE END 4 */

/* Init FreeRTOS */

void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
       
  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  
    /* real time control task */
  taskENTER_CRITICAL();
  
	#if 0
    osTimerDef(chassisTimer, chasis_task);
    chassis_timer_id = osTimerCreate(osTimer(chassisTimer), osTimerPeriodic, NULL);  // 10 ms
		osTimerStart(chassis_timer_id, CHASSIS_TASK_PERIOD);
	#endif
		
  /* 
    osTimerDef(gimTimer, gimbal_task);
    gimbal_timer_id = osTimerCreate(osTimer(gimTimer), osTimerPeriodic, NULL); // 5 ms
  */
		
  /* USER CODE END RTOS_TIMERS */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
	#if 0
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);
	#endif

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  
    /* high priority task */
   
    
    /* low priority task */
		
	#if 0
    osThreadDef(errTask, detect_task, osPriorityNormal, 0, 128);  // 50 ms
    detect_task_t = osThreadCreate(osThread(errTask), NULL);
	#endif
	
	#if 1
		osThreadDef(testTask, test_task, osPriorityNormal, 0, 1024);  // 50 ms
    test_task_t = osThreadCreate(osThread(testTask), NULL);
	#endif
	
	#if 0
		osThreadDef(serialDebugTask, serial_debug_task, osPriorityNormal, 0, 512);  // 20 ms
    serial_debug_task_t = osThreadCreate(osThread(serialDebugTask), NULL);
	
	#endif
	
  taskEXIT_CRITICAL();
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */
}

/* StartDefaultTask function */


/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
