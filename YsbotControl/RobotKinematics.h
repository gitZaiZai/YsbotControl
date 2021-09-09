// Copyright 2016, 亚思信息科技有限责任公司
// All right reserved.
//
// Author: Liu Run Dan ( lrdcz@163.com )
//
// Yasi Robot Motion Control System - 亚思机器人运动控制系统

#ifndef robot_kinematics_h
#define robot_kinematics_h
#include <stdexcept>
#include "ConfigReader.h"
#include <vector>
#include <map>
#include "KinematicsBase.h"
// #include "PosInfo.h"

class RobotKinematics
{
private:
 	char* robot_kinematics_descriptor;
    
	const ConfigReader& configuration_list;

	void really_change_robot_kinematics_type (const char*, const ConfigReader&) throw (YsbotsException,std::bad_alloc);
	
	Ysbot::DHParaBase dh_base;
	int  axis_num_kine;
	bool use_robot_lib;
	int  nGroupIdx;

public:
	static RobotKinematics& get_main_robot_kinematics() throw ();
	static bool is_main_robot_kinematics_available () throw ();
	static RobotKinematics* main_robot_kinematics;

	RobotKinematics (const ConfigReader&) throw (YsbotsException, std::bad_alloc );
	~RobotKinematics () throw ();
 
	bool change_robot_kinematics_type (const char*) throw ();

	bool change_robot_kinematics_type (const char*, const ConfigReader&) throw ();

	const char* get_robot_kinematics_type () const throw ();
  
	void* pFwdKineFuc;
	void* pInvKineFuc;

	void* pInitExtensionFuc;
	void* pPreProcessMotionCmdFuc;

	const Ysbot::DHParaBase& get_link_dh(const int) throw ();

	bool is_use_robot_lib() { return use_robot_lib; }
	void set_axis_num(int n);
	//for test kinematics algorithm of numeric.
 	void  define_start_pos( const AxisPos& );
	const double get_step_ratio() const throw ();

	int fwd_kine( const AxisPos& apos, CartPos* cpos );
	int inv_kine( const CartPos& cpos, AxisPos* apos );

	int init_robot_lib( bool, int ngroupidx );

	CCriticalSection  dataLock;
};

#define KINE  RobotKinematics::get_main_robot_kinematics()

#endif