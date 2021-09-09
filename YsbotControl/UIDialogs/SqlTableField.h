#ifndef _sql_table_field_h
#define _sql_table_field_h

#include "../../easySQLite/easySQLite/SqlCommon.h"
#include "../../easySQLite/easySQLite/SqlField.h"
#include "../../easySQLite/easySQLite/SqlDatabase.h"
#include "../../easySQLite/easySQLite/SqlTable.h"


enum TableType {
	TBLUser,
	TBLValInt,                   
	TBLValText,
	TBLValReal,
	TBLValBool,
	TBLValTime,
	TBLCartPose,
	TBLJointPose,
	TBLLogger,
	TBLNUM
};

static const char table_names [TBLNUM][23] = {        
	"tbUser",
	"tbValInt",
	"tbValText",
	"tbValReal",
	"tbValBool",
	"tbValTime",
	"tbCartPose",
	"tbJointPose",
	"tbLogger",

};

static sql::Field* definition(TableType tbType )
{
	static sql::Field user_def[] = 
	{
		sql::Field(sql::FIELD_KEY),
		sql::Field("userName",sql::type_text,sql::flag_not_null),
		sql::Field("userPassword",sql::type_text),
		sql::Field("grade",sql::type_int,sql::flag_not_null),
		sql::Field("administrator",sql::type_bool,sql::flag_not_null),
		sql::Field("state",sql::type_int,sql::flag_not_null),
		sql::Field(sql::DEFINITION_END),
	};

	static sql::Field int_def[] = 
	{
		sql::Field(sql::FIELD_KEY),
		sql::Field("varName",sql::type_text,sql::flag_not_null),
		sql::Field("varValue",sql::type_int,sql::flag_not_null),
		sql::Field("varDesc",sql::type_text),
		sql::Field("state",sql::type_int,sql::flag_not_null),
		sql::Field(sql::DEFINITION_END),
	};

	static sql::Field text_def[] = 
	{
		sql::Field(sql::FIELD_KEY),
		sql::Field("varName",sql::type_text,sql::flag_not_null),
		sql::Field("varValue",sql::type_text),
		sql::Field("varDesc",sql::type_text),
		sql::Field("state",sql::type_int),
		sql::Field(sql::DEFINITION_END),
	};

	static sql::Field real_def[] = 
	{
		sql::Field(sql::FIELD_KEY),
		sql::Field("varName",sql::type_text,sql::flag_not_null),
		sql::Field("varValue",sql::type_float,sql::flag_not_null),
		sql::Field("varDesc",sql::type_text),
		sql::Field("state",sql::type_int,sql::flag_not_null),
		sql::Field(sql::DEFINITION_END),
	};

	static sql::Field bool_def[] = 
	{
		sql::Field(sql::FIELD_KEY),
		sql::Field("varName",sql::type_text,sql::flag_not_null),
		sql::Field("varValue",sql::type_bool,sql::flag_not_null),
		sql::Field("varDesc",sql::type_text),
		sql::Field("state",sql::type_int,sql::flag_not_null),
		sql::Field(sql::DEFINITION_END),
	};

	static sql::Field time_def[] = 
	{
		sql::Field(sql::FIELD_KEY),
		sql::Field("varName",sql::type_text,sql::flag_not_null),
		sql::Field("varValue",sql::type_time,sql::flag_not_null),
		sql::Field("varDesc",sql::type_text),
		sql::Field("state",sql::type_int,sql::flag_not_null),
		sql::Field(sql::DEFINITION_END),
	};

	static sql::Field cartpose_def[] = 
	{
		sql::Field(sql::FIELD_KEY),
		sql::Field("varName",sql::type_text,sql::flag_not_null),
		sql::Field("x",sql::type_float,sql::flag_not_null),
		sql::Field("y",sql::type_float,sql::flag_not_null),
		sql::Field("z",sql::type_float,sql::flag_not_null),
		sql::Field("a",sql::type_float,sql::flag_not_null),
		sql::Field("b",sql::type_float,sql::flag_not_null),
		sql::Field("c",sql::type_float,sql::flag_not_null),
		sql::Field("fig",sql::type_int,sql::flag_not_null),
		sql::Field("a7",sql::type_float,sql::flag_not_null),
		sql::Field("a8",sql::type_float,sql::flag_not_null),
 		sql::Field("varDesc",sql::type_text),
		sql::Field("state",sql::type_int,sql::flag_not_null),
		sql::Field(sql::DEFINITION_END),
	};

	static sql::Field jntpose_def[] = 
	{
		sql::Field(sql::FIELD_KEY),
		sql::Field("varName",sql::type_text,sql::flag_not_null),
		sql::Field("a1",sql::type_float,sql::flag_not_null),
		sql::Field("a2",sql::type_float,sql::flag_not_null),
		sql::Field("a3",sql::type_float,sql::flag_not_null),
		sql::Field("a4",sql::type_float,sql::flag_not_null),
		sql::Field("a5",sql::type_float,sql::flag_not_null),
		sql::Field("a6",sql::type_float,sql::flag_not_null),
		sql::Field("a7",sql::type_float,sql::flag_not_null),
		sql::Field("a8",sql::type_float,sql::flag_not_null),
		sql::Field("varDesc",sql::type_text),
		sql::Field("state",sql::type_int,sql::flag_not_null),
		sql::Field(sql::DEFINITION_END),
	};

	static sql::Field log_def[] = 
	{
		sql::Field(sql::FIELD_KEY),
		sql::Field("type",sql::type_int,sql::flag_not_null),
		sql::Field("code",sql::type_int),
		sql::Field("state",sql::type_int),
		sql::Field("user",sql::type_text),
		sql::Field("behavior",sql::type_text),
		sql::Field("object",sql::type_text),
		sql::Field("detail",sql::type_text,sql::flag_not_null),
		sql::Field("time",sql::type_time,sql::flag_not_null),

		sql::Field(sql::DEFINITION_END),
	};

	switch (tbType)
	{
	case TBLUser: 	    return &user_def[0]; break;
	case TBLValInt: 	return &int_def[0];  break;
	case TBLValText: 	return &text_def[0]; break;
	case TBLValReal: 	return &real_def[0]; break;
	case TBLValBool: 	return &bool_def[0]; break;
	case TBLValTime: 	return &time_def[0]; break;
	case TBLCartPose: 	return &cartpose_def[0]; break;
	case TBLJointPose: 	return &jntpose_def[0];  break;
	case TBLLogger: 	return &log_def[0];      break;
	default: return NULL;
	}

}

#endif