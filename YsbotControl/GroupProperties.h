#ifndef _group_properties
#define _group_properties

#include "ConfigReader.h"

#define MAX_ERRSTR_LEN	500
#define MAX_JOINT_NUM	(12) 

#define CARD_INDEX					0  // card index
#define INTERPOLATION_PERIOD		1  // ms
#define USE_AXIS_NUM				(MAX_AXIS_NUM) // defined in MCCL.h
#define MAX_JOINT_SPEED				(1.00 * M_PI) // radian/sec
#define EPSLION					    (1.0e-10)

#define MAX_SERVOSTR_LEN 200

typedef struct _MECH_PARAM
{
	unsigned int motor_type;
	unsigned int encoder_dir;
	unsigned int pulse_mode;
	bool    reverse;
	long    rpm;
	long    ppr;
	double  gear_ratio;
	double  high_limit;
	double  low_limit;

	long    encoder_origin;
	double	mechanical_origin;
	long	encoder_resolution;

	double  pitch;
	unsigned int UpSensorMode;
	unsigned int DownSensorMode;

} MECH_PARAM;

// MOTION_PROFILE

typedef struct _MOTION_PROFILE
{
	double  dfMaxJAccel; // deg/sec^2
 	double  dfMaxCSpeed; // mm/sec
	double  dfMaxCAccel; // mm/sec^2
	double  dfMaxExternalVel;

} MOTION_PROFILE;

typedef struct _ENCODER_PARAM
{              
	WORD    wType;
	WORD    wAInverse;
	WORD    wBInverse;
	WORD    wCInverse;
	WORD    wABSwap;
	WORD    wInputRate;
 } ENCODER_PARAM;

typedef struct _HOME_PARAM
{              
	WORD    wMode;   
	WORD    wDirection; 
	WORD    wSensorMode;
 
	int     nIndexCount;
 
	double  dfAccTime;
	double  dfDecTime;
	double  dfHighSpeed;
	double  dfLowSpeed;
	double  dfOffset;
} HOME_PARAM;

struct GroupProperties 
{
// 	unsigned int read ( const ConfigReader& ) throw (InvalidConfigurationException);

	GroupProperties () throw ();
	GroupProperties (const ConfigReader&) throw (InvalidConfigurationException);
	GroupProperties (const GroupProperties&) throw ();
	const GroupProperties& operator= (const GroupProperties&) throw ();
	~GroupProperties () throw ();

	// mechanism parameters
	MECH_PARAM  mp[MAX_JOINT_NUM];

	// motion profiles
	MOTION_PROFILE  profile;

  	ENCODER_PARAM encConfig[MAX_JOINT_NUM];
  	HOME_PARAM    homeConfig[MAX_JOINT_NUM]; 

	unsigned int axis_num;
	char servoName[MAX_JOINT_NUM][MAX_SERVOSTR_LEN];
};

 
#endif

