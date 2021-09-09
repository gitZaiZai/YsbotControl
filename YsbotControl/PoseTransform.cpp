// Copyright 2016, 亚思信息科技有限责任公司
// All right reserved.
//
// Author: Liu Run Dan ( lrdcz@163.com )
//
// Yasi Robot Motion Control System - 亚思机器人运动控制系统

#include "stdafx.h"
#include "PoseTransform.h"
#include "MotionControl.h"
#include "RobotKinematics.h"

PoseTransform::PoseTransform() : total_interpolation_point(400)
{
	default_tcp_list.resize(total_interpolation_point);
}

PoseTransform::~PoseTransform()
{
	;
}

void PoseTransform::set_interpolation_point_num( unsigned int pointnum)
{
	total_interpolation_point = pointnum;
	default_tcp_list.resize(total_interpolation_point);
}

void PoseTransform::calc_tcp_pose( const CartPos* rob, const CartRef* cref, const CartPos* tool, CartPos* tcp,  ATTITUDE_MODE am )
{
	// 	PosInfo rpi = SMC.get_position(PosInfo::CTmachine);
	if ( !rob )
		return;

	CartPos rpos  = *rob;
	// 	Matrix robmat = rpos.toHomogeneousMat();
	Matrix robmat = CartPosToHomogeneousMat(rpos,am);
	if ( cref == NULL ) // base frame, for robot origin.
	{
		if ( tool == NULL ) // For six axis end.
		{
			*tcp = rpos;
		}
		else // for special tool set.
		{
			CartPos toolcart  = *tool;
			Matrix toolmat    = CartPosToHomogeneousMat(toolcart,am); 
			Matrix robtoolmat = robmat * toolmat;
			// 			CartPos tcpcart(robtoolmat);
			CartPos tcpcart = MatrixToCartPos(robtoolmat,am);
			tcpcart.fig = rpos.fig;
			*tcp = tcpcart;
		}
	}
	else
	{
		// have tool reference user coordinate.
		CartPos workcart = cref->cpos;
		Matrix workmat = CartPosToHomogeneousMat(workcart,am); 
		Matrix workinv = pinv( workmat );
		Matrix tcpmat;
		if ( tool == NULL )
		{
			tcpmat = workinv * robmat;
		}
		else
		{
			CartPos toolcart  = *tool;
			Matrix toolmat    = CartPosToHomogeneousMat(toolcart,am);
			tcpmat = workinv * robmat * toolmat;
		}

		CartPos tcpcart = MatrixToCartPos(tcpmat,am);
		tcpcart.fig = rpos.fig;
		*tcp = tcpcart;
	}
}

void PoseTransform::calc_axisend_pose( const CartPos* tcp, const CartRef* cref, const CartPos* tool, CartPos* axisend,  ATTITUDE_MODE am )
{
	if ( !tcp )
		return;

	CartPos tcpcart = *tcp;
	Matrix tcpmat = CartPosToHomogeneousMat(tcpcart,am);

	if ( cref == NULL ) // base frame, for robot origin.
	{
		if ( tool == NULL ) // For six axis end.
		{
			*axisend = *tcp;
		}
		else // for special tool set.
		{
			CartPos toolcart  = *tool;
			Matrix toolmat    = CartPosToHomogeneousMat(toolcart,am);
			Matrix toolinv    = pinv(toolmat);
			Matrix axisendmat = tcpmat * toolinv;

			CartPos axisendcart = MatrixToCartPos(axisendmat,am);
			axisendcart.fig = tcpcart.fig;
			*axisend = axisendcart;
		}
	}
	else
	{
		// have tool reference user coordinate.
		CartPos workcart = cref->cpos;
		Matrix workmat   = CartPosToHomogeneousMat(workcart,am);
		Matrix axisendmat;
		if ( tool == NULL )
		{
			axisendmat = workmat * tcpmat;
		}
		else
		{
			CartPos toolcart  = *tool;
			Matrix toolmat    = CartPosToHomogeneousMat(toolcart,am);
			Matrix toolinv    = pinv(toolmat);

			axisendmat = workmat * tcpmat * toolinv;
		}

		CartPos axisendcart = MatrixToCartPos(axisendmat,am);
		axisendcart.fig = tcpcart.fig;
		*axisend = axisendcart;
	}
}

const std::vector<CartPos>& PoseTransform::get_rotate_tcp_path( 
	int    axis, 
	double theta,
	const CartPos* rob, 
	const CartRef* cref, 
	const CartPos* tool )
{
	default_tcp_list.clear();

	CartPos tcp;
	calc_tcp_pose(rob, cref,tool,&tcp);
	CartPos toolcart = tcp;
	Matrix toolmat   = CartPosToHomogeneousMat(toolcart); // 4*4

	Matrix rotm(4,4);

	rotm = get_rotate_matrix(axis,theta,cref,toolmat);

	Matrix tcprotnext(4,4);
	tcprotnext << fourbyfourident;

	CartPos nexttoolpose;
	CartPos nextaxisend;

	CartPos lastaxisend = *rob;

	for( unsigned int i = 0; i < total_interpolation_point; i++ )
	{
		tcprotnext       = toolmat * rotm;
		nexttoolpose     = MatrixToCartPos(tcprotnext);

		// 		nexttoolpose.pos = tcp.pos; ??????????????? check
		nexttoolpose.fig = rob->fig;

		calc_axisend_pose( &nexttoolpose, cref,tool, &nextaxisend );
		bool res = check_tool_pose(nextaxisend,lastaxisend);
		if ( !res )
		{
			cout << " ERR: rotate tcp pose not valid!\n";
			break;
		}

		default_tcp_list.push_back( nextaxisend );
		lastaxisend = nextaxisend;
		toolmat    = tcprotnext;

	}

	return default_tcp_list;
}

const std::vector<CartPos>& PoseTransform::get_rotate_tcp_path_emuler( 
	int    axis, 
	double arcLength,
	const CartPos* rob, 
	const CartRef* cref, 
	const CartPos* tool )
{
	default_tcp_list.clear();

	CartPos tcp;
	calc_tcp_pose(rob, cref,tool,&tcp,AM_EULER);
	Vec3D toolPos  = tcp.pos;

	if ( cref != NULL)
	{
		CartPos tcpBaseMcs;
		calc_tcp_pose(rob, NULL, tool,&tcpBaseMcs,AM_EULER);
		toolPos  = tcpBaseMcs.pos;
	}

	Vec3D robotPos = rob->pos;
	double arcRadius = ( toolPos - robotPos ).length();

	if ( arcRadius == 0 )
	{
		return default_tcp_list;
	}

	double theta = arcLength / arcRadius;

	CartPos toolcart = tcp;
	Matrix toolmat   = CartPosToHomogeneousMat(toolcart,AM_EULER); // 4*4

	Matrix rotm(4,4);

	rotm = get_rotate_matrix(axis,theta,cref,toolmat);

	Matrix tcprotnext(4,4);
	tcprotnext << fourbyfourident;

	CartPos nexttoolpose;
	CartPos nextaxisend;

	CartPos lastaxisend = *rob;

	for( unsigned int i = 0; i < total_interpolation_point; i++ )
	{
		tcprotnext       = toolmat * rotm;
		nexttoolpose     = MatrixToCartPos(tcprotnext,AM_EULER);

		// 		nexttoolpose.pos = tcp.pos; ??????????????? check
		nexttoolpose.fig = rob->fig;

		calc_axisend_pose( &nexttoolpose, cref, tool, &nextaxisend, AM_EULER );

		default_tcp_list.push_back( nextaxisend );
		lastaxisend = nextaxisend;
		toolmat     = tcprotnext;

	}

	return default_tcp_list;
}

ReturnMatrix PoseTransform::get_rotate_matrix( int naxis, double theta, const CartRef* cref, const Matrix& tcp )
{
	Matrix rotm(4,4);

	if ( cref ) // around fixed axis
	{
		Matrix tcpinv = pinv(tcp);
		ColumnVector kvec(3);
		switch( naxis )
		{
		case ROTATE_Z_AXIS: kvec = tcpinv.SubMatrix(1,3,1,1); break;
		case ROTATE_Y_AXIS: kvec = tcpinv.SubMatrix(1,3,2,2); break;
		case ROTATE_X_AXIS: kvec = tcpinv.SubMatrix(1,3,3,3); break;
		default: break;
		} 
		rotm = rotk(theta/total_interpolation_point,kvec);
	}
	else
	{
		switch( naxis )
		{
		case ROTATE_Z_AXIS: rotm = rotx(theta/total_interpolation_point); break;
		case ROTATE_Y_AXIS: rotm = roty(theta/total_interpolation_point); break;
		case ROTATE_X_AXIS: rotm = rotz(theta/total_interpolation_point); break;
		default: break;
		} 

	}
	rotm.Release();
	return rotm;
}

bool PoseTransform::check_tool_pose( const CartPos& currpos, const CartPos& lastpos)
{
	AxisPos curraxis,lastaxis;

	if ( KINE.inv_kine( lastpos, &lastaxis ) != 0 ||
		KINE.inv_kine( currpos, &curraxis ) != 0 )
	{
		return false;
	}

	AxisPos diffang = curraxis - lastaxis;
	double maxang = M_PI/4;
	if ( fabs(diffang.q4) > maxang || fabs(diffang.q5) > maxang || fabs(diffang.q6) > maxang ||
		fabs(curraxis.q4) > 120   || fabs(curraxis.q5) > 90 )
	{
		return false;
	}

	return true;
}

///////////////////   static method for pose transform //////////////

ReturnMatrix  PoseTransform::VecToTranslMat( const Vec3D& v) 
{
	ColumnVector pos(3); 
	Real org[] = {v.x,v.y,v.z};
	pos << org;
	return trans( pos );
}

ReturnMatrix PoseTransform::VecToRotMat(const Vec3D& v, ATTITUDE_MODE am )
{
	ColumnVector ang(3); 
	ang(1) = v.x;
	ang(2) = v.y;
	ang(3) = v.z;

	if ( am == AM_RPY )
	{
		return rpy( ang );
	}
	else
		return eulzyz(ang);
}

Vec3D PoseTransform::RotMatToVec(const Matrix& mat, ATTITUDE_MODE am)
{
	if ( am == AM_RPY )
	{
		ColumnVector rpyc = irpy(mat);
		return Vec3D( rpyc(1), rpyc(2), rpyc(3) );  
	}
	else
	{
		ColumnVector rpyc = ieulzyz(mat);
		return Vec3D( rpyc(1), rpyc(2), rpyc(3) );  
		//		return Vec3D( rpyc(3), rpyc(2), rpyc(1) );  
	}
}

ReturnMatrix PoseTransform::CartPosToHomogeneousMat(const CartPos& cpos, ATTITUDE_MODE am)
{
	Matrix tor(4,4);
	tor = VecToTranslMat(cpos.pos) * VecToRotMat( cpos.ang, am );
	tor.Release();
	return tor; 
}

ReturnMatrix PoseTransform::CartPosToTranslMat(const CartPos& cpos)
{
	return VecToTranslMat(cpos.pos);
}

ReturnMatrix PoseTransform::CartPosToRotMat(const CartPos& cpos, ATTITUDE_MODE am )
{
	return VecToRotMat(cpos.ang, am);
}

CartPos PoseTransform::MatrixToCartPos( const Matrix& mat, ATTITUDE_MODE am )
{
	CartPos cpos;
	if( mat.Ncols() == 4 && mat.Nrows() == 4 )
	{
		//Matrix rpym = mat.SubMatrix(1,3,1,3);
		cpos.pos = Vec3D( mat(1,4), mat(2,4), mat(3,4) );
		cpos.fig = -1;
		if ( am == AM_RPY )
		{
			ColumnVector rpyc = irpy(mat);
			cpos.ang = Vec3D( rpyc(1), rpyc(2), rpyc(3) ); // (xyz)
		}
		else
		{
			ColumnVector rpyc = ieulzyz(mat);
			cpos.ang = Vec3D( rpyc(1), rpyc(2), rpyc(3) ); // (zyz)
			//			cpos.ang = Vec3D( rpyc(3), rpyc(2), rpyc(1) ); // (zyz)
		}

	}
	return cpos;

}

ReturnMatrix PoseTransform::AxisPosToMatrix( const AxisPos& ap )
{
	ColumnVector tor(MAX_JOINT_NUM);
	tor = 0;
	tor(1) = ap.q1;
	tor(2) = ap.q2;
	tor(3) = ap.q3;
	tor(4) = ap.q4;
	tor(5) = ap.q5;
	tor(6) = ap.q6;
	tor.Release();
	return tor;
}

AxisPos PoseTransform::MatrixToAxisPos( const ColumnVector& mat )
{
	AxisPos ap;
	double* dfjnt[] = { &ap.q1, &ap.q2, &ap.q3, &ap.q4, &ap.q5, &ap.q6 };

	for ( int i = 0; i < mat.Nrows(); i++ )
		*(dfjnt[i]) = mat(i+1);
	return ap;
}