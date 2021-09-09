// Copyright 2016, 亚思信息科技有限责任公司
// All right reserved.
//
// Author: Liu Run Dan ( lrdcz@163.com )
//
// Yasi Robot Motion Control System - 亚思机器人运动控制系统

#include "StdAfx.h"
#include "ScriptVariables.h"
#include "LuaScript.h"
#include "../DigitalInput.h"
#include "../DigitalOutput.h"
#include "../stringconvert.h"
#include "../MotionControl.h"
#include "../Conveyor.h"
#include "../SensableObject.h"
#include "../UIDialogs/SecurArea.h"
#include <algorithm> 

using namespace std;

ScriptVariables* ScriptVariables::singleton = 0;

ScriptVariables* ScriptVariables::getInstance()
{
	if (singleton == 0) 
	{
		singleton = new ScriptVariables();
		singleton->creat_system_variable();
		static VarCleaner vcl;
	}
	return singleton;
}

ScriptVariables::ScriptVariables(void)
{

	variable_info   = new ScriptVariableType();
	var_read_writer = new VariableReadWriter();
	sub_var_list.clear();
	sub_all_var.clear();
	cout << "Constuct ScriptVariables" << endl;
	TRACE(_T("Constuct ScriptVariables!\n")); 
	
    home_path = "";

// 	LuaScript::getInstance()->addVariableToState(svINTEGER,0,0);
	 
}

ScriptVariables::~ScriptVariables(void)
{
	if (singleton == this) 
	{       
		singleton = 0;
	}
	clear_all();
	sub_var_list.clear();
	sub_all_var.clear();
	delete variable_info;
	delete var_read_writer;

	cout << "destructor ScriptVariables" << endl;
	TRACE(_T("destructor ScriptVariables!\n")); 

}

void ScriptVariables::clear_variable(ScriptVarSetT svr)
{
	for( map<ScriptVarT,  map<string, void*>>::iterator it = var_list[svr].begin(); 
		it != var_list[svr].end(); 
		it++ ) 
	{
		for ( map<string, void*>::iterator innerIt = it->second.begin(); 
			innerIt != it->second.end(); 
			innerIt++ ) 
		{
			// delete variable object
			delete_value(it->first, innerIt->second);
		}
		it->second.clear();       // empty the inner map(s)
	}
	var_list[svr].clear();             // empty the outer map
}

void ScriptVariables::clear_all()
{
	for ( ScriptVarSetT svr = SVAR_SYSTEM; svr != SVAR_SET_NUM; )
	{
		clear_variable(svr);
		svr = static_cast<ScriptVarSetT>( (unsigned int)(svr) + 1 );
	}
}

bool ScriptVariables::find_variable(ScriptVarT sv, std::string key, ScriptVarSetT svm ) throw()
{
	if ( svm < SVAR_SYSTEM || svm > SVAR_PROGRAM )
 		return false;
	 
  	map<ScriptVarT,  map<string, void*>>::iterator it = var_list[svm].find(sv);
	if( it != var_list[svm].end())
	{
		map<string, void*>::iterator it2 = it->second.find( key );
		if( it2 != it->second.end() )
		{
			return true; // already exist var
		}
	}
 
	return false;
}

bool ScriptVariables::creat_variable( ScriptVarT sv, std::string key, void* pValue, ScriptVarSetT svm ) throw ()
{
 	if ( find_variable(sv,key,svm) )
	{
		return false;
	}
	if ( sv == svAXISPOS )
	{
		if ( find_variable(svCARTPOS,key,svm) )
			return false;
	}
	else if ( sv == svCARTPOS )
	{
		if ( find_variable(svAXISPOS,key,svm))
			return false;
	}
	return registerVariable( sv, key, pValue, svm );
}

void ScriptVariables::delete_variable( ScriptVarT sv, std::string key, ScriptVarSetT svm ) throw ()
{
	//std::map<ScriptVarT, std::map<std::string, void*>>* var_list = const_cast<std::map<ScriptVarT, std::map<std::string, void*>>*>(get_var_list(svm));
	if ( svm < SVAR_SYSTEM || svm > SVAR_PROGRAM )
		return;
 
 	map<ScriptVarT,  map<string, void*>>::iterator it = var_list[svm].find(sv);
	if( it != var_list[svm].end() )
	{
		map<string, void*>::iterator innerIt = it->second.find(key);
		if( innerIt != it->second.end() )
		{
			// delete variable object
 			delete_value(it->first, innerIt->second);
			it->second.erase(key);
 		}
	}
  
}

// void ScriptVariables::delete_variable( const char* vartype, const char* varname, ScriptVarSetT svm )
// {
// 	ScriptVarT st = get_variable_type( vartype );
//  	delete_variable(st,varname,svm);
// }

int ScriptVariables::get_num_variables( ScriptVarT type, ScriptVarSetT svm ) throw()
{
	if ( svm < SVAR_SYSTEM || svm > SVAR_PROGRAM )
		return 0;

 	map<ScriptVarT,  map<string, void*>>::iterator it = var_list[svm].find(type);
	if( it != var_list[svm].end() )
		return it->second.size();
	else
		return 0;
}

//////////////////////////////////////////////////////////////////////////

bool ScriptVariables::registerVariable( ScriptVarT sv, std::string key, void* pValue, ScriptVarSetT svm ) throw ()
{
	void* pValtmp = NULL;
	switch(sv)
	{
	case svAXISPOS: pValtmp = static_cast<void*>(new PosInfo(PosInfo::CTaxis)); break;
	case svCARTPOS:	pValtmp = static_cast<void*>(new PosInfo(PosInfo::CTmachine)); break;
	case svINTEGER: 
	case svBOOL:         
	case svRDI:          
	case svRDO:     pValtmp = static_cast<void*>(new int() );     break;
	case svREAL:	pValtmp = static_cast<void*>(new Real());   break;
	case svDYNAMIC: pValtmp = static_cast<void*>(new Dynamic());  break; 
	case svOVERLAP: pValtmp = static_cast<void*>(new Overlap());  break;
	case svDIGITALINPUT:  pValtmp = static_cast<void*>(new DigitalInput() );  break;
	case svDIGITALOUTPUT: pValtmp = static_cast<void*>(new DigitalOutput() ); break;
	case svCARTREF:
	case svTOOL:    pValtmp = static_cast<void*>(new CartRef()); break;
	case svSTRING:  pValtmp = static_cast<void*>(new char[VAR_STR_LEN]); 
		memset(pValtmp,'\0',VAR_STR_LEN*sizeof(char));
		break;
	case svLUAINTEGER: pValtmp = static_cast<void*>(new LuaInteger()); break;
	case svLUABOOL:    pValtmp = static_cast<void*>(new LuaBool()); break;
	case svLUAREAL:    pValtmp = static_cast<void*>(new LuaReal()); break;
	case svLUASTRING:  pValtmp = static_cast<void*>(new LuaString()); break;
	case svCONVEYOR:   pValtmp = static_cast<void*>(new Conveyor()); break;
	case svSensableObject:   pValtmp = static_cast<void*>(new SensableObject()); break;
	case svSecurArea:   pValtmp = static_cast<void*>(new SecurArea()); break;
	default: break;
	} 

 	var_list[svm][sv][key] = pValtmp; // insert map
	if ( pValue != NULL )
	{
 		copy_value( sv, pValtmp, pValue );  
 	}

	std::string newkey = key;
	if ( sv == svCARTREF || sv == svTOOL )
	{
  		if ( svm == SVAR_SYSTEM )  
		{
			newkey = STR_LABEL_SYSTEM + key;
		}
		else if ( svm == SVAR_GLOBAL )
		{
			newkey = STR_LABEL_GLOBAL + key;
		}
		static_cast<CartRef*>(pValtmp)->setSelfName(newkey.c_str()); // when new variable from UI
	}

	newkey = key;
    if ( svm == SVAR_SYSTEM )
	{
 		 newkey = STR_LABEL_SYSTEM + key;
 	}
	else if ( svm == SVAR_GLOBAL )
	{
 		 newkey = STR_LABEL_GLOBAL + key;
  	}
	LuaScript::getInstance()->addVariableToState(sv,newkey.c_str(),pValtmp);

	return true;
	// update location user interface.
// 	update_variable_ui(sv,key,pValtmp);
}

// void ScriptVariables::update_variable_ui(ScriptVarT sv, std::string key, void* pValue)
// {
// 	if ( variable_dialog && variable_dialog->GetSafeHwnd() )
// 	{
// 		string type = VARINFO->vardesc[sv]->getClassName(); 
// 		string typekey = key + ": " + type;
// 		variable_dialog->InsertVariable(type.c_str(), typekey.c_str(), pValue);
// 	}
// }

void ScriptVariables::delete_value(ScriptVarT sv, void* ptrValue ) throw()
{
	if ( ptrValue )
	{
		switch(sv)
		{
		case svAXISPOS:
		case svCARTPOS:	delete static_cast<PosInfo*>(ptrValue);	 break;
		case svINTEGER: 
		case svBOOL:    
		case svRDI:     
		case svRDO:     delete static_cast<int*>(ptrValue);      break; 
		case svREAL:    delete static_cast<Real*>(ptrValue);	 break;
		case svDYNAMIC: delete static_cast<Dynamic*>(ptrValue);  break; 
		case svOVERLAP: delete static_cast<Overlap*>(ptrValue);  break;
		case svDIGITALINPUT:  delete static_cast<DigitalInput*>(ptrValue); break;
		case svDIGITALOUTPUT: delete static_cast<DigitalOutput*>(ptrValue); break;
		case svCARTREF: 
		case svTOOL:    delete static_cast<CartRef*>(ptrValue); break;
		case svSTRING:  delete [] static_cast<char*>(ptrValue); break;
		case svLUAINTEGER: delete static_cast<LuaInteger*>(ptrValue); break;
		case svLUABOOL:    delete static_cast<LuaBool*>(ptrValue); break;
		case svLUAREAL:    delete static_cast<LuaReal*>(ptrValue); break;
		case svLUASTRING:  delete static_cast<LuaString*>(ptrValue); break;
		case svCONVEYOR :  delete static_cast<Conveyor*>(ptrValue); break;
 		case svSensableObject :  delete static_cast<SensableObject*>(ptrValue); break;
		case svSecurArea :  delete static_cast<SecurArea*>(ptrValue); break;

		default: break;
		} 

		ptrValue = NULL;
  	}
 }

void ScriptVariables::copy_value( ScriptVarT sv, void* dest, void* src) throw()
{
	if ( !dest || !src )
 		return;

 	switch(sv)
	{
	case svAXISPOS:
	case svCARTPOS: *(static_cast<PosInfo*>(dest)) = *(static_cast<PosInfo*>(src));	 break;
	case svINTEGER: 
	case svBOOL:         
	case svRDI:            
	case svRDO:     *(static_cast<int*>(dest))     = *(static_cast<int*>(src));      break;
	case svREAL:    *(static_cast<Real*>(dest))  = *(static_cast<Real*>(src));	 break;
	case svDYNAMIC: *(static_cast<Dynamic*>(dest)) = *(static_cast<Dynamic*>(src));  break; 
	case svOVERLAP: *(static_cast<Overlap*>(dest)) = *(static_cast<Overlap*>(src));  break;
	case svDIGITALINPUT:  *(static_cast<DigitalInput*>(dest))  = *(static_cast<DigitalInput*>(src)); break;
	case svDIGITALOUTPUT: *(static_cast<DigitalOutput*>(dest)) = *(static_cast<DigitalOutput*>(src)); break;
	case svCARTREF: 
	case svTOOL: *(static_cast<CartRef*>(dest)) = *(static_cast<CartRef*>(src)); break;
	case svSTRING: strcpy(static_cast<char*>(dest), static_cast<char*>(src)); break;

	case svLUAINTEGER: *(static_cast<LuaInteger*>(dest)) = *(static_cast<LuaInteger*>(src));  break;
	case svLUABOOL:    *(static_cast<LuaBool*>(dest))    = *(static_cast<LuaBool*>(src));  break;
	case svLUAREAL:    *(static_cast<LuaReal*>(dest))    = *(static_cast<LuaReal*>(src));  break;
	case svLUASTRING:  *(static_cast<LuaString*>(dest))  = *(static_cast<LuaString*>(src));  break;

	case svCONVEYOR: *(static_cast<Conveyor*>(dest))  = *(static_cast<Conveyor*>(src));  break;
	case svSensableObject: *(static_cast<SensableObject*>(dest))  = *(static_cast<SensableObject*>(src));  break;
	case svSecurArea: *(static_cast<SecurArea*>(dest))  = *(static_cast<SecurArea*>(src));  break;
	default: break;
	} 
}

//////////////////////////////////////////////////////////////////////////

void ScriptVariables::rename_variable( ScriptVarT sv, std::string key_old, std::string key_new, ScriptVarSetT svm ) throw ()
{
	if ( find_variable(sv, key_new,svm) || !find_variable(sv, key_old,svm) ) //?????????????
	{
		return;
	}

	if ( sv == svAXISPOS )
	{
		if ( find_variable(svCARTPOS,key_new,svm) )
 			return;
 	}
	else if ( sv == svCARTPOS )
	{
		if ( find_variable(svAXISPOS,key_new,svm))
 			return;
 	}

	void* old_val = get_value_ptr( sv, key_old, svm );
	registerVariable( sv, key_new, old_val, svm );
	delete_variable( sv, key_old, svm );
}

void ScriptVariables::set_value( ScriptVarT sv, std::string key, void* pValue, ScriptVarSetT svm ) throw ()
{
	if( find_variable( sv, key, svm ) )
	{
		copy_value( sv, get_value_ptr(sv,key,svm), pValue );
 	}
}

void* ScriptVariables::get_value_ptr(ScriptVarT type, std::string var_name, ScriptVarSetT svm ) throw()
{
	if ( find_variable(type,var_name,svm) )
	{
 		return var_list[svm][type][var_name];
 	}
	else
		return NULL;
}

void ScriptVariables::get_value(ScriptVarT type, std::string var_name, void* pValue,  ScriptVarSetT svm ) throw()
{
 	copy_value( type, pValue, get_value_ptr(type,var_name,svm) );
}
 
void ScriptVariables::read_var_from( const char* fname, ScriptVarSetT svm, bool appendProVar ) throw (YsbotsException)
{
	int n=0;
	if ( fname == NULL && svm == SVAR_SYSTEM )
	{
		//creat_system_variable( );
		return;
	}
	get_home_path(fname);

	ifstream in(fname);
	if( !in ) 
	{
 		throw YsbotsException(__FILE__ ": Could not read variable from disk.");
 	}

	if ( svm == SVAR_PROGRAM && !appendProVar )
	{
		clear_variable(SVAR_PROGRAM);
		sub_var_list.clear();
		sub_all_var.clear();
 	}

	do 
	{
		string varType;
		in >> varType;
		if (in.fail() || in.eof()) break;
		string varName;
		in >> varName;
		if (in.fail() || in.eof()) break;
		in.ignore( (numeric_limits<std::streamsize>::max)(),':');
		in.ignore( (numeric_limits<std::streamsize>::max)(),'=');

// 		string ddd = VARINFO->vardesc[svRDI]->getClassName();
		ScriptVarT st = get_variable_type( varType );

		if ( appendProVar && svm == SVAR_PROGRAM )
		{
			VariableKey vkey;
			sprintf( vkey.varName, "%s", varName.c_str() );
			vkey.varTyp = st;
			sub_all_var.push_back(vkey);
		}

		if ( varType == VARINFO->vardesc[svAXISPOS]->getClassName() )
		{
			PosInfo pi(PosInfo::CTaxis);
			in.ignore( (numeric_limits<std::streamsize>::max)(),'{');
			in >> pi;
			if (in.fail() || in.eof()) break;

			creat_variable( st, varName, &pi, svm );
			in.ignore( (numeric_limits<std::streamsize>::max)(),'}');

		}
		else if ( varType == VARINFO->vardesc[svCARTPOS]->getClassName() )
		{
			PosInfo pi(PosInfo::CTmachine);
			in.ignore( (numeric_limits<std::streamsize>::max)(),'{');

			in >> pi;
			if (in.fail() || in.eof()) break;

			creat_variable( st, varName, &pi, svm );
			in.ignore( (numeric_limits<std::streamsize>::max)(),'}');

		}
		else if ( varType == VARINFO->vardesc[svINTEGER]->getClassName() || 
			varType == VARINFO->vardesc[svBOOL]->getClassName()    ||
			varType == VARINFO->vardesc[svRDI]->getClassName()     ||
			varType == VARINFO->vardesc[svRDO]->getClassName()     )
		{
			ScriptVarT st = get_variable_type( varType );

			int iVal;
			in >> iVal;
			if (in.fail() || in.eof()) break;
			creat_variable( st, varName, &iVal, svm );
		}
		else if ( varType == VARINFO->vardesc[svREAL]->getClassName() )
		{
			double df;
			in >> df;
			if (in.fail() || in.eof()) break;
			creat_variable( st, varName, &df, svm );
		}
		else if ( varType == VARINFO->vardesc[svDYNAMIC]->getClassName() )
		{
			Dynamic dyn;
			in.ignore( (numeric_limits<std::streamsize>::max)(),'{');
			in >> dyn;
			if (in.fail() || in.eof()) break;
			creat_variable( st, varName, &dyn, svm );
			in.ignore( (numeric_limits<std::streamsize>::max)(),'}');
		}
		else if ( varType == VARINFO->vardesc[svOVERLAP]->getClassName() )
		{
			Overlap ovl;
			in.ignore( (numeric_limits<std::streamsize>::max)(),'{');
			in >> ovl;
			if (in.fail() || in.eof()) break;
			creat_variable( st, varName, &ovl, svm );
			in.ignore( (numeric_limits<std::streamsize>::max)(),'}');
		}
		else if ( varType == VARINFO->vardesc[svDIGITALINPUT]->getClassName() )
		{
			DigitalInput din;

			in.ignore( (numeric_limits<std::streamsize>::max)(),'{');
			in >> din;
			if (in.fail() || in.eof()) break;

			creat_variable( st, varName, &din, svm );
			in.ignore( (numeric_limits<std::streamsize>::max)(),'}');
		}
		else if ( varType == VARINFO->vardesc[svDIGITALOUTPUT]->getClassName() )
		{
			DigitalOutput dout;
			in.ignore( (numeric_limits<std::streamsize>::max)(),'{');
			in >> dout;
			if (in.fail() || in.eof()) break;

			creat_variable( st, varName, &dout, svm );
			in.ignore( (numeric_limits<std::streamsize>::max)(),'}');
		}
		else if ( varType == VARINFO->vardesc[svCARTREF]->getClassName() ||
			varType == VARINFO->vardesc[svTOOL]->getClassName() )
		{
 			CartRef cpref;
			std::string newStrkey = varName;
 			cpref.setSelfName( newStrkey.c_str() );
			in.ignore( (numeric_limits<std::streamsize>::max)(),'{');
			in >> cpref.cpos >> cpref.cartname;
			cpref.cpos.ang = cpref.cpos.ang * CONSTPI180;
			if (in.fail() || in.eof()) break;
			creat_variable( st, varName, &cpref, svm );
			in.ignore( (numeric_limits<std::streamsize>::max)(),'}');
		}
		else if ( varType == VARINFO->vardesc[svSTRING]->getClassName() )
		{
			char strtmp[VAR_STR_LEN];
			in >> strtmp;
			if (in.fail() || in.eof()) break;
			creat_variable( st, varName, strtmp, svm );
		}
		else if ( varType == VARINFO->vardesc[svLUAINTEGER]->getClassName() )
		{
			LuaInteger lint;
			in >> lint.m_value;
			if (in.fail() || in.eof()) break;
			creat_variable( st, varName, &lint, svm );
		}
		else if ( varType == VARINFO->vardesc[svLUABOOL]->getClassName() )
		{
			LuaBool lbool;
			std::string strval = "";
			in >> strval;
			if ( strval == "true" || strval == "on" || strval == "TRUE" || strval == "ON")
				lbool.m_value = true;
			else
				lbool.m_value = false;

			if (in.fail() || in.eof()) break;
			creat_variable( st, varName, &lbool, svm );
		}
		else if ( varType == VARINFO->vardesc[svLUAREAL]->getClassName() )
		{
			LuaReal ldf;
			in >> ldf.m_value;
			if (in.fail() || in.eof()) break;
			creat_variable( st, varName, &ldf, svm );
		}
		else if ( varType == VARINFO->vardesc[svLUASTRING]->getClassName() )
		{
			LuaString lstr;
			in >> lstr.m_value;
			if (in.fail() || in.eof()) break;
			creat_variable( st, varName, &lstr, svm );
		}
		else if ( varType == VARINFO->vardesc[svCONVEYOR]->getClassName() )
		{
			Conveyor lstr;
			in.ignore( (numeric_limits<std::streamsize>::max)(),'{');
			in >> lstr;
			if (in.fail() || in.eof()) break;
			creat_variable( st, varName, &lstr, svm );
			in.ignore( (numeric_limits<std::streamsize>::max)(),'}');
		}
		else if (  varType == VARINFO->vardesc[svSensableObject]->getClassName() )
		{
			SensableObject lstr;
			in.ignore( (numeric_limits<std::streamsize>::max)(),'{');
			in >> lstr;
			if (in.fail() || in.eof()) break;
			creat_variable( st, varName, &lstr, svm );
			in.ignore( (numeric_limits<std::streamsize>::max)(),'}');
		}
		else if ( varType == VARINFO->vardesc[svSecurArea]->getClassName()  )
		{
			SecurArea sarea;
			in.ignore( (numeric_limits<std::streamsize>::max)(),'{');
			in >> sarea;
			if (in.fail() || in.eof()) break;
			creat_variable( st, varName, &sarea, svm );
			in.ignore( (numeric_limits<std::streamsize>::max)(),'}');
		}
	} 
	while ( !(in.fail() || in.eof()) );

	in.close();

// 	show(cout);
}

// bool ScriptVariables::creat_variable( const char* varType, const char* varName, ScriptVarSetT svm )
// {
// 	ScriptVarT st = get_variable_type( varType );
// 	if ( find_variable(st, varName,svm) )
//  		return false;
// 
//  	return registerVariable( st, varName, NULL, svm );
// }

void ScriptVariables::write_var_at( const char* fname, ScriptVarSetT svm ) throw (YsbotsException)
{
 	ofstream out(fname, ios::out );
	if (!out) 
	{
		throw YsbotsException(__FILE__"could not open file for output variable");
	} 

	show(out,svm);

	out.close();
}

void ScriptVariables::show( ostream& out, ScriptVarSetT svm) throw()
{
	map<ScriptVarT,  map<string, void*>>::iterator itBegin;
	map<ScriptVarT,  map<string, void*>>::iterator itEnd;
 	 
	itBegin = var_list[svm].begin();
	itEnd   = var_list[svm].end();
 
	VariableKey vkey;
	bool isSubProgramVar = false;
	for( map<ScriptVarT,  map<string, void*>>::iterator it = itBegin; 
		it != itEnd; 
		it++ ) 
	{
 		for ( map<string, void*>::iterator innerIt = it->second.begin(); 
			innerIt != it->second.end(); 
			innerIt++ ) 
		{
			isSubProgramVar = false;
			if ( svm == SVAR_PROGRAM && sub_all_var.size() > 0 )
			{
				for ( unsigned int i = 0; i < sub_all_var.size(); ++i )
				{
					if ( sub_all_var[i].varTyp == it->first && strcmp( sub_all_var[i].varName, innerIt->first.c_str() ) == 0 )
					{
						isSubProgramVar = true;
						break;
					}
				}
			}

			if ( isSubProgramVar )
 				continue;
 
			out << VARINFO->vardesc[it->first]->getClassName() << ' ';   
			switch ( it->first )
			{
			case svAXISPOS: 
 			case svCARTPOS:	
				out << innerIt->first << " := { " << *( static_cast<PosInfo*>(innerIt->second) ) << " } ";
				break;
			case svINTEGER:
			case svBOOL:
			case svRDI:
			case svRDO:
				out << innerIt->first << " := " << *( static_cast<int*>(innerIt->second) );
				break;
 			case svREAL:	
				out << innerIt->first << " := " << *( static_cast<Real*>(innerIt->second) );
				break;
			case svDYNAMIC:
				out << innerIt->first << " := { " << *( static_cast<Dynamic*>(innerIt->second) ) << " } ";
				break;
			case svOVERLAP:
				out << innerIt->first << " := { " << *( static_cast<Overlap*>(innerIt->second) )  << " } ";
				break;
			case svDIGITALINPUT:
				out << innerIt->first << " := { " << *( static_cast<DigitalInput*>(innerIt->second) ) << " } ";
				break;
			case svDIGITALOUTPUT:
				out << innerIt->first << " := { " << *( static_cast<DigitalOutput*>(innerIt->second) )  << " } ";
				break;
			case svCARTREF:
				out << innerIt->first << " := { " << *( static_cast<CartRef*>(innerIt->second) )  << " } ";
				break;
			case svTOOL:
				out << innerIt->first << " := { " << *( static_cast<CartRef*>(innerIt->second) )  << " } ";
				break;
			case svSTRING:
				out << innerIt->first << " := " << ( static_cast<char*>(innerIt->second) );
				break;
			case svLUAINTEGER:
				out << innerIt->first << " := " << *( static_cast<LuaInteger*>(innerIt->second) ); break;
			case svLUABOOL:
				out << innerIt->first << " := " << *( static_cast<LuaBool*>(innerIt->second) ); break;
			case svLUAREAL:
				out << innerIt->first << " := " << *( static_cast<LuaReal*>(innerIt->second) ); break;
			case svLUASTRING:
				out << innerIt->first << " := " << *( static_cast<LuaString*>(innerIt->second) ); break;
			case svCONVEYOR:
				out << innerIt->first << " :=  { " << *( static_cast<Conveyor*>(innerIt->second) ) << " } "; break;
			case svSensableObject:
				out << innerIt->first << " :=  { " << *( static_cast<SensableObject*>(innerIt->second) ) << " } "; break;
			case svSecurArea:
				out << innerIt->first << " :=  { " << *( static_cast<SecurArea*>(innerIt->second) ) << " } "; break;

			default: break;	
			}

			out << endl;
 		}
 	}
}

ScriptVarT ScriptVariables::get_variable_type( std::string strtype )
{
 	ScriptVarT st = svINTEGER;
	for( unsigned int i = 0; i < svTypeNum; ++i )
	{
		std::string varstr = VARINFO->vardesc[i]->getClassName();
		if ( varstr.compare(strtype) == 0 )
		{
			st = static_cast<ScriptVarT>(i);
			break;
		}
	}
	return st;
}

void ScriptVariables::modify_variable_ui(ScriptVarT st, std::string key, void* pvar, ScriptVarSetT svm)
{
 	set_value(st,key,pvar,svm);
}

const char* ScriptVariables::get_type_name( ScriptVarT st ) throw()
{
	return VARINFO->vardesc[st]->getClassName(); 
}

void ScriptVariables::creat_system_variable( )
{
	ScriptVarT st = svAXISPOS;
	PosInfo piFirstHome(PosInfo::CTaxis);
	PosInfo piSecondHome(PosInfo::CTaxis);
	piFirstHome.set_axis_deg(0,0,90,0,0,0,0,0);
	piSecondHome.set_axis_deg(0,0,0,0,0,0,0,0);
	creat_variable( st, "Home1", &piFirstHome, SVAR_SYSTEM );
	creat_variable( st, "Home2", &piSecondHome, SVAR_SYSTEM );

    st = svCARTPOS;
	PosInfo cpos(PosInfo::CTmachine);
	cpos.set_cart_pos(600,0,600,1.75,0,-1.75,5,0,0);
	creat_variable( st, "CARTPOS", &cpos, SVAR_SYSTEM );
 
	st = svDYNAMIC;
	Dynamic dyn;

	if ( SMC.is_main_motion_control_available() )
	{
		if ( SMC.isCodeSysMotion )
		{
			dyn.JointAcc = 0.2;
			dyn.JointDec = 0.2;
			dyn.JointJerk = 1.0;
			dyn.JointVel = 0.1;
			dyn.PathAcc = 300;
			dyn.PathDec = 500;
			dyn.PathJerk = 1000;
			dyn.PathVel = 10;
			dyn.Override = 1.0;
		}
		else
		{
			dyn.JointAcc = 30;
			dyn.JointDec = 30;
			dyn.JointJerk = 100.0;
			dyn.JointVel = 10; // main speed.
			dyn.PathAcc = 300;
			dyn.PathDec = 500;
			dyn.PathJerk = 1000;
			dyn.PathVel = 10;
			dyn.Override = 1.0;
		}
	}

 	creat_variable( st, "DYN", &dyn, SVAR_SYSTEM );
	
	st = svOVERLAP;
	Overlap ovl;
	ovl.iBufferMode = Overlap::mcBuffered;
	ovl.iTransMode = Overlap::TMNone;
 	creat_variable( st, "OVL", &ovl, SVAR_SYSTEM );
 
	st = svDIGITALINPUT;
	DigitalInput din;
	din.device_id = 0;
    din.port = 1;
	din.virtual_input = false;
	creat_variable( st, "DIN1", &din, SVAR_SYSTEM );

	st = svDIGITALOUTPUT;
	DigitalOutput dout;
	dout.device_id = 2;
	dout.port = 3;
 	creat_variable( st, "DOUT1", &dout, SVAR_SYSTEM );

	st = svCARTREF;
	CartRef cpref;
	cpref.cpos.pos.x = 0;
 	creat_variable( st, SYSTEM_CARTREF_NAME, &cpref, SVAR_SYSTEM );
 
	st = svTOOL;
	CartRef tool;
	tool.cpos.pos.y = 0;
 	creat_variable( st, SYSTEM_TOOL_NAME, &tool, SVAR_SYSTEM );	

    st = svLUASTRING;
	LuaString lstr;
	lstr.m_value = "SystemString";
 	creat_variable( st, "String", &lstr, SVAR_SYSTEM );
	
	st = svLUAREAL;
	LuaReal ldf;
	ldf.m_value = 3.14;
	creat_variable( st, "PIDF", &ldf, SVAR_SYSTEM );

 	st = svLUABOOL;
	LuaBool lbool;
	lbool.m_value = false;
	creat_variable( st, "BOOLEAN", &lbool, SVAR_SYSTEM );

	st = svLUAINTEGER;
	LuaInteger lint;
	lint.m_value = 599;
	creat_variable( st, "INT", &lint, SVAR_SYSTEM );
 
}

const std::map<ScriptVarT, std::map<std::string, void*>>* ScriptVariables::get_var_list(ScriptVarSetT svm /* = SVAR_PROGRAM */)
{
 	return &var_list[svm];
}

void ScriptVariables::load_subprogram_var(const char* modname, char* chHomePath )
{
	vector<string>::iterator result = find( sub_var_list.begin( ), sub_var_list.end( ), modname ); //查找3  
	if ( result == sub_var_list.end( ) ) //没找到 
	{
		cout << "no found sub variable\n " << endl;
		string newmod = string(modname);
		sub_var_list.push_back( newmod );
		
		if ( home_path.empty() == false )
		{
			string subvarname = home_path + string(modname)+ ".var";
			read_var_from(subvarname.c_str(),SVAR_PROGRAM,true); // append subprogram variable to global list.

			string strtmp = home_path;
			string::size_type pos = 0;
			while( (pos = strtmp.find_first_of('\\', pos ) ) != string::npos )  //把文件路径中的单斜杠“\”换成双斜杠“\\”
			{  
				strtmp.insert(pos,"\\");//插入  
				pos = pos + 2;  
			}  
			sprintf( chHomePath, "%s", strtmp.c_str() );
		}
		else
		{
			*chHomePath = '\0';
		}
	}
	else //找到 
	{
		cout << "Yes found sub variable\n" << endl;  
	}

}

void ScriptVariables::get_home_path(const char* fullpath)
{
	if ( home_path.empty() && fullpath )
	{
 		string fname = string(fullpath);
		int npos = fname.find_last_of('\\');
		if ( npos != string::npos )
 			home_path = fname.substr(0,npos+1);
  	}
}