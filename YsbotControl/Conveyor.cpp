// Copyright 2016, 亚思信息科技有限责任公司
// All right reserved.
//
// Author: Liu Run Dan ( lrdcz@163.com )
//
// Yasi Robot Motion Control System - 亚思机器人运动控制系统

#include "stdafx.h"
#include "Conveyor.h"
#include "LuaScript/ScriptVariables.h"
#include "MotionControl.h"
#include "LuaScript/ScriptFunc.h"

Conveyor::Conveyor () : max_work_area(500),min_work_area(200)  
{
	iConveyorType = 0;
	iCoordType  = 1;

	belt_speed = 0.0;
	belt_acceleration = 100;
	belt_deceleration = 100;
	belt_jerk = 1000;

	xBeltOrg = 0;
	yBeltOrg = 0;
	zBeltOrg = 0;
	aBeltOrg = 0;
	bBeltOrg = 0;
	cBeltOrg = 0;

	xObjInitPos = 0;
	yObjInitPos = 0;
	zObjInitPos = 0;
	aObjInitPos = 0;
	bObjInitPos = 0;
	cObjInitPos = 0;

	sprintf(carRefObj.selfname,"%s","refSysObj");
	sprintf(carRefObj.cartname,"%s","PCS_2");

}

Conveyor::Conveyor (const Conveyor& di) 
{ 
	operator= (di); 
}

const Conveyor& Conveyor::operator= (const Conveyor& di) 
{
	this->iCoordType = di.iCoordType;
	this->iConveyorType = di.iConveyorType;

	this->belt_speed         = di.belt_speed;
	this->belt_acceleration = di.belt_acceleration;
	this->belt_deceleration = di.belt_deceleration;
	this->belt_jerk = di.belt_jerk;

	this->min_work_area = di.min_work_area;
	this->max_work_area = di.max_work_area;
	
	this->xBeltOrg = di.xBeltOrg;
	this->yBeltOrg = di.yBeltOrg;
	this->zBeltOrg = di.zBeltOrg;
	this->aBeltOrg = di.aBeltOrg;
	this->bBeltOrg = di.bBeltOrg;
	this->cBeltOrg = di.cBeltOrg;

	this->xObjInitPos = di.xObjInitPos;
	this->yObjInitPos = di.yObjInitPos;
	this->zObjInitPos = di.zObjInitPos;
	this->aObjInitPos = di.aObjInitPos;
	this->bObjInitPos = di.bObjInitPos;
	this->cObjInitPos = di.cObjInitPos;

	this->carRefObj = di.carRefObj;

	return *this;
}

void Conveyor::begin_track() throw ()
{
	if ( ScriptFunc::is_command_ignored() )
	{
		return;
	}

	CmdInfo cmd;
	cmd.iCommandType = CMD_CONVEYORBEGIN;
	cmd.destPos.pos_type = ( iCoordType == 0 ) ? PosInfo::CTproduct_1 : PosInfo::CTproduct_2; // PCS_1 or PCS_2 may be two belt work at the same time.
	cmd.iIoValue = iConveyorType;

	cmd.dyn.PathVel = belt_speed;  
	cmd.dyn.PathAcc = belt_acceleration;
	cmd.dyn.PathDec = belt_deceleration;
	cmd.dyn.PathJerk = belt_jerk;
	SMC.setCmdInfo(cmd);

	std::cout << " conveyor begin track " << endl;
}

void Conveyor::end_track()
{
	if ( ScriptFunc::is_command_ignored() )
	{
		return;
	}

	CmdInfo cmd;
	cmd.iCommandType = CMD_CONVEYOREND;
	cmd.iIoValue = iConveyorType;

	cmd.destPos.pos_type = ( iCoordType == 0 ) ? PosInfo::CTproduct_1 : PosInfo::CTproduct_2; // PCS_1 or PCS_2 may be two belt work at the same time.

	SMC.setCmdInfo(cmd);
	std::cout << " conveyor end track " << endl;
 
}

// whether use two tcbs according to object coordinate type, not conveyor coord type.
int Conveyor::wait(SensableObject* pObj)
{
	if ( ScriptFunc::is_command_ignored() )
	{
		return -1;
	}
	SensableObject* pSensorObj = pObj; 
 
	CmdInfo cmd;
	cmd.iCommandType = CMD_CONVEYORWAIT;
	cmd.iObjNo = pObj->iSensorType; // photoelectric or vision
	cmd.iIoValue = iConveyorType;
	cmd.auxPos.cart.pos.x = min_work_area;
	cmd.auxPos.cart.pos.y = max_work_area;
	cmd.startPos.cart = CartPos(xBeltOrg,yBeltOrg,zBeltOrg, aBeltOrg,bBeltOrg,cBeltOrg );
	cmd.destPos.pos_type = ( iCoordType == 0 ) ? PosInfo::CTproduct_1 : PosInfo::CTproduct_2; // PCS_1 or PCS_2
	if ( iCoordType != pObj->iCoordType )
	{
		pObj->iCoordType = iCoordType; // object's coord must keep same with conveyor
	}

	if ( pObj->iSensorType == 0 )
	{
  		;
	}
	else if ( pObj->iSensorType == 1 )
	{
 		int ret = ScriptFunc::WaitObjectWithVisionSensor(pObj); // wait when vision sensor detect object, trigger signal start track.
		if ( ret != 0 )
 			return -1;
   
	}

	cmd.destPos.cart = pObj->get_pos();
	set_obj_initpos( pObj->get_pos() );

	SMC.setCmdInfo(cmd);
	std::cout << " conveyor wait object = " <<  pObj->iSensorType << endl;
	int iCoordIndex = pObj->iCoordType; 
	ScriptFunc::ConveyorWaitObject(iCoordIndex); // wait when object go into work area.

	return 0;
}

void Conveyor::done(SensableObject* pObj, bool bTake)
{
	if ( ScriptFunc::is_command_ignored() )
	{
		return;
	}

	CmdInfo cmd;
	cmd.iCommandType = CMD_CONVEYORDONE;
	cmd.destPos.pos_type = ( iCoordType == 0 ) ? PosInfo::CTproduct_1 : PosInfo::CTproduct_2; // PCS_1 or PCS_2 may be two belt work at the same time.
	cmd.iIoValue = iConveyorType;

	ScriptFunc::ConveyorDone();
	SMC.setCmdInfo(cmd);
	
	std::cout << " conveyor done track = " << endl;
}

char* Conveyor::get_value( const char* name, char* strValue )
{
	check_value( name, false, strValue );  
	return 0;
}

void Conveyor::set_value( const char* name, char* strValue)
{
	check_value(name,true,strValue);
}

bool Conveyor::check_value( const char* name, const bool bset,  char* val )
{
	bool res = false;

	double* dfpos[] = { NULL, NULL, &belt_speed, &belt_acceleration, &belt_deceleration, &belt_jerk,
		                &min_work_area, &max_work_area, 
		                &xBeltOrg, &yBeltOrg, &zBeltOrg, &aBeltOrg, &bBeltOrg, &cBeltOrg,
					    &xObjInitPos, &yObjInitPos, &zObjInitPos, &aObjInitPos, &bObjInitPos, &cObjInitPos };

	int datasize = ARRAY_SIZE(dfpos);

	char prop[VAR_STR_LEN];
	for ( int i = 0; i < VARINFO->vardesc[svCONVEYOR]->getValuesTotal(); i++ )
	{
		if ( i >= datasize )
			break;

		VARINFO->vardesc[svCONVEYOR]->getValue(i+1,prop);
		if ( prop[0] != '\0' && strstr( prop, name ) )
		{
			if ( bset )
			{
				if ( dfpos[i] )
				{
					if ( strstr(prop,"ANGLE") )
						*dfpos[i] = deg2rad(atof( val ? val : 0 )) ;
					else
						*dfpos[i] = atof( val ? val : 0 );
 				}
				else
				{
					if( !val )
						;
					else 
					{
						if ( i == 0 )
						{
							iCoordType = atoi( val ? val : 0 );
						}
						else
							iConveyorType = atoi( val ? val : 0 );
					}
 				}
			}
			else
			{
				if ( dfpos[i] == NULL )
				{
					sprintf( val, "%d",  i == 0 ? iCoordType : iConveyorType  );
				}
				else
					sprintf( val, "%lf", *dfpos[i] );
			}
			res = true;
			break;

		}
	}

	if ( !bset && !res ) // read failed.
		*val = '\0';

	return res;

}

const CartRef Conveyor::refSysObject()
{
	if ( iCoordType == 0 )
	{
		sprintf(carRefObj.cartname,"%s","PCS_1");
 	}
	else if( iCoordType == 1 )
	{
		sprintf(carRefObj.cartname,"%s","PCS_2");
 	}
	return carRefObj;
}

void Conveyor::set_obj_initpos(const CartPos& objpos)
{
	xObjInitPos = objpos.pos.x;
	yObjInitPos = objpos.pos.y;
	zObjInitPos = objpos.pos.z;
	aObjInitPos = objpos.ang.x;
	bObjInitPos = objpos.ang.y;
	cObjInitPos = objpos.ang.z;
}

std::ostream& operator<< (std::ostream& os, const Conveyor& v)
{
	os << v.iCoordType << ' ' << v.iConveyorType << ' ' 
		<< v.belt_speed << ' ' << v.belt_acceleration << ' ' << v.belt_deceleration <<' ' << v.belt_jerk <<' '
		<< v.min_work_area << ' ' << v.max_work_area <<' '
	   << v.xBeltOrg << ' '<< v.yBeltOrg << ' '<< v.zBeltOrg << ' '<< v.aBeltOrg << ' '<< v.bBeltOrg << ' '<< v.cBeltOrg << ' '
	   << v.xObjInitPos << ' '<< v.yObjInitPos << ' '<< v.zObjInitPos << ' '<< v.aObjInitPos << ' '<< v.bObjInitPos << ' '<< v.cObjInitPos; 
	return os;
}

std::istream& operator>> (std::istream &in, Conveyor &v)
{
	in >> v.iCoordType >> v.iConveyorType >> v.belt_speed >> v.belt_acceleration >> v.belt_deceleration >> v.belt_jerk 
	   >> v.min_work_area >> v.max_work_area 
	   >> v.xBeltOrg  >> v.yBeltOrg >> v.zBeltOrg >> v.aBeltOrg >> v.bBeltOrg>> v.cBeltOrg 
	   >> v.xObjInitPos  >> v.yObjInitPos >> v.zObjInitPos >> v.aObjInitPos >> v.bObjInitPos>> v.cObjInitPos; 

	return in;
}