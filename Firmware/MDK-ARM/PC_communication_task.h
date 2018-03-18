#ifndef __PC_COMMUNICATION_TASK__
#define __PC_COMMUNICATION_TASK__

#define COMPUTER_TX_BUF_SIZE 1024
#define COMPUTER_FRAME_BUFLEN 200
#define COMMUNICATION_PERIOD 20

#include "bsp_uart.h"
void PC_communication_task(const void * argu);
void get_dma_memory_msg(DMA_Stream_TypeDef *dma_stream, uint8_t *mem_id, uint16_t *remain_cnt);
uint16_t data_pack_handle(uint16_t cmd_id, uint8_t *p_data, uint16_t len);
void send_all_pack_to_pc(void);
extern uint8_t computer_tx_buf[COMPUTER_TX_BUF_SIZE];
#endif
