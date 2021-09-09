// SyncSerial.h: interface for the SyncSerial class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SYNCSERIAL_H__2E99F093_A58B_4F3B_A0C9_DB9550BF51B4__INCLUDED_)
#define AFX_SYNCSERIAL_H__2E99F093_A58B_4F3B_A0C9_DB9550BF51B4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SyncSerial.h"

#define SERIAL_NO_ERR                0
#define SERIAL_ERR_PORT_NOT_OPENED  -1
#define SERIAL_OVERTIME             -2

enum CommunicationsState
{
	Sy_Successful = 0,
	Sy_ErrorRequest,
	Sy_ErrorResponse,
	Sy_NoAccomplish
};

 
class  SyncSerial  
{
public:
	SyncSerial();
	virtual ~SyncSerial();
public:
	bool InitPort(UINT portnr = 1, UINT baud = 19200, UINT parity = NOPARITY, 
		UINT databits = 8, UINT stopbits = ONESTOPBIT, int iOverTime=2000);
	int WriteToPort(BYTE *pByte, int iLen);
	int ReadFromPort(BYTE *pByte, int iLen);
	int ReadFromPort( BYTE *pByte, int *piLen );

	int recv_from_serial_port( unsigned char *lpInBuffer, int dwBytesToRead );
	int send_to_serial_port( unsigned char *lpSndBuffer, int dwBytesToWrite );

	HANDLE GetHandle();
	bool CloseComPort();
    bool get_flag_open() { return m_flag_open; } 
protected:
	HANDLE m_hComm;  //´®¿Ú¾ä±ú
private:
	COMMTIMEOUTS		m_TimeOuts;
	COMMTIMEOUTS		m_CommTimeouts;
	DCB					m_dcb;
	
	bool                is_blocking;
private:
	bool m_flag_open;
};
#endif // !defined(AFX_SYNCSERIAL_H__2E99F093_A58B_4F3B_A0C9_DB9550BF51B4__INCLUDED_)
