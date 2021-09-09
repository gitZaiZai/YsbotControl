// Copyright 2016, 亚思信息科技有限责任公司
// All right reserved.
//
// Author: Liu Run Dan ( lrdcz@163.com )
//
// Yasi Robot Motion Control System - 亚思机器人运动控制系统

#ifndef motion_control_h
#define motion_control_h

#include <stdexcept>
#include "MotionControlType.h"
#include "ConfigReader.h"
#include "Time.h"
#include <vector>
#include <map>
#include "Trigger.h"
#include "ThreadTaskTypeBase.h"
#include "PosInfo.h"
#include "InOutObject.h"

 struct TeachPosInfo {
      
    bool haveReached;      
    PosInfo backwardPos;
	PosInfo forwardPos;
	int status;
	bool first_forward;
  };

class MotionControl : public ThreadTaskTypeBase
{
private:
	MotionControlType* the_motion_control;
	char* motion_control_descriptor;
 
	const ConfigReader& configuration_list;

	void really_change_motion_control_type (const char*, const ConfigReader&) throw (YsbotsException, std::bad_alloc );
	void process_manual_teach ( unsigned char axis, unsigned char dir );

 	bool             use_mcc_rio;

 	unsigned int thread_task(ThreadTaskTypeBase*);
	
 	unsigned short* remote_input;
	unsigned short* local_input;
    unsigned short gpio_input_value;

	int rio_word_num;
	int local_word_num;
	MotionCoreInfo mc_info_thread;
	bool isInitSystem;
	CmdInfo command_info;
	std::vector<Trigger> trigger_list;

	PosInfo currpos;
	int jog_home_step;
	PosInfo homepos;
	CmdInfo m_command;
	double max_manual_speed;
	bool   is_pc_pendant; // for debug simulator.


	CmdInfo last_teach_command;
	TeachPosInfo teach_info;

public:
	static MotionControl& get_main_motion_control() throw ();
	static bool is_main_motion_control_available () throw ();
	static MotionControl* main_motion_control;
	bool second_page;
	int key_switch;
	bool isCodeSysMotion;
	enum { JOG_NOTHOME = 0, JOG_HOMESTART = 1, JOG_HOMING = 2, JOG_HOMEEND = 3 };

	enum { TEACH_END = 0, TEACH_PREPARE = 1, TEACH_FORWARDMOVING = 2, TEACH_BACKWARDMOVING = 3 };

	MotionControl (const ConfigReader&) throw (YsbotsException, std::bad_alloc );
	~MotionControl () throw ();
 
	bool change_motion_control_type (const char*) throw ();

	bool change_motion_control_type (const char*, const ConfigReader&) throw ();

	const char* get_motion_control_type () const throw ();

	void getMotionControlTypeList(std::vector<std::string> &ptl);
 
	int  init_system();
	int  close_system();
	inline void  set_override( const double vel=100 ) { the_motion_control->set_override(vel); }

	inline int  setCmdInfo( const CmdInfo& gi ); 
	int setCmdInfo( CommandT ); 

	inline const int  get_group_status(unsigned int grpIdx = 0) { return the_motion_control->get_group_status(grpIdx); }
	inline const MotionCoreInfo& get_motion_core_information() throw() { return the_motion_control->get_motion_core_information(); }
	inline int  goHome( int axisID)            { return the_motion_control->goHome(axisID); }
	inline int  abortHome( )                   { return the_motion_control->abortHome(); }
	inline int  get_home_status(int axismark)  { return the_motion_control->get_home_status(axismark); }

	inline const char* get_error()     { return the_motion_control->get_error(); }
	
	const PosInfo&  get_position( const PosInfo::CoordinateType pct= PosInfo::CTaxis, const LocationDataSource lds= LDScommad ); 
 
	bool process_motion_information( Ysbot::Time t ) throw();
	int read_input ( unsigned short* val, unsigned short itype=MCS_GPIO  ) throw ()    { return the_motion_control->read_input( val, itype ); }

 	int write_word (unsigned short pn, unsigned short val, unsigned short itype=MCS_GPIO,
                    bool inqueue = false, double dfDelayTime = 0   ) throw ()  
    { return the_motion_control->write_word(pn,val,itype,inqueue,dfDelayTime); }

	int read_word ( unsigned short pn, unsigned short* pval, unsigned short itype=MCS_GPIO  ) throw () { return the_motion_control->read_word( pn, pval, itype); }

	int mc_din( unsigned short, unsigned short*,  unsigned short itype=MCS_GPIO );
	int mc_dout( unsigned short, unsigned short,  unsigned short itype=MCS_GPIO, bool inqueue = false, double dfDelayTime = 0 );

	void add_trigger ( const Trigger& tr);
	void clear_trigger ();
	void process_trigger (unsigned long long);

 // void abort_motion ();
// 	void hold_motion ();
	// jog motion or start stop auto program and so on.
	bool response_key_press();

	void set_joghome_step( const int step );
	const int get_joghome_step();

	void set_home_pos( const PosInfo& pi ) { homepos = pi; }
	const PosInfo& get_home_pos () { return homepos; }

	int  jog_go_home( const PosInfo& pi );

	void set_pc_pendant (bool pt) { is_pc_pendant = pt; }
	PosInfo get_last_teach_pos ();
	void set_last_teach_pos ( const PosInfo& pi );
	void reset_teach_info ();

	inline bool is_queue_full () { return the_motion_control->is_queue_full(); }
	void adjust_velocity (CmdInfo& curr_teach_cmd);

};

#define SMC  MotionControl::get_main_motion_control()
#endif