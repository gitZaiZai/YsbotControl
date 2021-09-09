// Copyright 2016, 亚思信息科技有限责任公司
// All right reserved.
//
// Author: Liu Run Dan ( lrdcz@163.com )
//
// Yasi Robot Motion Control System - 亚思机器人运动控制系统

#include "StdAfx.h"
#include "MotionControl.h"
#include "MotionControlFactory.h"
#include "Journal.h"
#include <cstring>
#include "WorldModel.h"
#include "YsbotControlDlg.h"
#include "stringconvert.h"

using namespace std;

MotionControl* MotionControl::main_motion_control=NULL;

MotionControl& MotionControl::get_main_motion_control () throw () { return *MotionControl::main_motion_control; }

bool MotionControl::is_main_motion_control_available () throw()
{
	return main_motion_control != NULL;
}

const char* MotionControl::get_motion_control_type () const throw () { return motion_control_descriptor; }

MotionControl::~MotionControl () throw () 
{
	if( MotionControl::main_motion_control == this )
		MotionControl::main_motion_control = NULL;  
  
	stop_task_thread();

	close_system();

	trigger_list.clear();

	delete [] remote_input;
	delete [] local_input;

  	delete the_motion_control;
	if ( motion_control_descriptor )
	{
		delete [] motion_control_descriptor;
		motion_control_descriptor = NULL;
	}
}

MotionControl::MotionControl (const ConfigReader& vread) throw ( YsbotsException, std::bad_alloc ) : ThreadTaskTypeBase("Motion Control"), 
	the_motion_control(NULL), configuration_list(vread),isInitSystem(false),
	second_page(false),isCodeSysMotion(false),max_manual_speed(10),
	remote_input(NULL),rio_word_num(4),local_word_num(4),trigger_list(1000),
	key_switch(0),jog_home_step(JOG_NOTHOME),is_pc_pendant(false) 
{
	if( MotionControl::main_motion_control == NULL  )
		MotionControl::main_motion_control=this;  

 	the_motion_control = NULL;
	motion_control_descriptor = NULL;
	trigger_list.clear();
// 	string confline = "MCCL";
// 	really_change_motion_control_type (confline.c_str(), vread);

	string confline;
	if (vread.get("motion_control_type", confline)<=0) {
		JERROR("no config line \"motion_control_type\" found");
		throw InvalidConfigurationException ("motion_control_type");
	}
	try{
		really_change_motion_control_type (confline.c_str(), vread);
	}catch(YsbotsException& e){
		JERROR((std::string ("creating motion control of type ")+confline+std::string(" failed due to: ")+std::string(e.what())).c_str());
		throw (e);
	}

	const IoModuleParameters& imp = the_motion_control->get_mcc_io_parameters();
	if( WorldModel::is_main_world_model_available() ) 
	{  
 		WorldModel::get_main_world_model().set_io_module_parameters( the_motion_control->get_mcc_io_parameters(), IOSmotioncard );
	    max_manual_speed = MWM.get_group_properties().profile.dfMaxExternalVel;
	}
	else 
	{
		JWARNING("no worldmodel available");
	}

 	use_mcc_rio    = imp.is_connected;
	task_cycle_time = imp.scan_cycle_time;
	rio_word_num   = imp.x_word_size;
	local_word_num = imp.local_input_size;

    remote_input = new unsigned short[rio_word_num];
	local_input  = new unsigned short[local_word_num];
	for ( int i = 0; i < rio_word_num; i++ )
 		remote_input[i] = 0;
 
	for ( int i = 0; i < local_word_num; i++ )
 		local_input[i] = 0;
 
	gpio_input_value = 0;

	if ( WorldModel::is_main_world_model_available() && isCodeSysMotion == false )
	{
		InOutObject ionew;
		Ysbot::Time t;
		ionew.object_type = InOutObject::estop; // one byte
		ionew.index = 0;
		ionew.state = 0;
		MWM.set_io_information( ionew, t );

		ionew.object_type = InOutObject::ot_positive; // bit for one input port
		ionew.index = 0;
		ionew.state = 0;
		MWM.set_io_information( ionew, t );

		ionew.object_type = InOutObject::ot_negative;
		ionew.index = 0;
		ionew.state = 0;
		MWM.set_io_information( ionew, t );

		ionew.object_type = InOutObject::home_sensor;
		ionew.index = 0;
		ionew.state = 0;
		MWM.set_io_information( ionew, t );

		ionew.object_type = InOutObject::gpio_input;
		ionew.index = 0;
		ionew.state = 0;
		MWM.set_io_information( ionew, t );
	}

	teach_info.haveReached = true;
	teach_info.first_forward = true;
	teach_info.status = TEACH_END;
 }

bool MotionControl::change_motion_control_type (const char* pt) throw () {
	return change_motion_control_type (pt, configuration_list);
}

bool MotionControl::change_motion_control_type (const char* pt, const ConfigReader& vread) throw () {
	try{
		really_change_motion_control_type (pt, vread);
	}catch(bad_alloc&)
	{
		JWARNING(("Change of MotionControl type failed due to lack of memory"));
		return false;
	} 
	return true;
}

void MotionControl::really_change_motion_control_type (const char* pt, const ConfigReader& vread) throw ( YsbotsException, std::bad_alloc ) {
	MotionControlType* new_motion_control;
	char* new_descriptor;
	try
	{
		string plt (pt);
		new_motion_control = MotionControlFactory::get_motion_control_factory()->get_motion_control (plt, vread);
	}catch(invalid_argument){
		throw InvalidConfigurationException ("motion_control_type");
  	}

	new_descriptor = new char [strlen(pt)+1];
	strcpy(new_descriptor,pt);
	if (the_motion_control!=NULL) {
		delete the_motion_control;
		delete [] motion_control_descriptor;
	}

	the_motion_control=new_motion_control;
	motion_control_descriptor=new_descriptor;
	if ( strcmp( motion_control_descriptor, "CodeSys") == 0 )
		isCodeSysMotion = true; 
}

void MotionControl::getMotionControlTypeList(std::vector<std::string> &ptl)
{
	MotionControlFactory::get_motion_control_factory()->motion_control_list(ptl);
}

//////////////////////////////////////////////////////////////////////////
unsigned int MotionControl::thread_task( ThreadTaskTypeBase* pParam )
{
	MotionControl *pMotionCard = ( MotionControl* )pParam;
 
	Ysbot::Time current_time;
	Ysbot::Time read_time;
	const long int loop_time = pMotionCard->task_cycle_time;

	unsigned short lio[6] = {0};
	unsigned short rio[8] = {0};
	bool use_codesys_softmotion = isCodeSysMotion;
	bool use_pc_keyboard_pendant = pMotionCard->is_pc_pendant;

	while( !exitFlag )
	{
// 		if ( current_time.elapsed_msec() > loop_time )
		{
// 			for ( int i = 0; i < pMotionCard->local_word_num; i++ )
//  				pMotionCard->local_input[i] = 0;
// 			for ( int i = 0; i < pMotionCard->rio_word_num; i++ )
// 				pMotionCard->remote_input[i] = 0;


			Lock();

 			pMotionCard->mc_info_thread = pMotionCard->get_motion_core_information(); //  状态更新

			if ( use_codesys_softmotion )
			{
				pMotionCard->read_input( pMotionCard->remote_input, MCS_REMOTEIO );
			}
			else
			{
				if ( KINE.is_main_robot_kinematics_available() )
				{
					pMotionCard->process_trigger(pMotionCard->mc_info_thread.nCommandIndex); // ??????????????????????????????
				}

				pMotionCard->read_input( pMotionCard->local_input, MCS_LOCALIO );
				pMotionCard->read_input( pMotionCard->remote_input, MCS_REMOTEIO );
				pMotionCard->read_input( &(pMotionCard->gpio_input_value), MCS_GPIO);
			}	

			UnLock();

			current_time.update();
		}
		Sleep(1); // important for another thread to read variable.
	}
 
	return 0;
}

bool MotionControl::process_motion_information( Ysbot::Time t ) throw()
{
 	InOutObject ionew;
	///////////////// Local input /////////////////////////////////////////
	if ( isCodeSysMotion == false )
	{
		ionew.object_type = InOutObject::estop; // one byte
		ionew.index = 0;
		ionew.state = local_input[0];
		MWM.set_io_information( ionew, t );

		ionew.object_type = InOutObject::ot_positive; // bit for one input port
		ionew.index = 0;
		ionew.state = local_input[1];
		MWM.set_io_information( ionew, t );

		ionew.object_type = InOutObject::ot_negative;
		ionew.index = 0;
		ionew.state = local_input[2];
		MWM.set_io_information( ionew, t );

		ionew.object_type = InOutObject::home_sensor;
		ionew.index = 0;
		ionew.state = local_input[3];
		MWM.set_io_information( ionew, t );

		ionew.object_type = InOutObject::gpio_input;
		ionew.index = 0;
		ionew.state = gpio_input_value;
		MWM.set_io_information( ionew, t );
	}

	////////////////// Remote input ////////////////////////////////////////
	for ( int i = 0; i < rio_word_num; i++ )
	{
		ionew.object_type = InOutObject::ext_input;
		ionew.index = i;
		ionew.state = remote_input[i];

		MWM.set_io_information( ionew, t );
	}

	// 	if ( strcmp( motion_control_descriptor, "CodeSys") == 0 )
	// 	{ 
	// 		mc_info_thread = get_motion_core_information();
	// 	}  

	MWM.set_motion_core_info(mc_info_thread);

	if ( isInitSystem  && ( MWM.get_robot_state().pos_known == false ) )
	{	
		if ( isCodeSysMotion )
		{ 
			MWM.set_startpos_known(true);
		}
		else
		{
			const PosInfo& spos = MWM.get_servo_position();
			if ( spos.valid )
			{
				CmdInfo cmd;
				cmd.iCommandType   = CMD_SETPOSITION;
				cmd.customStartPos = true;
				cmd.startPos   = spos;
				SMC.setCmdInfo(cmd);
				MWM.set_startpos_known(true);

				printf("start position of motion system has be defined successfully!\n");
			}
		}
	}

	// 	const char * err = get_error();
	if ( mc_info_thread.errString[0] != '\0' )
	{
 		MSD.ShowInfo( Ansi2Unicode( mc_info_thread.errString ).c_str(), JTERROR, 3900, true );

		if ( mc_info_thread.iMotionStatus == MotionCoreInfo::msRUNNING )
		{
			SMC.setCmdInfo(CMD_ABORT);
		}
		if ( mc_info_thread.emergency_stop == false ) // ??????????
		{
			if ( SMC.isCodeSysMotion )
			{
				;
			}
			else
			{
				setCmdInfo( CMD_RESET );
			}
			mc_info_thread.errString[0] = '\0';
		}
		program_dialog->on_free_script();
	}

	std::string alarmmsg;
	char tmp[100];
	bool iserr = false;

	unsigned long alarm = mc_info_thread.driver_alarm;
	ionew.object_type = InOutObject::alarm_input;
	ionew.index = 0;
	ionew.state = static_cast<unsigned short>(alarm);
	MWM.set_io_information( ionew, t ); 

	if ( isCodeSysMotion == false )
	{ 
		if ( isInitSystem && Servo::is_main_servo_available() &&
			Servo::get_main_servo().is_connect_servo() )
		{
			for ( int i = 0; i < 6; i++ )
			{
				if ( GETBIT(alarm,i) == 1 )
				{
					sprintf(tmp," Err%d",i+1);
					alarmmsg += std::string(tmp);
					iserr = true;
				}
			}
			if ( iserr )
			{
				// for test
				program_dialog->on_free_script();
				SMC.setCmdInfo(CMD_ABORT);
 				MSD.ShowInfo( Ansi2Unicode( alarmmsg ).c_str(), JTERROR, 3910, true );
			}
			// 		printf("servo error : %0x\n",alarm);
		}
	}

	return true;
}

int MotionControl::mc_din( unsigned short nb, unsigned short* val,  unsigned short itype )
{
	int ret = 0;
	unsigned short valtmp = 0;
	if ( itype == MCS_GPIO ) // 16bit
	{
		ret = read_word( 0, &valtmp, MCS_GPIO );
		if ( ret == 0 && nb < 16 && nb >= 0 )
			*val = GETBIT( valtmp, nb );
	}
	else if ( itype == MCS_REMOTEIO )
	{
		int word_NO  = static_cast<int>( floor( static_cast<double>(nb) / 16.0 ) ); 
		int bit_NO   = nb % 16;
		ret = read_word( word_NO, &valtmp, MCS_REMOTEIO );

		if (ret == 0)
		{
			*val = GETBIT(valtmp,bit_NO);
		}
	}
	else if ( itype == MCS_LOCALIO )
	{
		ret = read_word( nb, &valtmp, MCS_LOCALIO );
		if ( ret == 0 )
			*val = valtmp;
	}

	return ret;
}

int MotionControl::mc_dout( unsigned short nb, unsigned short val, unsigned short itype, bool inqueue, double dfDelayTime )
{
	int ret = 0;
	unsigned short valTemp = 0;
 	const InOutObjectList& old = MWM.get_io_objects(IOSmotioncard);
	InOutObject ionew;

	if ( itype == MCS_GPIO )
	{
 		ionew.object_type = InOutObject::gpio_output;
		ionew.index       = 0;
		valTemp = old.get_state( ionew.object_type, ionew.index );

		if ( val == 1 )
			SETBIT( valTemp, nb );
		else
			CLRBIT( valTemp, nb );
 
		ret = write_word(nb,valTemp,MCS_GPIO,inqueue,dfDelayTime);
 	}
	else if ( itype == MCS_REMOTEIO ) // codesys ethercat io.
	{
		int word_NO  = static_cast<int>( floor( static_cast<double>(nb) / 16.0 ) ); 
		int bit_NO   = nb % 16;

		ionew.object_type = InOutObject::ext_output;
		ionew.index       = word_NO;

		valTemp = old.get_state( ionew.object_type, ionew.index );

		unsigned short cur_bit_val = GETBIT( valTemp, bit_NO );
		if ( cur_bit_val == val )
			return 0;

		if ( val == 1 )
			SETBIT( valTemp, bit_NO );
		else
			CLRBIT( valTemp, bit_NO );

		ret = write_word( word_NO, valTemp, MCS_REMOTEIO,inqueue,dfDelayTime );

 	}
	else if ( itype == MCS_LOCALIO )
	{
		int nBit = nb - 10;
		if ( nb >= 0 )
		{
			ionew.object_type = InOutObject::pos_ready;
			ionew.index       = 0;
			valTemp = old.get_state( ionew.object_type, ionew.index );
			if ( valTemp != val )
			{
				valTemp = val;
				write_word(nb,valTemp,MCS_LOCALIO);
			}
		}
		else
		{
			ionew.object_type = InOutObject::servo_enable;
			ionew.index       = 0;
			valTemp = old.get_state( ionew.object_type, ionew.index );
			
			if ( GETBIT(valTemp,nBit) != val )
			{
				SETBIT(valTemp,nBit);
				write_word(nb,valTemp,MCS_LOCALIO);
			}
 		}
	}

	if ( ret >= 0 )
	{
		Ysbot::Time t;
		ionew.state = valTemp;
		MWM.set_io_information(ionew,t);
	}

	return ret;
}

int MotionControl::init_system()
{ 
	int ret = the_motion_control->init_system(); 
	if ( ret == 0 )
	{
		isInitSystem = true;  
		start_task_thread();
	}
	return ret;
}

int MotionControl::close_system()
{  
	int ret = the_motion_control->close_system(); 
	isInitSystem = false; 
	return ret;
}

int MotionControl::setCmdInfo( const CmdInfo& gi )
{
	if (MWM.is_main_world_model_available())
	{
		MWM.update_robot_state(gi);
 	}
	if ( gi.iCommandType == CMD_RESET )
	{
		mc_info_thread.errString[0] = '\0';
		teach_info.status = TEACH_END;
	}
	return the_motion_control->setCmdInfo(gi); 
}

int MotionControl::setCmdInfo( CommandT cmdtype )
{
	command_info.iCommandType = cmdtype;
	command_info.dyn.JointDec = 1.0;
	command_info.dyn.PathDec  = 1e4;
	MWM.update_robot_state(command_info);

	return setCmdInfo(command_info);
}

void MotionControl::add_trigger(const Trigger& tr)
{
  	Lock();
    trigger_list.push_back(tr);
  	UnLock();
}

void MotionControl::clear_trigger()
{
  	Lock();
	trigger_list.clear();
  	UnLock();
}

const PosInfo& MotionControl::get_position( const PosInfo::CoordinateType pct/* = PosInfo::CTaxis */, const LocationDataSource lds/* = LDScommad */ )
{
// 	const PosInfo& pi = the_motion_control->get_position( pct, lds ); // ????????????????????
	Lock(); // conflict with process trigger.
  	currpos = the_motion_control->get_position( pct, lds );
	UnLock();
	return currpos;
}

void MotionControl::process_trigger(unsigned long long current_index)
{
	std::vector<Trigger>::iterator it    = trigger_list.begin();
	std::vector<Trigger>::iterator itend = trigger_list.end();

	CartPos cpos;
	PosInfo rpos;
	 
	rpos  = get_position( PosInfo::CTmachine, LDScommad ); // ???????
	 
	double curr_dist = 0;
	double dist = 0;
	double percent = 0;
	double path_length = 0;
	bool   val  = false;
 
	while( it < itend )
	{
		if ( current_index == (*it).command_index )
		{
			val  = ( (*it).dout.output_value == 1 ? true : false );
			dist = (*it).distance;
			if ( (*it).trigger_type == onDISTANCE )
			{
				if ( (*it).distance_type == FROMBEGIN )
				{
					cpos = rpos.cart - ((*it).path_start.cart);
					curr_dist = cpos.pos.length();
					if ( curr_dist > dist )
					{
 						(*it).dout.set_on_trigger( val );
 						it    = trigger_list.erase(it);
						itend = trigger_list.end();
					}
					else
						it++;
					cout << " trigger curr_dist S = " << curr_dist << '\n';
				}
				else
				{
					cpos = rpos.cart - ((*it).path_end.cart);
					curr_dist = cpos.pos.length();
					if ( curr_dist < dist )
					{
 						(*it).dout.set_on_trigger( val );
					 
						it = trigger_list.erase(it);
						itend = trigger_list.end();
					}
					else
						it++;
					cout << " trigger curr_dist E = " << curr_dist << '\n';
				}	
			}
			else if ( (*it).trigger_type == onPOSITION )
			{
				percent = (*it).path_percent;
				cpos = ((*it).path_end.cart) - ((*it).path_start.cart);
				CartPos posvec = rpos.cart - ((*it).path_start.cart);
				curr_dist   = posvec.pos.length();
				path_length = cpos.pos.length();
				if ( path_length==0)
 					continue;

 				if ( (curr_dist/path_length) >= (percent*0.01) )
				{
 					(*it).dout.set_on_trigger( val );
  					it = trigger_list.erase(it);
  					itend = trigger_list.end();
				}
				else
					it++;

				cout << " trigger percent = " << curr_dist/path_length * 100 << '\n';
			}
			else
 				it++;
 		}
		else
			it++;
	}
}

bool MotionControl::response_key_press()
{
	if ( MWM.is_main_world_model_available() == false )
		return false;

	const KeyObjectList& kol = MWM.get_key_list();
	// 	kol.show(std::cout); // for test

	if ( GETBIT( kol.keylist[0], 0 ) ) // stop = 0
	{
		if ( program_dialog->GetSafeHwnd() )
		{
			program_dialog->on_free_script(true);
			// 				if ( isCodeSysMotion )
			// 				{
			// 					SMC.setCmdInfo(CMD_ABORT); 
			// 					//SMC.setCmdInfo(CMD_ABORTJOG); 
			// 				}
			// 				else
			// 				{
			// 					SMC.setCmdInfo(CMD_ABORT); 
			// 				}
		}
 
		cout << " stop com key! " << '\n';
		return true;
	}

	bool enable_press = false;
	unsigned char databyte = kol.keylist[4] & 0X0C;
	if ( databyte == 4 )
		enable_press = true;
	else
		enable_press = false;
	MWM.set_enable(enable_press);
	// 0 remote; 1 play; 2 teach
	key_switch = kol.keylist[4] & 0X03;

	unsigned char axis = 0;
	unsigned char dir  = 0;
	unsigned char databyte_first  = kol.keylist[0];
	unsigned char databyte_second = kol.keylist[1];

	unsigned char datashift = kol.keylist[1] & 0X40;
	static int step = 0;
	if ( datashift != 0 )
	{
		step = 1;
	}
	else
	{
		if ( step == 1 )
		{
			second_page = !second_page;
			step = 0;
		}
	}
	MWM.set_in_second(second_page);

	if ( GETBIT(databyte_first,2) ^ GETBIT(databyte_first,3) )
	{
		int bpos = 0;
		if ( second_page )
			bpos = 6;
		SETBIT(axis,bpos);
		if (GETBIT(databyte_first,2))
			SETBIT(dir,bpos);
	}
	if ( GETBIT(databyte_first,4) ^ GETBIT(databyte_first,5) )
	{
		int bpos = 1;
		if ( second_page )
			bpos = 7;

		SETBIT(axis,bpos);
		if (GETBIT(databyte_first,4))
			SETBIT(dir,bpos);
	}
	if ( GETBIT(databyte_first,6) ^ GETBIT(databyte_first,7) && !second_page )
	{
		SETBIT(axis,2);
		if (GETBIT(databyte_first,6))
			SETBIT(dir,2);
	}

	if ( GETBIT(databyte_second,0) ^ GETBIT(databyte_second,1)  && !second_page )
	{
		SETBIT(axis,3);
		if (GETBIT(databyte_second,0))
			SETBIT(dir,3);
	}
	if ( GETBIT(databyte_second,2) ^ GETBIT(databyte_second,3)  && !second_page )
	{
		SETBIT(axis,4);
		if (GETBIT(databyte_second,2))
			SETBIT(dir,4);
	}
	if ( GETBIT(databyte_second,4) ^ GETBIT(databyte_second,5)  && !second_page )
	{
		SETBIT(axis,5);
		if (GETBIT(databyte_second,4))
			SETBIT(dir,5);
	}

	// 	printf("jog home step = %d\n",jog_home_step);

	if ( key_switch == 2 ) // omTEACH
	{
		if ( axis != 0 ) // key pressed.
		{
			if ( MWM.get_robot_state().servo_on == false )
			{
				if ( isCodeSysMotion )
				{
					SMC.setCmdInfo(CMD_ABORTJOG); 
				}
				else
					SMC.setCmdInfo(CMD_ABORT); 

 				MSD.ShowInfo(_T("示教，设备未打开伺服使能！"),JTWARNING,0,true);

				teach_info.status = TEACH_END;
				return true;
			}

			if ( !enable_press )
			{
 				MSD.ShowInfo(_T(" 示教，请按下使能开关！"),JTWARNING,0,true);

				if ( MWM.get_robot_state().in_teachmoving )
				{
					if ( teach_info.status == TEACH_FORWARDMOVING || teach_info.status == TEACH_BACKWARDMOVING ||
						MWM.get_robot_state().mc_info.iMotionStatus != MotionCoreInfo::msSTOP )
					{
						SMC.setCmdInfo(CMD_ABORT);
					}
					teach_info.status = TEACH_END;
				}
				else
				{
					if ( isCodeSysMotion )
					{
						SMC.setCmdInfo(CMD_ABORTJOG); 
					}
					else
						SMC.setCmdInfo(CMD_ABORT); 

				}
				return true;
			}

			if ( MWM.get_robot_state().in_teachmoving  )//位置示教允许中
			{
				process_manual_teach( axis, dir );
				return true;
			}

			CartRef cartref = MWM.get_jog_cart().refSys;
			CartRef tool    = MWM.get_jog_tool().refSys;
			m_command.iCommandType  = CMD_JOG;
			m_command.iJogCoordType = MWM.get_robot_state().coordinate; // jtJOINT; 
			m_command.chJogAxisMark = axis;
			m_command.chJogDirMark  = dir;
			m_command.cartref = &cartref;
			m_command.tool    = &tool.cpos;
			// speed parameters
			m_command.dyn.custom_parameter = false;
			//  			cmd.dyn.JointVel  = MWM.get_external_speed();
			if ( isCodeSysMotion )
			{
				if ( mc_info_thread.iMotionStatus == MotionCoreInfo::msSTOP )
				{
					SMC.setCmdInfo( m_command );
				}
			}
			else
			{
				if ( mc_info_thread.iMotionStatus == MotionCoreInfo::msSTOP )
					SMC.setCmdInfo( m_command );
			}
			return true;
		}
		else if ( jog_home_step == JOG_HOMESTART ||
			jog_home_step == JOG_HOMING )
		{
			if ( jog_home_step == JOG_HOMESTART )
			{
				//CmdInfo cmd;
				m_command.iCommandType  = CMD_PTP;
				m_command.destPos       = homepos; 
				m_command.dyn.custom_parameter = false;
				m_command.dyn.JointVel  = MWM.get_external_speed() > 10 ? 10 : MWM.get_external_speed();
				SMC.setCmdInfo(m_command); 
				jog_home_step = JOG_HOMING;
			}
			else if ( jog_home_step == JOG_HOMING )
			{
				if ( !enable_press )
				{
					SMC.setCmdInfo(CMD_ABORT); 
					jog_home_step = JOG_HOMEEND;
				}
			}
		}
		else // when jog key press didn't press
		{
			// for check user teach program
			if ( ProgramEditor::is_running_flag == true ) 
			{
				if ( !enable_press )
				{
					if ( program_dialog->GetSafeHwnd() )
					{
						program_dialog->on_free_script(true);
					}					
 					MSD.ShowInfo(_T("试运行，请按下使能开关！"),JTWARNING,0,true);

					return true;
				}
			}
			else // the other is all stop
			{
				if ( MWM.get_robot_state().in_teachmoving  ) // 位置示教允许中
				{
					if ( MWM.get_robot_state().mc_info.iMotionStatus != MotionCoreInfo::msSTOP || 
						teach_info.status == TEACH_FORWARDMOVING || 
						teach_info.status == TEACH_BACKWARDMOVING )
					{
						SMC.setCmdInfo(CMD_ABORT); 
					}
					teach_info.status = TEACH_END;
 				}
				else
				{
					if ( MWM.get_robot_state().mc_info.iMotionStatus != MotionCoreInfo::msSTOP )
					{
						if ( isCodeSysMotion )
						{
							SMC.setCmdInfo(CMD_ABORT); 

							SMC.setCmdInfo(CMD_ABORTJOG); 
						}
						else
							SMC.setCmdInfo(CMD_ABORT); 
					}
 				}

			}

			return false;
		}
	}
	else // remote or play mode
	{
		if ( axis != 0 )
		{
 			MSD.ShowInfo(_T(" 示教，请切换到示教模式！"),JTWARNING,0,true);
 		}
	}

	return true;
}

void MotionControl::set_joghome_step( const int step ) 
{ 
	Lock();
	jog_home_step = step; 
	UnLock();
}

const int MotionControl::get_joghome_step()
{
	return jog_home_step;
}

int MotionControl::jog_go_home( const PosInfo& pi )
{
	printf("ptp home position\n");
	if ( MWM.get_robot_state().servo_on )
	{
		if ( MWM.get_robot_state().operate == omTEACH )
		{
			Lock();
			homepos = pi;
			jog_home_step = JOG_HOMESTART; 
			UnLock();
		}
		else
		{
			MSD.ShowInfo(_T("请切换到示教模式！"),JTMESSAGE);
		}
	}
	else
		MSD.ShowInfo(_T("打开伺服使能！"),JTMESSAGE);

	return 0;
}


PosInfo MotionControl::get_last_teach_pos()
{
	PosInfo lastPos;
	return lastPos; 
}

// + and finished.
void MotionControl::set_last_teach_pos( const PosInfo& pi )
{
	static bool bFirstStart = true;
	if ( mc_info_thread.iMotionStatus == MotionCoreInfo::msSTOP )
	{
		if ( bFirstStart )
		{
			last_teach_command.destPos = get_position( PosInfo::CTaxis );
			bFirstStart = false;
		}
		else
		{

			if ( 1 /* && curr.dest == teach.dest */ )
			{
				last_teach_command.destPos = pi;
			}
		}
	}

}

void MotionControl::adjust_velocity( CmdInfo& curr_teach_cmd )
{
	curr_teach_cmd.dyn.JointVel = 20;
	curr_teach_cmd.dyn.JointAcc = 30;
	curr_teach_cmd.dyn.JointDec = 30;
	curr_teach_cmd.dyn.JointJerk = 100.0;

	curr_teach_cmd.dyn.PathVel = 100;
	curr_teach_cmd.dyn.PathAcc = 750;
	curr_teach_cmd.dyn.PathDec = 750;
	curr_teach_cmd.dyn.PathJerk = 1e6;


 	const double max_teach_vel= 20;  
	double actual_vel_percent   = min( MWM.get_external_speed(), max_teach_vel );
  
	if ( curr_teach_cmd.iJogCoordType == jtJOINT )
	{
 		curr_teach_cmd.dyn.JointVel = actual_vel_percent;  
		curr_teach_cmd.dyn.JointAcc = 30;
		curr_teach_cmd.dyn.JointDec = 30;
		curr_teach_cmd.dyn.JointJerk = 100.0;

	}
	else
	{ 
		curr_teach_cmd.dyn.PathVel = 1000 * actual_vel_percent * 0.01;
		curr_teach_cmd.dyn.PathAcc = 750;
		curr_teach_cmd.dyn.PathDec = 750;
		curr_teach_cmd.dyn.PathJerk = 1e6;
	}
}

void MotionControl::process_manual_teach( unsigned char axis, unsigned char dir )
{
 	if ( axis == 1 ) 
	{
		PosInfo curr_pos;
		switch( teach_info.status )
		{
		case TEACH_END: 
			{
				teach_info.status = TEACH_PREPARE;
				m_command = MWM.get_teach_command();
				if ( dir ) // -
				{
					if ( teach_info.first_forward ) // whether executed forward move once. 
					{
						teach_info.status = TEACH_END;
 						return;
					}
  				}
				else // +
				{
					if ( teach_info.first_forward )
					{
 						teach_info.first_forward = false;
						teach_info.backwardPos = get_position(PosInfo::CTaxis);
					}
					else
					{
						if ( teach_info.haveReached )
						{
							teach_info.backwardPos = teach_info.forwardPos;
						}
					}
			    }
			}
			break;
		case TEACH_PREPARE:
			{
				teach_info.haveReached = false;
				if ( dir )
				{
					teach_info.status = TEACH_BACKWARDMOVING;
					m_command.destPos = teach_info.backwardPos;
				}
				else
				{
					teach_info.status = TEACH_FORWARDMOVING;
					teach_info.forwardPos = m_command.destPos;
				}

				m_command.dyn.custom_parameter = false;
				m_command.dyn.JointVel  = MWM.get_external_speed() > 20 ? 20 : MWM.get_external_speed();
				adjust_velocity(m_command);
				SMC.setCmdInfo(m_command);
			}
			break;
		case TEACH_FORWARDMOVING:
		case TEACH_BACKWARDMOVING:
			{
				if ( mc_info_thread.iMotionStatus == MotionCoreInfo::msSTOP )
				{
					curr_pos = get_position(m_command.destPos.pos_type);
 					if ( curr_pos == m_command.destPos )
					{
						teach_info.haveReached = true;
					}
				}
			}
			break;
 		default: break;
		}
 	}
	cout << " teach status = " << (int)teach_info.status << " pos = [ " << 
		rad2deg( teach_info.backwardPos.axis.q1 ) << " ] [ " << rad2deg(  teach_info.forwardPos.axis.q1 ) << " ]" << '\n';
}

void MotionControl::reset_teach_info()
{
// 	teach_info.backwardPos = get_position(PosInfo::CTaxis);
// 	teach_info.forwardPos  = get_position(PosInfo::CTaxis);
	teach_info.status = TEACH_END;
	teach_info.haveReached = true;
}