#ifndef motion_control_dummy_h
#define motion_control_dummy_h

#include "MotionControltype.h"
#include "ConfigReader.h"

class MotionControlDummy :
	public MotionControlType
{
public:
	MotionControlDummy(const ConfigReader&) throw (InvalidConfigurationException);
	~MotionControlDummy(void);
   
	virtual int  init_system()  { return 0; }
	virtual int  close_system() { return 0; }
	virtual void set_override( const double =100 ) { ; }

	virtual const int  get_group_status( unsigned int grpIdx = 0 ) { return 1; }

	virtual int  setCmdInfo(const CmdInfo& )         { return 0; }

	virtual int  goHome( int )         { return 0; }
	virtual int  abortHome( )          { return 0; }
	virtual int  get_home_status(int )    { return 0; }

	virtual const char*    get_error()     { return 0; }
	virtual const PosInfo&  get_position(  const PosInfo::CoordinateType = PosInfo::CTaxis, const LocationDataSource = LDScommad )          { return pi; }
	virtual const MotionCoreInfo& get_motion_core_information () throw () { return gi; }

	virtual int  movePath( const CmdInfo* )  { return 0; }
  
	virtual const  IoModuleParameters& get_mcc_io_parameters() const throw () { return ip; }
	virtual void process_motion_information() {;}
	virtual int read_input ( unsigned short*, unsigned short =0  ) throw ()  { return 0; }
	virtual int write_word( unsigned short, unsigned short, unsigned short =0, bool inqueue = false, double dfDelayTime = 0 ) throw () { return 0; }
	virtual int read_word ( unsigned short, unsigned short*, unsigned short =0  ) throw () { return 0; }
	bool is_queue_full () {return false;}

private:
 	IoModuleParameters ip;
	MotionCoreInfo       gi;
	PosInfo         pi;
 	std::string strerr;
};

#endif