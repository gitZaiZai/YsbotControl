// Copyright 2016, 亚思信息科技有限责任公司
// All right reserved.
//
// Author: Liu Run Dan ( lrdcz@163.com )
//
// Yasi Robot Motion Control System - 亚思机器人运动控制系统

#ifndef kewei_plc_h
#define kewei_plc_h

#include "FieldBusDevicetype.h"
#include "ConfigReader.h"
#include "SyncSerial.h"
#include <vector>

class KeWeiPLC : public FieldBusDeviceType
{
public:
	KeWeiPLC(const ConfigReader&) throw (InvalidConfigurationException,std::bad_alloc);
	~KeWeiPLC(void);

	int  connect ( );
	int  disconnect ( ) { return m_SerialMaster.CloseComPort(); }

	int  din( const int, bool* );
	int  dout( const int, const bool );

	int  read_data_reg (  const int, unsigned short* );
	int  write_data_reg ( const int, const unsigned short  );

	int  read_multiple_word (  const int, const unsigned int, unsigned short* );
	int  write_multiple_word ( const int, const unsigned int, const unsigned short*  );

	const  IoModuleParameters& get_fieldbus_io_parameters() const throw () { return fp; }

private:

	bool Init(UINT portnr = 1, UINT baud = 19200, UINT parity = EVENPARITY, UINT databits = 8, UINT stopbits = ONESTOPBIT, int iOverTime=2000);
	bool CloseCom ()      { return m_SerialMaster.CloseComPort();  }
	bool get_flag_open () { return m_SerialMaster.get_flag_open(); }

	int WriteToPlc ( const int, const unsigned int, const unsigned short* );
 	int ReadFromPlc ( const int, const unsigned int, unsigned short*);

	int FX_H_atoi (const   char   *s);
	int CalculateSum (BYTE *pch, int iLen, bool bFillFlag = true );
	bool CheckSum (BYTE *pch, int iLen);
	BYTE GetData (BYTE num);

	SyncSerial m_SerialMaster;

	unsigned int portnr;
	unsigned int baud;
	unsigned int parity;
	unsigned int databits;
	unsigned int stopbits;
	int iOverTime;
	bool open_port;
 
	unsigned char SlaveID;
 
	int total_y_size;
	int total_x_size;

	IoModuleParameters fp;
	bool use_plc;
};

#endif