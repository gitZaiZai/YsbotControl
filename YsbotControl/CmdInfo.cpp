// Copyright 2016, 亚思信息科技有限责任公司
// All right reserved.
//
// Author: Liu Run Dan ( lrdcz@163.com )
//
// Yasi Robot Motion Control System - 亚思机器人运动控制系统

#include "StdAfx.h"
#include "CmdInfo.h"
#include "LuaScript/ScriptVariables.h"


Dynamic::Dynamic()
{
	PathVel = 10.0;  //	位置速度(mm/s)
	PathAcc = 50.0;  //	位置加速度(mm/s^2)
	PathDec = 50.0;  //	位置减速度(mm/s^2)
	PathJerk = 1000000.0;

	OriVel = 10.0;   //	旋转速度(deg/s)
	OriAcc = 10.0;   //	旋转加速度(deg/s^2)
	OriDec = 10.0;   //    旋转减速度(deg/s^2)
	OriJerk = 1000000.0;

	JointVel = 10.0; //	关节速度(%)
	JointAcc = 30.0; //	关节加速度(%)
	JointDec = 30.0; //	关节减速度(%)
	JointJerk = 100.0;

	Override = 0.0;
	custom_parameter = false;
}

const Dynamic& Dynamic::operator= (const Dynamic& dyn) 
{
	PathVel  = dyn.PathVel;
	PathAcc  = dyn.PathAcc;
	PathDec  = dyn.PathDec;
	PathJerk = dyn.PathJerk;

	JointVel  = dyn.JointVel;
	JointAcc  = dyn.JointAcc;
	JointDec  = dyn.JointDec;
	JointJerk = dyn.JointJerk;

	OriVel = dyn.OriVel;
	OriAcc = dyn.OriAcc;
	OriDec = dyn.OriDec;
	OriJerk = dyn.OriJerk;

	Override = dyn.Override;
	custom_parameter = dyn.custom_parameter;
	return *this;
}

char* Dynamic::get_value( const char* name, char* strValue )
{
	check_value( name, false, strValue ); 
	return strValue;
}

void Dynamic::set_value( const char* name, char* strValue)
{
	check_value(name,true,strValue);
}

void Dynamic::check_value( const char* name, const bool bset,  char* val )
{
	double* dfdyn[] = { &PathVel, &PathAcc, &PathDec, &PathJerk, 
		&JointVel,&JointAcc,&JointDec, &JointJerk,
		&OriVel,  &OriAcc,  &OriDec, &OriJerk,
		&Override, NULL };

	char prop[VAR_STR_LEN];
	for ( int i = 0; i < VARINFO->vardesc[svDYNAMIC]->getValuesTotal(); i++ )
	{
		VARINFO->vardesc[svDYNAMIC]->getValue(i+1,prop);
		if ( prop[0] != '\0' &&  strstr( prop, name ) )
		{
			if ( bset )
			{
				if (dfdyn[i])
				{
					if ( strstr(prop,"Joint") )
						*dfdyn[i] = atoi( val ? val : 0 );
					else
						*dfdyn[i] = atof( val ? val : 0 );
				}
				else
				{
					bool b = false;
					if( !val )
						;
					else if( isdigit( val[ 0 ] ) )
						b = atoi( val ? val : 0 ) > 0 ? true : false;
					else if( strcmp( val, "on"  ) == 0 ||
						strcmp( val, "true") == 0 ||
						strcmp( val, "yes" ) == 0 )
						b = true;
					custom_parameter = ( b == true ) ? true : false;
				}
			}
			else
			{
				if ( strstr(prop,"Joint") )
				{
					sprintf( val, "%d", *dfdyn[i] );
				}
				else
					sprintf( val, "%lf", dfdyn[i] == NULL ? custom_parameter : *dfdyn[i] );
			}
			return;
		}
	}

	if ( !bset )
		*val = '\0';


}

Overlap::Overlap()
{
	iBufferMode = Overlap::mcBuffered;
	iTransMode  = Overlap::TMNone;
	TransitionParameter[0] = 0.0;
	TransitionParameter[1] = 0.0;
	TransitionParameter[2] = 0.0;
}

const Overlap& Overlap::operator= (const Overlap& ove) 
{
	iBufferMode = ove.iBufferMode;
	iTransMode  = ove.iTransMode;

	for ( int i = 0; i < 3; ++i )
 		TransitionParameter[i] = ove.TransitionParameter[i];

 	return *this;
}

char* Overlap::get_value( const char* name, char* strValue )
{
	check_value( name, false, strValue ); 
	return strValue;
}

void Overlap::set_value( const char* name, char* strValue)
{
	check_value(name,true,strValue);
}

void Overlap::check_value( const char* name, const bool bset,  char* val )
{
	double* dfovl[] = { NULL, 
		NULL, 
		&TransitionParameter[0], 
		&TransitionParameter[1],
		&TransitionParameter[2] };

	char prop[VAR_STR_LEN];
	for ( int i = 0; i < VARINFO->vardesc[svOVERLAP]->getValuesTotal(); i++ )
	{
		VARINFO->vardesc[svOVERLAP]->getValue(i+1,prop);
		if ( prop[0] != '\0' &&  strstr( prop, name ) )
		{
			if ( bset )
			{
				if (dfovl[i])
					*dfovl[i] = atof( val ? val : 0 );
				else
				{
 					if( !val )
						;
					else 
					{
 					   if ( i == 0 )
					   {
						   iBufferMode = BufferMode( atoi( val ? val : 0 ) );
					   }
					   else
						   iTransMode = TransitionMode( atoi( val ? val : 0 ) );
 					}
 				}
			}
			else
			{
				if ( dfovl[i] == NULL )
				{
					sprintf( val, "%d", i == 0 ? iBufferMode : iTransMode );
				}
				else
				    sprintf( val, "%lf", *dfovl[i] );
			}
			return;
		}
	}

	if ( !bset )
		*val = '\0';

}

CmdInfo::CmdInfo(void)
{
	iObjNo        = -1;
	iSourceLineNo = -1;
	iCommandType     = CMD_NULL;
	 
// 	iPosCoordType   = mcACS;
 
	dyn.JointAcc = 100.0;
	dyn.JointDec = 100.0;
	dyn.JointVel = 10.0;
	dyn.PathAcc  = 100.0;
	dyn.PathDec  = 100.0;
	dyn.PathVel  = 10.0;
	dyn.OriAcc   = 10.0;
	dyn.OriDec   = 10.0;
	dyn.OriVel   = 10.0;
	dyn.Override = 10.0;
	dyn.custom_parameter = false;
 	 
	dec_time = 0.0;

	ovl.iBufferMode = Overlap::mcBuffered;
	ovl.iTransMode  = Overlap::TMNone;
	for ( int i=0; i < 3; i++ )
	{
		ovl.TransitionParameter[i] = 0.0;
	}
 
	cartref = NULL;
	tool    = NULL;

	HomeMark = 0;
 
	iInOutNo = -1;
	iIoValue =  0;

	chJogAxisMark = 0x0000;
	chJogDirMark  = 0x0000;

	iDelayTime = 0;

	iJogCoordType = jtJOINT;

	customStartPos = false;
	bOnOff         = false;
}


CmdInfo::~CmdInfo(void)
{
}

const CmdInfo& CmdInfo::operator=( const CmdInfo& gi ) throw ()
{
	iObjNo        = gi.iObjNo;
	iSourceLineNo = gi.iSourceLineNo;
	iCommandType  = gi.iCommandType;

// 	iPosCoordType = gi.iPosCoordType;
	 
	startPos  = gi.startPos ;
	destPos   = gi.destPos ;
	auxPos      = gi.auxPos ;

	cartref      = gi.cartref; // user coordinate system
	tool         = gi.tool;    // tool coordinate system

	dyn.JointAcc = gi.dyn.JointAcc;
	dyn.JointDec = gi.dyn.JointDec;
	dyn.JointVel = gi.dyn.JointVel;
	dyn.JointJerk = gi.dyn.JointJerk;

	dyn.PathAcc  = gi.dyn.PathAcc;
	dyn.PathDec  = gi.dyn.PathDec;
	dyn.PathVel  = gi.dyn.PathVel;
	dyn.PathJerk = gi.dyn.PathJerk;

	dyn.OriAcc   = gi.dyn.OriAcc;
	dyn.OriDec   = gi.dyn.OriDec;
	dyn.OriVel   = gi.dyn.OriVel;
	dyn.OriJerk  = gi.dyn.OriJerk;

	dyn.Override = gi.dyn.Override;
	dyn.custom_parameter = gi.dyn.custom_parameter; 

	ovl.iBufferMode = gi.ovl.iBufferMode;
	ovl.iTransMode  = gi.ovl.iTransMode;
	for ( int i=0; i < 3; i++ )
		ovl.TransitionParameter[i] = gi.ovl.TransitionParameter[i];

	dec_time    = gi.dec_time;

	HomeMark    = gi.HomeMark;
 
	iInOutNo      = gi.iInOutNo;
	iIoValue      = gi.iIoValue;

	chJogAxisMark = gi.chJogAxisMark;
	chJogDirMark  = gi.chJogDirMark;

	iDelayTime    = gi.iDelayTime;

	iJogCoordType = gi.iJogCoordType;

	customStartPos = gi.customStartPos;
	bOnOff         = gi.bOnOff;

 	return *this;
}

std::ostream& operator<< (std::ostream& os, const Dynamic& v)
{
	os <<' '<< v.PathVel  << ' ' << v.PathAcc <<' '<< v.PathDec << ' ' << v.PathJerk << 
		' '<< v.JointVel << ' ' << v.JointAcc << ' ' << v.JointDec << ' ' << v.JointJerk <<
		' '<< v.OriVel  << ' ' << v.OriAcc  <<' '<< v.OriDec  << ' ' << v.OriJerk  <<  
		' '<< v.Override <<' '<< (int)v.custom_parameter << ' ';
	return os;
}

std::istream& operator>> (std::istream &in, Dynamic &v)
{
	int temp = 0;
	in >> v.PathVel >> v.PathAcc >> v.PathDec >> v.PathJerk >> 
		v.JointVel >> v.JointAcc >> v.JointDec >> v.JointJerk >> 
		v.OriVel  >> v.OriAcc  >> v.OriDec >> v.OriJerk >>
		v.Override >> temp;
	v.custom_parameter = (temp == 1) ? true : false ;

	return in;
}

std::ostream& operator<< (std::ostream& os, const Overlap& v)
{
	os <<' '<< v.iBufferMode << ' ' << v.iTransMode<<' '<< v.TransitionParameter[0] << ' ' << v.TransitionParameter[1] <<' '<< v.TransitionParameter[2] << ' ';
	return os;
}

std::istream& operator>> (std::istream &in, Overlap &v)
{
	int temp = 0;
	in >> temp;
	v.iBufferMode = static_cast<Overlap::BufferMode>(temp);
	in >> temp;
 	v.iTransMode  = static_cast<Overlap::TransitionMode>(temp);
	in >> v.TransitionParameter[0] >> v.TransitionParameter[1] >> v.TransitionParameter[2];

	return in;
}
