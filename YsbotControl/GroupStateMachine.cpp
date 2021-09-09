#include "StdAfx.h"
#include "GroupStateMachine.h"
#include "MotionControl.h" 

GroupStateMachine::GroupStateMachine(void)
{
	current = GrpDisable;
	isWait = false;
	isHold = false;
	isDelaying = false;
}
 
GroupStateMachine::~GroupStateMachine(void)
{
}

const GroupStateMachine& GroupStateMachine::operator= (const GroupStateMachine& src) throw () 
{
	current = src.current;
	 
	return (*this);
}

GroupState GroupStateMachine::get_state() const throw ()
{
	return current;
}

void GroupStateMachine::set_state(GroupState gs) throw ()
{
	current = gs;
}

void GroupStateMachine::update( const CmdInfo& cf ) throw ()
{
// 	if ( last_cmd.iCommandType != CMD_ABORT )
	{
		last_cmd = cf;
 	}

 	switch( cf.iCommandType )
	{
	case CMD_JOG:          
	case CMD_PTP:       
	case CMD_LIN:          
	case CMD_CIR:          
	case CMD_SPLINE:
		if ( current ==  GrpStandby )
		{
			current = GrpMoving;
 		}
		break;
	case CMD_DELAYING:
// 		if ( current == GrpMoving || current == GrpStandby )
		if ( SMC.isCodeSysMotion )
		{
 			if ( !isDelaying )
			{
				isDelaying = true;
			}
			else
				isDelaying = false;
 		}
		else
		{
			if ( current == GrpStandby )
			{
				current = GrpDelaying;
				isDelaying = true;
			}
			else if ( current == GrpDelaying )
			{
				isDelaying = false;
			}
		}
		 
		break;
	case CMD_WAITIO: // before after
// 	case CMD_TIMEDELAY:
 		if ( current == GrpStandby )
		{
			current = GrpWait;
			isWait = true;
 		}
		else if ( current == GrpWait )
		{
			isWait = false;
		}
  		break;
	case CMD_HALT: 
		cout << "---------------hold command !!!!!!!!!!!!!!!!!!!!!!!!!! "<< '\n';
		if ( current == GrpMoving )
		{
			current = GrpStopping;
		}
		else if ( current == GrpWait ) 
		{
			current = GrpHold;
			isHold = true;
		}
 		break;
	case CMD_CONTINUE:
		if ( current == GrpHold )
		{
			isHold = false;
			//current = lastGs;
			if ( isWait )
			{
				current = GrpWait;
			}
			else if ( isDelaying )
			{
				current = GrpDelaying;
			}
			else
				current = GrpStandby;
 		}            
		break;
	case CMD_ABORT: 
		isWait = false;
		isHold = false;
		isDelaying = false;
		last_cmd.iCommandType = CMD_NULL;
		if ( current == GrpMoving )
		{
			current = GrpStopping;
		}
		else if ( current == GrpHold || current == GrpWait || current == GrpDelaying )
		{
			current = GrpStandby;
		}
 		break;
	case CMD_RESET:  
		if ( current == GrpErrorStop )
		{
			current = GrpDisable;
 		}
		last_cmd.iCommandType = CMD_NULL;
		break;
	case CMD_POWER:
		if ( cf.bOnOff == false )
		{
			current = GrpDisable; 
		}
		else if( cf.bOnOff == true )
		{
			if ( current != GrpErrorStop )
			{
				current = GrpStandby;
			}
 		}
		break;
 	case CMD_ERRORSTOP:
		current = GrpErrorStop; 
		last_cmd.iCommandType = CMD_NULL;
		break;
	default: break;
	}
 
}

/************************************************************************/
/*   GMS_RUNNING					0
     GMS_STOP						1
     GMS_HOLD						2
     GMS_DELAYING					3
     GMS_BLOCKHOLD                  4
     GMS_MPGING                     5                                   */
/************************************************************************/

void GroupStateMachine::update( MotionCoreInfo::MotionStatus motion_state ) throw ()
{
	if ( current == GrpErrorStop || current == GrpDisable )
 		return;
    
	CmdInfo cmd;
	if ( current == GrpStopping )
	{
		switch ( motion_state ) //??????????????????????????????
		{
		case MotionCoreInfo::msRUNNING: 
			if ( last_cmd.iCommandType == CMD_HALT )
			{
				cmd.iCommandType = CMD_HALT;   
				SMC.setCmdInfo(cmd); 
			}
			break;
		case MotionCoreInfo::msSTOP:  
			current = GrpStandby;
 			break;
		case MotionCoreInfo::msHOLD:  
			current = GrpHold; 
			isHold = true;    
			break;
		case MotionCoreInfo::msDELAYING: 
			break;
 		default: break;
  		}
 	}
	else if ( current == GrpHold ) 
	{
		if ( isHold == false )
		{
			current = static_cast<GroupState>(motion_state);
 		}	
	}
	else if(current == GrpWait)
	{
		if ( isWait == false )
		{
			current = static_cast<GroupState>(motion_state);
		}	
	}
	else if ( current == GrpStandby )
	{
		// 在两条动作指令之间，有IO等操作指令同步执行顺序时，发出hold指令有可能失效，
		// 因此，等内核的状态变成静止时，判断上个指令是否为hold，若是，且下个状态变为运动状态，
		// 则重发hold指令，停止运动。如果是程序的末尾，则不需重发，直接结束程序，跳过hold状态。
		if ( motion_state == MotionCoreInfo::msRUNNING && last_cmd.iCommandType == CMD_HALT )
		{
			current = GrpStopping;
			cmd.iCommandType = CMD_HALT;   
			SMC.setCmdInfo(cmd); 
		}
		else
		   current = static_cast<GroupState>(motion_state);
 	}
 	else 
	{
		current = static_cast<GroupState>(motion_state);
  	}

}
