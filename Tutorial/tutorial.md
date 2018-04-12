# Tutorial
## 1. Environment
可以从百度云盘上下载需要用到的软件：

链接：https://pan.baidu.com/s/17SSUVGQZozwbELNAPX0c8g 密码：lv78

安装教程可见其中的README.txt

## 2.电机控制
### 2.1 PID control (RmLib/pid.c)

``` c
void PID_struct_init(pid_t* pid, uint32_t mode,
    uint32_t maxout, uint32_t intergral_limit,
    float kp,float ki,float kd)

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

### 2.2 电机反馈数据
电机反馈数据由电机所接的can总线发送至主控板，主控板接收到一帧can信号后会自动调用以下函数(User_bsp/bsp_can.c)
``` c
void HAL_CAN_RxCpltCallback(CAN_HandleTypeDef* _hcan){
  switch (_hcan->pRxMsg->StdId)
  {
    case CAN_3510_M1_ID:
    case CAN_3510_M2_ID:
    case CAN_3510_M3_ID:
    case CAN_3510_M4_ID:
    {
			//if(gim.ctrl_mode == GIMBAL_INIT)break; //TODO
      static uint8_t i;
      i = _hcan->pRxMsg->StdId - CAN_3510_M1_ID;
			if(chassis.motor[i].msg_cnt++ <= 50)
			{
				get_moto_offset(&chassis.motor[i], _hcan);
				chassis.ctrl_mode=CHASSIS_RELAX;
			}else encoder_data_handle( _hcan,&chassis.motor[i]);
      err_detector_hook(CHASSIS_M1_OFFLINE + i);
    }
    break; ... 
```