// Copyright 2016, 亚思信息科技有限责任公司
// All right reserved.
//
// Author: Liu Run Dan ( lrdcz@163.com )
//
// Yasi Robot Motion Control System - 亚思机器人运动控制系统

#ifndef _servo_h
#define _servo_h

#include <stdexcept>
#include "ServoType.h"
#include "ConfigReader.h"
#include "GroupProperties.h"
#include <vector>
#include <map>
#include "ThreadTaskTypeBase.h"

class Servo : public ThreadTaskTypeBase
{
private:
	ServoType* the_servo;
	char* servo_descriptor;
	std::map<std::string, ServoType*> list_of_servo; 
	std::vector<ServoType*> active_servo; 

	const ConfigReader& configuration_list;

	void really_change_servo_type (const char*, const ConfigReader&) throw (YsbotsException,std::bad_alloc);

	bool open_port_flag;
	bool bNewServoPos;
    int  servo_num;
    
	unsigned int thread_task(ThreadTaskTypeBase*);
	double servo_pos[MAX_JOINT_NUM];

	bool is_simulted[MAX_JOINT_NUM];
	char errormsg[MAX_ERRSTR_LEN];

	volatile bool    knowAbsPos;
	bool        isConnectServo;

public:
	static Servo& get_main_servo() throw ();
	static bool is_main_servo_available () throw ();
	static Servo* main_servo;

	Servo (const ConfigReader&) throw (YsbotsException, std::bad_alloc );
	~Servo () throw ();
 
	bool change_servo_type (const char*) throw ();

	bool change_servo_type (const char*, const ConfigReader&) throw ();

	const char* get_servo_type () const throw ();

	void getServoTypeList(std::vector<std::string> &ptl);
 
	int read_pos( unsigned int iDeviceID,	long* );

	int clear_pos( unsigned int iDeviceID ); 

	int clear_alarm( unsigned int iDeviceID );

	bool init_port () throw (YsbotsException, std::bad_alloc );

	bool close();

	int  read_pos_from_servo( double* pos );

	int clear_all_servo_pos(void);

	void  process_servo_message();

	bool is_connect_servo() { return isConnectServo; }
};

#endif