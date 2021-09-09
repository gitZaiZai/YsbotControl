// Copyright 2016, 亚思信息科技有限责任公司
// All right reserved.
//
// Author: Liu Run Dan ( lrdcz@163.com )
//
// Yasi Robot Motion Control System - 亚思机器人运动控制系统

#ifndef _servoFactory_h
#define _servoFactory_h

#include "ServoType.h"
#include "ConfigReader.h"
#include <string>
#include <vector>
#include <map>


class ServoBuilder {
public:
	virtual ~ServoBuilder () throw () {;}

	virtual ServoType* get_servo (const std::string&, const ConfigReader&, ServoType*) throw ( ) =0;
};

class ServoFactory {
private:
	std::map<std::string, ServoBuilder*> list_of_plugins; 
	static ServoFactory* the_only_factory;                
	ServoFactory () throw ();                            
	~ServoFactory() throw ();                             

public:
	static void destroy_factory();

	static ServoFactory* get_servo_factory () throw (std::bad_alloc);

	void sign_up (const std::string, ServoBuilder*) throw (std::bad_alloc);

	ServoType* get_servo (const std::string, const ConfigReader&) throw ( YsbotsException,std::bad_alloc,std::invalid_argument );

	void servo_list (std::vector<std::string>&) const throw (std::bad_alloc);
};


#endif
