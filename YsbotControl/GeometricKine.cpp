// Copyright 2016, 亚思信息科技有限责任公司
// All right reserved.
//
// Author: Liu Run Dan ( lrdcz@163.com )
//
// Yasi Robot Motion Control System - 亚思机器人运动控制系统

#include "StdAfx.h"
#include "GeometricKine.h"
#include "EpcioBase.h"
#include "Journal.h"
#include "Time.h"

#define MAX_SOLUTION_NUM		(8)
#define TOLERANCE				(0.01)
#define SINGULARITY_RANGE		(M_PI * 3.0 / 180)
#define PI					    M_PI

namespace Ysbot
{

Time    cycletime;
double  DH_a1 = 150;  // 150
double  DH_a2 = 570;  // 570
double  DH_a3 = 130;  // 130
double  DH_d3 = 0.0;
double  DH_d4 = 640;  // 640
double  DH_d6 = 95;   // 95
double  DH_origin_offset_z = 450;  // 450

double  DH_elbow_post_threshold = atan( DH_a3 / DH_d4 );
int axis_num_kine_geometric = 6;
//////////////////////////////////////////////////////////////////////////

DHExParam exDHPara;

//////////////////////////////////////////////////////////////////////////


BOOL FXV6_fwk( const AxisPos& jnt_point, CartPos* crt_point );
void FXV6_ink2(const CartPos& crt_point, double q4,	double q5, 
	           const AxisPos& pre_jnt_point, AxisPos* jnt_point );

int init_kine_geometric(const ConfigReader& reader) throw (InvalidConfigurationException)
{
	memset(&exDHPara,0,sizeof(exDHPara));

  	if ( reader.get("ARMKIN::DHP_a1", DH_a1) < 0 || DH_a1 < 0 )
		throw  InvalidConfigurationException ("DHP_a1");
	if ( reader.get("ARMKIN::DHP_a2", DH_a2) < 0 || DH_a2 < 0 )
		throw  InvalidConfigurationException ("DHP_a2");
	if ( reader.get("ARMKIN::DHP_a3", DH_a3) < 0 || DH_a3 < 0 )
		throw  InvalidConfigurationException ("DHP_a3");

    reader.get("ARMKIN::DHP_d3", DH_d3);

	if ( reader.get("ARMKIN::DHP_d4", DH_d4) < 0 || DH_d4 < 0 )
		throw  InvalidConfigurationException ("DHP_a1");
	if ( reader.get("ARMKIN::DHP_d6", DH_d6) < 0 || DH_d6 < 0 )
		throw  InvalidConfigurationException ("DHP_a2");
	if ( reader.get("ARMKIN::DHP_offset_z", DH_origin_offset_z) < 0 || DH_origin_offset_z < 0 )
		throw  InvalidConfigurationException ("DHP_offset_z");

	DH_elbow_post_threshold = atan( DH_a3 / DH_d4 );

	//////////////////////////////////////////////////////////////////////////
	reader.get("ARMKIN::J34_CoupleRatio", exDHPara.J34_CoupleRatio);
	reader.get("ARMKIN::J45_CoupleRatio", exDHPara.J45_CoupleRatio);
	reader.get("ARMKIN::J56_CoupleRatio", exDHPara.J56_CoupleRatio);
	reader.get("ARMKIN::J46_CoupleRatio", exDHPara.J46_CoupleRatio);

	reader.get("ARMKIN::Tool_X_Offset", exDHPara.Tool.X_Offset);
	reader.get("ARMKIN::Tool_Y_Offset", exDHPara.Tool.Y_Offset);
	reader.get("ARMKIN::Tool_Z_Offset", exDHPara.Tool.Z_Offset);
	reader.get("ARMKIN::Tool_RX_Offset", exDHPara.Tool.RX_Offset);
	reader.get("ARMKIN::Tool_RY_Offset", exDHPara.Tool.RY_Offset);
	reader.get("ARMKIN::Tool_RZ_Offset", exDHPara.Tool.RZ_Offset);

	reader.get("ARMKIN::Work_X_Offset", exDHPara.Work.X_Offset);
	reader.get("ARMKIN::Work_Y_Offset", exDHPara.Work.Y_Offset);
	reader.get("ARMKIN::Work_Z_Offset", exDHPara.Work.Z_Offset);
	reader.get("ARMKIN::Work_RX_Offset", exDHPara.Work.RX_Offset);
	reader.get("ARMKIN::Work_RY_Offset", exDHPara.Work.RY_Offset);
	reader.get("ARMKIN::Work_RZ_Offset", exDHPara.Work.RZ_Offset);
	 
// 	CString msg;
// 	msg.Format(_T("dh = %.2f  %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f  %.2f %.2f %.2f %.2f %.2f \n"),
// 		        exDHPara.J34_CoupleRatio,exDHPara.J45_CoupleRatio,exDHPara.J56_CoupleRatio,exDHPara.J46_CoupleRatio,
// 				exDHPara.Tool.X_Offset,exDHPara.Tool.Y_Offset,exDHPara.Tool.Z_Offset,
// 				exDHPara.Tool.RX_Offset,exDHPara.Tool.RY_Offset,exDHPara.Tool.RZ_Offset,
// 				exDHPara.Work.X_Offset,exDHPara.Work.Y_Offset,exDHPara.Work.Z_Offset,
// 				exDHPara.Work.RX_Offset,exDHPara.Work.RY_Offset,exDHPara.Work.RZ_Offset );
// 	TRACE(msg);
	TRACE(_T("Constuct GeometricKinematics!\n")); 

	return 0;
}

void set_kine_axis_num(int naxis)
{
	axis_num_kine_geometric = naxis;
}

void    Set6DofDHParam( const DH6dofParam& dhp )
{
	DH_a1 = dhp.a1;
	DH_a2 = dhp.a2;
	DH_a3 = dhp.a3;
	DH_d3 = dhp.d3;
	DH_d4 = dhp.d4;
	DH_d6 = dhp.d6;
	DH_origin_offset_z = dhp.origin_offset_z;

	DH_elbow_post_threshold = atan( dhp.a3 / dhp.d4 );
}

void    Get6DofDHParam( DH6dofParam* pdhp )
{
	pdhp->a1 = DH_a1;
	pdhp->a2 = DH_a2;
	pdhp->a3 = DH_a3;
	pdhp->d3 = DH_d3;
	pdhp->d4 = DH_d4;
	pdhp->d6 = DH_d6;
	pdhp->origin_offset_z = DH_origin_offset_z;
}

void get_extend_dhpara(DHExParam* exdh)
{
 	memcpy(exdh,&exDHPara,sizeof(DHExParam));
}

BOOL __stdcall Geometric_Fwk(int nGroup,	int nSynAxisNum,
	const double* pdfFrom,	double* pdfTo,
	DWORD& dwPosture, void* pvExtension )
{
	if( nGroup != 0 || nSynAxisNum != axis_num_kine_geometric )
	{
		double fnum = nSynAxisNum;
		cout << "fwk err : fnum  = " << fnum << " dnum = " << axis_num_kine_geometric << '\n';
		return FALSE;
	}

	CartPos  crt;
	AxisPos  jnt_pos;

	// prepare input data
	jnt_pos.q1 = pdfFrom[0];
	jnt_pos.q2 = PI/2 - pdfFrom[1];
	jnt_pos.q3 = PI/2 - pdfFrom[2];
	jnt_pos.q4 = pdfFrom[3] - PI;
	jnt_pos.q5 = pdfFrom[4];
	jnt_pos.q6 = pdfFrom[5];

	// forward kinematic transformation
	BOOL  bArmLefty = FXV6_fwk(jnt_pos, &crt);

	// prepare output data
	pdfTo[0] = crt.pos.x;
	pdfTo[1] = crt.pos.y;
	pdfTo[2] = crt.pos.z + DH_origin_offset_z;
	pdfTo[3] = crt.ang.x;
	pdfTo[4] = crt.ang.y;
	pdfTo[5] = crt.ang.z;

	// determine robot posture
	dwPosture = DeterminePosture(bArmLefty, nSynAxisNum, pdfFrom,DH_elbow_post_threshold);

	// store last posture of the robot in case of changes of arm and elbow
	// posture in inverse kinematic transformation
	if( pvExtension )
	{
		EXTENSION  *ext = static_cast<EXTENSION*>( pvExtension );
		ext->last_posture = dwPosture;
		ext->pre_pre_J5 = ext->pre_J5;
		ext->pre_J5 = pdfFrom[4];
	}

	return TRUE;
}

BOOL FXV6_fwk( const AxisPos& jnt_point, CartPos* crt_point )
{
	TRANSFORMATION_RULES  tr;
	double c1,s1,c2,s2,c3,s3,c4,s4,c5,s5,c6,s6,c23,s23;
	double tem1=0;
	double tem2=0;

	s1 = sin(jnt_point.q1);	c1 = cos(jnt_point.q1);
	s2 = sin(jnt_point.q2);	c2 = cos(jnt_point.q2);
	s3 = sin(jnt_point.q3);	c3 = cos(jnt_point.q3);
	s4 = sin(jnt_point.q4);	c4 = cos(jnt_point.q4);
	s5 = sin(jnt_point.q5);	c5 = cos(jnt_point.q5);
	s6 = sin(jnt_point.q6);	c6 = cos(jnt_point.q6);

	s23 = sin(jnt_point.q2 + jnt_point.q3);
	c23 = cos(jnt_point.q2 + jnt_point.q3);

	// Forward Kine.Rules :
	// Those rules are derived by Ben, fx(reference book:"Robot Analysis",Lung-Wen Tsai).
	// and help to get transformation matrix 

	tem1 = c23*(c4*c5*c6-s4*s6)-s23*s5*c6;
	tem2 = s4*c5*c6+c4*s6;
	tr.U1 = c1*(tem1)+s1*tem2;
	tr.U2 = s1*(tem1)-c1*tem2;
	tr.U3 = s23*(c4*c5*c6-s4*s6)+c23*s5*c6;

	tem1 = -c23*(c4*c5*s6+s4*c6)+s23*s5*s6;
	tem2 = -s4*c5*s6+c4*c6;	
	tr.V1 = c1*(tem1)+s1*tem2;
	tr.V2 = s1*(tem1)-c1*tem2;
	tr.V3 = -s23*(c4*c5*s6+s4*c6)-c23*s5*s6;

	tem1 = c23*c4*s5+s23*c5;
	tr.W1 = c1*tem1 + s1*s4*s5;	
	tr.W2 = s1*tem1 - c1*s4*s5;	
	tr.W3 = s23*c4*s5-c23*c5;			

	tem1 = 	DH_a1+DH_a2*c2+DH_a3*c23+DH_d4*s23+DH_d6*(c23*c4*s5+s23*c5);
	tr.D1 = c1*tem1 + DH_d6*s1*s4*s5;	
	tr.D2 = s1*tem1 - DH_d6*c1*s4*s5;	
	tr.D3 = DH_a2*s2+DH_a3*s23-DH_d4*c23+DH_d6*(s23*c4*s5-c23*c5);	


	// to fill up data 
	ORIENTATION  orient;
	tr2orient(tr, &orient);

	crt_point->pos.x  = tr.D1;
	crt_point->pos.y  = tr.D2;
	crt_point->pos.z  = tr.D3;

	crt_point->ang.x = orient.rx;
	crt_point->ang.y = orient.ry;
	crt_point->ang.z = orient.rz;

	// see Spong: P.95: [px py pz] is wrist center
	// select the arm parameter is RIGHTY(1) or LEFTY(-1)

	double  px = tr.D1 - DH_d6 * tr.W1;
	double  py = tr.D2 - DH_d6 * tr.W2;

	//	double  pz = tr.D3 - DH_d6 * tr.W3;

	return ((px * c1 + py * s1) >= 0);
}

BOOL FXV6_ink(const CartPos& crt_point, AxisPos* jnt_point)
{
	// retrieve transformation matrix from orientation. see orient2tr()
	ORIENTATION  orient;
	TRANSFORMATION_RULES  tr;

	// valid orientation range: (-180) ~ [180]
	orient.rx = valid_angle(crt_point.ang.x, -PI, PI);
	orient.ry = valid_angle(crt_point.ang.y, -PI, PI);
	orient.rz = valid_angle(crt_point.ang.z, -PI, PI);

	// retrieve transformation matrix
	orient2tr(orient, &tr);

	// Rules of inverse kinamatics derived by Ben Liu fx.
	// d is the position of tool center (End-Effector).
	double  dx = crt_point.pos.x;
	double  dy = crt_point.pos.y;
	double  dz = crt_point.pos.z;

	// see Spong: P.95: p is wrist center
	double  px = dx - DH_d6 * tr.W1;
	double  py = dy - DH_d6 * tr.W2;
	double  pz = dz - DH_d6 * tr.W3;

	// check if the wrister center is out of range
	double  fJ2 = sqrt(px * px + py * py);
	double  xx  = pow(px - DH_a1 * px / fJ2, 2);
	double  yy  = pow(py - DH_a1 * py / fJ2, 2);
	double  zz  = pow(pz, 2);

	// max. length from J2 to wrist center
	double  dfMaxWorkSpace = sqrt(DH_d4 * DH_d4 + DH_a3 * DH_a3) + DH_a2;

	if( sqrt(xx + yy + zz) > dfMaxWorkSpace )
		return FALSE;
	//
	double theta1,theta2,theta3,theta4,theta5,theta6;
	double c1,s1,c2,s2,c3,s3,c4,s4,c5,s5,c6,s6,c23,s23;
	double u1,v1,r1,u2,v2,r2;
	double k1,k2,k3;

	// J1
	theta1 = atan2(py, px);

	if( !(crt_point.fig & POST_ARM_LEFTY) )
		theta1 += PI;

	theta1 = valid_angle(theta1, -PI, PI);
	c1 = cos(theta1);
	s1 = sin(theta1);

	// J3 (-2pi ~ 2pi)
	k1 = 2 * DH_a2 * DH_d4;
	k2 = 2 * DH_a2 * DH_a3;
	k3 = px*px+py*py+pz*pz - 2*px*DH_a1*c1 - 2*py*DH_a1*s1 + DH_a1*DH_a1 - DH_a2*DH_a2 - DH_a3*DH_a3 - DH_d4*DH_d4;

	if( crt_point.fig & POST_ARM_LEFTY )
	{
		int  dir = (crt_point.fig & POST_ELBOW_BELOW) ? 1: -1;
		theta3 = 2 * atan2(k1 + dir * sqrt(k1*k1+k2*k2-k3*k3), k2+k3);
	}
	else
	{
		int  dir = (crt_point.fig & POST_ELBOW_BELOW) ? -1: 1;
		theta3 = 2 * atan2(k1 + dir * sqrt(k1*k1+k2*k2-k3*k3), k2+k3);
	}

	c3 = cos(theta3);
	s3 = sin(theta3);

	// J2 (-pi ~ pi)
	u1 = DH_a2+DH_a3*c3+DH_d4*s3;
	v1 = -DH_a3*s3 + DH_d4*c3;
	r1 = px*c1 + py*s1-DH_a1;
	u2 = DH_a3*s3-DH_d4*c3;
	v2 = DH_a2+DH_a3*c3 + DH_d4*s3;
	r2 = pz;	
	s2 = (u2*r1-u1*r2)/(u2*v1-u1*v2);
	c2 = (r1-v1*s2)/u1;
	theta2 = atan2(s2, c2);

	// J5 (0 ~ pi)
	s23 = sin(theta2+theta3);
	c23 = cos(theta2+theta3);
	theta5 = acos(tr.W1*c1*s23 + tr.W2*s1*s23 - tr.W3*c23);

	if( fabs(theta5) < EPSLION ) { theta5 = EPSLION; }
	s5 = sin(theta5);
	c5 = cos(theta5);

	// J4 (0 ~ 2pi)
	s4 = (tr.W1*s1 - tr.W2*c1)/s5;
	c4 = (tr.W1*c1*c23 + tr.W2*s1*c23 + tr.W3*s23)/s5;

	theta4 = atan2(s4, c4);
	theta4 += PI;

	// J6 (-pi ~ pi)
	s6 =  (tr.V1*c1*s23 + tr.V2*s1*s23 - tr.V3*c23)/s5;
	c6 = -(tr.U1*c1*s23 + tr.U2*s1*s23 - tr.U3*c23)/s5;
	theta6 = atan2(s6, c6);


	// flip may change the result of J4, J5 & J6
	if( ( (crt_point.fig & POST_ARM_LEFTY) && !(crt_point.fig & POST_NONFLIP)) ||
		(!(crt_point.fig & POST_ARM_LEFTY) &&  (crt_point.fig & POST_NONFLIP)) )
	{
		theta5 = -theta5;
		theta4 = (theta4 > 0) ? (theta4 - PI): (theta4 + PI);
		theta6 = (theta6 > 0) ? (theta6 - PI): (theta6 + PI);
	}

	// single range: (-180) ~ [ 180]
	// double range: (-360) ~ [-180] + (180) ~ [360]
	if( crt_point.fig & POST_J4DOUBLE )
	{
		if( theta4 > 0 && theta4 <= PI )
			theta4 -= (2.0 * PI);
		else if( theta4 <= 0 && theta4 > -PI )
			theta4 += (2.0 * PI);
	}
	else
	{
		if ( theta4 > PI && theta4 <= (2 * PI) )
			theta4 -= (2.0 * PI);
		else if( theta4 <= -PI && theta4 > (-2.0 * PI) )
			theta4 += (2.0 * PI);
	}

	if( crt_point.fig & POST_J6DOUBLE )
	{
		if( theta6 > 0 && theta6 <= PI )
			theta6 -= (2.0 * PI);
		else if( theta6 <= 0 && theta6 > -PI )
			theta6 += (2.0 * PI);
	}
	else
	{
		if( theta6 > PI && theta6 <= (2.0 * PI) )
			theta6 -= (2.0 * PI);
		else if( theta6 <= -PI && theta6 > (-2.0 * PI) )
			theta6 += (2.0 * PI);
	}


	// fit to denso anwser
	theta2 = PI/2 - theta2;
	theta3 = PI/2 - theta3;

	// constrain J2 and J3 whithin -180 ~ 180 degrees
	theta2 = valid_angle(theta2, -PI, PI);
	theta3 = valid_angle(theta3, -PI, PI);


	// to fill up data 	
	jnt_point->q1 = theta1;
	jnt_point->q2 = PI/2 - theta2;
	jnt_point->q3 = PI/2 - theta3;
	jnt_point->q4 = theta4 - PI;
	jnt_point->q5 = theta5;
	jnt_point->q6 = theta6;

	return TRUE;
}

BOOL __stdcall Geometric_Ink(int nGroup,	int nSynAxisNum,
	const double* pdfFrom,	const double* pdfPrevResult,
	double* pdfTo,	DWORD& dwPosture,
	void* pvExtension )
{
	Time elps;

	if( nGroup != 0 || nSynAxisNum != axis_num_kine_geometric )
		return FALSE;

	CString str;
// 	str.Format(_T("Gik_cp = %.2f  %.2f  %.2f %.2f  %.2f %.2f\n "),
// 		pdfFrom[0],pdfFrom[1],pdfFrom[2],
// 		pdfFrom[3],pdfFrom[4],pdfFrom[5]);
// 	TRACE(str);
	// maintain arm & elbow posture
	if( pvExtension )
	{
		EXTENSION  *ext = static_cast<EXTENSION*>( pvExtension );
		DWORD  last_post = ext->last_posture;
		DWORD  post_to_maintain = POST_ARM_LEFTY | POST_ELBOW_BELOW | POST_NONFLIP;
		dwPosture &= (~post_to_maintain);
		dwPosture |= (last_post & post_to_maintain);
	}


	CartPos  crt_pos;
	AxisPos  jnt_pos;

	// prepare input data
	crt_pos.pos.x   = pdfFrom[0];
	crt_pos.pos.y   = pdfFrom[1];
	crt_pos.pos.z   = pdfFrom[2] - DH_origin_offset_z;
	crt_pos.ang.x  = pdfFrom[3];
	crt_pos.ang.y  = pdfFrom[4];
	crt_pos.ang.z  = pdfFrom[5];
	crt_pos.fig = dwPosture;

	// inverse kinematic transformation
	if( !FXV6_ink(crt_pos, &jnt_pos) )
		return FALSE;

	// prepare output data
	pdfTo[0] = jnt_pos.q1;
	pdfTo[1] = PI/2 - jnt_pos.q2;
	pdfTo[2] = PI/2 - jnt_pos.q3;
	pdfTo[3] = jnt_pos.q4 + PI;
	pdfTo[4] = jnt_pos.q5;
	pdfTo[5] = jnt_pos.q6;

	if( pdfPrevResult )
	{
		for(int i = 0; i < nSynAxisNum; ++i)
		{

			if( fabs(fabs(pdfTo[i] - pdfPrevResult[i]) - 2*PI) < MAX_JOINT_SPEED )
			{
				if( pdfTo[i] > pdfPrevResult[i] )
					pdfTo[i] -= (2.0 * PI);
				else
					pdfTo[i] += (2.0 * PI);
			}
		}

		// Handling of singular problem 
		if( fabs(pdfTo[4]) < SINGULARITY_RANGE )
		{
			pdfTo[3] = pdfPrevResult[3];


			if( fabs(pdfPrevResult[4] + pdfTo[4]) < MAX_JOINT_SPEED )
			{
				if( pvExtension )
				{
					EXTENSION  *ext = static_cast<EXTENSION*>( pvExtension );

					if( fabs(ext->pre_pre_J5) > fabs(pdfPrevResult[4]) &&
						fabs(pdfTo[4])        > fabs(pdfPrevResult[4]) )
					{
						pdfTo[4] *= -1;
					}
				}
			}
			// 
			AxisPos  pre_jnt_pos;
			pre_jnt_pos.q1 = pdfPrevResult[0];
			pre_jnt_pos.q2 = PI/2 - pdfPrevResult[1];
			pre_jnt_pos.q3 = PI/2 - pdfPrevResult[2];
			pre_jnt_pos.q4 = pdfPrevResult[3] - PI;
			pre_jnt_pos.q5 = pdfPrevResult[4];
			pre_jnt_pos.q6 = pdfPrevResult[5];

			FXV6_ink2(crt_pos, pdfTo[3] - PI, pdfTo[4], pre_jnt_pos, &jnt_pos);

			pdfTo[0] = jnt_pos.q1;
			pdfTo[1] = PI/2 - jnt_pos.q2;
			pdfTo[2] = PI/2 - jnt_pos.q3;
			pdfTo[5] = jnt_pos.q6;
		}

		dwPosture = DeterminePosture(
			dwPosture & POST_ARM_LEFTY,
			nSynAxisNum,
			pdfTo,DH_elbow_post_threshold );
	}

	if( pvExtension )
	{
		EXTENSION  *ext = static_cast<EXTENSION*>( pvExtension );
		ext->last_posture = dwPosture;
		ext->pre_pre_J5 = ext->pre_J5;
		ext->pre_J5 = pdfTo[4];
	}

// 	str.Format(_T("Gik_dq = %.2f  %.2f  %.2f %.2f  %.2f %.2f T=%d\n "),
// 		pdfTo[0]*CONST180PI,pdfTo[1]*CONST180PI,pdfTo[2]*CONST180PI,
// 		pdfTo[3]*CONST180PI,pdfTo[4]*CONST180PI,pdfTo[5]*CONST180PI,
// 		dwPosture );

// 	str.Format(_T("%d\n "),	elps.elapsed_usec());
// 	TRACE(str);
	return TRUE;
}

void FXV6_get_J1(const CartPos& tc,
	double q4,	double q5,
	double* J1a,double* J1b )
{
	const double  s4 = sin(q4);
	const double  s5 = sin(q5);

	const double  r0 = asin(DH_d6 * s4 * s5 / sqrt(tc.pos.x * tc.pos.x + tc.pos.y * tc.pos.y));
	const double  r1 = atan2(tc.pos.y, tc.pos.x);

	*J1a =  r0 + r1;
	*J1b = -r0 + r1;
}

void FXV6_get_J2(const CartPos& tc,
	double q1,double q4, double q5,
	double* J2a, double* J2b )
{
	const double  s1 = sin(q1);
	const double  c1 = cos(q1);
	const double  c4 = cos(q4);
	const double  s5 = sin(q5);
	const double  c5 = cos(q5);

	const double  t0 = (c1 * tc.pos.x) + (s1 * tc.pos.y) - DH_a1;
	const double  t1 = c4 * s5 * DH_d6 + DH_a3;
	const double  t2 = c5 * DH_d6 + DH_d4;
	const double  t3 = ((t0 * t0) + (tc.pos.z * tc.pos.z) + (DH_a2 * DH_a2) - (t1 * t1) - (t2 * t2)) / (2 * DH_a2);
	const double  t4 = sqrt(t0 * t0 + tc.pos.z * tc.pos.z);

	const double  r0 = acos(t3 / t4);
	const double  r1 = atan2(tc.pos.z, t0);

	*J2a =  r0 + r1;
	*J2b = -r0 + r1;
}

void FXV6_get_J3(const CartPos& tc,
	double q1,	double q2,	double q4,	double q5,	
	double* J3a, double* J3b )
{
	const double  s2 = sin(q2);
	const double  c4 = cos(q4);
	const double  s5 = sin(q5);
	const double  c5 = cos(q5);

	const double  t0 =  DH_a3 + DH_d6 * c4 * s5;
	const double  t1 = -DH_d4 - DH_d6 * c5;
	const double  t2 = sqrt(t0 * t0 + t1 * t1);

	const double  r0 = acos((tc.pos.z - DH_a2 * s2) / t2);
	const double  r1 = atan2(t0, t1);

	*J3a =  r0 + r1 - q2;
	*J3b = -r0 + r1 - q2;
}

double FXV6_get_J6(const CartPos& tc,double q1,	double q2,	double q3,	double q4,	double q5 )
{
	const double  s1  = sin(q1);	const double  c1 = cos(q1);
	const double  s2  = sin(q2);	const double  c2 = cos(q2);
	const double  s3  = sin(q3);	const double  c3 = cos(q3);
	const double  s4  = sin(q4);	const double  c4 = cos(q4);
	const double  s5  = sin(q5);	const double  c5 = cos(q5);

	const double  s23 = sin(q2 + q3);
	const double  c23 = cos(q2 + q3);


	// retrieve ideal RX (expected)
	TRANSFORMATION_RULES  tr;
	ORIENTATION  orient;
	double  rx_exp[3];

	orient.rx = valid_angle(tc.ang.x, -M_PI, M_PI);
	orient.ry = valid_angle(tc.ang.y, -M_PI, M_PI);
	orient.rz = valid_angle(tc.ang.z, -M_PI, M_PI);

	orient2tr(orient, &tr);

	rx_exp[0] = tr.U1;
	rx_exp[1] = tr.U2;
	rx_exp[2] = tr.U3;


	// retrieve current RZ (real)
	const double  t0 = c23 * c4 * s5 + s23 * c5;
	double  rz_real[3] = {
		c1 * t0 + s1 * s4 * s5,
		s1 * t0 - c1 * s4 * s5,
		s23 * c4 * s5 - c23 * c5
	};


	// retrieve the vector, named ideal RX', that ideal RX projects on the 
	// plane consists of real RX and real RY
	double  vector_t0[3];

	vector_cross(rx_exp, rz_real, vector_t0);
	vector_cross(rz_real, vector_t0, rx_exp);


	// retrieve real RX and real RY while J6 = 0
	double  rx_real[3] = {
		c1 * (c23 * c4 * c5 - s23 * s5) + s1 * s4 * c5,
		s1 * (c23 * c4 * c5 - s23 * s5) - c1 * s4 * c5,
		s23 * c4 * c5 + c23 * s5
	};

	double  ry_real[3] = {
		s1 * c4 - c1 * c23 * s4,
		-c1 * c4 - s1 * c23 * s4,
		-s23 * s4
	};

	// retrieve the angle difference between expected RX and real RX
	double  length_rx_exp  = sqrt( vector_dot(rx_exp,  rx_exp)  );
	double  length_rx_real = sqrt( vector_dot(rx_real, rx_real) );

	double  c6 = vector_dot(rx_exp, rx_real) / (length_rx_exp * length_rx_real);
	if( c6 >  1.0 ) { c6 =  1.0; }
	if( c6 < -1.0 ) { c6 = -1.0; }

	double  q6 = acos( c6 );

	// cause the range of arccos result is 0 ~ 180, it's necessary to 
	// determine the direction of q6
	double  length_ry_real = sqrt( vector_dot(ry_real, ry_real) );
	double  cos_rx_exp_to_ry_real = vector_dot(rx_exp, ry_real) / (length_rx_exp * length_ry_real);
	double  ang_rx_exp_to_ry_real = acos( cos_rx_exp_to_ry_real );

	return ( ang_rx_exp_to_ry_real < (M_PI/2) ) ? q6: -q6;
}

void  FXV6_ink2( const CartPos& crt_point,
	double q4,	double q5,
	const AxisPos& pre_jnt_point,	AxisPos* jnt_point )
{
	AxisPos  possible_solution[MAX_SOLUTION_NUM];
	double  sol_a, sol_b;

	// J1
	FXV6_get_J1(crt_point, q4, q5, &sol_a, &sol_b);
	int i = 0;
	for( i = 0; i < MAX_SOLUTION_NUM; i++)
		possible_solution[i].q1 = (i < (MAX_SOLUTION_NUM/2)) ? sol_a: sol_b;

	// J2
	for( i = 0; i < MAX_SOLUTION_NUM; i+=(MAX_SOLUTION_NUM/2))
	{
		FXV6_get_J2(crt_point, possible_solution[i].q1, q4, q5, &sol_a, &sol_b);
		for(int j = i; j < (i + MAX_SOLUTION_NUM/2); ++j)
			possible_solution[j].q2 = (j < (i + MAX_SOLUTION_NUM/4)) ? sol_a: sol_b;
	}

	// J3
	for( i = 0; i < MAX_SOLUTION_NUM; i+=(MAX_SOLUTION_NUM/4))
	{
		FXV6_get_J3(
			crt_point,
			possible_solution[i].q1,
			possible_solution[i].q2,
			q4,
			q5,
			&sol_a,
			&sol_b);

		possible_solution[i + 0].q3 = sol_a;
		possible_solution[i + 1].q3 = sol_b;
	}

	// J4, J5
	for( i = 0; i < MAX_SOLUTION_NUM; i++)
	{
		possible_solution[i].q4 = q4;
		possible_solution[i].q5 = q5;
	}

	CartPos  tc; // tool center
	double  min_diff = 1.0e+100;
	int  best_sol = -1; // best solution

	for( i = 0; i < MAX_SOLUTION_NUM; ++i)
	{
		FXV6_fwk(possible_solution[i], &tc);
		if( fabs(tc.pos.x - crt_point.pos.x) < TOLERANCE &&
			fabs(tc.pos.y - crt_point.pos.y) < TOLERANCE &&
			fabs(tc.pos.z - crt_point.pos.z) < TOLERANCE )
		{
			double  diff =
				pow(possible_solution[i].q1 - pre_jnt_point.q1, 2)+
				pow(possible_solution[i].q2 - pre_jnt_point.q2, 2)+
				pow(possible_solution[i].q3 - pre_jnt_point.q3, 2);

			if( diff < min_diff )
			{
				min_diff = diff;
				best_sol = i;
			}
		}
	}

	if( best_sol >= 0 )
		*jnt_point = possible_solution[best_sol];

	// J6
	jnt_point->q6 = FXV6_get_J6(
		crt_point,
		jnt_point->q1,
		jnt_point->q2,
		jnt_point->q3,
		jnt_point->q4,
		jnt_point->q5);
}

}
