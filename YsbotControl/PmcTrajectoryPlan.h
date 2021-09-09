// Copyright 2016, ��˼��Ϣ�Ƽ��������ι�˾
// All right reserved.
//
// Author: Liu Run Dan ( lrdcz@163.com )
//
// Yasi Robot Motion Control System - ��˼�������˶�����ϵͳ

#ifndef pmc_trajectoryplan_h
#define pmc_trajectoryplan_h

#include "PmcBase.h"
#include "PoseTransform.h"

typedef struct _SPEED_CONFIG
{
	double  dfProgSpeed; // 0.01 ~ 100
	BOOL    bJointMode;  // otherwise, Cartesian mode
	double  dfDistance[12];
} SPEED_CONFIG;

class PmcTrajectoryPlan
{
public:
	PmcTrajectoryPlan( const ConfigReader&, PmcBase&  );
	~PmcTrajectoryPlan(void);

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
    PmcBase& epcio;
	CmdInfo last_cmd;
 	PosInfo last_axis_pos;
	PoseTransform pose_transform;

	BOOL CheckCirCommand(const double* P,  // [in] start point
		const double* Q,  // [in] pass-through point
		const double* R,  // [in] end point
		double* O,        // [out] center point
		double* nv);      // [out] normal vector  


	const PosInfo&  get_last_axispos() throw();
	void  set_last_axispos(const PosInfo&) throw();
	double GetLinearFuzzyValue(const double& value, const double& left,const double& right ); 

};

#endif