#ifndef pmc_motion_control_h
#define pmc_motion_control_h

#include "MotionControltype.h"
#include "ConfigReader.h"
#include "PmcJogTeach.h"
#include "PmcTrajectoryPlan.h"

class PmcMotionControl : public MotionControlType
{
public:
	PmcMotionControl(const ConfigReader&) throw (InvalidConfigurationException);
	~PmcMotionControl(void);

	inline int  init_system();
	inline int  close_system();
	inline void set_override( const double od = 100 ) { base_pmc.setOverride(od); }

	int  goHome( int )  { return 0; }
	int  abortHome( )   { return 0; }
	inline int  get_home_status( int )    { return 0; }

	int  movePath( const CmdInfo* )  { return 0; }
	inline int  setCmdInfo(const CmdInfo& );

 	inline const MotionCoreInfo& get_motion_core_information () throw () { return base_pmc.get_motion_information(); }
    inline const int  get_group_status(unsigned int grpIdx = 0) { return base_pmc.get_group_status(grpIdx); }

	inline const char*  get_error() { return base_pmc.get_error(); }
	inline const PosInfo&  get_position( const PosInfo::CoordinateType pct= PosInfo::CTaxis, const LocationDataSource lds= LDScommad ) throw(YsbotsException); 
 
	inline const  IoModuleParameters& get_mcc_io_parameters() const throw () { return base_pmc.get_mcc_io_parameters(); }

	int read_input ( unsigned short*, unsigned short =0  ) throw ();
	int write_word (unsigned short, unsigned short, unsigned short =0, bool inqueue = false, double dfDelayTime = 0  ) throw ();
	int read_word ( unsigned short, unsigned short*, unsigned short =0  ) throw ();
	bool is_queue_full () {return false;}

	CmdInfo  cmd_info;
 
private:
	PmcBase           base_pmc; // Don't change the order of the following three objects created. 
	Pmc::PmcJogTeach  jog_pmc;
	PmcTrajectoryPlan tp_pmc;

	unsigned int axis_num;
};

#endif