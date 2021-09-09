// Copyright 2016, 亚思信息科技有限责任公司
// All right reserved.
//
// Author: Liu Run Dan ( lrdcz@163.com )
//
// Yasi Robot Motion Control System - 亚思机器人运动控制系统

#ifndef _pose_transform_h
#define _pose_transform_h

#include "CartPos.h"
#include "AxisPos.h"
#include <vector>

#define ROTATE_X_AXIS						0x0008
#define ROTATE_Y_AXIS						0x0010
#define ROTATE_Z_AXIS						0x0020
enum ATTITUDE_MODE
{
	AM_RPY = 0,
	AM_EULER,
};

class PoseTransform
{
public:
	PoseTransform();
	~PoseTransform();

	void calc_tcp_pose ( const CartPos* rob, const CartRef* cref, const CartPos* tool, CartPos* tcp, ATTITUDE_MODE am = AM_RPY );
	void calc_axisend_pose ( const CartPos* tcp, const CartRef* cref, const CartPos* tool, CartPos* axisend, ATTITUDE_MODE am = AM_RPY );

	const std::vector<CartPos>& get_rotate_tcp_path ( int axis, double theta, const CartPos* rob, const CartRef* cref, const CartPos* tool );

	const std::vector<CartPos>& get_rotate_tcp_path_emuler ( int axis, double arcLength, const CartPos* rob, const CartRef* cref, const CartPos* tool );

	static ReturnMatrix  CartPosToHomogeneousMat(const CartPos&,ATTITUDE_MODE am = AM_RPY );
	static ReturnMatrix  CartPosToTranslMat(const CartPos& );
	static ReturnMatrix  CartPosToRotMat(const CartPos&, ATTITUDE_MODE am = AM_RPY );
	static CartPos       MatrixToCartPos( const Matrix&, ATTITUDE_MODE am = AM_RPY );
	static ReturnMatrix  AxisPosToMatrix( const AxisPos& );
	static AxisPos       MatrixToAxisPos( const ColumnVector& );

	static ReturnMatrix  VecToTranslMat( const Vec3D& );
	static ReturnMatrix  VecToRotMat(const Vec3D&, ATTITUDE_MODE am = AM_RPY );
	static Vec3D         RotMatToVec(const Matrix&, ATTITUDE_MODE am = AM_RPY);

	void set_interpolation_point_num ( unsigned int );

private:
	std::vector<CartPos> default_tcp_list;
	unsigned int total_interpolation_point;
	ReturnMatrix get_rotate_matrix( int naxis, double theta, const CartRef* cref, const Matrix& tcp );

	bool check_tool_pose(const CartPos&, const CartPos&);
};

#endif
