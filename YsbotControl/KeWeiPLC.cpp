// Copyright 2016, 亚思信息科技有限责任公司
// All right reserved.
//
// Author: Liu Run Dan ( lrdcz@163.com )
//
// Yasi Robot Motion Control System - 亚思机器人运动控制系统

#include "StdAfx.h"
#include "KeWeiPLC.h"
#include "FieldBusDeviceFactory.h"
#include "YsibotsException.h"
#include "Journal.h"
#include <strstream>
#include <iomanip>
#include "stringconvert.h"
#include "WorldModel.h"
#include <vector>

#define MAX_READ_WORD (60)

using namespace std;

const BYTE STX = 0x02;
const BYTE ETX = 0x03;
const BYTE EOT = 0x04;
const BYTE ENQ = 0x05;
const BYTE ACK = 0x06;
const BYTE NAK = 0x15;
const BYTE C[]="0123456789ABCDEF";

namespace
{
	class Builder : public FieldBusDeviceBuilder 
	{
		static Builder the_builder;
	public:
		Builder () {
			FieldBusDeviceFactory::get_fieldbus_device_factory ()->sign_up (std::string("KeWei"), this);
		}
		FieldBusDeviceType* get_fieldbus_device (const std::string&, const ConfigReader& reader, FieldBusDeviceType*) throw (std::bad_alloc) {
			return new KeWeiPLC (reader);
		}
	};
	Builder the_builder;
}

BYTE KeWeiPLC::GetData(BYTE num)
{
 	BYTE D;
	D = C[num];

	return D;
}

KeWeiPLC::KeWeiPLC (const ConfigReader& reader) throw (InvalidConfigurationException,std::bad_alloc) :SlaveID('A'),
	parity(NOPARITY),databits(8),stopbits(ONESTOPBIT),use_plc(false)
{
 	TRACE(_T("Constuct KeWeiPLC!\n")); 

	if ( reader.get("KeWei::port", portnr) <= 0 ) 
	{
 		throw InvalidConfigurationException ("KeWei_port");
	}
	baud = 115200;
	reader.get("KeWei::baud", baud);
 
// 	reader.get("KeWei::parity", parity); 
// 	reader.get("KeWei::databit", databits); 
// 	reader.get("KeWei::stopbit", stopbits);
	std::string stradr, adr;
	if ( reader.get("KeWei::x_first_address", stradr) <= 0 ) 
	{
 		throw InvalidConfigurationException ("KeWei_str_x_address");
	}
	adr = stradr.substr(1); // erase 'D'
	fp.x_fst_address     = atoi(adr.c_str());

	if ( reader.get("KeWei::y_first_address", stradr) <= 0 ) 
	{
 		throw InvalidConfigurationException ("KeWei_str_y_address");
	}
	adr = stradr.substr(1);  
	fp.y_fst_address = atoi(adr.c_str());

	if ( reader.get("KeWei::mx_first_address", stradr) <= 0 ) 
	{
 		throw InvalidConfigurationException ("KeWei_str_mx_address");
	}
	adr = stradr.substr(1);  
	fp.mx_fst_address = atoi(adr.c_str());

	if ( reader.get("KeWei::my_first_address", stradr) <= 0 ) 
	{
 		throw InvalidConfigurationException ("KeWei_str_my_address");
	}
	adr = stradr.substr(1);  
	fp.my_fst_address = atoi(adr.c_str());

	if ( reader.get("KeWei::d_first_address", stradr) <= 0 ) 
	{
 		throw InvalidConfigurationException ("KeWei_str_d_address");
	}
	adr = stradr.substr(1);  
	fp.d_fst_address = atoi(adr.c_str());

	int itemp = 10;
 	reader.get("KeWei::slave_address", itemp);
	SlaveID = GetData(itemp);
 
    reader.get("KeWei::x_num",  fp.x_point_num);
	reader.get("KeWei::y_num",  fp.y_point_num);	 
	reader.get("KeWei::mx_num", fp.mx_relay_num);	 
	reader.get("KeWei::my_num", fp.my_relay_num); 
	reader.get("KeWei::d_num",  fp.d_register_num );
	reader.get("KeWei::use_kewei_plc",  use_plc );
	fp.scan_cycle_time = 100;
	reader.get("KeWei::scan_cycle_time",  fp.scan_cycle_time );

	bool res = false;
	if ( use_plc )
	{
		res = Init( portnr, baud, parity, databits, stopbits );
		if ( !res )
		{
			throw HardwareException(" KeWei PLC port open failed");
		}
 	}

	fp.y_word_size  = static_cast<int>( ceil( static_cast<double>(fp.y_point_num)  / 16.0 ) );
	fp.my_word_size = static_cast<int>( ceil( static_cast<double>(fp.my_relay_num) / 16.0 ) );
	total_y_size    = fp.y_word_size + fp.my_word_size;
 
	fp.x_word_size  = static_cast<int>( ceil( static_cast<double>(fp.x_point_num)  / 16.0 ) );
	fp.mx_word_size = static_cast<int>( ceil( static_cast<double>(fp.mx_relay_num) / 16.0 ) );
	total_x_size    = fp.x_word_size + fp.mx_word_size;

	fp.max_read_word = MAX_READ_WORD;
 	fp.is_connected   = res;
}

KeWeiPLC::~KeWeiPLC()
{
	CloseCom();
}

int KeWeiPLC::connect()
{
	return Init( portnr, baud, parity, databits, stopbits );
}

int KeWeiPLC::din( const int port, bool* val )
{
//	TRACE(_T("kewei input!\n"));
	int pnb = port;
	if ( port > fp.x_point_num+fp.mx_relay_num )
 		return -1;

 	if( port > fp.x_point_num - 1 )
 		pnb += ( fp.x_word_size * 16 - fp.x_point_num ); // port id jump for 16*N word operate 

 	int word_NO  = static_cast<int>( floor( static_cast<double>(pnb) / 16.0 ) ); 
	int bit_NO   = pnb % 16;

	if( word_NO >= total_x_size || word_NO < 0 )
		return -1;


	int adr = fp.x_fst_address;
	if ( word_NO < fp.x_word_size ) // x input
	{
		adr = adr + word_NO;
	}
	else if ( word_NO >= fp.x_word_size && word_NO < total_x_size ) // m reg input
	{
		int offset_adr = word_NO - fp.x_word_size;
		adr = fp.mx_fst_address + offset_adr;
	}
	unsigned short valTemp = 0;
	int iret = ReadFromPlc( adr, 1, &valTemp ); // read one word from PLC.

	if ( iret == 0 )
	{
		*val = GETBIT( valTemp, bit_NO );
 	}

	return iret;
}

int KeWeiPLC::dout( const int port, const bool val )
{
//	TRACE(_T("kewei output!\n"));
	int pnb = port;
	if ( port > fp.y_point_num+fp.my_relay_num )
		return -1;

	if( port > fp.y_point_num - 1 )
		pnb += ( fp.y_word_size * 16 - fp.y_point_num ); // jump for 16 

	int word_NO  = static_cast<int>( floor( static_cast<double>(pnb) / 16.0 ) ); 
	int bit_NO   = pnb % 16;

	if( word_NO >= total_y_size || word_NO < 0 )
 		return -1;

	unsigned short valTemp = 0;

	const InOutObjectList& old = MWM.get_io_objects(IOSfieldbus);
	InOutObject ionew;
	 
    int adr = fp.y_fst_address;
	if ( word_NO < fp.y_word_size ) // y output
	{
		adr = fp.y_fst_address + word_NO;
		ionew.object_type = InOutObject::filedbus_y;
		ionew.index = word_NO;
	}
	else if ( word_NO >= fp.y_word_size && word_NO < total_y_size ) // m reg output
	{
		int offset_adr = word_NO - fp.y_word_size;
		adr = fp.my_fst_address + offset_adr;
		ionew.object_type = InOutObject::filedbus_my;
		ionew.index = offset_adr;
 	}	
	valTemp = old.get_state( ionew.object_type, ionew.index );

	bool cur_bit_val = GETBIT( valTemp, bit_NO );
	if ( cur_bit_val == val )
 		return 0;

 	if ( val )
 		SETBIT( valTemp, bit_NO );
 	else
		CLRBIT( valTemp, bit_NO );

	
	int ret = WriteToPlc( adr, 1, &valTemp );

	ionew.state = valTemp;

	Ysbot::Time t;
	MWM.set_io_information(ionew,t);

 	return ret;
}

// N 1 8 固定协议
bool KeWeiPLC::Init(UINT portnr/* = 1*/, UINT baud/* = 19200*/, UINT parity /*= EVENPARITY*/
	, UINT databits/* = 8*/, UINT stopbits /*= ONESTOPBIT*/, int iOverTime/*=2000*/)
{
	bool bReturn = m_SerialMaster.InitPort(portnr, baud, parity, databits, stopbits, iOverTime);
	return bReturn;
}

//////////////////////////////////////////////////////////////////////////

// send data
int KeWeiPLC::CalculateSum( BYTE *pch, int iLen, bool bFillFlag )
{
	BYTE sum = 0;
	for( int i=1 ; i< iLen-2 ; i++ )
	{
		sum = sum+pch[i];
	}

	if ( bFillFlag )
	{
		pch[iLen-1]=GetData((BYTE)(sum&0x0f));
		pch[iLen-2]=GetData((BYTE)((sum&0xf0)>>4));
 	}

	return sum;
}

// receive data
bool KeWeiPLC::CheckSum( BYTE *pch, int iLen )
{
	BYTE sum = CalculateSum( pch, iLen, false );
	 
	if( pch[iLen-1] == GetData((BYTE)(sum&0x0f)) && 
		pch[iLen-2] == GetData((BYTE)((sum&0xf0)>>4)) )
	{
		return true;
	}
	return false;
}

int KeWeiPLC::FX_H_atoi(const   char   *s)
{
	const char   *p   =   s;
	char   c;
	int   i   =   0;
	int v=0;
	while(c=*p++)
	{
		if(c>='0'   &&   c<='9')
			i=0;
		else
		{
			if(c>='A'   &&   c<='F')
				i=1;
			else
				i=2;
		}		
		switch(i)
		{
		case 0:
			{
				v = v*16 + (c-'0');				
			}
			break;
		case 1:
			{
				v = v*16 + (c-0X37);
			}
			break;
		case 2:
			{
				return v;
				//v=-1;
			}
			break;
		default:
			break;
		}
	}
	return v;
} 

int KeWeiPLC::WriteToPlc( const int adr, const unsigned int iCount, const unsigned short* pvalue )
{
	if ( !use_plc )
		return 0;

 	int ireg     = adr;
	char sReg[5] = {0};
	sprintf(sReg,"%04d",ireg);

	char dataCnt[3] = {0};
	int iCnt = iCount;
	if ( iCnt > 0X40 ) 		iCnt = 0x40;
	if ( iCnt < 1 ) 		iCnt = 1;
	sprintf(dataCnt,"%02X",iCnt);

	const int length = 17 + 4 * iCnt;

 	unsigned char* chbuff = new unsigned char[length];
	memset(chbuff,0,length * sizeof(unsigned char));
	chbuff[0] = 05;
	chbuff[1] = '0';
	chbuff[2] = SlaveID;
	chbuff[3] = 'F';
	chbuff[4] = 'F';
	chbuff[5] = 'W';
	chbuff[6] = 'W';
	chbuff[7] = 'A';
	chbuff[8] = 'D';
	chbuff[9]  = sReg[0];
	chbuff[10] = sReg[1];
	chbuff[11] = sReg[2];
	chbuff[12] = sReg[3];
	chbuff[13] = static_cast<unsigned char>(dataCnt[0]);
	chbuff[14] = static_cast<unsigned char>(dataCnt[1]);

	int baseadr = 15;

	BYTE BB[4] = {0};
	unsigned short data = 0;
	for ( int i = 0; i < iCnt; i++ )
	{
		data = *pvalue++;
		BB[0]=GetData((BYTE)((data&0x0000000F)));
		BB[1]=GetData((BYTE)((data&0x000000F0)>>4));
		BB[2]=GetData((BYTE)((data&0x00000F00)>>8));
		BB[3]=GetData((BYTE)((data&0x0000F000)>>12));

		chbuff[baseadr++] = BB[3];
		chbuff[baseadr++] = BB[2];
		chbuff[baseadr++] = BB[1];
		chbuff[baseadr++] = BB[0];

		if (baseadr > ( length - 4 ) )
		{
			break;
		}
	}

	CalculateSum(chbuff, length);
	int ret = 0;
	unsigned char chReads[255];
	int iReadLength=0;
	const unsigned int retriesMax = 100;
	unsigned int failCount = 0;
	do 
	{
		ret = m_SerialMaster.WriteToPort(chbuff, length);
 		Sleep(10+5*iCnt);

		iReadLength = m_SerialMaster.ReadFromPort( chReads, 5 );
		
		if ( failCount++ >= retriesMax) 
		{
			ret = -3;
			break;
		}
		else
		{
		    TRACE(_T("len = %d %d\n"), iReadLength, failCount );
 		}

	} while ( ret != 0 || iReadLength != 5 );

	delete [] chbuff;
	if ( iReadLength != 5 )
	{
		return -1;
	}
	return 0;
}

int KeWeiPLC::ReadFromPlc( const int adr, const unsigned int iCount, unsigned short* pvalue )
{
	//05 30 41 46 46 57 52 41 44 37 32 32 33 30 31 35 41
	if ( !use_plc )
 		return 0;

  	int ireg     = adr;
	char sReg[5] = {0};
	sprintf(sReg,"%04d",ireg);

	char dataCnt[3] = {0};
	unsigned int iCnt = iCount;
	if ( iCnt > MAX_READ_WORD )  // max is 64		
		iCnt = MAX_READ_WORD;
	if ( iCnt < 1 ) 		
		iCnt = 1;
  
	sprintf(dataCnt,"%02X",iCnt);
 
	const int length = 17;
	unsigned char chbuff[17]={0};
	chbuff[0] = 0x05;
	chbuff[1] = '0';
	chbuff[2] = SlaveID;
	chbuff[3] = 'F';
	chbuff[4] = 'F';
	chbuff[5] = 'W';
	chbuff[6] = 'R';
	chbuff[7] = 0x30;
	chbuff[8] = 'D';
	chbuff[9]  = sReg[0];
	chbuff[10] = sReg[1];
	chbuff[11] = sReg[2];
	chbuff[12] = sReg[3];
	chbuff[13] = dataCnt[0];
	chbuff[14] = dataCnt[1];
	CalculateSum(chbuff, length);

	int ret = m_SerialMaster.WriteToPort( chbuff, length );
	
 	Sleep(10+5*iCnt);

	const unsigned int iReads = 8 + 4 * iCnt;

 	unsigned char* chReads = new unsigned char[iReads];
//	unsigned char chReads[512];
	memset(chReads,0, iReads * sizeof(unsigned char));
	int iReadLength=0;
	const unsigned int retriesMax = 100;
	unsigned int failCount = 0;
 	 
	iReadLength = m_SerialMaster.ReadFromPort( chReads, iReads );
 
	if ( iReadLength != iReads || !CheckSum( chReads, iReads ) )
	{
		delete [] chReads;
		return -2;
	}

	unsigned short int itemp;
	char C[5] = {0};

	unsigned int baseadr = 5;

	for ( unsigned int i = 0; i < iCnt; i++ )
	{
		C[0] =chReads[baseadr++]; // 5 6 7 8, 9 10 11 12,...
		C[1] =chReads[baseadr++];
		C[2] =chReads[baseadr++];
		C[3] =chReads[baseadr++];
		C[4] = 0;

		itemp = FX_H_atoi(C);

		*( pvalue+i ) = itemp;

		if( baseadr > iReads - 4 )
			break;
	}

 	delete[] chReads;
	return 0;
}

int KeWeiPLC::read_data_reg( const int addrOffset, unsigned short* pdata )
{
	int addr = fp.d_fst_address + addrOffset;
	return 	ReadFromPlc( addr, 1, pdata );
}

int KeWeiPLC::write_data_reg( const int addrOffset, const unsigned short data)
{
	int addr = fp.d_fst_address + addrOffset;
	return 	WriteToPlc( addr, 1, &data );
}

int KeWeiPLC::read_multiple_word( const int addr, const unsigned int iCount, unsigned short* pdata)
{
	return ReadFromPlc( addr, iCount, pdata );
}

int KeWeiPLC::write_multiple_word( const int addr, const unsigned int iCount, const unsigned short* pdata)
{
	return WriteToPlc( addr, iCount, pdata );
}