// Copyright 2016, 亚思信息科技有限责任公司
// All right reserved.
//
// Author: Liu Run Dan ( lrdcz@163.com )
//
// Yasi Robot Motion Control System - 亚思机器人运动控制系统

#include "StdAfx.h"
#include "SensableObject.h"
#include "LuaScript/ScriptVariables.h"
#include "MotionControl.h"
#include "LuaScript/ScriptFunc.h"

SensableObject::SensableObject()  
{
	iSensorType = 0; // vision sensor or  photoelectric sensor, radar sensors or laser sensors  
	iCoordType  = 1;
	simulate_obj = false;

 	xObjectPos = 0;
	yObjectPos = 0;
	zObjectPos = 0;
	aObjectPos = 0;
	bObjectPos = 0;
	cObjectPos = 0;

	width = 0;
	height = 10;

	sprintf(carRefObj.selfname,"%s","refSysObj");
	sprintf(carRefObj.cartname,"%s","PCS_2");
}


SensableObject::~SensableObject(void)
{
}
 
SensableObject::SensableObject (const SensableObject& src) 
{ 
	operator= (src); 
}

const SensableObject& SensableObject::operator= (const SensableObject& src) 
{
	this->iCoordType  = src.iCoordType;
	this->iSensorType = src.iSensorType;
    this->simulate_obj = src.simulate_obj;

	this->xObjectPos = src.xObjectPos;
	this->yObjectPos = src.yObjectPos;
	this->zObjectPos = src.zObjectPos;
	this->aObjectPos = src.aObjectPos;
	this->bObjectPos = src.bObjectPos;
	this->cObjectPos = src.cObjectPos;

	this->width = src.width;
	this->height = src.height;

	this->carRefObj = src.carRefObj;

	return *this;
}
 
char* SensableObject::get_value( const char* name, char* strValue )
{
	check_value( name, false, strValue );  
	return 0;
}

void SensableObject::set_value( const char* name, char* strValue)
{
	check_value(name,true,strValue);
}

bool SensableObject::check_value( const char* name, const bool bset,  char* val )
{
	bool res = false;

	double* dfpos[] = { NULL, NULL,	NULL, &xObjectPos, &yObjectPos, &zObjectPos, &aObjectPos, &bObjectPos, &cObjectPos, &width, &height };

	int datasize = ARRAY_SIZE(dfpos);

	char prop[VAR_STR_LEN];
	for ( int i = 0; i < VARINFO->vardesc[svSensableObject]->getValuesTotal(); i++ )
	{
		if ( i >= datasize )
			break;

		VARINFO->vardesc[svSensableObject]->getValue(i+1,prop);
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
							iCoordType = atoi(  val ? val : 0 );
						}
						else if (i == 1)
						{
							iSensorType = atoi( val ? val : 0 );
 						}
						else
						{
							bool b = false;
							if( strcmp( val, "on"  ) == 0 ||
								strcmp( val, "true") == 0 ||
								strcmp( val, "yes" ) == 0 )
								b = true;
							simulate_obj = ( b == true ) ? true : false;
						}
					}
				}
			}
			else
			{
				if ( dfpos[i] == NULL )
				{
					if ( i == 0 )
					{
						sprintf( val, "%d", iCoordType );
					}
					else if ( i == 1 )
					{
						sprintf( val, "%d", iSensorType  );

					}
					else if ( i == 2 )
					{
						sprintf( val, "%d", simulate_obj  );
 					}
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

const CartRef SensableObject::refSys()
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

void SensableObject::set_pos(const CartPos& objpos)
{
	xObjectPos = objpos.pos.x;
	yObjectPos = objpos.pos.y;
	zObjectPos = objpos.pos.z;
	aObjectPos = objpos.ang.x;
	bObjectPos = objpos.ang.y;
	cObjectPos = objpos.ang.z;
}

CartPos SensableObject::get_pos()
{
	return CartPos(xObjectPos,yObjectPos,zObjectPos,aObjectPos,bObjectPos,cObjectPos,5);
}

std::ostream& operator<< (std::ostream& os, const SensableObject& v)
{
	os << v.iCoordType << ' ' << v.iSensorType << ' ' << (int)v.simulate_obj << ' '
 	   << v.xObjectPos << ' '<< v.yObjectPos << ' '<< v.zObjectPos << ' '<< v.aObjectPos << ' '<< v.bObjectPos << ' '<< v.cObjectPos << ' '
	   << v.width << ' '<< v.height ; 
	return os;
}

std::istream& operator>> (std::istream &in, SensableObject &v)
{
	in >> v.iCoordType >> v.iSensorType >> v.simulate_obj >> v.xObjectPos  >> v.yObjectPos >> v.zObjectPos >> v.aObjectPos >> v.bObjectPos >> v.cObjectPos 
		>> v.width >> v.height ; 

	return in;
}