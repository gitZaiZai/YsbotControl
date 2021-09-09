#ifndef custom_motion_h
#define custom_motion_h
#include "PoseTransform.h"
#include "PmcBase.h"

namespace Pmc
{
 	typedef struct _CONE
	{
		double  pt_start[3];   // wrist center start point
		double  vec_center[3]; // center vector
		double  vec_start[3];  // start vector
		double  vec_normal[3]; // normal vector
		double  vec_U[3];
		double  theta;
		double  radius;
		Matrix  mat_prev;
		Matrix  mat_post;

		_CONE()
		{
			mat_prev.resize(3, 3);
			mat_post.resize(3, 3);
		}

	} CONE;

    #define TOTALPOINT 500
	typedef struct _point
	{
		CartPos   rp;
		double Interval;
	} POINT1 ;

	typedef struct _toolrotate
	{
		int     TotalPoint;
		POINT1  Trajectory[TOTALPOINT];
	} TOOLROTATE ;

class CustomMotion
{
public:
	CustomMotion(PmcBase&);
	virtual ~CustomMotion();

	int	 rbt_RX_V6(	double dfTheta, WORD wGroupIndex = 0);
	int	 rbt_RY_V6(	double dfTheta, WORD wGroupIndex = 0);
	int	 rbt_RZ_V6(	double dfTheta, WORD wGroupIndex = 0);
	int	 rbt_Cone_V6(double dfBaseVectX,
		double dfBaseVectY,
		double dfBaseVectZ,
		double dfTheta,
		WORD wGroupIndex /*=0*/);
 
	int  rotate_tool_control_point( int nAxis, double theta, const CartPos* rob, const CartRef* cref, const CartPos* tool );

private:
	PoseTransform pose_trans;
	PmcBase& pmc_base;
	TOOLROTATE*  pTool;
};

}

#endif