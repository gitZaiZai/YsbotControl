// Copyright 2016, 亚思信息科技有限责任公司
// All right reserved.
//
// Author: Liu Run Dan ( lrdcz@163.com )
//
// Yasi Robot Motion Control System - 亚思机器人运动控制系统

#include "StdAfx.h"
#include "Servo.h"
#include "ServoFactory.h"
#include "Journal.h"
#include <cstring>
#include "PosInfo.h"
#include "Time.h"
#include "WorldModel.h"
#include "UIDialogs/state_dialog.h"
#include "stringconvert.h"

using namespace std;

Servo* Servo::main_servo=NULL;

Servo& Servo::get_main_servo () throw () { return *Servo::main_servo; }

bool Servo::is_main_servo_available () throw()
{
	return main_servo != NULL;
}

const char* Servo::get_servo_type () const throw () { return servo_descriptor; }

Servo::~Servo () throw () 
{
	if( Servo::main_servo == this )
		Servo::main_servo = NULL;  
	
	map<string, ServoType*>::const_iterator it;
 	 
	for( it = list_of_servo.begin();  it != list_of_servo.end(); it++  )
	{
		if ( (it)->second != NULL )
		{
			delete (it)->second;
 		}
	}
	list_of_servo.clear();
    active_servo.clear();

 	delete the_servo;

	if ( servo_descriptor )
	{
		delete [] servo_descriptor;
 	}

}

Servo::Servo (const ConfigReader& vread) throw ( YsbotsException, std::bad_alloc ) : ThreadTaskTypeBase("Servo driver"), the_servo(NULL), configuration_list(vread),
	active_servo(12),open_port_flag(false),servo_num(0),knowAbsPos(false),bNewServoPos(false),isConnectServo(true)
{
	if( Servo::main_servo == NULL  )
		Servo::main_servo=this;  
	 
	memset(errormsg,'\0',MAX_ERRSTR_LEN);

	task_cycle_time = 150;
	vread.get("Servo::task_cycle_time",       task_cycle_time );
	vread.get("Servo::connect_motor_driver",  isConnectServo  );

	for ( int i = 0; i < MAX_JOINT_NUM; ++i )
	{
 		servo_pos[i] = 0;
		is_simulted[i] = false;
	}
 
	list_of_servo.clear();
	active_servo.clear();
	string lastType = "";
	string confline;
	the_servo = NULL;
	servo_descriptor = NULL;
 
	ServoType* new_servo = NULL;

	if ( isConnectServo )
	{
		if ( MWM.is_main_world_model_available() )
		{
			const GroupProperties& group_properties = MWM.get_group_properties();

			servo_num = group_properties.axis_num;

			for ( int i = 0; i < servo_num; i++ ) 
			{
				confline = group_properties.servoName[i];

				if ( !confline.empty() )
				{
					map<std::string, ServoType*>::iterator mit = list_of_servo.find (confline);
					if ( mit != list_of_servo.end() )
						new_servo = mit->second;
					else 
					{
						try
						{
							new_servo = ServoFactory::get_servo_factory()->get_servo (confline, vread);
						}catch(invalid_argument){
							throw InvalidConfigurationException ("Servo_type");
						}

						list_of_servo.insert ( pair<string,ServoType*>( confline, new_servo) );
					}

					if ( strcmp("Dummy",confline.c_str()) == 0 )
						is_simulted[i] = true;
				}
				else
				{
					new_servo = NULL;
					JERROR("no config line \"Servo_type\" found");
					is_simulted[i] = true;
					throw InvalidConfigurationException ("Servo_type");
				}
				active_servo.push_back( new_servo );
			}
		}

		try
		{
			init_port();
		}
		catch (YsbotsException& e)
		{
			throw (e);
		}

		start_task_thread();
	}
 
}

bool Servo::change_servo_type (const char* pt) throw () {
	return change_servo_type (pt, configuration_list);
}

bool Servo::change_servo_type (const char* pt, const ConfigReader& vread) throw () {
	try{
		really_change_servo_type (pt, vread);
	}catch(bad_alloc&)
	{
		JWARNING(("Change of Servo type failed due to lack of memory"));
		return false;
	} 
	return true;
}

void Servo::really_change_servo_type (const char* pt, const ConfigReader& vread) throw ( YsbotsException,std::bad_alloc) {
	ServoType* new_servo;
	char* new_descriptor;
	try
	{
		string plt (pt);
		new_servo = ServoFactory::get_servo_factory()->get_servo (plt, vread);
	}catch(invalid_argument){
		throw InvalidConfigurationException ("Servo_type_type");
 	}

	new_descriptor = new char [strlen(pt)+1];
	strcpy(new_descriptor,pt);
	if (the_servo!=NULL) {
		delete the_servo;
		delete [] servo_descriptor;
	}

	the_servo=new_servo;
	servo_descriptor=new_descriptor;

	init_port();
}

void Servo::getServoTypeList(std::vector<std::string> &ptl)
{
	ServoFactory::get_servo_factory()->servo_list(ptl);
}

int  Servo::read_pos( unsigned int iDeviceID, long* pData )
{
	if( Servo::main_servo == NULL  )
		return -1;

	if ( active_servo.empty())
	{
		return -11;
	}
	if ( iDeviceID >= active_servo.size() || active_servo.size() == 0 || active_servo[iDeviceID] == NULL  )
	{
		return -10;
 	}
	Lock();
	int ret = active_servo[iDeviceID]->read_pos( iDeviceID+1, pData );
	UnLock();
	return ret;
}

int  Servo::clear_pos( unsigned int iDeviceID ) 
{
	if ( iDeviceID >= active_servo.size() || active_servo.size() == 0 || active_servo[iDeviceID] == NULL )
	{
		return -10;
	}
	Lock();
	int ret = active_servo[iDeviceID]->clear_pos( iDeviceID+1 );
	UnLock();
	return ret;
}

int   Servo::clear_alarm( unsigned int iDeviceID )
{
	if ( iDeviceID >= active_servo.size() || active_servo.size() == 0 || active_servo[iDeviceID] == NULL )
	{
		return -10;
	}
	Lock();
	int ret = active_servo[iDeviceID]->clear_alarm( iDeviceID+1 );
	UnLock();
	return ret;
}

bool Servo::init_port() throw (YsbotsException, bad_alloc )
{
	bool bret = true;
	map<string, ServoType*>::const_iterator it;

	string ename;
	 
	for( it = list_of_servo.begin();  it != list_of_servo.end(); it++  )
	{
		bool res = (*it).second->init_port();
		if ( !res )
		{
			ename = (*it).first;
			ename = ename + " port open failed!";
 			bret = false;
			throw HardwareException( ename.c_str() );
 		}
	}
   
 	open_port_flag = bret;

	return bret;
}

bool  Servo::close()
{
	bool bret = false;
	map<string, ServoType*>::const_iterator it;

	for( it = list_of_servo.begin();  it != list_of_servo.end(); it++  )
	{
		bret = (*it).second->close();
		if ( !bret )
		{
			return false;
		}
	}
	open_port_flag = false;

	return true;
}

// in rad
int Servo::read_pos_from_servo( double* pos )
{
	if ( !open_port_flag )
	{
 		if ( init_port() == false )
		{
			strcpy( errormsg, " com not open!" );
			return -10;
		}
	}

 	bool result[MAX_JOINT_NUM] = {false};
	int nret      = 0;
	int failcount = 0;
	const int maxTimes = 100;
 	long puls = 0;

	unsigned int resolution    = 131072;
	long     int encoderOrigin = 0;
	double       gear          = 100.0;
	double  mechanicalOrigin   = 0.0;
	int     encdir             = 1;
	double  hightLimit         = 360.0;

	const GroupProperties& gp = MWM.get_group_properties();

	char msgservo[255];
	std::string msgout = "";

	for ( unsigned int nDevice =0 ; nDevice < gp.axis_num; ++nDevice )
	{
 		resolution    = gp.mp[nDevice].encoder_resolution;
		gear          = gp.mp[nDevice].gear_ratio;
		encdir        = gp.mp[nDevice].encoder_dir ? -1:1;
		hightLimit    = gp.mp[nDevice].high_limit;
		encoderOrigin = gp.mp[nDevice].encoder_origin;
		mechanicalOrigin = gp.mp[nDevice].mechanical_origin;
 		do 
		{
 			if( failcount++ > maxTimes )
			{
				break;
			}
			 
			nret = read_pos( nDevice, &puls );
			 
			Sleep(1);

		} while ( nret != 0 );

		if ( nret == 0 )
		{
 			if ( is_simulted[nDevice] == false )
			{
				long total_pls = puls - encoderOrigin;
				double total_ang = static_cast<double>(total_pls) * ( 360.0 / static_cast<double>(resolution) );
				double arm_ang = total_ang / gear;

				double real_ang = fmod( arm_ang, 360 );
				bool twoturns = fabs(hightLimit) > 180 ? true : false;
				if( twoturns )
				{
					real_ang = fmod( encdir *  real_ang + mechanicalOrigin, 360 );
					*pos = deg2rad( real_ang );
				}
				else
				{
					double real_arm_ang = encdir * real_ang + mechanicalOrigin;
					Angle ang;
					ang.set_deg( real_arm_ang );
					*pos = deg2rad( ang.get_deg_180() );
				}
			}
			else
			{
				double real_arm_ang = encdir * static_cast<double>(puls) * 0.01 + mechanicalOrigin;
				Angle ang;
				ang.set_deg( real_arm_ang );
				*pos = deg2rad( ang.get_deg_180() );
 			}

			result[nDevice] = true;
		}
		else
		{
			nret = -1 * ( 10+nDevice ); // error code
			result[nDevice] = false;
			sprintf( msgservo," servo_%d",nDevice+1);
			msgout = msgout + std::string(msgservo);
		}
 		pos++;
	}

	if ( !msgout.empty() )
	{
		msgout = " read error : " + msgout;
        strcpy( errormsg, msgout.c_str() );
		return -20;
	}

	return nret;
}

int Servo::clear_all_servo_pos()
{
	int ret = 0;
	for ( int i=0; i < servo_num; i++)
	{
 		int single_ret = clear_pos(i);
 		if (single_ret != 0 )
		{
			ret = single_ret;
		}
	}
	return ret;
}

unsigned int Servo::thread_task( ThreadTaskTypeBase* pParam)
{
	Servo *pServo = ( Servo* )pParam;

	Ysbot::Time current_time;
	
	const long int loop_time = pServo->task_cycle_time;
	double pos[MAX_JOINT_NUM] = {0};
	int nret = -1;

	while( !pServo->exitFlag )
	{
 		if ( current_time.elapsed_msec() > loop_time )
		{
			memset(pos,0,sizeof(double)*MAX_JOINT_NUM);

// 			Lock(); // liurundan 3.29
  			nret = pServo->read_pos_from_servo(pos); 
// 			UnLock();

			if ( nret == 0 )
			{
				memcpy( pServo->servo_pos,pos, sizeof(double)*MAX_JOINT_NUM );
				knowAbsPos = true;
				pServo->bNewInfo = true;
 			}
			else
			{
				cerr << " servo read failed!\n";
				pServo->bNewInfo = false;
			}
   
			current_time.update();
		}
		Sleep(1); // important for another thread to read variable.
	}

	return 0;
}

void Servo::process_servo_message()
{
	if ( MWM.is_main_world_model_available() )
	{
		PosInfo fpos;
		if ( isConnectServo )
		{
			fpos.valid = bNewInfo;
			fpos.axis.set_rad(servo_pos[0],servo_pos[1],servo_pos[2],servo_pos[3],servo_pos[4],servo_pos[5]);
			fpos.auxi.set_rad(servo_pos[6],servo_pos[7],0,0,0,0);
		}
		else
		{
			fpos.valid = true;
			fpos.axis.set_deg(0,0,90,0,0,0);
			fpos.auxi.set_rad(0,0,0, 0,0,0);
		}
 	
		MWM.set_servo_position(fpos);		

		if ( errormsg[0] != '\0' )
		{
 			MSD.ShowInfo( Ansi2Unicode( errormsg ).c_str(), JTERROR, 3920, true );
		}
 	}
}