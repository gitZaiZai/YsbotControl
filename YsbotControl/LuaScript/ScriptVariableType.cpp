// Copyright 2016, 亚思信息科技有限责任公司
// All right reserved.
//
// Author: Liu Run Dan ( lrdcz@163.com )
//
// Yasi Robot Motion Control System - 亚思机器人运动控制系统

#include "stdafx.h"
#include "ScriptVariableType.h"
#include "../stringconvert.h"

/*****************************************************************************/
/********************   STRUCT VARIABLEKEY  **********************************/
/*****************************************************************************/

VariableKey::VariableKey() throw ()
{
	sprintf(varName,"NULL");
	sprintf(propName,"NULL");
	sprintf(strKey,"NULL");
	varName[0]  = '\0';
	propName[0] = '\0';
	strKey[0]   = '\0';
    varSet = SVAR_PROGRAM;
	varTyp = svAXISPOS;
	propTyp = svINTEGER;
	nRow = -1;
	first_members = true;
	valid = true;
}

VariableKey::VariableKey (const VariableKey& src) throw ()
{
 	operator= (src);
}

const VariableKey& VariableKey::operator= (const VariableKey& src) throw ()
{
	sprintf(varName,"%s", src.varName);
	sprintf(propName,"%s", src.propName);
	sprintf(strKey,"%s", src.strKey);
	varSet = src.varSet;
	varTyp = src.varTyp;
	propTyp = src.propTyp;
	nRow   = src.nRow;
	first_members = src.first_members;
	valid = src.valid;
 	return *this;
}

/*****************************************************************************/
/********************   CLASS SCRIPTVARIABLETYPE   ***************************/
/*****************************************************************************/

ScriptVariableType::ScriptVariableType()
{
	setSeparatorStr(":");

	init_varialbe_prop();
 	classify_var_category( vardesc );

}

ScriptVariableType::~ScriptVariableType()
{
 	for ( int i=0; i < svTypeNum; ++i)
	{
		if ( vardesc[i])
		{
			delete vardesc[i];
			vardesc[i] = NULL;
		}
	}

	for ( int i=0; i < CATEGORY_NUM; ++i)
	{
		if ( var_category[i])
		{
			delete var_category[i];
			var_category[i] = NULL;
		}
	}
}

void ScriptVariableType::init_varialbe_prop()
{
	for ( int i=0; i < svTypeNum; ++i)
		vardesc[i] = NULL;

	sprintf( pose_label, "%s", "[ . . . ]" );

	char prop[256];
	char idstr[10];
	vardesc[svAXISPOS] = new GenericValues("APOS",8);
	for ( int i=0; i < 8; ++i )
	{
		sprintf( idstr, "%d",i+1 );
		sprintf( prop, "a%d: ANGLE",i+1 );
		vardesc[svAXISPOS]->addSetting(idstr, prop, (int)(svREAL) );
	}

	vardesc[svCARTPOS] = new GenericValues("CPOS",9);
	vardesc[svCARTPOS]->addSetting("1", "X: REAL",(int)(svREAL) );
	vardesc[svCARTPOS]->addSetting("2", "Y: REAL",(int)(svREAL));
	vardesc[svCARTPOS]->addSetting("3", "Z: REAL",(int)(svREAL));
	vardesc[svCARTPOS]->addSetting("4", "A: ANGLE",(int)(svREAL));
	vardesc[svCARTPOS]->addSetting("5", "B: ANGLE",(int)(svREAL));
	vardesc[svCARTPOS]->addSetting("6", "C: ANGLE",(int)(svREAL));
	vardesc[svCARTPOS]->addSetting("7", "Fig: INT",(int)(svINTEGER));
	vardesc[svCARTPOS]->addSetting("8", "a7: ANGLE",(int)(svREAL));
	vardesc[svCARTPOS]->addSetting("9", "a8: ANGLE",(int)(svREAL));

	vardesc[svINTEGER] = new GenericValues("INTEGER", 0);
	vardesc[svBOOL]    = new GenericValues("BOOLEA",0);
	vardesc[svRDI]     = new GenericValues("RDI", 0);
	vardesc[svRDO]     = new GenericValues("RDO", 0);
	vardesc[svREAL]    = new GenericValues("FLOAT",0);

	vardesc[svDYNAMIC] = new GenericValues("DYN",14); // 11
	vardesc[svDYNAMIC]->addSetting("1", "pathVel: REAL",(int)(svREAL));
	vardesc[svDYNAMIC]->addSetting("2", "pathAcc: REAL",(int)(svREAL));
	vardesc[svDYNAMIC]->addSetting("3", "pathDec: REAL",(int)(svREAL));
	vardesc[svDYNAMIC]->addSetting("4", "pathJerk: REAL",(int)(svREAL));
	vardesc[svDYNAMIC]->addSetting("5", "jointVel: PERCENT",(int)(svINTEGER));
	vardesc[svDYNAMIC]->addSetting("6", "jointAcc: PERCENT",(int)(svINTEGER));
	vardesc[svDYNAMIC]->addSetting("7", "jointDec: PERCENT",(int)(svINTEGER));
	vardesc[svDYNAMIC]->addSetting("8", "jointJerk: PERCENT",(int)(svINTEGER));
	vardesc[svDYNAMIC]->addSetting("9", "oriVel: REAL",(int)(svREAL));
	vardesc[svDYNAMIC]->addSetting("10", "oriAcc: REAL",(int)(svREAL));
	vardesc[svDYNAMIC]->addSetting("11", "oriDec: REAL",(int)(svREAL));
	vardesc[svDYNAMIC]->addSetting("12", "oriJerk: REAL",(int)(svREAL));
	vardesc[svDYNAMIC]->addSetting("13", "override: REAL",(int)(svREAL));
	vardesc[svDYNAMIC]->addSetting("14", "customParam: BOOL",(int)(svBOOL));

	vardesc[svOVERLAP] = new GenericValues("OVL",5);
	vardesc[svOVERLAP]->addSetting("1", "bufferMode: INT",(int)(svINTEGER));
	vardesc[svOVERLAP]->addSetting("2", "transMode: INT",(int)(svINTEGER));
	vardesc[svOVERLAP]->addSetting("3", "transParam0: REAL",(int)(svREAL));
	vardesc[svOVERLAP]->addSetting("4", "transParam1: REAL",(int)(svREAL));
	vardesc[svOVERLAP]->addSetting("5", "transParam2: REAL",(int)(svREAL));

	vardesc[svDIGITALINPUT] = new GenericValues("DIN",5);
	vardesc[svDIGITALINPUT]->addSetting("1", "deviceID: INT",(int)(svINTEGER));
	vardesc[svDIGITALINPUT]->addSetting("2", "port: INT",(int)(svINTEGER));
	vardesc[svDIGITALINPUT]->addSetting("3", "inputValue: INT",(int)(svINTEGER));
	vardesc[svDIGITALINPUT]->addSetting("4", "waitTime: INT",(int)(svINTEGER));
	vardesc[svDIGITALINPUT]->addSetting("5", "virtualInput: BOOL",(int)(svBOOL));

	vardesc[svDIGITALOUTPUT] = new GenericValues("DOUT",5);
	vardesc[svDIGITALOUTPUT]->addSetting("1", "deviceID: INT",(int)(svINTEGER));
	vardesc[svDIGITALOUTPUT]->addSetting("2", "port: INT",(int)(svINTEGER));
	vardesc[svDIGITALOUTPUT]->addSetting("3", "outputValue: INT",(int)(svINTEGER));
	vardesc[svDIGITALOUTPUT]->addSetting("4", "pulseTime: INT",(int)(svINTEGER));
	vardesc[svDIGITALOUTPUT]->addSetting("5", "forceOutput: BOOL",(int)(svBOOL));

	vardesc[svCARTREF] = new GenericValues("CARTREF",7);
	vardesc[svCARTREF]->addSetting("1", "X: REAL",(int)(svREAL));
	vardesc[svCARTREF]->addSetting("2", "Y: REAL",(int)(svREAL));
	vardesc[svCARTREF]->addSetting("3", "Z: REAL",(int)(svREAL));
	vardesc[svCARTREF]->addSetting("4", "A: ANGLE",(int)(svREAL));
	vardesc[svCARTREF]->addSetting("5", "B: ANGLE",(int)(svREAL));
	vardesc[svCARTREF]->addSetting("6", "C: ANGLE",(int)(svREAL));
	vardesc[svCARTREF]->addSetting("7", "refCart: STRING",(int)(svSTRING));

	vardesc[svTOOL] = new GenericValues("TOOL",7);
	vardesc[svTOOL]->addSetting("1", "X: REAL",(int)(svREAL));
	vardesc[svTOOL]->addSetting("2", "Y: REAL",(int)(svREAL));
	vardesc[svTOOL]->addSetting("3", "Z: REAL",(int)(svREAL));
	vardesc[svTOOL]->addSetting("4", "A: ANGLE",(int)(svREAL));
	vardesc[svTOOL]->addSetting("5", "B: ANGLE",(int)(svREAL));
	vardesc[svTOOL]->addSetting("6", "C: ANGLE",(int)(svREAL));
	vardesc[svTOOL]->addSetting("7", "refCart: STRING",(int)(svSTRING));

	vardesc[svSTRING] = new GenericValues("CHAR",0);

	vardesc[svLUAINTEGER] = new GenericValues("INT",0);
	vardesc[svLUABOOL]    = new GenericValues("BOOL",0);
	vardesc[svLUAREAL]    = new GenericValues("REAL",0);
	vardesc[svLUASTRING]  = new GenericValues("STRING",0);

	vardesc[svCONVEYOR] = new GenericValues("CONVEYOR",20);
	vardesc[svCONVEYOR]->addSetting("1", "coordType: INT",(int)(svINTEGER));
	vardesc[svCONVEYOR]->addSetting("2", "conveyorType: INT",(int)(svINTEGER));
	vardesc[svCONVEYOR]->addSetting("3", "speed: REAL",(int)(svREAL));
	vardesc[svCONVEYOR]->addSetting("4", "acc: REAL",(int)(svREAL));
	vardesc[svCONVEYOR]->addSetting("5", "deacc: REAL",(int)(svREAL));
	vardesc[svCONVEYOR]->addSetting("6", "jerk: REAL",(int)(svREAL));
 	vardesc[svCONVEYOR]->addSetting("7", "minArea: REAL",(int)(svREAL));
	vardesc[svCONVEYOR]->addSetting("8", "maxArea: REAL",(int)(svREAL));
	vardesc[svCONVEYOR]->addSetting("9", "xBeltOrg: REAL",(int)(svREAL));
	vardesc[svCONVEYOR]->addSetting("10", "yBeltOrg: REAL",(int)(svREAL));
	vardesc[svCONVEYOR]->addSetting("11", "zBeltOrg: REAL",(int)(svREAL));
	vardesc[svCONVEYOR]->addSetting("12", "aBeltOrg: ANGLE",(int)(svREAL));
	vardesc[svCONVEYOR]->addSetting("13", "bBeltOrg: ANGLE",(int)(svREAL));
	vardesc[svCONVEYOR]->addSetting("14", "cBeltOrg: ANGLE",(int)(svREAL));
	vardesc[svCONVEYOR]->addSetting("15", "xObjInitPos: REAL",(int)(svREAL));
	vardesc[svCONVEYOR]->addSetting("16", "yObjInitPos: REAL",(int)(svREAL));
	vardesc[svCONVEYOR]->addSetting("17", "zObjInitPos: REAL",(int)(svREAL));
	vardesc[svCONVEYOR]->addSetting("18", "aObjInitPos: ANGLE",(int)(svREAL));
	vardesc[svCONVEYOR]->addSetting("19", "bObjInitPos: ANGLE",(int)(svREAL));
	vardesc[svCONVEYOR]->addSetting("20", "cObjInitPos: ANGLE",(int)(svREAL));

	vardesc[svSensableObject] = new GenericValues("SENSABLEOBJECT",11);
	vardesc[svSensableObject]->addSetting("1", "coordType: INT",(int)(svINTEGER));
	vardesc[svSensableObject]->addSetting("2", "sensorType: INT",(int)(svINTEGER));
 	vardesc[svSensableObject]->addSetting("3", "simulate: BOOL",(int)(svBOOL));
	vardesc[svSensableObject]->addSetting("4", "xPos: REAL",(int)(svREAL));
	vardesc[svSensableObject]->addSetting("5", "yPos: REAL",(int)(svREAL));
	vardesc[svSensableObject]->addSetting("6", "zPos: REAL",(int)(svREAL));
	vardesc[svSensableObject]->addSetting("7", "aPos: ANGLE",(int)(svREAL));
	vardesc[svSensableObject]->addSetting("8", "bPos: ANGLE",(int)(svREAL));
	vardesc[svSensableObject]->addSetting("9", "cPos: ANGLE",(int)(svREAL));
	vardesc[svSensableObject]->addSetting("10", "width: REAL",(int)(svREAL));
	vardesc[svSensableObject]->addSetting("11", "height: REAL",(int)(svREAL));

	vardesc[svSecurArea] = new GenericValues("AREA",15);
	vardesc[svSecurArea]->addSetting("1", "shapeType: INT",(int)(svINTEGER));
	vardesc[svSecurArea]->addSetting("2", "areaType: INT",(int)(svINTEGER));
	vardesc[svSecurArea]->addSetting("3", "activeVar: INT",(int)(svINTEGER));
	vardesc[svSecurArea]->addSetting("4", "stateVar: INT",(int)(svINTEGER));
	vardesc[svSecurArea]->addSetting("5", "active: BOOL",(int)(svBOOL));
	vardesc[svSecurArea]->addSetting("6", "emgcOut: BOOL",(int)(svBOOL));
 	vardesc[svSecurArea]->addSetting("7", "xPos: REAL",(int)(svREAL));
	vardesc[svSecurArea]->addSetting("8", "yPos: REAL",(int)(svREAL));
	vardesc[svSecurArea]->addSetting("9", "zPos: REAL",(int)(svREAL));
	vardesc[svSecurArea]->addSetting("10", "aPos: ANGLE",(int)(svREAL));
	vardesc[svSecurArea]->addSetting("11", "bPos: ANGLE",(int)(svREAL));
	vardesc[svSecurArea]->addSetting("12", "cPos: ANGLE",(int)(svREAL));
 	vardesc[svSecurArea]->addSetting("13", "shapeParam0: REAL",(int)(svREAL));
	vardesc[svSecurArea]->addSetting("14", "shapeParam1: REAL",(int)(svREAL));
	vardesc[svSecurArea]->addSetting("15", "shapeParam2: REAL",(int)(svREAL));

}

// 【全称 ： 解释 ： 缩写】
const char* ScriptVariableType::getVarDescStr( ScriptVarT sct )
{
	string strVariableInfo = vardesc[sct]->getClassName()+string(getSeparatorStr());
	string strVarDesc = "";
	switch( sct )
	{
	case	    svAXISPOS: strVarDesc  = " 关节轴位置，位置变量不能重名" + string(getSeparatorStr()) + string("ap"); break;             
	case		svCARTPOS: strVarDesc  = " 笛卡尔位置，位置变量不能重名" + string(getSeparatorStr()) + string("cp"); break; 
	case		svINTEGER: strVarDesc  = " 整型                        " + string(getSeparatorStr()) + string("i"); break; 
	case		svBOOL:    strVarDesc  = " 布尔型                      " + string(getSeparatorStr()) + string("b"); break; 
	case		svRDI:     strVarDesc  = " 远程输入变量                " + string(getSeparatorStr()) + string("rdi"); break; 
	case		svRDO:     strVarDesc  = " 远程输出变量                " + string(getSeparatorStr()) + string("rdo"); break; 
	case		svREAL:    strVarDesc  = " 实数型                      " + string(getSeparatorStr()) + string("df"); break; 
	case		svDYNAMIC: strVarDesc  = " 速度加减速动力学            " + string(getSeparatorStr()) + string("dyn"); break; 
	case		svOVERLAP: strVarDesc  = " 路径重叠平滑                " + string(getSeparatorStr()) + string("ovl"); break; 
	case		svDIGITALINPUT: strVarDesc  = " 数字输入变量           " + string(getSeparatorStr()) + string("din"); break; 
	case		svDIGITALOUTPUT: strVarDesc = " 数字输出变量           " + string(getSeparatorStr()) + string("dout"); break; 
	case		svCARTREF:      strVarDesc  = " 参考坐标               " + string(getSeparatorStr()) + string("cref"); break; 
	case		svTOOL:         strVarDesc  = " 工具坐标               " + string(getSeparatorStr()) + string("tool"); break; 
	case		svSTRING:       strVarDesc  = " 字符串                 " + string(getSeparatorStr()) + string("str"); break; 
	case		svLUAINTEGER:   strVarDesc  = " 整型                   " + string(getSeparatorStr()) + string("i"); break; 
	case		svLUABOOL:      strVarDesc  = " 布尔型                 " + string(getSeparatorStr()) + string("b"); break; 
	case		svLUAREAL:      strVarDesc  = " 实数型                 " + string(getSeparatorStr()) + string("df"); break; 
	case		svLUASTRING:    strVarDesc  = " 字符串                 " + string(getSeparatorStr()) + string("str"); break; 
	case		svCONVEYOR:     strVarDesc  = " 传送带                 " + string(getSeparatorStr()) + string("conve"); break; 
	case		svSensableObject: strVarDesc  = " 可感知物体对象       " + string(getSeparatorStr()) + string("object"); break; 
	case		svSecurArea: strVarDesc       = " 安全区域             " + string(getSeparatorStr()) + string("area"); break; 
	default:                    return NULL;
	}

	strVariableInfo += strVarDesc;
	strcpy ( strVariable, strVariableInfo.c_str() );

	return strVariable;

}


void ScriptVariableType::classify_var_category( GenericValues** scriptvar)
{
	for ( int i=0; i < CATEGORY_NUM; ++i)
		var_category[i] = NULL;

	var_category[BASIC_KIND] = new GenericValues("基本类型",4);
	var_category[BASIC_KIND]->addSetting("1", getVarDescStr(svLUAINTEGER), (int)(svLUAINTEGER) );
	var_category[BASIC_KIND]->addSetting("2", getVarDescStr(svLUABOOL), (int)(svLUABOOL) );
	var_category[BASIC_KIND]->addSetting("3", getVarDescStr(svLUAREAL), (int)(svLUAREAL) );
	var_category[BASIC_KIND]->addSetting("4", getVarDescStr(svLUASTRING), (int)(svLUASTRING) );

	var_category[POSE_KIND] = new GenericValues("位置相关",2);
	var_category[POSE_KIND]->addSetting("1", getVarDescStr(svAXISPOS), (int)(svAXISPOS) );
	var_category[POSE_KIND]->addSetting("2",getVarDescStr(svCARTPOS), (int)(svCARTPOS) );

	var_category[MOTION_KIND] = new GenericValues("运动相关", 2);
	var_category[MOTION_KIND]->addSetting("1", getVarDescStr(svDYNAMIC), (int)(svDYNAMIC) );
	var_category[MOTION_KIND]->addSetting("2",getVarDescStr(svOVERLAP), (int)(svOVERLAP) );

	var_category[COORDINATE_KIND] = new GenericValues("坐标工具", 4);
	var_category[COORDINATE_KIND]->addSetting("1", getVarDescStr(svCARTREF), (int)(svCARTREF));
	var_category[COORDINATE_KIND]->addSetting("2", getVarDescStr(svTOOL), (int)(svTOOL));
	var_category[COORDINATE_KIND]->addSetting("3", getVarDescStr(svCONVEYOR), (int)(svCONVEYOR));
	var_category[COORDINATE_KIND]->addSetting("4", getVarDescStr(svSensableObject), (int)(svSensableObject));
 
	var_category[SYSTEM_KIND] = new GenericValues("系统技术", 2);
// 	char strIdx[32];
// 	for ( int i = 0 ;i < 20; ++i )
// 	{
// 		sprintf(strIdx,"%d",i);
// 	    var_category[SYSTEM_KIND]->addSetting(strIdx, ( string("area:")+"AREA" ).c_str()); // future develop
//  	}
	var_category[SYSTEM_KIND]->addSetting("1", getVarDescStr(svSecurArea), (int)(svSecurArea) ); // future develop
	var_category[SYSTEM_KIND]->addSetting("2", ( string("TIME")+getSeparatorStr()+"时间变量            "+getSeparatorStr()+"time" ).c_str());

	var_category[INOUT_KIND] = new GenericValues("输入输出",4);
	var_category[INOUT_KIND]->addSetting("1", getVarDescStr(svDIGITALINPUT), (int)(svDIGITALINPUT));
	var_category[INOUT_KIND]->addSetting("2", getVarDescStr(svDIGITALOUTPUT), (int)(svDIGITALOUTPUT));
	var_category[INOUT_KIND]->addSetting("3", getVarDescStr(svRDI), (int)(svRDI));
	var_category[INOUT_KIND]->addSetting("4", getVarDescStr(svRDO), (int)(svRDO));


}
 
void ScriptVariableType::get_prop( ScriptVarT sv, std::vector<std::string>& strprop, bool hasType )
{
	strprop.clear();

	char charProp[VAR_STR_LEN];
	std::string strname = "";
	for( int i = 0; i < vardesc[sv]->getValuesTotal(); ++i )
	{
		vardesc[sv]->getValue( i+1, charProp );
		if ( charProp == '\0')
		{
 			strname = "";
		}
		else
		{
			if ( hasType )
				strname = string(charProp);
			else
			{
				std::deque<std::string> proplist;
				split_string( proplist, charProp, SPLIT_STR );

				if ( proplist.size() > 1 )
				{
					strname = proplist[0];
				}
				proplist.clear();
			}
		}
 		strprop.push_back(strname);
	}
 
}

const char* ScriptVariableType::getSeparatorStr( ) const
{
	return strSeparator;
}

bool ScriptVariableType::setSeparatorStr( char * str )
{
	strcpy( strSeparator, str );
	return true;
}

bool ScriptVariableType::isBaseVariableType (  ScriptVarT sct  ) const
{
	return sct == svINTEGER  || sct == svBOOL  || sct == svSTRING || sct == svREAL;
}
