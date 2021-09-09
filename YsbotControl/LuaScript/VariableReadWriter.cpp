// Copyright 2016, 亚思信息科技有限责任公司
// All right reserved.
//
// Author: Liu Run Dan ( lrdcz@163.com )
//
// Yasi Robot Motion Control System - 亚思机器人运动控制系统

#include "StdAfx.h"
#include "VariableReadWriter.h"
#include "ScriptVariables.h"
#include "../stringconvert.h"
#include "ScriptVariables.h"
#include "BasicScriptVariable.h"
#include "ScriptVariableType.h"
#include "../DigitalInput.h"
#include "../DigitalOutput.h"
#include "../Conveyor.h"
#include "../UIDialogs/SecurArea.h"

using namespace std;

VariableReadWriter::VariableReadWriter(void)
{
}


VariableReadWriter::~VariableReadWriter(void)
{
}

bool VariableReadWriter::write_variable(const char* varname,const char *propname,const char *strValue, bool single, ScriptVarSetT svm )
{
	bool res = true;
	const char* split = ": ";
	std::deque<std::string> varlist, proplist;
	split_string( varlist, varname, split );
	split_string( proplist, propname, split );
	
	string curr_var_type = "";
	string curr_var_name = "";
	string curr_data_type = "";
	if ( single )
	{
 		curr_var_type = proplist.size() > 1 ? proplist[1] : "";
		curr_var_name = proplist.size() > 0 ? proplist[0] : "";
	}
 	else
	{
		curr_var_type = varlist.size() > 1 ? varlist[1] : "";
		curr_var_name = varlist.size() > 0 ? varlist[0] : "";
 	}

	ScriptVarT sv = LVAR->get_variable_type(string(curr_var_type));
	BasicScriptVariable* ptrVar = NULL; 
 	switch(sv)
	{
	case svAXISPOS:  
	case svCARTPOS:	ptrVar = static_cast<PosInfo*>(LVAR->get_value_ptr(sv,curr_var_name,svm)); break;
	case svINTEGER: 
	case svBOOL:         
	case svRDI:          
	case svRDO:      
	case svREAL:	break;
	case svDYNAMIC: ptrVar = static_cast<Dynamic*>(LVAR->get_value_ptr(sv,curr_var_name,svm)); break; 
	case svOVERLAP: ptrVar = static_cast<Overlap*>(LVAR->get_value_ptr(sv,curr_var_name,svm)); break;
	case svDIGITALINPUT:  ptrVar = static_cast<DigitalInput*>(LVAR->get_value_ptr(sv,curr_var_name,svm));  break;
	case svDIGITALOUTPUT: ptrVar = static_cast<DigitalOutput*>(LVAR->get_value_ptr(sv,curr_var_name,svm)); break;
	case svCARTREF:  
	case svTOOL:    ptrVar = static_cast<CartRef*>(LVAR->get_value_ptr(sv,curr_var_name,svm)); break;
	case svSTRING:   break;
	default: break;
	} 

	curr_data_type = proplist.size() > 1 ? proplist[1] : "";
	varlist.clear();
	proplist.clear();
	if ( curr_data_type.empty() )
	{
		cout << "unkown variable type\n";
		return false;
	}

	if ( is_value_valid( curr_data_type.c_str(),strValue) )
	{
		cout << curr_data_type << '\n';
		if ( ptrVar )
		{
			ptrVar->set_value( propname, (char *)strValue );
		}
		else
		{
			void* pval = LVAR->get_value_ptr(sv, curr_var_name,svm);
			if ( pval )
			{
				if ( sv == svREAL )
					*( double * )( pval ) = atof( strValue ? strValue : 0 );
				else if ( sv == svBOOL )
				{
					bool b = false;
					if( !strValue )
						;
					else if( isdigit( strValue[ 0 ] ) )
						b = atoi( strValue ? strValue : 0 ) > 0 ? true : false;
					else if( strcmp( strValue, "on"  ) == 0 ||
						strcmp( strValue, "true") == 0 ||
						strcmp( strValue, "yes" ) == 0 )
						b = true;
					*( int * )( pval ) = ( b == true ) ? 1 : 0;
				}
				else if ( sv == svSTRING )
				{
					strcpy( ( char * ) ( pval ), strValue );
				}
				else if ( sv == svLUAINTEGER )
				{
					LuaInteger* pdata = ( LuaInteger * )( pval );
					pdata->m_value = atoi( strValue ? strValue : 0 );
				}
				else if ( sv == svLUABOOL )
				{
					LuaBool* pdata = ( LuaBool * )( pval );
					bool b = false;
					if( !strValue )
						;
					else if( isdigit( strValue[ 0 ] ) )
						b = atoi( strValue ? strValue : 0 ) > 0 ? true : false;
					else if( strcmp( strValue, "on"  ) == 0 ||
						strcmp( strValue, "true") == 0 ||
						strcmp( strValue, "yes" ) == 0 )
						b = true;
					pdata->m_value = b;
				}
				else if ( sv == svLUAREAL )
				{
					LuaReal* pdata = ( LuaReal * )( pval );
					pdata->m_value = atof( strValue ? strValue : 0 );
				}
				else if ( sv == svLUASTRING )
				{
					LuaString* pdata = ( LuaString * )( pval );
					pdata->m_value = std::string(strValue);
				}
				else
				{
					*( int * )( pval ) = atoi( strValue ? strValue : 0 );
				}		
			}
			else
				cout << "unkown variable name\n";
  		}
 	}
	else
	{
		cout << "input not valid!" << '\n';
		res = false;
	}


	return res;
}

bool VariableReadWriter::write_variable( VariableKey* varKey, const char *strValue  )
{
	bool res = true;
 
	ScriptVarSetT svm = varKey->varSet;
	ScriptVarT sv     = varKey->varTyp;

	BasicScriptVariable* ptrVar = NULL; 
	switch(sv)
	{
	case svAXISPOS:  
	case svCARTPOS:	ptrVar = static_cast<PosInfo*>(LVAR->get_value_ptr(sv,varKey->varName,svm)); break;
	case svINTEGER: 
	case svBOOL:         
	case svRDI:          
	case svRDO:      
	case svREAL:	break;
	case svDYNAMIC: ptrVar = static_cast<Dynamic*>(LVAR->get_value_ptr(sv,varKey->varName,svm)); break; 
	case svOVERLAP: ptrVar = static_cast<Overlap*>(LVAR->get_value_ptr(sv,varKey->varName,svm)); break;
	case svDIGITALINPUT:  ptrVar = static_cast<DigitalInput*>(LVAR->get_value_ptr(sv,varKey->varName,svm));  break;
	case svDIGITALOUTPUT: ptrVar = static_cast<DigitalOutput*>(LVAR->get_value_ptr(sv,varKey->varName,svm)); break;
	case svCARTREF:  
	case svTOOL:    ptrVar = static_cast<CartRef*>(LVAR->get_value_ptr(sv,varKey->varName,svm)); break;
	case svSTRING:   break;
	case svCONVEYOR: ptrVar = static_cast<Conveyor*>(LVAR->get_value_ptr(sv,varKey->varName,svm)); break;
	case svSensableObject: ptrVar = static_cast<SensableObject*>(LVAR->get_value_ptr(sv,varKey->varName,svm)); break;
	case svSecurArea: ptrVar = static_cast<SecurArea*>(LVAR->get_value_ptr(sv,varKey->varName,svm)); break;
	default: break;
	} 
 	 
	ScriptVarT dataType = sv;
	if ( varKey->first_members == false )
	{
		dataType = varKey->propTyp;
	}
 
	if ( is_value_valid( dataType, strValue ) )
	{
 		if ( ptrVar ) //write stuct data 
		{
			ASSERT( varKey->strKey[0] != '\0');//
			ptrVar->set_value( varKey->strKey, (char *)strValue ); // varkey->propName
		}
		else
		{
			void* pval = LVAR->get_value_ptr( sv, varKey->varName, svm );
			if ( pval )
			{
				if ( sv == svREAL )
					*( double * )( pval ) = atof( strValue ? strValue : 0 );
				else if ( sv == svBOOL )
				{
					bool b = false;
					if( !strValue )
						;
					else if( isdigit( strValue[ 0 ] ) )
						b = atoi( strValue ? strValue : 0 ) > 0 ? true : false;
					else if( strcmp( strValue, "on"  ) == 0 ||
						strcmp( strValue, "true") == 0 ||
						strcmp( strValue, "yes" ) == 0 )
						b = true;
					*( int * )( pval ) = ( b == true ) ? 1 : 0;
				}
				else if ( sv == svSTRING )
				{
					strcpy( ( char * ) ( pval ), strValue );
				}
				else if ( sv == svLUAINTEGER )
				{
					LuaInteger* pdata = ( LuaInteger * )( pval );
					pdata->m_value = atoi( strValue ? strValue : 0 );
				}
				else if ( sv == svLUABOOL )
				{
					LuaBool* pdata = ( LuaBool * )( pval );
					bool b = false;
					if( !strValue )
						;
					else if( isdigit( strValue[ 0 ] ) )
						b = atoi( strValue ? strValue : 0 ) > 0 ? true : false;
					else if( strcmp( strValue, "on"  ) == 0 ||
						strcmp( strValue, "true") == 0 ||
						strcmp( strValue, "yes" ) == 0 )
						b = true;
					pdata->m_value = b;
				}
				else if ( sv == svLUAREAL )
				{
					LuaReal* pdata = ( LuaReal * )( pval );
					pdata->m_value = atof( strValue ? strValue : 0 );
				}
				else if ( sv == svLUASTRING )
				{
					LuaString* pdata = ( LuaString * )( pval );
					pdata->m_value = std::string(strValue);
				}
				else
				{
					*( int * )( pval ) = atoi( strValue ? strValue : 0 );
				}		
			}
			else
			{
				cout << "unkown variable name\n";
				res = false;
			}
		}
	}
	else
	{
		cout << "input not valid!" << '\n';
		res = false;
	}


	return res;
}

bool VariableReadWriter::is_value_valid(const char* vartype, const char *strValue)
{
	bool ret = true;
	if ( strcmp( vartype, VARINFO->vardesc[svINTEGER]->getClassName()) == 0 || 
		 strcmp( vartype, VARINFO->vardesc[svRDI]->getClassName())     == 0 ||
		 strcmp( vartype, VARINFO->vardesc[svRDO]->getClassName())     == 0   )
	{
         if ( is_number(strValue) != 1 )
 			ret = false;
 	}
	else if ( strcmp( vartype, VARINFO->vardesc[svREAL]->getClassName()) == 0 )
	{
		if ( is_number(strValue) != 3 && is_number(strValue) != 1 )
			ret = false;
	}
	else if ( strcmp( vartype, VARINFO->vardesc[svSTRING]->getClassName()) == 0 )
	{
// 		bool bflag = false;
// 		int length =  strlen(strValue);
// 		for(int i = 0 ; i < length ; i++)
// 		{
// 			int charinput = strValue[i];
// 			if( (charinput >= 'a' && charinput <= 'z') || (charinput >= 'A'&& charinput <= 'Z') )
// 			{
// 				bflag = true;
// 				break;
// 			} 
// 		}
// 		if ( !bflag )
// 			ret = false;
		if ( is_number(strValue) != 0 )
		   ret = false;
	}
	else if ( strcmp( vartype, VARINFO->vardesc[svBOOL]->getClassName()) == 0 )
	{
		if ( strcmp(strValue,"true") != 0 && strcmp(strValue,"TRUE") != 0 &&
			 strcmp(strValue,"false") != 0 && strcmp(strValue,"FALSE") != 0 )
		{
			ret = false;
		}
	}
	return ret;
}


bool VariableReadWriter::is_value_valid(ScriptVarT sv, const char *strValue)
{
	bool ret = true;
	if ( sv == svINTEGER || sv == svRDI || sv == svRDO )
	{
		if ( is_number(strValue) != 1 )
			ret = false;
	}
	else if ( sv == svREAL )
	{
		if ( is_number(strValue) != 3 && is_number(strValue) != 1 )
			ret = false;
	}
	else if ( sv == svSTRING )
	{
 		if ( is_number(strValue) != 0 )
			ret = false;
	}
	else if ( sv == svBOOL )
	{
		if ( strcmp(strValue,"true") != 0 && strcmp(strValue,"TRUE") != 0 &&
			strcmp(strValue,"false") != 0 && strcmp(strValue,"FALSE") != 0 )
		{
			ret = false;
		}
	}
	return ret;
}


bool VariableReadWriter::read_variable( ScriptVarT sv, void* valueptr, std::vector<std::string>& valuelist )
{
	valuelist.clear();

	bool ret = true;
 
	if ( valueptr == NULL )
		return false;

	BasicScriptVariable* ptrVar = NULL; 
	switch(sv)
	{
	case svAXISPOS:  
	case svCARTPOS:	ptrVar = static_cast<PosInfo*>(valueptr); break;
	case svINTEGER: 
	case svBOOL:         
	case svRDI:          
	case svRDO:      
	case svREAL:	break;
	case svDYNAMIC: ptrVar = static_cast<Dynamic*>(valueptr); break; 
	case svOVERLAP: ptrVar = static_cast<Overlap*>(valueptr); break;
	case svDIGITALINPUT:  ptrVar = static_cast<DigitalInput*>(valueptr); break;
	case svDIGITALOUTPUT: ptrVar = static_cast<DigitalOutput*>(valueptr); break;
	case svCARTREF:  
	case svTOOL:    ptrVar = static_cast<CartRef*>(valueptr); break;
	case svSTRING:   break;
	case  svCONVEYOR: ptrVar = static_cast<Conveyor*>(valueptr); break;
	case  svSensableObject: ptrVar = static_cast<SensableObject*>(valueptr); break;
	case  svSecurArea: ptrVar = static_cast<SecurArea*>(valueptr); break;
	default: break;
	} 

	char newvalue[VAR_STR_LEN];
	if ( ptrVar )
	{
 		char strValue[VAR_STR_LEN];
		char prop[VAR_STR_LEN];

		for ( int i = 0; i < VARINFO->vardesc[sv]->getValuesTotal(); i++ )
		{
			VARINFO->vardesc[sv]->getValue(i+1,prop);
			ptrVar->get_value (prop, strValue );
			if ( strstr(prop,"ANGLE") )
			{
				double tmp = atof( strValue ? strValue : 0 );
				sprintf( newvalue, "%.2f", rad2deg(tmp) );
				valuelist.push_back(std::string(newvalue));
			}
			else if ( strstr(prop,"BOOL") )
			{
				int tmp = atoi( strValue ? strValue : 0 );
				std::string strtmp = tmp > 0 ? "true" : "false";
				valuelist.push_back( strtmp );
			}
			else if ( strstr(prop,"REAL") )
			{
				double tmp = atof( strValue ? strValue : 0 );
				sprintf( newvalue, "%.2f", tmp );
				valuelist.push_back(std::string(newvalue));
			}
			else
			{
 			    valuelist.push_back(std::string(strValue));
			}
 		}
	}
	else
	{
		if ( sv == svREAL )
		{
			double tmp = *static_cast<double*>(valueptr);
			sprintf( newvalue, "%.2f", tmp );
			valuelist.push_back(std::string(newvalue));
		}
		else if ( sv == svBOOL )
		{
 			std::string strtmp = *static_cast<int*>(valueptr) > 0 ? "true" : "false";
			valuelist.push_back( strtmp );
		}
		else if ( sv == svSTRING )
		{
 			valuelist.push_back( std::string(static_cast<char*>(valueptr)) );
		}
		else if ( sv == svLUAINTEGER )
		{
			LuaInteger* pdata = ( LuaInteger * )( valueptr );
			sprintf( newvalue, "%d", pdata->m_value );
			valuelist.push_back(std::string(newvalue));
		}
		else if ( sv == svLUABOOL )
		{
			LuaBool* pdata = ( LuaBool * )( valueptr );
			std::string strtmp = pdata->m_value ? "true" : "false";
			valuelist.push_back( strtmp );
		}
		else if ( sv == svLUAREAL )
		{
			LuaReal* pdata = ( LuaReal * )( valueptr );
			sprintf( newvalue, "%.2f", pdata->m_value );
			valuelist.push_back(std::string(newvalue));
		}
		else if ( sv == svLUASTRING )
		{
			LuaString* pdata = ( LuaString * )( valueptr );
			valuelist.push_back(pdata->m_value);
		}
		else
		{
			sprintf( newvalue, "%d", *static_cast<int*>(valueptr) );
			valuelist.push_back(std::string(newvalue));
		}
  	}

	return ret;
}