#ifndef _GroupPropertiesReadWriter_h
#define _GroupPropertiesReadWriter_h

#include "LuaScript/GenericValues.h"
#include "GroupProperties.h"

enum GroupParameterKind
{
	PARAMETER_AXIS      = 0,
	PARAMETER_MOTION    = 1,
	PARAMETER_ENCODER   = 2,
	PARAMETER_HOME      = 3,
 	PARAMETER_KIND_NUM  = 4,

};

class GroupPropertiesReadWriter
{
public:
	GroupPropertiesReadWriter(const ConfigReader&);
	virtual ~GroupPropertiesReadWriter(void);

	GenericValues* group_properties_descriptor[PARAMETER_KIND_NUM];
    
	void read_group_parameter( const GroupProperties&, const char *paramKind, const char *paramName, char *strValue ); // change parameter to string for show in UI.
	bool write_group_parameter( GroupProperties&, const char *paramKind, const char *paramName, const char *strValue ); // receive data from UI to change parameter.

	bool save_parameter_to_file(const GroupProperties&);

private:
	void init_group_descriptor();
    void read_axis_parameter( const GroupProperties&, const int axisID, const int paraID,  char* strValue );
	void read_motion_parameter( const GroupProperties&, const int paraID,  char* strValue );

	bool write_axis_parameter( GroupProperties&, const int axisID, const int paraID, const char* strValue );
	bool write_motion_parameter( GroupProperties&, const int paraID, const char* strValue );

	bool scan_parameter( const GroupProperties&, const char *paramKind, const char *paramName, char *strValue, bool bRead = true );

	bool save_axis_parameter(const GroupProperties&, const char *paramName, const int axisID, const int paraID);
	bool save_motion_parameter(const GroupProperties&, const char *paramName, const int paraID);

	ConfigReader& cfg;
};

#endif