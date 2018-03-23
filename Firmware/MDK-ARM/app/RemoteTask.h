#ifndef _REOMTE_TASK_H_
#define _REOMTE_TASK_H_
#include "main.h"
#include "ramp.h"
/*
****************************************************************************
*
*																	MARCO
****************************************************************************
*/
//remote control parameters
#define REMOTE_CONTROLLER_STICK_OFFSET      1024u   

#define STICK_TO_CHASSIS_SPEED_REF_FACT     0.45f
#define STICK_TO_PITCH_ANGLE_INC_FACT       0.004f
#define STICK_TO_YAW_ANGLE_INC_FACT         0.004f
#define FRICTION_WHEEL_MAX_DUTY             1600

//mouse control parameters
#define MOUSE_TO_PITCH_ANGLE_INC_FACT 		0.025f
#define MOUSE_TO_YAW_ANGLE_INC_FACT 		0.025f

#define NORMAL_FORWARD_BACK_SPEED 			500
#define NORMAL_LEFT_RIGHT_SPEED   			700
#define HIGH_FORWARD_BACK_SPEED 			660
#define HIGH_LEFT_RIGHT_SPEED   			800

#define FRICTION_RAMP_TICK_COUNT			100
#define MOUSE_LR_RAMP_TICK_COUNT			50
#define MOUSR_FB_RAMP_TICK_COUNT			60

#define REMOTE_SWITCH_VALUE_UP         		0x01u  
#define REMOTE_SWITCH_VALUE_DOWN			0x02u
#define REMOTE_SWITCH_VALUE_CENTRAL			0x03u

#define REMOTE_SWITCH_CHANGE_1TO3      (uint8_t)((REMOTE_SWITCH_VALUE_UP << 2) | REMOTE_SWITCH_VALUE_CENTRAL)   
#define REMOTE_SWITCH_CHANGE_2TO3      (uint8_t)((REMOTE_SWITCH_VALUE_DOWN << 2) | REMOTE_SWITCH_VALUE_CENTRAL)  
#define REMOTE_SWITCH_CHANGE_3TO1      (uint8_t)((REMOTE_SWITCH_VALUE_CENTRAL << 2) | REMOTE_SWITCH_VALUE_UP)
#define REMOTE_SWITCH_CHANGE_3TO2      (uint8_t)((REMOTE_SWITCH_VALUE_CENTRAL << 2) | REMOTE_SWITCH_VALUE_DOWN)

#define REMOTE_SWITCH_CHANGE_1TO3TO2   (uint8_t)((REMOTE_SWITCH_VALUE_UP << 4) |\
                                                 (REMOTE_SWITCH_VALUE_CENTRAL << 2) |\
                                                 (REMOTE_SWITCH_VALUE_DOWN))   

#define REMOTE_SWITCH_CHANGE_2TO3TO1   (uint8_t)((REMOTE_SWITCH_VALUE_DOWN << 4) |\
                                                 (REMOTE_SWITCH_VALUE_CENTRAL << 2) |\
                                                 (REMOTE_SWITCH_VALUE_UP)) 

#define REMOTE_SWITCH_VALUE_BUF_DEEP   16u
#define DBUS_MAX_LEN 50
#define DBUS_BUFLEN  18


/**********************************************************************************
 * bit      :15   14   13   12   11   10   9   8   7   6     5     4   3   2   1
 * keyboard : V    C    X	  Z    G    F    R   E   Q  CTRL  SHIFT  D   A   S   W
 **********************************************************************************/
#define KEY_W 			(remote_info.key_code &0x0001 )		//bit 0
#define KEY_S 			(remote_info.key_code &0x0002 )
#define KEY_A 			(remote_info.key_code &0x0004	)
#define KEY_D 			(remote_info.key_code &0x0008	)
#define KEY_SHIFT 	(remote_info.key_code &0x0010	)
#define KEY_CTRL 		(remote_info.key_code &0x0020	)
#define KEY_Q 			(remote_info.key_code &0x0040	)
#define KEY_E				(remote_info.key_code &0x0080	)
#define KEY_R 			(remote_info.key_code &0x0100	)
#define KEY_F 			(remote_info.key_code &0x0200	)
#define KEY_G 			(remote_info.key_code &0x0400	)
#define KEY_Z 			(remote_info.key_code &0x0800	)
#define KEY_X 			(remote_info.key_code &0x1000	)
#define KEY_C 			(remote_info.key_code &0x2000	)
#define KEY_V 			(remote_info.key_code &0x4000	)	//bit 15
#define KEY_B				(remote_info.key_code &0x8000	)
/******************************************************/


//RC_CtrlData
/*协议定义：
* ch0 右摇杆 364-1024-1684 左->右
* ch1 右摇杆 364-1024-1684 下->上
*	ch2 左摇杆 364-1024-1684 左->右
* ch3 左摇杆 364-1024-1684 下->上
*  s1 left  switch  上-中-下  1-3-2
*  s2 right ... 
*/


typedef __packed struct
{
	int16_t ch0;
	int16_t ch1;
	int16_t ch2;
	int16_t ch3;
	int8_t s1;
	int8_t s2;
}Remote;
typedef __packed struct
{
	int16_t x;
	int16_t y;
	int16_t z;
	uint8_t press_l;
	uint8_t press_r;
}Mouse;	

typedef __packed struct
{
	Remote rc;
	Mouse mouse;
	uint16_t key_code;
}RC_Ctl_t;

enum
{
  RC_UP = 1,
  RC_MI = 3,
  RC_DN = 2,
};




void testRemoteTask(void);
void dbus_uart_init(void);
void RemoteDataPrcess(uint8_t *pData);
int is_keyboard_mode(void);
static int UART_Receive_DMA_No_IT(UART_HandleTypeDef* huart, uint8_t* pData, uint32_t Size);
extern uint8_t RemoteData[DBUS_MAX_LEN];
extern RC_Ctl_t remote_info;
#endif

