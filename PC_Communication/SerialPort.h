//
// Created by su on 2/3/18.
//

#ifndef PC_COMMUNICATION_SERIALPORT_H
#define PC_COMMUNICATION_SERIALPORT_H

#include <stdint.h>

class SerialPort
{
public:
    SerialPort(char* portname,int baudrate=9600);
    SerialPort();
    int ReadFromSerial(char* buffer,int size );
    int SendToSerial(char*buffer,int size);
    bool is_connect();
    void closePort();
    char readByte();
private:
    int fd;
    int BaudRate;
    bool is_connected;
};


#endif //PC_COMMUNICATION_SERIALPORT_H
