// Copyright 2016, 亚思信息科技有限责任公司
// All right reserved.
//
// Author: Liu Run Dan ( lrdcz@163.com )
//
// Yasi Robot Motion Control System - 亚思机器人运动控制系统

#ifndef _servoDummy_h
#define _servoDummy_h
#include "servotype.h"
#include "ConfigReader.h"

class ServoDummy :
	public ServoType
{
public:
	ServoDummy(const ConfigReader&) throw (InvalidConfigurationException);
	~ServoDummy(void);

	bool init_port();

	int   read_pos( unsigned int iDeviceID, long* );

	int   clear_pos( unsigned int iDeviceID );
	int   clear_alarm( unsigned int iDeviceID );

	bool  close();

private:
	ConfigReader& configuration_list;
	bool open_port;
};

#endif