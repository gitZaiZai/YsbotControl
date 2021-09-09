#include "stdafx.h"
#include "VisionSerialPort.h"
#include "Time.h"
#include "WorldModel.h"

using namespace std;
 
int m_VisDataLength = 0; 
 
VisionSerialPort::VisionSerialPort()
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
VisionSerialPort::~VisionSerialPort()
{
 	StopMonitoring();

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
BOOL VisionSerialPort::InitPort(CWnd* pPortOwner,
	UINT  portnr,		// portnumber (1..4)
	UINT  baud,			// baudrate
	char  parity,		// parity 
	UINT  databits,		// databits 
	UINT  stopbits,		// stopbits 
	DWORD dwCommEvents,	// EV_RXCHAR, EV_CTS etc
	UINT  writebuffersize)	// size to the writebuffer
{
// 	assert(portnr > 0 && portnr < MaxSerialPortNum+1);
// 	assert(pPortOwner != NULL);
 
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

		m_flag_open = false;
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
UINT VisionSerialPort::CommThread(LPVOID pParam)
{
	// Cast the void pointer passed to the thread back to
	// a pointer of VisionSerialPort class
	VisionSerialPort *port = (VisionSerialPort*)pParam;

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
 
				if (CommEvent & EV_RXCHAR)
					// Receive character event from port.
					ReceiveChar(port, comstat);

				break;
			}  
		case 2: // write event
			{
				// Write character event from port
				WriteChar(port,m_VisDataLength);
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
BOOL VisionSerialPort::StartMonitoring() 
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
BOOL VisionSerialPort::RestartMonitoring()
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
BOOL VisionSerialPort::StopMonitoring()
{
 	if ( m_flag_open )
	{
		exitFlag = true;

		do
		{
			SetEvent(m_hShutdownEvent);
			Sleep(1);
		}while( m_bThreadAlive );

		TRACE("VisionSerialPort thread ended!\n");
	}
	return TRUE;	
}


//
// If there is a error, give the right message
//
void VisionSerialPort::ProcessErrorMessage(char* ErrorText)
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
void VisionSerialPort::WriteChar(VisionSerialPort* port,int length)
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
void VisionSerialPort::ReceiveChar(VisionSerialPort* port, COMSTAT comstat)
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
 	 
 	} // end forever loop

}
  
//
// Write a string to the port
//
void VisionSerialPort::WriteToPort(char* string)
{	
	if ( !m_flag_open )
		return;

// 	assert(m_hComm != 0);

	memset(m_szWriteBuffer, 0, sizeof(m_szWriteBuffer));
	strcpy(m_szWriteBuffer, string);

	// set event for write
	SetEvent(m_hWriteEvent);
}

//
// Write a string with fixed length to the port
//
void VisionSerialPort::WriteToPort(char* string,int length)
{
	if ( !m_flag_open )
		return;

// 	assert(m_hComm != 0);

	m_VisDataLength = length;

	memset(m_szWriteBuffer, 0, sizeof(m_szWriteBuffer));

	for(int i=0;i<length;i++)
		m_szWriteBuffer[i]=string[i];

	// set event for write
	SetEvent(m_hWriteEvent);
}

//
// Return the device control block
//
DCB VisionSerialPort::GetDCB()
{
	return m_dcb;
}

//
// Return the communication event masks
//
DWORD VisionSerialPort::GetCommEvents()
{
	return m_dwCommEvents;
}

//
// Return the output buffer size
//
DWORD VisionSerialPort::GetWriteBufferSize()
{
	return m_nWriteBufferSize;
}

int VisionSerialPort::GetPortNO()
{
	return m_nPortNr;
}

 
 












