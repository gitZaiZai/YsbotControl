#include "StdAfx.h"
#include "GroupPropertiesReadWriter.h"
#include "stringconvert.h"
#include "WorldModel.h"

using namespace std;

GroupPropertiesReadWriter::GroupPropertiesReadWriter( const ConfigReader& vr) : cfg(const_cast<ConfigReader&>(vr))
{
	for ( int i=0; i < PARAMETER_KIND_NUM; ++i)
		group_properties_descriptor[i] = NULL;

	init_group_descriptor();
}


GroupPropertiesReadWriter::~GroupPropertiesReadWriter(void)
{
	for ( int i=0; i < PARAMETER_KIND_NUM; ++i)
	{
		if ( group_properties_descriptor[i])
		{
			delete group_properties_descriptor[i];
			group_properties_descriptor[i] = NULL;
		}
	}
}

void GroupPropertiesReadWriter::init_group_descriptor()
{
	/////////////////////////////////////////////////
	group_properties_descriptor[PARAMETER_AXIS] = new GenericValues("AXIS",15);
	group_properties_descriptor[PARAMETER_AXIS]->addSetting("1", "servo_type: STRING");
	group_properties_descriptor[PARAMETER_AXIS]->addSetting("2", "encoder_dir: INT");
	group_properties_descriptor[PARAMETER_AXIS]->addSetting("3", "pulse_mode: INT");
	group_properties_descriptor[PARAMETER_AXIS]->addSetting("4", "reverse: BOOL");
	group_properties_descriptor[PARAMETER_AXIS]->addSetting("5", "rated_speed: INT");
	group_properties_descriptor[PARAMETER_AXIS]->addSetting("6", "driver_ppr: INT");
	group_properties_descriptor[PARAMETER_AXIS]->addSetting("7", "gear_ratio: REAL");
	group_properties_descriptor[PARAMETER_AXIS]->addSetting("8", "positive_limit: REAL");
	group_properties_descriptor[PARAMETER_AXIS]->addSetting("9", "negative_limit: REAL");
	group_properties_descriptor[PARAMETER_AXIS]->addSetting("10", "encoder_origin: INT");
	group_properties_descriptor[PARAMETER_AXIS]->addSetting("11", "mechanical_origin: INT");
	group_properties_descriptor[PARAMETER_AXIS]->addSetting("12", "encoder_resolution: INT");
	group_properties_descriptor[PARAMETER_AXIS]->addSetting("13", "pitch: REAL");
	group_properties_descriptor[PARAMETER_AXIS]->addSetting("14", "UpSensorMode: INT");
	group_properties_descriptor[PARAMETER_AXIS]->addSetting("15", "DownSensorMode: INT");

	//////////////////////////////////////////////////////////////////////////
	group_properties_descriptor[PARAMETER_MOTION] = new GenericValues("MOTION",4);
	group_properties_descriptor[PARAMETER_MOTION]->addSetting("1", "max_joint_acc: REAL");
	group_properties_descriptor[PARAMETER_MOTION]->addSetting("2", "max_cartesian_vel: REAL");
	group_properties_descriptor[PARAMETER_MOTION]->addSetting("3", "max_cartesian_acc: REAL");
	group_properties_descriptor[PARAMETER_MOTION]->addSetting("4", "max_external_vel: REAL");

	//////////////////////////////////////////////////////////////////////////
	group_properties_descriptor[PARAMETER_ENCODER] = new GenericValues("ENCODER",0);

	//---------------------------------------
	group_properties_descriptor[PARAMETER_HOME]    = new GenericValues("HOME",0);
}

void GroupPropertiesReadWriter::read_group_parameter( const GroupProperties& group, const char *paramKind, const char *paramName, char *strValue )
{
	if ( !strValue )
 		return;

	scan_parameter( group, paramKind, paramName, strValue, true );
}

void GroupPropertiesReadWriter::read_motion_parameter( const GroupProperties& group, const int paraID, char* strValue )
{
 	switch( paraID )
	{
	case 1: sprintf( strValue, "%.2f", group.profile.dfMaxJAccel );      break;
	case 2: sprintf( strValue, "%.2f", group.profile.dfMaxCSpeed );      break;
	case 3: sprintf( strValue, "%.2f", group.profile.dfMaxCAccel );      break;
	case 4: sprintf( strValue, "%.2f", group.profile.dfMaxExternalVel ); break;
 	default: break;
	}
}

void GroupPropertiesReadWriter::read_axis_parameter( const GroupProperties& group, const int axisID, const int paraID, char* strValue )
{
	switch( paraID )
	{
	case 1: sprintf( strValue, "%s", group.servoName[axisID] );      break;
	case 2: sprintf( strValue, "%d", group.mp[axisID].encoder_dir ); break;
	case 3: sprintf( strValue, "%d", group.mp[axisID].pulse_mode );  break;
	case 4: sprintf( strValue, "%s", group.mp[axisID].reverse ? "true" : "false" );     break;
	case 5: sprintf( strValue, "%d", group.mp[axisID].rpm );         break;
	case 6: sprintf( strValue, "%d", group.mp[axisID].ppr );         break;
	case 7: sprintf( strValue, "%.2f", group.mp[axisID].gear_ratio ); break;
	case 8: sprintf( strValue, "%.2f", group.mp[axisID].high_limit ); break;
	case 9: sprintf( strValue, "%.2f", group.mp[axisID].low_limit );  break;
	case 10: sprintf( strValue, "%d", group.mp[axisID].encoder_origin );     break;
	case 11: sprintf( strValue, "%d", group.mp[axisID].mechanical_origin );  break;
	case 12: sprintf( strValue, "%d", group.mp[axisID].encoder_resolution ); break;
	case 13: sprintf( strValue, "%.2f", group.mp[axisID].pitch );             break;
	case 14: sprintf( strValue, "%d", group.mp[axisID].UpSensorMode );       break;
	case 15: sprintf( strValue, "%d", group.mp[axisID].DownSensorMode );     break;
	default: break;
	}
}

bool GroupPropertiesReadWriter::write_group_parameter( GroupProperties& group,  const char *paramKind, const char *paramName, const char *strValue )
{
	if ( !strValue )
		return false;

	scan_parameter( group, paramKind, paramName, const_cast<char*>(strValue), false );

	return true;
}

bool GroupPropertiesReadWriter::write_axis_parameter( GroupProperties& group, const int axisID, const int paraID, const char* strValue )
{
	bool bReturn = true, b;
	int numBerType = is_number(strValue);

	if ( paraID == 2 || paraID == 3 || paraID == 5 || paraID == 6 || 
		 paraID == 10 || paraID == 12 || paraID == 14 || paraID == 15 )
	{
		if ( numBerType != 1 ) // Integer
 			return false;
 	}
	else if ( paraID == 7 || paraID == 8 || paraID == 9 || paraID == 11 || paraID == 13  )
	{
		if ( numBerType != 3 )
 			return false;
 	}
	else if( paraID == 1 || paraID == 4 ) // string
	{
		if ( numBerType != 0 )
			return false;
 	}


	switch( paraID )
	{
	case 1: strcpy( group.servoName[axisID], strValue );     break;
	case 2: group.mp[axisID].encoder_dir = atoi(strValue);   break;
	case 3: group.mp[axisID].pulse_mode  = atoi(strValue);   break;
	case 4: 
		b = false;
		if( !strValue )
			;
		else if( isdigit( strValue[ 0 ] ) )
			b = atoi( strValue ? strValue : 0 );
 		else if( strcmp( strValue, "on"  ) == 0 ||
			strcmp( strValue, "true") == 0 ||
			strcmp( strValue, "yes" ) == 0 )
 			b = true;
 		group.mp[axisID].reverse = ( b == true ) ? true : false;
		break;
	case 5: group.mp[axisID].rpm        = atol( strValue ); break;
	case 6: group.mp[axisID].ppr        = atol( strValue ); break;
	case 7: group.mp[axisID].gear_ratio = atof( strValue ); break;
	case 8: group.mp[axisID].high_limit = atof( strValue ); break;
	case 9: group.mp[axisID].low_limit  = atof( strValue ); break;
	case 10: group.mp[axisID].encoder_origin     = atol(strValue);   break;
	case 11: group.mp[axisID].mechanical_origin  = atof( strValue ); break;
	case 12: group.mp[axisID].encoder_resolution = atol( strValue ); break;
	case 13: group.mp[axisID].pitch              = atof( strValue ); break;
	case 14: group.mp[axisID].UpSensorMode       = atoi( strValue ); break;
	case 15: group.mp[axisID].DownSensorMode     = atoi( strValue ); break;
	default: break;
	}
    return true;
}

bool GroupPropertiesReadWriter::write_motion_parameter( GroupProperties& group, const int paraID, const char* strValue )
{
	switch( paraID )
	{
	case 1: group.profile.dfMaxJAccel = atof( strValue );      break;
	case 2: group.profile.dfMaxCSpeed = atof( strValue );      break;
	case 3: group.profile.dfMaxCAccel = atof( strValue );      break;
	case 4: group.profile.dfMaxExternalVel = atof( strValue ); break;
	default: break;
	}
	return true;
}

bool GroupPropertiesReadWriter::scan_parameter( const GroupProperties& group, const char *paramKind, const char *paramName, char *strValue,  bool bRead )
{
	bool isvalid = false;
	char paramInfo[VAR_STR_LEN];
	if ( strstr( paramKind, group_properties_descriptor[PARAMETER_AXIS]->getClassName() ) )
	{
		std::deque<std::string> strlist;
		split_string( strlist, paramKind, " " );
		int axis_id = 0;
		if ( strlist.size() > 1)
		{
			string2int(axis_id,strlist[1]);
			axis_id = axis_id - 1;
		}

		for ( int j = 0; j < group_properties_descriptor[PARAMETER_AXIS]->getValuesTotal(); j++ )
		{
			group_properties_descriptor[PARAMETER_AXIS]->getValue(j+1,paramInfo);
			if ( strcmp( paramName, paramInfo ) == 0 )
			{
				if ( bRead )
 					read_axis_parameter( group, axis_id, j+1, strValue );
 				else
				    isvalid = write_axis_parameter( const_cast<GroupProperties&>(group), axis_id, j+1, strValue );
				break;
			}
		}

		strlist.clear();
	}
	else if ( strstr( paramKind, group_properties_descriptor[PARAMETER_MOTION]->getClassName() ) )
	{
		for ( int j = 0; j < group_properties_descriptor[PARAMETER_MOTION]->getValuesTotal(); j++ )
		{
			group_properties_descriptor[PARAMETER_MOTION]->getValue(j+1,paramInfo);
			if ( strcmp( paramName, paramInfo ) == 0 )
			{
				if ( bRead )
					read_motion_parameter( group, j+1, strValue );
				else
					isvalid = write_motion_parameter( const_cast<GroupProperties&>(group), j+1, strValue );
 				break;
			}
		}
	}
	return isvalid;
}

bool GroupPropertiesReadWriter::save_parameter_to_file(const GroupProperties& gp)
{
	int aixsNum = MWM.get_group_properties().axis_num;
	string strPara = ""; 
	string strFiled = group_properties_descriptor[PARAMETER_AXIS]->getClassName();
	char strprefix[VAR_STR_LEN];
	char paramInfo[VAR_STR_LEN];

	for ( int i = 0; i < aixsNum; ++i )
	{
		sprintf(strprefix,"_%d",i+1);
		for ( int j = 0; j < group_properties_descriptor[PARAMETER_AXIS]->getValuesTotal(); j++ )
		{
	         group_properties_descriptor[PARAMETER_AXIS]->getValue(j+1,paramInfo);
			 std::deque<std::string> strlist;
			 split_string( strlist, paramInfo, ": " );
			 strPara = strFiled + strprefix+"::"+strlist[0];
			 save_axis_parameter( gp, strPara.c_str(), i, j+1 );
			 strlist.clear();
		}
	}

	//------------------------------------------------------------------
	strFiled = group_properties_descriptor[PARAMETER_MOTION]->getClassName();
	for ( int j = 0; j < group_properties_descriptor[PARAMETER_MOTION]->getValuesTotal(); j++ )
	{
		group_properties_descriptor[PARAMETER_MOTION]->getValue(j+1,paramInfo);
		std::deque<std::string> strlist;
		split_string( strlist, paramInfo, ": " );
		strPara = strFiled+"::"+strlist[0];
		save_motion_parameter( gp, strPara.c_str(), j+1 );
		strlist.clear();
	}

	cfg.write_all();

	return true;
}

bool GroupPropertiesReadWriter::save_axis_parameter( const GroupProperties& group, const char *paramName, const int axisID, const int paraID )
{
	switch( paraID )
	{
	case 1: cfg.set( paramName, group.servoName[axisID] );      break;
	case 2: cfg.set( paramName, group.mp[axisID].encoder_dir ); break;
	case 3: cfg.set( paramName, group.mp[axisID].pulse_mode );  break;
	case 4: cfg.set( paramName, group.mp[axisID].reverse );     break;
	case 5: cfg.set( paramName, group.mp[axisID].rpm );         break;
	case 6: cfg.set( paramName, group.mp[axisID].ppr );         break;
	case 7: cfg.set( paramName, group.mp[axisID].gear_ratio ); break;
	case 8: cfg.set( paramName, group.mp[axisID].high_limit ); break;
	case 9: cfg.set( paramName, group.mp[axisID].low_limit );  break;
	case 10: cfg.set( paramName, group.mp[axisID].encoder_origin );     break;
	case 11: cfg.set( paramName, group.mp[axisID].mechanical_origin );  break;
	case 12: cfg.set( paramName, group.mp[axisID].encoder_resolution ); break;
	case 13: cfg.set( paramName, group.mp[axisID].pitch );             break;
	case 14: cfg.set( paramName, group.mp[axisID].UpSensorMode );       break;
	case 15: cfg.set( paramName, group.mp[axisID].DownSensorMode );     break;
	default: break;
	}
	return true;
}

bool GroupPropertiesReadWriter::save_motion_parameter(const GroupProperties& group, const char *paramName, const int paraID)
{
	switch( paraID )
	{
	case 1: cfg.set( paramName, group.profile.dfMaxJAccel );     break;
	case 2: cfg.set( paramName, group.profile.dfMaxCSpeed  );    break;
	case 3: cfg.set( paramName, group.profile.dfMaxCAccel );     break;
	case 4: cfg.set( paramName, group.profile.dfMaxExternalVel );break;
	default: break;
	}

	return true;
}