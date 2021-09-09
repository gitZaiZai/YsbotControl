#ifndef pmc_jog_teach_h
#define pmc_jog_teach_h

#include "PmcBase.h"
#include "PoseTransform.h"
#include "CustomMotion.h"

namespace Pmc
{
 
class PmcJogTeach
{
public:
	PmcJogTeach(const ConfigReader&,  PmcBase&);
	~PmcJogTeach(void);

	void  set_teach_speed(double extVel);
 	int   grp_jog( const CmdInfo& );
 
	int jog_cartesian_coordinate( const CmdInfo& );
	int jog_joint_coordinate(const CmdInfo& );

// 	int rotate_tool_control_point( int nAxis, double theta, const CartPos* rob, const CartRef* cref, const CartPos* tool );

protected:
private:
	PmcBase& pmc_base;
	PoseTransform pose_trans;
	CustomMotion custom_motion;
};
 
}

#endif