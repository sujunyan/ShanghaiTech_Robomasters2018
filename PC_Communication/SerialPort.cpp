//
// Created by su on 2/3/18.
//

#include "SerialPort.h"
#include <stdio.h>		//Standard input/output definitions
#include <string.h>		//String function definitions
#include <unistd.h>		//UNIX standard function definitions
#include <fcntl.h>		//File control definitions
#include <errno.h>		//Error number definitions
#include <termios.h>	//POSIX terminal control definitions
#include <iostream>		//Input-Output Streams
#include <sys/ioctl.h>

SerialPort::SerialPort(char *portname,int baudrate) {
    struct termios options;
    fd = open(portname, O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd == -1){
        std::cout << "Port Failed to Open"<<std::endl;
        is_connected=0;
    }
    else
    {
          // fcntl(fd, F_SETFL,  FNDELAY); 
              // Sets the read() function to return NOW and not wait for data to enter buffer if there isn't anything there.

        //Configure port for 8N1 transmission
        tcgetattr(fd, &options);                    //Gets the current options for the port

        BaudRate=baudrate;
        switch (baudrate)
        {
            case 9600:baudrate=B9600;break;
            case 115200:baudrate=B115200;break;
            case 19200:baudrate=B19200;break;
            case 38400:baudrate=B38400;break;
        }
        cfsetispeed(&options, baudrate);                //Sets the Input Baud Rate
        cfsetospeed(&options, baudrate);                //Sets the Output Baud Rate
        options.c_cflag |= (CLOCAL | CREAD);        //? all these set options for 8N1 serial operations
        options.c_cflag &= ~PARENB;                    //?
        options.c_cflag &= ~CSTOPB;                    //?
        options.c_cflag &= ~CSIZE;                    //?
        options.c_cflag &= ~CRTSCTS;
        options.c_cflag |= CS8;                        //?
        tcsetattr(fd, TCSANOW, &options);            //Set the new options for the port "NOW"
        is_connected=1;
        printf("Port open successfully\n");
    }


}

int SerialPort::ReadFromSerial(char *buffer, int size) {
    if(!is_connect())return -1;
    int tmp;
    ioctl(fd,FIONREAD,&tmp);
    if(tmp==0) // There is no input in the buffer
    {
        //printf("No data in the buffer\n");
        return 0;
    }
    tmp=read(fd,buffer,size);
    if(tmp==-1)
    {
        printf("Read from serial failed\n");
        close(fd);
        is_connected=0;
    }
    return tmp;
}

int SerialPort::SendToSerial(char *buffer, int size) {
    if(!is_connect())return -1;

    int tmp=write(fd,buffer,size);
    if(tmp==-1)
    {
        printf("Write to serial failed\n");
        close(fd);
        is_connected=0;
    }
    return tmp;
}

bool SerialPort::is_connect() {
    if(!is_connected)
    {
        printf("Port is not connected\n");
    }
    return is_connected;
}

void SerialPort::closePort() {
    close(fd);
}

char SerialPort::readByte() {
    char ch;
    while (ReadFromSerial(&ch,1)==0);
    return ch;

}

SerialPort::SerialPort() {is_connected=0;}
