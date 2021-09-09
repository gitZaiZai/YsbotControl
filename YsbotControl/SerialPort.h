/*
**	FILENAME			CSerialPort.h
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

#ifndef __SERIALPORT_H__
#define __SERIALPORT_H__

#define WM_COMM_BREAK_DETECTED		WM_USER+1	// A break was detected on input.
#define WM_COMM_CTS_DETECTED		WM_USER+2	// The CTS (clear-to-send) signal changed state. 
#define WM_COMM_DSR_DETECTED		WM_USER+3	// The DSR (data-set-ready) signal changed state. 
#define WM_COMM_ERR_DETECTED		WM_USER+4	// A line-status error occurred. Line-status errors are CE_FRAME, CE_OVERRUN, and CE_RXPARITY. 
#define WM_COMM_RING_DETECTED		WM_USER+5	// A ring indicator was detected. 
#define WM_COMM_RLSD_DETECTED		WM_USER+6	// The RLSD (receive-line-signal-detect) signal changed state. 
#define WM_COMM_RXCHAR				WM_USER+7	// A character was received and placed in the input buffer. 
#define WM_COMM_RXFLAG_DETECTED		WM_USER+8	// The event character was received and placed in the input buffer.  
#define WM_COMM_TXEMPTY_DETECTED	WM_USER+9	// The last character in the output buffer was sent.  
#define MaxSerialPortNum			200


class CSerialPort
{														 
public:
	int GetPortNO();
	// construction and destruction
	virtual		~CSerialPort();
	CSerialPort();
// 	static CSerialPort* getInstance();

	// port initialization											
	BOOL		InitPort( CWnd* pPortOwner, UINT portnr = 1, UINT baud = 9600, char parity = 'N', UINT databits = 8, UINT stopsbits = 1, DWORD dwCommEvents = EV_RXCHAR|EV_CTS, UINT nBufferSize = 512);

	// start/stop comm watching
	BOOL		StartMonitoring();
	BOOL		RestartMonitoring();
	BOOL		StopMonitoring();

	DWORD		GetWriteBufferSize();
	DWORD		GetCommEvents();
	DCB			GetDCB();
	void        set_command(bool b, DWORD sigMask );
	BYTE        GetData(BYTE num);

	void		WriteToPort(char* string);
	void		WriteToPort(char* string,int length);
	static bool jog_axis_control( unsigned char* );
	bool        get_flag_open() { return m_flag_open; } 

public:
	static bool second_page; 
	static bool enable_press; 
	static int  key_switch; 
	static bool receive_message;
	CRITICAL_SECTION	m_csCommunicationSync;

protected:
// 	static CSerialPort* singleton;

	// protected member functions
	void		ProcessErrorMessage(char* ErrorText);
	static UINT	CommThread(LPVOID pParam);
	static void	ReceiveChar(CSerialPort* port, COMSTAT comstat);
	static void	WriteChar(CSerialPort* port,int length);
//   	virtual int ProcessReceiveChar( unsigned char chRX ) = 0;
//     int ProcessReceiveChar( unsigned char chRX );
// 	int PraseReceiveBuff( unsigned char* chBuff, int iLength);
	static void process_data_frame( CSerialPort* port, unsigned char* chBuff );
 
	// thread
	CWinThread*			m_Thread;

	// synchronization objects
	BOOL				m_bThreadAlive;
	volatile bool       exitFlag;

	// handles
	HANDLE				m_hShutdownEvent;
	HANDLE				m_hComm;
	HANDLE				m_hWriteEvent;

	// Event array. 
	// One element is used for each event. There are two event handles for each port.
	// A Write event and a receive character event which is located in the overlapped structure (m_ov.hEvent).
	// There is a general shutdown when the port is closed. 
	HANDLE				m_hEventArray[3];

	// structures
	OVERLAPPED			m_ov;
	COMMTIMEOUTS		m_CommTimeouts;
	DCB					m_dcb;
 
	// misc
	UINT				m_nPortNr;
	char*				m_szWriteBuffer;
	DWORD				m_dwCommEvents;
	DWORD				m_nWriteBufferSize;

	bool                m_flag_open;
	CWnd*               m_pOwner;

private:
// 	class PortCleaner
// 	{
// 	public:
// 		PortCleaner() {}
// 		~PortCleaner()
// 		{
// 			if ( CSerialPort::getInstance() )
// 			{
// 				delete CSerialPort::getInstance();
// 			}
// 		}
// 	};
};

unsigned char TransHex( char chHex );

// #define TPD CSerialPort::getInstance()

#endif __SERIALPORT_H__


