#include "StdAfx.h"
#include "JogTeach.h"
#include "RobotKinematics.h"
#include "PoseTransform.h"
#include "WorldModel.h"

namespace Mtc
{

JogTeach::JogTeach(const ConfigReader& reader,  EpcioBase& card ) : epcio(card)
{
	;
}

JogTeach::~JogTeach(void)
{
}

void JogTeach::set_teach_speed(double extVel)
{
	const GroupProperties& gp = MWM.get_group_properties();
	double  ptp_speed  = extVel; // epcio.getOverride(); // use external speed
	double  max_teach_vel = gp.profile.dfMaxExternalVel;

	if( ptp_speed > max_teach_vel )
		ptp_speed = max_teach_vel;

	double  feed_speed = 0.01 * ptp_speed * MCC_GetSysMaxSpeed();
	ASSERT( ptp_speed >= 0 && ptp_speed <= 100 );
	VERIFY( MCC_SetPtPSpeed( ptp_speed,  epcio.nGroupIndex) );
	VERIFY( MCC_SetFeedSpeed(feed_speed, epcio.nGroupIndex) );
	// acceleration
	VERIFY( MCC_SetAccTime(500, epcio.nGroupIndex) == NO_ERR );
	VERIFY( MCC_SetDecTime(500, epcio.nGroupIndex) == NO_ERR );
}
 
int JogTeach::grp_jog( const CmdInfo& cf )
{
	int ret = 0;
	int axis = cf.chJogAxisMark;
	int dir  = cf.chJogDirMark;
	set_teach_speed(cf.dyn.JointVel);

	VERIFY( MCC_SetIncrease(epcio.nGroupIndex) == NO_ERR );

 	int nJogDir[MAX_JOINT_NUM] = {1};

	for (unsigned int i = 0; i < epcio.axis_num; i++ )
	{
		nJogDir[i] = ( ( 1 << i ) & dir ) ? -1 : 1;
	}
	if ( cf.iJogCoordType == jtJOINT )
	{
		if ( axis != 0 )
		{
			ret = MCC_PtP_V6(
				nJogDir[0] * 10 * M_PI,
				nJogDir[1] * 10 * M_PI,
				nJogDir[2] * 10 * M_PI,
				nJogDir[3] * 10 * M_PI,
				nJogDir[4] * 10 * M_PI,
				nJogDir[5] * 10 * M_PI,
				epcio.nGroupIndex,
				axis );
		}
 	
	}
	else if( cf.iJogCoordType == jtUSER )
	{
		if ( cf.cartref == NULL ) // base frame, for robot origin.
		{
			if ( cf.tool == NULL ) // For six axis end.
			{
				DriverArmAlongXYBase( nJogDir, axis );
			}
			else // for special tool set.
			{
				;
			}
		}
		else
		{
			// have tool reference user coordinate.
			if ( cf.tool == NULL )
			{
				DriverArmAlongXYBase( nJogDir, axis );
			}
			else
			{
				;
			}
		}
	}
	else if ( cf.iJogCoordType == jtTOOL )
	{
		if ( cf.tool == NULL )
		{
			DriverArmAlongTool(nJogDir,axis);
		}
		else
		{
			DriverArmAlongTool(nJogDir,axis);
		}
	}

	VERIFY( MCC_SetAbsolute(epcio.nGroupIndex) == NO_ERR );
	return ret;
}

void JogTeach::DriverArmAlongXYBase( int* nDir, unsigned int nAxis )
{
	if( nAxis >= EPCIO_AXIS_X && nAxis < EPCIO_AXIS_U )
	{
		int ret =  MCC_Line_V6(
			nDir[0] * 10000.0, // 10000
			nDir[1] * 10000.0,
			nDir[2] * 10000.0, 
			0,	0,	0,	0, // FIG
			epcio.nGroupIndex,
			nAxis 
			); // ret >= 0
		// 		TRACE(_T("line motion ret = %d %d %d\n"),ret, nDir[0], nDir[1]);
	}
	else
	{
		double  rate = MCC_GetPtPSpeed(epcio.nGroupIndex);
		VERIFY( MCC_SetFeedSpeed(0.01 * rate * MAX_JOINT_SPEED, epcio.nGroupIndex) );
		switch( nAxis )
		{
		case EPCIO_AXIS_U: rbt_RX_V6(nDir[3] * 3600, epcio.nGroupIndex); break;
		case EPCIO_AXIS_V: rbt_RY_V6(nDir[4] * 3600, epcio.nGroupIndex); break;
		case EPCIO_AXIS_W: rbt_RZ_V6(nDir[5] * 3600, epcio.nGroupIndex); break;
		default: break;
		}
	}
}

void JogTeach::DriverArmAlongTool( int* nDir, unsigned int nAxis )
{
	double  axis_vector[3] = {0};
	DWORD   dwFIG;

	PosInfo cur_pos = epcio.get_position( PosInfo::CTmachine, LDScommad, epcio.nGroupIndex );
	dwFIG = (DWORD)cur_pos.cart.fig;

	Vec3D dir = cur_pos.cart.ang;
	Matrix rot = PoseTransform::VecToRotMat(dir);
	int col = 1;
	switch( nAxis )
	{
	case EPCIO_AXIS_X: col = 1;	break;
	case EPCIO_AXIS_Y: col = 2; break;
	case EPCIO_AXIS_Z: col = 3;	break;
	default: break;
	}
	for ( int i = 0; i < 3; i++)
		axis_vector[i] = rot(i+1,col); // ????????????????

	if( nAxis >= EPCIO_AXIS_X && nAxis < EPCIO_AXIS_U )
	{
		VERIFY( MCC_Line_V6(
			nDir[0] * axis_vector[0] * 10000.0,
			nDir[1] * axis_vector[1] * 10000.0,
			nDir[2] * axis_vector[2] * 10000.0,
			0,
			0,
			0,
			dwFIG, // FIG
			epcio.nGroupIndex,
			EPCIO_AXIS_X | EPCIO_AXIS_Y | EPCIO_AXIS_Z
			) >= 0 );
	}
	else  
	{
		// re-adjust feed speed because the speed unit becomes radian/sec here
		double  rate = MCC_GetPtPSpeed(epcio.nGroupIndex);
		VERIFY( MCC_SetFeedSpeed(0.01 * rate * MAX_JOINT_SPEED, epcio.nGroupIndex) );

		int mdir = 1;
		switch( nAxis )
		{
		case EPCIO_AXIS_U: mdir = nDir[3]; break;
		case EPCIO_AXIS_V: mdir = nDir[4]; break;
		case EPCIO_AXIS_W: mdir = nDir[5]; break;
		default: break;
		}

		VERIFY( rbt_Cone_V6(
			axis_vector[0],
			axis_vector[1],
			axis_vector[2],
			mdir * 3600,
			epcio.nGroupIndex) >= 0 );
	};
}


//////////////// custom motion /////////////////////////////////////////
// Callback functions

double __stdcall ConeStartMotion(int nGroup,
	int nSynAxisNum,
	int nCommandIndex,
	BOOL bAbsolute,
	const double* pdfStartPos,
	void* pvBuffer)
{
	ASSERT( nSynAxisNum == 6 && pvBuffer );

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
	delete (CONE*) pvBuffer;
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
	ASSERT( nSynAxisNum == 6 && pvBuffer );	
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

int	rbt_Cone_V6(double dfBaseVectX,
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

	return MCC_CustomMotion(
		ConeStartMotion,
		ConeInterpolation,
		ConeCleanUp,
		pCone,
		wGroupIndex);
}

////////////////////ToolCone/////////////////////////////////////////
int		 rbt_RX_V6(double dfTheta, WORD wGroupIndex /*=0*/)
{
	return rbt_Cone_V6(1, 0, 0, dfTheta, wGroupIndex);
}

int		 rbt_RY_V6(double dfTheta, WORD wGroupIndex /*=0*/)
{
	return rbt_Cone_V6(0, 1, 0, dfTheta, wGroupIndex);
}

int		 rbt_RZ_V6(double dfTheta, WORD wGroupIndex /*=0*/)
{
	return rbt_Cone_V6(0, 0, 1, dfTheta, wGroupIndex);
}


}