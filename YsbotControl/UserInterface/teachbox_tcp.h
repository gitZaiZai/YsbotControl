// Copyright 2016, 亚思信息科技有限责任公司
// All right reserved.
//
// Author: Liu Run Dan ( lrdcz@163.com )
//
// Yasi Robot Motion Control System - 亚思机器人运动控制系统

#ifndef _Ysbot_teachbox_h_
#define _Ysbot_teachbox_h_

#include <afxsock.h>  

namespace Ysbot {

#define TEACHBOX_SERVER_IP ("192.168.11.21") // ???????????????
#define TEACHBOX_SERVER_PORT 502 


/* Local */
#define COMM_TIME_OUT           -0x0C
#define PORT_SOCKET_FAILURE     -0x0D
#define SELECT_FAILURE          -0x0E
#define TOO_MANY_DATA           -0x0F
#define INVALID_CRC             -0x10
#define INVALID_EXCEPTION_CODE  -0x11
#define CONNECTION_CLOSED       -0x12


BOOL RunVncTeachboxTcp( HWND hwndconst, const char* hostname = NULL );

void ExitTeachboxTcpThread(void);
void SetLedAndBuzzerData(unsigned int data1,unsigned int data2);
void TcpToSerial(unsigned char *Sou,unsigned char *Des);



#define TEACHBOX_VERSION_CMD 0x1
#define TEACHBOX_KEY_CMD 0x2
#define TEACHBOX_MPG_CMD 0x3
#define TEACHBOX_JOYSTICK_CMD 0x4
#define TEACHBOX_IO_CMD 0x5


static enum ThreadState
{
    STOPPED,
    RUNNING,
    SHUTDOWN
} ePollThreadState;


}

#endif