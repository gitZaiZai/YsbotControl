#ifndef pmc_base_h
#define pmc_base_h

#include "ConfigReader.h"
#include "CmdInfo.h"
#include "MotionCoreInfo.h"
#include "RobotKinematics.h"
#include "IoModuleParameters.h"

#include "PMC_8.86/MCSL.h"
#include "PMC_8.86/MCSL_Fun.h"
#include "Time.h"

struct PmcErrors
{
	int         nErrorCode;
	const char  lpszDesc[128];
};

static const PmcErrors errors [19] = 
{
	0xF102,		("������ͣ�쳣��ע�ⰲȫ������"),
	0xF104,		("��Ч��Բ����Բ�����"),
	0xF105,		("�����˶�ѧ�任����"),
	0xF106,		("�����˶�ѧ�任����"),
	0xF203,		("�����ٶȳ����޶�ֵ��"),
	0xF204,		("������ٶȳ����޶�ֵ��"),
	0xF301,		("J1�ᳬ������λ�ã�"),
	0xF302,		("J2�ᳬ������λ�ã�"),
	0xF303,		("J3�ᳬ������λ�ã�"),
	0xF304,		("J4�ᳬ������λ�ã�"),
	0xF305,		("J5�ᳬ������λ�ã�"),
	0xF306,		("J6�ᳬ������λ�ã�"),
	0xF501,		("In-position failed."),
	0xF801,		("Tracking error (J1)."),
	0xF802,		("Tracking error (J2)."),
	0xF803,		("Tracking error (J3)."),
	0xF804,		("Tracking error (J4)."),
	0xF805,		("Tracking error (J5)."),
	0xF806,		("Tracking error (J6)."),
};

class PmcBase
{
public:
	PmcBase(const ConfigReader&) throw (InvalidConfigurationException);
	~PmcBase(void);

	int  init_system ();
	int  close_system ();
	int  resetGrp ();
	void resetCmdIdx(); 
	int  powerGrp (bool);

	int  get_home_status ( int )      { return 0; }

	const int get_group_status (unsigned int = 0);
	const char* get_error ();

	const PosInfo&  get_position ( const PosInfo::CoordinateType = PosInfo::CTaxis, const LocationDataSource = LDScommad, WORD groupIndex = 0 ) throw(HardwareException);

 	const MotionCoreInfo& get_motion_information () throw ();
	inline double  getOverride ( ) { return  dfOverride; }
	const IoModuleParameters& get_mcc_io_parameters() const throw () { return mcc_io_param; }

	inline void setOverride ( const double& ovr = 100 ) {  dfOverride = ovr; }
	int   setPosition ( const CmdInfo& );
   
	int      nGroupIndex;
	unsigned int axis_num;

	static int GetRIOInputValue(WORD *pwValue, WORD wSet, WORD wPort, WORD wCardIndex = 0);
	static int SetRIOOutputValue(WORD wValue, WORD wSet, WORD wPort, WORD wCardIndex = 0);
	 // 20090723 James
	int EnquRIOOutputValue(WORD wBitMask, WORD wValue, WORD wSet, WORD wPort, double dfDelayTime = 0, WORD wGroupIndex = 0); 
	
	int GetGPIOInputValue(DWORD* pdwValue,WORD wCardIndex = 0);
	int SetGPIOOutputValue(DWORD dwValue, WORD wCardIndex = 0);
	int QueueGPIOOutputValue(DWORD dwBitMask, DWORD dwValue, double dfDelayTime = 0, WORD wGroupIndex = 0 );

	int SetPowerAxis(int axisID, bool enable );
	int SetPositionReady( bool enable );

 
private:
	const char* strerr;
 	bool        isSimulate;

 	MotionCoreInfo gi;
	double   dfOverride;
	IoModuleParameters mcc_io_param;
	PosInfo current_rloc;
	const char* GetErrorDescription (int nErrorCode);

	int  get_pos_servoCom ( double* );
 
	int  set_kinematicTrans ( int naxis, bool );

	static void _stdcall LIO_ISR_Function_PMC(LIOINT *pstINTSource);
	static void _stdcall RIO_ISR_Function_PMC(RIOINT *pstINTSource);
	static void _stdcall ALARM_ISR_Function_PMC(ALMINT *pstINTSource);

	Ysbot::Time timeTest;
};

#endif