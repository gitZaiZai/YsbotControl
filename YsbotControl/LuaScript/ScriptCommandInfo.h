// Copyright 2016, 亚思信息科技有限责任公司
// All right reserved.
//
// Author: Liu Run Dan ( lrdcz@163.com )
//
// Yasi Robot Motion Control System - 亚思机器人运动控制系统

#ifndef _script_command_info_h
#define _script_command_info_h

#include "GenericValues.h"
#include "ScriptVariableType.h"
#include <vector>

enum ScriptCommandSetT 
{ 
	SM_SET_MOVEMENT,               
	SM_SET_SETTING,
	SM_SET_SYSTEM,
	SM_SET_SYSFUNCTIONS,
	SM_SET_INPUTOUTPUT,
	SM_SET_DECLARATION,
	SM_SET_CUSTOMFUNCTIONS,
	SM_MAX_SETS,    /*!< maximum nr of script command sets     */ // 7
};

enum ScriptCommandT 
{ 
	SM_PTPABS,               
	SM_PTPREL,               
	SM_LINEABS,
	SM_LINEREL,
 	SM_CIRCLEABS,
	SM_CIRCLEREL,
 	SM_WAITTIME,
	SM_WAITISFINISHED,

	SM_DYNAMIC,
	SM_OVERLAP,
	SM_REFSYS,
	SM_TOOL,
	SM_SPEED,
	SM_BLEND,

	SM_DINWAIT,
	SM_DOUTSET,
	SM_DOUTPULSE,
	SM_ONDISTANCE,
	SM_ONPOSITION,
	SM_READWHILEMOTION,
	SM_DIWAIT,
	SM_DIREAD,
	SM_DOSET,
 
	SM_TRACKVIS,
	SM_INFOTIP,
	SM_GETROBPOS,
	SM_LOADSUB,

 	SM_DEF_AXISPOS,
	SM_DEF_CARTPOS,
	SM_DEF_POSINFO,
	SM_DEF_REFSYS,
	SM_DEF_TOOL,
	SM_DEF_DIN,
	SM_DEF_DOUT,
	SM_DEF_INT,
	SM_DEF_BOOL,
	SM_DEF_REAL,
	SM_DEF_STRING,

	SM_SYS_LOCAL,
 	SM_SYS_IFELSE,
	SM_SYS_ELSEIF,
	SM_SYS_ELSE,
	SM_SYS_WHILEDOEND,
 	SM_SYS_REPEATUNTIL,
	SM_SYS_DEFINEDFUNCTION,

	SM_SYS_PRINT,
	SM_SYS_ABS,
	SM_SYS_SIN,
	SM_SYS_COS,
	SM_SYS_ASIN,
	SM_SYS_ACOS,
	SM_SYS_TAN,
	SM_SYS_ATAN,
	SM_SYS_ATAN2,
	SM_SYS_FLOOR,
	SM_SYS_CEIL,
	SM_SYS_RAD,
	SM_SYS_DEG,
	SM_SYS_EXP,
	SM_SYS_SQRT,
	SM_SYS_LOG,
	SM_SYS_MOD,
	SM_SYS_RANDOM,

	SM_MAX_COMMANDS       /*!< maximum nr of script commands     */ // 53
};

struct RegParseResult
{
 	std::vector <VariableKey> params;  
    ScriptCommandT sct;
	
	inline RegParseResult(bool b);  
	int nCmdStart;
	int nCmdEnd;
	int nParamStart;
	int nParamEnd;
	bool valid;
	int nCmdType;  //0 : var1 = APOS(), 1: PTP(),  2 : dinv1:Set()
	int  nFromID;
	int nRow;
	void clear(); 
};

RegParseResult::RegParseResult(bool b) : valid(b)
{}


class ScriptCommandInfo
{
public:
	ScriptCommandInfo();
	virtual ~ScriptCommandInfo();

	GenericValues* CommandParameters[SM_MAX_COMMANDS];
 	GenericValues* CommandCategory[SM_MAX_SETS];
 
	std::vector<std::string> regStringList;
	std::string              regSplitCmd;
	std::string              regSplitReturnVal;

	const char*   getSeparatorStr (                            ) const;
	bool          setSeparatorStr ( char           *str        );
	bool          isClassMemberFunction (  ScriptCommandT sct  )const;
	bool          isVariableDeclaration( ScriptCommandT sct ) const;
	bool          isLuaSystemFunction (ScriptCommandT sct ) const;

	int parse_command ( const char* strCmd, RegParseResult* result );

private:
	void init_parameters_list ();
	void init_category_list   ();
	void init_regex_list ();

	const char* getCmdDescStr( ScriptCommandT sct );
    char strSeparator[10];
	char strCommand[255];
};


#endif