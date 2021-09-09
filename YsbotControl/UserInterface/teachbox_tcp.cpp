// Copyright 2016, 亚思信息科技有限责任公司
// All right reserved.
//
// Author: Liu Run Dan ( lrdcz@163.com )
//
// Yasi Robot Motion Control System - 亚思机器人运动控制系统

#include "stdafx.h"
#include "teachbox_tcp.h"
#include "windows.h"
#include "afxmt.h"
#include "vnc_2_2.h"
#include "../InOutObject.h"
#include "../WorldModel.h"
#include "../MotionControl.h"

namespace Ysbot {

static HANDLE   hTeachboxTcpThread;
ThreadState teachboxTcpThreadRun=STOPPED;
static SOCKET teachboxTcpclientSocket;
struct sockaddr_in teachboxTcpServerAddr;
unsigned int teachboxKeyValue[2];
unsigned int teachboxLedAndBuzzerValue[2];
unsigned int teachboxMpgSumValue;
unsigned int teachboxMpgIncValue;
unsigned int teachboxJoystickValue[2];
static CCriticalSection  dataLock;
static CRITICAL_SECTION hPollLock;

char chServerAddress[128] = "192.168.11.21";

#define TCP_DATA_LOCK dataLock.Lock()
#define TCP_DATA_UNLOCK dataLock.Unlock()


HWND dlgMsgWnd;

void TcpToSerial(unsigned char *Sou,unsigned char *Des)
{
	Des[0]=Sou[1]<<1; //j1-,j2-,j3-
	if (GETBIT(Sou[0],7))  //Stop按键
		SETBIT(Des[0],0);
	else
		CLRBIT(Des[0],0);

	Des[1]=Sou[2] << 1;  //J4-,J5-,J6-
	if (GETBIT(Sou[1],7))  //J4+
		SETBIT(Des[1],0);
	else
		CLRBIT(Des[1],0);

	Des[2]=Sou[3]<<1; //底部按钮Mot-,ROB,JOG,F/B,STP,V-,V+
	if (GETBIT(Sou[2],7))  //F2
		SETBIT(Des[2],0);
	else
		CLRBIT(Des[2],0);

	Des[3]=Sou[0]; //左边按键
	CLRBIT(Des[3],7);//备用

	Des[4]=Sou[4]; //钥匙，安全开关


}

void SetLedAndBuzzerData(unsigned int data1,unsigned int data2)
{
   TCP_DATA_LOCK;
	teachboxLedAndBuzzerValue[0]=data1;
	teachboxLedAndBuzzerValue[1]=data2;
	TCP_DATA_UNLOCK;
}



/* Closes the network connection and socket in TCP mode */
static void close_tcp(SOCKET socket)
{
      //  shutdown(socket, SHUT_RDWR);
        closesocket(socket);
}


enum ThreadState
eGetPollingThreadState(  )
{
    enum ThreadState eCurState;

    EnterCriticalSection( &hPollLock );
    eCurState = teachboxTcpThreadRun;
    LeaveCriticalSection( &hPollLock );

    return eCurState;
}

static int send_data(SOCKET socket,char *data,unsigned int len)
{
	int ret;
	ret = ::send(socket, data,len, 0);
	 if ((ret == -1) || (ret != len)) {
                ret = PORT_SOCKET_FAILURE;
                printf( "Write port/socket failure\n");
        }      
        return ret;
}

static int recv_data(SOCKET socket,char *data,unsigned int len)
{
	int read_ret;
	read_ret = ::recv(socket, data, len, 0);
	 if (read_ret == 0) {
                        printf("Connection closed\n");
                        return CONNECTION_CLOSED;
         } else if (read_ret < 0) {
                        /* The only negative possible value is -1 */
                        printf("Read port/socket failure\n");
                        return PORT_SOCKET_FAILURE;
        }
	return read_ret;
}


static int SetVersionData(SOCKET socket,char *data,unsigned int len)
{
    int ret;
	memset(data,0,len);
	data[0]=0x1;
	data[4]=0x1;
	ret=send_data(socket,data,0x9);
	if (0x9!=ret)	{
		return ret;   
	}
	return ret; 
}


static int SetJoystickData(SOCKET socket,char *data,unsigned int len)
{
    int ret;
	unsigned int dataTemp[2];
	dataTemp[0]=(data[4]&0xff);
	dataTemp[0]<<=0x8;
	dataTemp[0]|=(data[3]&0xff);
	dataTemp[0]<<=0x8;
	dataTemp[0]|=(data[2]&0xff);
	dataTemp[0]<<=0x8;
	dataTemp[0]|=(data[1]&0xff);

	dataTemp[1]=(data[8]&0xff);
	dataTemp[1]<<=0x8;
	dataTemp[1]|=(data[7]&0xff);
	dataTemp[1]<<=0x8;
	dataTemp[1]|=(data[6]&0xff);
	dataTemp[1]<<=0x8;
	dataTemp[1]|=(data[5]&0xff);


	TCP_DATA_LOCK;
	teachboxJoystickValue[0]=dataTemp[0];
	teachboxJoystickValue[1]=dataTemp[1];
	TCP_DATA_UNLOCK;

	memset(data,0,len);
	data[0]=0x7;

	TCP_DATA_LOCK;
	data[1]=teachboxLedAndBuzzerValue[0]&0xff;
	data[2]=(teachboxLedAndBuzzerValue[0]>>8)&0xff;
	data[3]=(teachboxLedAndBuzzerValue[0]>>16)&0xff;
	data[4]=(teachboxLedAndBuzzerValue[0]>>24)&0xff;

	data[5]=teachboxLedAndBuzzerValue[1]&0xff;
	data[6]=(teachboxLedAndBuzzerValue[1]>>8)&0xff;
	data[7]=(teachboxLedAndBuzzerValue[1]>>16)&0xff;
	data[8]=(teachboxLedAndBuzzerValue[1]>>24)&0xff;
	TCP_DATA_UNLOCK;
	ret=send_data(socket,data,0x9);
	if (0x9!=ret)	{
		return ret;   
	}
	return ret;  
}

static int SetMpgData(SOCKET socket,char *data,unsigned int len)
{
    int ret;
	unsigned int dataTemp[2];
	dataTemp[0]=(data[4]&0xff);
	dataTemp[0]<<=0x8;
	dataTemp[0]|=(data[3]&0xff);
	dataTemp[0]<<=0x8;
	dataTemp[0]|=(data[2]&0xff);
	dataTemp[0]<<=0x8;
	dataTemp[0]|=(data[1]&0xff);

	dataTemp[1]=(data[8]&0xff);
	dataTemp[1]<<=0x8;
	dataTemp[1]|=(data[7]&0xff);
	dataTemp[1]<<=0x8;
	dataTemp[1]|=(data[6]&0xff);
	dataTemp[1]<<=0x8;
	dataTemp[1]|=(data[5]&0xff);


	TCP_DATA_LOCK;
	teachboxMpgSumValue=dataTemp[0];
	teachboxMpgIncValue=dataTemp[1];
	TCP_DATA_UNLOCK;

	::PostMessage(dlgMsgWnd,WM_TECHBOX_MPG,teachboxMpgIncValue,teachboxMpgSumValue);

	memset(data,0,len);
	data[0]=0x7;

	TCP_DATA_LOCK;
	data[1]=teachboxLedAndBuzzerValue[0]&0xff;
	data[2]=(teachboxLedAndBuzzerValue[0]>>8)&0xff;
	data[3]=(teachboxLedAndBuzzerValue[0]>>16)&0xff;
	data[4]=(teachboxLedAndBuzzerValue[0]>>24)&0xff;

	data[5]=teachboxLedAndBuzzerValue[1]&0xff;
	data[6]=(teachboxLedAndBuzzerValue[1]>>8)&0xff;
	data[7]=(teachboxLedAndBuzzerValue[1]>>16)&0xff;
	data[8]=(teachboxLedAndBuzzerValue[1]>>24)&0xff;
	TCP_DATA_UNLOCK;
	ret=send_data(socket,data,0x9);
	if (0x9!=ret)	{
		return ret;   
	}
	return ret;  
}


static int SetKeyData(SOCKET socket,char *data,unsigned int len)
{
    int ret;
	unsigned int dataTemp[2];
	dataTemp[0]=(data[4]&0xff);
	dataTemp[0]<<=0x8;
	dataTemp[0]|=(data[3]&0xff);
	dataTemp[0]<<=0x8;
	dataTemp[0]|=(data[2]&0xff);
	dataTemp[0]<<=0x8;
	dataTemp[0]|=(data[1]&0xff);

	dataTemp[1]=(data[8]&0xff);
	dataTemp[1]<<=0x8;
	dataTemp[1]|=(data[7]&0xff);
	dataTemp[1]<<=0x8;
	dataTemp[1]|=(data[6]&0xff);
	dataTemp[1]<<=0x8;
	dataTemp[1]|=(data[5]&0xff);

	if ( 0 )
	{
		unsigned char databuf[5] = {0};

		databuf[3] = static_cast<unsigned char>( (dataTemp[0] & 0xff000000) >> 24 );     
		databuf[2] = static_cast<unsigned char>( (dataTemp[0] & 0x00ff0000) >> 16 );
		databuf[1] = static_cast<unsigned char>( (dataTemp[0] & 0x0000ff00) >> 8 );
		databuf[0] = static_cast<unsigned char>( dataTemp[0] & 0x000000ff );

		databuf[4] = static_cast<unsigned char>( dataTemp[1] & 0x000000ff );
		unsigned char tcpdata[5] = {0};
 
		Ysbot::TcpToSerial( databuf, tcpdata ); 
		//	TCP_DATA_UNLOCK;

		if ( MWM.is_main_world_model_available() )
		{
			MWM.set_key_list( KeyObjectList( tcpdata, 5) ); 
		}
		if ( SMC.is_main_motion_control_available() )
		{
			SMC.response_key_press();
		}
	}
	else
	{
 		TCP_DATA_LOCK;
		teachboxKeyValue[0]=dataTemp[0];
		teachboxKeyValue[1]=dataTemp[1];
		TCP_DATA_UNLOCK;
 		::PostMessage(dlgMsgWnd,WM_PENDANT_KEYPRESS,(WPARAM)&teachboxKeyValue[0],(LPARAM)&teachboxKeyValue[1]);
 	}
 
//	 ::PostMessage(dlgMsgWnd,WM_PENDANT_KEYPRESS,(WPARAM)&teachboxKeyValue[0],(LPARAM)&teachboxKeyValue[1]);
// 	unsigned char databuf[5] = {0};
//  
// 	databuf[3] = static_cast<unsigned char>( (teachboxKeyValue[0] & 0xff000000) >> 24 );     
// 	databuf[2] = static_cast<unsigned char>( (teachboxKeyValue[0] & 0x00ff0000) >> 16 );
// 	databuf[1] = static_cast<unsigned char>( (teachboxKeyValue[0] & 0x0000ff00) >> 8 );
// 	databuf[0] = static_cast<unsigned char>( teachboxKeyValue[0] & 0x000000ff );
// 
// 	databuf[4] = static_cast<unsigned char>( teachboxKeyValue[1] & 0x000000ff );
//   
// 	unsigned char tcpdata[5] = {0};
// 	TcpToSerial( databuf,tcpdata ); 
// 
// // 	char msg[255] = {'0'};
// // 	sprintf( msg, "%02X %02X %02X %02X %02X \n", tcpdata[0], tcpdata[1], tcpdata[2], tcpdata[3], tcpdata[4]);
// // 	std::cout << msg;
// 
// 	::SendMessage(dlgMsgWnd,WM_PENDANT_KEYPRESS,(WPARAM) tcpdata,0);
 
	//////////////////////////////////////////////////////////////////////////

	memset(data,0,len);
	data[0]=0x7;

	TCP_DATA_LOCK;
	data[1]=teachboxLedAndBuzzerValue[0]&0xff;
	data[2]=(teachboxLedAndBuzzerValue[0]>>8)&0xff;
	data[3]=(teachboxLedAndBuzzerValue[0]>>16)&0xff;
	data[4]=(teachboxLedAndBuzzerValue[0]>>24)&0xff;

	data[5]=teachboxLedAndBuzzerValue[1]&0xff;
	data[6]=(teachboxLedAndBuzzerValue[1]>>8)&0xff;
	data[7]=(teachboxLedAndBuzzerValue[1]>>16)&0xff;
	data[8]=(teachboxLedAndBuzzerValue[1]>>24)&0xff;
	TCP_DATA_UNLOCK;
	ret=send_data(socket,data,0x9);
	if (0x9!=ret)	{
		return ret;   
	}
	return ret;  
}

DWORD           WINAPI
dwPollingThread( LPVOID lpParameter )
{
	 int ret = 0;
	 char query[256];
	  int reuse = 0;
	 // 设置为非阻塞的socket  
   int iMode = 1;  
//    ioctlsocket(teachboxTcpclientSocket, FIONBIO, (u_long FAR*)&iMode);   
	teachboxTcpThreadRun=RUNNING;

	// 超时时间  
    struct timeval tm;  
    tm.tv_sec  = 5;  
    tm.tv_usec = 0;  
//    int ret = -1; 
CONNET_TEACHBOX:
	  //Create Socket
    teachboxTcpclientSocket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if ( teachboxTcpclientSocket == INVALID_SOCKET )
    {
        printf("Create socket failed!\n");
        return FALSE;
    }
	reuse=0x1;
	if (setsockopt(teachboxTcpclientSocket, SOL_SOCKET,SO_REUSEADDR, (char*)&reuse, sizeof(reuse)) < 0)
     {
               printf("setsockopet error!\n");
               return FALSE;
    }
	 int iTimeOut = 3000;
    setsockopt(teachboxTcpclientSocket,SOL_SOCKET,SO_RCVTIMEO,(char*)&iTimeOut,sizeof(iTimeOut));
    setsockopt(teachboxTcpclientSocket,SOL_SOCKET,SO_SNDTIMEO,(char*)&iTimeOut,sizeof(iTimeOut));   

	teachboxTcpServerAddr.sin_family = AF_INET;
    teachboxTcpServerAddr.sin_addr.s_addr = inet_addr(chServerAddress); // TEACHBOX_SERVER_IP
    teachboxTcpServerAddr.sin_port = htons(TEACHBOX_SERVER_PORT);
    memset(teachboxTcpServerAddr.sin_zero, 0x00, 8);
	Sleep(1000);
    ret = connect(teachboxTcpclientSocket,(struct sockaddr*)&teachboxTcpServerAddr, sizeof(teachboxTcpServerAddr));
    if ( ret == SOCKET_ERROR )
    {
         fd_set set;  
        FD_ZERO(&set);  
        FD_SET(teachboxTcpclientSocket, &set);  
  
        if (select(-1, NULL, &set, NULL, &tm) <= 0)  
        {  
            ret = -1; // 有错误(select错误或者超时)  
        }  
        else  
        {  
            int error = -1;  
            int optLen = sizeof(int);  
            getsockopt(teachboxTcpclientSocket, SOL_SOCKET, SO_ERROR, (char*)&error, &optLen);   
              
            // 之所以下面的程序不写成三目运算符的形式， 是为了更直观， 便于注释  
            if (0 != error)  
            {  
                ret = -1; // 有错误  
            }  
            else  
            {  
                ret = 1;  // 无错误  
            }  
        }  
		
        if(teachboxTcpThreadRun) 
			goto CONNET_TEACHBOX;
		else return 0;
    }
	// 设回为阻塞socket
	//iMode = 0;
	//ioctlsocket(teachboxTcpclientSocket, FIONBIO, (u_long FAR*)&iMode); //设置为阻塞模式


    while(RUNNING==teachboxTcpThreadRun)	{
		ret=recv_data(teachboxTcpclientSocket,query,sizeof(query));
		if (0x9==ret)	{
			switch(query[0])
			{
			case TEACHBOX_VERSION_CMD:
				ret=SetVersionData(teachboxTcpclientSocket,query,sizeof(query));
				if (0x9!=ret)	{
					goto CONNET_TEACHBOX;
				}
				break;
			case TEACHBOX_KEY_CMD:
				ret=SetKeyData(teachboxTcpclientSocket,query,sizeof(query));
				if (0x9!=ret)	{
					goto CONNET_TEACHBOX;
				}
				break;
			case TEACHBOX_MPG_CMD:
				ret=SetMpgData(teachboxTcpclientSocket,query,sizeof(query));
				if (0x9!=ret)	{
					goto CONNET_TEACHBOX;
				}
				break;
			case TEACHBOX_JOYSTICK_CMD:
				break;
			case TEACHBOX_IO_CMD:
				break;
			}
		}
		else if(ret<0x0)
		{
			close_tcp(teachboxTcpclientSocket);
			Sleep(1000);
			goto CONNET_TEACHBOX;
		}
	}
	close_tcp(teachboxTcpclientSocket);
	hTeachboxTcpThread=0x0;
    return 0;
}


BOOL
bCreatePollingThread( void )
{
    BOOL            bResult;
  
   if( ( hTeachboxTcpThread = CreateThread( NULL, 0, dwPollingThread, NULL, 0, NULL ) ) == NULL )
   {
            /* Can't create the polling thread. */
            bResult = FALSE;
    }
    return TRUE;
}

void ExitTeachboxTcpThread(void)
{
	if(0x0==hTeachboxTcpThread) return;
	 EnterCriticalSection( &hPollLock );
    teachboxTcpThreadRun=STOPPED;
    LeaveCriticalSection( &hPollLock );


	Sleep(1000);
	WaitForSingleObject(dwPollingThread,INFINITE);
	hTeachboxTcpThread=0x0;

	DeleteCriticalSection(&hPollLock);
}


BOOL RunVncTeachboxTcp( HWND hwnd, const char* hostname )
{
	WSADATA Ws;
    
//     int AddrLen = 0;
// 	BOOL ret = FALSE;
	dlgMsgWnd=hwnd;
	InitializeCriticalSection( &hPollLock );
    //Init Windows Socket
    if ( WSAStartup(MAKEWORD(2,2), &Ws) != 0 )
    {
        printf("Init windows socket failed!\n");
        return FALSE;
    }
  
	sprintf( chServerAddress,"%s", hostname );

	return bCreatePollingThread();
	
}

}