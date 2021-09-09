// Copyright 2016, ��˼��Ϣ�Ƽ��������ι�˾
// All right reserved.
//
// Author: Liu Run Dan ( lrdcz@163.com )
//
// Yasi Robot Motion Control System - ��˼�������˶�����ϵͳ

#ifndef _modify_variable_directly_h
#define _modify_variable_directly_h

#include "ScriptVariableType.h"
#include <vector>

class VariableReadWriter
{
public:
	VariableReadWriter(void);
	virtual ~VariableReadWriter(void);
	
	bool write_variable(const char* varname,const char *propname, const char *strValue, bool single = false, ScriptVarSetT svm= SVAR_PROGRAM );
	bool read_variable( ScriptVarT sv, void* valueptr, std::vector<std::string>& valuelist );

	bool write_variable( VariableKey* varKey, const char *strValue );

private:
	bool is_value_valid(const char* vartype,const char *strValue);
	bool is_value_valid(ScriptVarT sv,const char *strValue);

};

#endif
