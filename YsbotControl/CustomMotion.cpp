#include "stdafx.h"
#include "CustomMotion.h"
#include "RobotKinematics.h"

//////////////// custom motion /////////////////////////////////////////
// Callback functions
namespace Pmc 
{ 

double __stdcall ConeStartMotion(int nGroup,
	int nSynAxisNum,
	int nCommandIndex,
	BOOL bAbsolute,
	const double* pdfStartPos,
	void* pvBuffer)
{
	ASSERT( nSynAxisNum == 8 && pvBuffer );

	double DH_d6 = 80; // need get data from kine module.
	if ( RobotKinematics::is_main_robot_kinematics_available() )
	{
		DH_d6 = KINE.get_link_dh(6).d;
	}

	CONE  *pCone = (CONE*) pvBuffer;

	// retrieve the wrist center	
	ORIENTATION  orient;
	TRANSFORMATION_RULES  tr;

	// valid orientation range: (-180) ~ [180]
	orient.rx = valid_angle(pdfStartPos[3], -M_PI, M_PI);
	orient.ry = valid_angle(pdfStartPos[4], -M_PI, M_PI);
	orient.rz = valid_angle(pdfStartPos[5], -M_PI, M_PI);

	// retrieve transformation matrix
	orient2tr(orient, &tr);

	// wrist center
	double  px = pdfStartPos[0] - DH_d6 * tr.W1;
	double  py = pdfStartPos[1] - DH_d6 * tr.W2;
	double  pz = pdfStartPos[2] - DH_d6 * tr.W3;

	// compute the center point that the wrist center encircles :: lrd need change 
	double  t =
		( pCone->vec_normal[0] * (px - pdfStartPos[0]) + // use tool pos
		pCone->vec_normal[1] * (py - pdfStartPos[1]) +
		pCone->vec_normal[2] * (pz - pdfStartPos[2]) );

	double  center[3] = {
		pdfStartPos[0] + pCone->vec_normal[0] * t,  // use tool pos
		pdfStartPos[1] + pCone->vec_normal[1] * t,
		pdfStartPos[2] + pCone->vec_normal[2] * t
	};

	// prepare information that cone motion will use

	pCone->pt_start[0] = px;
	pCone->pt_start[1] = py;
	pCone->pt_start[2] = pz;

	pCone->vec_U[0] = tr.U1; // use tool rx
	pCone->vec_U[1] = tr.U2;
	pCone->vec_U[2] = tr.U3;
	vector_unitize(pCone->vec_U, pCone->vec_U);

	// center vector (from start point to center)
	pCone->vec_center[0] = center[0] - px;
	pCone->vec_center[1] = center[1] - py;
	pCone->vec_center[2] = center[2] - pz;

	// radius
	pCone->radius = sqrt( vector_dot(pCone->vec_center, pCone->vec_center) );

	// unitize center vector
	vector_unitize(pCone->vec_center, pCone->vec_center);

	// start vector
	vector_cross(pCone->vec_center, pCone->vec_normal, pCone->vec_start);
	vector_unitize(pCone->vec_start, pCone->vec_start);

	// prepare rotation matrix
	if( fabs(pCone->vec_normal[0]) >= EPSLION ||
		fabs(pCone->vec_normal[1]) >= EPSLION )
	{
		const double  rz = atan2(pCone->vec_normal[1], pCone->vec_normal[0]);
		const double  ry = acos(pCone->vec_normal[2]);

		Matrix  m(3, 3);
		get_z_rotation_matrix( rz, pCone->mat_prev);
		get_y_rotation_matrix( ry, m);
		pCone->mat_prev *= m;

		get_y_rotation_matrix(-ry, pCone->mat_post);
		get_z_rotation_matrix(-rz, m);
		pCone->mat_post *= m;
	}
	// total distance
	return fabs(pCone->theta);
}

///////////////////////////////////////////////////////////////////////////////////////////////

void   __stdcall ConeCleanUp(int nGroup,
	int nCommandIndex,
	void* pvBuffer)
{
	if ( pvBuffer )
	{
		delete (CONE*) pvBuffer;
		pvBuffer = NULL;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////

BOOL   __stdcall ConeInterpolation(int nGroup,
	int nSynAxisNum,
	int nCommandIndex,
	const double* pdfStartPos,
	const double* pdfCurPos,
	double dfCurDist,
	double dfTotDist,
	double* pdfNextPosOffset,
	void* pvBuffer)
{
	ASSERT( nSynAxisNum == 8 && pvBuffer );	
	CONE  *pCone = (CONE*) pvBuffer;	

	// compute next wrist center
	double  next_wrist_center[3] = {0};
	dfCurDist = (pCone->theta >= 0) ? dfCurDist: -dfCurDist;

	double  r  = pCone->radius;
	double  c0 = sin( dfCurDist );
	double  c1 = 1.0 - cos( dfCurDist );

	for(int nAxis = 0; nAxis < 3; nAxis++)
	{
		next_wrist_center[nAxis] =
			r * c0 * pCone->vec_start[nAxis] +
			r * c1 * pCone->vec_center[nAxis]+
			pCone->pt_start[nAxis];
	}

	// compute the orientation
	double  U[3] = {0};
	double  V[3] = {0};
	double  W[3] = {0};

	// W ::use tool rx ry rz calculate flange posture
	W[0] = pdfStartPos[0] - next_wrist_center[0];
	W[1] = pdfStartPos[1] - next_wrist_center[1];
	W[2] = pdfStartPos[2] - next_wrist_center[2];

	// U
	vector_rotate(
		pCone->vec_U,
		pCone->vec_normal,
		dfCurDist,
		pCone->mat_prev,
		pCone->mat_post,
		U);

	// V
	vector_cross(W, U, V);
	vector_unitize(U, U);
	vector_unitize(V, V);
	vector_unitize(W, W);

	// retrieve the orientation
	ORIENTATION  orient;
	TRANSFORMATION_RULES  tr;

	tr.U1 = U[0]; tr.U2 = U[1]; tr.U3 = U[2];
	tr.V1 = V[0]; tr.V2 = V[1]; tr.V3 = V[2];
	tr.W1 = W[0]; tr.W2 = W[1]; tr.W3 = W[2];

	tr2orient(tr, &orient);

	// prepare output data :: x y z rx ry rz offset
	pdfNextPosOffset[0] = 0;
	pdfNextPosOffset[1] = 0;
	pdfNextPosOffset[2] = 0;
	pdfNextPosOffset[3] = orient.rx - pdfStartPos[3];
	pdfNextPosOffset[4] = orient.ry - pdfStartPos[4];
	pdfNextPosOffset[5] = orient.rz - pdfStartPos[5];
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Exported functions
CustomMotion::CustomMotion(PmcBase& card) : pmc_base(card)
{
	pTool = NULL;
}

CustomMotion::~CustomMotion()
{
// 	if (pTool)
// 	{
// 		delete pTool;
// 		pTool = NULL;
// 	}
}

int	CustomMotion::rbt_Cone_V6(double dfBaseVectX,
	double dfBaseVectY,
	double dfBaseVectZ,
	double dfTheta,
	WORD wGroupIndex /*=0*/)
{
	if( fabs(dfBaseVectX) < EPSLION && 
		fabs(dfBaseVectY) < EPSLION && 
		fabs(dfBaseVectZ) < EPSLION )
		return PARAMETER_ERR;

	CONE*  pCone = new CONE;

	pCone->vec_normal[0] = dfBaseVectX;
	pCone->vec_normal[1] = dfBaseVectY;
	pCone->vec_normal[2] = dfBaseVectZ;
	vector_unitize(pCone->vec_normal, pCone->vec_normal);

	pCone->theta = dfTheta * M_PI / 180.0; 
	return MCS_CustomMotion(
		ConeStartMotion,
		ConeInterpolation,
		ConeCleanUp,
		pCone,
		wGroupIndex);
}

////////////////////ToolCone/////////////////////////////////////////
int	CustomMotion::rbt_RX_V6(double dfTheta, WORD wGroupIndex /*=0*/)
{
	return rbt_Cone_V6(1, 0, 0, dfTheta, wGroupIndex);
}

int	CustomMotion::rbt_RY_V6(double dfTheta, WORD wGroupIndex /*=0*/)
{
	return rbt_Cone_V6(0, 1, 0, dfTheta, wGroupIndex);
}

int	CustomMotion::rbt_RZ_V6(double dfTheta, WORD wGroupIndex /*=0*/)
{
	return rbt_Cone_V6(0, 0, 1, dfTheta, wGroupIndex);
}


double __stdcall ToolRotStartMotion(int nGroup,
	int nSynAxisNum,
	int nCommandIndex,
	BOOL bAbsolute,
	const double* pdfStartPos,
	void* pvParameter)
{
	TOOLROTATE  *ptool = (TOOLROTATE*) pvParameter;

	double TotalDist;
	int I;

	TotalDist = 0;

	for(I = 0;I < ptool->TotalPoint - 1;I++)
	{ 
		if( I == 0 )
		{
			TotalDist = TotalDist + sqrt( (ptool->Trajectory[I].rp.pos.x-pdfStartPos[0])*(ptool->Trajectory[I].rp.pos.x-pdfStartPos[0]) + 
				(ptool->Trajectory[I].rp.pos.y-pdfStartPos[1])*(ptool->Trajectory[I].rp.pos.y-pdfStartPos[1]) +
				(ptool->Trajectory[I].rp.pos.z-pdfStartPos[2])*(ptool->Trajectory[I].rp.pos.z-pdfStartPos[2]) );
			ptool->Trajectory[I].Interval = TotalDist;
		}
		else
		{
			TotalDist = TotalDist + sqrt( (ptool->Trajectory[I].rp.pos.x-ptool->Trajectory[I-1].rp.pos.x)*(ptool->Trajectory[I].rp.pos.x-ptool->Trajectory[I-1].rp.pos.x) + 
				(ptool->Trajectory[I].rp.pos.y-ptool->Trajectory[I-1].rp.pos.y)*(ptool->Trajectory[I].rp.pos.y-ptool->Trajectory[I-1].rp.pos.y) +
				(ptool->Trajectory[I].rp.pos.z-ptool->Trajectory[I-1].rp.pos.z)*(ptool->Trajectory[I].rp.pos.z-ptool->Trajectory[I-1].rp.pos.z) );
			ptool->Trajectory[I].Interval = TotalDist;
		}
	}

	return TotalDist;

}

void   __stdcall ToolRotCleanUp(int nGroup,
	int nCommandIndex,
	void* pvParameter)
{
	if (pvParameter)
	{
		delete (TOOLROTATE*) pvParameter;
		pvParameter = NULL;
	}

}
 
BOOL   __stdcall ToolRotInterpolation(int nGroup,
	int nSynAxisNum,
	int nCommandIndex,
	const double* pdfStartPos,
	const double* pdfCurPos,
	double dfCurDist,
	double dfTotDist,
	double* pdfNextPosOffset,
	void* pvParameter)
{
	TOOLROTATE  *ptool = (TOOLROTATE*) pvParameter;

	// calculate next interpolation point
	double d[6];

	int IntervalIndex;
	double Rate;
	int I;

	IntervalIndex = 0;
	for( I = 0 ; I < (ptool->TotalPoint - 1); I++ )
	{
		if(dfCurDist >= ptool->Trajectory[I].Interval)
			IntervalIndex = I;
	}

	if (IntervalIndex == 0)
	{
		if(ptool->Trajectory[IntervalIndex].Interval == 0)
		{
			d[0] = 0;
			d[1] = 0;
			d[2] = 0;
			d[3] = 0;
			d[4] = 0;
			d[5] = 0;
		}
		else 
		{
			Rate = dfCurDist/(ptool->Trajectory[IntervalIndex].Interval);

			d[0] = Rate * (ptool->Trajectory[IntervalIndex].rp.pos.x - pdfStartPos[0]) ;
			d[1] = Rate * (ptool->Trajectory[IntervalIndex].rp.pos.y - pdfStartPos[1]) ;
			d[2] = Rate * (ptool->Trajectory[IntervalIndex].rp.pos.z - pdfStartPos[2]) ;
			d[3] = Rate * (ptool->Trajectory[IntervalIndex].rp.ang.x - pdfStartPos[3]) ;
			d[4] = Rate * (ptool->Trajectory[IntervalIndex].rp.ang.y - pdfStartPos[4]) ;
			d[5] = Rate * (ptool->Trajectory[IntervalIndex].rp.ang.z - pdfStartPos[5]) ;

		}
	}
	else
	{
		Rate = ( dfCurDist - ptool->Trajectory[IntervalIndex - 1].Interval )/
			( ptool->Trajectory[IntervalIndex].Interval - ptool->Trajectory[IntervalIndex - 1].Interval );
		d[0] = Rate * ( ptool->Trajectory[IntervalIndex].rp.pos.x - ptool->Trajectory[IntervalIndex-1].rp.pos.x ) + 
			ptool->Trajectory[IntervalIndex-1].rp.pos.x - pdfStartPos[0] ;
		d[1] = Rate * ( ptool->Trajectory[IntervalIndex].rp.pos.y - ptool->Trajectory[IntervalIndex-1].rp.pos.y ) + 
			ptool->Trajectory[IntervalIndex-1].rp.pos.y - pdfStartPos[1] ;
		d[2] = Rate * ( ptool->Trajectory[IntervalIndex].rp.pos.z - ptool->Trajectory[IntervalIndex-1].rp.pos.z ) + 
			ptool->Trajectory[IntervalIndex-1].rp.pos.z - pdfStartPos[2] ;

		d[3] = Rate * ( ptool->Trajectory[IntervalIndex].rp.ang.x - ptool->Trajectory[IntervalIndex-1].rp.ang.x ) + 
			ptool->Trajectory[IntervalIndex-1].rp.ang.x - pdfStartPos[3] ;
		d[4] = Rate * ( ptool->Trajectory[IntervalIndex].rp.ang.y - ptool->Trajectory[IntervalIndex-1].rp.ang.y ) + 
			ptool->Trajectory[IntervalIndex-1].rp.ang.y - pdfStartPos[4] ;
		d[5] = Rate * ( ptool->Trajectory[IntervalIndex].rp.ang.z - ptool->Trajectory[IntervalIndex-1].rp.ang.z ) + 
			ptool->Trajectory[IntervalIndex-1].rp.ang.z - pdfStartPos[5] ;

	}

	// calculate next interpolation point
	pdfNextPosOffset[0] = d[0];
	pdfNextPosOffset[1] = d[1];
	pdfNextPosOffset[2] = d[2];
	pdfNextPosOffset[3] = d[3];
	pdfNextPosOffset[4] = d[4];
	pdfNextPosOffset[5] = d[5];

	return TRUE;
}

int CustomMotion::rotate_tool_control_point( int nAxis, double theta, const CartPos* rob, const CartRef* cref, const CartPos* tool )
{
	const std::vector<CartPos>& axisend_list = pose_trans.get_rotate_tcp_path( 
		nAxis,
		theta,
		rob, 
		cref,  // around world axis.
		tool );

	// 	CmdInfo cmd;
	// 	cmd.iCommandType = CMD_BLEND;
	// 	cmd.bOnOff = true;
	// 	SMC.setCmdInfo(cmd);

	VERIFY( MCS_SetAbsolute(pmc_base.nGroupIndex) == NO_ERR );

	pTool = new TOOLROTATE;
	pTool->TotalPoint = axisend_list.size();

	int ret = 0;
	for (unsigned int i = 0; i < axisend_list.size(); ++i )
	{
		pTool->Trajectory[i].rp = axisend_list[i];
	}

	return MCS_CustomMotion(
		ToolRotStartMotion,
		ToolRotInterpolation,
		ToolRotCleanUp,
		pTool,
		pmc_base.nGroupIndex );

}

}