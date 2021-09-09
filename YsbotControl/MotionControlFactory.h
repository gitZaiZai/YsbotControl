// Copyright 2016, 亚思信息科技有限责任公司
// All right reserved.
//
// Author: Liu Run Dan ( lrdcz@163.com )
//
// Yasi Robot Motion Control System - 亚思机器人运动控制系统

#ifndef motion_control_factory_h
#define motion_control_factory_h
#include "MotionControlType.h"
#include "ConfigReader.h"
#include <string>
#include <vector>
#include <map>


class MotionControlBuilder {
public:
	virtual ~MotionControlBuilder () throw () {;}

	virtual MotionControlType* get_motion_control (const std::string&, const ConfigReader&, MotionControlType*) throw ( ) =0;
};

class MotionControlFactory {
private:
	std::map<std::string, MotionControlBuilder*> list_of_plugins; 
	static MotionControlFactory* the_only_factory;                
	MotionControlFactory () throw ();                            
	~MotionControlFactory() throw ();                             

public:
	static void destroy_factory();

	static MotionControlFactory* get_motion_control_factory () throw (std::bad_alloc);

	void sign_up (const std::string, MotionControlBuilder*) throw (std::bad_alloc);

	MotionControlType* get_motion_control (const std::string, const ConfigReader&) throw ( YsbotsException,std::bad_alloc,std::invalid_argument );

	void motion_control_list (std::vector<std::string>&) const throw (std::bad_alloc);
};



#endif