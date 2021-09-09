#include "StdAfx.h"
#include "PmcTrajectoryPlan.h"
#include "WorldModel.h"

PmcTrajectoryPlan::PmcTrajectoryPlan(const ConfigReader& reader, PmcBase& card ) : epcio(card)
{
 	;
}


PmcTrajectoryPlan::~PmcTrajectoryPlan(void)
{
	;
}

BOOL PmcTrajectoryPlan::CheckCirCommand(const double* P,  // [in] start point
	const double* Q,  // [in] pass-through point
	const double* R,  // [in] end point
	double* O,   
	double* nv)   
{
	if( (P[0] == Q[0] && P[1] == Q[1] && P[2] == Q[2]) ||
		(P[0] == R[0] && P[1] == R[1] && P[2] == R[2]) ||
		(Q[0] == R[0] && Q[1] == R[1] && Q[2] == R[2]) )
		return FALSE;
 
	double  a[3], b[3], c[3], normal_vector[3];

	vector_sub(Q, P, a);
	vector_sub(R, P, b);

	vector_cross(a, b, normal_vector);

	nv[0] = normal_vector[0];
	nv[1] = normal_vector[1];
	nv[2] = normal_vector[2];


	// find circular vector c: from start point to center of the circle
	//    c = [(b^2)(a^2 - a.b) / (2.0 * (a x b))].a +
	//        [(a^2)(b^2 - a.b) / (2.0 * (a x b))].b

	double  aa = vector_dot(a, a);
	double  bb = vector_dot(b, b);
	double  ab = vector_dot(a, b);

	// (a x b)^2 = ((a^2)(b^2) - (a.b)^2)
	double  denominator = 2.0 * (aa * bb - ab * ab);
	double  alpha       = bb * (aa - ab) / denominator;
	double  beta        = aa * (bb - ab) / denominator;

	c[0] = alpha * a[0] + beta * b[0];
	c[1] = alpha * a[1] + beta * b[1];
	c[2] = alpha * a[2] + beta * b[2];

	O[0] = P[0] + c[0];
	O[1] = P[1] + c[1];
	O[2] = P[2] + c[2];

	return TRUE;
}

bool PmcTrajectoryPlan::set_speed( const SPEED_CONFIG& psc )
{
	if( epcio.nGroupIndex >= 0 )
	{
//   		double  dfSpeedRate = 0.0001 * epcio.getOverride() * psc.dfProgSpeed;
  		double dfSpeedRate = 0.0001 * MWM.get_external_speed() * psc.dfProgSpeed;
		const GroupProperties& gp = MWM.get_group_properties();
 
		if( psc.bJointMode )
		{
			int     nDominAxis = 0;
			double  dfMaxReqTime = 0;
			double  dfMaxJSpeed[MAX_JOINT_NUM] = {0};

			for(unsigned int i = 0; i < epcio.axis_num; ++i)
			{
				// retrieve maximum joint speed
				dfMaxJSpeed[i] =  gp.mp[i].rpm * 360.0 / 60.0 / gp.mp[i].gear_ratio;

				// determine the dominant joint
				double  dfJSpeed  = dfSpeedRate * dfMaxJSpeed[i];
				double  dfReqTime = psc.dfDistance[i] / dfJSpeed;

				if( dfReqTime > dfMaxReqTime )
				{
					nDominAxis = i;
					dfMaxReqTime = dfReqTime;
				}
			}

			// determine acceleration (and deceleration) time
			double  dfSpeed = dfSpeedRate * dfMaxJSpeed[nDominAxis];
			double  dfAccel = dfSpeedRate * dfSpeedRate * gp.profile.dfMaxJAccel; 
			double  dfAccelTime = 1000 * dfSpeed / dfAccel;

			// check if distance is too short to reach the constant speed section
			if( (dfSpeed * dfSpeed / dfAccel) > psc.dfDistance[nDominAxis] )
				dfAccelTime = 1000.0 * sqrt( psc.dfDistance[nDominAxis] / dfAccel );

			if( dfAccelTime <= 0 )
				dfAccelTime = 500; // safety
			VERIFY( MCS_SetPtPSpeed(100.0 * dfSpeedRate, epcio.nGroupIndex) );
			VERIFY( MCS_SetAccTime(dfAccelTime, epcio.nGroupIndex) == NO_ERR );
			VERIFY( MCS_SetDecTime(dfAccelTime, epcio.nGroupIndex) == NO_ERR );
		}
		else
		{
			// determine acceleration (and deceleration) time
			double  dfSpeed = dfSpeedRate * gp.profile.dfMaxCSpeed;
			double  dfAccel = dfSpeedRate * dfSpeedRate * gp.profile.dfMaxCAccel;
			double  dfAccelTime = 1000.0 * dfSpeed / dfAccel;

			// check if distance is too short to reach the constant speed section
			if( (dfSpeed * dfSpeed / dfAccel) > psc.dfDistance[0] )
				dfAccelTime = 1000.0 * sqrt( psc.dfDistance[0] / dfAccel );

			if( dfAccelTime <= 0 )
				dfAccelTime = 500;

			VERIFY( MCS_SetFeedSpeed(dfSpeed,   epcio.nGroupIndex) );
			VERIFY( MCS_SetAccTime(dfAccelTime, epcio.nGroupIndex) == NO_ERR );
			VERIFY( MCS_SetDecTime(dfAccelTime, epcio.nGroupIndex) == NO_ERR );
		}
	}

	return ( (epcio.nGroupIndex >= 0) ? true: false );

}

void PmcTrajectoryPlan::set_last_axispos( const PosInfo& pi) throw()
{ 
	last_axis_pos = pi;  
}

const PosInfo& PmcTrajectoryPlan::get_last_axispos()  throw()
{
	if( epcio.nGroupIndex >= 0 && MCS_GetMotionStatus(epcio.nGroupIndex) == GMS_STOP ) 
	{ 
		last_axis_pos = epcio.get_position();
		VERIFY(  KINE.fwd_kine( last_axis_pos.axis, &last_axis_pos.cart ) == NO_ERR );
	}
	
 	return  last_axis_pos;
}

int PmcTrajectoryPlan::move_ptp( const CmdInfo& cf )
{
	PosInfo destpi    = cf.destPos;
	PosInfo last_rpos = get_last_axispos();
	ColumnVector startcol(MAX_JOINT_NUM);
	startcol = last_rpos.axis.toColumnVector();
	startcol(7) = last_axis_pos.auxi.q1;
	startcol(8) = last_axis_pos.auxi.q2;

	if ( cf.ovl.iBufferMode == Overlap::mcBlendingPrevious )
	{
		blendGrp(true);
	}
	else if( cf.ovl.iBufferMode == Overlap::mcBuffered )
	{
		blendGrp(false);
	}

  	// speed & acceleration settings
	SPEED_CONFIG  sc;
	double vel = cf.dyn.JointVel;
	if ( vel > 100 )
 		vel = 100;
 	if ( vel < 0.01 )
 		vel = 0.01;
 	sc.dfProgSpeed = vel;
	sc.bJointMode  = TRUE;
 
	if( cf.destPos.pos_type == PosInfo::CTmachine )
	{
		CartPos axisendpos;
		pose_transform.calc_axisend_pose( &destpi.cart, cf.cartref, cf.tool, &axisendpos );
		destpi.cart = axisendpos;
 		int invres = KINE.inv_kine( destpi.cart, &(destpi.axis) );
		if ( invres != 0 )
		{
			cerr << " ERR : destination not reach.\n";
			return invres;
		}
	}
	else if ( cf.destPos.pos_type == PosInfo::CTaxis )
	{
		VERIFY(  KINE.fwd_kine( destpi.axis, &destpi.cart ) == NO_ERR );
	}

 	ColumnVector destcol(MAX_JOINT_NUM);
	destcol = destpi.axis.toColumnVector();
	destcol(7) = destpi.auxi.q1;
	destcol(8) = destpi.auxi.q2;

	for( unsigned int i = 0; i < epcio.axis_num; ++i )
		sc.dfDistance[i] = fabs( rad2deg(destcol(i+1)) - rad2deg( startcol(i+1) ) );

	set_speed( sc );

 	// update last target positions
  	set_last_axispos( destpi );

	switch(cf.ovl.iBufferMode)
	{
	case Overlap::mcBlendingPrevious: blendGrp(true); break;
	case Overlap::mcBuffered:         blendGrp(false); break;
	case Overlap::mcAborting:         stopGrp(); break;
	default: break;
	}
 
	// launch motion command
	return MCS_PtP_V6(
		destcol(1),
		destcol(2),
		destcol(3),
		destcol(4),
		destcol(5),
		destcol(6),
		destcol(7),
		destcol(8),
		epcio.nGroupIndex);
 }

int PmcTrajectoryPlan::move_line( const CmdInfo& cf)
{
 	PosInfo destpi    = cf.destPos;
	PosInfo last_rpos = get_last_axispos();

	if ( cf.ovl.iBufferMode == Overlap::mcBlendingPrevious )
	{
		blendGrp(true);
	}
	else if( cf.ovl.iBufferMode == Overlap::mcBuffered )
	{
		blendGrp(false);
	}
	// calculate traveling distance in Cartesian coordinate
 	DWORD   dwFIG;
 
	if( cf.destPos.pos_type == PosInfo::CTmachine )
	{
		CartPos axisendpos;
		pose_transform.calc_axisend_pose( &destpi.cart, cf.cartref, cf.tool, &axisendpos );
		destpi.cart = axisendpos;

  		int invres = KINE.inv_kine( destpi.cart, &(destpi.axis) );
		if ( invres != 0 )
 			return invres;
  	}
	else if ( cf.destPos.pos_type == PosInfo::CTaxis )
	{
  		VERIFY(  KINE.fwd_kine( destpi.axis, &destpi.cart ) == NO_ERR );
   	}

// update last target positions
 	set_last_axispos( destpi );
  	
	dwFIG = destpi.cart.fig;

	// speed & acceleration settings
	SPEED_CONFIG  sc;
// 	sc.dfProgSpeed = cf.dyn.PathVel;
	double vel = cf.dyn.JointVel;
	if ( vel > 100 )
		vel = 100;
	if ( vel < 0.01 )
		vel = 0.01;
	sc.dfProgSpeed = vel;

	sc.bJointMode  = FALSE;
 	
 	// 计算空间距离，用于决定速度参数
	sc.dfDistance[0] = sqrt( (destpi.cart.pos - last_rpos.cart.pos).squared_length() );

	set_speed( sc );

	switch(cf.ovl.iBufferMode)
	{
	case Overlap::mcBlendingPrevious: blendGrp(true); break;
	case Overlap::mcBuffered:         blendGrp(false); break;
	case Overlap::mcAborting:         stopGrp(); break;
	default: break;
	}
 
	return MCS_Line_V6(
			destpi.cart.pos.x,
			destpi.cart.pos.y,
			destpi.cart.pos.z,
			destpi.cart.ang.x,
			destpi.cart.ang.y,
			destpi.cart.ang.z,
			destpi.auxi.q1,
			destpi.auxi.q2,
			dwFIG,
			epcio.nGroupIndex );
 
}


int PmcTrajectoryPlan::wait_io( const CmdInfo& cf)
{
	return 0;
}

int PmcTrajectoryPlan::move_cir( const CmdInfo& cf)
{
	PosInfo destpi    = cf.destPos;
	PosInfo mediumpi  = cf.auxPos;
	PosInfo last_rpos = get_last_axispos();

	if ( cf.ovl.iBufferMode == Overlap::mcBlendingPrevious )
	{
		blendGrp(true);
	}
	else if( cf.ovl.iBufferMode == Overlap::mcBuffered )
	{
		blendGrp(false);
	}
	// calculate traveling distance in Cartesian coordinate
	DWORD   dwFIG;

	if( cf.destPos.pos_type == PosInfo::CTmachine )
	{
		CartPos axisendpos;
		pose_transform.calc_axisend_pose( &destpi.cart, cf.cartref, cf.tool, &axisendpos );
		destpi.cart = axisendpos;

		int invres = KINE.inv_kine( destpi.cart, &(destpi.axis) );
		if ( invres != 0 )
			return invres;
	}
	else if ( cf.destPos.pos_type == PosInfo::CTaxis )
	{
		VERIFY(  KINE.fwd_kine( destpi.axis, &destpi.cart ) == NO_ERR );
	}

	if( cf.auxPos.pos_type == PosInfo::CTmachine )
	{
		CartPos axisendpos;
		pose_transform.calc_axisend_pose( &mediumpi.cart, cf.cartref, cf.tool, &axisendpos );
		mediumpi.cart = axisendpos;

		int invres = KINE.inv_kine( mediumpi.cart, &(mediumpi.axis) );
		if ( invres != 0 )
			return invres;
	}
	else if ( cf.auxPos.pos_type == PosInfo::CTaxis )
	{
		VERIFY(  KINE.fwd_kine( mediumpi.axis, &mediumpi.cart ) == NO_ERR );
	}
 
	// speed & acceleration settings
	SPEED_CONFIG  sc;
// 	sc.dfProgSpeed = cf.dyn.PathVel;

	double vel = cf.dyn.JointVel;
	if ( vel > 100 )
		vel = 100;
	if ( vel < 0.01 )
		vel = 0.01;
	sc.dfProgSpeed = vel;

	sc.bJointMode  = FALSE;

	// calculate traveling distance
	double  dfCenter[3], nv[3];
	double  dfCPosLast[6], dfCPosMedium[6], dfCPosTarget[6];
	dfCPosLast[0] = last_rpos.cart.pos.x;
	dfCPosLast[1] = last_rpos.cart.pos.y;
	dfCPosLast[2] = last_rpos.cart.pos.z;
	dfCPosLast[3] = last_rpos.cart.ang.x;
	dfCPosLast[4] = last_rpos.cart.ang.y;
	dfCPosLast[5] = last_rpos.cart.ang.z;

	dfCPosMedium[0] = mediumpi.cart.pos.x;
	dfCPosMedium[1] = mediumpi.cart.pos.y;
	dfCPosMedium[2] = mediumpi.cart.pos.z;
	dfCPosMedium[3] = mediumpi.cart.ang.x;
	dfCPosMedium[4] = mediumpi.cart.ang.y;
	dfCPosMedium[5] = mediumpi.cart.ang.z;

	dfCPosTarget[0] = destpi.cart.pos.x;
	dfCPosTarget[1] = destpi.cart.pos.y;
	dfCPosTarget[2] = destpi.cart.pos.z;
	dfCPosTarget[3] = destpi.cart.ang.x;
	dfCPosTarget[4] = destpi.cart.ang.y;
	dfCPosTarget[5] = destpi.cart.ang.z;

	if( CheckCirCommand(dfCPosLast, dfCPosMedium, dfCPosTarget, dfCenter, nv) )
	{
		vector_unitize(nv, nv);
		// radius
		double  dfCenterVect[3] = {
			dfCenter[0] - dfCPosLast[0],
			dfCenter[1] - dfCPosLast[1],
			dfCenter[2] - dfCPosLast[2]
		};
		double  dfRadius = sqrt( vector_dot(dfCenterVect, dfCenterVect) );

		// Up
		vector_unitize(dfCenterVect, dfCenterVect);

		// U1
		double  dfBaseVect[3];
		vector_cross(dfCenterVect, nv, dfBaseVect);
		vector_unitize(dfBaseVect, dfBaseVect);

		// phi (rotation angle)
		double  delta[3] = {
			dfCPosTarget[0] - dfCPosLast[0],
			dfCPosTarget[1] - dfCPosLast[1],
			dfCPosTarget[2] - dfCPosLast[2]
		};
		double  PR_length = sqrt( vector_dot(delta, delta) );

		// a (dot) b = |a|.|b|.cos(theta)
		double  theta = acos( vector_dot(delta, dfBaseVect) / PR_length );
		ASSERT( theta > 0 && theta <= M_PI );

		double  phi = 2 * theta;

		// Distance
		sc.dfDistance[0] = dfRadius * phi;

		set_speed( sc );
 
		// update last target positions
		set_last_axispos( destpi );

		dwFIG = destpi.cart.fig;

		// launch motion command
		return MCS_Arc_V6(
			dfCPosMedium[0],
			dfCPosMedium[1],
			dfCPosMedium[2],
			dfCPosTarget[0],
			dfCPosTarget[1],
			dfCPosTarget[2],
			dfCPosTarget[3],
			dfCPosTarget[4],
			dfCPosTarget[5],
			dwFIG,
			epcio.nGroupIndex );
	}
	else
		return -1;

	return 0;
}

int PmcTrajectoryPlan::move_spline( const CmdInfo& cf)
{
	return 0;
}

int PmcTrajectoryPlan::blendGrp(bool nOnOff)
{
 	bool  bCurrentBlendStatus = (MCS_CheckBlend(epcio.nGroupIndex) == NO_ERR);

	if( ( nOnOff && !bCurrentBlendStatus) ||
		(!nOnOff &&  bCurrentBlendStatus) )
	{
		if( nOnOff )
		{
			MCS_EnableBlend( epcio.nGroupIndex);
			cout << " ---------- blend motion start -----" << '\n';
		}
		else
		{
			MCS_DisableBlend(epcio.nGroupIndex);
			cout << " ---------- blend motion end -----" << '\n';
		}
	}
 
	return 0;
}

// in ms
int PmcTrajectoryPlan::delaying(long int dt /* = 100 */)
{
 	return MCS_DelayMotion( (DWORD)dt, epcio.nGroupIndex );
}

int PmcTrajectoryPlan::haltGrp()
{
	return MCS_HoldMotion( epcio.nGroupIndex, FALSE );
}

int PmcTrajectoryPlan::continueGrp()
{
	return MCS_ContiMotion(epcio.nGroupIndex);
}

int PmcTrajectoryPlan::stopGrp(const double dec/* =500 */)
{
 	epcio.resetCmdIdx();

	const double min_ext_vel  = 10.0;
	const double max_ext_vel  = 100.0;
	const double min_dec_time = 100.0;
	const double max_dec_time = 500.0;

	const MotionCoreInfo& mc = epcio.get_motion_information();
	const GroupProperties& gp = MWM.get_group_properties();
	double currfeedvel = mc.dfCurFeedSpeed;
	double dectime = 500;
	double velaxis[MAX_JOINT_NUM]={0};

	double maxjointvel = 0;
	unsigned int axisid = 0;

	if ( mc.nCmdType == 0 ) // PTP ??????????????????????
	{
		for (unsigned int i = 0; i < gp.axis_num; ++i )
		{
			double jointvel = mc.dfAxisSpeed[i];
			if ( jointvel > maxjointvel )
			{
				maxjointvel = jointvel;
				axisid = i;
			}
		}
		dectime = ( rad2deg( maxjointvel ) / gp.profile.dfMaxJAccel) * 1e3 + ( 1 + 1.0/(static_cast<double>(axisid)+1) )*10;
 	}
	else if ( 0 < mc.nCmdType && mc.nCmdType < 6 ) // CP
	{
  		dectime = ( currfeedvel / gp.profile.dfMaxCSpeed) * 1e3;
  	}
	double dectimepower = dectime * dectime;
// 	double curr_vel = MWM.get_external_speed();
// 	double krate    = GetLinearFuzzyValue(curr_vel,min_ext_vel,max_ext_vel);
// 	double dec_time = krate * (max_dec_time-min_dec_time) + min_dec_time;


// 	cout << "dec_time = " << dec_time << '\n';

//  	cout << "speed = " << currfeedvel << " stop dec time = " << dectimepower << '\n';

	return MCS_AbortMotionEx( dectimepower, epcio.nGroupIndex );
}

double PmcTrajectoryPlan::GetLinearFuzzyValue(const double& value, const double& left, const double& right)
{
 	if ( value < left )  
		return 0.0;
	else if (value > right) 
		return 1.0;
	else 
		return (value - left) / (right - left);
	 
}
