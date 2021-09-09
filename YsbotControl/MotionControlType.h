// Copyright 2016, 亚思信息科技有限责任公司
// All right reserved.
//
// Author: Liu Run Dan ( lrdcz@163.com )
//
// Yasi Robot Motion Control System - 亚思机器人运动控制系统

#ifndef  _MotionControl_type_h
#define  _MotionControl_type_h

#include "CmdInfo.h"
#include "IoModuleParameters.h"
#include "MotionCoreInfo.h"

class MotionControlType
{
 
public:

	virtual ~MotionControlType () throw () {;}

	virtual int  init_system()  = 0;
	virtual int  close_system() = 0;
	virtual void set_override( const double =100 ) = 0;
	
	virtual int  setCmdInfo(const CmdInfo& )        = 0;
	virtual int  movePath( const CmdInfo* )    = 0;

	virtual int  goHome( int )         = 0;
	virtual int  abortHome( )          = 0;
	virtual int  get_home_status( int )    = 0;

 	virtual const MotionCoreInfo& get_motion_core_information () throw () =0;

	virtual const int   get_group_status(unsigned int ) = 0;
	virtual const char* get_error()      = 0;
	virtual const PosInfo&  get_position(  const PosInfo::CoordinateType = PosInfo::CTaxis, const LocationDataSource = LDScommad )  throw(YsbotsException)= 0 ;
	virtual const  IoModuleParameters& get_mcc_io_parameters() const throw () = 0;

	virtual int read_input ( unsigned short*, unsigned short =0  ) throw () =0;
	virtual int write_word ( unsigned short, unsigned short, unsigned short =0, bool inqueue = false, double dfDelayTime = 0  ) throw () =0;
	virtual int read_word ( unsigned short, unsigned short*, unsigned short =0  ) throw () =0;

	virtual bool is_queue_full ()  = 0;
 };

#endif