#include "Serial_Debug.h"
#include "sys_config.h"

UBaseType_t serial_debug_stack_surplus;

#define MAX_SERIAL_BUFFER_SIZE 10000
uint8_t serial_debug_buffer[MAX_SERIAL_BUFFER_SIZE];
uint16_t serial_debug_buffer_size=0;
void serial_debug_task(void const *argu)
{
//  static uint32_t cnt=0;
  uint32_t wake_time = osKernelSysTick();
  while(1)
  {
		// TODO
		#if 1
		//printf("test %d\n\r",cnt++);
		//LED_G_ON;
		if(serial_debug_buffer_size>0)
		{
			write_uart_noblocking(&PC_HUART, serial_debug_buffer, serial_debug_buffer_size);
			serial_debug_buffer_size=0;
		}
    #endif
    serial_debug_stack_surplus = uxTaskGetStackHighWaterMark(NULL);
    //LED_G_OFF;
    osDelayUntil(&wake_time, SERIAL_DEBUG_PERIOD);  
  }
}

void send_serial_debug_msg(void){
	if(serial_debug_buffer_size>0)
	{
			write_uart_noblocking(&PC_HUART, serial_debug_buffer, serial_debug_buffer_size);
			serial_debug_buffer_size=0;
	}
}

//  redirect the printf function
int fputc(int ch, FILE *f){   
	#ifdef SERIAL_DEBUG
	if(serial_debug_buffer_size<MAX_SERIAL_BUFFER_SIZE)
		serial_debug_buffer[serial_debug_buffer_size++]=ch;
	#endif
	//else MAX_SERIAL_BUFFER_SIZE=0;
   return ch;
}



void testSerialDebug(void){
	static int cnt=0;
	//printf("imu_data:ax %d, ay %d, az %d\n\r",imu_data.ax,imu_data.ay,imu_data.az);
	printf("%d\r\n",cnt++);
	//SEGGER_RTT_printf(0,"imu_data:ax %d, ay %d, az %d\n\r",imu_data.ax,imu_data.ay,imu_data.az);
}

void checkUART(){
	if(huart1.gState==HAL_UART_STATE_READY &&
		huart1.RxState==HAL_UART_STATE_READY)
		printf("uart1 Ready");
	else 
		printf("uart1 not Ready. gState=%d RxState=%d \r\n", huart1.gState,huart1.RxState);
}

void write_uart_noblocking(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size){
	HAL_UART_Transmit_IT(huart,pData,Size);
}
void write_uart_blocking(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size){
	HAL_UART_Transmit(huart,pData,Size,100);
}


