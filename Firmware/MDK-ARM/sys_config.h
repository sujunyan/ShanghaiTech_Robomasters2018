
#ifndef __SYS_CONFIG__
#define __SYS_CONFIG__

#include "freeRTOS.h"
#include "stdlib.h"
#include "cmsis_os.h"
#include "test_motor.h"
#include "RemoteTask.h"
#include "Serial_Debug.h"

#define DEFAULT_TUNE  300
#define RC_RESOLUTION 660

#define PC_HUART huart6
#define DBUS_HUART huart1

#endif

