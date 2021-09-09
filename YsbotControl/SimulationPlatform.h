// Copyright 2016, 亚思信息科技有限责任公司
// All right reserved.
//
// Author: Liu Run Dan ( lrdcz@163.com )
//
// Yasi Robot Motion Control System - 亚思机器人运动控制系统

#ifndef _SimulationPlatform_h
#define _SimulationPlatform_h

#include "ConfigReader.h"
#include "TcpSocket.h"
#include "ThreadTaskTypeBase.h"

class SimulationPlatform : public ThreadTaskTypeBase
{
public:
	SimulationPlatform(const ConfigReader&);
	~SimulationPlatform(void);
	static SimulationPlatform& get_main_simulation_platform ();
	static bool is_main_simulation_platform_available ();

	void startup_simulator();
	void close_simulator();

	void toggle_send_data();
 	void set_joint_data( double* );

private:
	bool is_connect;
	bool is_local_host;
	bool use_simulator;
	typedef union 
	{
		double f;
		char c[8];
	} DOU_CONV;

	std::string home_path; 
	std::string robot_type;

	static SimulationPlatform*	 main_simulation_platform;

	TcpSocket tcp_socket;
	 
	bool             send_data;
 	double           joints[6];

	enum { ROBOTSIM = 0, ROBOTDK = 1 };

	unsigned int thread_task(ThreadTaskTypeBase*);
	double Change(double val);
 
	bool run_rokisim();
	void close_rokisim();

	int sim_platform_type;
};

#endif
