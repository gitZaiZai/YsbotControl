// Copyright 2016, 亚思信息科技有限责任公司
// All right reserved.
//
// Author: Liu Run Dan ( lrdcz@163.com )
//
// Yasi Robot Motion Control System - 亚思机器人运动控制系统

#include "StdAfx.h"
#include "SimulationPlatform.h"
#include "stringconvert.h"
#include "Time.h"
#include "MotionControl.h"

SimulationPlatform*  SimulationPlatform::main_simulation_platform = NULL;

SimulationPlatform&  SimulationPlatform::get_main_simulation_platform () 
{ 
	return * SimulationPlatform::main_simulation_platform; 
}

bool  SimulationPlatform::is_main_simulation_platform_available () throw()
{
	return main_simulation_platform != NULL;
}

SimulationPlatform::SimulationPlatform(const ConfigReader& vread): ThreadTaskTypeBase("simulator"), send_data(true),
	is_connect(false),is_local_host(false),use_simulator(false),sim_platform_type(ROBOTSIM),robot_type("dr605")
{
	if( SimulationPlatform::main_simulation_platform == NULL ) 
		SimulationPlatform::main_simulation_platform = this; 

	for (int i=0; i < 6; ++i)
		joints[i] = 0;

	home_path = vread.getenv();

	vread.get( "SimulationPlatform::use_simulator",  use_simulator );
	if ( use_simulator )
	{
		vread.get("SimulationPlatform::communication_cycle_time",  task_cycle_time );
		std::string rokisim_address ="";
		int port = 30000;
		vread.get( "SimulationPlatform::port",  port );
		if ( vread.get("SimulationPlatform::rokisim_address",rokisim_address) > -1 )
		{
			is_connect = tcp_socket.init_as_client( rokisim_address.c_str(), port );
			if ( strcmp(rokisim_address.c_str(),"127.0.0.1") == 0 )
				is_local_host = true;
			else
				is_local_host = false;
		}
		else
		{
			is_connect = tcp_socket.init_as_client("127.0.0.1",4001);
			is_local_host = true;
		}

		std::string platname = "";
		vread.get( "SimulationPlatform::platform_type",  platname );
		vread.get( "SimulationPlatform::robot_type",  robot_type );
		if ( platname == "robotsim")
		{
			sim_platform_type = ROBOTSIM;
		}
		else
			sim_platform_type = ROBOTDK;
	}

}

SimulationPlatform::~SimulationPlatform(void)
{
	if( SimulationPlatform::main_simulation_platform == this ) 
		SimulationPlatform::main_simulation_platform = NULL; 
}

unsigned int SimulationPlatform::thread_task( ThreadTaskTypeBase* pParam)
{
	SimulationPlatform *pSimulator = ( SimulationPlatform* )pParam;

	Ysbot::Time current_time;
	const long int loop_time = pSimulator->task_cycle_time;
	const int nSimType = pSimulator->sim_platform_type;

	double sendjnt[6] = {0};

	unsigned int nSize[2];
	nSize[0] = 0x06000000;
	nSize[1] = 0x01000000;
	char sendBuf[100] = {0};

	if ( nSimType == ROBOTSIM )
		memcpy(sendBuf,nSize,2*sizeof(unsigned int));

	while( !exitFlag )
	{
		if ( !pSimulator->send_data )
		{
			Sleep(1);
			continue;
		}  

		if ( current_time.elapsed_msec() > loop_time )
		{
			Lock();
			for ( int i=0; i < 6; ++i)
			{
				if ( nSimType == ROBOTSIM )
					sendjnt[i] = pSimulator->Change(pSimulator->joints[i]) ;
				else
					sendjnt[i] = pSimulator->joints[i];
			}
			UnLock();

			if ( nSimType == ROBOTSIM )
			{
				memcpy(sendBuf+2*sizeof(unsigned int),sendjnt,6*sizeof(double));
				bool b = pSimulator->tcp_socket.sendto( sendBuf, 2*sizeof(unsigned int)+6*sizeof(double) );
			}
			else
			{
				sprintf(sendBuf,"axis = %d : %8.4f %8.4f %8.4f %8.4f %8.4f %8.4f",
					6, sendjnt[0],sendjnt[1],sendjnt[2],sendjnt[3],sendjnt[4],sendjnt[5] );
				bool b = pSimulator->tcp_socket.sendto( sendBuf, strlen(sendBuf) );

			}

			current_time.update();
		}
		Sleep(1); // important for another thread to read variable.
	}

	return 0;
}

void SimulationPlatform::set_joint_data( double* dfjnt)
{
	if ( use_simulator )
	{
		for ( int i = 0; i < 6; ++i )
		{
			joints[i] = dfjnt[i];
		}

		if ( SMC.isCodeSysMotion )
		{	
			joints[1] = -dfjnt[1];
			joints[2] = -dfjnt[2];
			joints[5] = valid_angle( dfjnt[5] + 180, -360,360 );
		}
		else
		{
			joints[2] -= 90;
		}
	}
}

void SimulationPlatform::toggle_send_data()
{
	send_data = !send_data;	
}

double SimulationPlatform::Change(double val) 
{
	DOU_CONV ac, ac1;
	ac.f = val;
	ac1.c[0] = ac.c[7];
	ac1.c[1] = ac.c[6];
	ac1.c[2] = ac.c[5];
	ac1.c[3] = ac.c[4];
	ac1.c[4] = ac.c[3];
	ac1.c[5] = ac.c[2];
	ac1.c[6] = ac.c[1];
	ac1.c[7] = ac.c[0];

	return ac1.f;
}

void SimulationPlatform::startup_simulator()
{
	if ( !use_simulator )
		return;

	if ( is_connect )
	{
		bool success = true;
		if ( is_local_host )
		{
			if ( sim_platform_type == ROBOTSIM )
			{
				success = run_rokisim();
				Sleep(10);
			}
			else
			{
				// run robotdk py file.
				int res;
				std::string strpath = home_path + "RoKiSim\\RobotDK";
				CString robot_python_file =  _T("startup_robotdk.py");
				if ( robot_type == "dr608" )
				{
					robot_python_file = _T("startup_robotdk_608.py");
				}

				ShellExecute( NULL,_T("open"),robot_python_file,_T(""), CString(Ansi2Unicode(strpath).c_str()), SW_SHOWNORMAL );
				res = GetLastError();
				if ( res != 0 && res < 32 )
				{
					switch(res)
					{
					case ERROR_FILE_NOT_FOUND: AfxMessageBox(_T("文件名出错！"));break;
					case ERROR_PATH_NOT_FOUND: AfxMessageBox(_T("文件路径出错！"));break;
					case ERROR_BAD_FORMAT: AfxMessageBox(_T("EXE文件无效！"));    break;
					case SE_ERR_NOASSOC:AfxMessageBox(_T("没有关联的EXE执行程序！")); break;
					default: break;
					}
					success = false;
				}
				success = true;
				Sleep(10); // 10
			}
		}
		if ( success )
		{
			start_task_thread();
		}
		else
			use_simulator = false;
	}
	else
		std::cerr << "simulator Tcp connect failed!\n";	


}

bool SimulationPlatform::run_rokisim()
{
	HWND wnd = ::FindWindow(NULL,_T("RoKiSim 1.7"));
	if ( NULL == wnd )
	{
		int res;
		std::string strpath = home_path + "RoKiSim\\bin";
		ShellExecute( NULL,_T("open"),_T("RoKiSim.exe"),_T("MOTOMAN_UP50N.xml"), CString(Ansi2Unicode(strpath).c_str()), SW_SHOWNORMAL );
		res = GetLastError();
		if ( res < 32 )
		{
			switch(res)
			{
			case ERROR_FILE_NOT_FOUND: AfxMessageBox(_T("文件名出错！"));break;
			case ERROR_PATH_NOT_FOUND: AfxMessageBox(_T("文件路径出错！"));break;
			case ERROR_BAD_FORMAT: AfxMessageBox(_T("EXE文件无效！"));    break;
			case SE_ERR_NOASSOC:AfxMessageBox(_T("没有关联的EXE执行程序！")); break;
			default: break;
			}
			return false;
		}
		return true;
	}
	else
	{
		std::cout << "RoKiSim is running!\n";
	}
	return true;
}

void SimulationPlatform::close_rokisim()
{
	HWND wnd = ::FindWindow(NULL,_T("RoKiSim 1.7"));
	if ( wnd )
	{
		::PostMessage(wnd,WM_QUIT,NULL,NULL);
	}
}

void SimulationPlatform::close_simulator()
{
	if ( !use_simulator )
		return;

	send_data = false;
	if ( is_local_host )
	{
		if ( sim_platform_type == ROBOTSIM )
		{
			close_rokisim();
		}
		else
		{
			// close other platform.
			tcp_socket.sendto( "quit", 4 );
		}
	}	

}
