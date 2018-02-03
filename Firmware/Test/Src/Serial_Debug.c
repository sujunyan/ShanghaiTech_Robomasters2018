#include "Serial_Debug.h"
#include "test_app.h"
#include "test_imu.h"
extern IMUDataTypedef imu_data;
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;
extern UART_HandleTypeDef huart6;
extern DMA_HandleTypeDef hdma_usart1_rx;
void testSerialDebug(void)
{
	static int cnt=0;
	//printf("imu_data:ax %d, ay %d, az %d\n\r",imu_data.ax,imu_data.ay,imu_data.az);
	printf("%d\r\n",cnt++);
	//SEGGER_RTT_printf(0,"imu_data:ax %d, ay %d, az %d\n\r",imu_data.ax,imu_data.ay,imu_data.az);
}

void checkUART()
{
	if(huart1.gState==HAL_UART_STATE_READY &&
		huart1.RxState==HAL_UART_STATE_READY)
		printf("uart1 Ready");
	else 
		printf("uart1 not Ready. gState=%d RxState=%d \r\n", huart1.gState,huart1.RxState);
}

/*
	Redirect the printf function to SWO
*/
#ifdef DEBUGSWD


int fputc(int ch, FILE *f) {
  //ITM_SendChar(ch);
	SEGGER_RTT_printf(0,(char *)&ch);
  return(ch);
}




int fputs(const char * s , FILE * f)//redirect the printf function
{
	HAL_UART_Transmit(&huart2,(uint8_t *)&s,sizeof(s),10);
	return 1;
}


int fgetc(FILE *f)  //  redirect the scanf function
{ 
	uint8_t ch;
	HAL_UART_Receive_IT(&huart2, &ch, 1);
	return ch;
}

#endif


/*
	Redirect the printf function to serial uart2
*/

#ifdef DEBUGSERIAAL

int fputc(int ch, FILE *f)   //  redirect the printf function
{
		HAL_UART_Transmit(&huart2,(uint8_t *)&ch,sizeof(ch),1);
    return ch;
}

int fputs(const char * s , FILE * f)//redirect the printf function
{
	HAL_UART_Transmit(&huart2,(uint8_t *)&s,sizeof(s),1);
	return 1;
}


int fgetc(FILE *f)  //  redirect the scanf function
{ 
	uint8_t ch;
	HAL_UART_Receive_IT(&huart2, &ch, 1);
	return ch;
}

#endif


void Send_Debug_Msg(uint8_t* Msg,uint16_t Size){}

