#include "PC_communication_task.h"
#include "sys_config.h"
#include "cmsis_os.h"
#include "bsp_uart.h"
#include "protocol.h"
uint8_t computer_tx_buf[COMPUTER_TX_BUF_SIZE];
UBaseType_t pc_comm_surplus;


void PC_communication_task(const void * argu){
	osEvent event;
 // uint8_t unpack_flag = 0;
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
				//unpack_data(pc_dma_rxbuff[current_memory_id],);
    }
    if (event.value.signals & PC_DMA_FULL_SIGNAL)
		{
				get_dma_memory_msg(COMPUTER_HUART.hdmarx->Instance, &current_memory_id, &remain_data_counter);
			//unpack_data();
		}
		
    send_all_pack_to_pc();
		
		
		pc_comm_surplus=uxTaskGetStackHighWaterMark(NULL);
		
	}
}



// return the frame length
uint16_t data_pack_handle(uint16_t cmd_id, uint8_t *p_data, uint16_t len)
{
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

void send_all_pack_to_pc(void)
{
	// TODO 
}

void get_dma_memory_msg(DMA_Stream_TypeDef *dma_stream, uint8_t *mem_id, uint16_t *remain_cnt)
{
  *mem_id     = dma_current_memory_target(dma_stream);
  *remain_cnt = dma_current_data_counter(dma_stream);
}
void unpack_data(uint8_t* buffer, uint16_t begin, uint16_t end)
{

}
