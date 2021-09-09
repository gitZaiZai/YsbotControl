#include "StdAfx.h"
#include "GroupProperties.h"
#include "CartPos.h"
#include <stdio.h>
using namespace std;

GroupProperties::GroupProperties( const ConfigReader& vread )  throw (InvalidConfigurationException)
{
	memset( mp, 0, sizeof(MECH_PARAM) * MAX_JOINT_NUM );
	memset( encConfig, 0, sizeof(ENCODER_PARAM) * MAX_JOINT_NUM );
	memset( homeConfig, 0, sizeof(HOME_PARAM) * MAX_JOINT_NUM );
	memset( &profile, 0, sizeof(profile) );

	CString str(_T(""));
	double dfTmp = 0.0;
	int tmp = 0;
	unsigned int num = 0;

	std::vector<std::string> strAxis;
	strAxis.clear();

	vread.get("AXES", strAxis);
	if ( strAxis.size() == 0 )
		throw InvalidConfigurationException ("Axis");
	else
		num = strAxis.size();

	unsigned int iTemp = 0;
	std::string servostr = "";
	bool bTemp = false;
	for (unsigned int j = 0; j < num; j++ )
	{
		if ( vread.get((strAxis[j]+string("::servo_type")).c_str(), iTemp)  )
			mp[j].motor_type = iTemp;

		if ( vread.get((strAxis[j]+string("::servo_type")).c_str(), servostr) <= 0 )
		{
			sprintf(servoName[j],"Dummy");
		}
		else
			strcpy(servoName[j],servostr.c_str());

		if ( vread.get((strAxis[j]+string("::encoder_dir")).c_str(), iTemp)  )
			mp[j].encoder_dir = iTemp;
		if ( vread.get((strAxis[j]+string("::pulse_mode")).c_str(), iTemp)  )
			mp[j].pulse_mode = iTemp;
		if ( vread.get((strAxis[j]+string("::reverse")).c_str(), bTemp)  )
			mp[j].reverse = bTemp;
		if ( vread.get((strAxis[j]+string("::rated_speed")).c_str(), iTemp)  )
			mp[j].rpm = iTemp;
		if ( vread.get((strAxis[j]+string("::driver_ppr")).c_str(), iTemp)  )
			mp[j].ppr = iTemp;
		if ( vread.get((strAxis[j]+string("::gear_ratio")).c_str(), dfTmp)  )
			mp[j].gear_ratio = dfTmp;
		if ( vread.get((strAxis[j]+string("::positive_limit")).c_str(), dfTmp)  )
			mp[j].high_limit = dfTmp;
		if ( vread.get((strAxis[j]+string("::negative_limit")).c_str(), dfTmp)  )
			mp[j].low_limit = dfTmp;

		if ( vread.get((strAxis[j]+string("::encoder_origin")).c_str(), iTemp)  )
			mp[j].encoder_origin = iTemp;
		if ( vread.get((strAxis[j]+string("::mechanical_origin")).c_str(), dfTmp)  )
			mp[j].mechanical_origin = dfTmp;
		if ( vread.get((strAxis[j]+string("::encoder_resolution")).c_str(), iTemp)  )
			mp[j].encoder_resolution = iTemp;

		if ( vread.get((strAxis[j]+string("::pitch")).c_str(), dfTmp)  )
			mp[j].pitch = dfTmp;
		//////////////// hard limit config parameters ///////////////////////////////////// 
		if ( vread.get((strAxis[j]+string("::UpSensorMode")).c_str(), iTemp)  )
			mp[j].UpSensorMode = iTemp;
		if ( vread.get((strAxis[j]+string("::DownSensorMode")).c_str(), iTemp)  )
			mp[j].DownSensorMode = iTemp;

		//////////////// home config parameters ///////////////////////////////////// 
		if ( vread.get((strAxis[j]+string("::Mode")).c_str(), iTemp)  )
			homeConfig[j].wMode = iTemp;
		if ( vread.get((strAxis[j]+string("::Direction")).c_str(), iTemp)  )
			homeConfig[j].wDirection = iTemp;
		if ( vread.get((strAxis[j]+string("::SensorMode")).c_str(), iTemp)  )
			homeConfig[j].wSensorMode = iTemp;
		if ( vread.get((strAxis[j]+string("::IndexCount")).c_str(), tmp)  )
			homeConfig[j].nIndexCount = tmp;
		if ( vread.get((strAxis[j]+string("::AccTime")).c_str(), dfTmp)  )
			homeConfig[j].dfAccTime = dfTmp;
		if ( vread.get((strAxis[j]+string("::DecTime")).c_str(), dfTmp)  )
			homeConfig[j].dfDecTime = dfTmp;
		if ( vread.get((strAxis[j]+string("::HighSpeed")).c_str(), dfTmp)  )
			homeConfig[j].dfHighSpeed = dfTmp;
		if ( vread.get((strAxis[j]+string("::LowSpeed")).c_str(), dfTmp)  )
			homeConfig[j].dfLowSpeed = dfTmp;
		if ( vread.get((strAxis[j]+string("::Offset")).c_str(), dfTmp)  )
			homeConfig[j].dfOffset = dfTmp;

		//////////////// Encoder config parameters ///////////////////////////////////// 
		if ( vread.get((strAxis[j]+string("::ENC_Type")).c_str(), iTemp)  )
			encConfig[j].wType = iTemp;
		if ( vread.get((strAxis[j]+string("::ENC_AInverse")).c_str(), iTemp)  )
			encConfig[j].wAInverse = iTemp;
		if ( vread.get((strAxis[j]+string("::ENC_BInverse")).c_str(), iTemp)  )
			encConfig[j].wBInverse = iTemp;
		if ( vread.get((strAxis[j]+string("::ENC_CInverse")).c_str(), iTemp)  )
			encConfig[j].wCInverse = iTemp;
		if ( vread.get((strAxis[j]+string("::ENC_ABSwap")).c_str(), iTemp)  )
			encConfig[j].wABSwap = iTemp;
		if ( vread.get((strAxis[j]+string("::ENC_InputRate")).c_str(), iTemp)  )
			encConfig[j].wInputRate = iTemp;
	}

	if (vread.get( "MOTION::max_joint_acc", dfTmp ) > 0)
		profile.dfMaxJAccel = dfTmp; // deg/sec^2
	else
		throw InvalidConfigurationException ("MOTION::max_joint_acc");

	if (vread.get( "MOTION::max_cartesian_vel", dfTmp ) > 0)
		profile.dfMaxCSpeed = dfTmp; // mm/sec
	if (vread.get( "MOTION::max_cartesian_acc", dfTmp ) > 0)
		profile.dfMaxCAccel = dfTmp; 
	if (vread.get( "MOTION::max_external_vel", dfTmp ) > 0)
		profile.dfMaxExternalVel = dfTmp; 

	strAxis.clear();
	axis_num = num;
}
  
GroupProperties::GroupProperties () throw ()
{
	memset( mp, 0, sizeof(MECH_PARAM) * MAX_JOINT_NUM );
	memset( encConfig, 0, sizeof(ENCODER_PARAM) * MAX_JOINT_NUM );
	memset( homeConfig, 0, sizeof(HOME_PARAM) * MAX_JOINT_NUM );
	memset( &profile, 0, sizeof(MOTION_PROFILE) );
	memset( servoName, '\0', sizeof(char) * MAX_SERVOSTR_LEN *  MAX_JOINT_NUM );

	for (int j = 0; j < MAX_JOINT_NUM; j++ )
	{
		mp[j].motor_type	= 0;
		mp[j].encoder_dir	= 0;
		mp[j].pulse_mode	= 1;
		mp[j].reverse		= false;
		mp[j].rpm			= 3000;
		mp[j].ppr			= 8000;
		mp[j].gear_ratio	= 100;
		mp[j].high_limit	= 120;
		mp[j].low_limit	= -120;

		mp[j].encoder_origin     = 0;
		mp[j].mechanical_origin  = 0.0;
		mp[j].encoder_resolution = 131072;

		mp[j].pitch = 2 * M_PI;
		mp[j].UpSensorMode   = 2;
		mp[j].DownSensorMode = 2; // SL_UNUSED 
	}

	profile.dfMaxJAccel = 1500;
	profile.dfMaxCSpeed = 2000;
	profile.dfMaxCAccel = 2000;
	profile.dfMaxExternalVel = 20;
	axis_num = 0;
}

GroupProperties::~GroupProperties () throw () 
{
	;
}

GroupProperties::GroupProperties (const GroupProperties& gp) throw ()
{
	operator= (gp);
}

const GroupProperties& GroupProperties::operator= (const GroupProperties& fg) throw () 
{
	memcpy( mp, fg.mp, sizeof(MECH_PARAM) * MAX_JOINT_NUM );
	memcpy( encConfig, fg.encConfig, sizeof(ENCODER_PARAM) * MAX_JOINT_NUM );
	memcpy( homeConfig, fg.homeConfig, sizeof(HOME_PARAM) * MAX_JOINT_NUM );
	memcpy( &profile, &fg.profile, sizeof(MOTION_PROFILE) );

	axis_num = fg.axis_num;
	memcpy(servoName,fg.servoName,sizeof(char) * MAX_SERVOSTR_LEN * MAX_JOINT_NUM);
	return *this;
}
	 