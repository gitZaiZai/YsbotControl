// Copyright 2016, 亚思信息科技有限责任公司
// All right reserved.
//
// Author: Liu Run Dan ( lrdcz@163.com )
//
// Yasi Robot Motion Control System - 亚思机器人运动控制系统

#ifndef trajectory_plan_h
#define trajectory_plan_h

#include "EpcioBase.h"

typedef struct _SPEED_CONFIG
{
	double  dfProgSpeed; // 0.01 ~ 100
	BOOL    bJointMode;  // otherwise, Cartesian mode
	double  dfDistance[12];
} SPEED_CONFIG;

class TrajectoryPlan
{
public:
	TrajectoryPlan( const ConfigReader&, EpcioBase&  );
	~TrajectoryPlan(void);

	int move_line(   const CmdInfo& );
	int move_ptp(    const CmdInfo& );
	int move_cir(    const CmdInfo& );
	int move_spline( const CmdInfo& );
 	int wait_io(     const CmdInfo& );
	bool  set_speed( const SPEED_CONFIG& psc );

	int  blendGrp (bool);
	int  haltGrp ();
	int  continueGrp ();
	int  stopGrp ( const double= 500 );
	int  delaying (long int= 100);
	int  goHome ( int )               { return 0; }
	int  abortHome ()                 { return 0; }
	int  movePath ( const CmdInfo* )  { return 0; }

private:
    EpcioBase& epcio;
	CmdInfo last_cmd;
 	PosInfo last_axis_pos;

	BOOL CheckCirCommand(const double* P,  // [in] start point
		const double* Q,  // [in] pass-through point
		const double* R,  // [in] end point
		double* O,        // [out] center point
		double* nv);      // [out] normal vector  


	const PosInfo&  get_last_axispos() throw();
	void  set_last_axispos(const PosInfo&) throw();
};

#endif