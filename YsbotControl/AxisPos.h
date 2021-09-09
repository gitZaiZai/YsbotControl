// Copyright 2016, 亚思信息科技有限责任公司
// All right reserved.
//
// Author: Liu Run Dan ( lrdcz@163.com )
//
// Yasi Robot Motion Control System - 亚思机器人运动控制系统

#ifndef AxisPos_h
#define AxisPos_h

#include "Angle.h"
#include "LuaScript/BasicScriptVariable.h"

class AxisPos : public BasicScriptVariable 
{
public:
	double q1;
	double q2;
	double q3;
	double q4;
	double q5;
	double q6;

	AxisPos () : q1(0), q2(0), q3(0), q4(0), q5(0), q6(0) {;}
	AxisPos (double dX, double dY, double dZ, double dA, double dB, double dC ) : q1(dX), q2(dY), q3(dZ), q4(dA),q5(dB), q6(dC) {;}
	AxisPos (const AxisPos& rl) { operator= (rl); }
	AxisPos (const ColumnVector& cv, const int rows = 6 );
	const AxisPos& operator= (const AxisPos& rl) 
	{
		q1=rl.q1;
		q2=rl.q2;
		q3=rl.q3;
		q4=rl.q4;
		q5=rl.q5;
		q6=rl.q6;
		return *this;
	}
	const AxisPos get_rad() const throw ();
	const AxisPos get_deg() const throw ();
	void set_rad (double, double, double,double, double, double) throw ();
	void set_deg (double, double, double,double, double, double) throw ();
 
	char* get_value( const char* name, char* strValue);
	void  set_value( const char* name, char* strValue);
	bool  check_value( const char* name, const bool, char* );

	ReturnMatrix  toColumnVector();

	bool operator== (const AxisPos) const throw ();
	bool operator!= (const AxisPos) const throw ();

	AxisPos operator+ (const AxisPos) const throw ();
	const AxisPos& operator+= (const AxisPos) throw ();
	AxisPos operator- (const AxisPos) const throw ();
	const AxisPos& operator-= (const AxisPos) throw ();
	AxisPos operator- () const throw ();
	const AxisPos& operator*= (double) throw ();
	const AxisPos& operator/= (double) throw ();    
	double operator* (const AxisPos) const throw ();

};

AxisPos operator* (AxisPos, double) throw ();
AxisPos operator* (double, AxisPos) throw ();
AxisPos operator/ (AxisPos, double) throw ();   

std::ostream& operator<< (std::ostream& os, const AxisPos& v);
std::istream& operator>> (std::istream &in, AxisPos& v);

#endif