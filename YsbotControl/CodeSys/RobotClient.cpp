// Copyright 2016, 亚思信息科技有限责任公司
// All right reserved.
//
// Author: Liu Run Dan ( lrdcz@163.com )
//
// Yasi Robot Motion Control System - 亚思机器人运动控制系统

#include "stdafx.h"
#include "RobotClient.h"
#include <iomanip>
#include "../WorldModel.h"
#include "../program_dialog.h"

#define DEBUG_SERVER 0

using namespace Ysbot;
using namespace std;

RobotClient::RobotClient(void) : ThreadTaskTypeBase("RobotClient"),bConnectSuccess(false),isAbortAllCommand(false),
	bHoldMotion(false), nBufferSize(13),bCommandQueueFull(false),bMotionFinished(true),bWaitFinishedActive(false),nWaitMotionIndex(0)
{
	send_buf_len = 0;
	recv_buf_len = 0;
    iCmdIndex = 0;
 	use_sharememory = false;
	ZeroMemory(&recv_server_data,sizeof(IECServerData));

//	use_sharememory = true;
	if ( use_sharememory )
	{
		clientsock = new ShareMemeoryConnect();
 	}
	else
		clientsock = new TcpSocketOnce();

	if ( 0 )
	{
		shareMem = new ShareMemeoryConnect();
		shareMem->init_as_client(0,0);
 	}
	{
		shareMem = NULL;
	}
}
 
RobotClient::~RobotClient(void)
{
	stop_task_thread();

	if ( clientsock )
	{
		delete clientsock;
		clientsock = NULL;
	}

	if ( command_list.size() > 0 )
	{
		command_list.clear();
	}

	if ( shareMem )
	{
		delete shareMem;
 	}
}

bool RobotClient::init( int port, const char * addr)
{
	bool bres = clientsock->init_as_client(addr,port);
	return bres;
}

bool RobotClient::connect_to_server()
{
	bool res=false;
	int i=0;

	//if ( receive_all(100) )
	//    return true;
 
	while (i<10 && !res)
	{
		putInit();
		send();
		Sleep(100); 
		res=receive_all(1000);
		i++;
	}
 
	res&=getInit();
  
 	bConnectSuccess = res;

	return res;
}

bool RobotClient::receive_all(int timeout_ms)
{
	bool bres = false;
	if (clientsock)
	{
		bres = clientsock->receive(recv_buf,recv_buf_len,BUF_MAX_LEN,timeout_ms);
	}
	return bres;
}

bool RobotClient::send()
{
 	bool bres = false;
	if ( clientsock )
	{
		bres = clientsock->sendto(send_buf,send_buf_len);
 	}

	if ( !bres ) 
	{
		std::cerr << "Problems n sending ... \n";
		return false;
	}
	// std::cout << "Sended:[" << send_buf << "]\n";
	send_buf_len=0;

 
	return true;
}

bool RobotClient::read_share_mem( IECServerData* servdata )
{
	bool ret = false;
 	if ( shareMem )
	{
		shareMem->receive(recv_buf,recv_buf_len,BUF_MAX_LEN);
		ret = ReadCmdData( (BYTE*)recv_buf, 23, sizeof(IECServerData), servdata );
	}
	return ret;
}

bool RobotClient::put(const std::stringstream &sbuf)
{
	int sbuf_len = strlen(sbuf.str().c_str());

	if ((sbuf_len + send_buf_len) > BUF_MAX_LEN)
	{
		std::cerr << "Can't put more Data in send_buf\n\r";
		return false;
	}

	char * buf_ptr = &send_buf[send_buf_len];

	memcpy(buf_ptr, sbuf.str().c_str(), sbuf_len);

	send_buf_len+= sbuf_len;

	//std::cout << "Put :{" << sbuf.str().c_str() << "} in buffer\n\r" << std::flush;

	return true;
}

bool RobotClient::putInit()
{
	std::stringstream sbuf;
	sbuf << "@INIT ";
	return put(sbuf);
}

bool RobotClient::getInit()
{
	if (recv_buf_len == 0) return false;

	char *ptr= strstr(recv_buf, "@INIT");
	if (ptr != NULL)
		return true;
	else
		return false;
}
 
bool RobotClient::putClientCommand( ClientCommand cmd )
{
	bool ret = WriteCmdData( (BYTE*)send_buf, 23, sizeof(ClientCommand), &cmd );

	if ( ret )
	{
		send_buf_len+= sizeof(ClientCommand)+sizeof(DATA_HEADER)-1;
 	}
 	return ret;
 }

bool RobotClient::WriteCmdData( BYTE* pDest, DWORD nCommandCode, DWORD dwDataSize, const LPVOID pBuf )
{
	// 檢驗數據的合理性
	if (NULL == pDest)
	{
		m_dwLastError = ERROR_NO_MAPFILE;
		SetLastError(ERROR_NO_MAPFILE);
		return false;
	}
	if (NETRGUSER_CMD_NONE == nCommandCode)
	{
		m_dwLastError = ERROR_INVALID_CMDCODE;
		SetLastError(ERROR_INVALID_CMDCODE);
		return false;
	}
	if (dwDataSize > 0 && pBuf == NULL)
	{
		m_dwLastError = ERROR_INVALID_USER_BUFFER;
		SetLastError(ERROR_INVALID_USER_BUFFER);
		return false;
	}
	if ( dwDataSize + sizeof(DATA_HEADER) > BUF_MAX_LEN )
	{
		m_dwLastError = ERROR_BUFFER_OVERFLOW;
		SetLastError(ERROR_BUFFER_OVERFLOW);
		return false;
	}
	// 填寫數據結構
	// 文件頭
	DATA_HEADER dataHeader;
	dataHeader.nCommandCode = nCommandCode;
	dataHeader.dwDataSize = dwDataSize;
	ZeroMemory( pDest, BUF_MAX_LEN );
	memcpy(pDest, &dataHeader, sizeof(DATA_HEADER));
	// 數據塊
	LPDATA_HEADER pData = (LPDATA_HEADER)pDest;
	memcpy(pData->bInfo, pBuf, dwDataSize);
	return true;
}

bool RobotClient::getServerData( IECServerData* servdata)
{
 	bool ret = ReadCmdData( (BYTE*)recv_buf, 23, sizeof(IECServerData), servdata );
	return ret;
}

// 讀取命令數據
bool RobotClient::ReadCmdData( BYTE*  pDest, DWORD dwCommandCode, DWORD dwBufSize, LPVOID pOutBuf)
{
	LPDATA_HEADER pHeader = (LPDATA_HEADER)pDest;
	if (NULL == pHeader)
	{
		m_dwLastError = ERROR_NO_MAPFILE;
		SetLastError(ERROR_NO_MAPFILE);
		return false;
	}
	if (NETRGUSER_CFM_CODE != pHeader->dwConfirmCode)
	{
		m_dwLastError = ERROR_INVALID_CFMCODE;
		SetLastError(ERROR_INVALID_CFMCODE);
		return false;
	}
	if (NETRGUSER_CMD_NONE == pHeader->nCommandCode)
	{
		m_dwLastError = ERROR_INVALID_CMDCODE;
		SetLastError(ERROR_INVALID_CMDCODE);
		return false;
	}
	if (pHeader->dwDataSize > dwBufSize)
	{
		m_dwLastError = ERROR_BUFFER_OVERFLOW;
		SetLastError(ERROR_BUFFER_OVERFLOW);
		return false;
	}
	if (pHeader->nCommandCode != dwCommandCode)
	{
		m_dwLastError = ERROR_INVALID_CMDCODE;
		SetLastError(ERROR_INVALID_CMDCODE);
		return false;
	}

	ZeroMemory(pOutBuf, dwBufSize);
	// 拷貝數據到緩衝區
	memcpy(pOutBuf, pHeader->bInfo, pHeader->dwDataSize);
	pHeader->dwConfirmCode = 0;
	return true;
}

bool RobotClient::getCommandReturn(unsigned long& iCmdRet)
{
	char *ptr= strstr(recv_buf, "@CMDRET");
	if (ptr == NULL) return false;

    unsigned long _ret = 0;
	int res=sscanf( ptr, "@CMDRET[ %ld ]", &_ret );

	if (res < 1) {
		std::cerr << "Problems on parsing: {" << ptr << "}\n\r";
		return false;
	}

	iCmdRet = _ret;
	return true;
}
 
unsigned int RobotClient::thread_task( ThreadTaskTypeBase* pParam)
{
	RobotClient *pRobotClient = ( RobotClient* )pParam;
    ClientCommand currCmd;
    IECServerData server_data;
    ZeroMemory(&server_data,sizeof(IECServerData));
	ZeroMemory(&currCmd,sizeof(Ysbot::ClientCommand));
	currCmd.Acceleration = 1000;
	currCmd.AccFactor = 0.2;
	currCmd.JerkFactor = 1.0;
	currCmd.Deceleration = 1000;
	currCmd.Jerk = 10000;

	currCmd.iCommandIndex = 0;
	lastCmd.iCommandIndex = 0;

	DWORD dwStart   = GetTickCount();
	DWORD dwCurrent = GetTickCount();
	DWORD dwDiff = 0;

// 	deque<ClientCommand>::iterator cit;
// 	deque<ClientCommand>::iterator cit_end = command_list.end();
	 
	CString cmdmsg;
	 
	unsigned int iLastActionCmdIndex = 0;
	bool bIecQueueFull = false;
	bool bLastIecQueueFull = false;
	bool bSendCommand = false;
	bool bRecvReturn = false;
    ULINT iMotCmdIndexBeforeHold = 0;
	ULINT currMotCmdIndex = 0;
	ULINT iCmdRet = 0;
	bool bInitIndex = false;

	cout << setiosflags(ios::fixed) << setprecision(4);

	while( !pRobotClient->exitFlag )
	{
		currCmd.CmdType = IEC_CMD_ILLEGAL;
        bRecvReturn  = false;
		bSendCommand = false; 
		bIecQueueFull = false;

 		bool bres = pRobotClient->receive_all(5000);
 		if ( bres )
		{
			//ZeroMemory(&server_data,sizeof(IECServerData));
 			bool retServerData = pRobotClient->getServerData( &server_data );
            if ( !retServerData )
 				continue;
              
			if ( !bInitIndex )
			{
				iCmdIndex = server_data.iCommandIndex;
				lastCmd.iCommandIndex = server_data.iCommandIndex;
				bInitIndex = true;
			}

#if DEBUG_SERVER
			cout << "[ " << server_data << " ]" << '\n'; 	// error 		 
#endif
  
			// send command to server.
			Lock();
			recv_server_data  = server_data;
 			if ( command_list.size() > 0 )
			{
				currCmd = command_list.front();
			}
  
 			UnLock();

			// command is executed, and return message from sever.
 			if ( server_data.iCommandIndex == lastCmd.iCommandIndex )
			{
				bRecvReturn = true;
				if ( server_data.errID == 0 )
				{			
					ULINT iCmdRet = server_data.iCommandRet; //action command queue index from server.
					if ( iCmdRet > 0 )  
					{
						if ( iLastActionCmdIndex != server_data.iCommandIndex )
						{
							unsigned int lineno  = lastCmd.iSourceLineNo;
							ADD_PROG_COMMAND( iCmdRet, lineno );
							// pushback list. iCmdRet, lineno
							iLastActionCmdIndex = server_data.iCommandIndex;
							printf("\n Acitve command ID = %d\n",iLastActionCmdIndex);
#if DEBUG_SERVER
							cout << "save action command motion index " << iCmdRet << " and source line no " << lineno << " to list.\n";

#endif
						}
					}
				}
				else
				{
					std::string errstring = "";
					for( int i = 0; i < ARRAY_SIZE(error_description); ++i )
					{
						if ( error_description[i].nErrorCode == server_data.errID )
						{
							errstring = error_description[i].lpszDesc;
							break;
						}
		
					}
					if ( server_data.errID == 50104 ) // 超时错误，允许指令进入队列，reset
					{
						isAbortAllCommand = false;
					}
#if DEBUG_SERVER
					cout << "fail execute command! errorID = " << server_data.errID << ' ' << errstring << '\n'; 
#endif
				}

				if (lastCmd.CmdType == IEC_CMD_GROUPSTOP || lastCmd.CmdType == IEC_CMD_GROUPHALT || lastCmd.CmdType == IEC_CMD_STOPJOG  )
				{
					bHoldMotion = false;
					isAbortAllCommand = false;
				}
				else if ( lastCmd.CmdType == IEC_CMD_SETBUFFERSIZE )
				{
// 					nBufferSize = lastCmd.iParams[0];
// 					nBufferSize = nBufferSize > 64 ? 64 :( nBufferSize < 15 ? 15 : nBufferSize ); 
					 
				}
   			}
			else
			{
#if DEBUG_SERVER
				cout << "command not return " << lastCmd.iCommandIndex << ' ' << '\n';
#endif
			}

// 			if ( currCmd.CmdType == CMD_WAITFINISHED )
// 			{
// 				if ( !bMotionFinished )
// 				{
// 					bIecQueueFull = true;
// 				}
// 				if ( server_data.group_state == Standby )
// 				{
// 					Lock();
// 					bMotionFinished = true;
// 					if ( command_list.size() > 0 )
// 					{
// 						command_list.pop_front();
// 					}
// 					UnLock();	
// 				}
// 			}
            
			// whether iec motion queue is full
			if ( server_data.group_state == Moving )
			{
				currMotCmdIndex = server_data.iCurrMotionIndex;
				iCmdRet = server_data.iCommandRet;

				if ( currMotCmdIndex > 0 && iCmdRet > 0 && iCmdRet - currMotCmdIndex > nBufferSize )
				{
 					bIecQueueFull = true;
				}
				else // hold-->continue, occurs waiting.
				{
					if ( currMotCmdIndex > 0 && currMotCmdIndex == iMotCmdIndexBeforeHold ) 
					{
						//TRACE(_T("continue delay start\n"));
						bIecQueueFull = bLastIecQueueFull;
					}
				}

 				// check if hightlight lineno
				// for( int i = 0; i < list.size(); ++i )
				//.......... 
 			}

			if ( currCmd.CmdType == IEC_CMD_GROUPINTERRUPT )
			{
				if ( currMotCmdIndex != 0 )
				{
					iMotCmdIndexBeforeHold = currMotCmdIndex;
					bLastIecQueueFull = bIecQueueFull;
				}
  			}
               
 			if ( server_data.iMotionState == 0 ) //motion idle
			{
				if ( bIecQueueFull )
				{
					if ( currCmd.CmdType == IEC_CMD_GROUPINTERRUPT || currCmd.CmdType == IEC_CMD_GROUPCONTINUE || 
						 currCmd.CmdType == IEC_CMD_POWEROFF       || currCmd.CmdType == IEC_CMD_POWERON )
					{
						bSendCommand = true;
					}
				}
				else
				{
					if ( lastCmd.CmdType == IEC_CMD_GROUPINTERRUPT )
					{
						if ( currCmd.CmdType == IEC_CMD_GROUPCONTINUE )
							bSendCommand = true;
 					}
					else if ( currCmd.CmdType != IEC_CMD_ILLEGAL )
					{
						bSendCommand = true;
					}
 				}
			}
			else if ( server_data.iMotionState == 100 ) // execute command.
			{
#if DEBUG_SERVER
				cout << "wait: command return.\n";
#endif
				dwCurrent = GetTickCount();
				dwDiff = dwCurrent - dwStart;
			}
  
			bSendCommand = bSendCommand && bRecvReturn; // need receive return value before send command.

			if ( currCmd.CmdType == IEC_CMD_GROUPHALT      || currCmd.CmdType == IEC_CMD_GROUPSTOP || 
				 currCmd.CmdType == IEC_CMD_RESETGROUP     || currCmd.CmdType == IEC_CMD_POWEROFF  ||
				 currCmd.CmdType == IEC_CMD_GROUPINTERRUPT || currCmd.CmdType == IEC_CMD_STOPJOG      )
			{
				bSendCommand = true;
			} 
			else
			{
				if ( server_data.errID != 0 )
				{
					bSendCommand = false;
				}
			}

     	}
		else
		{
#if DEBUG_SERVER
			cout << "revc nothing! \n";
#endif
		}
        
		// for debug.

 		Lock();

		if ( server_data.errID != 0 )
		{
			if ( command_list.size() > 0 )
 				command_list.clear();  
 		}

		if ( bSendCommand )
		{
 			iCmdIndex++;
			currCmd.iCommandIndex = iCmdIndex;

			dwStart = GetTickCount();	

			if ( command_list.size() > 0 )
			{
				if ( currCmd.CmdType == IEC_CMD_GROUPSTOP || currCmd.CmdType == IEC_CMD_GROUPHALT || 
					 currCmd.CmdType == IEC_CMD_POWEROFF  || currCmd.CmdType == IEC_CMD_STOPJOG )
				{
					command_list.clear();
				}
				else
				{
					command_list.pop_front();
				}
 			}

			if ( bCommandQueueFull && command_list.size() < MAX_COMMAND_NUM - 10 )
			{
 				bCommandQueueFull = false;
			}
 		}
		else
		{
			currCmd.CmdType = IEC_CMD_ILLEGAL;
  		}

		if ( use_sharememory )
		{
			if ( currCmd.CmdType != IEC_CMD_ILLEGAL && bSendCommand )
			{
				putClientCommand(currCmd);
				send();
			}
		}
		else
		{
			putClientCommand(currCmd);
			send();
		}
 
		UnLock();

		if ( currCmd.CmdType != IEC_CMD_ILLEGAL )
 			lastCmd = currCmd;

		if ( bSendCommand )
		{

#if DEBUG_SERVER
			cmdmsg.Format(_T("Normal: No.%d command sended, type = %d size = %d.\n"), currCmd.iCommandIndex, currCmd.CmdType, command_list.size() );
			TRACE(cmdmsg); 
#endif
		}
  
		Sleep(1); // important for another thread to read variable.
	}

	return 0;
}

bool RobotClient::putCommandInQueue ( Ysbot::ClientCommand cmd )
{
	bool ret = false;

	if ( !bConnectSuccess )
	{
		cout << "failed to connect PLC server!\n";
		return false;
	}
 	 
	if( cmd.CmdType == IEC_CMD_ILLEGAL )
		return false;
	
	if ( isAbortAllCommand )
 		return false;

	// in auto play mode valid.
	if ( ( bHoldMotion && 
		   cmd.CmdType != IEC_CMD_GROUPCONTINUE && 
		   cmd.CmdType != IEC_CMD_GROUPSTOP     && 
		   cmd.CmdType != IEC_CMD_GROUPHALT     &&
		   cmd.CmdType != IEC_CMD_POWEROFF    ) ||
		 ( !bHoldMotion && cmd.CmdType == IEC_CMD_GROUPCONTINUE )    )
	{
		return false;
	}
 
	Lock();
  
	if ( recv_server_data.errID != 0 ) // have error.
	{
 		if ( cmd.CmdType != IEC_CMD_RESETGROUP ) //cmd.CmdType != CMD_GROUPSTOP && cmd.CmdType != CMD_GROUPHALT &&
		{
			if ( command_list.size() > 0 )
			{
				command_list.clear();
 			}
			UnLock();
			return false;
		}
	}
  
	if ( cmd.CmdType == IEC_CMD_GROUPINTERRUPT || cmd.CmdType == IEC_CMD_GROUPCONTINUE ||
		 cmd.CmdType == IEC_CMD_POWERON        ||  cmd.CmdType == IEC_CMD_POWEROFF  )
	{
		if ( cmd.CmdType == IEC_CMD_GROUPINTERRUPT )
		{
			bHoldMotion = true;
		}
		else if(  cmd.CmdType == IEC_CMD_GROUPCONTINUE  )
		{
			bHoldMotion = false;
		}

		if ( command_list.size() < MAX_COMMAND_NUM - 4 )
		{
 			command_list.push_front(cmd);
			ret = true;
		}
	}
	else if ( cmd.CmdType == IEC_CMD_GROUPSTOP || cmd.CmdType == IEC_CMD_GROUPHALT || cmd.CmdType == IEC_CMD_STOPJOG  )
	{
		isAbortAllCommand = true;
		command_list.push_front(cmd);
		ret = true;
	}
	else	// 2 less to save space for group hold command
	{
		if ( command_list.size() < MAX_COMMAND_NUM - 10 )
		{
			command_list.push_back(cmd);
			ret = true;
			bCommandQueueFull = false;
 		}
		else
		{
			bCommandQueueFull = true;

			if ( command_list.size() < MAX_COMMAND_NUM - 2 )
			{
				ret = true;
				command_list.push_back(cmd);
			}
		}
	}
 

	UnLock();

	if ( !ret )
	{
		//cerr << ("command queue is full, too many commands\n");
	}
	else
	{
 		CString listmsg;
		listmsg.Format(_T("command queue %d, size = %d.\n"),(int)bCommandQueueFull, command_list.size() );
		TRACE(listmsg); 
	}

	return ret;
 
}

bool RobotClient::sendCommandDirect( Ysbot::ClientCommand ccm )
{
	Lock();

	if ( command_list.size() > 0 )
 		command_list.clear();
  	 
	iCmdIndex++;
	ccm.iCommandIndex = iCmdIndex;

	putClientCommand(ccm);
	send();

	UnLock();

	lastCmd = ccm;

	return true;
}

bool RobotClient::sendMessageDirect( char *str )
{
	std::stringstream sbuf;
	sbuf << str;
	Lock();
	put(sbuf);
	send();
	UnLock();
	return true;
}

bool RobotClient::get_server_data( Ysbot::IECServerData* pSerData )
{
// 	*pSerData = share_server_data;
//   	cout << "[ " << share_server_data.group_state << " | " << share_server_data.errID << " ]" << '\n'; 	// error
	Lock();
	*pSerData = recv_server_data;
	UnLock();
	return true;
}

// script buffer state, not motion library buffer.
bool RobotClient::is_queue_full()
{
 	IECServerData sd;
	get_server_data(&sd);

 	if ( sd.errID == 0 )
 		return bCommandQueueFull;
 	else
		return false;

	
}

bool RobotClient::is_finished()
{
 	return bMotionFinished;
}

void RobotClient::set_motion_finished(bool b)
{
	bMotionFinished = b; 
}

void RobotClient::ADD_PROG_COMMAND(ULINT cmdindex, unsigned int lineno)
{
	if( cmdindex > 0 )
	{
		ProgCommandInfo g_pci = MWM.get_prog_command_info();

		unsigned int index = g_pci.nCmdCount;
		static unsigned long cnttest = 0;
		if ( index > MAX_PROG_COMMAND-1 )
		{
			index = 0;
			g_pci.nCurCmd = 0;
			g_pci.nCmdCount = 0;
			printf("\n******* reset pointer command line to 0 [ times = %d ] **********\n\n", ++cnttest );
 		}
		g_pci.index_line_list[index].command_index = cmdindex;
		g_pci.index_line_list[index].line_no = lineno-1;

 		g_pci.nCmdCount++;
		MWM.set_prog_command_info(g_pci);
 
		// avoid runtime-51 error, buffer overwrite.
// 		if ( index == MAX_PROG_COMMAND-1 )
// 		{
// 			if ( wait_motion_finish(lineno-1) ) // lock thread, wait all motion finished
// 			{
//  				memset(&g_pci, 0, sizeof(g_pci));
// 				MWM.set_prog_command_info(g_pci);
//  			}
// 		}
	}
}

bool RobotClient::wait_motion_finish(unsigned int edit_line_no)
{
 	cout << "wait motion stop 2h---------" << endl; // ??? 2 hours

	if ( MWM.is_main_world_model_available() )
	{
		MWM.set_wait_signal(true);

		unsigned int cnt = 0;
		do // stop 状态更新有延迟
		{
 			if ( ProgramEditor::is_running_flag == false )
			{
				cout << "wait end 22 ---------" << endl;
				break;
			}
 
			if ( MWM.get_robot_state().iGroupStatus == GrpHold )
			{
 				continue;
			}
 
			MotionCoreInfo mc = MWM.get_robot_state().mc_info;
			if ( mc.iMotionStatus == MotionCoreInfo::msSTOP )
			{
				break;
			}
 		}while(true);
	}
 	MWM.set_wait_signal(false);
	if ( ProgramEditor::is_running_flag )
	{
		program_dialog->editor_.HighlightLine(edit_line_no-1);
	}

	return true;
	 
}