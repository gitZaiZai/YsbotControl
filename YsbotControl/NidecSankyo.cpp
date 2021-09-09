// Copyright 2016, 亚思信息科技有限责任公司
// All right reserved.
//
// Author: Liu Run Dan ( lrdcz@163.com )
//
// Yasi Robot Motion Control System - 亚思机器人运动控制系统

#include "StdAfx.h"
#include "NidecSankyo.h"
#include "ServoFactory.h"
#include "YsibotsException.h"
#include "Journal.h"
#include "stringconvert.h"

using namespace std;

namespace
{
	class Builder : public ServoBuilder 
	{
		static Builder the_builder;
	public:
		Builder () {
			ServoFactory::get_servo_factory ()->sign_up (std::string("Nidec"), this);
		}
		ServoType* get_servo (const std::string&, const ConfigReader& reader, ServoType*) throw (std::bad_alloc) {
			return new NidecSankyo (reader);
		}
	};
	Builder the_builder;
}


NidecSankyo::NidecSankyo(const ConfigReader& reader) throw (InvalidConfigurationException): configuration_list(const_cast<ConfigReader&>(reader)),
	portnr(1),baud(38400),parity(NOPARITY),databits(8),stopbits(ONESTOPBIT),iOverTime(2000),open_port(false)
{
	if ( reader.get("Nidec::port", portnr) <= 0 ) 
	{
		JERROR("no config line \"ServoDriver_port\" found");
		throw InvalidConfigurationException ("ServoMaxsine_port");
	}

	if ( reader.get("Nidec::baud", baud) <= 0 ) 
	{
		JERROR("no config line \"ServoDriver_baud\" found");
		throw InvalidConfigurationException ("ServoMaxsine_baud");
	}

	if ( reader.get("Nidec::parity", parity) <= 0 ) 
	{
		JERROR("no config line \"ServoDriver_databit\" found");
		throw InvalidConfigurationException ("ServoMaxsine_databit");
	}

	if ( reader.get("Nidec::databit", databits) <= 0 ) 
	{
		JERROR("no config line \"ServoDriver_parity\" found");
		throw InvalidConfigurationException ("ServoMaxsine_parity");
	}

	if ( reader.get("Nidec::stopbit", stopbits) <= 0 ) 
	{
		JERROR("no config line \"ServoDriver_stopbit\" found");
		throw InvalidConfigurationException ("ServoMaxsine_stopbit");
	}
	TRACE(_T("Constuct ServoMaxsine!\n")); 
}


NidecSankyo::~NidecSankyo(void)
{
}

unsigned int crc_table [16]={   
	0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,   
	0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef,   
};  

// puchMsg 为数据指针，len 为数据字节长度   
unsigned short CRC_CCITT(unsigned char *puchMsg , unsigned int usDataLen)    
{   
	unsigned int crc  = 0xFFFF; // CRC 初始码   
	unsigned char da4 = 0;  
	unsigned char *ptr = puchMsg;
	while(usDataLen--!=0)   
	{   
		da4 = ((unsigned char)(crc/256))/16;   
		crc <<= 4;   
		crc ^= crc_table [da4^(*ptr /16)];   
		da4 = ((unsigned char)(crc/256)/16);   
		crc <<= 4;   
		crc ^= crc_table [da4^(*ptr&0x0f)];   
		ptr++;   
	}   
	return(crc);   
}   
 
BYTE NidecSankyo::Number2HexChar(BYTE num)
{
	static BYTE C[]="0123456789ABCDEF";
	BYTE D;
	D = C[num];	
	return D;
}

BOOL NidecSankyo::Init( UINT portnr /* = 1 */, UINT baud /* = 57600 */, UINT parity /* = NOPARITY */, 
	UINT databits /* = 8 */, UINT stopbits /* = ONESTOPBIT */, int iOverTime /* = 2000 */ )
{
	BOOL bReturn = m_SerialMaster.InitPort(portnr, baud, parity, databits, stopbits, iOverTime);
	return bReturn;
}

int NidecSankyo::CLEAR_EA05_DATA(unsigned int iDeviceID)
{
	// 	if( m_SerialMaster.get_flag_open() == false )
	// 		return -1;

	int iReturn = -1;

	const int length = 8;
	unsigned char chbuff[length]={0};

	chbuff[0] = 0X24;  
	chbuff[1] = iDeviceID; // address         
	chbuff[2] = 0X0;         // control code
	chbuff[3] = 0X1F;      // command code  

	chbuff[4] = 0X00;
	chbuff[5] = 0X02;      // command parameter
	chbuff[6] = 0;
	chbuff[7] = 0;

	unsigned int icrc = CRC_CCITT(chbuff, length-2);
	chbuff[6] = ((BYTE)(icrc>>8));
	chbuff[7] = (BYTE)(icrc);

	iReturn = m_SerialMaster.WriteToPort((BYTE*)chbuff, length);
	if( iReturn != 0 )
		return -1;

	Sleep(35);

	unsigned char chReads[255];
	const int iReads = 8;  

	int iReadLength = 0;
	iReadLength = m_SerialMaster.ReadFromPort( (BYTE*)chReads, iReads );
	if( iReadLength != iReads )
	{
		return -2;
	}

	unsigned short crc_val_read = CRC_CCITT( chReads, iReads-2 );  
	unsigned char chHigh = ((BYTE)(crc_val_read>>8));
	unsigned char chLow  = (BYTE)(crc_val_read);

	if( chHigh != chReads[iReads-2] || chLow != chReads[iReads-1] )  
	{
		return -3;
	}

	if( chReads[5] != 0 || chReads[4] != 0 ) // para1
	{
		return -4;
	}

	if( chReads[2] != 0X80 )
	{
		return chReads[2]; // error code
	}

	return 0;

}

int NidecSankyo::READ_EA05_DATA_EX(unsigned int iDeviceID,short &nEncoderLaps,DWORD &wEncoderPulses)
{

	int iReturn = -1;

	const int length = 12;
	unsigned char chbuff[length] = {0};

	chbuff[0] = 0X28;  
	chbuff[1] = iDeviceID; // address         
	chbuff[2] = 0X0;       // control code
	chbuff[3] = 0x62;      // command code  

	chbuff[4] = 0X00;
	chbuff[5] = 0X01;      // command parameter
	chbuff[6] = 0;
	chbuff[7] = 0;
	chbuff[8] = 0;
	chbuff[9] = 0;
	chbuff[10] = 0;         // CRC FCS
	chbuff[11] = 0;

	unsigned int icrc = CRC_CCITT(chbuff, length-2);
	chbuff[10] = ((BYTE)(icrc>>8));
	chbuff[11] = (BYTE)(icrc);

	iReturn = m_SerialMaster.WriteToPort((BYTE*)chbuff, length);
	if( iReturn != 0 )
		return -1;

	Sleep(35);

	unsigned char chReads[255] = {0};
	const int iReads = 16;  

	int iReadLength = 0;
	iReadLength = m_SerialMaster.ReadFromPort( (BYTE*)chReads, iReads );
	if( iReadLength != iReads )
	{
		return -2;
	}

	unsigned short crc_val_read = CRC_CCITT( chReads, iReads-2 );  
	unsigned char chHigh = ((BYTE)(crc_val_read>>8));
	unsigned char chLow  = (BYTE)(crc_val_read);

	if( chHigh != chReads[iReads-2] || chLow != chReads[iReads-1] )  
	{
		return -3;
	}

	// 	if( chReads[5] != 0 || chReads[4] != 0 ) // para1
	// 	{
	// 		return -4;
	// 	}

	if( chReads[2] != 0X80 )
	{
		return chReads[2]; // error code
	}

	DWORD data8  = chReads[8];
	DWORD data9  = chReads[9];
	DWORD data10 = chReads[10];
	DWORD data11 = chReads[11];

	wEncoderPulses = (data8 << 24) ^ ( data9 << 16 ) ^ ( data10 << 8 ) ^ data11;

	short data_high = chReads[12];
	short data_low  = chReads[13];

	nEncoderLaps = ( data_high << 8 ) ^ data_low;

	return 0;
}

int NidecSankyo::GET_STATE_VALUE_2(unsigned int iDeviceID,unsigned int iAddress)
{
	return 0;
}

// 0XC2 编码器／转子机械角(一圈)   UI4  (2^31)-1
// 0XC3 编码器／转子机械角(积算值)  I4 -(2^31)~(2^31)-1
int NidecSankyo::GET_STATE_VALUE_4(unsigned int iDeviceID,unsigned int iAddress, long &wEncoderData) // 0XC2 0XC3
{
	int iReturn = -1;

	const int length = 8;
	unsigned char chbuff[length]={0};

	chbuff[0] = 0X24;  
	chbuff[1] = iDeviceID;   // address         
	chbuff[2] = 0X0;         // control code
	chbuff[3] = 0X11;        // command code  

	chbuff[4] = 0X00;
	chbuff[5] = iAddress;      // command parameter 0XC3
	chbuff[6] = 0;
	chbuff[7] = 0;

	unsigned int icrc = CRC_CCITT(chbuff, length-2);
	chbuff[6] = ((BYTE)(icrc>>8));
	chbuff[7] = (BYTE)(icrc);

	iReturn = m_SerialMaster.WriteToPort((BYTE*)chbuff, length);
	if( iReturn != 0 )
		return -1;

	Sleep(35);

	unsigned char chReads[255] = {0};
	const int iReads = 10;  

	int iReadLength = 0;
	iReadLength = m_SerialMaster.ReadFromPort( (BYTE*)chReads, iReads );
	if( iReadLength != iReads )
	{
		return -2;
	}

	unsigned short crc_val_read = CRC_CCITT( chReads, iReads-2 );  
	unsigned char chHigh = ((BYTE)(crc_val_read>>8));
	unsigned char chLow  = (BYTE)(crc_val_read);

	if( chHigh != chReads[iReads-2] || chLow != chReads[iReads-1] )  // CRC
	{
		return -3;
	}

	if( chReads[2] != 0X80 )
	{
		return chReads[2]; // error code
	}

	DWORD data4 = chReads[4];
	DWORD data5 = chReads[5];
	DWORD data6 = chReads[6];
	DWORD data7 = chReads[7];

	wEncoderData = (data4 << 24) ^ ( data5 << 16 ) ^ ( data6 << 8 ) ^ data7;

	return 0;

}
  
int   NidecSankyo::read_pos( unsigned int iDeviceID, long* pData )
{
	if ( !open_port )
	{
		TRACE(_T("servo port no open!\n"));
		return -1;
	}
    long totalPls = 0;
	int iRes  = GET_STATE_VALUE_4( iDeviceID, 0XC3, totalPls );
 	if( iRes == 0 )
	{
		 *pData = totalPls;
 	}
	else
	{
		return -2;
 	}

// 	TRACE(_T("read pos NO.%d NidecSankyo!\n"),iDeviceID); 

	return 0;
}

int   NidecSankyo::clear_pos( unsigned int iDeviceID )
{
	if ( !open_port )
	{
		TRACE(_T("servo port no open!\n"));
		return -1;
	}

	const int retriesMax = 10;
	int failcount = 0;

	int ret = -1;
	long residual_pls = 0;

	TRACE(_T("clear_pos NO.%d NidecSankyo!\n"),iDeviceID); 
// 	do 
// 	{
// 		Sleep(1);
// 		ret = CLEAR_EA05_DATA(iDeviceID);
//         if ( ret != 0 )
//         {
// 			++failcount;
//         }
//  	} while (failcount < retriesMax);
//  
// 	failcount = 0;
// 	do 
//  	{
// 		Sleep(1);
// 		ret = read_pos( iDeviceID, &residual_pls );
// 		if ( ret != 0 )
// 		{
// 			++failcount;
// 		}
// 	}while (failcount < retriesMax);

	CLEAR_EA05_DATA(iDeviceID);
 	read_pos( iDeviceID, &residual_pls );

	unsigned long long id = iDeviceID;
	string prefix = "AXIS_" + to_string(id);

	configuration_list.set( ( prefix+string("::encoder_origin") ).c_str(), residual_pls );   

	if ( residual_pls != 0 )
	{
		//configuration_list.write_config_file( strconfile.c_str() );
		configuration_list.write_all();

	}
	return ret;
}

int   NidecSankyo::clear_alarm( unsigned int iDeviceID )
{
	if ( !open_port )
	{
		TRACE(_T("servo port no open!\n"));
		return -1;
	}
	TRACE(_T("clear_alarm NO.%d NidecSankyo!\n"),iDeviceID); 

	return 0;
}

bool NidecSankyo::init_port()
{
	TRACE(_T("init_port NidecSankyo!\n")); 

	bool bReturn = m_SerialMaster.InitPort(portnr, baud, parity, databits, stopbits, iOverTime);
	open_port = bReturn;

 	return bReturn;
}

bool NidecSankyo::close()
{
	TRACE(_T("CloseComPort NidecSankyo!\n")); 
	open_port = false;

	return m_SerialMaster.CloseComPort(); 
}
