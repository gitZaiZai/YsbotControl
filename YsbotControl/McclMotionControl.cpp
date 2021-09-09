#include "StdAfx.h"
#include "McclMotionControl.h"
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
			MotionControlFactory::get_motion_control_factory ()->sign_up (std::string("MCCL"), this);
		}
		MotionControlType* get_motion_control (const std::string&, const ConfigReader& reader, MotionControlType*) throw (std::bad_alloc) {
			return new McclMotionControl (reader);
		}
	};
	Builder the_builder;
}

McclMotionControl::McclMotionControl (const ConfigReader& reader) throw (InvalidConfigurationException) : base_epcio(reader),
	jog_mtc(reader,base_epcio),tp_mtc(reader,base_epcio)
{
 	TRACE(_T("Constuct McclMotionControl!\n")); 
}

McclMotionControl::~McclMotionControl()
{
  	//close_system(); // ???????????????????????? often throw exception
}

int McclMotionControl::init_system()
{
	return base_epcio.init_system();
 }

int McclMotionControl::close_system()
{
	TRACE(_T("close McclMotionControl!\n")); 
	return base_epcio.close_system();
}

const PosInfo& McclMotionControl::get_position( const PosInfo::CoordinateType pct/* = mcACS */, const LocationDataSource lds/* = LDScommad */ ) throw(YsbotsException)
{
	return base_epcio.get_position( pct, lds ); 
}

int McclMotionControl::setCmdInfo( const CmdInfo& cf )
{
	int ret = 0;
 	switch( cf.iCommandType )
	{
	case CMD_JOG:         ret = jog_mtc.grp_jog( cf );        break;
	case CMD_PTP:         ret = tp_mtc.move_ptp( cf );        break;
	case CMD_LIN:         ret = tp_mtc.move_line( cf );       break;
	case CMD_CIR:         ret = tp_mtc.move_cir( cf );        break;
	case CMD_SPLINE:      ret = tp_mtc.move_spline( cf );     break;
	case CMD_WAITIO:      ret = tp_mtc.wait_io(cf);           break;
	case CMD_DELAYING:    ret = tp_mtc.delaying( cf.iDelayTime );   break;
	case CMD_HALT:        ret = tp_mtc.haltGrp();                   break;
	case CMD_CONTINUE:    ret = tp_mtc.continueGrp();               break;
	case CMD_ABORT:       ret = tp_mtc.stopGrp();                   break;
	case CMD_RESET:       ret = base_epcio.resetGrp();              break;
	case CMD_POWER:       ret = base_epcio.powerGrp(cf.bOnOff);   break;
	case CMD_BLEND:       ret = tp_mtc.blendGrp(cf.bOnOff);   break;
	case CMD_SETPOSITION:   base_epcio.setPosition( cf ); break;
	case CMD_RESETCMDINDEX: base_epcio.resetCmdIdx();     break;
	default:break;
	}

	return ret;
}
 
int McclMotionControl::read_input( unsigned short* pVal, unsigned short io_type )
{
	int ret    = 0;
	if ( io_type == MCS_REMOTEIO ) // 4 words
	{
		const  IoModuleParameters& ip = base_epcio.get_mcc_io_parameters();
		int word_size  = ip.x_word_size;

		int set_idx = 0;
		for ( int i = 0; i < word_size; i++ )
		{
			set_idx = ( i >=4 ) ? 1 : 0;
			ret = base_epcio.GetRIOInputValue( pVal+i, RIO_SET0+set_idx, RIO_PORT0+i, CARD_INDEX );
		}
 	}
	else if ( io_type == MCS_LOCALIO ) // 4 words
	{
		ret = MCC_GetEmgcStopStatus( pVal, CARD_INDEX	);
		unsigned short pot = 0;
		unsigned short not = 0;
		unsigned short home_sensor = 0;
		unsigned short posTmp = 0;
		unsigned short negTmp = 0;
		unsigned short homeTmp = 0;
		
 		for ( unsigned int i = 0; i < base_epcio.axis_num; i++ )
		{
			ret = MCC_GetLimitSwitchStatus( &posTmp, 1, i, CARD_INDEX ); // OT+
			ret = MCC_GetLimitSwitchStatus( &negTmp, 0, i, CARD_INDEX ); // OT-
			ret = MCC_GetHomeSensorStatus(  &homeTmp, i, CARD_INDEX );

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
	return ret;
}

int McclMotionControl::read_word( unsigned short port, unsigned short* val, unsigned short io_type/* =0 */ )
{
	int ret     = 0;
	int set_idx = 0;

	if ( port >= 4 )
	{
		set_idx = 1;
		port -= 4;
	} 

	ret = base_epcio.GetRIOInputValue( val, RIO_SET0+set_idx, RIO_PORT0+port, CARD_INDEX );
 
	return ret;
}

int McclMotionControl::write_word( unsigned short port, unsigned short val, unsigned short io_type, bool inqueue, double dfDelayTime )
{
	const  IoModuleParameters& ip = base_epcio.get_mcc_io_parameters();
	int word_size  = ip.y_word_size;

	int ret     = 0;
	int set_idx = 0;
	
	if ( port >= 4 )
	{
		set_idx = 1;
		port -= 4;
	}
 
	ret = base_epcio.SetRIOOutputValue( val, RIO_SET0+set_idx, RIO_PORT0+port, CARD_INDEX );

	InOutObject ionew;
	ionew.object_type = InOutObject::ext_output;
	ionew.index = port;
	ionew.state = val;

	Ysbot::Time t;
	MWM.set_io_information( ionew, t );
	 
	return 0;
}