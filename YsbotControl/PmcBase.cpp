#include "StdAfx.h"
#include "PmcBase.h"
#include "Servo.h"
#include "WorldModel.h"
#include "kinematics.h"

//#define USE_LIB


int nLIOTriggerCount_PMC[7] = {0, 0, 0, 0, 0, 0, 0};
int DriverAlarmFlag_PMC[MAX_JOINT_NUM] = { 0, 0, 0, 0, 0, 0, 0,0 , 0, 0, 0,0};

PmcBase::PmcBase(const ConfigReader& vr ) throw (InvalidConfigurationException):isSimulate(true),dfOverride(10),
	axis_num(MAX_JOINT_NUM),nGroupIndex(-1)
{
 	unsigned int ex_num = 0;
	unsigned int ey_num = 0;
	bool use_mcc_rio = false;
	unsigned int cycle = 0;

	vr.get("MCC_RIO::extend_x_num", ex_num); //???????????????????????
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

	vr.get("simulate_motion",       isSimulate );

}

PmcBase::~PmcBase(void)
{
	;
}
 
int PmcBase::init_system()
{
	TRACE(_T("init PMC MotionControl!\n")); 
	timeTest.update();

	MCS_CloseSystem();

	int  pulse_limit[MAX_AXIS_NUM] = {0};

	const GroupProperties& group_properties = MWM.get_group_properties();
 	axis_num = group_properties.axis_num;
 
	MCS_SetSysMaxSpeed( group_properties.profile.dfMaxCSpeed );

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

		VERIFY( MCS_SetMacParam(&smp, nCh, CARD_INDEX) == NO_ERR );

		//set encoder parameters
		enc_para = group_properties.encConfig[nCh];
		sec.wType      = enc_para.wType;
		sec.wAInverse  = enc_para.wAInverse;
		sec.wBInverse  = enc_para.wBInverse;
		sec.wCInverse  = enc_para.wCInverse;
		sec.wABSwap    = enc_para.wABSwap;
		sec.wInputRate = enc_para.wInputRate;
		
		VERIFY( MCS_SetEncoderConfig(&sec, nCh, CARD_INDEX) == NO_ERR );
	
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
 		MCS_SetHomeConfig(&stHomeConfig, nCh, CARD_INDEX);  //  go-home configures are the same for all axes
	}

	MCS_CloseAllGroups();
	VERIFY( (nGroupIndex = MCS_CreateGroup(0, 1, 2, 3, 4, 5, 6, 7,CARD_INDEX)) >= 0 );
	int nret = -1;

 	set_kinematicTrans( axis_num, true );

	SYS_CARD_CONFIG  scc;
	scc.wCardType = PMC_8000_CARD;
	if ( isSimulate )
	{
		nret = MCS_InitSimulation(INTERPOLATION_PERIOD, &scc, 1);
	}
	else
	{
		nret = MCS_InitSystem(INTERPOLATION_PERIOD, &scc, 1);
	}

	if( nret == NO_ERR ) 
	{
		MCS_SetAbsolute(nGroupIndex);   

		MCS_SetAccTime(500, nGroupIndex);   
		MCS_SetDecTime(500, nGroupIndex);
		MCS_SetAccType('S',nGroupIndex);
		MCS_SetDecType('S',nGroupIndex);

		MCS_SetMaxPulseSpeed(
			pulse_limit[0],
			pulse_limit[1],
			pulse_limit[2],
			pulse_limit[3],
			pulse_limit[4],
			pulse_limit[5],
			pulse_limit[6],
			pulse_limit[7],
			CARD_INDEX);

		MCS_SetMaxPulseAcc(
			pulse_limit[0],
			pulse_limit[1],
			pulse_limit[2],
			pulse_limit[3],
			pulse_limit[4],
			pulse_limit[5],
			pulse_limit[6],
			pulse_limit[7],
			CARD_INDEX);

		MCS_SetOverTravelCheck(1, 1, 1, 1, 1, 1, 1, 1, nGroupIndex); // soft limit

		if ( !isSimulate )
		{
			VERIFY( MCS_EnableRIOSetControl(  RIO_SET0) == NO_ERR );
			VERIFY( MCS_EnableRIOSlaveControl(RIO_SET0) == NO_ERR );
		}

		// 		SetSystemState(SS_READY);
		// 		AfxGetMainWnd()->SendMessage(WM_SET_SYSTEM_STATE, SS_READY);
		int det = MCS_SetLIORoutine(LIO_ISR_Function_PMC,CARD_INDEX);
		det = MCS_SetTimer(100000);
		det = MCS_GetErrorCode();
		det = MCS_EnableTimer(CARD_INDEX);
		det = MCS_EnableTimerTrigger();

		det = MCS_SetLIOTriggerType(LIO_INT_RISE,LIO_LDI_OTP0);
		det = MCS_EnableLIOTrigger(LIO_LDI_OTP0);

		MCS_SetRIORoutine(RIO_ISR_Function_PMC, CARD_INDEX);//  link a interrupt service function for remote I/O set 0
		MCS_SetRIOTriggerType(RIO_INT_RISE, RIO_SET0, RIO_DI0, CARD_INDEX);//  set trigger mode of remote I/O set 0's digital input 0
		MCS_EnableRIOInputTrigger(RIO_SET0, RIO_DI0, CARD_INDEX);//  enable the signal from remote I/O set 0's digital input 0 to trigger a interrupt service function

        //GPIO0~7 0: INPUT, 1: OUTPUT
		MCS_SetGPIOOutputEnable(0,0,CARD_INDEX);
		MCS_SetGPIOOutputEnable(1,0,CARD_INDEX);//GPIO8~15
		MCS_SetGPIOOutputEnable(2,1,CARD_INDEX); 
		MCS_SetGPIOOutputEnable(3,1,CARD_INDEX);
		MCS_SetGPIOGateEnable(1,CARD_INDEX);

 		MCS_SetAlarmRoutine(ALARM_ISR_Function_PMC,CARD_INDEX);
// 		for ( int i = 0; i < 8; i++ )
// 		{
//  			MCS_SetAlarmTriggerType( ALM_INT_RISE, i, CARD_INDEX);
// 			MCS_EnableAlarmTrigger(i,CARD_INDEX);
//  		}

		if ( isSimulate )
		{
			double pos[] = { 0, 0, 90, 0, 0, 0, 0, 0 };
			for( unsigned int i = 0; i < axis_num; ++i )
			{
				MCS_DefinePos(i, deg2rad(pos[i]), nGroupIndex);
			}
  		}
		else
		{
			SetPositionReady(true);
		}

		return 0;
	}
	else
	{
		AfxMessageBox( _T(" 机械手控制系统初始化失败！") );
		return -1;
	}

	return 0;
 }

int PmcBase::close_system()
{
 	SetPositionReady(false);
	MCS_DisableRIOSlaveControl(RIO_SET0, CARD_INDEX);//  disable remote I/O set 0' slave node
	MCS_DisableRIOSetControl(RIO_SET0, CARD_INDEX);  //  disable remote I/O set 0
  	MCS_CloseSystem();
 	return 0;
}

int PmcBase::set_kinematicTrans( int naxis, bool bSet )
{
	int  retval;

	if ( RobotKinematics::is_main_robot_kinematics_available() )
	{
  		KINE.set_axis_num(naxis);

		if ( KINE.is_use_robot_lib() )
		{
			retval = KINE.init_robot_lib( bSet, nGroupIndex );
			return retval;
		}
		else
		{
			if (bSet)
			{
				retval = MCS_SetKinematicTrans(
					static_cast<FWDKINEFUN>(RobotKinematics::get_main_robot_kinematics().pFwdKineFuc),
					static_cast<INVKINEFUN>(RobotKinematics::get_main_robot_kinematics().pInvKineFuc),
					sizeof(EXTENSION),
					static_cast<INIT_EXTENSION>(KINE.pInitExtensionFuc),
					nGroupIndex );
  			}
			else
			{
				retval = MCS_SetKinematicTrans(NULL, NULL);
 			}
  		}
 	}
	else
	{
		retval = MCS_SetKinematicTrans(NULL, NULL);
 	}

	if( retval == NO_ERR )
	{
		if( bSet )
			MCS_SetPreProcessMotionCommandCallback( static_cast<PRE_PROCESS_MOTION_COMMAND>(KINE.pPreProcessMotionCmdFuc), nGroupIndex );
		else
			MCS_SetPreProcessMotionCommandCallback(NULL);
	}

	return retval;
}
 
const PosInfo&  PmcBase::get_position( const PosInfo::CoordinateType pct /* = mcACS */, const LocationDataSource lds /* = LDScommad */,	WORD groupIndex ) throw(HardwareException)
{
	int ret = 0;
	double dj[MAX_JOINT_NUM] = {0};

	if ( lds == LDScommad )
	{
		ret = MCS_GetCurPos( &dj[0], &dj[1], &dj[2], &dj[3],  &dj[4], &dj[5],&dj[6], &dj[7], groupIndex);
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
			if ( KINE.is_main_robot_kinematics_available() )
			{
				KINE.fwd_kine( current_rloc.axis, &current_rloc.cart );
			}
 		}
 	}
	current_rloc.pos_type = pct;
 
	return current_rloc;
}

int PmcBase::get_pos_servoCom( double* pos )
{
// 	if ( Servo::is_main_servo_available() == false )
//  		return -1;
//  	
// 	int nret = Servo::get_main_servo().read_pos_from_servo( pos ); // in rad

	int nret = -1;
	if ( MWM.is_main_world_model_available() )
	{
 		const PosInfo& spos = MWM.get_servo_position();
		AxisPos apos = spos.axis;
		pos[0] = apos.q1; pos[1] = apos.q2; pos[2] = apos.q3;
		pos[3] = apos.q4; pos[4] = apos.q5; pos[5] = apos.q6;
		pos[6] = spos.auxi.q1; pos[7] = spos.auxi.q2; 
	    
		nret = spos.valid ? 0 : -1;
	}
	
	return nret;
}
 
const int PmcBase::get_group_status(unsigned int grpIdx)
{
	return MCS_GetMotionStatus(grpIdx);
}

int PmcBase::powerGrp(bool nOnOff)
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
				VERIFY( MCS_SetServoOn(nCh, CARD_INDEX) == NO_ERR );
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
				VERIFY( MCS_SetServoOff(nCh, CARD_INDEX) == NO_ERR );
			}
 		}

		ionew.state = 0; // 2^N - 1
  	}
	MWM.set_io_information(ionew,t);
	return 0;
}
  

int PmcBase::setPosition( const CmdInfo& cf )
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
			MCS_DefinePos(i, deg2rad(pos[i]), nGroupIndex);
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
			int ret = MCS_DefinePos(i, pos[i], nGroupIndex);
		}
 	}

 	KINE.define_start_pos( axis ); // for numeric inv kinematics.

	return ret;
}

const char* PmcBase::get_error()
{
	int  nError = MCS_GetErrorCode(nGroupIndex);
	strerr = GetErrorDescription( nError );
	if ( nError != 0 )
	{
		TRACE("error = %d \n",nError);
		MCS_ClearError(nGroupIndex);
	}
 	return strerr;
}

const char*	PmcBase::GetErrorDescription(int nErrorCode)
{
 	for(int i = 0; i < ARRAY_SIZE(errors); ++i)
	{
		if( errors[i].nErrorCode == nErrorCode )
			return errors[i].lpszDesc;
	}

	return NULL;
}

int PmcBase::resetGrp()
{
	MCS_ClearError(nGroupIndex);
	for ( int i= 0; i < MAX_JOINT_NUM; i++ )
	{
		DriverAlarmFlag_PMC[i] = 0;
	}
	return 0;
}

void PmcBase::resetCmdIdx()
{
	MCS_ResetCommandIndex(nGroupIndex);
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
const MotionCoreInfo& PmcBase::get_motion_information() throw ()
{
	int CmdCount = 0;
	COMMAND_INFO  ci = {0};
	int  nError = MCS_GetErrorCode(nGroupIndex);
// 	strerr = GetErrorDescription( nError );
	WORD wEStopStatus = 0;
 	VERIFY( MCS_GetEmgcStopStatus(&wEStopStatus, CARD_INDEX) == NO_ERR );
	// 0 GMS_RUNNING:  executing a motion command
	// 1 GMS_STOP:     stopping status and none of command exist in a command queue
	// 2 GMS_HOLD:     holding status (because usig MCC_HoldMotion())  and some commands exist in a command queue
	// 3 GMS_DELAYING: delay status   (because usig MCC_DelayMotion()) and some commands exist in a command queue
	int motion_status = MCS_GetMotionStatus(nGroupIndex);

	MCS_GetCurCommand(&ci, nGroupIndex); 		//  get a current executing motion command

	MCS_GetCommandCount(&CmdCount,nGroupIndex); 		//  get motion command stocks in a motion queue

	gi.iMotionStatus = static_cast<MotionCoreInfo::MotionStatus>(motion_status);
	gi.nCmdType      = ci.nType;
	gi.nCommandIndex = ci.nCommandIndex;
	gi.dfFeedSpeed   = ci.dfFeedSpeed;
	gi.nCommandCount = CmdCount;
	gi.emergency_stop = (wEStopStatus == 1) ? true : false;

	gi.nHomeStatus   = MCS_GetGoHomeStatus();

	for ( int i=0; i < MAX_JOINT_NUM; i++ )
	{
		gi.dfPos[i] = ci.dfPos[i];
	}

	if ( !isSimulate )
	{
		DWORD alarmword = 0;
		MCS_GetAlarmInputValue(&alarmword,CARD_INDEX);
		gi.driver_alarm = alarmword;
    } 
	
	//////////////////////////////////////////////////////////////////////////

// 	if ( timeTest.elapsed_sec() > 15 )
// 	{
// 		nError = 0xF303;
// 	}
// 
// 	if ( timeTest.elapsed_sec() > 30 )
// 	{
// 		nError = 0;
// 	}
	//////////////////////////////////////////////////////////////////////////


 	if ( nError != 0 )
	{
  	    sprintf( gi.errString,"%s", GetErrorDescription( nError ) );
  	}
	else
		gi.errString[ 0 ] = '\0';

	gi.dfCurFeedSpeed = MCS_GetCurFeedSpeed(nGroupIndex);//  get current feed rate
	double axis_speed[MAX_JOINT_NUM] = {0};
	//  get current every axis's speed
	MCS_GetSpeed(&gi.dfAxisSpeed[0], &gi.dfAxisSpeed[1], &gi.dfAxisSpeed[2], &gi.dfAxisSpeed[3], 
		&gi.dfAxisSpeed[4], &gi.dfAxisSpeed[5],&gi.dfAxisSpeed[6], &gi.dfAxisSpeed[7], 
		nGroupIndex); 

//  	cout << "emgc = " << gi.emergency_stop << " err code = " << nError << " : " << std::string(gi.errString) << endl;

	const MotionCoreInfo& mc = gi;
	double maxjointvel = 0;
	for (unsigned int i = 0; i < axis_num; ++i )
	{
		double jointvel = mc.dfAxisSpeed[i];
		if ( jointvel > maxjointvel )
		{
			maxjointvel = jointvel;
		}
	}
	double currfeedvel = mc.dfCurFeedSpeed;

//   	cout << "Motion type = " << gi.nCmdType << " MaxJointVel : " << maxjointvel << " FeedSpeed = " << currfeedvel << '\n';
	return gi;
}

void _stdcall PmcBase::LIO_ISR_Function_PMC(LIOINT *pstINTSource)
{
	if (pstINTSource->OTP0)
		nLIOTriggerCount_PMC[0]++;//  local I/O 0 Trigger

	if (pstINTSource->OTP1)
		nLIOTriggerCount_PMC[1]++;//  local I/O 1 Trigger

	if (pstINTSource->OTP2)
		nLIOTriggerCount_PMC[2]++;//  local I/O 2 Trigger

	if (pstINTSource->OTP3)
		nLIOTriggerCount_PMC[3]++;//  local I/O 3 Trigger

	if (pstINTSource->OTP4)
		nLIOTriggerCount_PMC[4]++;//  local I/O 4 Trigger

	if (pstINTSource->OTP5)
		nLIOTriggerCount_PMC[5]++;//  local I/O 5 Trigger

	if (pstINTSource->OTP6)
		nLIOTriggerCount_PMC[6]++;//  local I/O 6 Trigger

	static int ic = 0;
	if (pstINTSource->HOME0)
	{
		TRACE(_T("HOME0 callback fuction %d !\n"),	ic++);
	}
}

void _stdcall PmcBase::RIO_ISR_Function_PMC(RIOINT *pstINTSource)
{
	static int ic = 0;
	if (pstINTSource->RDI0)//  check whether this ISR was triggered with a signal from set 0's digital input 0 
	{
		TRACE(_T("set 0's digital input 0 %d !\n"),	ic++);
	}
	if (pstINTSource->RDI1)//  check whether this ISR was triggered with a signal from set 0's digital input 1 
	{
		TRACE(_T("set 0's digital input 1 %d !\n"),	ic++);
	}
	if (pstINTSource->RDI2)//  check whether this ISR was triggered with a signal from set 0's digital input 2 
	{
		TRACE(_T("set 0's digital input 2 %d !\n"),	ic++);
	}
	if (pstINTSource->RDI3)//  check whether this ISR was triggered with a signal from set 0's digital input 3 
	{
		TRACE(_T("set 0's digital input 3 %d !\n"),	ic++);
	}
}

void _stdcall PmcBase::ALARM_ISR_Function_PMC(ALMINT *pstINTSource)
{
	static int ic = 0;
	 
	if (pstINTSource->ALM0)//  check whether this ISR was triggered with a signal from set 0's digital input 0 
	{
		DriverAlarmFlag_PMC[0] = 1;
		cout << "set 0's digital input 0 %d !\n";
	}
	if (pstINTSource->ALM1)//  check whether this ISR was triggered with a signal from set 0's digital input 1 
	{
		DriverAlarmFlag_PMC[1] = 1;
		cout << "set 0's digital input 1 %d !\n";
	}
	if (pstINTSource->ALM2)//  check whether this ISR was triggered with a signal from set 0's digital input 2 
	{
		DriverAlarmFlag_PMC[2] = 1;
		cout << "set 0's digital input 2 %d !\n";
	}
	if (pstINTSource->ALM3)//  check whether this ISR was triggered with a signal from set 0's digital input 3 
	{
		DriverAlarmFlag_PMC[3] = 1;
		cout << "set 0's digital input 3 %d !\n";
	}
	if (pstINTSource->ALM4)//  check whether this ISR was triggered with a signal from set 0's digital input 0 
	{
		DriverAlarmFlag_PMC[4] = 1;
		cout << "set 0's digital input 4 %d !\n";
	}
	if (pstINTSource->ALM5)//  check whether this ISR was triggered with a signal from set 0's digital input 1 
	{
		DriverAlarmFlag_PMC[5] = 1;
		cout << "set 0's digital input 5 %d !\n";
	}
	if (pstINTSource->ALM6)//  check whether this ISR was triggered with a signal from set 0's digital input 2 
	{
		DriverAlarmFlag_PMC[6] = 1;
		cout << "set 0's digital input 6 %d !\n";
	}
	if (pstINTSource->ALM7)//  check whether this ISR was triggered with a signal from set 0's digital input 3 
	{
		DriverAlarmFlag_PMC[7] = 1;
		cout << "set 0's digital input 7 %d !\n";
	}
}

int PmcBase::GetRIOInputValue(WORD *pwValue, WORD wSet, WORD wPort, WORD wCardIndex /* = 0 */)
{
	return MCS_GetRIOInputValue(pwValue,wSet,wPort,wCardIndex);
}

int PmcBase::SetRIOOutputValue(WORD wValue, WORD wSet, WORD wPort, WORD wCardIndex /* = 0 */)
{
	return MCS_SetRIOOutputValue( wValue,wSet,wPort,wCardIndex);
}

int PmcBase::EnquRIOOutputValue(WORD wBitMask, WORD wValue, WORD wSet, WORD wPort, double dfDelayTime, WORD wGroupIndex )
{
	return MCS_QueueRIOOutputValue(wBitMask,wValue,wSet,wPort,dfDelayTime,wGroupIndex);
}

int PmcBase::GetGPIOInputValue(DWORD* pdwValue,WORD wCardIndex /* = 0 */)
{
 	return MCS_GetGPIOInputValue(pdwValue,CARD_INDEX);
}

int PmcBase::SetGPIOOutputValue(DWORD dwValue, WORD wCardIndex /* = 0 */)
{
	return MCS_SetGPIOOutputValue(dwValue,CARD_INDEX);
}

int PmcBase::QueueGPIOOutputValue(DWORD dwBitMask, DWORD dwValue, double dfDelayTime /* = 0 */, WORD wGroupIndex /* = 0 */ )
{
	return MCS_QueueGPIOOutputValue( dwBitMask, dwValue, dfDelayTime, nGroupIndex );
}

int PmcBase::SetPowerAxis(int axisID, bool enable )
{
	if ( enable )
 		return MCS_SetServoOn(axisID,CARD_INDEX);
 	else
		return MCS_SetServoOff(axisID,CARD_INDEX);
}

int PmcBase::SetPositionReady( bool enable )
{
	if ( enable )
		return MCS_EnablePosReady(CARD_INDEX);
	else
		return MCS_DisablePosReady(CARD_INDEX);

	Ysbot::Time t;
	InOutObject ionew;
	ionew.object_type = InOutObject::pos_ready;
	ionew.state = enable ? 1:0;
	MWM.set_io_information(ionew,t);
}