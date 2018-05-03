
#ifndef __SYS_CONFIG__
#define __SYS_CONFIG__

#include "RemoteTask.h"
#include "Serial_Debug.h"
#include "detect_task.h"
#include "freeRTOS.h"
#include "stdlib.h"
#include "cmsis_os.h"

#define DEFAULT_TUNE  300
#define RC_RESOLUTION 660.0f
/******************************* config macros *********************************************/

#define CALI_DONE
#define GIMBAL_FOLLOW_CHASSIS
//#define NO_CASCADE_CONTROL // only use position as the feedback but no imu data
#define SERIAL_DEBUG
#define LAUNCH_INFANTRY (remote_info.rc.s2 == RC_MI && remote_info.rc.last_s2 == RC_DN && remote_info.rc.s1 == RC_DN)


/*************************chassis setting*********************************************************/
/* remote mode chassis move speed limit */
/* left and right speed (mm/s) */
#define CHASSIS_RC_MAX_SPEED_X  3300.0f
#define CHASSIS_RC_MOVE_RATIO_X 1.0f
/* back and forward speed (mm/s) */
#define CHASSIS_RC_MAX_SPEED_Y  3300.0f
#define CHASSIS_RC_MOVE_RATIO_Y 1.0f
/* chassis rotation speed (deg/s) */
/* used only chassis open loop mode */
#define CHASSIS_RC_MAX_SPEED_R 150.0f
#define CHASSIS_RC_MOVE_RATIO_R 1.0f

/* keyboard mode speed limit */

#define CHASSIS_KB_MAX_SPEED_X  1000.0f 
#define CHASSIS_KB_MOVE_RATIO_X 1.0f

#define CHASSIS_KB_MAX_SPEED_Y  1000.0f
#define CHASSIS_KB_MOVE_RATIO_Y 1.0f

#define CHASSIS_KB_MAX_SPEED_R 10.0f
#define CHASSIS_KB_MOVE_RATIO_R 1.0f

/************************ chassis parameter ********************************************************/

#define RADIUS     76  // the radius of wheel(mm)
#define PERIMETER  478 //the perimeter of wheel(mm)

//#define WHEELTRACK 403  // wheel track distance(mm)
//#define WHEELBASE  385 // wheelbase distance(mm)

#define WHEELTRACK 740  // wheel track distance(mm)
#define WHEELBASE  550 // wheelbase distance(mm)

#define GIMBAL_X_OFFSET 150 // gimbal is relative to chassis center x axis offset(mm) 
#define GIMBAL_Y_OFFSET 0 /* gimbal is relative to chassis center y axis offset(mm) */

/* chassis motor use 3508 default */
/* define CHASSIS_EC60 to use EC60 */
//#define CHASSIS_EC60

#ifdef CHASSIS_EC60
  /* chassis motor use EC60 */
  /* the deceleration ratio of chassis motor */
  #define CHASSIS_DECELE_RATIO (1.0f)
  /* single 3508 motor maximum speed, unit is rpm */
  #define MAX_WHEEL_RPM        400   //440rpm = 3500mm/s
  /* chassis maximum translation speed, unit is mm/s */
  #define MAX_CHASSIS_VX_SPEED 3300  //415rpm
  #define MAX_CHASSIS_VY_SPEED 3300
  /* chassis maximum rotation speed, unit is degree/s */
  #define MAX_CHASSIS_VR_SPEED 300
#else
  /* chassis motor use 3508 */
  /* the deceleration ratio of chassis motor */
  #define CHASSIS_DECELE_RATIO (1.0f/19.0f)
  /* single 3508 motor maximum speed, unit is rpm */
  #define MAX_WHEEL_RPM        8500  //8347rpm = 3500mm/s
  /* chassis maximum translation speed, unit is mm/s */
  #define MAX_CHASSIS_VX_SPEED 3300  //8000rpm
  #define MAX_CHASSIS_VY_SPEED 3300
  /* chassis maximum rotation speed, unit is degree/s */
  #define MAX_CHASSIS_VR_SPEED 360   //5000rpm
#endif

/**************************gimbal setting*************************************************************************************/


/* remote mode gimbal speed limit */

#define GIMBAL_RC_MOVE_RATIO_PIT 0.002f
#define GIMBAL_RC_MOVE_RATIO_YAW 0.001f 
 

/************************** gimbal parameter ******************************************************************/
/* the ratio of motor encoder value translate to degree */
#define ENCODER_ANGLE_RATIO    (360.0f/8192.0f)
#define PIT_DECELE_RATIO       1.0f /* the deceleration ratio of pitch axis motor */
#define YAW_DECELE_RATIO       1.0f    //(5.0f/8.0f) /* the deceleration ratio of yaw axis motor */
#define TRI_MOTO_POSITIVE_DIR  1.0f 

/*************************** gimbal relevant *********************************/
#define GIMBAL_CASCADE_CTRL
#define PIT_ANGLE_MAX        30
#define PIT_ANGLE_MIN        -30
#define YAW_ANGLE_MAX        20
#define YAW_ANGLE_MIN        -20
#define LEFT_FRICTION        TIM12->CCR1
#define RIGHT_FIRCTION       TIM12->CCR2
/**************************shot  setting********************************/
/* shot speed */
#define DEFAULT_FRIC_WHEEL_SPEED 2500 //maximum value is 2500
#define TRIGGER_MOTOR_SPEED      2000  /* shot frequence */
/************************ uart relevant********************************************/
#define PC_HUART huart6
#define DBUS_HUART         huart1 //for dji remote controler reciever
#define JUDGE_HUART        huart3 //connected to judge system
#define COMPUTER_HUART     huart6 //connected to manifold/TXone


/* can relevant ******************************************/
#define CHASSIS_CAN       hcan1
#define ZGYRO_CAN         hcan2
#define CHASSIS_ZGYRO_CAN hcan1
#define GIMBAL_CAN        hcan1
#define TRIGGER_CAN       hcan1


/***** math relevant ***************************************/


#define RADIAN_COEF 57.3f // radian coefficient 180/PI
#define PI          3.142f

#define VAL_LIMIT(val, min, max) \
do {\
if((val) <= (min))\
{\
  (val) = (min);\
}\
else if((val) >= (max))\
{\
  (val) = (max);\
}\
} while(0)\

#define ABS(val) (val > 0 ? val:-val)




/* imu temperature control *******************************************/
#define IMU_PWM_PULSE      TIM3->CCR2
#define DEFAULT_IMU_TEMP   50
/******************BEEP*********************/
#define BEEP_TUNE TIM3->ARR
#define BEEP_CTRL TIM3->CCR1
/* communication task macros ******************************************/
#define JUDGE_UART_TX_SIGNAL   ( 1 << 0 )
#define JUDGE_UART_IDLE_SIGNAL ( 1 << 1 )
#define JUDGE_DMA_FULL_SIGNAL  ( 1 << 2 )

#define PC_UART_TX_SIGNAL      ( 1 << 3 )
#define PC_UART_IDLE_SIGNAL    ( 1 << 4 )
#define PC_DMA_FULL_SIGNAL     ( 1 << 5 )

#define GIMBAL_MOTOR_MSG_SEND  ( 1 << 6 )
#define CHASSIS_MOTOR_MSG_SEND ( 1 << 7 )

#define SHOT_TASK_EXE_SIGNAL   ( 1 << 8 )
#define INFO_GET_EXE_SIGNAL    ( 1 << 9 )



#endif
