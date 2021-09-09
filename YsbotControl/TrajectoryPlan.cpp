#include "StdAfx.h"
#include "TrajectoryPlan.h"
#include "WorldModel.h"

TrajectoryPlan::TrajectoryPlan(const ConfigReader& reader, EpcioBase& card ) : epcio(card)
{
 	;
}


TrajectoryPlan::~TrajectoryPlan(void)
{
	;
}

BOOL TrajectoryPlan::CheckCirCommand(const double* P,  // [in] start point
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

bool TrajectoryPlan::set_speed( const SPEED_CONFIG& psc )
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
			VERIFY( MCC_SetPtPSpeed(100.0 * dfSpeedRate, epcio.nGroupIndex) );
			VERIFY( MCC_SetAccTime(dfAccelTime, epcio.nGroupIndex) == NO_ERR );
			VERIFY( MCC_SetDecTime(dfAccelTime, epcio.nGroupIndex) == NO_ERR );
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

			VERIFY( MCC_SetFeedSpeed(dfSpeed,   epcio.nGroupIndex) );
			VERIFY( MCC_SetAccTime(dfAccelTime, epcio.nGroupIndex) == NO_ERR );
			VERIFY( MCC_SetDecTime(dfAccelTime, epcio.nGroupIndex) == NO_ERR );
		}
	}

	return ( (epcio.nGroupIndex >= 0) ? true: false );

}

void TrajectoryPlan::set_last_axispos( const PosInfo& pi) throw()
{ 
	last_axis_pos = pi;  
}

const PosInfo& TrajectoryPlan::get_last_axispos()  throw()
{
	if( epcio.nGroupIndex >= 0 && MCC_GetMotionStatus(epcio.nGroupIndex) == GMS_STOP ) 
	{ 
		last_axis_pos = epcio.get_position();
		VERIFY(  KINE.fwd_kine( last_axis_pos.axis, &last_axis_pos.cart ) == NO_ERR );
	}

 	return  last_axis_pos;
}

int TrajectoryPlan::move_ptp( const CmdInfo& cf )
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
	double vel = 10;
	if ( cf.dyn.JointVel > 100 )
 		vel = 100;
 	if ( cf.dyn.JointVel < 0.01 )
 		vel = 0.01;
 	sc.dfProgSpeed = vel;
	sc.bJointMode  = TRUE;
 
	if( cf.destPos.pos_type == PosInfo::CTmachine )
	{
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
	return MCC_PtP_V6(
		destcol(1),
		destcol(2),
		destcol(3),
		destcol(4),
		destcol(5),
		destcol(6),
		epcio.nGroupIndex);
}

int TrajectoryPlan::move_line( const CmdInfo& cf)
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
	sc.dfProgSpeed = cf.dyn.PathVel;
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

	return MCC_Line_V6(
		destpi.cart.pos.x,
		destpi.cart.pos.y,
		destpi.cart.pos.z,
		destpi.cart.ang.x,
		destpi.cart.ang.y,
		destpi.cart.ang.z,
		dwFIG,
		epcio.nGroupIndex );
 
}


int TrajectoryPlan::wait_io( const CmdInfo& cf)
{
	return 0;
}

int TrajectoryPlan::move_cir( const CmdInfo& cf)
{
	return 0;
}

int TrajectoryPlan::move_spline( const CmdInfo& cf)
{
	return 0;
}

int TrajectoryPlan::blendGrp(bool nOnOff)
{
	bool  bCurrentBlendStatus = (MCC_CheckBlend(epcio.nGroupIndex) == NO_ERR);

	if( ( nOnOff && !bCurrentBlendStatus) ||
		(!nOnOff &&  bCurrentBlendStatus) )
	{
		if( nOnOff )
			MCC_EnableBlend( epcio.nGroupIndex);
		else
			MCC_DisableBlend(epcio.nGroupIndex);
	}
	return 0;
}

// in ms
int TrajectoryPlan::delaying(long int dt /* = 100 */)
{
	return MCC_DelayMotion( (DWORD)dt, epcio.nGroupIndex );
}

int TrajectoryPlan::haltGrp()
{
	return MCC_HoldMotion( epcio.nGroupIndex, FALSE );
}

int TrajectoryPlan::continueGrp()
{
	return MCC_ContiMotion(epcio.nGroupIndex);
}

int TrajectoryPlan::stopGrp(const double dec/* =500 */)
{
	return MCC_AbortMotionEx( dec, epcio.nGroupIndex );
}