#include <stm32f4xx.h>
#include "RemoteTask.h"

#include "sys_config.h"
#include "bsp_uart.h"
RC_Ctl_t remote_info;   //remote control data
uint8_t RemoteData[DBUS_MAX_LEN];


void testRemoteTask(){
	static int cnt=0;
	static int cnt1=0;

	if(cnt++%1==0)
	{
	
	#if 1
	printf("RC_CtrlData: ch0 %d, ch1 %d, ch2 %d, ch3 %d, s1 %d, s2 %d times %d\r\n",
					remote_info.rc.ch0,remote_info.rc.ch1,
					remote_info.rc.ch2,remote_info.rc.ch3,
					remote_info.rc.s1,remote_info.rc.s2,cnt1++);
	#endif
		/*
		printf("Remote Raw Data:"); 
		for(int i=0;i<18;i++)printf("%d,",RemoteData[i]);
		printf("\n\r");
		*/
		
		// key board data test
		#if 1
		printf("KeyBoard Data: \r\n");
		printf("mouse: x: %d y: %d z: %d left:%d right %d \r\n",remote_info.mouse.x,remote_info.mouse.y,
			remote_info.mouse.z,remote_info.mouse.press_l,remote_info.mouse.press_r);
		printf("key: W %d S %d A %d D %d ctrl %d shift %d\r\n with cnt %d \r\n\r\n",KEY_W,KEY_S,KEY_A,KEY_D,KEY_CTRL,KEY_SHIFT,cnt);
		#endif
		
		
	}
}
/*协议定义：
* ch0 右摇杆 364-1024-1684 左->右
* ch1 右摇杆 364-1024-1684 下->上
*	ch2 左摇杆 364-1024-1684 左->右
* ch3 左摇杆 364-1024-1684 下->上
*  s1 left  switch  上-中-下  1-3-2
*  s2 right ... 
*/
 
void RemoteDataPrcess(uint8_t *pData){
    if(pData == NULL)
    { 
        return;
    }
		
    remote_info.rc.ch0 = ((int16_t)pData[0] | ((int16_t)pData[1] << 8)) & 0x07FF; 
		remote_info.rc.ch0 -=1024;
		
    remote_info.rc.ch1 = (((int16_t)pData[1] >> 3) | ((int16_t)pData[2] << 5)) & 0x07FF;
		remote_info.rc.ch1 -=1024;
		
    remote_info.rc.ch2 = (((int16_t)pData[2] >> 6) | ((int16_t)pData[3] << 2) |
													((int16_t)pData[4] << 10)) & 0x07FF;
		remote_info.rc.ch2 -=1024;
		
    remote_info.rc.ch3 = (((int16_t)pData[4] >> 1) | ((int16_t)pData[5]<<7)) & 0x07FF;
		remote_info.rc.ch3 -=1024;
		
		// if the data is not valid 
		if ((abs(remote_info.rc.ch0) > RC_RESOLUTION) || \
      (abs(remote_info.rc.ch1) > RC_RESOLUTION) || \
      (abs(remote_info.rc.ch2) > RC_RESOLUTION) || \
      (abs(remote_info.rc.ch3) > RC_RESOLUTION))
		{
			memset(&remote_info.rc, 0, sizeof(RC_Ctl_t));
			return ;
		}
    
    remote_info.rc.s1 = ((pData[5] >> 4) & 0x000C) >> 2;
    remote_info.rc.s2 = ((pData[5] >> 4) & 0x0003);

    remote_info.mouse.x = ((int16_t)pData[6]) | ((int16_t)pData[7] << 8);
    remote_info.mouse.y = ((int16_t)pData[8]) | ((int16_t)pData[9] << 8);
    remote_info.mouse.z = ((int16_t)pData[10]) | ((int16_t)pData[11] << 8);    

    remote_info.mouse.press_l = pData[12];
    remote_info.mouse.press_r = pData[13];
 
    remote_info.key_code = ((int16_t)pData[14]) | ((int16_t)pData[15] << 8);

}


extern UART_HandleTypeDef huart1;
void dbus_uart_init(void){
  //open uart idle it
  __HAL_UART_CLEAR_IDLEFLAG(&DBUS_HUART);
  __HAL_UART_ENABLE_IT(&DBUS_HUART, UART_IT_IDLE);

  UART_Receive_DMA_No_IT(&DBUS_HUART, RemoteData, DBUS_MAX_LEN);
}


int is_keyboard_mode(void){
	if(remote_info.mouse.x || remote_info.mouse.y || remote_info.mouse.z||
			remote_info.mouse.press_l||remote_info.mouse.press_r ||remote_info.key_code)
		return 1;
	else return 0;
}


/**
  * @brief   enable global uart it and do not use DMA transfer done it
  * @param   uart IRQHandler id, receive buff, buff size
  * @retval  set success or fail
  */
static int UART_Receive_DMA_No_IT(UART_HandleTypeDef* huart, uint8_t* pData, uint32_t Size)
{
  uint32_t tmp1 = 0;

  tmp1 = huart->RxState;
  if (tmp1 == HAL_UART_STATE_READY)
  {
    if ((pData == NULL) || (Size == 0))
    {
        return HAL_ERROR;
    }

    /* Process Locked */
    __HAL_LOCK(huart);

    huart->pRxBuffPtr = pData;
    huart->RxXferSize = Size;
    
    huart->ErrorCode  = HAL_UART_ERROR_NONE;

    /* Enable the DMA Stream */
    HAL_DMA_Start(huart->hdmarx, (uint32_t)&huart->Instance->DR,
                  (uint32_t)pData, Size);

    /* Enable the DMA transfer for the receiver request by setting the DMAR bit
    in the UART CR3 register */
    SET_BIT(huart->Instance->CR3, USART_CR3_DMAR);

    /* Process Unlocked */
    __HAL_UNLOCK(huart);

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

