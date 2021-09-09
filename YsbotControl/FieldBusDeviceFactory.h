// Copyright 2016, 亚思信息科技有限责任公司
// All right reserved.
//
// Author: Liu Run Dan ( lrdcz@163.com )
//
// Yasi Robot Motion Control System - 亚思机器人运动控制系统

#ifndef fieldbus_device_factory_h
#define fieldbus_device_factory_h

#include "FieldBusDeviceType.h"
#include "ConfigReader.h"
#include <string>
#include <vector>
#include <map>


class FieldBusDeviceBuilder {
public:
	virtual ~FieldBusDeviceBuilder () throw () {;}

	virtual FieldBusDeviceType* get_fieldbus_device (const std::string&, const ConfigReader&, FieldBusDeviceType*) throw ( ) =0;
};

class FieldBusDeviceFactory {
private:
	std::map<std::string, FieldBusDeviceBuilder*> list_of_plugins; 
	static FieldBusDeviceFactory* the_only_factory;                
	FieldBusDeviceFactory () throw ();                            
	~FieldBusDeviceFactory() throw ();                             

public:
	static void destroy_factory();

	static FieldBusDeviceFactory* get_fieldbus_device_factory () throw (std::bad_alloc);

	void sign_up (const std::string, FieldBusDeviceBuilder*) throw (std::bad_alloc);

	FieldBusDeviceType* get_fieldbus_device (const std::string, const ConfigReader&) throw ( YsbotsException,std::bad_alloc,std::invalid_argument );

	void fieldbus_device_list (std::vector<std::string>&) const throw (std::bad_alloc);
};


#endif
