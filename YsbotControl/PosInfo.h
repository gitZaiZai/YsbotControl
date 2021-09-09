// Copyright 2016, 亚思信息科技有限责任公司
// All right reserved.
//
// Author: Liu Run Dan ( lrdcz@163.com )
//
// Yasi Robot Motion Control System - 亚思机器人运动控制系统

#ifndef _pos_info_h
#define _pos_info_h

#include "AxisPos.h"
#include "CartPos.h"
 
enum LocationDataSource {
	LDScommad = 0, 
	LDSservoCom, 
	LDSencodeFeedback,  
};
 
class PosInfo : public BasicScriptVariable
{
public:
	enum CoordinateType 
	{ 
		CTaxis = 0,               
		CTmachine,
		CTworld,
		CTproduct_1,
		CTproduct_2,
		CTtool
	};

	CartPos cart;                  
	AxisPos axis;            
	
	Vec3D   vtrans;
	Vec3D   vrot;

// 	double da1 ; // Position of additional axis P
// 	double da2 ;
// 	double da3 ;
// 	double da4 ;
// 	double da5 ;
// 	double da6 ;

	AxisPos auxi;

 	bool valid;             
   	CoordinateType  pos_type; // axis:0, cart:1, all:3

	explicit PosInfo (CoordinateType pt=CTaxis);
	PosInfo (const PosInfo& rl) { operator= (rl); }
	const PosInfo& operator= (const PosInfo& rl);
 	 
	void set_axis_rad (double, double, double,double, double, double,double =0, double =0) throw ();
	void set_axis_deg (double, double, double,double, double, double,double =0, double =0) throw ();
	void set_cart_pos (double dX, double dY, double dZ, double dA, double dB, double dC, int _fig, double =0, double =0 ) throw ();

	void set_axis( const AxisPos& _baxis,const AxisPos& _eaxis ) throw ();
	void set_cart( const CartPos& _cart, const AxisPos& _eaxis ) throw ();

	char* get_value( const char* name, char* strValue);
	void  set_value( const char* name, char* strValue);
	bool  check_value( const char* name, const bool, char* );

	bool               operator !=            ( const PosInfo &p           );
 	bool               operator ==            ( const PosInfo &p           );
 
	// print methods
	friend std::ostream& operator<< (std::ostream& os, const PosInfo& v);
	friend std::istream& operator>> (std::istream &in, PosInfo& v);
	friend std::istream& operator>> (std::istream &in, PosInfo& v);

// 	static int axisCoordinate;      
// 	static int machineCoordinate;        
// 	static int programCoordinate;        
// 	static int multipleCoordinate;  
 };

 
class LuaAPOS : public PosInfo
{
public:
	LuaAPOS() : PosInfo (CTaxis) {;}
	LuaAPOS( const PosInfo& rl ) 
	{ 
		cart   = rl.cart;
		axis   = rl.axis;
		vtrans = rl.vtrans;
		vrot   = rl.vrot;
		auxi   = rl.auxi;

		valid  = rl.valid;
		pos_type = rl.pos_type; 
	}

	LuaAPOS(double a1, double a2, double a3,double a4, double a5, double a6,double a7=0, double a8=0) : PosInfo (CTaxis) 
	{ 
		set_axis_deg(a1,a2,a3,a4,a5,a6,a7,a8);
	}
	~LuaAPOS() {;}

	const PosInfo& operator() ();
};

class LuaCPOS : public PosInfo
{
public:
	LuaCPOS() : PosInfo (CTmachine) {;}
	LuaCPOS( const PosInfo& rl ) 
	{ 
		cart   = rl.cart;
		axis   = rl.axis;
		vtrans = rl.vtrans;
		vrot   = rl.vrot;
		auxi   = rl.auxi;

		valid  = rl.valid;
		pos_type = rl.pos_type; 
	}
	LuaCPOS(double dX, double dY, double dZ, double dA, double dB, double dC, int _fig = -1,double a7=0, double a8=0 ) : PosInfo (CTmachine) 
	{
		set_cart_pos(dX,dY,dZ,dA,dB,dC,_fig,a7,a8);
	}
	~LuaCPOS() {;}

	const PosInfo& operator() ();

};

#endif
