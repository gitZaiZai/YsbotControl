// Copyright 2016, 亚思信息科技有限责任公司
// All right reserved.
//
// Author: Liu Run Dan ( lrdcz@163.com )
//
// Yasi Robot Motion Control System - 亚思机器人运动控制系统

#ifndef nidec_sankyo_h
#define nidec_sankyo_h
#include "servotype.h"
#include "ConfigReader.h"
#include "SyncSerial.h"

class NidecSankyo  : public ServoType
{
public:
	NidecSankyo(const ConfigReader& reader) throw (InvalidConfigurationException);
	~NidecSankyo(void);

	bool init_port();

	int   read_pos( unsigned int iDeviceID, long* );

	int   clear_pos( unsigned int iDeviceID );
	int   clear_alarm( unsigned int iDeviceID );

	bool  close();

	BOOL Init( UINT portnr   = 1, UINT baud     = 57600,      UINT parity   = NOPARITY, 
		UINT databits = 8, UINT stopbits = ONESTOPBIT, int iOverTime = 2000 );

	int READ_EA05_DATA_EX( unsigned int iDeviceID, short &nEncoderLaps,DWORD &wEncoderPulses );
	int CLEAR_EA05_DATA(unsigned int iDeviceID);
	int READ_EA05_DATA(unsigned int iDeviceID);
	int GET_STATE_VALUE_4(unsigned int iDeviceID,unsigned int iAddress,long &wEncoderData);
	int GET_STATE_VALUE_2(unsigned int iDeviceID,unsigned int iAddress);

	bool CloseCom() { return m_SerialMaster.CloseComPort(); }

private:
	BYTE  Number2HexChar(BYTE num);

private:
	SyncSerial m_SerialMaster;
	unsigned int portnr;
	unsigned int baud;
	unsigned int parity;
	unsigned int databits;
	unsigned int stopbits;
	int iOverTime;
	bool open_port;

	ConfigReader& configuration_list;

};

#endif