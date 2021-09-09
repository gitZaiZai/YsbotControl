#ifndef _dhpara_base_h
#define _dhpara_base_h

namespace Ysbot
{
struct DHParaBase 
{
	double a; // rad
 	double L; 
 	double d;
	bool   modifiedDH;
	DHParaBase () : a(0),L(0), d(0), modifiedDH(false)	{;}
	DHParaBase (const DHParaBase& dh) { operator= (dh); }
	const DHParaBase& operator= (const DHParaBase& dh) 
	{
		a = dh.a;
		L = dh.L;
		d = dh.d;
		modifiedDH = dh.modifiedDH;
		return *this;
	}
};
 
struct DH6dofParam
{
	double  a1;
	double  a2;
	double  a3;
	double  d3;
	double  d4;
	double  d6;
	double  origin_offset_z;
} ;

struct DHExParam
{
	double J34_CoupleRatio;
	double J45_CoupleRatio;
	double J56_CoupleRatio;
	double J46_CoupleRatio;
	struct TOOL
	{
		double  X_Offset;
		double  Y_Offset;
		double  Z_Offset;
		double  RX_Offset;
		double  RY_Offset;
		double  RZ_Offset;
	}Tool;
	struct WORK
	{
		double  X_Offset;
		double  Y_Offset;
		double  Z_Offset;
		double  RX_Offset;
		double  RY_Offset;
		double  RZ_Offset;
	}Work;
} ;

struct TeachPoint
{
	double  x; // mm
	double  y; // mm
	double  z; // mm

	double  rx; // rad
	double  ry; // rad
	double  rz; // rad
} ;

}

#endif
