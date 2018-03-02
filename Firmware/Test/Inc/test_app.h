/**
  *@file test_app.h
  *@date 2016-12-13
  *@author Albert.D
  *@brief 
  */
  
#ifndef _TEST__APP_H
#define _TEST__APP_H

#include "stm32f4xx_HAL.h"

typedef struct 
{
  uint8_t canTest;
  uint8_t uartTest;
  uint8_t imuTest;
  uint8_t usbTest;
  uint8_t keyTest;
  uint8_t allTest;
  
}Test_ResultTypedef;

extern Test_ResultTypedef sTestResult;



void Test_task(void);

#endif

