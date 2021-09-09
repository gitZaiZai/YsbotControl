// Copyright 2016, 亚思信息科技有限责任公司
// All right reserved.
//
// Author: Liu Run Dan ( lrdcz@163.com )
//
// Yasi Robot Motion Control System - 亚思机器人运动控制系统

#ifndef secur_area_h
#define secur_area_h

#include "../LuaScript/BasicScriptVariable.h"
#include "../PosInfo.h"

class SecurArea : public BasicScriptVariable
{
public:
	SecurArea ();
	SecurArea (const SecurArea& ) ;
	const SecurArea& operator= (const SecurArea& ) ;
	~SecurArea(void);

	int iShapeType;
	int iAreaType;
	
	int iActiveVar;
	int iStateVar;

	bool active;
	bool emgcOut;

	double xPos;
	double yPos;
	double zPos;
	double aPos;
	double bPos;
	double cPos;
	double shapeParam[3];
 

	char* get_value( const char* name, char* strValue);
	void  set_value( const char* name, char* strValue);
	bool  check_value( const char* name, const bool, char* );

	CartRef posRef;
 	const CartRef& get_ref_pos();
	void set_ref_pos(const CartRef&);

	bool IsPosInArea(const CartPos* tcp);
	bool IsRobPosInArea();

	void Activate() { active = true; }
	void Deactivate() { active = false; }
	void SetTransformation(double x, double y, double z);
	void SetBoxSize(double x, double y, double z);
	void SetCylinderSize(double r, double h);

	bool PosHasSpaceViolation(const CartPos* tcp) { return false; }
	void Connect(int nVar);
	void Disconnect();

   // accomplish in future by liurundan.
	void WaitRobInside();

};

std::ostream& operator<< (std::ostream& os, const SecurArea& v);
std::istream& operator>> (std::istream &in, SecurArea& v);


#endif