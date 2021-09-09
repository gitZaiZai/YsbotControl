// Copyright 2016, 亚思信息科技有限责任公司
// All right reserved.
//
// Author: Liu Run Dan ( lrdcz@163.com )
//
// Yasi Robot Motion Control System - 亚思机器人运动控制系统

#ifndef _robot_client_h_
#define _robot_client_h_

#include <stdio.h>
#include <iostream>
#include <sstream>
#include "ClientCommand.h"
#include "ServerData.h"
#include "../TcpSocket.h" 
#include "../ThreadTaskTypeBase.h"
#include <deque>
 

#define  MAX_COMMAND_NUM		(1000)

class RobotClient : public ThreadTaskTypeBase
{
public:
	RobotClient(void);
	~RobotClient(void);

	bool init(int port, const char * addr);
	bool connect_to_server();

	bool send();

	bool receive_all(int timeout_ms = -1);

	bool read_share_mem( Ysbot::IECServerData* servdata );

	bool putInit();
	bool getInit();

	bool putClientCommand ( Ysbot::ClientCommand );
	bool getServerData ( Ysbot::IECServerData* );
	bool getCommandReturn ( unsigned long& );
	bool putCommandInQueue ( Ysbot::ClientCommand );

	bool sendCommandDirect ( Ysbot::ClientCommand ccm          );
	bool sendMessageDirect ( char  *str         );

	unsigned int thread_task(ThreadTaskTypeBase*);

	bool get_server_data( Ysbot::IECServerData* );
	bool is_queue_full ();
	bool is_finished ();
	void set_motion_finished (bool b); 

private:
	bool WriteCmdData(      // 寫入命令數據
		BYTE*      pDest,
		DWORD nCommandCode,
		DWORD dwDataSize,
		const LPVOID pBuf);

	bool ReadCmdData(       // 讀取命令數據
		BYTE*      pDest,
		DWORD dwCommandCode,
		DWORD dwBufSize,
		LPVOID pOutBuf);

    void ADD_PROG_COMMAND(ULINT,unsigned int);
	bool wait_motion_finish (unsigned int edit_line_no);

	DWORD m_dwLastError;
	Ysbot::IECServerData	recv_server_data;
	bool  bConnectSuccess;
	std::deque<Ysbot::ClientCommand> command_list;
	unsigned long iCmdIndex;
	Ysbot::ClientCommand lastCmd;

	unsigned int nBufferSize;
	bool bCommandQueueFull;
	bool bMotionFinished;
	bool bWaitFinishedActive;
	unsigned long nWaitMotionIndex;
 
 	bool use_sharememory;
	TcpSocket* shareMem;

public:
  
 	TcpSocket* clientsock;
	Ysbot::IECServerData	share_server_data;

	char send_buf[BUF_MAX_LEN];
	int  send_buf_len;
	char recv_buf[BUF_MAX_LEN];
	char recv_shareMem[BUF_MAX_LEN];
	unsigned int  recv_buf_len;

	int ConnectionSocket;
	bool isAbortAllCommand; 
	bool bHoldMotion;

	bool put(const std::stringstream &sbuf);
};

#endif

