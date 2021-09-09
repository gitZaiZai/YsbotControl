// Copyright 2016, 亚思信息科技有限责任公司
// All right reserved.
//
// Author: Liu Run Dan ( lrdcz@163.com )
//
// Yasi Robot Motion Control System - 亚思机器人运动控制系统

#include "StdAfx.h"
#include "PosInfo.h"
#include "LuaScript/ScriptVariables.h"

/*****************************************************************************/
/********************** CLASS PosInfo    *************************************/
/*****************************************************************************/

// int PosInfo::axisCoordinate = 0;
// int PosInfo::machineCoordinate = 1;
// int PosInfo::programCoordinate = 2;
// int PosInfo::multipleCoordinate = 3;
 
PosInfo::PosInfo(CoordinateType pt) : cart(Vec3D::zero_vector,Vec3D::zero_vector),axis(0,0,0,0,0,0),vtrans(0,0,0),vrot(0,0,0),auxi(0,0,0,0,0,0)
{
 	valid = true; 
	pos_type = pt; 
}

const PosInfo& PosInfo::operator= (const PosInfo& rl)
{
	cart   = rl.cart;
	axis   = rl.axis;
	vtrans = rl.vtrans;
	vrot   = rl.vrot;
	auxi   = rl.auxi;
	 
	valid  = rl.valid;
	pos_type = rl.pos_type;

	return *this;
}

void PosInfo::set_axis_rad(double a1, double a2, double a3, double a4, double a5, double a6, double a7, double a8) throw ()
{
 	axis.set_rad(a1,a2,a3,a4,a5,a6);
	auxi.set_rad(a7,a8,0,0,0,0);
}

void PosInfo::set_axis_deg(double a1, double a2, double a3, double a4, double a5, double a6, double a7, double a8)throw ()
{
	axis.set_deg(a1,a2,a3,a4,a5,a6);
	auxi.set_deg(a7,a8,0,0,0,0);
}

void PosInfo::set_cart_pos(double dX, double dY, double dZ, double dA, double dB, double dC, int _fig, double a7/* =0 */, double a8/* =0 */ ) throw ()
{
 	cart.set_pos(dX,dY,dZ,dA,dB,dC,_fig);
	auxi.set_rad(a7,a8,0,0,0,0);
}

void PosInfo::set_axis( const AxisPos& _baxis,const AxisPos& _eaxis ) throw ()
{ 
	axis = _baxis;  
	auxi = _eaxis; 
}

void PosInfo::set_cart( const CartPos& _cart, const AxisPos& _eaxis ) throw ()
{ 
	cart = _cart;   
	auxi = _eaxis; 
}

char* PosInfo::get_value( const char* name, char* strValue )
{
  	bool ret = check_value( name, false, strValue ); 
	if ( !ret )
	{
		axis.get_value( name, strValue );
		if( strValue[ 0 ] == '\0' )
		{
			cart.get_value(name,strValue);
 		}
 	}
	return strValue;
}

void PosInfo::set_value( const char* name, char* strValue)
{
	bool ret = check_value(name,true,strValue);
	if ( !ret )
	{
		ret = axis.check_value( name, true, strValue );
		if ( !ret )
			cart.set_value( name, strValue );
	}
}

bool PosInfo::check_value( const char* name, const bool bset,  char* val )
{
	bool ret = false;

	double* ap[] = { &auxi.q2, &auxi.q1 };

	char prop[VAR_STR_LEN];
	int num = VARINFO->vardesc[svAXISPOS]->getValuesTotal();

	for ( int i = 0; i < ARRAY_SIZE(ap); i++ )
	{
		VARINFO->vardesc[svAXISPOS]->getValue(num-i,prop);
		if ( prop[0] != '\0' &&  strstr( prop, name ) ) 
  		{
			if ( bset )
			{
 				if ( strstr(prop,"ANGLE") )
					*ap[i] = deg2rad(atof( val ? val : 0 )) ;
				else
					*ap[i] = atof( val ? val : 0 );
			}
 			else
 				sprintf( val, "%lf", *ap[i] );
			ret = true;
			break;
 		}
	}
 
	if ( !bset && !ret )
 		*val = '\0';
 	
	return ret;
}

bool PosInfo::operator !=( const PosInfo &p )
{
	if ( pos_type != p.pos_type )
	{
		 return true;
	}
	else
	{
		if ( pos_type == CTaxis )
		{
			return ( (axis != p.axis) || (auxi != p.auxi) );
		}
		else
		{
			return (cart != p.cart);
		}
	}
}
 
bool PosInfo::operator ==( const PosInfo &p )
{
	if ( pos_type == p.pos_type )
	{
		if ( pos_type == CTaxis )
		{
			return ( (axis == p.axis) && (auxi == p.auxi) );
		}
		else
		{
			return (cart == p.cart);
		}
	}
    return false;
}

std::ostream& operator<< (std::ostream& os, const PosInfo& v) 
{
// 	static_cast<int>(v.pos_type)
	if ( v.pos_type == PosInfo::CTaxis )
	{
		os << v.axis.get_deg() << ' ' << v.auxi.get_deg() << ' ' << (v.valid ? "true" : "false" ) << ' ';
 	}
	else if( v.pos_type == PosInfo::CTmachine )
	{
		os << v.cart << ' ' << v.auxi.get_deg() << ' ' << (v.valid ? "true" : "false" ) << ' ';
 	}
 
	
	return os;
}

std::istream& operator>> (std::istream &in, PosInfo &v)
{
   	char valid[20];

	if ( v.pos_type == PosInfo::CTaxis )
	{
		in >> v.axis >> v.auxi >> valid;
		v.axis = v.axis * CONSTPI180;
		v.auxi = v.auxi * CONSTPI180;
	}
	else if ( v.pos_type == PosInfo::CTmachine )
	{
		in >> v.cart >> v.auxi >> valid;
		v.cart.ang = v.cart.ang * CONSTPI180;
		v.auxi     = v.auxi * CONSTPI180;
 	}
	if ( strcmp(valid,"true") == 0 )
	{
		v.valid = true;
	}
	else if ( strcmp(valid,"false") == 0 )
	{
		v.valid = false;
	}	
	return in;
}

const PosInfo& LuaAPOS::operator() ()
{
	return *this;
}

const PosInfo& LuaCPOS::operator() ()
{
	return *this;
}

