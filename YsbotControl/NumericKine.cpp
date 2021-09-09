// Copyright 2016, 亚思信息科技有限责任公司
// All right reserved.
//
// Author: Liu Run Dan ( lrdcz@163.com )
//
// Yasi Robot Motion Control System - 亚思机器人运动控制系统

#include "StdAfx.h"
#include "NumericKine.h"
#include "Journal.h"
#include <algorithm> 
#include <vector>
#include "Time.h"
#include "PoseTransform.h"

namespace Ysbot
{


Robot*  peter_fwk = NULL;
Robot*  peter_ink = NULL;
int     dof = 6;
double  dh_threshold = 1.0;
int     nAxisNum = 6;
ColumnVector curraxispos(6);

int init_kine_numeric(const ConfigReader& reader) throw (InvalidConfigurationException)
{
	std::string confline;
	dof = 6;
	if ( reader.get("DH_PARA::dh_file", confline) <= 0 ) 
	{
		JERROR("no config line \"NumericKinematics dh files\" found");
		throw InvalidConfigurationException ("NumericKinematics dh files");
	}

	if ( !confline.empty() )
	{
		int spos = confline.find('/');
		std::string newconfline = confline.substr(spos+1);
		int epos = newconfline.find('.');
		newconfline.erase(newconfline.begin()+epos, newconfline.end() );
		transform(newconfline.begin(), newconfline.end(), newconfline.begin(), ::toupper);  // change upper
		peter_fwk = new Robot( confline, newconfline );
		peter_ink = new Robot( confline, newconfline );
		dof = peter_fwk->get_dof();
		TRACE(_T("Constuct NumericKinematics!\n")); 
 		double DH_a3 = peter_fwk->links[3].get_a();
		double DH_d4 = peter_fwk->links[4].get_d();
		dh_threshold = atan( DH_a3 / DH_d4 );
 		return 0;
	}
	else
	{
		TRACE(_T("Constuct NumericKinematics error!\n")); 
		throw InvalidConfigurationException ("NumericKinematics config files");
   		return -1;
	}
}

void release_kine_numeric()
{
	if( peter_fwk )
	{
		delete peter_fwk;
		peter_fwk = NULL;
	}
	if( peter_ink )
	{
		delete peter_ink;
		peter_ink = NULL;
	}
}

void set_start_pos( const AxisPos& axis)
{
 	ColumnVector qr(6);
	qr(1) = axis.q1;
	qr(2) = axis.q2;
	qr(3) = axis.q3;
	qr(4) = axis.q4;
	qr(5) = axis.q5;
	qr(6) = axis.q6;

	if (peter_fwk)
	{
		peter_fwk->set_q(qr);
	}

	if ( peter_ink )
	{
		peter_ink->set_q(qr);
	}
 }

void GetDHParam( int linkID, DHParaBase* dh )
{
	if( dh == NULL || peter_fwk == NULL )
 		return;

 	dh->L = peter_fwk->links[linkID].get_a();
	dh->d = peter_fwk->links[linkID].get_d();
	dh->a = peter_fwk->links[linkID].get_alpha();
	dh->modifiedDH = peter_fwk->links[linkID].get_DH();
}

BOOL fwd_kine_numeric( const AxisPos& apos, CartPos* cpos )
{
	// 	####################transform joint var###################
	// 		q0 =[j(1)   pi/2-j(2)   pi/2-j(3)   j(4)   j(5)   pi+j(6)];
	// 	##########################################################
	if ( !peter_fwk )
 		return -1;

 	double s1 = sin(apos.q1);	
	double c1 = cos(apos.q1);

	AxisPos dq = apos;
	dq.q2 =  valid_angle( M_PI/2 - apos.q2, -M_PI, M_PI );
	dq.q3 =  valid_angle( M_PI/2 - apos.q3, -M_PI, M_PI );
	dq.q6 =  valid_angle( M_PI   + apos.q6, -M_PI, M_PI );

	ColumnVector qr = ColumnVector(6);
	Real jt[] = { dq.q1, dq.q2, dq.q3, dq.q4, dq.q5, dq.q6 };
	qr << jt;

    peter_fwk->set_q(qr);

	Matrix tobj = peter_fwk->kine();

	CartPos pi = PoseTransform::MatrixToCartPos( tobj );

	pi.pos = pi.pos * 1e3; // im mm

	*cpos = pi;
 
 	return ((pi.pos.x * c1 + pi.pos.y * s1) >= 0);
 }

BOOL __stdcall Numeric_Fwk(int nGroup,
	int nSynAxisNum,
	const double* pdfFrom,
	double* pdfTo,
	DWORD& dwPosture,
	void* pvExtension)
{
// 	if( nGroup != 0 || nSynAxisNum != 6 )
// 		return FALSE;

	static CartPos  crt;
	static AxisPos  jnt_pos;

	// prepare input data
	jnt_pos.q1 = pdfFrom[0];
	jnt_pos.q2 = pdfFrom[1];
	jnt_pos.q3 = pdfFrom[2];
	jnt_pos.q4 = pdfFrom[3];
	jnt_pos.q5 = pdfFrom[4];
	jnt_pos.q6 = pdfFrom[5];

	// forward kinematic transformation
	BOOL  bArmLefty = fwd_kine_numeric(jnt_pos, &crt);

	// prepare output data
	pdfTo[0] = crt.pos.x;
	pdfTo[1] = crt.pos.y;
	pdfTo[2] = crt.pos.z;
	pdfTo[3] = crt.ang.x;
	pdfTo[4] = crt.ang.y;
	pdfTo[5] = crt.ang.z;

	// determine robot posture
	dwPosture = DeterminePosture( bArmLefty, nSynAxisNum, pdfFrom, dh_threshold );
	return TRUE;
}

int inv_kine_numeric( const CartPos& cpos, AxisPos* apos  )
{
	if ( !peter_ink )
 		return -1;

 	CartPos pi = cpos;
	pi.pos *= 1e-3; // in m

// 	Matrix tobj = pi.toHomogeneousMat();
	Matrix tobj = PoseTransform::CartPosToHomogeneousMat(pi);
	bool conv = false;
	ColumnVector qinv(6);
 	qinv = 0.0;
  
	qinv = peter_ink->inv_kin( tobj, 0, 6, conv );

	// 	%%###################transform joint var####################
	// 	qinv(2)= pi/2 - qinv(2);
	// 	qinv(3)= pi/2 - qinv(3);
	// 	qinv(6)= qinv(6) - pi;
	// 	%%##########################################################
 
	if ( conv )
	{
		AxisPos ap(qinv);
		ap.q2 = valid_angle( M_PI/2 - ap.q2, -M_PI, M_PI );
		ap.q3 = valid_angle( M_PI/2 - ap.q3, -M_PI, M_PI );
		ap.q6 = valid_angle( ap.q6  - M_PI,  -M_PI, M_PI );

		*apos = ap;

		return 0;
	}
	else
	{
 		TRACE(_T("inv error!\n"));
		return -2;
	}
}

BOOL __stdcall Numeric_Ink(int nGroup, int nSynAxisNum, const double* pdfFrom, const double* pdfPrevResult, double* pdfTo, DWORD& dwPosture, void* pvExtension)
{
	Time elps;

// 	if( nGroup != 0 || nSynAxisNum != 6 )
// 		return FALSE;
 
	CString str;
// 	str.Format(_T("Nik_cp = %.2f  %.2f  %.2f %.2f  %.2f %.2f\n "),
// 		pdfFrom[0],pdfFrom[1],pdfFrom[2],
// 		pdfFrom[3],pdfFrom[4],pdfFrom[5]);
// 	TRACE(str);
// 	// maintain arm & elbow posture
	if( pvExtension )
	{
		EXTENSION  *ext = static_cast<EXTENSION*>( pvExtension );
		DWORD  last_post = ext->last_posture;
		DWORD  post_to_maintain = POST_ARM_LEFTY | POST_ELBOW_BELOW | POST_NONFLIP;
		dwPosture &= (~post_to_maintain);
		dwPosture |= (last_post & post_to_maintain);
	}
	
	static CartPos  crt_pos;
	static AxisPos  jnt_pos;

	// prepare input data
	crt_pos.pos.x  = pdfFrom[0];
	crt_pos.pos.y  = pdfFrom[1];
	crt_pos.pos.z  = pdfFrom[2];
	crt_pos.ang.x  = pdfFrom[3];
	crt_pos.ang.y  = pdfFrom[4];
	crt_pos.ang.z  = pdfFrom[5];
	crt_pos.fig    = dwPosture;

	if ( pdfPrevResult )
	{
		ColumnVector qr(6);
		for ( int i = 1; i <= 6; i++ )
			qr(i) = pdfPrevResult[i-1];

		qr(2) = valid_angle(M_PI/2 - qr(2),-M_PI,M_PI);
		qr(3) = valid_angle(M_PI/2 - qr(3),-M_PI,M_PI);
		qr(6) = valid_angle(M_PI   + qr(6),-M_PI,M_PI);

		peter_ink->set_q(qr);	

	}


	// inverse kinematic transformation
	if( inv_kine_numeric(crt_pos, &jnt_pos) != 0 )
		return FALSE;

	// prepare output data
	pdfTo[0] = jnt_pos.q1;
	pdfTo[1] = jnt_pos.q2;
	pdfTo[2] = jnt_pos.q3;
	pdfTo[3] = jnt_pos.q4;
	pdfTo[4] = jnt_pos.q5;
	pdfTo[5] = jnt_pos.q6;

	if (pdfPrevResult)
	{
		for(int i = 0; i < nSynAxisNum; ++i)
		{
			if( fabs(fabs(pdfTo[i] - pdfPrevResult[i]) - 2*M_PI) < MAX_JOINT_SPEED )
			{
				if( pdfTo[i] > pdfPrevResult[i] )
					pdfTo[i] -= (2.0 * M_PI);
				else
					pdfTo[i] += (2.0 * M_PI);
			}
		}	

		dwPosture = DeterminePosture(
			dwPosture & POST_ARM_LEFTY,
			nSynAxisNum,
			pdfTo,dh_threshold );
 	}

	if( pvExtension )
	{
		EXTENSION  *ext = static_cast<EXTENSION*>( pvExtension );
		ext->last_posture = dwPosture;
		ext->pre_pre_J5 = ext->pre_J5;
		ext->pre_J5 = pdfTo[4];
	}

// 	str.Format(_T("Nik_dq = %.2f  %.2f  %.2f %.2f  %.2f %.2f T= %d\n "),
// 		rad2deg(pdfTo[0]),rad2deg(pdfTo[1]),rad2deg(pdfTo[2]),
// 		rad2deg(pdfTo[3]),rad2deg(pdfTo[4]),rad2deg(pdfTo[5]),
// 		elps.elapsed_usec() );
// 	str.Format(_T("%d\n "),
// 		elps.elapsed_usec() );
// 	TRACE(str);

	return TRUE;
}


}