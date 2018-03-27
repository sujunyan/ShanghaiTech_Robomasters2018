//
// Created by su on 2/4/18.
//

#ifndef PC_COMMUNICATION_COMMUNICATENODE_H
#define PC_COMMUNICATION_COMMUNICATENODE_H

#define BOOST_SPIRIT_DEBUG
#include <cstdint>
#include "protocol.h"

#include <boost/asio.hpp>
#include <boost/exception/diagnostic_information.hpp>
using namespace boost;

class CommunicateNode
{
public:
    CommunicateNode(char* portname,int baudrate);
    ~CommunicateNode();
    void update_once();
    void update();
    void send_cali_cmd();
    void send_chasis_ctrl();
    void send_gimbal_ctrl();
    void send_data(float data1, float data2, float data3, uint8_t mask);
    send_board_t board_send_msg;

    bool is_open();
    void test();

private:
    unpack_data_t data;
    asio::io_service io;
    asio::serial_port port=asio::serial_port(io);
    receive_judge_t judge_rece_mesg;
    receive_board_t board_rece_msg;
    bool IsOpen;
    void unpack_data(unpack_data_t *p_obj, uint8_t sof); // unpack one data and return
    void board_data_handle(uint8_t*);
    void judgement_data_handle(uint8_t*);

    uint8_t readByte();
    void read_and_print();
    void print_judge_info();
    void print_board_info();

};

uint16_t protocol_packet_pack(uint16_t cmd_id, uint8_t *p_data, uint16_t len, uint8_t sof, uint8_t *tx_buf);
#endif //PC_COMMUNICATION_COMMUNICATENODE_H
