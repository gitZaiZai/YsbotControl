// Copyright 2016, ��˼��Ϣ�Ƽ��������ι�˾
// All right reserved.
//
// Author: Liu Run Dan ( lrdcz@163.com )
//
// Yasi Robot Motion Control System - ��˼�������˶�����ϵͳ

#include "stdafx.h"
#include "ScriptCommandInfo.h"
#include "ScriptVariables.h"
#include <regex>
#include "../stringconvert.h"

using namespace std;

void RegParseResult::clear() {	params.clear(); }

ScriptCommandInfo::ScriptCommandInfo()
{
	setSeparatorStr("@");

	init_parameters_list();
	init_category_list();
	regStringList.clear();
  	init_regex_list();
}

ScriptCommandInfo::~ScriptCommandInfo()
{
	for ( int i=0; i < SM_MAX_COMMANDS; ++i)
	{
		if ( CommandParameters[i])
		{
			delete CommandParameters[i];
			CommandParameters[i] = NULL;
		}
 	}

	for ( int i=0; i < SM_MAX_SETS; ++i)
	{
		if ( CommandCategory[i])
		{
			delete CommandCategory[i];
			CommandCategory[i] = NULL;
		}
	}
	regStringList.clear();

}

void ScriptCommandInfo::init_parameters_list()
{
 	for ( int i=0; i < SM_MAX_COMMANDS; ++i)
		CommandParameters[i] = NULL;

 	// basic macro
	CommandParameters[SM_PTPABS] = new GenericValues("PTP",3);
	CommandParameters[SM_PTPABS]->addSetting("1", "pos: POSITION_", (int)(svAXISPOS) );
	CommandParameters[SM_PTPABS]->addSetting("2", "dyn: DYNAMIC_(��ѡ����)", (int)(svDYNAMIC) );
	CommandParameters[SM_PTPABS]->addSetting("3", "ovl: OVERLAP_(��ѡ����)", (int)(svOVERLAP) );

	CommandParameters[SM_PTPREL] = new GenericValues("PTPRel",3);
	CommandParameters[SM_PTPREL]->addSetting("1", "dist: DISTANCE_", (int)(svAXISPOS) );
	CommandParameters[SM_PTPREL]->addSetting("2", "dyn: DYNAMIC_(��ѡ����)", (int)(svDYNAMIC) );
	CommandParameters[SM_PTPREL]->addSetting("3", "ovl: OVERLAP_(��ѡ����)", (int)(svOVERLAP) );

	CommandParameters[SM_LINEABS] = new GenericValues("Lin",3);
	CommandParameters[SM_LINEABS]->addSetting("1", "pos: POSITION_", (int)(svAXISPOS) );
	CommandParameters[SM_LINEABS]->addSetting("2", "dyn: DYNAMIC_(��ѡ����)", (int)(svDYNAMIC) );
	CommandParameters[SM_LINEABS]->addSetting("3", "ovl: OVERLAP_(��ѡ����)", (int)(svOVERLAP) );

	CommandParameters[SM_LINEREL] = new GenericValues("LinRel",3);
	CommandParameters[SM_LINEREL]->addSetting("1", "dist: DISTANCE_", (int)(svAXISPOS) );
	CommandParameters[SM_LINEREL]->addSetting("2", "dyn: DYNAMIC_(��ѡ����)", (int)(svDYNAMIC) );
	CommandParameters[SM_LINEREL]->addSetting("3", "ovl: OVERLAP_(��ѡ����)", (int)(svOVERLAP) );

	CommandParameters[SM_CIRCLEABS] = new GenericValues("Cir",4);
	CommandParameters[SM_CIRCLEABS]->addSetting("1", "pos: POSITION_", (int)(svAXISPOS) );
	CommandParameters[SM_CIRCLEABS]->addSetting("2", "pos: POSITION_", (int)(svAXISPOS) );
	CommandParameters[SM_CIRCLEABS]->addSetting("3", "dyn: DYNAMIC_(��ѡ����)", (int)(svDYNAMIC) );
	CommandParameters[SM_CIRCLEABS]->addSetting("4", "ovl: OVERLAP_(��ѡ����)", (int)(svOVERLAP) );

	CommandParameters[SM_CIRCLEREL] = new GenericValues("CirRel",4);
	CommandParameters[SM_CIRCLEREL]->addSetting("1", "dist: DISTANCE_", (int)(svAXISPOS) );
	CommandParameters[SM_CIRCLEREL]->addSetting("2", "dist: DISTANCE_", (int)(svAXISPOS) );
	CommandParameters[SM_CIRCLEREL]->addSetting("3", "dyn: DYNAMIC_(��ѡ����)", (int)(svDYNAMIC) );
	CommandParameters[SM_CIRCLEREL]->addSetting("4", "ovl: OVERLAP_(��ѡ����)", (int)(svOVERLAP) );

	CommandParameters[SM_WAITTIME] = new GenericValues("WaitTime",1);
	CommandParameters[SM_WAITTIME]->addSetting("1", "timeMs: DINT",(int)(svINTEGER));

	CommandParameters[SM_WAITISFINISHED] = new GenericValues("WaitIsFinished",0);

	// set up 
	CommandParameters[SM_DYNAMIC] = new GenericValues("Dyn",1);
	CommandParameters[SM_DYNAMIC]->addSetting("1", "dyn: DYNAMIC_", (int)(svDYNAMIC) );

 	CommandParameters[SM_OVERLAP] = new GenericValues("Ovl",1);
	CommandParameters[SM_OVERLAP]->addSetting("1", "ovl: OVERLAP_", (int)(svOVERLAP) );

	CommandParameters[SM_REFSYS] = new GenericValues("RefSys",1);
	CommandParameters[SM_REFSYS]->addSetting("1", "refSys: REFSYS_", (int)(svCARTREF) );

	CommandParameters[SM_TOOL] = new GenericValues("Tool",1);
	CommandParameters[SM_TOOL]->addSetting("1", "tool: TOOL_", (int)(svTOOL) );

	CommandParameters[SM_SPEED] = new GenericValues("SPEED",1);
	CommandParameters[SM_SPEED]->addSetting("1", "speed: PERCENT",(int)(svINTEGER));

	CommandParameters[SM_BLEND] = new GenericValues("BLEND",1);
	CommandParameters[SM_BLEND]->addSetting("1", "blend: BOOL",(int)(svBOOL));

	// I/O module
	CommandParameters[SM_DINWAIT] = new GenericValues("DIN:Wait",3);
	CommandParameters[SM_DINWAIT]->addSetting("1", "din: DIN", (int)(svDIGITALINPUT) );
	CommandParameters[SM_DINWAIT]->addSetting("2", "val: BOOL",(int)(svBOOL));
	CommandParameters[SM_DINWAIT]->addSetting("3", "timeMs: DINT",(int)(svINTEGER));

	CommandParameters[SM_DOUTSET] = new GenericValues("DOUT:Set",2);
	CommandParameters[SM_DOUTSET]->addSetting("1", "dout: DOUT", (int)(svDIGITALOUTPUT) );
	CommandParameters[SM_DOUTSET]->addSetting("2", "val: BOOL",(int)(svBOOL));

	CommandParameters[SM_DOUTPULSE] = new GenericValues("DOUT:Pulse",3);
	CommandParameters[SM_DOUTPULSE]->addSetting("1", "dout: DOUT",  (int)(svDIGITALOUTPUT) );
	CommandParameters[SM_DOUTPULSE]->addSetting("2", "val: BOOL",   (int)(svBOOL));
	CommandParameters[SM_DOUTPULSE]->addSetting("3", "timeMs: DINT",(int)(svINTEGER));
 
	CommandParameters[SM_ONDISTANCE] = new GenericValues("OnDistance",4);
	CommandParameters[SM_ONDISTANCE]->addSetting("1", "type: DISTANCETYPE",(int)(svINTEGER));
	CommandParameters[SM_ONDISTANCE]->addSetting("2", "dist: REAL",        (int)(svREAL));
	CommandParameters[SM_ONDISTANCE]->addSetting("3", "dout: DOUT",        (int)(svDIGITALOUTPUT) );
	CommandParameters[SM_ONDISTANCE]->addSetting("4", "val: BOOL",         (int)(svBOOL));

	CommandParameters[SM_ONPOSITION] = new GenericValues("OnPosition",3);
	CommandParameters[SM_ONPOSITION]->addSetting("1", "path: PERCENT",     (int)(svINTEGER));
	CommandParameters[SM_ONPOSITION]->addSetting("2", "dout: DOUT",        (int)(svDIGITALOUTPUT) );
	CommandParameters[SM_ONPOSITION]->addSetting("3", "val: BOOL",         (int)(svBOOL));

	CommandParameters[SM_READWHILEMOTION] = new GenericValues("ReadWhileMotion",3);
	CommandParameters[SM_READWHILEMOTION]->addSetting("1", "path: PERCENT",  (int)(svINTEGER));
	CommandParameters[SM_READWHILEMOTION]->addSetting("2", "bitNr: INT",     (int)(svINTEGER));
	CommandParameters[SM_READWHILEMOTION]->addSetting("3", "deviceType: INT",(int)(svINTEGER));

	CommandParameters[SM_DIWAIT] = new GenericValues("DIWait",5);
	CommandParameters[SM_DIWAIT]->addSetting("1", "deviceType: INT",(int)(svINTEGER));
	CommandParameters[SM_DIWAIT]->addSetting("2", "bitNr: INT",     (int)(svINTEGER));
	CommandParameters[SM_DIWAIT]->addSetting("3", "val: BOOL",      (int)(svBOOL));
	CommandParameters[SM_DIWAIT]->addSetting("4", "timeMs: DINT",   (int)(svINTEGER));
	CommandParameters[SM_DIWAIT]->addSetting("5", "virtual: BOOL",  (int)(svBOOL));

	CommandParameters[SM_DIREAD] = new GenericValues("DIRead",4);
	CommandParameters[SM_DIREAD]->addSetting("1", "deviceType: INT",(int)(svINTEGER));
	CommandParameters[SM_DIREAD]->addSetting("2", "bitNr: INT",     (int)(svINTEGER));
	CommandParameters[SM_DIREAD]->addSetting("3", "queue: BOOL",    (int)(svBOOL));
	CommandParameters[SM_DIREAD]->addSetting("4", "virtual: BOOL",  (int)(svBOOL));

	CommandParameters[SM_DOSET] = new GenericValues("DOSet",5);
	CommandParameters[SM_DOSET]->addSetting("1", "deviceType: INT",(int)(svINTEGER));
	CommandParameters[SM_DOSET]->addSetting("2", "bitNr: INT",     (int)(svINTEGER));
	CommandParameters[SM_DOSET]->addSetting("3", "val: BOOL",      (int)(svBOOL));
	CommandParameters[SM_DOSET]->addSetting("4", "queue: BOOL",    (int)(svBOOL));
	CommandParameters[SM_DOSET]->addSetting("5", "delayTimeMs: DINT",(int)(svINTEGER));


	// ���ܿ� application module
	CommandParameters[SM_TRACKVIS] = new GenericValues("TrackVis",2);
	CommandParameters[SM_TRACKVIS]->addSetting("1", "dyn: DYNAMIC_(��ѡ����)", (int)(svDYNAMIC) );
	CommandParameters[SM_TRACKVIS]->addSetting("2", "ovl: OVERLAP_(��ѡ����)", (int)(svOVERLAP) );

	CommandParameters[SM_INFOTIP] = new GenericValues("Info",1);
	CommandParameters[SM_INFOTIP]->addSetting("1", "text: STRING",(int)(svSTRING));

	CommandParameters[SM_LOADSUB] = new GenericValues("LOADSUB",1);
	CommandParameters[SM_LOADSUB]->addSetting("1", "text: STRING",(int)(svSTRING));

	CommandParameters[SM_GETROBPOS] = new GenericValues("GetRobPos",1); // 
	CommandParameters[SM_GETROBPOS]->addSetting("1", "pos: POSITION",(int)(svAXISPOS));

	// declaration
	CommandParameters[SM_DEF_AXISPOS] = new GenericValues("APOS", 2 );
	CommandParameters[SM_DEF_AXISPOS]->addSetting("1", "localVariable: BOOL",(int)(svBOOL));
	CommandParameters[SM_DEF_AXISPOS]->addSetting("2", "varName: STRING",(int)(svSTRING));

	CommandParameters[SM_DEF_CARTPOS] = new GenericValues("CPOS", 2 );
	CommandParameters[SM_DEF_CARTPOS]->addSetting("1", "localVariable: BOOL",(int)(svBOOL));
	CommandParameters[SM_DEF_CARTPOS]->addSetting("2", "varName: STRING",(int)(svSTRING));

	CommandParameters[SM_DEF_POSINFO] = new GenericValues("POSINFO", 2 );
	CommandParameters[SM_DEF_POSINFO]->addSetting("1", "localVariable: BOOL",(int)(svBOOL));
	CommandParameters[SM_DEF_POSINFO]->addSetting("2", "varName: STRING",(int)(svSTRING));

	CommandParameters[SM_DEF_REFSYS] = new GenericValues("CARTREF", 2 );
	CommandParameters[SM_DEF_REFSYS]->addSetting("1", "localVariable: BOOL",(int)(svBOOL));
	CommandParameters[SM_DEF_REFSYS]->addSetting("2", "varName: STRING",(int)(svSTRING));

	CommandParameters[SM_DEF_TOOL] = new GenericValues("TOOL", 2 );
	CommandParameters[SM_DEF_TOOL]->addSetting("1", "localVariable: BOOL",(int)(svBOOL));
	CommandParameters[SM_DEF_TOOL]->addSetting("2", "varName: STRING",(int)(svSTRING));

	CommandParameters[SM_DEF_DIN] = new GenericValues("DIN", 2 );
	CommandParameters[SM_DEF_DIN]->addSetting("1", "localVariable: BOOL",(int)(svBOOL));
	CommandParameters[SM_DEF_DIN]->addSetting("2", "varName: STRING",(int)(svSTRING));

	CommandParameters[SM_DEF_DOUT] = new GenericValues("DOUT", 2 );
	CommandParameters[SM_DEF_DOUT]->addSetting("1", "localVariable: BOOL",(int)(svBOOL));
	CommandParameters[SM_DEF_DOUT]->addSetting("2", "varName: STRING",(int)(svSTRING));

	CommandParameters[SM_DEF_INT] = new GenericValues("INT", 2 );
	CommandParameters[SM_DEF_INT]->addSetting("1", "localVariable: BOOL",(int)(svBOOL));
	CommandParameters[SM_DEF_INT]->addSetting("2", "varName: STRING",(int)(svSTRING));

	CommandParameters[SM_DEF_BOOL] = new GenericValues("BOOL", 2 );
	CommandParameters[SM_DEF_BOOL]->addSetting("1", "localVariable: BOOL",(int)(svBOOL));
	CommandParameters[SM_DEF_BOOL]->addSetting("2", "varName: STRING",(int)(svSTRING));

	CommandParameters[SM_DEF_REAL] = new GenericValues("REAL", 2 );
	CommandParameters[SM_DEF_REAL]->addSetting("1", "localVariable: BOOL",(int)(svBOOL));
	CommandParameters[SM_DEF_REAL]->addSetting("2", "varName: STRING",(int)(svSTRING));

	CommandParameters[SM_DEF_STRING] = new GenericValues("STRING", 2 );
	CommandParameters[SM_DEF_STRING]->addSetting("1", "localVariable: BOOL",(int)(svBOOL));
	CommandParameters[SM_DEF_STRING]->addSetting("2", "varName: STRING",(int)(svSTRING));

	CommandParameters[SM_SYS_LOCAL] = new GenericValues("local", 1 );
	CommandParameters[SM_SYS_LOCAL]->addSetting("1", "varName: STRING",(int)(svSTRING));

	// system

	CommandParameters[SM_SYS_IFELSE] = new GenericValues("if ( ... ) then ...end",0);
	CommandParameters[SM_SYS_ELSEIF] = new GenericValues("elseif ( ... ) then",0);
	CommandParameters[SM_SYS_ELSE] = new GenericValues("else",0);
	CommandParameters[SM_SYS_WHILEDOEND] = new GenericValues("while ( ... ) do ...end",0);
   	CommandParameters[SM_SYS_REPEATUNTIL] = new GenericValues("repeat ...until ( ... )",1);
	CommandParameters[SM_SYS_REPEATUNTIL]->addSetting("1", "loopCount: DINT",(int)(svINTEGER));

 	CommandParameters[SM_SYS_DEFINEDFUNCTION] = new GenericValues("function name ( ... ) ...end",0);

	CommandParameters[SM_SYS_PRINT] = new GenericValues("print",1);
	CommandParameters[SM_SYS_PRINT]->addSetting("1", "text: STRING",(int)(svSTRING));

	// system functions
	CommandParameters[SM_SYS_ABS] = new GenericValues("math.abs",1);
	CommandParameters[SM_SYS_ABS]->addSetting("1", "param: REAL",(int)(svREAL));

	CommandParameters[SM_SYS_SIN] = new GenericValues("math.sin",1);
	CommandParameters[SM_SYS_SIN]->addSetting("1", "param: REAL",(int)(svREAL));

	CommandParameters[SM_SYS_COS] = new GenericValues("math.cos",1);
	CommandParameters[SM_SYS_COS]->addSetting("1", "param: REAL",(int)(svREAL));

	CommandParameters[SM_SYS_ASIN] = new GenericValues("math.asin",1);
	CommandParameters[SM_SYS_ASIN]->addSetting("1", "param: REAL",(int)(svREAL));

	CommandParameters[SM_SYS_ACOS] = new GenericValues("math.acos",1);
	CommandParameters[SM_SYS_ACOS]->addSetting("1", "param: REAL",(int)(svREAL));

	CommandParameters[SM_SYS_TAN] = new GenericValues("math.tan",1);
	CommandParameters[SM_SYS_TAN]->addSetting("1", "param: REAL",(int)(svREAL));

	CommandParameters[SM_SYS_ATAN] = new GenericValues("math.atan",2);
	CommandParameters[SM_SYS_ATAN]->addSetting("1", "numerator: REAL",(int)(svREAL));
	CommandParameters[SM_SYS_ATAN]->addSetting("2", "denominator: REAL",(int)(svREAL));

	CommandParameters[SM_SYS_ATAN2] = new GenericValues("math.atan2",2);
	CommandParameters[SM_SYS_ATAN2]->addSetting("1", "numerator: REAL",(int)(svREAL));
	CommandParameters[SM_SYS_ATAN2]->addSetting("2", "denominator: REAL",(int)(svREAL));

	CommandParameters[SM_SYS_FLOOR] = new GenericValues("math.floor",1);
    CommandParameters[SM_SYS_FLOOR]->addSetting("1", "param: REAL",(int)(svREAL));

	CommandParameters[SM_SYS_CEIL] = new GenericValues("math.ceil",1);
	CommandParameters[SM_SYS_CEIL]->addSetting("1", "param: REAL",(int)(svREAL));

	CommandParameters[SM_SYS_RAD] = new GenericValues("math.rad",1);
	CommandParameters[SM_SYS_RAD]->addSetting("1", "param: REAL",(int)(svREAL));
	CommandParameters[SM_SYS_DEG] = new GenericValues("math.deg",1);
	CommandParameters[SM_SYS_DEG]->addSetting("1", "param: REAL",(int)(svREAL));

	CommandParameters[SM_SYS_EXP] = new GenericValues("math.exp",1);
	CommandParameters[SM_SYS_EXP]->addSetting("1", "param: REAL",(int)(svREAL));
	CommandParameters[SM_SYS_SQRT] = new GenericValues("math.sqrt",1);
	CommandParameters[SM_SYS_SQRT]->addSetting("1", "param: REAL",(int)(svREAL));
	CommandParameters[SM_SYS_LOG] = new GenericValues("math.log",1);
	CommandParameters[SM_SYS_LOG]->addSetting("1", "param: REAL",(int)(svREAL));
	CommandParameters[SM_SYS_MOD] = new GenericValues("math.mod",1);
	CommandParameters[SM_SYS_MOD]->addSetting("1", "param: REAL",(int)(svREAL));

	CommandParameters[SM_SYS_RANDOM] = new GenericValues("math.random",2);
	CommandParameters[SM_SYS_RANDOM]->addSetting("1", "minVal: REAL",(int)(svREAL));
	CommandParameters[SM_SYS_RANDOM]->addSetting("2", "maxVal: REAL",(int)(svREAL));

}

const char* ScriptCommandInfo::getCmdDescStr( ScriptCommandT sct )
{
 	string strCommandInfo = CommandParameters[sct]->getClassName()+string(getSeparatorStr());
	string strCommandDesc = "";
	switch( sct )
	{
	case SM_PTPABS:				strCommandDesc = "�㵽���˶�������λ��";	break;              
	case SM_PTPREL:				strCommandDesc = "�㵽���˶������λ��";	break;              
	case SM_LINEABS:			strCommandDesc = "ֱ���˶�������λ��";		break;
	case SM_LINEREL:			strCommandDesc = "ֱ���˶������λ��";		break;
	case SM_CIRCLEABS:			strCommandDesc = "Բ���˶�������λ��";		break;
	case SM_CIRCLEREL:			strCommandDesc = "ֱ���˶������λ��";		break;
	case SM_WAITTIME:			strCommandDesc = "��ʱָ��ʱ��";			break;
	case SM_WAITISFINISHED:		strCommandDesc = "�ȴ��˶�����";			break;

	case SM_DYNAMIC:			strCommandDesc = "���ö���ָ��Ķ���ѧ����"; break;
	case SM_OVERLAP:			strCommandDesc = "���ù켣��ƽ���ȼ�";			break;
	case SM_REFSYS:				strCommandDesc = "�����û�����";				break;
	case SM_TOOL:				strCommandDesc = "���ù�������";				break;
	case SM_SPEED:				strCommandDesc = "����ȫ������";				break;
	case SM_BLEND:				strCommandDesc = "�����켣ƽ��";				break;

	case SM_DINWAIT:			strCommandDesc = "�������ȴ�ָ��IO�˿ڵ��ź�";		break;
	case SM_DOUTSET:			strCommandDesc = "�������IO�˿���λ��λ";			break;
	case SM_DOUTPULSE:          strCommandDesc = "���������IO�˿���������ź�";		break;
	case SM_ONDISTANCE:         strCommandDesc = "��·���м�ָ��λ�����IO�ź�";        break;
	case SM_ONPOSITION:         strCommandDesc = "��·���м�ָ��λ�ã��ٷֱȣ����IO�ź�"; break;
	case SM_READWHILEMOTION:    strCommandDesc = "���˶������ж�ȡIO�����ź�";				break;
	case SM_DIWAIT:				strCommandDesc = "�ȴ�IO�˿��ź�";							 break;
	case SM_DIREAD:				strCommandDesc = "����IO�˿��ź�";							break;
	case SM_DOSET:				strCommandDesc = "IO�˿���λ��λ"; break;

	case SM_TRACKVIS:			strCommandDesc = "�����Ӿ�ʶ�𵽵�Ŀ��λ��"; break;
	case SM_INFOTIP:			strCommandDesc = "����Ϣ�����������Ϣ"; break;
	case SM_GETROBPOS:          strCommandDesc = "��ȡ�����˵�ǰλ��"; break;
	case SM_LOADSUB:			strCommandDesc = "�����ӳ���ģ��"; break;

	case SM_DEF_AXISPOS:        strCommandDesc = "����ؽڱ���"; break;
	case SM_DEF_CARTPOS:        strCommandDesc = "����ֱ�Ǳ���"; break;
	case SM_DEF_POSINFO:        strCommandDesc = "����λ�ñ���"; break;
	case SM_DEF_REFSYS:         strCommandDesc = "����ο��������"; break;
	case SM_DEF_TOOL:			strCommandDesc = "���幤���������"; break;
	case SM_DEF_DIN:			strCommandDesc = "��������ģ�����"; break;
	case SM_DEF_DOUT:			strCommandDesc = "�������ģ�����"; break;
	case SM_DEF_INT:			strCommandDesc = "�������ͱ���"; break;
	case SM_DEF_BOOL:			strCommandDesc = "���岼���ͱ���"; break;
	case SM_DEF_REAL:			strCommandDesc = "����ʵ������"; break;
	case SM_DEF_STRING:         strCommandDesc = "�����ַ�������"; break;

	case SM_SYS_LOCAL:          strCommandDesc = "����ֲ�����"; break;
	case SM_SYS_IFELSE:         strCommandDesc = "�����߼�ѡ������"; break;
	case SM_SYS_ELSEIF:         strCommandDesc = "�����߼�ѡ������"; break;
	case SM_SYS_ELSE:           strCommandDesc = "�����߼�ѡ������"; break;
	case SM_SYS_WHILEDOEND:     strCommandDesc = "����ѭ������"; break;
 	case SM_SYS_REPEATUNTIL:    strCommandDesc = "�м������������Ƶĳ���ѭ��"; break;
	case SM_SYS_DEFINEDFUNCTION:strCommandDesc = "�����Ӻ���ģ��"; break;

	case SM_SYS_PRINT:          strCommandDesc = "�����Ϣ�����Կ���̨"; break;
	case SM_SYS_ABS:			strCommandDesc = "��ȡ����ֵ"; break;
	case SM_SYS_SIN:			strCommandDesc = "����"; break;
	case SM_SYS_COS:			strCommandDesc = "����"; break;
	case SM_SYS_ASIN:			strCommandDesc = "������"; break;
	case SM_SYS_ACOS:			strCommandDesc = "������"; break;
	case SM_SYS_TAN:			strCommandDesc = "����"; break;
	case SM_SYS_ATAN:			strCommandDesc = "������"; break;
	case SM_SYS_ATAN2:          strCommandDesc = "������"; break;
	case SM_SYS_FLOOR:          strCommandDesc = "��������"; break;
	case SM_SYS_CEIL:			strCommandDesc = "��������"; break;
	case SM_SYS_RAD:			strCommandDesc = "�Ƕ�ת����"; break;
	case SM_SYS_DEG:			strCommandDesc = "����ת�Ƕ�"; break;
	case SM_SYS_EXP:			strCommandDesc = "ָ������"; break;
	case SM_SYS_SQRT:			strCommandDesc = "����һ���Ǹ�ʵ����ƽ����"; break;
	case SM_SYS_LOG:			strCommandDesc = "��������"; break;
	case SM_SYS_MOD:			strCommandDesc = "���ຯ��"; break;
	case SM_SYS_RANDOM:         strCommandDesc = "��������ָ������������"; break;
	default:                    return NULL;
	}

	strCommandInfo += strCommandDesc;
	strcpy ( strCommand, strCommandInfo.c_str() );

	return strCommand;

}

void ScriptCommandInfo::init_category_list()
{
	for ( int i=0; i < SM_MAX_SETS; ++i)
		CommandCategory[i] = NULL;
 
	CommandCategory[SM_SET_MOVEMENT] = new GenericValues("�˶�",8);
	CommandCategory[SM_SET_MOVEMENT]->addSetting("1", getCmdDescStr(SM_PTPABS),			(int)(SM_PTPABS) );
	CommandCategory[SM_SET_MOVEMENT]->addSetting("2", getCmdDescStr(SM_PTPREL),			(int)(SM_PTPREL)         );
	CommandCategory[SM_SET_MOVEMENT]->addSetting("3", getCmdDescStr(SM_LINEABS),		(int)(SM_LINEABS)        );
	CommandCategory[SM_SET_MOVEMENT]->addSetting("4", getCmdDescStr(SM_LINEREL),		(int)(SM_LINEREL)        );
	CommandCategory[SM_SET_MOVEMENT]->addSetting("5", getCmdDescStr(SM_CIRCLEABS),		(int)(SM_CIRCLEABS)      );
	CommandCategory[SM_SET_MOVEMENT]->addSetting("6", getCmdDescStr(SM_CIRCLEREL),		(int)(SM_CIRCLEREL)      );
	CommandCategory[SM_SET_MOVEMENT]->addSetting("7", getCmdDescStr(SM_WAITTIME),		(int)(SM_WAITTIME)       );
	CommandCategory[SM_SET_MOVEMENT]->addSetting("8", getCmdDescStr(SM_WAITISFINISHED), (int)(SM_WAITISFINISHED) );
 
	CommandCategory[SM_SET_SETTING] = new GenericValues("����",6);
	CommandCategory[SM_SET_SETTING]->addSetting("1", getCmdDescStr(SM_DYNAMIC),			(int)(SM_DYNAMIC) );
	CommandCategory[SM_SET_SETTING]->addSetting("2", getCmdDescStr(SM_OVERLAP),			(int)(SM_OVERLAP) );
	CommandCategory[SM_SET_SETTING]->addSetting("3", getCmdDescStr(SM_REFSYS),          (int)(SM_REFSYS)  );
	CommandCategory[SM_SET_SETTING]->addSetting("4", getCmdDescStr(SM_TOOL),            (int)(SM_TOOL)    );
	CommandCategory[SM_SET_SETTING]->addSetting("5", getCmdDescStr(SM_SPEED),           (int)(SM_SPEED)   );
	CommandCategory[SM_SET_SETTING]->addSetting("6", getCmdDescStr(SM_BLEND),           (int)(SM_BLEND)   );
	
	CommandCategory[SM_SET_SYSTEM] = new GenericValues("ϵͳ",8);
	CommandCategory[SM_SET_SYSTEM]->addSetting("1", getCmdDescStr(SM_SYS_IFELSE),			(int)(SM_SYS_IFELSE) );
	CommandCategory[SM_SET_SYSTEM]->addSetting("2", getCmdDescStr(SM_SYS_ELSEIF),			(int)(SM_SYS_ELSEIF) );
	CommandCategory[SM_SET_SYSTEM]->addSetting("3", getCmdDescStr(SM_SYS_ELSE),			    (int)(SM_SYS_ELSE) );
	CommandCategory[SM_SET_SYSTEM]->addSetting("4", getCmdDescStr(SM_SYS_WHILEDOEND),		(int)(SM_SYS_WHILEDOEND) );
	CommandCategory[SM_SET_SYSTEM]->addSetting("5", getCmdDescStr(SM_SYS_REPEATUNTIL),	    (int)(SM_SYS_REPEATUNTIL) );
	CommandCategory[SM_SET_SYSTEM]->addSetting("6", getCmdDescStr(SM_SYS_LOCAL),			(int)(SM_SYS_LOCAL) );
	CommandCategory[SM_SET_SYSTEM]->addSetting("7", getCmdDescStr(SM_SYS_DEFINEDFUNCTION),	(int)(SM_SYS_DEFINEDFUNCTION) );
	CommandCategory[SM_SET_SYSTEM]->addSetting("8", getCmdDescStr(SM_SYS_PRINT),			(int)(SM_SYS_PRINT) );

	CommandCategory[SM_SET_SYSFUNCTIONS] = new GenericValues("����",17);
	CommandCategory[SM_SET_SYSFUNCTIONS]->addSetting("1", getCmdDescStr(SM_SYS_ABS),				(int)(SM_SYS_ABS) );
	CommandCategory[SM_SET_SYSFUNCTIONS]->addSetting("2", getCmdDescStr(SM_SYS_SIN),				(int)(SM_SYS_SIN) );
	CommandCategory[SM_SET_SYSFUNCTIONS]->addSetting("3", getCmdDescStr(SM_SYS_COS),				(int)(SM_SYS_COS) );
	CommandCategory[SM_SET_SYSFUNCTIONS]->addSetting("4", getCmdDescStr(SM_SYS_ASIN),			    (int)(SM_SYS_ASIN) );
	CommandCategory[SM_SET_SYSFUNCTIONS]->addSetting("5", getCmdDescStr(SM_SYS_ACOS),			    (int)(SM_SYS_ACOS) );
	CommandCategory[SM_SET_SYSFUNCTIONS]->addSetting("6", getCmdDescStr(SM_SYS_TAN),				(int)(SM_SYS_TAN) );
	CommandCategory[SM_SET_SYSFUNCTIONS]->addSetting("7", getCmdDescStr(SM_SYS_ATAN),				(int)(SM_SYS_ATAN) );
	CommandCategory[SM_SET_SYSFUNCTIONS]->addSetting("8", getCmdDescStr(SM_SYS_ATAN2),				(int)(SM_SYS_ATAN2) );
	CommandCategory[SM_SET_SYSFUNCTIONS]->addSetting("9", getCmdDescStr(SM_SYS_FLOOR),				(int)(SM_SYS_FLOOR) );
	CommandCategory[SM_SET_SYSFUNCTIONS]->addSetting("10", getCmdDescStr(SM_SYS_CEIL),				(int)(SM_SYS_CEIL) );
	CommandCategory[SM_SET_SYSFUNCTIONS]->addSetting("11", getCmdDescStr(SM_SYS_RAD),				(int)(SM_SYS_RAD) );
	CommandCategory[SM_SET_SYSFUNCTIONS]->addSetting("12", getCmdDescStr(SM_SYS_DEG),				(int)(SM_SYS_DEG) );
	CommandCategory[SM_SET_SYSFUNCTIONS]->addSetting("13", getCmdDescStr(SM_SYS_EXP),				(int)(SM_SYS_EXP) );
	CommandCategory[SM_SET_SYSFUNCTIONS]->addSetting("14", getCmdDescStr(SM_SYS_SQRT),				(int)(SM_SYS_SQRT) );
	CommandCategory[SM_SET_SYSFUNCTIONS]->addSetting("15", getCmdDescStr(SM_SYS_LOG),				(int)(SM_SYS_LOG) );
	CommandCategory[SM_SET_SYSFUNCTIONS]->addSetting("16", getCmdDescStr(SM_SYS_MOD),				(int)(SM_SYS_MOD) );
	CommandCategory[SM_SET_SYSFUNCTIONS]->addSetting("17", getCmdDescStr(SM_SYS_RANDOM),			(int)(SM_SYS_RANDOM) );
 
	CommandCategory[SM_SET_INPUTOUTPUT] = new GenericValues("�������",9);
	CommandCategory[SM_SET_INPUTOUTPUT]->addSetting("1", getCmdDescStr(SM_DINWAIT),			(int)(SM_DINWAIT) );
	CommandCategory[SM_SET_INPUTOUTPUT]->addSetting("2", getCmdDescStr(SM_DOUTSET),			(int)(SM_DOUTSET) );
	CommandCategory[SM_SET_INPUTOUTPUT]->addSetting("3", getCmdDescStr(SM_DOUTPULSE),		(int)(SM_DOUTPULSE) );
	CommandCategory[SM_SET_INPUTOUTPUT]->addSetting("4", getCmdDescStr(SM_ONDISTANCE),		(int)(SM_ONDISTANCE) );
	CommandCategory[SM_SET_INPUTOUTPUT]->addSetting("5", getCmdDescStr(SM_ONPOSITION),		(int)(SM_ONPOSITION) );
	CommandCategory[SM_SET_INPUTOUTPUT]->addSetting("6", getCmdDescStr(SM_READWHILEMOTION), (int)(SM_READWHILEMOTION) );
	CommandCategory[SM_SET_INPUTOUTPUT]->addSetting("7", getCmdDescStr(SM_DIWAIT),			(int)(SM_DIWAIT) );
	CommandCategory[SM_SET_INPUTOUTPUT]->addSetting("8", getCmdDescStr(SM_DIREAD),			(int)(SM_DIREAD) );
	CommandCategory[SM_SET_INPUTOUTPUT]->addSetting("9", getCmdDescStr(SM_DOSET),			(int)(SM_DOSET) );
  
	CommandCategory[SM_SET_DECLARATION] = new GenericValues("��������",11);
	CommandCategory[SM_SET_DECLARATION]->addSetting("1", getCmdDescStr(SM_DEF_AXISPOS),		(int)(SM_DEF_AXISPOS) );
	CommandCategory[SM_SET_DECLARATION]->addSetting("2", getCmdDescStr(SM_DEF_CARTPOS),		(int)(SM_DEF_CARTPOS) );
	CommandCategory[SM_SET_DECLARATION]->addSetting("3", getCmdDescStr(SM_DEF_POSINFO),		(int)(SM_DEF_POSINFO) );
	CommandCategory[SM_SET_DECLARATION]->addSetting("4", getCmdDescStr(SM_DEF_REFSYS),		(int)(SM_DEF_REFSYS)  );
	CommandCategory[SM_SET_DECLARATION]->addSetting("5", getCmdDescStr(SM_DEF_TOOL),		(int)(SM_DEF_TOOL)	  );
	CommandCategory[SM_SET_DECLARATION]->addSetting("6", getCmdDescStr(SM_DEF_DIN),			(int)(SM_DEF_DIN)	  );
	CommandCategory[SM_SET_DECLARATION]->addSetting("7", getCmdDescStr(SM_DEF_DOUT),		(int)(SM_DEF_DOUT)    );
	CommandCategory[SM_SET_DECLARATION]->addSetting("8", getCmdDescStr(SM_DEF_INT),			(int)(SM_DEF_INT)     );
	CommandCategory[SM_SET_DECLARATION]->addSetting("9", getCmdDescStr(SM_DEF_BOOL),		(int)(SM_DEF_BOOL)    );
	CommandCategory[SM_SET_DECLARATION]->addSetting("10", getCmdDescStr(SM_DEF_REAL),		(int)(SM_DEF_REAL)    );
	CommandCategory[SM_SET_DECLARATION]->addSetting("11", getCmdDescStr(SM_DEF_STRING),		(int)(SM_DEF_STRING)  );

 	CommandCategory[SM_SET_CUSTOMFUNCTIONS] = new GenericValues("���ƹ���",4);
	CommandCategory[SM_SET_CUSTOMFUNCTIONS]->addSetting("1", getCmdDescStr(SM_TRACKVIS),	(int)(SM_TRACKVIS)  );
	CommandCategory[SM_SET_CUSTOMFUNCTIONS]->addSetting("2", getCmdDescStr(SM_INFOTIP),		(int)(SM_INFOTIP)   );
	CommandCategory[SM_SET_CUSTOMFUNCTIONS]->addSetting("3", getCmdDescStr(SM_GETROBPOS),	(int)(SM_GETROBPOS) );
	CommandCategory[SM_SET_CUSTOMFUNCTIONS]->addSetting("4", getCmdDescStr(SM_LOADSUB),		(int)(SM_LOADSUB) );

// 	CommandCategory[SM_SET_FLOWCONTROL] = new GenericValues("���̿���",4);
// 	CommandCategory[SM_SET_FLOWCONTROL]->addSetting("1", getCmdDescStr(SM_SYS_IFELSE),			(int)(SM_SYS_IFELSE) );
// 	CommandCategory[SM_SET_FLOWCONTROL]->addSetting("2", getCmdDescStr(SM_SYS_WHILEDOEND),		(int)(SM_SYS_WHILEDOEND) );
// 	CommandCategory[SM_SET_FLOWCONTROL]->addSetting("3", getCmdDescStr(SM_SYS_FORDOEND),		(int)(SM_SYS_FORDOEND) );
// 	CommandCategory[SM_SET_FLOWCONTROL]->addSetting("4", getCmdDescStr(SM_SYS_REPEATUNTIL),	    (int)(SM_SYS_REPEATUNTIL) );


	
}
  
const char* ScriptCommandInfo::getSeparatorStr( ) const
{
  return strSeparator;
}
 
bool ScriptCommandInfo::setSeparatorStr( char * str )
{
  strcpy( strSeparator, str );
  return true;
}

bool ScriptCommandInfo::isClassMemberFunction( ScriptCommandT sct ) const
{
	return sct == SM_DOUTSET  || sct == SM_DOUTPULSE  || sct == SM_DINWAIT;
}

bool ScriptCommandInfo::isVariableDeclaration( ScriptCommandT sct ) const
{
	return sct >= SM_DEF_AXISPOS && sct <= SM_DEF_STRING;
}


bool ScriptCommandInfo::isLuaSystemFunction(ScriptCommandT sct) const
{
	return ( sct >= SM_SYS_LOCAL && sct <= SM_SYS_DEFINEDFUNCTION );
}

void ScriptCommandInfo::init_regex_list()
{
 	regSplitCmd         = "^(\\w{2,50}):([A-Za-z]{3,50})$"; // split [ din:Set ]
	regSplitReturnVal   = "^\\w*\\.?\\w*\\s*=\\s*(\\w{2,50}\\.[A-Za-z]{3,50})$"; // [ aa.b3 = math.random( 2.3, -4.5 ) ]
// 	string regDefString = "^\\s*([A-Za-z]{3,50})\\s*(\\w{2,50})(?:\\s*|\\s*--.*)$";
	string regDefString = "^(?:\\s*|\\s*local\\s*)(\\w{2,50})\\s*=\\s*([A-Za-z]{3,50})\\s*\\(\\s*\\)(?:\\s*|\\s*--.*)$"; // [ local var1 = APOS() ]
	string reFunction   = "([A-Za-z]{3,50}|\\w{2,50}:[A-Za-z]{3,50}|\\w*\\.?\\w*\\s*=?\\s*\\w{2,50}\\.[A-Za-z]{3,50})"; // [ PTP | din:Set | math.sin ]
	string reParam      = "\\s*(\\w+|\\w{2,50}\\.\\w+|[-+]?[0-9]*\\.?[0-9]+)\\s*";                            // [ Apos_1 or 9 or false or -0.314] 
	 
	string re1="^\\s*";      // text start	
	string re2=reFunction;	 // group 1
	string re3="\\s*";	     // space
	string re4="\\(";        // '('
	string re5="(";	         // ?: not save in group, save all parameters to group2 when not use [?:].
	string re6  = reParam+"|";	 // group3
	string re7  = reParam+","+reParam+"|";	         // group4, group5 Variable Name 1 and 2
	string re8  = reParam+","+reParam+","+reParam+"|";	 // group6, group7, group8, var 1,2,3
	string re9  = reParam+","+reParam+","+reParam+","+reParam+"|";	         // group4, group5 Variable Name 1 and 2
	string re10 = reParam+","+reParam+","+reParam+","+reParam+","+reParam;	 // group6, group7, group8, var 1,2,3
	string re11=")";     // group2 end.
	string re12="\\)";	// '('
	string re13="(?:\\s*|\\s*--.*)$";	// text end

	string regCmdString = re1+re2+re3+re4+re5+re6+re7+re8+re9+re10+re11+re12+re13 ;
  
	regStringList.push_back(regDefString);   
	regStringList.push_back(regCmdString);
  
}

int ScriptCommandInfo::parse_command( const char* strCmd, RegParseResult* result )
{
	// ������ҵĽ��     
	result->valid = false;
	result->nCmdType = 0;
	result->clear();

	std::locale loc("");    
	std::cout.imbue(loc);    // ��������ʱ������֧������·����
 	 
	string text = string(strCmd);
	smatch ms;  
	regex regExpress;    
	bool matchSuccess = false;
	unsigned int nCmdRegType = 0;
	for ( unsigned int i = 0; i < regStringList.size(); ++i )
	{
		regExpress = regex(regStringList[i]);
		if( regex_match( text, ms, regExpress ) )    
		{    
			cout<<("������ʽƥ��ɹ�")<<std::endl;  
			matchSuccess = true;
			nCmdRegType = i;
			break;
		}    
	}

	if ( !matchSuccess || ms.size() < 3 )
 		return 0;
 
	result->valid = true;
 
	unsigned int isize = ms.size();
	 
	for( unsigned int i = 0; i < ms.size(); i++ )
	{
		std::cout << "regRes :" << ms[i].str() << endl;	
	}

	string strCmdName = "";
	string objectName = "";
	ScriptCommandT scmdType = SM_PTPABS;
	string regCmd = ( nCmdRegType == 0 ? ms[2].str() : ms[1].str() );
	regex regCmdExpress(regSplitCmd); // split ':'
	regex regCmdRetExpress(regSplitReturnVal); // split ':'
	smatch msPart;  

	if( regex_match( regCmd, msPart, regCmdExpress ) ) 
	{
		nCmdRegType = 2;
		objectName = msPart[1].str();
		regCmd     = msPart[2].str();
	}
    else if ( regex_match( regCmd, msPart, regCmdRetExpress ) )
    {
		regCmd     = msPart[1].str();
    }

	bool bfind = false;
 	for ( int i = SM_PTPABS; i < SM_MAX_COMMANDS; ++i )
	{
		strCmdName = string(CommandParameters[i]->getClassName());
		if ( i == SM_DINWAIT || i == SM_DOUTSET || i == SM_DOUTPULSE )
		{
			strCmdName = strCmdName.substr( strCmdName.find_last_of(":")+1 ); // cut object name, reserve :Set.
		}
	 
 		if ( strcmp(regCmd.c_str(),strCmdName.c_str()) == 0 )
		{
			scmdType = (ScriptCommandT)(i);
			bfind = true;
			break;
		}
 		 
	}

	if ( !bfind )
 		return 0;
 
	result->sct = scmdType;
	result->nCmdType = nCmdRegType;

	vector<string> params;

	//0 : var1 = APOS(), 1: PTP(),  2 : dinv1:Set()
	if ( nCmdRegType == 0 )
	{
		result->nCmdStart = ms[2].first - text.begin();
		result->nCmdEnd   = ms[2].second - text.begin();

		result->nParamStart = ms[1].first - text.begin();
		result->nParamEnd   = ms[1].second- text.begin();

		size_t fi = text.find("local", 0);    
		if (fi != text.npos )
			params.push_back("true");
		else
			params.push_back("false");
		params.push_back(ms[1].str());
	}
	else 
	{
		result->nCmdStart = ms[1].first - text.begin();
		result->nCmdEnd   = ms[1].second - text.begin();

		result->nParamStart = ms[2].first - text.begin();
		result->nParamEnd   = ms[2].second- text.begin();

		if ( nCmdRegType == 2)
		{
			params.push_back(objectName);
		}
 
		for( unsigned int i = 3; i < ms.size(); i++ )
		{
			if ( ms[i].str().length() > 0 )
			{
				params.push_back(ms[i].str());
			}
		}
	}
  
	VariableKey vkey;
	vkey.valid = true;
	std::string paramName;

	for ( int i = 0; i < CommandParameters[scmdType]->getValuesTotal(); ++i )
	{
		vkey.varTyp = (ScriptVarT)(CommandParameters[scmdType]->getValueKind(i+1));
		vkey.varSet = SVAR_PROGRAM;

		if ( (unsigned int)(i) < params.size() )
		{
			paramName = params[i];
			if ( strstr( paramName.c_str(), STR_LABEL_SYSTEM ) )
			{
				vkey.varSet = SVAR_SYSTEM;
				paramName = paramName.substr( paramName.find_last_of("_")+1 ); // cut label, reserve name
			}
			else if ( strstr(params[i].c_str(),STR_LABEL_GLOBAL) )
			{
				vkey.varSet = SVAR_GLOBAL;
				paramName = paramName.substr( paramName.find_last_of("_")+1 );
			}
 
			sprintf( vkey.varName, "%s", paramName.c_str() );
		}
		else
		{
			sprintf( vkey.varName, "%s", "nil" );
			vkey.varTyp = svSTRING; 
 		}
 		result->params.push_back(vkey);
	}
 
	return 1;
}