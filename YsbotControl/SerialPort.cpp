/*
**	FILENAME			CSerialPort.cpp
**
**	PURPOSE				This class can read, write and watch one serial port.
**						It sends messages to its owner when something happends on the port
**						The class creates a thread for reading and writing so the main
**						program is not blocked.
**
**	CREATION DATE		15-09-1997
**	LAST MODIFICATION	12-11-1997
**
**	AUTHOR				Remon Spekreijse
**
**
*/   

#include "stdafx.h"
#include "SerialPort.h"

#include <assert.h>
#include <iostream>
#include "Time.h"
#include "InOutObject.h"
#include "MotionControl.h"
#include "program_dialog.h"
#include "WorldModel.h"
#include "YsbotControlDlg.h"
#include "UIDialogs/state_dialog.h"

using namespace std;
//User defined
int m_nDataLength = 0; 
//
// Constructor
//
// CSerialPort* CSerialPort::singleton = NULL;
// 
// CSerialPort* CSerialPort::getInstance()
// {
// 	if (singleton == NULL) 
// 	{
// 		singleton = new CSerialPort();
// 		static PortCleaner cl;
// 	}
// 	return singleton;
// }
bool CSerialPort::second_page  = false;  
bool CSerialPort::enable_press = false;  
bool CSerialPort::receive_message = false;  
int  CSerialPort::key_switch   = -1;

CSerialPort::CSerialPort()
{
	m_hComm = NULL;

	// initialize overlapped structure members to zero
	m_ov.Offset = 0;
	m_ov.OffsetHigh = 0;

	// create events
	m_ov.hEvent = NULL;
	m_hWriteEvent = NULL;
	m_hShutdownEvent = NULL;

	m_szWriteBuffer = NULL;

	m_bThreadAlive = FALSE;
	exitFlag = false;
	m_flag_open = false;
}

//
// Delete dynamic memory
//
CSerialPort::~CSerialPort()
{
// 	exitFlag = true;
//  	do
// 	{
// 		SetEvent(m_hShutdownEvent);
// 		Sleep(1);
// 	} while (m_bThreadAlive);
    StopMonitoring();
	if ( m_flag_open )
	{
		DeleteCriticalSection(&m_csCommunicationSync);
	}

	if( m_hComm != NULL)
	{
		CloseHandle(m_hComm);
		m_hComm = NULL;
	}
	
	if( m_hShutdownEvent != NULL)
		CloseHandle(m_hShutdownEvent);
	
	if( m_ov.hEvent != NULL)
		CloseHandle(m_ov.hEvent);
	
	if( m_hWriteEvent != NULL)
		CloseHandle(m_hWriteEvent);

	if (m_szWriteBuffer)
	{
		delete [] m_szWriteBuffer;
		m_szWriteBuffer = NULL;
	}
}

//
// Initialize the port. This can be port 1 to 4.
//
BOOL CSerialPort::InitPort(CWnd* pPortOwner,
						   UINT  portnr,		// portnumber (1..4)
						   UINT  baud,			// baudrate
						   char  parity,		// parity 
						   UINT  databits,		// databits 
						   UINT  stopbits,		// stopbits 
						   DWORD dwCommEvents,	// EV_RXCHAR, EV_CTS etc
						   UINT  writebuffersize)	// size to the writebuffer
{
	assert(portnr > 0 && portnr < MaxSerialPortNum+1);
	assert(pPortOwner != NULL);


	// create events
	if( m_ov.hEvent != NULL)
		ResetEvent(m_ov.hEvent);
	else
		m_ov.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	
	if (m_hWriteEvent != NULL)
		ResetEvent(m_hWriteEvent);
	else
		m_hWriteEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	
	if (m_hShutdownEvent != NULL)
		ResetEvent(m_hShutdownEvent);
	else
		m_hShutdownEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	// initialize the event objects
	m_hEventArray[0] = m_hShutdownEvent;	// highest priority
	m_hEventArray[1] = m_ov.hEvent;
	m_hEventArray[2] = m_hWriteEvent;

	// initialize critical section
	InitializeCriticalSection(&m_csCommunicationSync);
	
	m_pOwner = pPortOwner;

	if (m_szWriteBuffer != NULL)
		delete [] m_szWriteBuffer;
	m_szWriteBuffer = new char[writebuffersize];

	m_nPortNr = portnr;

	m_nWriteBufferSize = writebuffersize;
	m_dwCommEvents = dwCommEvents;

	BOOL bResult = FALSE;
	char *szPort = new char[50];
	char *szBaud = new char[50];

	// now it critical!
	EnterCriticalSection(&m_csCommunicationSync);

	// if the port is already opened: close it
	if (m_hComm != NULL)
	{
		CloseHandle(m_hComm);
		m_hComm = NULL;
	}

	// prepare port strings
	sprintf(szPort, "\\\\.\\COM%d", portnr);
	sprintf(szBaud, "baud=%d parity=%c data=%d stop=%d", baud, parity, databits, stopbits);

	// get a handle to the port
	m_hComm = CreateFileA(szPort,						// communication port string (COMX)
					     GENERIC_READ | GENERIC_WRITE,	// read/write types
					     0,								// comm devices must be opened with exclusive access
					     NULL,							// no security attributes
					     OPEN_EXISTING,					// comm devices must use OPEN_EXISTING
					     FILE_FLAG_OVERLAPPED,			// Async I/O
					     0);							// template must be 0 for comm devices

	if (m_hComm == INVALID_HANDLE_VALUE)
	{
		// port not found
		delete [] szPort;
		delete [] szBaud;
		delete [] m_szWriteBuffer;
		m_szWriteBuffer = NULL;
		m_flag_open = false;
		DeleteCriticalSection(&m_csCommunicationSync);
		return FALSE;
	}


	// set the timeout values
	m_CommTimeouts.ReadIntervalTimeout = 1000;
	m_CommTimeouts.ReadTotalTimeoutMultiplier = 1000;
	m_CommTimeouts.ReadTotalTimeoutConstant = 1000;
	m_CommTimeouts.WriteTotalTimeoutMultiplier = 1000;
	m_CommTimeouts.WriteTotalTimeoutConstant = 1000;

	// configure
	if (SetCommTimeouts(m_hComm, &m_CommTimeouts))
	{						   
		if (SetCommMask(m_hComm, dwCommEvents))
		{
			if (GetCommState(m_hComm, &m_dcb))
			{
 				m_dcb.fRtsControl = RTS_CONTROL_ENABLE;		// set RTS bit high!
				if ( BuildCommDCBA( szBaud, &m_dcb) )
				{
					if (SetCommState(m_hComm, &m_dcb))
						; // normal operation... continue
					else
						ProcessErrorMessage("SetCommState()");
				}
				else
					ProcessErrorMessage("BuildCommDCB()");
			}
			else
				ProcessErrorMessage("GetCommState()");
		}
		else
			ProcessErrorMessage("SetCommMask()");
	}
	else
		ProcessErrorMessage("SetCommTimeouts()");

	delete [] szPort;
	delete [] szBaud;

	// flush the port
	PurgeComm(m_hComm, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);

	// release critical section
	LeaveCriticalSection(&m_csCommunicationSync);

	TRACE("Initialisation for communicationport %d completed.\nUse Startmonitor to communicate.\n", portnr);

	m_flag_open = true;
	return TRUE;
}

//
//  The CommThread Function.
//
UINT CSerialPort::CommThread(LPVOID pParam)
{
	// Cast the void pointer passed to the thread back to
	// a pointer of CSerialPort class
	CSerialPort *port = (CSerialPort*)pParam;

	// Set the status variable in the dialog class to
	// TRUE to indicate the thread is running.
	port->m_bThreadAlive = TRUE;	
		
	// Misc. variables
	DWORD BytesTransfered = 0; 
	DWORD Event = 0;
	DWORD CommEvent = 0;
	DWORD dwError = 0;
	COMSTAT comstat = {0};
	BOOL  bResult = TRUE;
		
	// Clear comm buffers at startup
	if (port->m_hComm)		// check if the port is opened
		PurgeComm(port->m_hComm, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);

	// begin forever loop.  This loop will run as long as the thread is alive.
	for (;;) 
	{ 

		// Make a call to WaitCommEvent().  This call will return immediatly
		// because our port was created as an async port (FILE_FLAG_OVERLAPPED
		// and an m_OverlappedStructerlapped structure specified).  This call will cause the 
		// m_OverlappedStructerlapped element m_OverlappedStruct.hEvent, which is part of the m_hEventArray to 
		// be placed in a non-signaled state if there are no bytes available to be read,
		// or to a signaled state if there are bytes available.  If this event handle 
		// is set to the non-signaled state, it will be set to signaled when a 
		// character arrives at the port.

		// we do this for each port!
		if ( port->exitFlag )
		{
			break;
		}

		bResult = WaitCommEvent(port->m_hComm, &Event, &port->m_ov);

		if (!bResult)  
		{ 
			// If WaitCommEvent() returns FALSE, process the last error to determin
			// the reason..
			switch (dwError = GetLastError()) 
			{ 
			case ERROR_IO_PENDING: 	
				{ 
					// This is a normal return value if there are no bytes
					// to read at the port.
					// Do nothing and continue
					break;
				}
			case 87:
				{
					// Under Windows NT, this value is returned for some reason.
					// I have not investigated why, but it is also a valid reply
					// Also do nothing and continue.
					break;
				}
			default:
				{
					// All other error codes indicate a serious error has
					// occurred.  Process this error.
					port->ProcessErrorMessage("WaitCommEvent()");
					break;
				}
			}
		}
		else
		{
			// If WaitCommEvent() returns TRUE, check to be sure there are
			// actually bytes in the buffer to read.  
			//
			// If you are reading more than one byte at a time from the buffer 
			// (which this program does not do) you will have the situation occur 
			// where the first byte to arrive will cause the WaitForMultipleObjects() 
			// function to stop waiting.  The WaitForMultipleObjects() function 
			// resets the event handle in m_OverlappedStruct.hEvent to the non-signaled state
			// as it returns.  
			//
			// If in the time between the reset of this event and the call to 
			// ReadFile() more bytes arrive, the m_OverlappedStruct.hEvent handle will be set again
			// to the signeled state. When the call to ReadFile() occurs, it will 
			// read all of the bytes from the buffer, and the program will
			// loop back around to WaitCommEvent().
			// 
			// At this point you will be in the situation where m_OverlappedStruct.hEvent is set,
			// but there are no bytes available to read.  If you proceed and call
			// ReadFile(), it will return immediatly due to the async port setup, but
			// GetOverlappedResults() will not return until the next character arrives.
			//
			// It is not desirable for the GetOverlappedResults() function to be in 
			// this state.  The thread shutdown event (event 0) and the WriteFile()
			// event (Event2) will not work if the thread is blocked by GetOverlappedResults().
			//
			// The solution to this is to check the buffer with a call to ClearCommError().
			// This call will reset the event handle, and if there are no bytes to read
			// we can loop back through WaitCommEvent() again, then proceed.
			// If there are really bytes to read, do nothing and proceed.
		
			bResult = ClearCommError(port->m_hComm, &dwError, &comstat);

			if (comstat.cbInQue == 0)
				continue;
		}	// end if bResult

		// Main wait function.  This function will normally block the thread
		// until one of nine events occur that require action.
		Event = WaitForMultipleObjects(3, port->m_hEventArray, FALSE, INFINITE);

		switch (Event)
		{
		case 0:
			{
				// Shutdown event.  This is event zero so it will be
				// the highest priority and be serviced first.
 				port->m_bThreadAlive = FALSE;
				
				// Kill this thread.  break is not needed, but makes me feel better.
				AfxEndThread(100);
				break;
			}
		case 1:	// read event
			{
				GetCommMask(port->m_hComm, &CommEvent);
// 				if (CommEvent & EV_CTS)
// 					::SendMessage(port->m_pOwner->m_hWnd, WM_COMM_CTS_DETECTED, (WPARAM) 0, (LPARAM) port->m_nPortNr);
// 				if (CommEvent & EV_RXFLAG)
// 					::SendMessage(port->m_pOwner->m_hWnd, WM_COMM_RXFLAG_DETECTED, (WPARAM) 0, (LPARAM) port->m_nPortNr);
// 				if (CommEvent & EV_BREAK)
// 					::SendMessage(port->m_pOwner->m_hWnd, WM_COMM_BREAK_DETECTED, (WPARAM) 0, (LPARAM) port->m_nPortNr);
// 				if (CommEvent & EV_ERR)
// 					::SendMessage(port->m_pOwner->m_hWnd, WM_COMM_ERR_DETECTED, (WPARAM) 0, (LPARAM) port->m_nPortNr);
// 				if (CommEvent & EV_RING)
// 					::SendMessage(port->m_pOwner->m_hWnd, WM_COMM_RING_DETECTED, (WPARAM) 0, (LPARAM) port->m_nPortNr);
				
				if (CommEvent & EV_RXCHAR)
					// Receive character event from port.
					ReceiveChar(port, comstat);
					
				break;
			}  
		case 2: // write event
			{
				// Write character event from port
				WriteChar(port,m_nDataLength);
				break;
			}

		} // end switch

	} // close forever loop

	port->m_bThreadAlive = FALSE;	

	return 0;
}

//
// start comm watching
//
BOOL CSerialPort::StartMonitoring() 
{
 	if ( !m_flag_open )
	{
		return FALSE;
	}

	if (!(m_Thread = AfxBeginThread(CommThread, this)))
		return FALSE;
	TRACE("Thread started CommThread \n");
 	return TRUE;	
}

//
// Restart the comm thread
//
BOOL CSerialPort::RestartMonitoring()
{
	if ( !m_flag_open )
	{
		return FALSE;
	}

	TRACE("Thread resumed\n");
	m_Thread->ResumeThread();
	return TRUE;	
}
 
//
// Suspend the comm thread
//
BOOL CSerialPort::StopMonitoring()
{
// 	TRACE("Thread suspended\n");
// 	m_Thread->SuspendThread(); 
	if ( m_flag_open )
	{
		exitFlag = true;

		do
		{
			SetEvent(m_hShutdownEvent);
			Sleep(1);
		}while( m_bThreadAlive );

		TRACE("CSerialPort thread ended!\n");
	}
 	return TRUE;	
}


//
// If there is a error, give the right message
//
void CSerialPort::ProcessErrorMessage(char* ErrorText)
{
	char *Temp = new char[200];
	
	LPVOID lpMsgBuf;

	FormatMessageA( 
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPSTR) &lpMsgBuf,
		0,
		NULL 
	);

	sprintf(Temp, "WARNING:  %s Failed with the following error: \n%s\nPort: %d\n", (char*)ErrorText, lpMsgBuf, m_nPortNr); 
	MessageBoxA(NULL, Temp, "Application Error", MB_ICONSTOP);

	LocalFree(lpMsgBuf);
	delete[] Temp;
}

//
// Write a character.
//
void CSerialPort::WriteChar(CSerialPort* port,int length)
{
	BOOL bWrite = TRUE;
	BOOL bResult = TRUE;

	DWORD BytesSent = 0;

	ResetEvent(port->m_hWriteEvent);

	// Gain ownership of the critical section
	EnterCriticalSection(&port->m_csCommunicationSync);

	if (bWrite)
	{
		// Initialize variables
		port->m_ov.Offset = 0;
		port->m_ov.OffsetHigh = 0;

		// Clear buffer
		PurgeComm(port->m_hComm, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);

		bResult = WriteFile(port->m_hComm,							// Handle to COMM Port
							port->m_szWriteBuffer,					// Pointer to message buffer in calling finction
							length,									// Length of message to send
							//strlen((char*)port->m_szWriteBuffer),
							&BytesSent,								// Where to store the number of bytes sent
							&port->m_ov);							// Overlapped structure

		// deal with any error codes
		if (!bResult)  
		{
			DWORD dwError = GetLastError();
			switch (dwError)
			{
				case ERROR_IO_PENDING:
					{
						// continue to GetOverlappedResults()
						BytesSent = 0;
						bWrite = FALSE;
						break;
					}
				default:
					{
						// all other error codes
						port->ProcessErrorMessage("WriteFile()");
					}
			}
		} 
		else
		{
			LeaveCriticalSection(&port->m_csCommunicationSync);
		}
	} // end if(bWrite)

	if (!bWrite)
	{
		bWrite = TRUE;
	
		bResult = GetOverlappedResult(port->m_hComm,	// Handle to COMM port 
									  &port->m_ov,		// Overlapped structure
									  &BytesSent,		// Stores number of bytes sent
									  TRUE); 			// Wait flag

		LeaveCriticalSection(&port->m_csCommunicationSync);

		// deal with the error code 
		if (!bResult)  
		{
			port->ProcessErrorMessage("GetOverlappedResults() in WriteFile()");
		}	
	} // end if (!bWrite)

	// Verify that the data size send equals what we tried to send
// 	if (BytesSent != strlen((char*)port->m_szWriteBuffer))
// 	{
// 		TRACE("WARNING: WriteFile() error.. Bytes Sent: %d; Message Length: %d\n", BytesSent, strlen((char*)port->m_szWriteBuffer));
// 	}

	if (BytesSent != length )
	{
		TRACE( "WARNING: WriteFile() error.. Bytes Sent: %d; Message Length: %d\n", BytesSent, length );
	}
}

//
// Character received. Inform the owner
//
void CSerialPort::ReceiveChar(CSerialPort* port, COMSTAT comstat)
{
	BOOL  bRead = TRUE; 
	BOOL  bResult = TRUE;
	DWORD dwError = 0;
	DWORD BytesRead = 0;
	unsigned char RXBuff;

	const unsigned int total_byte = 14;
	static unsigned char keydata[total_byte] = {0};
	static unsigned int  bytecount = 0;
	static unsigned char keyvalue[5] = {0};
	unsigned int iDataSum = 0;
	unsigned int sum      = 0;
	for (;;) 
	{ 
		// Gain ownership of the comm port critical section.
		// This process guarantees no other part of this program 
		// is using the port object. 
		if ( port->exitFlag )
		{
			break;
		}

		if ( WaitForSingleObject(port->m_hShutdownEvent,0) == WAIT_OBJECT_0 )
		{
			return;
		}
		EnterCriticalSection(&port->m_csCommunicationSync);

		// ClearCommError() will update the COMSTAT structure and
		// clear any other errors.

		bResult = ClearCommError(port->m_hComm, &dwError, &comstat);

		LeaveCriticalSection(&port->m_csCommunicationSync);

		// start forever loop.  I use this type of loop because I
		// do not know at runtime how many loops this will have to
		// run. My solution is to start a forever loop and to
		// break out of it when I have processed all of the
		// data available.  Be careful with this approach and
		// be sure your loop will exit.
		// My reasons for this are not as clear in this sample 
		// as it is in my production code, but I have found this 
		// solution to be the most efficient way to do this.

		if (comstat.cbInQue == 0)
		{
			// break out when all bytes have been read
			break;
		}

		EnterCriticalSection(&port->m_csCommunicationSync);

		if (bRead)
		{
			bResult = ReadFile(port->m_hComm,		// Handle to COMM port 
				&RXBuff,				// RX Buffer Pointer
				1,					// Read one byte
				&BytesRead,			// Stores number of bytes read
				&port->m_ov);		// pointer to the m_ov structure
			// deal with the error code 
			if (!bResult)  
			{ 
				switch (dwError = GetLastError()) 
				{ 
				case ERROR_IO_PENDING: 	
					{ 
						// asynchronous i/o is still in progress 
						// Proceed on to GetOverlappedResults();
						bRead = FALSE;
						break;
					}
				default:
					{
						// Another error has occurred.  Process this error.
						port->ProcessErrorMessage("ReadFile()");
						break;
					} 
				}
			}
			else
			{
				// ReadFile() returned complete. It is not necessary to call GetOverlappedResults()
				bRead = TRUE;
			}
		}  // close if (bRead)

		if (!bRead)
		{
			bRead = TRUE;
			bResult = GetOverlappedResult(port->m_hComm,	// Handle to COMM port 
				&port->m_ov,		// Overlapped structure
				&BytesRead,		// Stores number of bytes read
				TRUE); 			// Wait flag

			// deal with the error code 
			if (!bResult)  
			{
				port->ProcessErrorMessage("GetOverlappedResults() in ReadFile()");
			}	
		}  // close if (!bRead)

		LeaveCriticalSection(&port->m_csCommunicationSync);

		if ( RXBuff == '*' )
		{
			bytecount = 0;
		}
		else
		{
			++bytecount;
			if ( bytecount > 13 )
			{
				bytecount = 0;
 			}
		}	
		keydata[bytecount] = RXBuff;
		
		if ( receive_message &&
			 RXBuff == '#' && bytecount == total_byte-1 && keydata[0] == '*' )
		{
			process_data_frame( port, keydata );
		}
 	} // end forever loop

}

void CSerialPort::process_data_frame( CSerialPort* port, unsigned char* chBuff )
{
	static unsigned char keyvalue[5] = {0};

	keyvalue[0] = (TransHex(chBuff[1])<<4) + TransHex(chBuff[2]);
	keyvalue[1] = (TransHex(chBuff[3])<<4) + TransHex(chBuff[4]);
	keyvalue[2] = (TransHex(chBuff[5])<<4) + TransHex(chBuff[6]);
	keyvalue[3] = (TransHex(chBuff[7])<<4) + TransHex(chBuff[8]);
	keyvalue[4] = (TransHex(chBuff[9])<<4) + TransHex(chBuff[10]);

	unsigned int sum = keyvalue[0] + keyvalue[1] + keyvalue[2] + keyvalue[3] + keyvalue[4];

	unsigned int iDataSum = (TransHex(chBuff[11])<<4) + TransHex(chBuff[12]);

// 	if ( iDataSum == sum )
	{
// 		if ( MWM.is_main_world_model_available()    == false ||
// 			 SMC.is_main_motion_control_available() == false )
// 		{
// 			return;
// 		}
 	
// 		if ( jog_axis_control(keyvalue) )
// 		{
// 			return;
// 		}
		 
 		// notify parent that a byte was received
		::SendMessage((port->m_pOwner)->m_hWnd, WM_PENDANT_KEYPRESS, (WPARAM) keyvalue, (LPARAM) port->m_nPortNr);
	}
}

bool CSerialPort::jog_axis_control( unsigned char* keyvalue )
{
	if ( GETBIT( keyvalue[0],0) ) // stop = 0
	{
		if ( program_dialog->GetSafeHwnd() )
		{
			program_dialog->on_free_script(true);
			//SMC.setCmdInfo(CMD_HALT); 
		}
		cout << " stop com key! " << '\n';
		return true;
	}

  	unsigned char databyte = keyvalue[4] & 0X0C;
 	if ( databyte == 4 )
		enable_press = true;
	else
 		enable_press = false;
    
// 0 remote; 1 play; 2 teach
	key_switch = keyvalue[4] & 0X03;
 
	unsigned char axis = 0;
	unsigned char dir  = 0;
	unsigned char databyte_first  = keyvalue[0];
	unsigned char databyte_second = keyvalue[1];

 	unsigned char datashift = keyvalue[1] & 0X40;
 	static int step = 0;
	if ( datashift != 0 )
	{
		step = 1;
 	}
	else
	{
 		if ( step == 1 )
		{
			second_page = !second_page;
			step = 0;
		}
	}

	if ( GETBIT(databyte_first,2) ^ GETBIT(databyte_first,3) )
	{
		int bpos = 0;
		if ( second_page )
			bpos = 6;
		SETBIT(axis,bpos);
		if (GETBIT(databyte_first,2))
			SETBIT(dir,bpos);
	}
	if ( GETBIT(databyte_first,4) ^ GETBIT(databyte_first,5) )
	{
		int bpos = 1;
		if ( second_page )
			bpos = 7;

		SETBIT(axis,bpos);
		if (GETBIT(databyte_first,4))
			SETBIT(dir,bpos);
	}
	if ( GETBIT(databyte_first,6) ^ GETBIT(databyte_first,7) && !second_page )
	{
		SETBIT(axis,2);
		if (GETBIT(databyte_first,6))
			SETBIT(dir,2);
	}

	if ( GETBIT(databyte_second,0) ^ GETBIT(databyte_second,1)  && !second_page )
	{
		SETBIT(axis,3);
		if (GETBIT(databyte_second,0))
			SETBIT(dir,3);
	}
	if ( GETBIT(databyte_second,2) ^ GETBIT(databyte_second,3)  && !second_page )
	{
		SETBIT(axis,4);
		if (GETBIT(databyte_second,2))
			SETBIT(dir,4);
	}
	if ( GETBIT(databyte_second,4) ^ GETBIT(databyte_second,5)  && !second_page )
	{
		SETBIT(axis,5);
		if (GETBIT(databyte_second,4))
			SETBIT(dir,5);
	}

 	if ( key_switch == 2 ) // omTEACH
	{
		if ( axis != 0 )
		{
			if ( MWM.get_robot_state().servo_on == false )
			{
				SMC.setCmdInfo(CMD_ABORT); 
 				MSD.ShowInfo(_T(" 示教，设备未打开伺服使能！"),JTWARNING);
 				return true;
 			}

			if ( !enable_press )
			{
				SMC.setCmdInfo(CMD_ABORT); 
 				MSD.ShowInfo(_T(" 示教，请按下使能开关！"),JTWARNING);
 				return true;
 			}

			CmdInfo cmd;
			CartRef cartref = MWM.get_jog_cart().refSys;
			CartRef tool    = MWM.get_jog_tool().refSys;
			cmd.iCommandType  = CMD_JOG;
			cmd.iJogCoordType = MWM.get_robot_state().coordinate; // jtJOINT; 
			cmd.chJogAxisMark = axis;
			cmd.chJogDirMark  = dir;
			cmd.cartref = &cartref;
 			cmd.tool    = &tool.cpos;
 			// speed parameters
			cmd.dyn.custom_parameter = false;
			cmd.dyn.JointVel  = MWM.get_external_speed() > 10 ? 10 : MWM.get_external_speed();
			SMC.setCmdInfo( cmd );
			cout << "----------- jog axis control ------ " << '\n';
			return true;
		}
		else
		{
			// for check user teach program
 			if ( ProgramEditor::is_running_flag == true ) 
			{
				if ( !enable_press )
				{
					if ( program_dialog->GetSafeHwnd() )
					{
						program_dialog->on_free_script(true);
					}					
 					MSD.ShowInfo(_T("试运行，请按下使能开关！"),JTWARNING);
 					return true;
				}
			}
			else // the other is all stop
			{
				if ( MWM.get_robot_state().mc_info.iMotionStatus != MotionCoreInfo::msSTOP )
				{
					SMC.setCmdInfo(CMD_ABORT); 
				}
			}

			return false;
		}
	}
	else // remote or play
	{
		if ( axis != 0 )
		{
 			MSD.ShowInfo(_T("示教，请切换到示教模式！"),JTWARNING);
 		}
	}
	return false;
}
//
// Write a string to the port
//
void CSerialPort::WriteToPort(char* string)
{	
	if ( !m_flag_open )
 		return;

 	assert(m_hComm != 0);

	memset(m_szWriteBuffer, 0, sizeof(m_szWriteBuffer));
	strcpy(m_szWriteBuffer, string);

	// set event for write
	SetEvent(m_hWriteEvent);
}

//
// Write a string with fixed length to the port
//
void CSerialPort::WriteToPort(char* string,int length)
{
	if ( !m_flag_open )
		return;

	assert(m_hComm != 0);
	
	m_nDataLength = length;
	
	memset(m_szWriteBuffer, 0, sizeof(m_szWriteBuffer));

	for(int i=0;i<length;i++)
		m_szWriteBuffer[i]=string[i];
	
	// set event for write
	SetEvent(m_hWriteEvent);
}

//
// Return the device control block
//
DCB CSerialPort::GetDCB()
{
	return m_dcb;
}

//
// Return the communication event masks
//
DWORD CSerialPort::GetCommEvents()
{
	return m_dwCommEvents;
}

//
// Return the output buffer size
//
DWORD CSerialPort::GetWriteBufferSize()
{
	return m_nWriteBufferSize;
}

int CSerialPort::GetPortNO()
{
	return m_nPortNr;
}

void CSerialPort::set_command(bool b, DWORD sigMask )
{
	if ( !m_flag_open )
	{
		return;
	}
	 
	static char cmd = '0';
	if (b)
	{
		cmd |= sigMask;
	}
	else
	{
		cmd &= (~sigMask);
	}
	const int length = 8;
	char chbuff[length] = {0};
	chbuff[0] = '$';
	chbuff[1] = '0';
	chbuff[2] = '0';
	chbuff[3] = GetData((char)((cmd&0x00f0)>>4));
	chbuff[4] = GetData((char)(cmd&0x000f)); 
	chbuff[5] = '0';
	chbuff[6] = '1';
	chbuff[7] = '&';

	unsigned int iSend1;
	unsigned int iSend2;
	unsigned int iDataSum;
	iSend1 = (TransHex(chbuff[1])<<4) + TransHex(chbuff[2]);
	iSend2 = (TransHex(chbuff[3])<<4) + TransHex(chbuff[4]);

	iDataSum = iSend1 + iSend2;

	char s1 = GetData((char)(iDataSum&0x000f));
	char s2 = GetData((char)((iDataSum&0x00f0)>>4));
	chbuff[5] = s2;
	chbuff[6] = s1;

	EnterCriticalSection(&m_csCommunicationSync);
	WriteToPort(chbuff,length);
	LeaveCriticalSection(&m_csCommunicationSync);
}

const BYTE C[]="0123456789ABCDEF";
BYTE CSerialPort::GetData(BYTE num)
{
	BYTE D;
	D = C[num];

	return D;
}
 
//////////////////////////////////////////////////////////////////////////
unsigned char TransHex( char chHex )//将字符转换为16进制
{
	if ( chHex>='0' && chHex<='9' )
	{
		return chHex-'0';
	}
	else if ( chHex>='A' && chHex<='F' )
	{
		return chHex-'A'+10;
	}
	return -1;
}

 













