#include <iostream>
#include <cstdlib>
#include <cstdio>
#include "CommunicateNode.h"
#include <boost/exception/diagnostic_information.hpp>
int main(void){
        char buffer[]="hello world\n";
        //char portname[]="COM5";
        char portname[]="/dev/ttyS10";
#if 0
    try{
        CommunicateNode node(portname,115200);
        int cnt=0;
        char ch;
        while(node.is_open()) {
#if 1
            //printf("test\n");
            node.update_once();
            if (cnt++ % 10 == 0) {
                node.send_data(1.11, 2.22, 3.33, 0);
                //node.test();
            }
#endif
        }
    }
    catch (const boost::exception& ex) {
        // error handling
        std::cerr << boost::diagnostic_information(ex);
    }
#endif
        uint8_t p_data[] = "cali";
        uint8_t msg [100];
        uint16_t  size = protocol_packet_pack(CALI_GIMBAL_ID, p_data , sizeof(p_data),DN_REG_ID,msg );
        for (int i = 0; i < size ; ++i) {
                printf("%x, ",msg[i]);
        }
        printf("\n");
        uint16_t test = CALI_GIMBAL_ID;
        printf("%x %x \n",
               *( (uint8_t*)(&test)) ,
               *( (uint8_t*) ( (&test)+1) )
        );
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
        printf("test before reutrn\n");
        return 0;
}
