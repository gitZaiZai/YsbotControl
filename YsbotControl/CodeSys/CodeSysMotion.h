// Copyright 2016, 亚思信息科技有限责任公司
// All right reserved.
//
// Author: Liu Run Dan ( lrdcz@163.com )
//
// Yasi Robot Motion Control System - 亚思机器人运动控制系统

#ifndef codesys_motion_control_h
#define codesys_motion_control_h

#include "../MotionControltype.h"
#include "../ConfigReader.h"
#include "RobotClient.h"
#include "../PoseTransform.h"
#include "../GroupProperties.h"

class CodeSysMotion  : public MotionControlType
{
public:
	CodeSysMotion(const ConfigReader&) throw (InvalidConfigurationException);
	~CodeSysMotion(void);

	int  init_system();
	int  close_system();
	inline void set_override( const double od = 100 ) { ; }

	int  goHome( int )  { return 0; }
	int  abortHome( )   { return 0; }
	inline int  get_home_status( int )    { return 0; }

	int  movePath( const CmdInfo* )  { return 0; }
	inline int  setCmdInfo(const CmdInfo& );

	const MotionCoreInfo& get_motion_core_information () throw ();
	const int  get_group_status(unsigned int grpIdx = 0);

	const char*  get_error();
	inline const PosInfo&  get_position( const PosInfo::CoordinateType pct= PosInfo::CTaxis, const LocationDataSource lds= LDScommad ) throw(YsbotsException); 

	inline const  IoModuleParameters& get_mcc_io_parameters() const throw () { return iomodule_para; }

	int read_input ( unsigned short*, unsigned short =0  ) throw ();
	int write_word (unsigned short, unsigned short, unsigned short =0, bool inqueue = false, double dfDelayTime = 0  ) throw ();
	int read_word ( unsigned short, unsigned short*, unsigned short =0  ) throw ();
	inline bool is_queue_full () { return robot_client.is_queue_full(); }

	CmdInfo  cmd_info;

private:
	RobotClient				robot_client;
 	Ysbot::IECServerData	server_data;
	Ysbot::IECServerData	server_data_from_shm;
	Ysbot::ClientCommand	client_command;

    MotionCoreInfo motion_info;
	PosInfo        pos_info;
	IoModuleParameters iomodule_para;
	unsigned int axis_num;

	bool  bConnectSuccess;
	bool  bEnableUserCoordinate;
	bool  bAutoPlay;
	bool  bExitAuto;
	bool  bStartSimulator;
	bool  bJogMotion;

	int   robot_type;
	std::string addrCurr;

	PoseTransform pose_trans;
	PosInfo last_robot_pos;
	Ysbot::SMC_DYN_LIMITS max_path_dyn;
  
	Ysbot::SMC_DYN_LIMITS max_joint_dyn[6];
	
	bool reset_position;
	double encoder_pos[MAX_JOINT_NUM];

	bool  creat_connect(const char * addr);
	void  jog_motion( const CmdInfo& cmd_info );
	void  jog_motion_group ( const CmdInfo& cmd_info );

	int   get_jog_type();

	void set_robot_type ( int iType = 0 );
	void enable_robot_group ( bool =true);
	void auto_play_program ();
	int  wait_queue_notfull ();
	int  wait_finished ();
	void delay_motion ( const CmdInfo& cmd_info );
	void wait_input ( const CmdInfo& cmd_info );
	void set_digital_ouput (const CmdInfo& cmd_info);
	void stop_jog(const CmdInfo& cmd_info);
	void move_ptp(const CmdInfo& cmd_info);
	void move_line(const CmdInfo& cmd_info);
	void move_circle(const CmdInfo& cmd_info);

	void move_ptp_rel(const CmdInfo& cmd_info);
	void move_line_rel(const CmdInfo& cmd_info);
	void move_circle_rel(const CmdInfo& cmd_info);


	void reset_group ();
	void hold_motion ();
	void continue_motion ();
	void abort_motion (const CmdInfo& cmd_info);
	void stop_motion (const CmdInfo& cmd_info);
	void power_servo (const CmdInfo& cmd_info);
	void set_position (const CmdInfo& cmd_info);
	void set_group_override  (const CmdInfo& cmd_info);

	void SetCoordinateTransform (const CmdInfo& coordref );

	int  rotate_tcp_with_tool (const CmdInfo& cmd_info);

	const PosInfo&  get_last_axispos() throw();
	void set_last_axispos(const PosInfo&) throw();
	void set_max_path_dyn ();
	void set_max_joint_dyn ();
	// void SetToolTransform ();

	void conveyor_begin (const CmdInfo& cmd_info);
	void conveyor_end (const CmdInfo& cmd_info);
	int  conveyor_wait (const CmdInfo& cmd_info);
	void conveyor_done (const CmdInfo& cmd_info);

};

#endif