//
// Created by su on 2/4/18.
//

#include "CommunicateNode.h"
#include "board_info.h"
#include "judgement_info.h"
#include "protocol.h"
#include <cstring>
#include <iostream>

#include <unistd.h>

uint8_t* protocol_packet_pack(uint16_t cmd_id, uint8_t *p_data, uint16_t len, uint8_t sof, uint8_t *tx_buf) {
  uint16_t frame_length = HEADER_LEN + CMD_LEN + len + CRC_LEN;
  frame_header_t *p_header = (frame_header_t*)tx_buf;

  p_header->sof          = sof;
  p_header->data_length  = len;
  p_header->seq          = 0;
  //p_header->seq          = seq++;

  memcpy(&tx_buf[HEADER_LEN], (uint8_t*)&cmd_id, CMD_LEN);
  append_crc8_check_sum(tx_buf, HEADER_LEN);
  memcpy(&tx_buf[HEADER_LEN + CMD_LEN], p_data, len);
  append_crc16_check_sum(tx_buf, frame_length);

  return tx_buf;
}

void CommunicateNode::unpack_data(unpack_data_t *p_obj, uint8_t sof) {
  uint8_t byte = 0;
  int static cnt=0;
  bool flag=1;
  while (flag)
  {
    //independent thread, need not mutex
    byte=port.readByte();
    //printf("unpack_step: %d \n",p_obj->unpack_step);
    switch(p_obj->unpack_step)
    {
      case STEP_HEADER_SOF:
      {
        if(byte == sof)
        {
          p_obj->unpack_step = STEP_LENGTH_LOW;
          p_obj->protocol_packet[p_obj->index++] = byte;
        }
        else
        {
          p_obj->index = 0;
        }
      }break;

      case STEP_LENGTH_LOW:
      {
        p_obj->data_len = byte;
        p_obj->protocol_packet[p_obj->index++] = byte;
        p_obj->unpack_step = STEP_LENGTH_HIGH;
      }break;

      case STEP_LENGTH_HIGH:
      {
          //printf("step_length_high header size %d \n",p_obj->p_header==NULL);
        p_obj->data_len |= (byte << 8);
        p_obj->protocol_packet[p_obj->index++] = byte;

        if(p_obj->data_len < (PROTOCAL_FRAME_MAX_SIZE - HEADER_LEN - CRC_LEN))
        {
          p_obj->unpack_step = STEP_FRAME_SEQ;
          p_obj->p_header->data_length=p_obj->data_len;
        }
        else
        {
          p_obj->unpack_step = STEP_HEADER_SOF;
          p_obj->index = 0;
        }
      }break;

      case STEP_FRAME_SEQ:
      {
        p_obj->protocol_packet[p_obj->index++] = byte;
        p_obj->unpack_step = STEP_HEADER_CRC8;
      }break;

      case STEP_HEADER_CRC8:
      {
        //printf("Check the msg %d\n",cnt++);
        p_obj->protocol_packet[p_obj->index++] = byte;

        if (p_obj->index == HEADER_LEN)
        {
          if ( verify_crc8_check_sum(p_obj->protocol_packet, HEADER_LEN) )
        //  if(1)
          {
            //p_obj->p_header->data_length=p_obj->data_len;
            p_obj->unpack_step = STEP_DATA_CRC16;
            //printf("Check CRC8 sucessfully with data_length %d  %d with data \n",p_obj->p_header->data_length,p_obj->data_len);
            /*
              for (int i = 0; i < 5 ; ++i)
            {
              printf("%x, ",p_obj->protocol_packet[i]);
            }
            printf("\n");
             */
          }
          else
          {
            //printf("check_CRC8 failed\n");
            p_obj->unpack_step = STEP_HEADER_SOF;
            p_obj->index = 0;
          }
        }
      }break;

      case STEP_DATA_CRC16:
      {
        if (p_obj->index < (HEADER_LEN + CMD_LEN + p_obj->data_len + CRC_LEN))
        {
           p_obj->protocol_packet[p_obj->index++] = byte;
        }
        if (p_obj->index >= (HEADER_LEN + CMD_LEN + p_obj->data_len + CRC_LEN))
        {
          p_obj->unpack_step = STEP_HEADER_SOF;
          p_obj->index = 0;


          if ( verify_crc16_check_sum(p_obj->protocol_packet, HEADER_LEN + CMD_LEN + p_obj->data_len + CRC_LEN) )
          {
            //printf("Check CRC16 sucessfully\n");
            if (sof == UP_REG_ID)
            {
              board_data_handle(p_obj->protocol_packet);
            }
            else  //DN_REG_ID
            {
              judgement_data_handle(p_obj->protocol_packet);
            }
              flag=0;
          }
          //else printf("check_CRC16 failed\n");
        }
      }break;

      default:
      {
        p_obj->unpack_step = STEP_HEADER_SOF;
        p_obj->index = 0;
      }break;
    }
  }
}

void CommunicateNode::board_data_handle(uint8_t *p_frame)
{
  frame_header_t *p_header = (frame_header_t*)p_frame;
  memcpy(p_header, p_frame, HEADER_LEN);
  uint16_t data_length = p_header->data_length;
  uint16_t cmd_id      = *(uint16_t *)(p_frame + HEADER_LEN);
  uint8_t *data_addr   = p_frame + HEADER_LEN + CMD_LEN;


  switch (cmd_id)
  {
      case CHASSIS_DATA_ID:
      memcpy(&board_rece_msg.chassis_information, data_addr, data_length);
        break;
      case GIMBAL_DATA_ID:
          memcpy(&board_rece_msg.chassis_information, data_addr, data_length);
          break;
      case SHOOT_TASK_DATA_ID:
          memcpy(&board_rece_msg.shoot_task_information, data_addr, data_length);
          break;
      case INFANTRY_ERR_ID:
          memcpy(&board_rece_msg.bottom_error_data, data_addr, data_length);
          break;
      case CONFIG_RESPONSE_ID:
          memcpy(&board_rece_msg.structure_config_data, data_addr, data_length);
          break;
      case CALI_RESPONSE_ID:
          memcpy(&board_rece_msg.cali_response_data, data_addr, data_length);
          break;
      case REMOTE_CTRL_INFO_ID:
          memcpy(&board_rece_msg.remote_ctrl_data, data_addr, data_length);
          break;
      case BOTTOM_VERSION_ID:
          memcpy(&board_rece_msg.version_info_data, data_addr, data_length);
          break;
  }


}

void CommunicateNode::judgement_data_handle(uint8_t * p_frame) {
  frame_header_t *p_header = (frame_header_t*)p_frame;
  memcpy(p_header, p_frame, HEADER_LEN);

  uint16_t data_length = p_frame[1]|p_frame[2]<<8;
  uint16_t cmd_id      = p_frame[HEADER_LEN]| (p_frame [HEADER_LEN+1])<<8;
  //  uint16_t cmd_id      = *(uint16_t*)(p_frame+HEADER_LEN);
  uint8_t *data_addr   = p_frame + HEADER_LEN + CMD_LEN;

#if 1
    if(cmd_id)
    {
      printf("handle judge data\n");
      //print_data(p_frame);
      print_all_packet(p_frame);
      printf("judge_data handle with len %d cmd_id %x \n", data_length, cmd_id);
      //printf("HEADER_LEN= %d the data is ",HEADER_LEN);

    }
#endif
  switch (cmd_id)
  {
    case GAME_INFO_ID:
      memcpy(&judge_rece_mesg.game_information, data_addr, data_length);
    break;

    case REAL_BLOOD_DATA_ID:
      memcpy(&judge_rece_mesg.blood_changed_data, data_addr, data_length);
    break;

    case REAL_SHOOT_DATA_ID:
      memcpy(&judge_rece_mesg.real_shoot_data, data_addr, data_length);
    break;

    case STU_CUSTOM_DATA_ID:
      memcpy(&judge_rece_mesg.student_download_data, data_addr, data_length);
    break;

//    case REAL_FIELD_DATA_ID:
//      memcpy(&judge_rece_mesg.rfid_data, data_addr, data_length);
//    break;
//
//    case GAME_RESULT_ID:
//      memcpy(&judge_rece_mesg.game_result_data, data_addr, data_length);
//    break;
//
//    case GAIN_BUFF_ID:
//      memcpy(&judge_rece_mesg.get_buff_data, data_addr, data_length);
//    break;
//
//    case CLIENT_TO_ROBOT_ID:
//      memcpy(&judge_rece_mesg.student_download_data, data_addr, data_length);
//    break;
  }

  //printf("judge_data handle complete\n");
  /* forward data */
  //data_packet_pack(cmd_id, data_addr, data_length, UP_REG_ID);

}

CommunicateNode::CommunicateNode(char *portname, int baudrate) {
    IsOpen=0;
    port=SerialPort(portname,baudrate);
    while (!port.is_connect())
    {
      printf("Try to connect to the USB port\n");
      //usleep(10000);
    }

    data.p_header=new frame_header_t;

    memset(&judge_rece_mesg,0,sizeof(judge_rece_mesg));
    memset(&board_rece_msg,0,sizeof(board_rece_msg));
    IsOpen=1;
}

void CommunicateNode::update() {
    while (true)
    {
       update_once();
    }
}

void CommunicateNode::update_once() {
        //unpack_data(&data,UP_REG_ID);
        //printf("update begin\n");
        unpack_data(&data,DN_REG_ID);
        //printf("data updated\n");
}

void CommunicateNode::print_judge_info() {

    // cmd_id=1 game_info
#if 0
    game_robot_state_t state=judge_rece_mesg.game_information;
    printf("stage_remain_time is %d\n",state.stage_remain_time);
    printf("game process is %d\n",state.game_process);
    printf("remain_hp is:%d max: %d\n",judge_rece_mesg.game_information.remain_hp,judge_rece_mesg.game_information.max_hp);
#endif

#if 0

    real_shoot_t shoot=judge_rece_mesg.real_shoot_data;
    printf("bullet speed is %f frequency is %d\n",shoot.bullet_speed,shoot.bullet_freq);

    printf("robot_hurt_data is:%d %d\n",
            judge_rece_mesg.blood_changed_data.armor_type,
            judge_rece_mesg.blood_changed_data.hurt_type
    );
#endif

}

void CommunicateNode::print_board_info() {

}

void CommunicateNode::test() {
    print_judge_info();
}

CommunicateNode::~CommunicateNode() {
}

bool CommunicateNode::is_open() {
  return IsOpen;
}





