// Copyright 2016, 亚思信息科技有限责任公司
// All right reserved.
//
// Author: Liu Run Dan ( lrdcz@163.com )
//
// Yasi Robot Motion Control System - 亚思机器人运动控制系统

#include "StdAfx.h"
#include "FieldBusDevice.h"

#include "FieldBusDeviceFactory.h"
#include "Journal.h"
#include <cstring>
#include "WorldModel.h"

using namespace std;

FieldBusDevice* FieldBusDevice::main_fieldbus_device=NULL;

FieldBusDevice& FieldBusDevice::get_main_fieldbus_device () throw () { return *FieldBusDevice::main_fieldbus_device; }

bool FieldBusDevice::is_main_fieldbus_device_available () throw()
{
	return main_fieldbus_device != NULL;
}

const char* FieldBusDevice::get_fieldbus_device_type () const throw () { return fieldbus_device_descriptor; }

FieldBusDevice::~FieldBusDevice () throw () 
{
	if( FieldBusDevice::main_fieldbus_device == this )
		FieldBusDevice::main_fieldbus_device = NULL;
   
	stop_task_thread();

	delete [] x_input;
	delete [] x_relay;
	delete [] regdata;

  	delete the_fieldbus_device;
	if (fieldbus_device_descriptor)
	{
		delete [] fieldbus_device_descriptor;
		fieldbus_device_descriptor = NULL;
	}

}

FieldBusDevice::FieldBusDevice (const ConfigReader& vread) throw ( YsbotsException, std::bad_alloc ) : ThreadTaskTypeBase("FieldBus Device"), 
	the_fieldbus_device(NULL), configuration_list(vread) 
{
	if( FieldBusDevice::main_fieldbus_device == NULL  )
		FieldBusDevice::main_fieldbus_device=this;  

 	the_fieldbus_device = NULL;
	fieldbus_device_descriptor = NULL;
	x_input = NULL;
	x_relay = NULL;
	regdata = NULL;
  
	string confline = "KeWei";     //  

	if (vread.get("fieldbus_device_type", confline)<=0) 
	{
		JERROR("no config line \"fieldbus_device_type_type\" found");
		throw InvalidConfigurationException ("fieldbus_device_type");
	}
	try
	{
		really_change_fieldbus_device_type (confline.c_str(), vread);
	}catch(YsbotsException& e)
	{
		JERROR((std::string ("creating fieldbus_device_type of type ")+confline+std::string(" failed due to: ")+std::string(e.what())).c_str());
		throw (e);
	}

	// HMI display I/O status.
	const IoModuleParameters& imp = the_fieldbus_device->get_fieldbus_io_parameters();
	if( WorldModel::is_main_world_model_available() ) 
	{  
		WorldModel::get_main_world_model().set_io_module_parameters( imp, IOSfieldbus );
	}
	else 
	{
		JWARNING("no worldmodel available");
	}

	px_word_num = imp.x_word_size;
	mx_word_num = imp.mx_word_size;
	dr_word_num = imp.d_register_num;
	px_addr     = imp.x_fst_address;
	mx_addr     = imp.mx_fst_address;
	dr_addr     = imp.d_fst_address;
	max_words   = imp.max_read_word;
	bool isconnect = imp.is_connected;
	task_cycle_time = imp.scan_cycle_time;

	x_input = new unsigned short[px_word_num];
	x_relay = new unsigned short[mx_word_num];
	regdata = new unsigned short[dr_word_num];
	
	for ( unsigned int i = 0; i < px_word_num; i++ )
 		x_input[i] = 0;
 	for ( unsigned int i = 0; i < mx_word_num; i++ )
		x_relay[i] = 0;
	for ( unsigned int i = 0; i < dr_word_num; i++ )
        regdata[i] = 0;
  
  	if ( isconnect )
	{
		start_task_thread();
 	}

}

bool FieldBusDevice::change_fieldbus_device_type (const char* pt) throw () {
	return change_fieldbus_device_type (pt, configuration_list);
}

bool FieldBusDevice::change_fieldbus_device_type (const char* pt, const ConfigReader& vread) throw () {
	try{

		really_change_fieldbus_device_type (pt, vread);
	}catch(bad_alloc&)
	{
		JWARNING(("Change of FieldBusDevice type failed due to lack of memory"));
		return false;
	} 
	return true;
}

void FieldBusDevice::really_change_fieldbus_device_type (const char* pt, const ConfigReader& vread) throw ( YsbotsException,std::bad_alloc) {
	FieldBusDeviceType* new_fieldbus_device;
	char* new_descriptor;
	try
	{
		string plt (pt);
		new_fieldbus_device = FieldBusDeviceFactory::get_fieldbus_device_factory()->get_fieldbus_device (plt, vread);
	}catch(invalid_argument){
		throw InvalidConfigurationException ("fieldbus_device_type");
 	}

	new_descriptor = new char [strlen(pt)+1];
	strcpy(new_descriptor,pt);
	if (the_fieldbus_device!=NULL) {
		delete the_fieldbus_device;
		delete [] fieldbus_device_descriptor;
	}

	the_fieldbus_device=new_fieldbus_device;
	fieldbus_device_descriptor=new_descriptor;
 
}

void FieldBusDevice::getFieldBusDeviceTypeList(std::vector<std::string> &ptl)
{
	FieldBusDeviceFactory::get_fieldbus_device_factory()->fieldbus_device_list(ptl);
}

int  FieldBusDevice::din ( const int port, bool* val)
{ 
	int ret;
	Lock();
	ret = the_fieldbus_device->din( port, val ); 
	UnLock();
 	return ret;
}
int  FieldBusDevice::dout ( const int port, const bool val) 
{ 
	int ret;
	Lock();
    ret = the_fieldbus_device->dout( port, val );
	UnLock();
 	return ret;
}

int  FieldBusDevice::read_data_reg( const int addroffset, unsigned short* pvar )
{ 
	int ret;
	Lock();
	ret = the_fieldbus_device->read_data_reg(addroffset,pvar);
	UnLock();
	return ret;
}

int  FieldBusDevice::write_data_reg( const int addroffset, const unsigned short var)  
{ 
	int ret;
	Lock();
    ret = the_fieldbus_device->write_data_reg(addroffset,var);
	UnLock();
	return ret; 
}

int  FieldBusDevice::read_multiple_word( const int addr, const int cnt, unsigned short* pvar )
{ 
	int ret;
	Lock();
 	ret = the_fieldbus_device->read_multiple_word(addr,cnt,pvar);
	UnLock();
	return ret; 
}

int  FieldBusDevice::write_multiple_word( const int addr, const int cnt, const unsigned short* pvar)  
{ 
	int ret;
	Lock();
	ret = the_fieldbus_device->write_multiple_word(addr,cnt,pvar); 
	UnLock();
	return ret;
}

//////////////////////////////////////////////////////////////////////////

unsigned int FieldBusDevice::thread_task( ThreadTaskTypeBase* pParam) 
{
	FieldBusDevice *pIoModule = ( FieldBusDevice* )pParam;
 
	int pxAddr = pIoModule->px_addr;
	int mxAddr = pIoModule->mx_addr;
	int drAddr = pIoModule->dr_addr;    // may be a variable address for scan data register in PLC.
	int npx = pIoModule->px_word_num;
	int nmx = pIoModule->mx_word_num;
	int ndr = pIoModule->dr_word_num; // may be a variable.

	const int max_read_reg_words = pIoModule->max_words;

	int number_times = static_cast<int>( ceil( static_cast<double>(ndr) / max_read_reg_words ) );
	int remainder    = ndr % max_read_reg_words;
     
	unsigned short* pstX          = pIoModule->x_input;
	unsigned short* pstMX         = pIoModule->x_relay;
	unsigned short* pstRegData    = pIoModule->regdata;
 
 	std::vector<int> cnt;
  	cnt.reserve(number_times);
	for ( int i = 0; i < number_times; i++ )
 		cnt.push_back(max_read_reg_words);

	if ( remainder != 0 && cnt.size() > 0 )
		cnt[number_times-1] = remainder;

	Ysbot::Time current_time;
	const long int loop_time = pIoModule->task_cycle_time;
	int nret = 0;
 	while( !exitFlag )
	{
// 		if ( current_time.elapsed_msec() > loop_time )
		{
//  			for ( unsigned int i = 0; i < pIoModule->px_word_num; i++ )
// 				pstX[i] = 0;
// 			for ( unsigned int i = 0; i < pIoModule->mx_word_num; i++ )
// 				pstMX[i] = 0;
// 			for ( unsigned int i = 0; i < pIoModule->dr_word_num; i++ )
// 				pstRegData[i] = 0;

			Lock();

			nret = pIoModule->read_multiple_word( pxAddr, npx, pstX );
 
			nret = pIoModule->read_multiple_word( mxAddr, nmx, pstMX ); // read more words lead to memory leak easily.
  
			for ( int i = 0; i < number_times; i++ )
			{
   				nret = pIoModule->read_multiple_word( drAddr + i * max_read_reg_words, cnt[i], pstRegData + i * max_read_reg_words );
 			}
  
			UnLock();

			if ( nret == 0 )
 				bNewInfo = true;
 			else
				bNewInfo = false;

			current_time.update();
 		}
 		Sleep(1); // important for another thread to read variable.
	}

 	cnt.clear();

	return 0;
}

bool FieldBusDevice::process_io_signals( Ysbot::Time t ) throw()
{
	if ( bNewInfo == false )
 		return false;
 
   	InOutObject ionew;
  
//     Lock();
	for (unsigned int i = 0; i < px_word_num; i++ )
	{
		ionew.object_type = InOutObject::fieldbus_x;
		ionew.index = i;
		ionew.state = x_input[i];
		MWM.set_io_information( ionew, t );
 	}

	for ( unsigned int i = 0; i < mx_word_num; i++ )
	{
		ionew.object_type = InOutObject::fieldbus_mx;
		ionew.index = i;
  	
		ionew.state = x_relay[i];

		MWM.set_io_information( ionew, t );
	}

	for ( unsigned int i = 0; i < dr_word_num; i++ )
	{
		ionew.object_type = InOutObject::filedbus_d;
		ionew.index = i;

 		ionew.state = regdata[i];

		MWM.set_io_information( ionew, t );
	}

	ThreadTaskTypeBase::bNewInfo = false;
//     UnLock();

	return true;
}

void FieldBusDevice::change_scan_area( const int addrfst, const int c ) throw()
{
	Lock();

	dr_addr = addrfst;

	if ( regdata )
	{
		delete [] regdata;
 	}

	dr_word_num = c;
	regdata     = new unsigned short[c];

	UnLock();
}