// Copyright 2016, 亚思信息科技有限责任公司
// All right reserved.
//
// Author: Liu Run Dan ( lrdcz@163.com )
//
// Yasi Robot Motion Control System - 亚思机器人运动控制系统

#ifndef _ScriptVariables_h
#define _ScriptVariables_h

#include <string>
#include <map>
#include <list>
#include <vector>
#include "../CmdInfo.h"
#include "ScriptVariableType.h"
#include "VariableReadWriter.h" 

class ScriptVariables
{
public:
	~ScriptVariables(void);
	static ScriptVariables* getInstance();

	void read_var_from( const char* fname, ScriptVarSetT svm = SVAR_PROGRAM, bool appendProVar = false )  throw (YsbotsException);
	void write_var_at( const char* fname, ScriptVarSetT svm = SVAR_PROGRAM  )  throw (YsbotsException);

	bool creat_variable( ScriptVarT, std::string, void* pValue, ScriptVarSetT svm = SVAR_PROGRAM  )  throw ();
	void delete_variable( ScriptVarT, std::string, ScriptVarSetT svm = SVAR_PROGRAM  )  throw ();
	void rename_variable( ScriptVarT, std::string, std::string , ScriptVarSetT svm = SVAR_PROGRAM )  throw ();
	void set_value ( ScriptVarT, std::string, void* pValue, ScriptVarSetT svm = SVAR_PROGRAM  )  throw ();
	void get_value ( ScriptVarT type, std::string var_name, void* pValue, ScriptVarSetT svm = SVAR_PROGRAM )  throw ();
	int  get_num_variables( ScriptVarT, ScriptVarSetT svm = SVAR_PROGRAM  ) throw();
	const char* get_type_name( ScriptVarT st) throw();
	void show( ostream& out, ScriptVarSetT svm = SVAR_PROGRAM  ) throw ();

// 	void update_variable_ui (ScriptVarT, std::string, void* pValue);
	void modify_variable_ui (ScriptVarT st, std::string, void*, ScriptVarSetT svm = SVAR_PROGRAM  );
	ScriptVarT get_variable_type( std::string );
	void* get_value_ptr (ScriptVarT type, std::string var_name, ScriptVarSetT svm = SVAR_PROGRAM )  throw ();
	ScriptVariableType* get_variable_info() { return variable_info; }
	VariableReadWriter* get_variable_readwriter () { return var_read_writer; }

// 	bool creat_variable( const char* vartype, const char* varname, ScriptVarSetT svm = SVAR_PROGRAM  );
//	void delete_variable( const char* vartype, const char* varname, ScriptVarSetT svm = SVAR_PROGRAM  )  throw ();
	void clear_all();
    void clear_variable ( ScriptVarSetT svr = SVAR_PROGRAM );

	const std::map<ScriptVarT, std::map<std::string, void*>>* get_var_list(ScriptVarSetT svm = SVAR_PROGRAM);
 
	void load_subprogram_var(const char* modname, char* chHomePath );

protected:
	ScriptVariables(void);
	static ScriptVariables* singleton;

private:
	bool registerVariable(ScriptVarT, std::string, void* pValue, ScriptVarSetT svm = SVAR_PROGRAM )  throw ();
	bool find_variable(ScriptVarT, std::string, ScriptVarSetT svm = SVAR_PROGRAM ) throw ();
	void delete_value(ScriptVarT, void* ptrValue ) throw();
	void copy_value( ScriptVarT, void*, void* ) throw();

	void creat_system_variable(); // system variable need not change.

	void get_home_path (const char* fullpath);

private:

	std::map<ScriptVarT, std::map<std::string, void*>> var_list[SVAR_SET_NUM];

	ScriptVariableType* variable_info;
	VariableReadWriter* var_read_writer;

	std::vector<string> sub_var_list; // subprogram variable file name
	std::vector<VariableKey> sub_all_var;

	std::string home_path;
	// collector garbage.
	class VarCleaner
	{
	public:
		VarCleaner() {}
		~VarCleaner()
		{
			if ( ScriptVariables::getInstance() )
			{
				delete ScriptVariables::getInstance();
			}
		}
	};
};

#define LVAR    ScriptVariables::getInstance()
#define VARINFO ScriptVariables::getInstance()->get_variable_info()
#define VARRW   ScriptVariables::getInstance()->get_variable_readwriter()

#endif