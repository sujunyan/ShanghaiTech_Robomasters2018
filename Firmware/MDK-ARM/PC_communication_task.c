#include "PC_communication_task.h"
#include "sys_config.h"
#include "cmsis_os.h"
#include "bsp_uart.h"
#include "protocol.h"
#include "infantry_info.h"
#include "chassis_task.h"
#include "shoot_task.h"
#include "gimbal_task.h"
uint8_t computer_tx_buf[COMPUTER_TX_BUF_SIZE];
UBaseType_t pc_comm_surplus;
receive_pc_t pc_rece_mesg;

void PC_receive_task(const void * argu){
	osEvent event;
 // uint8_t unpack_flag = 0;
	static uint16_t begin;
	computer_uart_init();
	while(1)
	{
		event = osSignalWait(PC_UART_TX_SIGNAL | \
                         PC_UART_IDLE_SIGNAL | \
                         PC_DMA_FULL_SIGNAL, osWaitForever);
		uint8_t  current_memory_id;
		uint16_t remain_data_counter;		
		
		if 	(event.value.signals & PC_UART_IDLE_SIGNAL) 						
    {
        get_dma_memory_msg(COMPUTER_HUART.hdmarx->Instance, &current_memory_id, &remain_data_counter);
				//printf("In IDLE memory id %d remain_data_counter %d \r\n",current_memory_id,remain_data_counter);
				unpack_data(pc_dma_rxbuff[current_memory_id],& begin,UART_RX_DMA_SIZE-remain_data_counter);
    }
    if (event.value.signals & PC_DMA_FULL_SIGNAL)
		{
				get_dma_memory_msg(COMPUTER_HUART.hdmarx->Instance, &current_memory_id, &remain_data_counter);
				//printf("In FULL memory id %d remain_data_counter %d \r\n",current_memory_id,remain_data_counter);
				unpack_data(pc_dma_rxbuff[current_memory_id],& begin,UART_RX_DMA_SIZE);
				begin=0;	
		}
		
   // send_all_pack_to_pc();
		
		
		pc_comm_surplus=uxTaskGetStackHighWaterMark(NULL);
		
	}
}



// return the frame length
uint16_t data_pack_handle(uint16_t cmd_id, uint8_t *p_data, uint16_t len){
  memset(computer_tx_buf, 0, COMPUTER_FRAME_BUFLEN);
  frame_header_t *p_header = (frame_header_t*)computer_tx_buf;
  
  p_header->sof          = DN_REG_ID;
  p_header->data_length  = len;
  
  memcpy(&computer_tx_buf[HEADER_LEN], (uint8_t*)&cmd_id, CMD_LEN);
  append_crc8_check_sum(computer_tx_buf, HEADER_LEN);
  
  memcpy(&computer_tx_buf[HEADER_LEN + CMD_LEN], p_data, len);
  append_crc16_check_sum(computer_tx_buf, HEADER_LEN + CMD_LEN + len + CRC_LEN);
	
	uint16_t frame_length = HEADER_LEN + CMD_LEN + len + CRC_LEN;
	return frame_length;

}

void send_all_pack_to_pc(void){
	 
	//rc_info_t rc_info;
	//memset(rc_info,0,sizeof(rc_info));
	//static uint8_t data[]="hello world\r\n";
	uint16_t size=data_pack_handle(REMOTE_CTRL_INFO_ID,(uint8_t*)&remote_info,sizeof(remote_info));
	
	
	write_uart_blocking(&COMPUTER_HUART,computer_tx_buf,size);
	
	
	// TODO 
}


void unpack_data(uint8_t* buffer, uint16_t *begin, uint16_t end){
	static unpack_data_t p_obj;	
	memset(&p_obj,0,sizeof(p_obj));
  uint8_t byte = 0;
  while (*begin < end)
  {
    byte = buffer[(*begin)++];
    switch(p_obj.unpack_step)
    {
      case STEP_HEADER_SOF:
      {
        if(byte == UP_REG_ID || byte== DN_REG_ID)
        {
          p_obj.unpack_step = STEP_LENGTH_LOW;
          p_obj.protocol_packet[p_obj.index++] = byte;
        }
        else
        {
          p_obj.index = 0;
        }
      }break;
      
      case STEP_LENGTH_LOW:
      {
        p_obj.data_len = byte;
        p_obj.protocol_packet[p_obj.index++] = byte;
        p_obj.unpack_step = STEP_LENGTH_HIGH;
      }break;
      
      case STEP_LENGTH_HIGH:
      {
        p_obj.data_len |= (byte << 8);
        p_obj.protocol_packet[p_obj.index++] = byte;

        if(p_obj.data_len < (PROTOCAL_FRAME_MAX_SIZE - HEADER_LEN - CRC_LEN))
        {
          p_obj.unpack_step = STEP_FRAME_SEQ;
        }
        else
        {
          p_obj.unpack_step = STEP_HEADER_SOF;
          p_obj.index = 0;
        }
      }break;
    
      case STEP_FRAME_SEQ:
      {
        p_obj.protocol_packet[p_obj.index++] = byte;
        p_obj.unpack_step = STEP_HEADER_CRC8;
      }break;

      case STEP_HEADER_CRC8:
      {
        p_obj.protocol_packet[p_obj.index++] = byte;

        if (p_obj.index == HEADER_LEN)
        {
          if ( verify_crc8_check_sum(p_obj.protocol_packet, HEADER_LEN) )
          {
            p_obj.unpack_step = STEP_DATA_CRC16;
          }
          else
          {
            p_obj.unpack_step = STEP_HEADER_SOF;
            p_obj.index = 0;
          }
        }
      }break;  

      case STEP_DATA_CRC16:
      {
        if (p_obj.index < (HEADER_LEN + CMD_LEN + p_obj.data_len + CRC_LEN))
        {
           p_obj.protocol_packet[p_obj.index++] = byte;  
        }
        if (p_obj.index >= (HEADER_LEN + CMD_LEN + p_obj.data_len + CRC_LEN))
        {
          p_obj.unpack_step = STEP_HEADER_SOF;
          p_obj.index = 0;

          if ( verify_crc16_check_sum(p_obj.protocol_packet, HEADER_LEN + CMD_LEN + p_obj.data_len + CRC_LEN) )
          {
            if (p_obj.protocol_packet[p_obj.index++] == UP_REG_ID) //sof
            {
              pc_data_handle(p_obj.protocol_packet);
            }
            else  //DN_REG_ID
            {
              //judgement_data_handle(p_obj->protocol_packet);
            }
          }
        }
      }break;

      default:
      {
        p_obj.unpack_step = STEP_HEADER_SOF;
        p_obj.index = 0;
      }break;
    }
  }

}

void get_dma_memory_msg(DMA_Stream_TypeDef *dma_stream, uint8_t *mem_id, uint16_t *remain_cnt){
  *mem_id     = dma_current_memory_target(dma_stream);
  *remain_cnt = dma_current_data_counter(dma_stream);
}

void pc_data_handle(uint8_t *p_frame){
  frame_header_t *p_header = (frame_header_t*)p_frame;
  memcpy(p_header, p_frame, HEADER_LEN);

  uint16_t data_length = p_header->data_length;
  uint16_t cmd_id      = *(uint16_t *)(p_frame + HEADER_LEN);
  uint8_t *data_addr   = p_frame + HEADER_LEN + CMD_LEN;

  taskENTER_CRITICAL();
  
  switch (cmd_id)
  {
    case CHASSIS_CTRL_ID:
		{
      memcpy(&pc_rece_mesg.chassis_control_data, data_addr, data_length);
			pc_chassis_control_data_handle(&pc_rece_mesg.chassis_control_data);
		}break;

    case GIMBAL_CTRL_ID:
		{
      memcpy(&pc_rece_mesg.gimbal_control_data, data_addr, data_length);
			pc_gimbal_control_data_handle(&pc_rece_mesg.gimbal_control_data);
    }break;

    case SHOOT_CTRL_ID:
		{
      memcpy(&pc_rece_mesg.shoot_control_data, data_addr, data_length);
			pc_shoot_control_data_handle(&pc_rece_mesg.shoot_control_data);
    }break;
  }
  
  taskEXIT_CRITICAL();
}

void pc_chassis_control_data_handle(chassis_ctrl_t* ptr){
	//printf("chasis ctrl data recv.\r\n");
	//chassis.ctrl_mode =ptr->ctrl_mode;
	chassis.vx=ptr->x_speed;
	chassis.vy=ptr->y_speed;
	chassis.vw=ptr->w_info.w_speed;
}
void pc_gimbal_control_data_handle(gimbal_ctrl_t* ptr){
	
	//gim.ctrl_mode=ptr->ctrl_mode;
	gim.pid.pit_angle_ref=ptr->pit_ref;
	gim.pid.yaw_angle_ref=ptr->yaw_ref;
//	printf("gimbal_ctrl_data recv %f %f \r\n\n",ptr->pit_ref,ptr->yaw_ref);
}
void pc_shoot_control_data_handle(shoot_ctrl_t* ptr){
	//printf("shoot_ctrl_data recv\r\n");
	shoot.ctrl_mode=ptr->shoot_cmd;
	shoot.fric_wheel_run = ptr->fric_wheel_run;
	shoot.fric_wheel_spd = ptr->fric_wheel_spd;

}

