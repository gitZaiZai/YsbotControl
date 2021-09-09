#ifndef _VISION_SERIALPORT_H_
#define _VISION_SERIALPORT_H_
  
#define MaxSerialPortNum			200

class VisionSerialPort
{														 
public:
	int GetPortNO();
	// construction and destruction
	virtual		~VisionSerialPort();
	VisionSerialPort();
 
 	BOOL		InitPort( CWnd* pPortOwner, UINT portnr = 1, UINT baud = 9600, char parity = 'N', UINT databits = 8, UINT stopsbits = 1, DWORD dwCommEvents = EV_RXCHAR|EV_CTS, UINT nBufferSize = 512);

	// start/stop comm watching
	BOOL		StartMonitoring();
	BOOL		RestartMonitoring();
	BOOL		StopMonitoring();

	DWORD		GetWriteBufferSize();
	DWORD		GetCommEvents();
	DCB			GetDCB();
 
	void		WriteToPort(char* string);
	void		WriteToPort(char* string,int length);
 
protected:
	// 	static VisionSerialPort* singleton;

	// protected member functions
	void		ProcessErrorMessage(char* ErrorText);
	static UINT	CommThread(LPVOID pParam);
	static void	ReceiveChar(VisionSerialPort* port, COMSTAT comstat);
	static void	WriteChar(VisionSerialPort* port,int length);
  
	// thread
	CWinThread*			m_Thread;

	// synchronization objects
	CRITICAL_SECTION	m_csCommunicationSync;
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
   
};

 
 
#endif  


