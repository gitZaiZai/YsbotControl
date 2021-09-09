// SyncSerial.cpp: implementation of the SyncSerial class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "stdio.h"
#include "SyncSerial.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SyncSerial::SyncSerial()
{
	m_hComm=NULL;
	is_blocking = true;
}

SyncSerial::~SyncSerial()
{
	try
	{
		CloseHandle(m_hComm);
		m_hComm=NULL;
	}
	catch (...)
	{

	}	
}

HANDLE SyncSerial::GetHandle()
{
	return m_hComm;
}

bool SyncSerial::InitPort(
						   UINT  portnr,		// portnumber (1..4)
						   UINT  baud,			// baudrate
						   UINT  parity,		// parity 
						   UINT  databits,		// databits 
						   UINT  stopbits,		// stopbits 
						   int iOverTime
							)
{
	BOOL bResult = FALSE;
	char *szPort = new char[50];
	char *szBaud = new char[50];
	
	// if the port is already opened: close it
	if( m_hComm != NULL )
	{
		CloseHandle(m_hComm);
		m_hComm = NULL;
	}
	

	// prepare port strings

	char TC;

	if ( parity == NOPARITY )
	{
		TC = 'N';
	}
	else if ( parity == ODDPARITY )
	{
		TC = 'O';
	}
	else if ( parity == EVENPARITY )
	{
		TC = 'e';
	}
 
	sprintf(szPort, ("\\\\.\\COM%d"), portnr);
	if ( stopbits == ONESTOPBIT )
	{
		sprintf (szBaud, ("baud=%d parity=%c data=%d stop=1"), baud, TC, databits);
	}
	else if ( stopbits == ONE5STOPBITS )
	{
		sprintf (szBaud, ("baud=%d parity=%c data=%d stop=1.5"), baud, TC, databits);
	}
	else if ( stopbits == TWOSTOPBITS )
	{
		sprintf (szBaud, ("baud=%d parity=%c data=%d stop=2"), baud, TC, databits);
	}	
	
	// get a handle to the port
	if( is_blocking == false )
	{
		m_hComm = CreateFileA(szPort,						// communication port string (COMX)
			GENERIC_READ | GENERIC_WRITE,	// read/write types
			0,								// comm devices must be opened with exclusive access
			NULL,							// no security attributes
			OPEN_EXISTING,					// comm devices must use OPEN_EXISTING
			FILE_FLAG_OVERLAPPED,			// Async I/O
			0);							// template must be 0 for comm devices

	}
	else
	{
		m_hComm = CreateFileA(szPort,						// communication port string (COMX)
			GENERIC_READ | GENERIC_WRITE,	// read/write types
			0,								// comm devices must be opened with exclusive access
			NULL,							// no security attributes
			OPEN_EXISTING,					// comm devices must use OPEN_EXISTING
			0,			                // Sync I/O
			0);							    // template must be 0 for comm devices

	}
	
	if (m_hComm == INVALID_HANDLE_VALUE)
	{
		// port not found
		delete [] szPort;
		delete [] szBaud;
		
		m_flag_open = false;
		return false;
	}
	
	// set the timeout values
	m_CommTimeouts.ReadIntervalTimeout = 100;
	m_CommTimeouts.ReadTotalTimeoutMultiplier = 10;
	m_CommTimeouts.ReadTotalTimeoutConstant = 1000;
	m_CommTimeouts.WriteTotalTimeoutMultiplier = 1000;
	m_CommTimeouts.WriteTotalTimeoutConstant = 1000;
	
	// configure
	if (SetCommTimeouts(m_hComm, &m_CommTimeouts))
	{						   
		if (GetCommState(m_hComm, &m_dcb))
		{
			// 			m_dcb.EvtChar = 'q';
			// 			m_dcb.fRtsControl = RTS_CONTROL_ENABLE;		// set RTS bit high!
			if (BuildCommDCBA(LPCSTR(szBaud), &m_dcb))
			{
				if (SetCommState(m_hComm, &m_dcb))
					; // normal operation... continue
				else
					;
			}
			else
				;
		}
		else
			;
	}
	else
		;
	
	delete [] szPort;
	delete [] szBaud;
	
	// flush the port
	PurgeComm(m_hComm, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);
	
	// release critical section
	
	
	m_flag_open = true; //串口标记为开状态
	return true;
}

int SyncSerial::WriteToPort(BYTE *pByte, int iLen)
{
	if( m_flag_open == false || !m_hComm )
		return SERIAL_ERR_PORT_NOT_OPENED;

	if( is_blocking )
	{
		int ret = send_to_serial_port(pByte,iLen);
		Sleep(5);
		return ret;
	}
	
	BOOL    bWrite       = TRUE;
	BOOL    bResult      = TRUE;
	DWORD   BytesSent    = 0;
	DWORD   dwBytesWrite = iLen;
	DWORD   dwErrorFlags;
// 	COMSTAT ComStat;

	OVERLAPPED m_osWrite;
	memset(&m_osWrite,0,sizeof(OVERLAPPED));
 	m_osWrite.Offset     = 0;
	m_osWrite.OffsetHigh = 0;
	m_osWrite.hEvent     = CreateEvent(NULL,TRUE,FALSE,NULL);

	if( bWrite )
	{
		// Clear buffer
		if( !ClearCommError(m_hComm, &dwErrorFlags, NULL) )
        {
            //清除输出缓冲
            PurgeComm(m_hComm, PURGE_TXABORT | PURGE_TXCLEAR);
			CloseHandle(m_osWrite.hEvent);
            return -3;
        }

// 		ClearCommError( m_hComm, &dwErrorFlags, &ComStat );
// 		PurgeComm( m_hComm, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);
        bResult = WriteFile( m_hComm,pByte,	dwBytesWrite, &BytesSent, &m_osWrite);
		// deal with any error codes
		if( !bResult )  
		{
			DWORD dwError = GetLastError();
			switch( dwError )
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
						CloseHandle(m_osWrite.hEvent);
						return SERIAL_OVERTIME;
					}
			}
		} 
		else
		{
			;
		} 
	}
	if( !bWrite )
	{
		bWrite = TRUE;
		
		bResult = GetOverlappedResult(m_hComm,	&m_osWrite,	&BytesSent,	TRUE ); 			// Wait flag
		// deal with the error code 
		if( !bResult )  
		{
			TRACE("GetOverlappedResults() in WriteFile()");
			CloseHandle(m_osWrite.hEvent);
			return SERIAL_OVERTIME;
		}	
	} // end if (!bWrite)
	
	// Verify that the data size send equals what we tried to send
	if( BytesSent != dwBytesWrite )   
	{
		TRACE("WARNING: WriteFile() error.. Bytes Sent: %d; Message Length: %d\n", BytesSent,dwBytesWrite );
	}
   
	CloseHandle(m_osWrite.hEvent);
	return 0;
}

int SyncSerial::ReadFromPort(BYTE *pByte, int iLen)
{
	if( m_flag_open == false || !m_hComm )
		return SERIAL_ERR_PORT_NOT_OPENED;

	if( is_blocking )
	{
 		return recv_from_serial_port(pByte,iLen);
	}

	OVERLAPPED m_osRead;
	memset( &m_osRead, 0, sizeof(OVERLAPPED) );
	m_osRead.hEvent = CreateEvent(NULL,TRUE,FALSE,NULL);
	
	COMSTAT ComStat;
	DWORD dwErrorFlags = 0;
	
	DWORD dwBytesRead  = 255;//读取的字节数
	BOOL  bReadStat = FALSE;
	BOOL  bResult   = TRUE;
	BOOL  bRead     = TRUE; 
	DWORD dwError   = 0;
// 	ClearCommError(m_hComm,&dwErrorFlags,&ComStat);
	if( !ClearCommError(m_hComm, &dwErrorFlags, &ComStat) )
	{
		//清除输入缓冲
		PurgeComm(m_hComm, PURGE_RXABORT | PURGE_RXCLEAR);
		CloseHandle( m_osRead.hEvent );
 		return -3;
	} 
	
    DWORD Event = 0;
 	if( bRead )
	{
		for(;;) 
		{ 
 			bResult = WaitCommEvent( m_hComm, &Event, &m_osRead );
			
			if( !bResult )  
			{ 
				dwError = GetLastError();
				if( dwError == ERROR_IO_PENDING || dwError == 87 )
				{
					break;
				}
				else
				{
					TRACE("Error in WaitCommEvent()! \n");
					CloseHandle( m_osRead.hEvent );
					return SERIAL_OVERTIME;
				}
			}
			else
			{
  				bResult = ClearCommError(m_hComm, &dwError, &ComStat);
				
				if( ComStat.cbInQue == 0 )
					continue;
			}
		}

		dwBytesRead = min( dwBytesRead, (DWORD)ComStat.cbInQue );
		bResult = ReadFile(m_hComm,pByte,iLen,&dwBytesRead,&m_osRead);
		// deal with the error code 
		if( !bResult )  
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
					CloseHandle(m_osRead.hEvent);
					return SERIAL_OVERTIME;
				} 
			}
		}
		else
		{
			// ReadFile() returned complete. It is not necessary to call GetOverlappedResults()
			bRead = TRUE;
		}
	}  // close if (bRead)
	
	if( !bRead )
	{
		bRead = TRUE;
		bResult = GetOverlappedResult( m_hComm,&m_osRead,&dwBytesRead,TRUE );
		
		// deal with the error code 
		if( !bResult )  
		{
			CloseHandle(m_osRead.hEvent);
			return SERIAL_OVERTIME;
		}	
	}  // close if (!bRead)
 
	
	PurgeComm(m_hComm, PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);
	CloseHandle(m_osRead.hEvent);

	return dwBytesRead;
	
}

int SyncSerial::ReadFromPort( BYTE *pByte, int *piLen )
{
	*piLen = ReadFromPort(pByte, *piLen );
	return 0;
}

bool SyncSerial::CloseComPort()
{
	BOOL b = CloseHandle(m_hComm);
	m_flag_open = FALSE;
	m_hComm  = NULL;
    return ( b ? true : false);
}

// lpInBuffer为接收数据的缓冲区指针，dwBytesToRead为准备读取的数据长度（字节数）
int SyncSerial::recv_from_serial_port( unsigned char *lpInBuffer, int dwBytesToRead )
{
	if( m_flag_open == false || !m_hComm )
		return SERIAL_ERR_PORT_NOT_OPENED;

	BOOL  bResult   = FALSE;
	
	COMSTAT ComStat;
	DWORD dwBytesRead = 0;
	DWORD dwErrorFlags;
	// 读取串行设备的当前状态
	ClearCommError( m_hComm, &dwErrorFlags, &ComStat );
	// 应该读取的数据长度
	dwBytesRead = min( (DWORD)dwBytesToRead, ComStat.cbInQue );
	if( dwBytesRead > 0 )	//读取数据
	{
		bResult = ReadFile( m_hComm, lpInBuffer, dwBytesRead, &dwBytesRead, NULL );
		if( !bResult )
		{
		    dwBytesRead = 0;
			return -3;
		}
	}
	else
		return -2;
	PurgeComm(m_hComm, PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);

    return dwBytesRead;
}

// lpSndBuffer为发送数据缓冲区指针，dwBytesToWrite为将要发送的字节长度
int SyncSerial::send_to_serial_port( unsigned char *lpSndBuffer, int dwBytesToWrite )
{
	if( m_flag_open == false || !m_hComm )
		return SERIAL_ERR_PORT_NOT_OPENED;

	COMSTAT ComStat;
	DWORD dwErrorFlags;
	ClearCommError( m_hComm, &dwErrorFlags, &ComStat );
	PurgeComm( m_hComm, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);

	BOOL bWriteState;
	// 实际发送的字节数
	DWORD dwBytesWritten;
 	bWriteState = WriteFile( m_hComm, lpSndBuffer, dwBytesToWrite, &dwBytesWritten, NULL );
	if( !bWriteState || (DWORD)dwBytesToWrite != dwBytesWritten )
		return -2;
	else
	   return 0;
}
