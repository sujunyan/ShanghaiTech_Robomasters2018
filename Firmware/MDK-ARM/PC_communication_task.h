#ifndef __PC_COMMUNICATION_TASK__
#define __PC_COMMUNICATION_TASK__

#define COMPUTER_TX_BUF_SIZE 1024
#define COMPUTER_FRAME_BUFLEN 200
#define COMMUNICATION_PERIOD 20
#define PROTOCAL_FRAME_MAX_SIZE  200

#include "bsp_uart.h"
#include "protocol.h"
#include "infantry_info.h"
typedef enum
{
  STEP_HEADER_SOF  = 0,
  STEP_LENGTH_LOW  = 1,
  STEP_LENGTH_HIGH = 2,
  STEP_FRAME_SEQ   = 3,
  STEP_HEADER_CRC8 = 4,
  STEP_DATA_CRC16  = 5,
} unpack_step_e;

typedef __packed struct
{
  frame_header_t *p_header;
  uint16_t       data_len;
  uint8_t        protocol_packet[PROTOCAL_FRAME_MAX_SIZE];
  unpack_step_e  unpack_step;
  uint16_t       index;
} unpack_data_t;

void PC_receive_task(const void * argu);
void get_dma_memory_msg(DMA_Stream_TypeDef *dma_stream, uint8_t *mem_id, uint16_t *remain_cnt);
uint16_t data_pack_handle(uint16_t cmd_id, uint8_t *p_data, uint16_t len);
void send_all_pack_to_pc(void);
void pc_data_handle(uint8_t *p_frame);
void unpack_data(uint8_t* buffer, uint16_t *begin, uint16_t end);

void pc_chassis_control_data_handle(chassis_ctrl_t*);
void pc_gimbal_control_data_handle(gimbal_ctrl_t*);
void pc_shoot_control_data_handle(shoot_ctrl_t*);
void PC_send_task(void const * argu);
void PC_send_msg_update(void);

extern uint8_t computer_tx_buf[COMPUTER_TX_BUF_SIZE];
#endif
