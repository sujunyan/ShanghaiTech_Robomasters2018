#include "Serial_Debug.h"
#include "test_app.h"
#include "test_imu.h"
extern IMUDataTypedef imu_data;
void Send_Debug_Msg(uint8_t* Msg,uint16_t Size)
{
	while(HAL_UART_Transmit_IT(&huart2,Msg,Size)==HAL_BUSY);
}



//int CNT=0;
int fputc(int ch, FILE *f)   //  redirect the printf function
{
		HAL_UART_Transmit(&huart2,(uint8_t *)&ch,sizeof(ch),10);
    return ch;
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

void test_serial()
{
	printf("imu_data:ax %d, ay %d, az %d\n\r",imu_data.ax,imu_data.ay,imu_data.az);
}

