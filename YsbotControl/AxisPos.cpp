// Copyright 2016, 亚思信息科技有限责任公司
// All right reserved.
//
// Author: Liu Run Dan ( lrdcz@163.com )
//
// Yasi Robot Motion Control System - 亚思机器人运动控制系统

#include "stdafx.h"
#include "AxisPos.h"
#include "GroupProperties.h"
#include "LuaScript/ScriptVariables.h"


/*****************************************************************************/
/********************** CLASS AxisPos    *************************************/
/*****************************************************************************/

AxisPos::AxisPos(const ColumnVector& cv, const int rows)
{
	double* ap[] = { &q1, &q2, &q3, &q4, &q5, &q6 };

	for ( int i = 0; i < cv.Nrows(); i++ )
		*(ap[i]) = cv(i+1);
}

const AxisPos AxisPos::get_rad() const throw ()
{
	return *this;
}

const AxisPos AxisPos::get_deg() const throw () 
{
	return AxisPos( q1*CONST180PI, q2*CONST180PI, q3*CONST180PI, q4*CONST180PI, q5*CONST180PI, q6*CONST180PI );
}

char* AxisPos::get_value( const char* name, char* strValue )
{
	check_value( name, false, strValue ); 
	return strValue;
}

void AxisPos::set_value( const char* name, char* strValue)
{
	check_value(name,true,strValue);
}

bool AxisPos::check_value( const char* name, const bool bset, char* val )
{
	bool ret = false;

	double* dfpos[] = { &q1, &q2, &q3, &q4, &q5, &q6 };
	int datasize = ARRAY_SIZE(dfpos);

	char prop[VAR_STR_LEN];
	for ( int i = 0; i < VARINFO->vardesc[svAXISPOS]->getValuesTotal(); i++ )
	{
		if ( i >= datasize )
			break;
		VARINFO->vardesc[svAXISPOS]->getValue(i+1,prop);
		if ( prop[0] != '\0' &&  strstr( prop, name ) ) 
 		{
			if ( bset )
			{
				if ( strstr(prop,"ANGLE") )
 					*dfpos[i] = deg2rad(atof( val ? val : 0 )) ;
 				else
					*dfpos[i] = atof( val ? val : 0 );
 			}
			else
			{
 				sprintf( val, "%lf", *dfpos[i] );
			}
			ret = true;
			break;
		}
	}

	if ( !bset && !ret )
		*val = '\0';

	return ret;
}

 
ReturnMatrix AxisPos::toColumnVector()
{
	ColumnVector tor(MAX_JOINT_NUM);
	tor = 0;
	tor(1) = this->q1;
	tor(2) = this->q2;
	tor(3) = this->q3;
	tor(4) = this->q4;
	tor(5) = this->q5;
	tor(6) = this->q6;

	return tor; 
}

void AxisPos::set_rad(double a1, double a2, double a3, double a4, double a5, double a6)
{
	q1 = a1;
	q2 = a2;
	q3 = a3;
	q4 = a4;
	q5 = a5;
	q6 = a6;
}

void AxisPos::set_deg (double a1, double a2, double a3, double a4, double a5, double a6) throw () 
{
	set_rad ( a1*CONSTPI180, a2*CONSTPI180, a3*CONSTPI180, a4*CONSTPI180, a5*CONSTPI180, a6*CONSTPI180 );
}

bool AxisPos::operator== (const AxisPos v) const throw () 
{
	return (q1==v.q1) && (q2==v.q2) && (q3==v.q3) && (q4==v.q4) && (q5==v.q5) && (q6==v.q6);
}

bool AxisPos::operator!= (const AxisPos v) const throw () 
{
	return (q1!=v.q1) || (q2!=v.q2) || (q3!=v.q3) || (q4!=v.q4) || (q5!=v.q5) || (q6!=v.q6);
}

AxisPos AxisPos::operator+ (const AxisPos v) const throw () 
{
	AxisPos res (*this);
	res+=v;
	return res;
}

const AxisPos& AxisPos::operator+= (const AxisPos v) throw () 
{
	q1+=v.q1;
	q2+=v.q2;
	q3+=v.q3;
	q4+=v.q4;
	q5+=v.q5;
	q6+=v.q6;
	return *this;
}

AxisPos AxisPos::operator- (const AxisPos v) const throw () 
{
	AxisPos res (*this);
	res-=v;
	return res;
}

const AxisPos& AxisPos::operator-= (const AxisPos v) throw () 
{
	q1-=v.q1;
	q2-=v.q2;
	q3-=v.q3;
	q4-=v.q4;
	q5-=v.q5;
	q6-=v.q6;
	return *this;
}

AxisPos AxisPos::operator- () const throw () 
{
	return AxisPos (-q1, -q2,-q3, -q4,-q5, -q6);
}

const AxisPos& AxisPos::operator*= (double s) throw () 
{
	q1*=s;
	q2*=s;
	q3*=s;
	q4*=s;
	q5*=s;
	q6*=s;
	return *this;
}

const AxisPos& AxisPos::operator/= (double s) throw () 
{
	q1/=s;
	q2/=s;
	q3/=s;
	q4/=s;
	q5/=s;
	q6/=s;

	return *this;
}


AxisPos operator* (AxisPos v, double s) throw () 
{
	AxisPos res (v);
	res *= s;
	return res;
}

AxisPos operator* (double s, AxisPos v) throw () 
{
	AxisPos res (v);
	res *= s;
	return res;
}

AxisPos operator/ (AxisPos v, double s) throw () 
{
	AxisPos res(v);
	res /= s;
	return res;
}

std::ostream& operator<< (std::ostream& os, const AxisPos& v)
{
	os <<' '<< v.q1 << ' ' << v.q2<<' '<< v.q3 << ' ' << v.q4<<' '<< v.q5 << ' ' << v.q6<<' ';
	return os;
}

std::istream& operator>> (std::istream &in, AxisPos &v)
{
	in >> v.q1 >> v.q2 >> v.q3 >> v.q4 >> v.q5 >> v.q6;
	return in;
}
