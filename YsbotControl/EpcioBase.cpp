#include "StdAfx.h"
#include "EpcioBase.h"
#include "Servo.h"
#include "WorldModel.h"

int nLIOTriggerCount[7] = {0, 0, 0, 0, 0, 0, 0};

EpcioBase::EpcioBase(const ConfigReader& vr ) throw (InvalidConfigurationException): isSimulate(true),dfOverride(10),
	axis_num(MAX_JOINT_NUM),nGroupIndex(-1)
{
 	unsigned int ex_num = 0;
	unsigned int ey_num = 0;
	bool use_mcc_rio = false;
	unsigned int cycle = 0;

	vr.get("MCC_RIO::extend_x_num", ex_num);
	vr.get("MCC_RIO::extend_y_num", ey_num);	 
	vr.get("MCC_RIO::use_mcc_rio",  use_mcc_rio);
	vr.get("MCC_RIO::scan_task_cycle",  cycle);

	int ex_word_size = static_cast<int>( ceil( static_cast<double>(ex_num) / 16.0 ) );
	int ey_word_size = static_cast<int>( ceil( static_cast<double>(ey_num) / 16.0 ) );

	mcc_io_param.is_connected = use_mcc_rio;
	mcc_io_param.scan_cycle_time = cycle;
	mcc_io_param.x_point_num = ex_num;
	mcc_io_param.y_point_num = ey_num;
	mcc_io_param.x_word_size = ex_word_size;
	mcc_io_param.y_word_size = ey_word_size;

	mcc_io_param.local_input_size = 4;

	vr.get("simulate_motion",  isSimulate);
}

EpcioBase::~EpcioBase(void)
{
	;
}
 
int EpcioBase::init_system()
{
	TRACE(_T("init MTC MotionControl!\n")); 

	MCC_CloseSystem();
	int  pulse_limit[6] = {0};

	const GroupProperties& group_properties = MWM.get_group_properties();
	MCC_SetSysMaxSpeed( group_properties.profile.dfMaxCSpeed );

	SYS_MAC_PARAM  smp;
	memset(&smp, 0, sizeof(SYS_MAC_PARAM));

	SYS_ENCODER_CONFIG  sec;
	memset(&sec, 0, sizeof(SYS_ENCODER_CONFIG));

	MECH_PARAM  mp;
	memset(&mp, 0, sizeof(MECH_PARAM));

	SYS_HOME_CONFIG    stHomeConfig; 	
	memset(&stHomeConfig, 0, sizeof(SYS_HOME_CONFIG));
	HOME_PARAM  home_para;
	memset(&home_para, 0, sizeof(HOME_PARAM));
	ENCODER_PARAM  enc_para;
	memset(&enc_para, 0, sizeof(ENCODER_PARAM));

	axis_num = group_properties.axis_num;
	if ( axis_num > 6)
 		axis_num = 6;

 	for(unsigned int nCh = 0; nCh < axis_num; ++nCh )
	{
		mp = group_properties.mp[nCh];

		pulse_limit[nCh] = int( double(INTERPOLATION_PERIOD * mp.rpm * mp.ppr) / 60.0 / 1000.0 );  

		smp.wPosToEncoderDir = mp.reverse ? 1: 0;
		smp.wRPM         = (WORD)( mp.rpm );
		smp.dwPPR        = mp.ppr;  
		smp.dfPitch      = deg2rad(mp.pitch);
		smp.dfGearRatio  = mp.gear_ratio;
		smp.dfHighLimit  = deg2rad(mp.high_limit);  // 决定MCCL系统的角度单位
		smp.dfLowLimit   = deg2rad(mp.low_limit);
		smp.wCommandMode = OCM_PULSE;
		smp.wPulseMode   = DDA_FMT_CW;
		smp.wPulseWidth  = 40;

		smp.wOverTravelUpSensorMode   = mp.UpSensorMode;   // OT- motor alarm.
		smp.wOverTravelDownSensorMode = mp.DownSensorMode;

		VERIFY( MCC_SetMacParam(&smp, nCh, CARD_INDEX) == NO_ERR );

		//set encoder parameters
		enc_para = group_properties.encConfig[nCh];
		sec.wType      = enc_para.wType;
		sec.wAInverse  = enc_para.wAInverse;
		sec.wBInverse  = enc_para.wBInverse;
		sec.wCInverse  = enc_para.wCInverse;
		sec.wABSwap    = enc_para.wABSwap;
		sec.wInputRate = enc_para.wInputRate;

		VERIFY( MCC_SetEncoderConfig(&sec, nCh, CARD_INDEX) == NO_ERR );

		//  set go-home configures
		home_para = group_properties.homeConfig[nCh];
		stHomeConfig.wMode                   = home_para.wMode;   
		stHomeConfig.wDirection              = home_para.wDirection;  //  negative direction
		stHomeConfig.wSensorMode             = home_para.wSensorMode; // use general I/O
		stHomeConfig.nIndexCount             = home_para.nIndexCount;
		stHomeConfig.dfAccTime               = home_para.dfAccTime;//  ms
		stHomeConfig.dfDecTime               = home_para.dfDecTime;//  ms
		stHomeConfig.dfHighSpeed             = home_para.dfHighSpeed; //  mm/s
		stHomeConfig.dfLowSpeed              = home_para.dfLowSpeed;  //  mm/s
		stHomeConfig.dfOffset                = home_para.dfOffset;

		MCC_SetHomeConfig(&stHomeConfig, nCh, CARD_INDEX);  //  go-home configures are the same for all axes

	}

	MCC_CloseAllGroups();
	VERIFY( (nGroupIndex = MCC_CreateGroup(0, 1, 2, 3, 4, 5, CARD_INDEX)) >= 0 );
	
	set_kinematicTrans( true );

	SYS_CARD_CONFIG  scc;
	scc.wCardType = EPCIO_6_AXIS_PCI_CARD;

	int nret = -1;
	if ( isSimulate )
	{
		nret = MCC_InitSimulation(INTERPOLATION_PERIOD, &scc, 1);
	}
	else
	{
		nret = MCC_InitSystem(INTERPOLATION_PERIOD, &scc, 1);
	}
	if( nret == NO_ERR ) 
	{
		MCC_SetAbsolute(nGroupIndex);   

		MCC_SetAccTime(500, nGroupIndex);   
		MCC_SetDecTime(500, nGroupIndex);
		MCC_SetAccType('S',nGroupIndex);
		MCC_SetDecType('S',nGroupIndex);

		MCC_SetMaxPulseSpeed(
			pulse_limit[0],
			pulse_limit[1],
			pulse_limit[2],
			pulse_limit[3],
			pulse_limit[4],
			pulse_limit[5],
			CARD_INDEX);

		MCC_SetMaxPulseAcc(
			pulse_limit[0],
			pulse_limit[1],
			pulse_limit[2],
			pulse_limit[3],
			pulse_limit[4],
			pulse_limit[5],
			CARD_INDEX);

		MCC_SetOverTravelCheck(1, 1, 1, 1, 1, 1, nGroupIndex); // soft limit

		if ( !isSimulate )
		{
			VERIFY( MCC_EnableRIOSetControl(  RIO_SET0) == NO_ERR );
			VERIFY( MCC_EnableRIOSlaveControl(RIO_SET0) == NO_ERR );
		}

		// 		SetSystemState(SS_READY);
		// 		AfxGetMainWnd()->SendMessage(WM_SET_SYSTEM_STATE, SS_READY);
		int det;
		// 		int det = MCC_SetLIORoutineEx(LIO_ISR_Function,CARD_INDEX);
		det = MCC_SetTimer(100000);
		det = MCC_GetErrorCode();
		det = MCC_EnableTimer(CARD_INDEX);
		det = MCC_EnableTimerTrigger();

		det = MCC_SetLIOTriggerType(LIO_INT_RISE,LIO_LDI0);
		det = MCC_EnableLIOTrigger(LIO_LDI0);

		// 		MCC_SetRIORoutineEx(RIO_ISR_Function, CARD_INDEX);//  link a interrupt service function for remote I/O set 0
		MCC_SetRIOTriggerType(RIO_INT_RISE, RIO_SET0, RIO_DI0, CARD_INDEX);//  set trigger mode of remote I/O set 0's digital input 0
		MCC_EnableRIOInputTrigger(RIO_SET0, RIO_DI0, CARD_INDEX);//  enable the signal from remote I/O set 0's digital input 0 to trigger a interrupt service function


		if (isSimulate)
		{
			double pos[] = { 0, 45, 90, 0, 45, 0, 0, 0 };
			for( unsigned int i = 0; i < axis_num; ++i )
			{
				MCC_DefinePos(i, deg2rad(pos[i]), nGroupIndex);
			}
		}

		return true;
	}
	else
	{
		AfxMessageBox( _T(" 机械手控制系统初始化失败！") );
		return false;
	}

	return 0;
}

int EpcioBase::close_system()
{
	if ( !isSimulate )
	{
 		MCC_DisableRIOSlaveControl(RIO_SET0, CARD_INDEX);//  disable remote I/O set 0' slave node
		MCC_DisableRIOSetControl(RIO_SET0, CARD_INDEX);  //  disable remote I/O set 0
 		MCC_CloseSystem();
  	}
 	return 0;
}

int EpcioBase::set_kinematicTrans( bool bSet )
{
	int  retval;

	if ( RobotKinematics::is_main_robot_kinematics_available() )
	{
		if ( KINE.is_use_robot_lib() )
		{
			retval = KINE.init_robot_lib( bSet, nGroupIndex );
			return retval;
		}
		else
		{
			if (bSet)
			{
				retval = MCC_SetKinematicTrans(
					static_cast<FWDKINEFUN>(RobotKinematics::get_main_robot_kinematics().pFwdKineFuc),
					static_cast<INVKINEFUN>(RobotKinematics::get_main_robot_kinematics().pInvKineFuc),
					sizeof(EXTENSION),
					static_cast<INIT_EXTENSION>(KINE.pInitExtensionFuc) );
			}
			else
			{
				retval = MCC_SetKinematicTrans(NULL, NULL);
			}
		}
	}
	else
	{
		retval = MCC_SetKinematicTrans(NULL, NULL);
	}

	if( retval == NO_ERR )
	{
		if( bSet )
			MCC_SetPreProcessMotionCommandCallback( static_cast<PRE_PROCESS_MOTION_COMMAND>(KINE.pPreProcessMotionCmdFuc) );
		else
			MCC_SetPreProcessMotionCommandCallback(NULL);
	}

	return retval;

}

const PosInfo&  EpcioBase::get_position( const PosInfo::CoordinateType pct /* = mcACS */, const LocationDataSource lds /* = LDScommad */,	WORD groupIndex ) throw(HardwareException)
{
	int ret = 0;
	double dj[MAX_JOINT_NUM] = {0};

	if ( lds == LDScommad )
	{
 		ret = MCC_GetCurPos( &dj[0], &dj[1], &dj[2], &dj[3],  &dj[4], &dj[5], groupIndex);
 		current_rloc.valid = (ret == 0) ? true : false;
    }
	else if ( lds == LDSservoCom )
	{
		ret = get_pos_servoCom( dj );
		current_rloc.valid = (ret == 0) ? true : false;
//   		current_rloc.valid = false;
//  		current_rloc.valid = true;
// 		dj[0] = M_PI / 2;
// 		dj[1] = M_PI / 2;
// 		dj[2] = M_PI / 2;
// 		dj[3] = M_PI / 2;
// 		dj[4] = M_PI / 2;
// 		dj[5] = M_PI / 2;

   	}

	if ( ret == NO_ERR )
	{
		current_rloc.set_axis_rad( dj[0], dj[1], dj[2], dj[3], dj[4], dj[5], dj[6], dj[7] ); 
		if ( pct == PosInfo::CTmachine )
		{
 			KINE.fwd_kine( current_rloc.axis, &current_rloc.cart );
 		}
 	}
	current_rloc.pos_type = pct;
 
	return current_rloc;
}

int EpcioBase::get_pos_servoCom( double* pos )
{
	if ( Servo::is_main_servo_available() == false )
 		return -1;
	 
	int nret = Servo::get_main_servo().read_pos_from_servo( pos ); // in rad

	return nret;
}
 
const int EpcioBase::get_grp_status(unsigned int grpIdx)
{
 	return MCC_GetMotionStatus(grpIdx);
}

int EpcioBase::powerGrp(bool nOnOff)
{
	Ysbot::Time t;
	InOutObject ionew;
	ionew.object_type = InOutObject::servo_enable;
	if ( nOnOff )
	{
		if ( !isSimulate )
		{
			for(unsigned int nCh = 0; nCh < axis_num; nCh++)
			{
 				VERIFY( MCC_SetServoOn(nCh, CARD_INDEX) == NO_ERR );
 			}
 		}

		ionew.state = (1 << axis_num ) - 1; // 2^N - 1
 
	}
	else
	{
		if ( !isSimulate )
		{
			for(unsigned int nCh = 0; nCh < axis_num; nCh++)
			{
 				VERIFY( MCC_SetServoOff(nCh, CARD_INDEX) == NO_ERR );
 			}
 		}

		ionew.state = 0; // 2^N - 1
  	}
	MWM.set_io_information(ionew,t);
	return 0;
}
  

int EpcioBase::setPosition( const CmdInfo& cf )
{
	double pos[MAX_JOINT_NUM] = {0};
	AxisPos axis;
	int ret = 0;
	if ( cf.customStartPos == false )
	{
		ret = get_pos_servoCom( pos ); // in deg
		if ( ret != 0 )
			return ret;
	 
		axis.set_deg( pos[0], pos[1],  pos[2], pos[3], pos[4], pos[5] );
		
		for( unsigned int i = 0; i < axis_num; ++i )
		{
 			MCC_DefinePos(i, deg2rad(pos[i]), nGroupIndex);
 		}

	}
	else
	{
		axis = cf.startPos.axis;
		pos[0] = cf.startPos.axis.q1;
		pos[1] = cf.startPos.axis.q2;
		pos[2] = cf.startPos.axis.q3;
		pos[3] = cf.startPos.axis.q4;
		pos[4] = cf.startPos.axis.q5;
		pos[5] = cf.startPos.axis.q6;
		pos[6] = cf.startPos.auxi.q1;
		pos[7] = cf.startPos.auxi.q2;
		for( unsigned int i = 0; i < axis_num; ++i )
		{
 			int ret = MCC_DefinePos(i, pos[i], nGroupIndex);
 		}
 	}

 	KINE.define_start_pos( axis ); // for numeric inv kinematics.

	return ret;
}

const char* EpcioBase::get_error()
{
 	int  nError = MCC_GetErrorCode(nGroupIndex);
 	strerr = GetErrorDescription( nError );
	if ( nError != 0 )
	{
		TRACE("error = %d \n",nError);
	}
 	return strerr;
}

const char*	EpcioBase::GetErrorDescription(int nErrorCode)
{
 	for(int i = 0; i < ARRAY_SIZE(errors); ++i)
	{
		if( errors[i].nErrorCode == nErrorCode )
			return errors[i].lpszDesc;
	}

	return NULL;
}

int EpcioBase::resetGrp()
{
	MCC_ClearError(nGroupIndex);
	return 0;
}

void EpcioBase::resetCmdIdx()
{
	MCC_ResetCommandIndex(nGroupIndex);
}
 
/*    
	stCommandInfo 

	members :	
		int     nType;	//  0: Pont to Point
						//  1: Line
						//  2: Clockwise Arc/Circle
						//  3: Counterclockwise Arc/Circle
						//  4: Clockwise Helica
						//  5: Counterclockwise Helica
						//  6: Delay
						//  7: Enable Blend
						//  8: Disable Blend
						//  9: Enable In-Position
						// 10: Disable In-Position
		int     nCommandIndex;
		double  dfFeedSpeed;
		double  dfPos[MAX_AXIS_NUM];
*/
const MotionCoreInfo& EpcioBase::get_motion_information() throw ()
{
 	int CmdCount = 0;
	COMMAND_INFO  ci = {0};
	int  nError = MCC_GetErrorCode(nGroupIndex);
	strerr = GetErrorDescription( nError );

	// 0 GMS_RUNNING:  executing a motion command
	// 1 GMS_STOP:     stopping status and none of command exist in a command queue
	// 2 GMS_HOLD:     holding status (because usig MCC_HoldMotion())  and some commands exist in a command queue
	// 3 GMS_DELAYING: delay status   (because usig MCC_DelayMotion()) and some commands exist in a command queue
	int motion_status = MCC_GetMotionStatus(nGroupIndex);

	MCC_GetCurCommand(&ci, nGroupIndex); 		//  get a current executing motion command

	MCC_GetCommandCount(&CmdCount,nGroupIndex); 		//  get motion command stocks in a motion queue

	gi.iMotionStatus = static_cast<MotionCoreInfo::MotionStatus>(motion_status);
	gi.nCmdType      = ci.nType;
	gi.nCommandIndex = ci.nCommandIndex;
	gi.dfFeedSpeed   = ci.dfFeedSpeed;
	gi.nCommandCount = CmdCount;

	gi.nHomeStatus   = MCC_GetGoHomeStatus();

	for (int i=0; i < MAX_JOINT_NUM; i++ )
	{
		gi.dfPos[i] = ci.dfPos[i];
	}

	if ( strerr )
	{
		sprintf(gi.errString,"%s", strerr);
	}

	gi.dfCurFeedSpeed = MCC_GetCurFeedSpeed(nGroupIndex);//  get current feed rate
	double axis_speed[MAX_JOINT_NUM] = {0};
	//  get current every axis's speed
	MCC_GetSpeed(&gi.dfAxisSpeed[0], &gi.dfAxisSpeed[1], &gi.dfAxisSpeed[2], &gi.dfAxisSpeed[3], &gi.dfAxisSpeed[4], &gi.dfAxisSpeed[5], nGroupIndex); 

	return gi;
}

void _stdcall EpcioBase::LIO_ISR_Function(LIOINT_EX *pstINTSource)
{
	if (pstINTSource->LDI0)
		nLIOTriggerCount[0]++;//  local I/O 0 Trigger

	if (pstINTSource->LDI1)
		nLIOTriggerCount[1]++;//  local I/O 1 Trigger

	if (pstINTSource->LDI2)
		nLIOTriggerCount[2]++;//  local I/O 2 Trigger

	if (pstINTSource->LDI3)
		nLIOTriggerCount[3]++;//  local I/O 3 Trigger

	if (pstINTSource->LDI4)
		nLIOTriggerCount[4]++;//  local I/O 4 Trigger

	if (pstINTSource->LDI5)
		nLIOTriggerCount[5]++;//  local I/O 5 Trigger

	if (pstINTSource->LDI6)
		nLIOTriggerCount[6]++;//  local I/O 6 Trigger

	static int ic = 0;
	if (pstINTSource->TIMER)
	{
		TRACE(_T("HOME0 callback fuction %d !\n"),	ic++);
	}
}

void __stdcall EpcioBase::RIO_ISR_Function(RIOINT_EX *pstINTSource)
{
	static int ic = 0;
	if (pstINTSource->SET0_DI0)//  check whether this ISR was triggered with a signal from set 0's digital input 0 
	{
		TRACE(_T("set 0's digital input 0 %d !\n"),	ic++);
	}
	if (pstINTSource->SET0_DI1)//  check whether this ISR was triggered with a signal from set 0's digital input 1 
	{
		TRACE(_T("set 0's digital input 1 %d !\n"),	ic++);
	}
	if (pstINTSource->SET0_DI2)//  check whether this ISR was triggered with a signal from set 0's digital input 2 
	{
		TRACE(_T("set 0's digital input 2 %d !\n"),	ic++);
	}
	if (pstINTSource->SET0_DI3)//  check whether this ISR was triggered with a signal from set 0's digital input 3 
	{
		TRACE(_T("set 0's digital input 3 %d !\n"),	ic++);
	}
}
  
int EpcioBase::GetRIOInputValue(WORD *pwValue, WORD wSet, WORD wPort, WORD wCardIndex /* = 0 */)
{
	return MCC_GetRIOInputValue(pwValue,wSet,wPort,wCardIndex);
}

int EpcioBase::SetRIOOutputValue(WORD wValue, WORD wSet, WORD wPort, WORD wCardIndex /* = 0 */)
{
	return MCC_SetRIOOutputValue( wValue,wSet,wPort,wCardIndex);
}

int EpcioBase::EnquRIOOutputValue(WORD wValue,WORD wMask, WORD wSet, WORD wPort, WORD wCardIndex /* = 0 */)
{
	return 0;
}

