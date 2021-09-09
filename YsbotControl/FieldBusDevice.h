// Copyright 2016, 亚思信息科技有限责任公司
// All right reserved.
//
// Author: Liu Run Dan ( lrdcz@163.com )
//
// Yasi Robot Motion Control System - 亚思机器人运动控制系统

#ifndef fieldbus_device_h
#define fieldbus_device_h

#include <stdexcept>
#include "FieldBusDeviceType.h"
#include "ConfigReader.h"
#include "Time.h"
#include <vector>
#include <map>
#include "ThreadTaskTypeBase.h"

class FieldBusDevice : public ThreadTaskTypeBase
{
private:
	FieldBusDeviceType* the_fieldbus_device;
	char* fieldbus_device_descriptor;
 
	const ConfigReader& configuration_list;

	void really_change_fieldbus_device_type (const char*, const ConfigReader&) throw (YsbotsException,std::bad_alloc);
 
	unsigned int px_word_num;
	unsigned int mx_word_num;
	unsigned int dr_word_num;
	int px_addr;
	int mx_addr;
	int dr_addr;

	int max_words;

	unsigned short* x_input;
	unsigned short* x_relay;
	unsigned short* regdata;
 	 
    unsigned int thread_task(ThreadTaskTypeBase*);

public:
	static FieldBusDevice& get_main_fieldbus_device() throw ();
	static bool is_main_fieldbus_device_available () throw ();
	static FieldBusDevice* main_fieldbus_device;

	FieldBusDevice (const ConfigReader&) throw (YsbotsException, std::bad_alloc );
	~FieldBusDevice () throw ();
 
	bool change_fieldbus_device_type (const char*) throw ();

	bool change_fieldbus_device_type (const char*, const ConfigReader&) throw ();

	const char* get_fieldbus_device_type () const throw ();

	void getFieldBusDeviceTypeList(std::vector<std::string> &ptl);
 	 
	int  din ( const int port, bool* val);
	int  dout ( const int port, const bool val);

	int  read_data_reg (  const int, unsigned short* );
	int  write_data_reg ( const int, const unsigned short  );

	int  read_multiple_word (  const int, const int, unsigned short* );
	int  write_multiple_word ( const int, const int, const unsigned short*  );
    
	bool process_io_signals( Ysbot::Time t ) throw();

 	void change_scan_area( const int, const int )  throw();
};

#endif