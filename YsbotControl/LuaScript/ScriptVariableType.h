// Copyright 2016, 亚思信息科技有限责任公司
// All right reserved.
//
// Author: Liu Run Dan ( lrdcz@163.com )
//
// Yasi Robot Motion Control System - 亚思机器人运动控制系统

#ifndef _script_variable_type_h
#define _script_variable_type_h

#include "GenericValues.h"
#include <vector>
#include "../LuaScript/LuaBool.h"
#include "../LuaScript/LuaInteger.h"
#include "../LuaScript/LuaReal.h"
#include "../LuaScript/LuaString.h"

#define SPLIT_STR           (": ")
#define STR_LABEL_GLOBAL    ("Glo_")
#define STR_LABEL_SYSTEM    ("Sys_")
#define STR_LUA_NIL         ("nil")

#define SYSTEM_CARTREF_NAME ("Base")
#define SYSTEM_TOOL_NAME    ("Flange")

enum ScriptVarSetT 
{ 
	SVAR_SYSTEM = 0, 
	SVAR_GLOBAL, 
	SVAR_PROGRAM, 
	SVAR_SET_NUM, 
};

enum ScriptVarCategory 
{ 
	BASIC_KIND      = 0,               
	POSE_KIND       = 1,
	MOTION_KIND     = 2,
	COORDINATE_KIND = 3,
	SYSTEM_KIND     = 4,
	INOUT_KIND      = 5,
	CATEGORY_NUM    = 6,
};

enum ScriptVarT 
{ 
	svAXISPOS  = 0,               
	svCARTPOS  = 1,
	svINTEGER  = 2,
	svBOOL     = 3,
	svRDI      = 4,
	svRDO      = 5,
	svREAL     = 6,
	svDYNAMIC  = 7,
	svOVERLAP  = 8,
	svDIGITALINPUT  = 9,
	svDIGITALOUTPUT = 10,
	svCARTREF       = 11,
	svTOOL          = 12,
	svSTRING        = 13,
	svLUAINTEGER    = 14,
	svLUABOOL       = 15,
	svLUAREAL       = 16,
	svLUASTRING     = 17,
	svCONVEYOR      = 18,
	svSensableObject = 19,
	svSecurArea     = 20,
	svTypeNum       = 21,
};

struct VariableKey {
	VariableKey() throw ();
 	VariableKey (const VariableKey&) throw ();
 	const VariableKey& operator= (const VariableKey&) throw ();
	ScriptVarSetT varSet;
	ScriptVarT varTyp;
	ScriptVarT propTyp;
	char varName[VAR_STR_LEN]; 
	char propName[VAR_STR_LEN]; 
	char strKey[VAR_STR_LEN];
	int  nRow;
	bool first_members;
	bool valid;
};

/*****************************************************************************/
/********************   CLASS SCRIPTVARIABLETYPE   ***************************/
/*****************************************************************************/

class ScriptVariableType
{
public:
	ScriptVariableType();
	virtual ~ScriptVariableType();
	 
	GenericValues* vardesc[svTypeNum];
	const char* get_pose_lable() { return pose_label; }

	void get_prop( ScriptVarT sv, std::vector<std::string>& strprop, bool hasType = false );

	GenericValues* var_category[CATEGORY_NUM];
	const char*   getSeparatorStr (                            ) const;
	bool          setSeparatorStr ( char           *str        );
	bool          isBaseVariableType (  ScriptVarT sct  ) const;

private:
	char pose_label[VAR_STR_LEN];
 	void classify_var_category(GenericValues**);
	void init_varialbe_prop ();
	const char* getVarDescStr( ScriptVarT svt );
	char strSeparator[10];
	char strVariable[255];
};


#endif