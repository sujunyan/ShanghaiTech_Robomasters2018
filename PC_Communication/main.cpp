#include <iostream>
#include <cstdlib>
#include <cstdio>
#include "CommunicateNode.h"
int main(void){
        char buffer[]="hello world\n";
        char portname[]="/dev/ttyUSB0";
    //printf("HEADER_LEN=%d\n",HEADER_LEN);
#if 1
        CommunicateNode node(portname,115200);
        int cnt=0;
        char ch;
        while(node.is_open())
        {
          #if 1
                node.update_once();
                if(cnt++%10==0)
                {
                    node.test();
                }
         #endif
        }
#endif
#if 0
        SerialPort serialPort(portname,115200);
        int cnt=0;
        int tmp=0;
        while( tmp!=-1&& cnt++<10000000000000000)
        {
            tmp=serialPort.ReadFromSerial(buffer, 1);
            if(tmp>0)
            printf("%x with Size %d \n",(unsigned char)buffer[0],tmp);
        }
        if(tmp==-1)
            printf("The error is %d\n",errno);
        serialPort.closePort();
#endif

        return 0;
}
