// Copyright 2016, 亚思信息科技有限责任公司
// All right reserved.
//
// Author: Liu Run Dan ( lrdcz@163.com )
//
// Yasi Robot Motion Control System - 亚思机器人运动控制系统

#include "StdAfx.h"
#include "RobotKinematics.h"
#include "Journal.h"
#include <cstring>
#include <cmath>
#include "GroupProperties.h"
#include "GeometricKine.h"
#include "NumericKine.h"
#include "kinematics.h"
#include "WorldModel.h"

using namespace Ysbot;

typedef BOOL (__stdcall *FWDKINEFUN) (
	int nGroup,
	int nSynAxisNum,
	const double* pdfFrom,
	double* pdfTo,
	DWORD& dwPosture,
	void* pvExtension
	);

typedef BOOL (__stdcall *INVKINEFUN) (
	int nGroup,
	int nSynAxisNum,
	const double* pdfFrom,
	const double* pdfPrevResult,
	double* pdfTo,
	DWORD& dwPosture,
	void* pvExtension
	);


BOOL __stdcall Dummy_Fwk(int nGroup,
	int nSynAxisNum,
	const double* pdfFrom,
	double* pdfTo,
	DWORD& dwPosture,
	void* pvExtension)
{
	if( nGroup != 0 || nSynAxisNum != 6 )
		return FALSE;

	for( int i = 0; i < 6; i++ )
		*(pdfTo+i) = 0.0;
	dwPosture = 1;
	return TRUE;
}

BOOL __stdcall Dummy_Ink(int nGroup,
	int nSynAxisNum,
	const double* pdfFrom,
	const double* pdfPrevResult,
	double* pdfTo,
	DWORD& dwPosture,
	void* pvExtension)
{
	if( nGroup != 0 || nSynAxisNum != 6 )
		return FALSE;

	for( int i = 0; i < 6; i++ )
		*(pdfTo+i) = 0.0;
	dwPosture = 1;

	return TRUE;
}

using namespace std;

RobotKinematics* RobotKinematics::main_robot_kinematics=NULL;

RobotKinematics& RobotKinematics::get_main_robot_kinematics () throw () { return *RobotKinematics::main_robot_kinematics; }

bool RobotKinematics::is_main_robot_kinematics_available () throw()
{
	return main_robot_kinematics != NULL;
}

const char* RobotKinematics::get_robot_kinematics_type () const throw () { return robot_kinematics_descriptor; }

RobotKinematics::~RobotKinematics () throw () 
{
	if ( RobotKinematics::main_robot_kinematics==this)
		RobotKinematics::main_robot_kinematics=NULL;   

	if( string(robot_kinematics_descriptor) == "Numeric" )
		release_kine_numeric();
 
	if ( robot_kinematics_descriptor )
	{
		delete [] robot_kinematics_descriptor;
		robot_kinematics_descriptor = NULL;
	}
}

RobotKinematics::RobotKinematics (const ConfigReader& vread) throw ( bad_alloc) : configuration_list(vread),pFwdKineFuc(NULL),pInvKineFuc(NULL),
	pInitExtensionFuc(NULL),pPreProcessMotionCmdFuc(NULL),axis_num_kine(6),use_robot_lib(false),nGroupIdx(-1)
{
	if( RobotKinematics::main_robot_kinematics == NULL  )
		RobotKinematics::main_robot_kinematics=this;  
  
	dh_base.L = 0;
	dh_base.a = 0;
	dh_base.d = 0;
	dh_base.modifiedDH = false;
 
	robot_kinematics_descriptor = NULL;
	string confline = "Geometric";     // epcio

	if (vread.get("robot_kinematics_type", confline)<=0) 
	{
		JERROR("no config line \"robot_kinematics_type\" found");
		throw InvalidConfigurationException ("robot_kinematics");
	}
	try
	{
		really_change_robot_kinematics_type (confline.c_str(), vread);
	}catch(YsbotsException& e)
	{
		JERROR((std::string ("creating robot kinematics of type ")+confline+std::string(" failed due to: ")+std::string(e.what())).c_str());
		throw (e);
	}

	TRACE(_T("Constuct RobotKinematics!\n")); 

}

bool RobotKinematics::change_robot_kinematics_type (const char* pt) throw () {
	return change_robot_kinematics_type (pt, configuration_list);
}

bool RobotKinematics::change_robot_kinematics_type (const char* pt, const ConfigReader& vread) throw () {
	try{
		really_change_robot_kinematics_type (pt, vread);
	}catch(bad_alloc&)
	{
		JWARNING(("Change of RobotMechanics type failed due to lack of memory"));
		return false;
	} 
	return true;
}

void RobotKinematics::really_change_robot_kinematics_type (const char* pt, const ConfigReader& vread) throw ( YsbotsException,std::bad_alloc) {
	
	char* new_descriptor;
	new_descriptor = new char [strlen(pt)+1];
	strcpy(new_descriptor,pt);

	if( robot_kinematics_descriptor != NULL ) 
	{
 		delete [] robot_kinematics_descriptor;
	}
 
	try {
		robot_kinematics_descriptor = new_descriptor;
		if ( string(robot_kinematics_descriptor) == "Geometric" ) // epcio
		{
			init_kine_geometric(vread);
		
			pFwdKineFuc = Geometric_Fwk;
			pInvKineFuc = Geometric_Ink;
		}
		else if ( string(robot_kinematics_descriptor) == "Numeric" ) // peter
		{
			init_kine_numeric(vread);

			pFwdKineFuc = Numeric_Fwk;
			pInvKineFuc = Numeric_Ink;

		}
		else if ( string(robot_kinematics_descriptor) == "RobotLib" ) // use robot lib
		{
            init_kine_geometric(vread);
 			use_robot_lib = true;
			pFwdKineFuc = Dummy_Fwk;
			pInvKineFuc = Dummy_Ink;
		}
		else
		{
			pFwdKineFuc = Dummy_Fwk;
			pInvKineFuc = Dummy_Ink;
		}
	}catch (invalid_argument){
		throw InvalidConfigurationException ("robot_kinematics_type");
	}
 

	pInitExtensionFuc       = V6_InitExtension;
	pPreProcessMotionCmdFuc = V6_PreProcessMotionCommand;
 
}

void RobotKinematics::set_axis_num(int n)
{
	axis_num_kine = n;
	Ysbot::set_kine_axis_num(axis_num_kine);
}
  
const DHParaBase& RobotKinematics::get_link_dh( const int linkID )
{
	if( RobotKinematics::main_robot_kinematics == NULL  )
		return dh_base;

	if ( string(robot_kinematics_descriptor) == "Geometric" ||
		 string(robot_kinematics_descriptor) == "RobotLib" )
	{
		Ysbot::DH6dofParam dhp;
	    Get6DofDHParam( &dhp );
		switch( linkID )
		{
			case 1:
				dh_base.L = dhp.a1;
				dh_base.d = dhp.origin_offset_z;
				dh_base.a = 0; // angle
				break;
			case 2:
				dh_base.L = dhp.a2;
				dh_base.d = 0;
				dh_base.a = 0;  
				break;
			case 3:
				dh_base.L = dhp.a3;
				dh_base.d = dhp.d3;
				dh_base.a = 0;  
				break;
			case 4:
				dh_base.L = 0;
				dh_base.d = dhp.d4;
				dh_base.a = 0;  
				break;
			case 5:
				dh_base.L = 0;
				dh_base.d = 0;
				dh_base.a = 0;  
				break;
			case 6:
				dh_base.L = 0;
				dh_base.d = dhp.d6;
				dh_base.a = 0;  
				break;
			default: break;
		}
	}
	else if ( string(robot_kinematics_descriptor) == "General" )
	{
		GetDHParam(linkID,&dh_base); 
	}
	return dh_base;
}
 
void RobotKinematics::define_start_pos( const AxisPos& axis )
{
	if ( string(robot_kinematics_descriptor) == "Numeric" )
	   set_start_pos(axis);

	const GroupProperties& group_properties = MWM.get_group_properties();
	set_axis_num(group_properties.axis_num);
}

int RobotKinematics::fwd_kine( const AxisPos& apos, CartPos* cpos )
{
	double joint[MAX_JOINT_NUM] = {0};
	ColumnVector aq = AxisPos(apos).toColumnVector();
	for ( int i = 0; i < aq.Nrows(); i++ )
		joint[i] = aq(i+1);

	double cartpos[MAX_JOINT_NUM] = {0};
	DWORD pdwPosture = 0;

	int errorcode = -1;
	int nret = -1;
	 
	if ( use_robot_lib )
	{
		if ( nGroupIdx != 0 )
 			nGroupIdx = 0;
  		nret = rbt_FwdKinematics_V6(joint,cartpos,&pdwPosture,nGroupIdx);
 	}
	else
	{
		BOOL b = ( (FWDKINEFUN)( pFwdKineFuc) )(
			0, axis_num_kine, joint, cartpos, pdwPosture, NULL );
		if( b )
 			nret = 0;
 	}

	if ( nret == 0 )
	{
		cpos->pos = Vec3D( cartpos[0], cartpos[1], cartpos[2] );
		cpos->ang = Vec3D( cartpos[3], cartpos[4], cartpos[5] );
		cpos->fig = pdwPosture;
		errorcode = 0;
	}
 

	return errorcode;
}

int RobotKinematics::inv_kine( const CartPos& cpos, AxisPos* apos )
{
	double cartpos[MAX_JOINT_NUM] = {0};
	double joint[MAX_JOINT_NUM] = {0};
	cartpos[0] = cpos.pos.x;
	cartpos[1] = cpos.pos.y;
	cartpos[2] = cpos.pos.z;
	cartpos[3] = cpos.ang.x;
	cartpos[4] = cpos.ang.y;
	cartpos[5] = cpos.ang.z;
	DWORD dwPosture = cpos.fig;

	int errorcode = -1;
	int nret = -1;
  
	if ( use_robot_lib )
	{
		nret = rbt_InvKinematics_V6(cartpos,dwPosture,joint,nGroupIdx);
 	}
	else
	{
		BOOL b = ( (INVKINEFUN)(pInvKineFuc) )(
			0, axis_num_kine, cartpos, NULL, joint, dwPosture, NULL );
		if(b)
 			nret = 0;
 	}
 
 	if ( nret == 0 )
	{
		apos->q1 = joint[0];
		apos->q2 = joint[1];
		apos->q3 = joint[2];
		apos->q4 = joint[3];
		apos->q5 = joint[4];
		apos->q6 = joint[5];
 		errorcode = 0;
 	}
	else
	{
		TRACE(_T("inv_kine error!\n"));
		cout << " RobotKinematics inv kine error! \n";
	}

	return errorcode;
  
}

int RobotKinematics::init_robot_lib(bool bset, int nGroupIndex )
{
	nGroupIdx = nGroupIndex;

	rbt_SetRobotType(6,nGroupIndex);

	DH_PARAM_EX m_dhp_ex;
 	memset(&m_dhp_ex,0,sizeof(m_dhp_ex));
 
	DH6DOF_PARAM dhp;
	memset(&dhp,0,sizeof(dhp));

	Ysbot::DHExParam myexdh;
	get_extend_dhpara(&myexdh);

	DH_PARAM_EX libexdh;
	memcpy(&libexdh,&myexdh,sizeof(DH_PARAM_EX));

	dhp.a1 = get_link_dh(1).L;
	dhp.a2 = get_link_dh(2).L;
	dhp.a3 = get_link_dh(3).L;
	dhp.d3 = get_link_dh(3).d;
	dhp.d4 = get_link_dh(4).d;
	dhp.d6 = get_link_dh(6).d;
	dhp.origin_offset_z = get_link_dh(1).d;
	rbt_Set6DofDHParam(dhp, nGroupIndex);

	rbt_SetDHParamEx(m_dhp_ex); // ?????

	int nret = rbt_SetKinematicTrans(bset,nGroupIndex);
	return nret;
}