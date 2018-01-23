#include <stm32f4xx.h>
#include "RemoteTask.h"

#include "SEGGER_RTT.h"
#include "test_uart.h"
#include "test_app.h"
RC_Ctl_t RC_CtrlData;   //remote control data
extern uint8_t RemoteData[RC_FRAME_LENGTH];
/*ChassisSpeed_Ref_t ChassisSpeedRef;
Gimbal_Ref_t GimbalRef;
FrictionWheelState_e friction_wheel_state = FRICTION_WHEEL_OFF;
static RemoteSwitch_t switch1;   //Ò£¿ØÆ÷×ó²à²¦¸Ë
static volatile Shoot_State_e shootState = NOSHOOTING;
static InputMode_e inputmode = REMOTE_INPUT;   //ÊäÈëÄ£Ê½Éè¶¨

RampGen_t frictionRamp = RAMP_GEN_DAFAULT;  //Ä¦²ÁÂÖÐ±ÆÂ
RampGen_t LRSpeedRamp = RAMP_GEN_DAFAULT;   //mouse×óÓÒÒÆ¶¯Ð±ÆÂ
RampGen_t FBSpeedRamp = RAMP_GEN_DAFAULT;   //mouseÇ°ºóÒÆ¶¯Ð±ÆÂ
*/
void testRemoteTask()
{
	static int cnt=0;
	static int cnt1=0;
	cnt++;
	if(cnt>20)
	{
		cnt=0;
	RemoteDataPrcess(RemoteData);
	SEGGER_RTT_printf(0,"RC_CtrlData: ch0 %d, ch1 %d, ch2 %d, ch3 %d, s1 %d, s2 %d times %d\r\n",
					RC_CtrlData.rc.ch0,RC_CtrlData.rc.ch1,
					RC_CtrlData.rc.ch2,RC_CtrlData.rc.ch3,
					RC_CtrlData.rc.s1,RC_CtrlData.rc.s2,cnt1++);
	}
}

 
void RemoteDataPrcess(uint8_t *pData)
{
    if(pData == NULL)
    { 
        return;
    }
    RC_CtrlData.rc.ch0 = ((int16_t)pData[0] | ((int16_t)pData[1] << 8)) & 0x07FF; 
    RC_CtrlData.rc.ch1 = (((int16_t)pData[1] >> 3) | ((int16_t)pData[2] << 5)) & 0x07FF;
    RC_CtrlData.rc.ch2 = (((int16_t)pData[2] >> 6) | ((int16_t)pData[3] << 2) |
                         ((int16_t)pData[4] << 10)) & 0x07FF;
    RC_CtrlData.rc.ch3 = (((int16_t)pData[4] >> 1) | ((int16_t)pData[5]<<7)) & 0x07FF;
    
    RC_CtrlData.rc.s1 = ((pData[5] >> 4) & 0x000C) >> 2;
    RC_CtrlData.rc.s2 = ((pData[5] >> 4) & 0x0003);

    RC_CtrlData.mouse.x = ((int16_t)pData[6]) | ((int16_t)pData[7] << 8);
    RC_CtrlData.mouse.y = ((int16_t)pData[8]) | ((int16_t)pData[9] << 8);
    RC_CtrlData.mouse.z = ((int16_t)pData[10]) | ((int16_t)pData[11] << 8);    

    RC_CtrlData.mouse.press_l = pData[12];
    RC_CtrlData.mouse.press_r = pData[13];
 
    RC_CtrlData.key.v = ((int16_t)pData[14]);// | ((int16_t)pData[15] << 8);
	
	//SetInputMode(&RC_CtrlData.rc);
	
	//RemoteControlProcess(&(RC_CtrlData.rc));
	/*
	switch(GetInputMode())
	{
		case REMOTE_INPUT:
		{
			//Ò£¿ØÆ÷¿ØÖÆÄ£Ê½
			RemoteControlProcess(&(RC_CtrlData.rc));
		}break;
		case KEY_MOUSE_INPUT:
		{
			//Ò£¿ØÆ÷¿ØÖÆÄ£Ê½
			MouseKeyControlProcess(&RC_CtrlData.mouse,&RC_CtrlData.key);
		}break;
		case STOP:
		{
			//½ô¼±Í£³µ
		}break;
	}
	*/
	
}