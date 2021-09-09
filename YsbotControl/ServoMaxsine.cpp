#include "StdAfx.h"
#include "ServoMaxsine.h"
#include "ServoFactory.h"
#include "YsibotsException.h"
#include "Journal.h"

namespace
{
	class Builder : public ServoBuilder 
	{
		static Builder the_builder;
	public:
		Builder () {
			ServoFactory::get_servo_factory ()->sign_up (std::string("Maxsine"), this);
		}
		ServoType* get_servo (const std::string&, const ConfigReader& reader, ServoType*) throw (std::bad_alloc) {
			return new ServoMaxsine (reader);
		}
	};
	Builder the_builder;
}

ServoMaxsine::ServoMaxsine (const ConfigReader& reader) throw (InvalidConfigurationException):portnr(1),
	baud(38400),parity(NOPARITY),databits(8),stopbits(ONESTOPBIT),iOverTime(2000),open_port(false)
{
	if ( reader.get("Maxsine::port", portnr) <= 0 ) 
	{
		JERROR("no config line \"ServoDriver_port\" found");
		throw InvalidConfigurationException ("ServoMaxsine_port");
	}

	if ( reader.get("Maxsine::baud", baud) <= 0 ) 
	{
		JERROR("no config line \"ServoDriver_baud\" found");
		throw InvalidConfigurationException ("ServoMaxsine_baud");
	}

	if ( reader.get("Maxsine::parity", parity) <= 0 ) 
	{
		JERROR("no config line \"ServoDriver_databit\" found");
		throw InvalidConfigurationException ("ServoMaxsine_databit");
	}

	if ( reader.get("Maxsine::databit", databits) <= 0 ) 
	{
		JERROR("no config line \"ServoDriver_parity\" found");
		throw InvalidConfigurationException ("ServoMaxsine_parity");
	}

	if ( reader.get("Maxsine::stopbit", stopbits) <= 0 ) 
	{
		JERROR("no config line \"ServoDriver_stopbit\" found");
		throw InvalidConfigurationException ("ServoMaxsine_stopbit");
	}
	TRACE(_T("Constuct ServoMaxsine!\n")); 

}

ServoMaxsine::~ServoMaxsine()
{
	;
}

int   ServoMaxsine::read_pos( unsigned int iDeviceID, long* pData )
{
	if ( !open_port )
	{
		TRACE(_T("servo port no open!\n"));
 		return -1;
	}
	 
	int pulse = 0;
// 	int ret = ReadIntFromPlc( iDeviceID, 0X1014, &pulse, 1 ); // 1 连读两个16bit参数，共32位

	short nEncoderLaps   = (short)(pulse & 0XFFFF);
	DWORD wEncoderPulses = (pulse >> 16) & 0XFFFF;

	*pData = nEncoderLaps * 131072 + wEncoderPulses;
	// 	CString strmsg;
	// 	strmsg.Format(" Lap %0X Pls %0X \n", nEncoderLaps,wEncoderPulses);
	// 	TRACE(strmsg);
// 	TRACE(_T("read pos NO.%d ServoMaxsine!\n"),iDeviceID); 

	return 0;
}

int   ServoMaxsine::clear_pos( unsigned int iDeviceID )
{
	if ( !open_port )
	{
		TRACE(_T("servo port no open!\n"));
		return -1;
	}
	TRACE(_T("clear_pos NO.%d ServoMaxsine!\n"),iDeviceID); 

	return 0;
}

int   ServoMaxsine::clear_alarm( unsigned int iDeviceID )
{
	if ( !open_port )
	{
		TRACE(_T("servo port no open!\n"));
		return -1;
	}
	TRACE(_T("clear_alarm NO.%d ServoMaxsine!\n"),iDeviceID); 

	return 0;
}

bool ServoMaxsine::init_port()
{
	TRACE(_T("init_port ServoMaxsine!\n")); 

 	bool bReturn = m_SerialMaster.InitPort(portnr, baud, parity, databits, stopbits, iOverTime);
	open_port = bReturn;

	return bReturn;
}

bool ServoMaxsine::close()
{
	TRACE(_T("CloseComPort ServoMaxsine!\n")); 
	open_port = false;

	return m_SerialMaster.CloseComPort(); 
}

unsigned short ServoMaxsine::CRC16( unsigned char *puchMsg , unsigned int usDataLen )
{
	const static unsigned char auchCRCHi[]=
	{
		0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,0x01,0xC0,
		0x80,0x41,0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,
		0x00,0xC1,0x81,0x40,0x00,0xC1,0x81,0x40,0x01,0xC0,
		0x80,0x41,0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,
		0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,0x00,0xC1,
		0x81,0x40,0x01,0xC0,0x80,0x41,0x01,0xC0,0x80,0x41,
		0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,0x00,0xC1,

		0x81,0x40,0x00,0xC1,0x81,0x40,0x01,0xc0,0x80,0x41,

		0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,0x01,0xC0,
		0x80,0x41,0x00,0xC1,0x81,0x40,0x00,0xC1,0x81,0x40,
		0x01,0xC0,0x80,0x41,0x01,0xC0,0x80,0x41,0x00,0xC1,
		0x81,0x40,0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,
		0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,0x01,0xC0,

		0x80,0x41,0x00,0xC1,0x81,0x40,0x00,0xC1,0x81,0x40,
		0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,0x01,0xC0,
		0x80,0x41,0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,
		0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,0x01,0xC0,
		0x80,0x41,0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,
		0x00,0xC1,0x81,0x40,0x00,0xC1,0x81,0x40,0x01,0xC0,

		0x80,0x41,0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,
		0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,0x01,0xC0,
		0x80,0x41,0x00,0xC1,0x81,0x40,0x00,0xC1,0x81,0x40,
		0x01,0xC0,0x80,0x41,0x01,0xC0,0x80,0x41,0x00,0xC1,
		0x81,0x40,0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,
		0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,0x01,0xC0,
		0x80,0x41,0x00,0xC1,0x81,0x40

	};

	const static unsigned char auchCRCLo[] =
	{
		0x00,0xC0,0xC1,0x01,0xC3,0x03,0x02,0xC2,0xC6,0x06,
		0x07,0xC7,0x05,0xC5,0xC4,0x04,0xCC,0x0C,0x0D,0xCD,
		0x0F,0xCF,0xCE,0x0E,0x0A,0xCA,0xCB,0x0B,0xC9,0x09,
		0x08,0xC8,0xD8,0x18,0x19,0xD9,0x1B,0xDB,0xDA,0x1A,
		0x1E,0xDE,0xDF,0x1F,0xDD,0x1D,0x1C,0xDC,0x14,0xD4,

		0xD5,0x15,0xD7,0x17,0x16,0xD6,0xD2,0x12,0x13,0xD3,
		0x11,0xD1,0xD0,0x10,0xF0,0x30,0x31,0xF1,0x33,0xF3,
		0xF2,0x32,0x36,0xF6,0xF7,0x37,0xF5,0x35,0x34,0xF4,
		0x3C,0xFC,0xFD,0x3D,0xFF,0x3F,0x3E,0xFE,0xFA,0x3A,

		0x3B,0xFB,0x39,0xF9,0xF8,0x38,0x28,0xE8,0xE9,0x29,
		0xEB,0x2B,0x2A,0xEA,0xEE,0x2E,0x2F,0xEF,0x2D,0xED,
		0xEC,0x2C,0xE4,0x24,0x25,0xE5,0x27,0xE7,0xE6,0x26,
		0x22,0xE2,0xE3,0x23,0xE1,0x21,0x20,0xE0,0xA0,0x60,

		0x61,0xA1,0x63,0xA3,0xA2,0x62,0x66,0xA6,0xA7,0x67,
		0xA5,0x65,0x64,0xA4,0x6C,0xAC,0xAD,0x6D,0xAF,0x6F,
		0x6E,0xAE,0xAA,0x6A,0x6B,0xAB,0x69,0xA9,0xA8,0x68,
		0x78,0xB8,0xB9,0x79,0xBb,0x7B,0x7A,0xBA,0xBE,0x7E,

		0x7F,0xBF,0x7D,0xBD,0xBC,0x7C,0xB4,0x74,0x75,0xB5,
		0x77,0xB7,0xB6,0x76,0x72,0xB2,0xB3,0x73,0xB1,0x71,
		0x70,0xB0,0x50,0x90,0x91,0x51,0x93,0x53,0x52,0x92,
		0x96,0x56,0x57,0x97,0x55,0x95,0x94,0x54,0x9C,0x5C,

		0x5D,0x9D,0x5F,0x9F,0x9E,0x5E,0x5A,0x9A,0x9B,0x5B,
		0x99,0x59,0x58,0x98,0x88,0x48,0x49,0x89,0x4B,0x8B,
		0x8A,0x4A,0x4E,0x8E,0x8F,0x4F,0x8D,0x4D,0x4C,0x8C,
		0x44,0x84,0x85,0x45,0x87,0x47,0x46,0x86,0x82,0x42,
		0x43,0x83,0x41,0x81,0x80,0x40
	};	
	unsigned char uchCRCHi = 0xFF ;     // 高字节初始值 /
	unsigned char uchCRCLo = 0xFF ;     
	unsigned uIndex;

	while ( usDataLen-- )
	{
		uIndex = uchCRCHi ^ *puchMsg++;
		uchCRCHi = uchCRCLo ^ auchCRCHi[uIndex];
		uchCRCLo = auchCRCLo[uIndex];
	}

	return ( uchCRCHi << 8 | uchCRCLo );
}

int ServoMaxsine::GetWriteFrame_Int16(BYTE *pbchFrame,
	int iDevicdAddress, 
	int iCommand, 
	unsigned short iRegAddress, 
	unsigned short iValue)
{
	pbchFrame[0] = iDevicdAddress;
	pbchFrame[1] = iCommand;
	pbchFrame[2] = (iRegAddress>>8);
	pbchFrame[3] = (iRegAddress<<8)>>8;
	pbchFrame[4] = (iValue>>8);
	pbchFrame[5] = (iValue<<8)>>8;

	unsigned int icrc = CRC16(pbchFrame, 6);
	pbchFrame[6] = ((BYTE)(icrc>>8));
	pbchFrame[7] = (BYTE)(icrc);
	return 8;
}

int ServoMaxsine::GetWriteFrame_Int32(BYTE *pbchFrame,
	int iDevicdAddress, 
	int iCommand, 
	unsigned short iRegAddress, 
	unsigned int iValue)
{
	pbchFrame[0] = iDevicdAddress;
	pbchFrame[1] = iCommand;
	pbchFrame[2] = (iRegAddress>>8);
	pbchFrame[3] = (iRegAddress<<8)>>8;
	pbchFrame[4] = 0;
	pbchFrame[5] = 2;
	pbchFrame[6] = 4;
	unsigned char *pch = (unsigned char *)&iValue;
	pbchFrame[7] = pch[3];
	pbchFrame[8] = pch[2];
	pbchFrame[9] = pch[1];
	pbchFrame[10] = pch[0];

	unsigned int icrc = CRC16(pbchFrame, 11);
	pbchFrame[11] = ((BYTE)(icrc>>8));
	pbchFrame[12] = (BYTE)(icrc);
	return 13;
}

int ServoMaxsine::GetReadFrame_Int32S(BYTE *pbchFrame, 
	int iDevicdAddress, 
	int iCommand, 
	unsigned short iRegStart, 
	int iRegCount)
{
	pbchFrame[0] = iDevicdAddress;
	pbchFrame[1] = iCommand;
	pbchFrame[2] = (iRegStart>>8);
	pbchFrame[3] = (iRegStart<<8)>>8;
	pbchFrame[4] = (iRegCount>>8);
	pbchFrame[5] = (iRegCount<<8)>>8;

	unsigned int icrc = CRC16(pbchFrame, 6);
	pbchFrame[6] = ((BYTE)(icrc>>8));
	pbchFrame[7] = (BYTE)(icrc);
	return 8;
}

int ServoMaxsine::ParseInt32(BYTE *pBuffer, int iSize, unsigned int *pVaule, int &iCount)
{
	int _iCount = (iSize-5)/4;
	for ( int i=0; i<_iCount; i++ )
	{
		unsigned char *up;
		up = (unsigned char *)(&pVaule[i]);
		up[0] = pBuffer[6+4*i];
		up[1] = pBuffer[5+4*i];
		up[2] = pBuffer[4+4*i];
		up[3] = pBuffer[3+4*i];
	}
	iCount = _iCount;
	return 0;
}

void ServoMaxsine::SwapDataHighLow( int& val )
{
	int ih = 0;
	int il = 0;
	ih = val;
	il = val;
	ih = ih >> 16;
	il = il << 16;
	val = ih ^ il;
}

int ServoMaxsine::WriteIntToPlc(int iDeviceID,  int iAddress, int value )
{
	int _iDevice = iDeviceID;
	int _iFunciton = 6;
	int _iRegAddress = iAddress;
	unsigned short _iValue = value;

	BYTE bchFrame[255];
	int iLen = GetWriteFrame_Int16(bchFrame, _iDevice, _iFunciton, _iRegAddress, _iValue);
	m_SerialMaster.WriteToPort(bchFrame, iLen);

	BYTE bchReceiveFrame[255];
	int iReceivLength = m_SerialMaster.ReadFromPort(bchReceiveFrame,255);
	const int iBytesMustbeRead = 8;
	if( iReceivLength == 8 )
	{
		unsigned int icrc = CRC16( bchReceiveFrame, 6);
		unsigned char crc_low = ((BYTE)(icrc>>8));
		unsigned char crc_hig = (BYTE)(icrc);

		if( crc_low != bchReceiveFrame[6] || crc_hig != bchReceiveFrame[7] )
		{
			return 2;
		}
	}
	else  
	{
		return 1;
	}

	return 0;
}

int ServoMaxsine::ReadIntFromPlc(int iDeviceID,  int iAddress, int *pvalue, int iCount )
{
	// iCount 为字数 4 byte,16 bit and 16 bit int iCount = 1;
	int _iDevice   = iDeviceID;
	int _iFunciton = 3;
	int _iRegCount = iCount*2; 
	int _iRegStart = iAddress;

	BYTE bchSendFrame[255];
	int iSendLength = GetReadFrame_Int32S(bchSendFrame, _iDevice, _iFunciton, _iRegStart, _iRegCount);	
	BYTE bchReceiveFrame[255];
	//发送读取指令
	int res = m_SerialMaster.WriteToPort(bchSendFrame, iSendLength);
	if ( res != 0 )
	{
		return res;
	}
	//接收数据
	Sleep(20);
	const int iBytesMustbeRead = 9 + (iCount-1)*4;
	int iReceivLength = m_SerialMaster.ReadFromPort(bchReceiveFrame,iBytesMustbeRead);

	if( iReceivLength == iBytesMustbeRead )
	{
		unsigned int icrc = CRC16( bchReceiveFrame, iReceivLength - 2 );
		unsigned char crc_low = ((BYTE)(icrc>>8));
		unsigned char crc_hig = (BYTE)(icrc);

		if( crc_low != bchReceiveFrame[iReceivLength-2] || crc_hig != bchReceiveFrame[iReceivLength-1] )
		{
			return -2;
		}
	}
	else  
	{
		return -1;
	}

	//解析数据
	unsigned int iValues[100];
	int iParseCount=0;
	ParseInt32(bchReceiveFrame,iReceivLength, iValues, iParseCount);

	for ( int i=0; i<iParseCount; i++ )
	{
		(pvalue)[i] = iValues[i];
		// 		SwapDataHighLow(*pvalue);
	}

	return 0;
}