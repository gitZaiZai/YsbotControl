#include "StdAfx.h"
#include "SecurArea.h"
#include "../LuaScript/ScriptVariables.h"
#include "../MotionControl.h"
#include "../LuaScript/ScriptFunc.h"


SecurArea::SecurArea()  
{
	iShapeType = 0;    
	iAreaType  = 0;
	iActiveVar = 0;
	iStateVar  = 0;

	active  = false;
	emgcOut = false;

	xPos = yPos = zPos = 0;
	aPos = bPos = cPos = 0;

	for ( int i = 0; i < 3; i++ )
 		shapeParam[i] = 0.0;
 
	sprintf(posRef.selfname,"%s","area0");
	sprintf(posRef.cartname,"%s","Base");
}


SecurArea::~SecurArea(void)
{
}

SecurArea::SecurArea (const SecurArea& src) 
{ 
	operator= (src); 
}

const SecurArea& SecurArea::operator= (const SecurArea& src) 
{
	this->iShapeType = src.iShapeType;
	this->iAreaType  = src.iAreaType;

	this->iActiveVar = src.iActiveVar;
 	this->iStateVar  = src.iStateVar;
	this->active  = src.active; 
	this->emgcOut =	src.emgcOut;
	
	this->xPos = src.xPos;
	this->yPos = src.yPos;
	this->zPos = src.zPos;
	this->aPos = src.aPos;
 	this->bPos = src.bPos;
	this->cPos = src.cPos;

	this->posRef = src.posRef;

	for ( int i = 0; i < 3; i++ )
		this->shapeParam[i] = src.shapeParam[i];

	return *this;
}

char* SecurArea::get_value( const char* name, char* strValue )
{
	check_value( name, false, strValue );  
	return 0;
}

void SecurArea::set_value( const char* name, char* strValue)
{
	check_value(name,true,strValue);
}

bool SecurArea::check_value( const char* name, const bool bset,  char* val )
{
	bool res = false;

	double* dfData[] = { NULL, NULL, NULL, NULL, NULL, NULL, 
		&xPos, &yPos, &zPos, &aPos, &bPos, &cPos, 
		&shapeParam[0], &shapeParam[1],&shapeParam[2] };

	int datasize = ARRAY_SIZE(dfData);

	char prop[VAR_STR_LEN];
	for ( int i = 0; i < VARINFO->vardesc[svSecurArea]->getValuesTotal(); i++ )
	{
		if ( i >= datasize )
			break;

		VARINFO->vardesc[svSecurArea]->getValue(i+1,prop);
		if ( prop[0] != '\0' && strstr( prop, name ) )
		{
			if ( bset )
			{
				if ( dfData[i] )
				{
					if ( strstr(prop,"ANGLE") )
						*dfData[i] = deg2rad(atof( val ? val : 0 )) ;
					else
						*dfData[i] = atof( val ? val : 0 );
				}
				else
				{
					if( !val )
						;
					else 
					{
						if ( i == 0 )
						{
							iShapeType = atoi(  val ? val : 0 );
						}
						else if (i == 1)
						{
							iAreaType = atoi( val ? val : 0 );
						}
						else if (i == 2)
						{
							iActiveVar = atoi( val ? val : 0 );
						}
						else if (i == 3)
						{
							iStateVar = atoi( val ? val : 0 );
						}
 						else
						{
							bool b = false;
							if( strcmp( val, "on"  ) == 0 ||
								strcmp( val, "true") == 0 ||
								strcmp( val, "yes" ) == 0 )
								b = true;
							if ( i == 4 )
 								active = ( b == true ) ? true : false;
 							else 
								emgcOut = ( b == true ) ? true : false;
  						}
					}
				}
			}
			else
			{
				if ( dfData[i] == NULL )
				{
					if ( i == 0 )
					{
						sprintf( val, "%d", iShapeType );
					}
					else if ( i == 1 )
					{
						sprintf( val, "%d", iAreaType  );

					}
					else if ( i == 2 )
					{
						sprintf( val, "%d", iActiveVar  );
					}
					else if ( i == 3 )
					{
						sprintf( val, "%d", iStateVar  );
 					}
					else if ( i == 4 )
					{
						sprintf( val, "%d", active  );
					}
					else 
					{
						sprintf( val, "%d", emgcOut  );
					}	
				}
				else
					sprintf( val, "%lf", *dfData[i] );
			}
			res = true;
			break;

		}
	}

	if ( !bset && !res ) // read failed.
		*val = '\0';

	return res;

}
 
void SecurArea::set_ref_pos(const CartRef& objpos)
{
	posRef = objpos;
	xPos = posRef.cpos.pos.x;
	yPos = posRef.cpos.pos.y;
	zPos = posRef.cpos.pos.z;
	aPos = rad2deg( posRef.cpos.ang.x);
	bPos = rad2deg( posRef.cpos.ang.y);
	cPos = rad2deg( posRef.cpos.ang.z);
}

const CartRef& SecurArea::get_ref_pos()
{
	posRef.cpos = CartPos(xPos,yPos,zPos,deg2rad(aPos),deg2rad(bPos),deg2rad(cPos));
	return posRef;
}

bool SecurArea::IsPosInArea(const CartPos* tcp)
{
 	if ( iShapeType == 0 ) // box
	{
		if ( xPos <= tcp->pos.x && yPos <= tcp->pos.y && zPos <= tcp->pos.z &&
			 tcp->pos.x <= xPos + shapeParam[0] && 
			 tcp->pos.y <= yPos + shapeParam[1] && 
			 tcp->pos.z <= zPos + shapeParam[2] )
			return true;
		else
			return false;
	}
	else // cylinder
	{
		Vec center(xPos,yPos);
		Vec p(tcp->pos.x,tcp->pos.y);
		Vec d = p-center;
		if ( d.squared_length() <= shapeParam[0]*shapeParam[0] &&
			 zPos <= tcp->pos.z && tcp->pos.z <= zPos + shapeParam[1] )
			return true;
		else
			return false;
	}
	return false;
}

bool SecurArea::IsRobPosInArea()
{
	CartPos rpos = SMC.get_position( PosInfo::CTmachine, LDScommad ).cart;
	return IsPosInArea(&rpos);
}

void SecurArea::Connect(int nVar)
{
	iActiveVar = nVar;
}

void SecurArea::Disconnect()
{
	iActiveVar = 0;
}

void SecurArea::SetTransformation(double x, double y, double z )
{
	xPos = x;
	yPos = y;
	zPos = z;
}

void SecurArea::SetBoxSize(double x, double y, double z)
{
	shapeParam[0] = x;
	shapeParam[1] = y;
	shapeParam[2] = z;
}

void SecurArea::SetCylinderSize(double r, double h)
{
	shapeParam[0] = r;
	shapeParam[1] = h;
}

// accomplish in future.
void SecurArea::WaitRobInside()
{
	;
}

std::ostream& operator<< (std::ostream& os, const SecurArea& v)
{
	os << v.iShapeType << ' ' << v.iAreaType << ' ' << v.iActiveVar << ' ' << v.iStateVar << ' ' << (int)v.active << ' ' << (int)v.emgcOut << ' '
	   << v.xPos << ' '<< v.yPos << ' '<< v.zPos << ' '<< v.aPos << ' '<< v.bPos << ' '<< v.cPos << ' '
	   << v.shapeParam[0] << ' ' << v.shapeParam[1] << ' ' << v.shapeParam[2] ; 
	return os;
}

std::istream& operator>> (std::istream &in, SecurArea &v)
{
	in >> v.iShapeType >> v.iAreaType >> v.iActiveVar >> v.iStateVar >> v.active >> v.emgcOut 
		>> v.xPos  >> v.yPos >> v.zPos >> v.aPos >> v.bPos >> v.cPos 
		>> v.shapeParam[0] >> v.shapeParam[1] >> v.shapeParam[2] ; 

	return in;
}