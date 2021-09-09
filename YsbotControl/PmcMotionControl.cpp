#include "StdAfx.h"
#include "PmcMotionControl.h"
#include "MotionControlFactory.h"
#include "YsibotsException.h"
#include "Journal.h"
#include "WorldModel.h"
#include "random.h"

namespace
{
	class Builder : public MotionControlBuilder 
	{
		static Builder the_builder;
	public:
		Builder () {
			MotionControlFactory::get_motion_control_factory ()->sign_up (std::string("PMC"), this);
		}
		MotionControlType* get_motion_control (const std::string&, const ConfigReader& reader, MotionControlType*) throw (std::bad_alloc) {
			return new PmcMotionControl (reader);
		}
	};
	Builder the_builder;
}

PmcMotionControl::PmcMotionControl (const ConfigReader& reader) throw (InvalidConfigurationException) :base_pmc(reader),
	jog_pmc(reader,base_pmc),tp_pmc(reader,base_pmc)
{
 	TRACE(_T("Constuct PmcMotionControl!\n")); 
}

PmcMotionControl::~PmcMotionControl()
{
//   	close_system(); // ???????????????????????? often throw exception
}

int PmcMotionControl::init_system()
{
	return base_pmc.init_system();
}

int PmcMotionControl::close_system()
{
	TRACE(_T("close PmcMotionControl!\n")); 
	return base_pmc.close_system();
}

const PosInfo& PmcMotionControl::get_position( const PosInfo::CoordinateType pct/* = mcACS */, const LocationDataSource lds/* = LDScommad */ ) throw(YsbotsException)
{
	return base_pmc.get_position( pct, lds ); 
}

int PmcMotionControl::setCmdInfo( const CmdInfo& cf )
{
	int ret = 0;
 	switch( cf.iCommandType )
	{
	case CMD_JOG:         ret = jog_pmc.grp_jog( cf );        break;
	case CMD_PTP:         ret = tp_pmc.move_ptp( cf );        break;
	case CMD_LIN:         ret = tp_pmc.move_line( cf );       break;
	case CMD_CIR:         ret = tp_pmc.move_cir( cf );        break;
	case CMD_SPLINE:      ret = tp_pmc.move_spline( cf );     break;
	case CMD_WAITIO:      ret = tp_pmc.wait_io(cf);           break;
	case CMD_DELAYING:    ret = tp_pmc.delaying( cf.iDelayTime );   break;
	case CMD_HALT:        ret = tp_pmc.haltGrp();                   break;
	case CMD_CONTINUE:    ret = tp_pmc.continueGrp();               break;
	case CMD_ABORT:       ret = tp_pmc.stopGrp(cf.dec_time);                   break;
	case CMD_RESET:       ret = base_pmc.resetGrp();                  break;
	case CMD_POWER:       ret = base_pmc.powerGrp(cf.bOnOff);   break;
	case CMD_BLEND:       ret = tp_pmc.blendGrp(cf.bOnOff);   break;
	case CMD_SETPOSITION:   base_pmc.setPosition( cf ); break;
	case CMD_RESETCMDINDEX: base_pmc.resetCmdIdx();     break;
	case CMD_TIMEDELAY: MCS_TimeDelay(cf.iDelayTime); break;
	default:break;
	}

	return ret;
}

//////////////////////////////////////////////////////////////////////////
//  called by child thread, use to show io state in UI.

int PmcMotionControl::read_input( unsigned short* pVal, unsigned short io_type )
{
	int ret    = 0;
	if ( io_type == MCS_REMOTEIO ) // 4 words
	{
		const  IoModuleParameters& ip = base_pmc.get_mcc_io_parameters();
		int word_size  = ip.x_word_size;

		int set_idx = 0;
		for ( int i = 0; i < word_size; i++ )
		{
			set_idx = ( i >=4 ) ? 1 : 0;
			ret = base_pmc.GetRIOInputValue( pVal+i, RIO_SET0+set_idx, RIO_PORT0+i, CARD_INDEX );
		}
 	}
	else if ( io_type == MCS_LOCALIO ) // 4 words
	{
 		ret = MCS_GetEmgcStopStatus( pVal, CARD_INDEX	);
 
		unsigned short pot = 0;
		unsigned short not = 0;
		unsigned short home_sensor = 0;
		unsigned short posTmp = 0;
		unsigned short negTmp = 0;
		unsigned short homeTmp = 0;
		
 		for ( unsigned int i = 0; i < base_pmc.axis_num; i++ )
		{
 			ret = MCS_GetLimitSwitchStatus( &posTmp, 1, i, CARD_INDEX ); // OT+
			ret = MCS_GetLimitSwitchStatus( &negTmp, 0, i, CARD_INDEX ); // OT-
			ret = MCS_GetHomeSensorStatus(  &homeTmp, i, CARD_INDEX );
 
			//////////////////// test/////////////////////////////////////////////////////

// 			posTmp = urandom() > 0.5 ? 1 : 0;
// 			negTmp = urandom() > 0.5 ? 1 : 0;
// 			homeTmp = urandom() > 0.5 ? 1 : 0;
			//////////////////// test/////////////////////////////////////////////////////

			if ( posTmp == 1 )  SETBIT(pot,i);
			else  			    CLRBIT(pot,i);

			if ( negTmp == 1 )  SETBIT(not,i);
			else  			    CLRBIT(not,i);

			if ( homeTmp == 1 )  SETBIT(home_sensor,i);
			else  			     CLRBIT(home_sensor,i);

 		}
		*(pVal+1) = pot;
		*(pVal+2) = not;
		*(pVal+3) = home_sensor;
	}
	else if ( io_type == MCS_GPIO )
	{
		DWORD dwInput = 0;
 		ret = base_pmc.GetGPIOInputValue( &dwInput );
// 		dwInput = 0x0000ff88;
// 		printf("gpio input =  %8lX \n",dwInput);
		if ( ret == NO_ERR )
		{
			*pVal = LOWORD(~dwInput);
		}
	}
	return ret;
}

//////////////////////////////////////////////////////////////////////////
// use to operator IO while program run.

int PmcMotionControl::read_word( unsigned short port, unsigned short* val, unsigned short io_type/* =0 */ )
{
	int ret     = 0;
	int set_idx = 0;

	if ( io_type == MCS_GPIO )
	{
		DWORD dwInput = 0;
		ret = base_pmc.GetGPIOInputValue( &dwInput );
 		if ( ret == NO_ERR )
 			*val = LOWORD(~dwInput);
 	}
	else if( io_type == MCS_REMOTEIO )
	{
		if ( port >= 4 )
		{
			set_idx = 1;
			port -= 4;
		} 

		ret = base_pmc.GetRIOInputValue( val, RIO_SET0+set_idx, RIO_PORT0+port, CARD_INDEX );
 	}
	else if ( io_type == MCS_LOCALIO ) // 0~7 POT+, 10~17 NOT-, 20~27 HOME
	{
		int bitPos = 0;
		unsigned short localio[4] = {0};
		memset(localio,0,sizeof(localio));
		read_input( localio, io_type );

		for ( int i = 2; i >= 0; --i )
		{
			bitPos = port - i*10;
			if ( bitPos >= 0 )
			{
				*val = GETBIT( localio[i+1], bitPos );
				break;
			}
		}
 	}
 
	return ret;
}

int PmcMotionControl::write_word( unsigned short port, unsigned short val, unsigned short io_type, bool inqueue, double dfDelayTime )
{
	int ret     = 0;
	int set_idx = 0;

// 	InOutObject ionew;
// 	unsigned short valTemp = 0;
// 	const InOutObjectList& old = MWM.get_io_objects(IOSmotioncard);

	if ( io_type == MCS_GPIO )
	{
 		WORD lw = 0;
		WORD hw = val;
		WORD bitMask = 1 << port;
		DWORD dwValue = MAKELONG(lw,hw);

		if ( inqueue )
		{
// 			printf("port %d, queue output gpio = %#8X\n", port, dwValue );
   			ret = base_pmc.QueueGPIOOutputValue( 0XFFFFFFFF, dwValue, dfDelayTime );
 		}
		else
		{
		   ret = base_pmc.SetGPIOOutputValue(dwValue);
		}
 	}
	else if ( io_type == MCS_REMOTEIO ) // ?????????????????????????
	{
		const  IoModuleParameters& ip = base_pmc.get_mcc_io_parameters();
		int word_size  = ip.y_word_size;

		if ( port >= 4 )
		{
			set_idx = 1;
			port -= 4;
		}

		if ( inqueue )
		{
			ret = base_pmc.EnquRIOOutputValue(0XFFFF,val,RIO_SET0+set_idx, RIO_PORT0+port,dfDelayTime);
		}
		else
		    ret = base_pmc.SetRIOOutputValue( val, RIO_SET0+set_idx, RIO_PORT0+port, CARD_INDEX );

// 		ionew.object_type = InOutObject::ext_output;
// 		ionew.index = port;
// 		ionew.state = val;
// 
// 		Ysbot::Time t;
// 		MWM.set_io_information( ionew, t );
	}
	else if ( io_type == MCS_LOCALIO ) // servo on, posRdy
	{
		int nBit = port - 10;
		bool bEnable = (val==1) ? true:false;
		if ( nBit >= 0 )
		{
			base_pmc.SetPositionReady( bEnable );
		}
		else
		{
			base_pmc.SetPowerAxis( port, bEnable );
		}
 	}
	 
	return ret;
}