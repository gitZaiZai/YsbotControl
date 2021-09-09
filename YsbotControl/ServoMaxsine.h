// Copyright 2016, 亚思信息科技有限责任公司
// All right reserved.
//
// Author: Liu Run Dan ( lrdcz@163.com )
//
// Yasi Robot Motion Control System - 亚思机器人运动控制系统

#ifndef _servoMaxsine_h
#define _servoMaxsine_h

#include "servotype.h"
#include "ConfigReader.h"
#include "SyncSerial.h"

class ServoMaxsine : public ServoType
{
public:
	ServoMaxsine(const ConfigReader&) throw (InvalidConfigurationException);
	~ServoMaxsine(void);

	bool init_port();

	int   read_pos( unsigned int iDeviceID, long* );

	int   clear_pos( unsigned int iDeviceID );
	int   clear_alarm( unsigned int iDeviceID );

	bool  close();

private:
	CString ChangeCharstr2Hexstr(CString Charstr);
	void    HexStringFilter(CString &str);
	char    CombineHexChar(char CharH,char CharL);
	CString DevideHexChar(char HexChar);
	BYTE    SumCRC( BYTE *pch, int iLen, int iFillFlag = 1 );
	BYTE    Number2HexChar(BYTE num);
	int     CheckCRC ( BYTE *pch, int iLen );

private:
	int WriteIntToPlc(int iDeviceID,  int iAddress, int value );
	int ReadIntFromPlc(int iDeviceID,  int iAddress, int *pvalue, int iCount = 1 );

	int ParseInt32(BYTE *pBuffer, int iSize, unsigned int *pVaule, int &iCount);
	int GetWriteFrame_Int16(BYTE *pbchFrame,
		int iDevicdAddress, 
		int iCommand, 
		unsigned short iRegAddress, 
		unsigned short iValue);
	int GetWriteFrame_Int32(BYTE *pbchFrame,
		int iDevicdAddress, 
		int iCommand, 
		unsigned short iRegAddress, 
		unsigned int iValue);
	int GetReadFrame_Int32S(BYTE *pbchFrame, 
		int iDevicdAddress, 
		int iCommand, 
		unsigned short iRegStart, 
		int iRegCount);
	unsigned short CRC16( unsigned char *puchMsg , unsigned int usDataLen );

	void SwapDataHighLow( int& val ); 

private:
	SyncSerial m_SerialMaster;
	unsigned int portnr;
	unsigned int baud;
	unsigned int parity;
	unsigned int databits;
	unsigned int stopbits;
	int iOverTime;
	bool open_port;

};

#endif