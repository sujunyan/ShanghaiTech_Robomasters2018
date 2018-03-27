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

uint16_t protocol_packet_pack(uint16_t cmd_id, uint8_t *p_data, uint16_t len, uint8_t sof, uint8_t *tx_buf) {
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

  return frame_length;
} // unused function -- send message

void CommunicateNode::unpack_data(unpack_data_t *p_obj, uint8_t sof) {
  uint8_t byte = 0;
  int static cnt = 0;
  bool flag = 1;
  while (flag)
  {
    byte = readByte();
      //printf("%x ",byte);
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
          //if(1)
          {
            p_obj->unpack_step = STEP_DATA_CRC16;
          }
          else
          {
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
        else if (p_obj->index >= (HEADER_LEN + CMD_LEN + p_obj->data_len + CRC_LEN))
        {
          p_obj->unpack_step = STEP_HEADER_SOF;
          p_obj->index = 0;


          if ( verify_crc16_check_sum(p_obj->protocol_packet, HEADER_LEN + CMD_LEN + p_obj->data_len + CRC_LEN) )
          {
            //if (sof == UP_REG_ID) // TODO
            {
              board_data_handle(p_obj->protocol_packet);
            }
           // else  //DN_REG_ID
            {
              judgement_data_handle(p_obj->protocol_packet);
            }
              flag=0;
          }
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

void CommunicateNode::send_data(float data1, float data2, float data3, uint8_t mask) {
#if 0
  extShowData_t package;
  package .data1 = data1;
  package . data2 = data2;
  package . data3 = data3;
  package . mask =mask;
#endif
#if 1
    gimbal_ctrl_t package;
    package.ctrl_mode=0;
    package.pit_ref=1;
    package.yaw_ref=2;
    package.visual_valid=3;
#endif
  uint8_t buf[100];
  uint16_t size = protocol_packet_pack(GIMBAL_CTRL_ID, (uint8_t*) &package, sizeof(package), DN_REG_ID, buf);
  boost::asio::write(port, boost::asio::buffer(buf,size));
    //read_and_print();


}

void CommunicateNode::board_data_handle(uint8_t *p_frame)
{
  frame_header_t *p_header = (frame_header_t*)p_frame;
  memcpy(p_header, p_frame, HEADER_LEN);
  uint16_t data_length = p_header->data_length;
  uint16_t cmd_id      = *(uint16_t *)(p_frame + HEADER_LEN);
  uint8_t *data_addr   = p_frame + HEADER_LEN + CMD_LEN;

    //printf("board data handle %x \n",cmd_id);
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
      {
          memcpy(&board_rece_msg.remote_ctrl_data, data_addr, data_length);
          //printf("remote_data recv\n");
      }
          break;
      case BOTTOM_VERSION_ID:
          memcpy(&board_rece_msg.version_info_data, data_addr, data_length);
          break;
  }
}

void CommunicateNode::judgement_data_handle(uint8_t *p_frame) {
  frame_header_t *p_header = (frame_header_t*)p_frame;
  memcpy(p_header, p_frame, HEADER_LEN);

  uint16_t data_length = p_frame[1] | p_frame[2] << 8;
  uint16_t cmd_id      = p_frame[HEADER_LEN] | (p_frame[HEADER_LEN+1])<<8;
  //uint16_t cmd_id      = *(uint16_t*)(p_frame+HEADER_LEN);
  uint8_t *data_addr   = p_frame + HEADER_LEN + CMD_LEN;

  #if 1
    //if(cmd_id)
    {
      //printf("handle judge data\n");
      //print_data(p_frame);
      // print_all_packet(p_frame);
      // printf("cmd_id = %d | %d << %d\n", p_frame[HEADER_LEN], p_frame[HEADER_LEN+1], 8);
      // printf("judge_data handle with len %d cmd_id %x \n", data_length, cmd_id);
      //printf("HEADER_LEN= %lu the data is ",HEADER_LEN);
      // printf("\n");
    }
  #endif

  switch (cmd_id)
    {
    case GAME_INFO_ID:
      //printf("it works 001\n");
      memcpy(&judge_rece_mesg.game_information, data_addr, data_length);
    break;

    case REAL_BLOOD_DATA_ID:
      //printf("it works 002\n");
      memcpy(&judge_rece_mesg.blood_changed_data, data_addr, data_length);
    break;

    case REAL_SHOOT_DATA_ID:
      printf("it works 003\n");
      memcpy(&judge_rece_mesg.real_shoot_data, data_addr, data_length);
    break;

    case POWER_HEAT_DATA_ID:
      //printf("it works 004\n");
      memcpy(&judge_rece_mesg.power_heat_data, data_addr, data_length);
    break;

    case REAL_FIELD_DATA_ID:
      //printf("it works 005\n");
      memcpy(&judge_rece_mesg.rfid_data, data_addr, data_length);
    break;

    case GAME_RESULT_ID:
      //printf("it works 006\n");
      memcpy(&judge_rece_mesg.game_result_data, data_addr, data_length);
    break;

    case GAIN_BUFF_ID:
      //printf("it works 007\n");
      memcpy(&judge_rece_mesg.get_buff_data, data_addr, data_length);
    break;

    case ROBOT_POSITION_ID:
      //printf("it works 008\n");
      memcpy(&judge_rece_mesg.robot_position_data, data_addr, data_length);
    break;

    case STU_CUSTOM_DATA_ID:
      //printf("it works 100\n");
      memcpy(&judge_rece_mesg.clinet_show_data, data_addr, data_length);
    break;

    case DOWNLOAD_DATA_ID:
      //printf("it works 102\n");
      memcpy(&judge_rece_mesg.clinet_download_data, data_addr, data_length);
    break;
  }

  //printf("judge_data handle complete\n");
  /* forward data */
  // data_packet_pack(cmd_id, data_addr, data_length, UP_REG_ID);
  /* error, no function named data_packet_pack */

}

CommunicateNode::CommunicateNode(char *portname, int baudrate) {

    IsOpen=0;
    try
    {
        port.open(portname);
        port.set_option(asio::serial_port::baud_rate(baudrate));
        using namespace asio;
        {
        port. set_option (serial_port :: flow_control (serial_port :: flow_control :: none ) ) ;
        port. set_option (serial_port :: parity (serial_port :: parity :: none ) ) ;
        port. set_option (serial_port :: stop_bits (serial_port :: stop_bits :: one ) ) ;
        port. set_option (serial_port :: character_size (8 ) ) ;
        };

    }
    catch (const boost::exception& ex) {
        // error handling
        std::cerr << boost::diagnostic_information(ex);
    }
    data.p_header = new frame_header_t;
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
        /* if open, other codes doesnt work */
        //printf("update begin\n");
        unpack_data(&data,DN_REG_ID);
        //printf("data updated\n");
}

void CommunicateNode::print_judge_info() {
  //cmd_id=1 game_info
  printf("================CHECKSTART======================\n");
  // 0x0001 game_information
      printf("----------0x0001 game_information-------------------------\n");
      extGameRobotState_t state = judge_rece_mesg.game_information;
      printf("stageRemainTime: %d\n",state.stageRemainTime);
      printf("gameProgress: %d\n",state.gameProgress);
      printf("remainHP: %d/%d\n",state.remainHP, state.maxHP);
  // 0x0002 blood_changed_data
  // 0x0003 real_shoot_data
      printf("-----0x0002 & 0x0003 blood_changed & real_shoot_data------\n");
      extShootData_t shoot = judge_rece_mesg.real_shoot_data;
      printf("bulletType: %d\n", shoot.bulletType);
      printf("bulletSpeed: %f\n", shoot.bulletSpeed);
      printf("bulletFreq: %d\n", shoot.bulletFreq);
      extRobotHurt_t hurt = judge_rece_mesg.blood_changed_data;
      printf("hurtArmorType: %d", hurt.armorType);
      switch (hurt.armorType)
      {
        case 0: printf(" (Front)\n"); break;
        case 1: printf(" (Left)\n"); break;
        case 2: printf(" (Behind)\n"); break;
        case 3: printf(" (Right)\n"); break;
        case 4: printf(" (Above_1)\n"); break;
        case 5: printf(" (Above_2)\n"); break;
        default: printf(" (Offline)\n"); break;
      }
      printf("hurtType: %d\n", hurt.hurtType);
  // 0x0004 power_heat_data
      printf("----------0x0004 power_heat_data--------------------------\n");
      extPowerHeatData_t power = judge_rece_mesg.power_heat_data;
      printf("chassisVolt: %f\n", power.chassisVolt);
      printf("chassisCurrent: %f\n", power.chassisCurrent);
      printf("chassisPower: %f\n", power.chassisPower);
      printf("chassisPowerBuffer: %f\n", power.chassisPowerBuffer);
  // 0x0005 rfid_data
      printf("----------0x0005 rfid_data--------------------------------\n");
      extRfidDetect_t detect = judge_rece_mesg.rfid_data;
      printf("cardType: %d\n", detect.cardType);
      printf("cardIdx: %d\n", detect.cardIdx);
  // 0x0006 game_result_data
      printf("----------0x0006 game_result_data-------------------------\n");
      extGameResult_t whowin = judge_rece_mesg.game_result_data;
      printf("winner: %d\n", whowin.winner);
  // 0x0007 get_buff_data
      printf("----------0x0007 get_buff_data----------------------------\n");
      extGetBuff_t getbuff = judge_rece_mesg.get_buff_data;
      printf("buffType: %d\n", getbuff.buffType);
      printf("buffAddition: %d\n", getbuff.buffAddition);
  // 0x0008 robot_position_data
      printf("----------0x0008 robot_position_data----------------------\n");
      extGameRobotPos_t position = judge_rece_mesg.robot_position_data;
      printf("x y z: %f / %f / %f\n", position.x, position.y, position.z);
      printf("yaw: %f\n", position.yaw);
  // 0x0100 clinet_show_data
      printf("----------0x0100 client_show_data-------------------------\n");
      extShowData_t userdeff = judge_rece_mesg.clinet_show_data;
      printf("data1: %f\n", userdeff.data1);
      printf("data2: %f\n", userdeff.data2);
      printf("data3: %f\n", userdeff.data3);
  // 0x0102 clinet_show_data
      printf("----------0x0102 clinet_download_data---------------------\n");
      extDownStreamData_t userdef = judge_rece_mesg.clinet_download_data;
      for (int i = 0; i < 32; i++) {
        printf("%d ", userdef.data[i]);
      }
      printf("\n");

  printf("================CHECKOVER======================\n");
}

void CommunicateNode::print_board_info() {

}

void CommunicateNode::test() {
#if 0
  unsigned char ch=readByte();

  if(ch==0xa5)printf("\n%x\n",ch);
  else printf("%x ",ch);
#endif
    print_judge_info();
}

CommunicateNode::~CommunicateNode() {
}

bool CommunicateNode::is_open() {
  return IsOpen;
}

uint8_t CommunicateNode::readByte() {
    uint8_t ch;
    asio::read(port,asio::buffer(&ch,1));
    return ch;
}

void CommunicateNode::read_and_print() {
    int cnt=0;
    char ch=readByte();
    while(ch!='\n' && cnt++<100)
    {
        printf("%c",ch);
        try{
            ch=readByte();
        }
        catch (const boost::exception& ex) {
        // error handling
        std::cerr << boost::diagnostic_information(ex);
    }
    }
    printf("\n");
}
