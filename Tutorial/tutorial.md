# Tutorial
## 1. Environment
可以从百度云盘上下载需要用到的软件：

链接：https://pan.baidu.com/s/17SSUVGQZozwbELNAPX0c8g 密码：lv78

安装教程可见其中的README.txt

## 2.电机控制
### 2.1 PID control (RmLib/pid.c)
#### (PID 控制各参数的配置详见main/init.c)
``` c
void PID_struct_init(pid_t* pid, uint32_t mode,
    uint32_t maxout, uint32_t intergral_limit,
    float kp,float ki,float kd)
```
```c
float pid_calc(pid_t *pid, float get, float set)
{
  pid->get = get;
  pid->set = set;
  pid->err[NOW] = set - get;

  if ((pid->input_max_err != 0) && (fabs(pid->err[NOW]) > pid->input_max_err))
      return 0;

  if (pid->pid_mode == POSITION_PID) //position PID
  {
      pid->pout = pid->p * pid->err[NOW];
      pid->iout += pid->i * pid->err[NOW];
      pid->dout = pid->d * (pid->err[NOW] - pid->err[LAST]);
    
      abs_limit(&(pid->iout), pid->integral_limit);
      pid->out = pid->pout + pid->iout + pid->dout;
      abs_limit(&(pid->out), pid->max_out);
  }
  else if (pid->pid_mode == DELTA_PID) //delta PID {... (useless code) }

  pid->err[LLAST] = pid->err[LAST];
  pid->err[LAST]  = pid->err[NOW];
  if ((pid->output_deadband != 0) && (fabs(pid->out) < pid->output_deadband))
    return 0;
  else
    return pid->out;
}

```



### 2.2 电机反馈数据(User_bsp/bsp_can.c)
电机反馈数据由电机所接的can总线发送至主控板，主控板接收到一帧can信号后会自动调用以下函数
``` c
void HAL_CAN_RxCpltCallback(CAN_HandleTypeDef* _hcan){
  switch (_hcan->pRxMsg->StdId)
  {
    case CAN_3510_M1_ID:
    case CAN_3510_M2_ID:
    case CAN_3510_M3_ID:
    case CAN_3510_M4_ID:
    {
      static uint8_t i;
      i = _hcan->pRxMsg->StdId - CAN_3510_M1_ID;
			if(chassis.motor[i].msg_cnt++ <= 50)
			{
				get_moto_offset(&chassis.motor[i], _hcan);
				chassis.ctrl_mode=CHASSIS_RELAX;
			}else encoder_data_handle( _hcan,&chassis.motor[i]);
		// 告诉看门狗底盘电机在线
      err_detector_hook(CHASSIS_M1_OFFLINE + i);
    }
    break; ... 
```
```c
void encoder_data_handle(CAN_HandleTypeDef* hcan,moto_measure_t* ptr){

	// "ecd" is short for "encoder"
	// 为码盘数值(0-8191)，指示电机当前转到的位置
  ptr->last_ecd = ptr->ecd;
  ptr->ecd      = (uint16_t)(hcan->pRxMsg->Data[0] << 8 | hcan->pRxMsg->Data[1]);

  /* ... some useless code ... */

  ptr->speed_rpm     = (int16_t)(hcan->pRxMsg->Data[2] << 8 | hcan->pRxMsg->Data[3]);
  ptr->given_current = (int16_t)(hcan->pRxMsg->Data[4] << 8 | hcan->pRxMsg->Data[5]);
	
}
```

### 2.3 发送电机控制信号(User_bsp/bsp_can.c)
``` c
void send_chassis_cur(int16_t iq1, int16_t iq2, int16_t iq3, int16_t iq4){
  CHASSIS_CAN.pTxMsg->StdId   = 0x200;
  CHASSIS_CAN.pTxMsg->IDE     = CAN_ID_STD;
  CHASSIS_CAN.pTxMsg->RTR     = CAN_RTR_DATA;
  CHASSIS_CAN.pTxMsg->DLC     = 0x08;
  CHASSIS_CAN.pTxMsg->Data[0] = iq1 >> 8;
  CHASSIS_CAN.pTxMsg->Data[1] = iq1;

  /*...balabala...*/
  HAL_CAN_Transmit(&CHASSIS_CAN, 10);
}
```

### 3 底盘控制 (ControlTask/chassis_task.c)
#### 定时器任务 chassis_task 10ms
麦肯阿姆轮的控制详解见[该网站](https://zhuanlan.zhihu.com/p/20282234)
``` c
// 通过遥控器接收到的信息设置相应的各方向的速度
void chasis_remote_handle(void); 

// 通过三个方向的速度计算出底盘四个轮子的速度并存在 chassis.wheel_speed_ref 中
void mecanum_calc(chassis.vx, chassis.vy, chassis.vw, chassis.wheel_speed_ref); 

// 更新底盘轮子的当前速度（在上文HAL_CAN_RxCpltCallback中获取到）
chassis.wheel_speed_fdb[i] = chassis.motor[i].speed_rpm;		
chassis.current[i] = pid_calc(&pid_spd[i], chassis.wheel_speed_fdb[i], chassis.wheel_speed_ref[i]);
```

### 4 遥控器信号处理 (User_bsp/bsp_uart.c)

```c
// 当接收到一个串口空闲信号(USART IDLE)时调用
static void uart_rx_idle_callback(UART_HandleTypeDef* huart);

// 处理遥控器信号并存储在全局变量remote_info
void RemoteDataPrcess(uint8_t *pData){
	remote_info.rc.ch0 = ((int16_t)pData[0] | ((int16_t)pData[1] << 8)) & 0x07FF; 
		remote_info.rc.ch0 -=1024;
		
    remote_info.rc.ch1 = (((int16_t)pData[1] >> 3) | ((int16_t)pData[2] << 5)) & 0x07FF;
		remote_info.rc.ch1 -=1024;
		/*...balabala...*/
}
```

### 5 云台控制(ControlTask/gimbal_task.c)
#### 定时器任务 gimbal_task 5ms
``` c
void gimbal_task(void const *argu)
{
	//更新云台的各类信息，主要为主控板imu的pitch与yaw的值，以及云台电机编码器pitch与yaw的值
	// 同时更新云台两个方向上的角速度 
	update_gimbal_sensor();

	// 通过不同的模式更新 gim.pid 中的相关参数
  switch (gim.ctrl_mode)
  {
    case GIMBAL_INIT: init_mode_handle();break;
    default:close_loop_handle();break;
  }


  // 喜闻乐见的pid控制
  pid_calc(&pid_yaw, gim.pid.yaw_angle_fdb, gim.pid.yaw_angle_ref);
  pid_calc(&pid_pit, gim.pid.pit_angle_fdb, gim.pid.pit_angle_ref); 

  //此处使用两次PID的原因为充分利用已有信息，即云台的角速度信息
  // 第一次通过反馈得到的位置信息输出需要的速度信息，第二次通过反馈得到的速度信息输出电流控制信息
  cascade_pid_ctrl();
  pid_calc(&pid_yaw_speed, gim.pid.yaw_speed_fdb, gim.pid.yaw_speed_ref);
  pid_calc(&pid_pit_speed, gim.pid.pit_speed_fdb, gim.pid.pit_speed_ref);

  if (gimbal_is_controllable())
  {
    gim.current[0] = pid_yaw_speed.out;
    gim.current[1] = pid_pit_speed.out;
  }
  /* ............*/
}
```

```c
void update_gimbal_sensor(void){
	static float yaw_ecd_ratio = YAW_ECD_DIR*YAW_DECELE_RATIO*ENCODER_ANGLE_RATIO;
  static float pit_ecd_ratio = PIT_ECD_DIR*PIT_DECELE_RATIO*ENCODER_ANGLE_RATIO;
  gim.sensor.yaw_relative_angle_ecd =  yaw_ecd_ratio*get_relative_pos(gim.yaw_motor.ecd, YAW_ECD_CENTER_OFFSET);
  gim.sensor.pit_relative_angle_ecd =  pit_ecd_ratio*get_relative_pos(gim.pit_motor.ecd, PIT_ECD_CENTER_OFFSET);
	
	gim.sensor.yaw_relative_angle_imu = YAW_IMU_DIR*(atti.yaw - gim.sensor.yaw_offset_angle_imu);
	gim.sensor.pit_relative_angle_imu = (atti.roll- gim.sensor.pit_offset_angle_imu);

	 /* 更新云台两个方向上的角速度 */
  gim.sensor.yaw_palstance =   YAW_IMU_DIR* (mpu_data.gy) / 16.384f; //unit: dps
  gim.sensor.pit_palstance = - PIT_IMU_DIR* mpu_data.gx / 16.384f; //unit: dps
}
```