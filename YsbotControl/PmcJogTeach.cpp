#include "StdAfx.h"
#include "PmcJogTeach.h"
#include "RobotKinematics.h"
#include "WorldModel.h"
#include "MotionControl.h"
#include "CustomMotion.h"

namespace Pmc
{
 
PmcJogTeach::PmcJogTeach(const ConfigReader& reader,  PmcBase& card ) : pmc_base(card),custom_motion(card)
{
	;
}

PmcJogTeach::~PmcJogTeach(void)
{
}

void PmcJogTeach::set_teach_speed(double extVel)
{
	const GroupProperties& gp = MWM.get_group_properties();
	double  ptp_speed  = extVel; // epcio.getOverride(); // use external speed
	double  max_teach_vel = gp.profile.dfMaxExternalVel;

	if( ptp_speed > max_teach_vel )
		ptp_speed = max_teach_vel;
 
	double  feed_speed = 0.01 * ptp_speed * MCS_GetSysMaxSpeed();
	ASSERT( ptp_speed >= 0 && ptp_speed <= 100 );
	VERIFY( MCS_SetPtPSpeed( ptp_speed,  pmc_base.nGroupIndex) );
	VERIFY( MCS_SetFeedSpeed(feed_speed, pmc_base.nGroupIndex) );
	// acceleration
	VERIFY( MCS_SetAccTime(500, pmc_base.nGroupIndex) == NO_ERR );
	VERIFY( MCS_SetDecTime(500, pmc_base.nGroupIndex) == NO_ERR );
}
 
int PmcJogTeach::grp_jog( const CmdInfo& cf )
{
	int ret = 0;
	int axis = cf.chJogAxisMark;
	int dir  = cf.chJogDirMark;
	set_teach_speed(cf.dyn.JointVel);
 	
	if ( cf.iJogCoordType == jtJOINT )
	{
		ret = jog_joint_coordinate(cf);
 	}
	else
	{
		ret = jog_cartesian_coordinate(cf);
	}

	VERIFY( MCS_SetAbsolute(pmc_base.nGroupIndex) == NO_ERR );

// 	else if( cf.iJogCoordType == jtUSER )
// 	{
//   		if ( cf.cartref == NULL ) // base frame, for robot origin.
// 		{
// 			if ( cf.tool == NULL ) // For six axis end.
// 			{
// 				DriverArmAlongXYBase( nJogDir, axis );
// 			}
// 			else // for special tool set.
// 			{
// 				;
// 			}
//  		}
// 		else
// 		{
// 			// have tool reference user coordinate.
// 			if ( cf.tool == NULL )
// 			{
// 				DriverArmAlongXYBase( nJogDir, axis );
// 			}
// 			else
// 			{
// 				;
// 			}
// 		}
// 	}
// 	else if ( cf.iJogCoordType == jtTOOL )
// 	{
// 		// have tool reference user coordinate.
// 		if ( cf.tool == NULL )
// 		{
// 			DriverArmAlongTool(nJogDir,axis);
// 		}
// 		else
// 		{
// 			DriverArmAlongTool(nJogDir,axis);
//  		}
//  	}
// 
// 	VERIFY( MCS_SetAbsolute(pmc_base.nGroupIndex) == NO_ERR );

	return ret;
}

int PmcJogTeach::jog_joint_coordinate(const CmdInfo& cf)
{
	VERIFY( MCS_SetIncrease(pmc_base.nGroupIndex) == NO_ERR );
	int ret = 0;
 	int axis = cf.chJogAxisMark;

	if ( axis != 0 )
	{
		int dir  = cf.chJogDirMark;
		int nJogDir[MAX_JOINT_NUM] = {1};
		for (unsigned int i = 0; i < pmc_base.axis_num; i++ )
		{
			nJogDir[i] = ( ( 1 << i ) & dir ) ? -1 : 1;
		}

		ret = MCS_PtP_V6(
			nJogDir[0] * 10 * M_PI,
			nJogDir[1] * 10 * M_PI,
			nJogDir[2] * 10 * M_PI,
			nJogDir[3] * 10 * M_PI,
			nJogDir[4] * 10 * M_PI,
			nJogDir[5] * 10 * M_PI,
			nJogDir[6] * 10 * M_PI,
			nJogDir[7] * 10 * M_PI,
			pmc_base.nGroupIndex,
			axis );
	}

	VERIFY( MCS_SetAbsolute(pmc_base.nGroupIndex) == NO_ERR );

	return ret;
}

// int PmcJogTeach::rotate_tool_control_point( int nAxis, double theta, const CartPos* rob, const CartRef* cref, const CartPos* tool )
// {
// 	const std::vector<CartPos>& axisend_list = pose_trans.get_rotate_tcp_path( 
// 		nAxis,
// 		theta,
// 		rob, 
// 		cref,  // around world axis.
// 		tool );
// 
// // 	CmdInfo cmd;
// // 	cmd.iCommandType = CMD_BLEND;
// // 	cmd.bOnOff = true;
// // 	SMC.setCmdInfo(cmd);
// 
// 	VERIFY( MCS_SetAbsolute(pmc_base.nGroupIndex) == NO_ERR );
// 
// 	TOOLROTATE*  pTool = new TOOLROTATE;
// 	pTool->TotalPoint = axisend_list.size();
// 	pTool->theta = theta; 
// 
// 	int ret = 0;
// 	for (unsigned int i = 0; i < axisend_list.size(); ++i )
// 	{
// 		pTool->Trajectory[i].rp = axisend_list[i];
// 	}
// 
// 	return MCS_CustomMotion(
// 		ToolRotStartMotion,
// 		ToolRotInterpolation,
// 		ToolRotCleanUp,
// 		pTool,
// 		pmc_base.nGroupIndex );
// 
// }

int PmcJogTeach::jog_cartesian_coordinate( const CmdInfo& cf )
{
	int ret   = 0;
	int nAxis = cf.chJogAxisMark;
	int dir   = cf.chJogDirMark;

	int nJogDir[MAX_JOINT_NUM] = {0};
	for (unsigned int i = 0; i < pmc_base.axis_num; i++ )
	{
		nJogDir[i] = ( ( 1 << i ) & dir ) ? -1 : 1;
	}

	PosInfo cur_pos = pmc_base.get_position( PosInfo::CTmachine, LDScommad, pmc_base.nGroupIndex );
	DWORD dwFIG = (DWORD)cur_pos.cart.fig;

	Matrix rot(3,3);
	ColumnVector vecmove(3);
  
	if ( cf.iJogCoordType == jtTOOL )
	{
		CartPos tcpcart; 
		MWM.calc_tcp_pose( &cur_pos.cart, NULL, cf.tool, &tcpcart );
		Vec3D ang = tcpcart.ang;
		rot = PoseTransform::VecToRotMat(ang);
	}
	else if ( cf.iJogCoordType == jtWORLD )
	{
		rot << threebythreeident; // identity matrix
	}
	else if ( cf.iCommandType == jtUSER )
	{
		Vec3D ang = cf.cartref->cpos.ang;
		rot = PoseTransform::VecToRotMat(ang);
 	}

	switch( nAxis )
	{
	case AXIS_X: vecmove = nJogDir[0] * rot.Column(1);	break;
	case AXIS_Y: vecmove = nJogDir[1] * rot.Column(2);  break;
	case AXIS_Z: vecmove = nJogDir[2] * rot.Column(3);	break;
	case AXIS_U: vecmove = nJogDir[3] * rot.Column(1);	break;
	case AXIS_V: vecmove = nJogDir[4] * rot.Column(2);  break;
	case AXIS_W: vecmove = nJogDir[5] * rot.Column(3);	break;
	case AXIS_X | AXIS_Y: vecmove = nJogDir[0] * rot.Column(1) + nJogDir[1] * rot.Column(2);  break;
	case AXIS_X | AXIS_Z: vecmove = nJogDir[0] * rot.Column(1) + nJogDir[2] * rot.Column(3);  break;
	case AXIS_Y | AXIS_Z: vecmove = nJogDir[1] * rot.Column(2) + nJogDir[2] * rot.Column(3);  break;
	case AXIS_X | AXIS_Y | AXIS_Z: vecmove = nJogDir[0] * rot.Column(1) + nJogDir[1] * rot.Column(2) + nJogDir[2] * rot.Column(3);		break; 
	case AXIS_U | AXIS_V: vecmove = nJogDir[3] * rot.Column(1) + nJogDir[4] * rot.Column(2);  break;
	case AXIS_U | AXIS_W: vecmove = nJogDir[3] * rot.Column(1) + nJogDir[5] * rot.Column(3);  break;
	case AXIS_V | AXIS_W: vecmove = nJogDir[4] * rot.Column(2) + nJogDir[5] * rot.Column(3);  break;
	case AXIS_U | AXIS_V | AXIS_W: vecmove = nJogDir[3] * rot.Column(1) + nJogDir[4] * rot.Column(2) + nJogDir[5] * rot.Column(3);		break;
	default: break;
	}

	vecmove = columnvector_unitize( vecmove );

	VERIFY( MCS_SetIncrease(pmc_base.nGroupIndex) == NO_ERR );

	if( nAxis >= AXIS_X && nAxis < AXIS_U )
	{
		ret = MCS_Line_V6(
			vecmove(1) * 10000.0,
			vecmove(2) * 10000.0,
			vecmove(3) * 10000.0,
			0,0,0,
			0,0,
			dwFIG, // FIG
			pmc_base.nGroupIndex,
			AXIS_X | AXIS_Y | AXIS_Z
			);
		VERIFY(  ret >= 0 );
	}
	else
	{
		if ( cf.tool == NULL )
		{
			// re-adjust feed speed because the speed unit becomes radian/sec here
			double  rate = MCS_GetPtPSpeed(pmc_base.nGroupIndex);
			VERIFY( MCS_SetFeedSpeed(0.01 * rate * MAX_JOINT_SPEED, pmc_base.nGroupIndex) );
			ret = custom_motion.rbt_Cone_V6(
				vecmove(1),
				vecmove(2),
				vecmove(3),
				3600,
				pmc_base.nGroupIndex ); 
			VERIFY( ret >= 0 );
		}
		else
		{
			int mdir = 1;
			switch( nAxis )
			{
			case AXIS_U: mdir = nJogDir[3]; break;
			case AXIS_V: mdir = nJogDir[4]; break;
			case AXIS_W: mdir = nJogDir[5]; break;
			default: break;
			}
			// reference tool coordinate.
			double rotdeg = 30;
			const RobotState& rs = MWM.get_robot_state(true);
			if ( rs.mc_info.iMotionStatus == 1 ) //GMS_STOP
			{
				if ( cf.iJogCoordType == jtTOOL )
				{
					ret = custom_motion.rotate_tool_control_point( nAxis, mdir*deg2rad(rotdeg), &cur_pos.cart, NULL, cf.tool );
				}
				else if ( cf.iJogCoordType == jtWORLD )
				{
					CartRef world;
					ret = custom_motion.rotate_tool_control_point( nAxis, mdir*deg2rad(rotdeg), &cur_pos.cart, &world, cf.tool );
				}
				else if ( cf.iCommandType == jtUSER )
				{
					ret = custom_motion.rotate_tool_control_point( nAxis, mdir*deg2rad(rotdeg), &cur_pos.cart, cf.cartref, cf.tool );
				}
			}

 		}

	}

	VERIFY( MCS_SetAbsolute(pmc_base.nGroupIndex) == NO_ERR );

	return ret;
}


}