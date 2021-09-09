// Copyright 2016, 亚思信息科技有限责任公司
// All right reserved.
//
// Author: Liu Run Dan ( lrdcz@163.com )
//
// Yasi Robot Motion Control System - 亚思机器人运动控制系统

#include "StdAfx.h"
#include "LuaScript.h"
#include "ScriptVariables.h"
#include "ScriptFunc.h"
#include "BinaryData.h"
#include "../CmdInfo.h"
#include "../DigitalInput.h"
#include "../DigitalOutput.h"
#include "../Conveyor.h"
#include "../SensableObject.h"
#include "../UIDialogs/SecurArea.h"
#include <luabind/luabind.hpp>
#include <luabind/adopt_policy.hpp>
#include <luabind/operator.hpp>
#include "ScriptVariableHelper.h"
#include "ScriptVariableType.h"

using namespace std;
using namespace luabind;
using namespace Ysbot;

luabind::scope registerLuaInteger(void);
luabind::scope registerLuaBool(void);
luabind::scope registerLuaReal(void);
luabind::scope registerLuaString(void);
luabind::scope registerLuaAngle(void);
luabind::scope registerLuaAPOS( const std::vector<std::string>& );
luabind::scope registerLuaCPOS( const std::vector<std::string>&, const std::vector<std::string>& );

LuaScript* LuaScript::singleton = NULL;

LuaScript* LuaScript::getInstance()
{
	if (singleton == NULL) 
	{
		singleton = new LuaScript();
		static LuaCleaner cl;
	}
	return singleton;
}

int globalVal = 99;

LuaScript::LuaScript(void)
{
	m_luaState = LuaState::New ();

	registerClassToLua();
	addFunctionToState();

	// 注册钩子
	m_luaState->doString( BinaryData::hook );
 
	command_info = new ScriptCommandInfo();

	TRACE(_T("Constuct LuaScript!\n")); 
}
 
LuaScript::~LuaScript(void)
{
	if (singleton == this ) 
	{
		singleton = NULL;
	}	
	 
	if ( m_luaState )
	{
		delete m_luaState;
		m_luaState = NULL;
	}
	delete command_info;

	TRACE(_T("destructor LuaScript!\n")); 
 }
 
lua_State* LuaScript::getState()
{
	return m_luaState->getState();
}

void LuaScript::debug_local_var()
{
	lua_getglobal(m_luaState->getState(),"printlocal");
	lua_pushstring(m_luaState->getState(),"c");

	if( lua_pcall(m_luaState->getState(), 1, 0, 0) != 0 )
	{
		const char *error = lua_tostring(m_luaState->getState(),-1);
		printf("%s\n",error);
		lua_pop(m_luaState->getState(),1);
	}
}

void LuaScript::registerClassToLua()
{
	std::vector<std::string> apos_prop_list;
    VARINFO->get_prop( svAXISPOS, apos_prop_list );

	std::vector<std::string> cart_prop_list;
	VARINFO->get_prop( svCARTPOS, cart_prop_list );

	std::vector<std::string> din_prop_list;
	VARINFO->get_prop( svDIGITALINPUT, din_prop_list );

	std::vector<std::string> dout_prop_list;
	VARINFO->get_prop( svDIGITALOUTPUT, dout_prop_list );

	std::vector<std::string> dyn_prop_list;
	VARINFO->get_prop( svDYNAMIC, dyn_prop_list );

	std::vector<std::string> ovl_prop_list;
	VARINFO->get_prop( svOVERLAP, ovl_prop_list );

	std::vector<std::string> conveyor_prop_list;
	VARINFO->get_prop( svCONVEYOR, conveyor_prop_list );

	std::vector<std::string> sensableobject_prop_list;
	VARINFO->get_prop( svSensableObject, sensableobject_prop_list );

	std::vector<std::string> area_prop_list;
	VARINFO->get_prop( svSecurArea, area_prop_list );

 	module(m_luaState->getState())
	[
 		class_<AxisPos>( "AxisPos" )
		.def(constructor<>())
		.def(constructor<double,double,double,double,double,double>())
		.def_readwrite ( apos_prop_list[0].c_str(), &AxisPos::q1) // a1
		.def_readwrite ( apos_prop_list[1].c_str(), &AxisPos::q2) // a2
		.def_readwrite ( apos_prop_list[2].c_str(), &AxisPos::q3) // a3
		.def_readwrite ( apos_prop_list[3].c_str(), &AxisPos::q4) // a4
		.def_readwrite ( apos_prop_list[4].c_str(), &AxisPos::q5)
		.def_readwrite ( apos_prop_list[5].c_str(), &AxisPos::q6)
		.def ("SetRad", &AxisPos::set_rad)
		.def ("SetDeg", &AxisPos::set_deg),

		class_<Vec3D>("Vec3D")
		.def(constructor<>())
		.def_readwrite ("x", &Vec3D::x)
		.def_readwrite ("y", &Vec3D::y)
		.def_readwrite ("z", &Vec3D::z),

		class_<CartPos> ("CartPos")
		.def(constructor<>())
		.def(constructor<double,double,double,double,double,double,int>())
		.def_readwrite ("pos", &CartPos::pos)
		.def_readwrite ("ang", &CartPos::ang)
		.def_readwrite ("fig", &CartPos::fig)
		.def ("SetPos", &CartPos::set_pos)
		.property (  cart_prop_list[0].c_str(), &CartPosHelper::get<0>, &CartPosHelper::set<0> ) // CPOS : x
		.property (  cart_prop_list[1].c_str(), &CartPosHelper::get<1>, &CartPosHelper::set<1> ) // y
		.property (  cart_prop_list[2].c_str(), &CartPosHelper::get<2>, &CartPosHelper::set<2> ) // z
		.property (  cart_prop_list[3].c_str(), &CartPosHelper::get<3>, &CartPosHelper::set<3> ) // A
		.property (  cart_prop_list[4].c_str(), &CartPosHelper::get<4>, &CartPosHelper::set<4> ) // B
		.property (  cart_prop_list[5].c_str(), &CartPosHelper::get<5>, &CartPosHelper::set<5> ), // C

		class_<CartRef> ("CartRef") // tool and work frame data.
		.def(constructor<>())
		.def_readwrite ("cpos", &CartRef::cpos)
        .property("cartname", &CartRef::getName, &CartRef::setName)
		.property (  cart_prop_list[0].c_str(), &CartRefHelper::get<0>, &CartRefHelper::set<0> ) // CPOS : x
		.property (  cart_prop_list[1].c_str(), &CartRefHelper::get<1>, &CartRefHelper::set<1> ) // y
		.property (  cart_prop_list[2].c_str(), &CartRefHelper::get<2>, &CartRefHelper::set<2> ) // z
		.property (  cart_prop_list[3].c_str(), &CartRefHelper::get<3>, &CartRefHelper::set<3> ) // A
		.property (  cart_prop_list[4].c_str(), &CartRefHelper::get<4>, &CartRefHelper::set<4> ) // B
		.property (  cart_prop_list[5].c_str(), &CartRefHelper::get<5>, &CartRefHelper::set<5> ), // C

		class_<PosInfo> ("PosInfo")
		.def(constructor<PosInfo::CoordinateType>())
		.enum_("CoordinateType")
		[
			value("CTaxis",0),
			value("CTmachine",1),
			value("CTworld",2),
			value("CTproduct_1",3),
			value("CTproduct_2",4),
			value("CTtool",5)
		]
		.def_readwrite ("cart",     &PosInfo::cart )
		.def_readwrite ("axis",     &PosInfo::axis )
		.def_readwrite ("vtrans",   &PosInfo::vtrans )
		.def_readwrite ("vrot",     &PosInfo::vrot   )
		.def_readwrite ("auxi",     &PosInfo::auxi   )
		.def_readwrite ("valid",    &PosInfo::valid  )
		.def_readwrite ("posType",  &PosInfo::pos_type)
		.def ("SetAxis", &PosInfo::set_axis)
		.def ("SetCart", &PosInfo::set_cart)
		.property (  apos_prop_list[0].c_str(), &PosInfoHelper::get<0>, &PosInfoHelper::set<0> ) // a1
		.property (  apos_prop_list[1].c_str(), &PosInfoHelper::get<1>, &PosInfoHelper::set<1> ) // a2
		.property (  apos_prop_list[2].c_str(), &PosInfoHelper::get<2>, &PosInfoHelper::set<2> ) // a3
		.property (  apos_prop_list[3].c_str(), &PosInfoHelper::get<3>, &PosInfoHelper::set<3> ) // a4
		.property (  apos_prop_list[4].c_str(), &PosInfoHelper::get<4>, &PosInfoHelper::set<4> )
		.property (  apos_prop_list[5].c_str(), &PosInfoHelper::get<5>, &PosInfoHelper::set<5> )
		.property (  apos_prop_list[6].c_str(), &PosInfoHelper::get<7>, &PosInfoHelper::set<7> ) // a7
		.property (  apos_prop_list[7].c_str(), &PosInfoHelper::get<8>, &PosInfoHelper::set<8> ) // a8 

		.property (  cart_prop_list[0].c_str(), &PosInfoHelper::get<0>, &PosInfoHelper::set<0> ) // CPOS : x
		.property (  cart_prop_list[1].c_str(), &PosInfoHelper::get<1>, &PosInfoHelper::set<1> ) // y
		.property (  cart_prop_list[2].c_str(), &PosInfoHelper::get<2>, &PosInfoHelper::set<2> ) // z
		.property (  cart_prop_list[3].c_str(), &PosInfoHelper::get<3>, &PosInfoHelper::set<3> ) // A
		.property (  cart_prop_list[4].c_str(), &PosInfoHelper::get<4>, &PosInfoHelper::set<4> ) // B
		.property (  cart_prop_list[5].c_str(), &PosInfoHelper::get<5>, &PosInfoHelper::set<5> ) // C
		.property (  cart_prop_list[6].c_str(), &PosInfoHelper::get<6>, &PosInfoHelper::set<6> ), // Fig.
 					 
		class_<DigitalInput> (LVAR->get_type_name(svDIGITALINPUT))
		.def(constructor<>())
		.def_readwrite (din_prop_list[0].c_str(),     &DigitalInput::device_id )
 		.def_readwrite (din_prop_list[1].c_str(),     &DigitalInput::port )
		.def_readwrite (din_prop_list[2].c_str(),     &DigitalInput::input_value )
		.def_readwrite (din_prop_list[3].c_str(),     &DigitalInput::wait_time )
		.def ("Read", &DigitalInput::read)
		.def ("Wait", &DigitalInput::wait)
		.def ("ReadBit", &DigitalInput::readbit)
		.def ("WaitBit", &DigitalInput::waitbit)
		.def ("ReadNoWait", &DigitalInput::readNoWait)
		.def ("ReadOnPosition", &DigitalInput::read_while_motion),
					 
		class_<DigitalOutput> (LVAR->get_type_name(svDIGITALOUTPUT))
		.def(constructor<>())
		.def_readwrite ( dout_prop_list[0].c_str(), &DigitalOutput::device_id )
 		.def_readwrite ( dout_prop_list[1].c_str(), &DigitalOutput::port )
		.def_readwrite ( dout_prop_list[2].c_str(), &DigitalOutput::output_value )
		.def_readwrite ( dout_prop_list[3].c_str(), &DigitalOutput::pulse_time )
		.def ("Set",  &DigitalOutput::set)
		.def ("Pulse", &DigitalOutput::pulse)
		.def ("SetBit",  &DigitalOutput::setbit)
		.def ("SetNoWait",  &DigitalOutput::set_on_trigger),
					 
		class_<Dynamic> (LVAR->get_type_name(svDYNAMIC))
		.def(constructor<>())
		.def_readwrite (dyn_prop_list[0].c_str(),  &Dynamic::PathVel)
		.def_readwrite (dyn_prop_list[1].c_str(),  &Dynamic::PathAcc)
		.def_readwrite (dyn_prop_list[2].c_str(),  &Dynamic::PathDec)
		.def_readwrite (dyn_prop_list[3].c_str(),  &Dynamic::PathJerk)
		.def_readwrite (dyn_prop_list[4].c_str(),  &Dynamic::JointVel)
		.def_readwrite (dyn_prop_list[5].c_str(),  &Dynamic::JointAcc)
		.def_readwrite (dyn_prop_list[6].c_str(),  &Dynamic::JointDec)
		.def_readwrite (dyn_prop_list[7].c_str(),  &Dynamic::JointJerk)
		.def_readwrite (dyn_prop_list[8].c_str(),  &Dynamic::OriVel)
		.def_readwrite (dyn_prop_list[9].c_str(),  &Dynamic::OriAcc)
		.def_readwrite (dyn_prop_list[10].c_str(),  &Dynamic::OriDec)
		.def_readwrite (dyn_prop_list[11].c_str(),  &Dynamic::OriJerk)
		.def_readwrite (dyn_prop_list[12].c_str(),  &Dynamic::Override)
		.def_readwrite (dyn_prop_list[13].c_str(), &Dynamic::custom_parameter),
					 
		class_<Overlap> (LVAR->get_type_name(svOVERLAP))
		.def(constructor<>())
		.def_readwrite (ovl_prop_list[0].c_str(),  &Overlap::iBufferMode)
		.def_readwrite (ovl_prop_list[1].c_str(),  &Overlap::iTransMode)
		.property (     ovl_prop_list[2].c_str(),  &OverlapHelper::get<0>, &OverlapHelper::set<0> ) 
		.property (     ovl_prop_list[3].c_str(),  &OverlapHelper::get<1>, &OverlapHelper::set<1> )
		.property (     ovl_prop_list[4].c_str(),  &OverlapHelper::get<2>, &OverlapHelper::set<2> ),
					 
		class_<int> ("integer")
		.def(constructor<>()),
		//  		class_<Real> ("Real")
		// 		.def(constructor<>()),
		class_<char> ("char")
		.def(constructor<>()),

		class_<Conveyor> (LVAR->get_type_name(svCONVEYOR))
		.def(constructor<>())
		.def_readwrite (conveyor_prop_list[0].c_str(),  &Conveyor::iCoordType)
		.def_readwrite (conveyor_prop_list[1].c_str(),  &Conveyor::iConveyorType)
		.def_readwrite (conveyor_prop_list[2].c_str(),  &Conveyor::belt_speed)
		.def_readwrite (conveyor_prop_list[3].c_str(),  &Conveyor::belt_acceleration)
		.def_readwrite (conveyor_prop_list[4].c_str(),  &Conveyor::belt_deceleration)
        .def_readwrite (conveyor_prop_list[5].c_str(),  &Conveyor::belt_jerk)
		.def_readwrite (conveyor_prop_list[6].c_str(),  &Conveyor::min_work_area)
		.def_readwrite (conveyor_prop_list[7].c_str(),  &Conveyor::max_work_area)
		.def_readwrite (conveyor_prop_list[8].c_str(),  &Conveyor::xBeltOrg)
		.def_readwrite (conveyor_prop_list[9].c_str(),  &Conveyor::yBeltOrg)
		.def_readwrite (conveyor_prop_list[10].c_str(),  &Conveyor::zBeltOrg)
		.def_readwrite (conveyor_prop_list[11].c_str(),  &Conveyor::aBeltOrg)
		.def_readwrite (conveyor_prop_list[12].c_str(),  &Conveyor::bBeltOrg)
		.def_readwrite (conveyor_prop_list[13].c_str(),  &Conveyor::cBeltOrg)
		.def_readwrite (conveyor_prop_list[14].c_str(),  &Conveyor::xObjInitPos)
		.def_readwrite (conveyor_prop_list[15].c_str(),  &Conveyor::yObjInitPos)
		.def_readwrite (conveyor_prop_list[16].c_str(),  &Conveyor::zObjInitPos)
		.def_readwrite (conveyor_prop_list[17].c_str(),  &Conveyor::aObjInitPos)
		.def_readwrite (conveyor_prop_list[18].c_str(),  &Conveyor::bObjInitPos)
		.def_readwrite (conveyor_prop_list[19].c_str(),  &Conveyor::cObjInitPos)
		.def ("BeginTrack",  &Conveyor::begin_track)
		.def ("EndTrack", &Conveyor::end_track)
		.def ("WaitObj", (int(Conveyor::*)(SensableObject*))  &Conveyor::wait )
		.def ("DoneTrack", (void(Conveyor::*)(SensableObject*,bool)) &Conveyor::done)
        .def ("RefSysObj", &Conveyor::refSysObject),

		class_<SensableObject> (LVAR->get_type_name(svSensableObject))
		.def(constructor<>())
		.def_readwrite (sensableobject_prop_list[0].c_str(),  &SensableObject::iCoordType)
		.def_readwrite (sensableobject_prop_list[1].c_str(),  &SensableObject::iSensorType)
		.def_readwrite (sensableobject_prop_list[2].c_str(),  &SensableObject::xObjectPos)
		.def_readwrite (sensableobject_prop_list[3].c_str(),  &SensableObject::yObjectPos)
		.def_readwrite (sensableobject_prop_list[4].c_str(),  &SensableObject::zObjectPos)
        .def_readwrite (sensableobject_prop_list[5].c_str(),  &SensableObject::aObjectPos)
		.def_readwrite (sensableobject_prop_list[6].c_str(),  &SensableObject::bObjectPos)
		.def_readwrite (sensableobject_prop_list[7].c_str(),  &SensableObject::cObjectPos)
		.def_readwrite (sensableobject_prop_list[8].c_str(),  &SensableObject::width)
		.def_readwrite (sensableobject_prop_list[9].c_str(),  &SensableObject::height)
        .def ("Coord", &SensableObject::refSys),

		class_<SecurArea> (LVAR->get_type_name(svSecurArea))
		.def(constructor<>())
		.def_readwrite (area_prop_list[0].c_str(),  &SecurArea::iShapeType)
		.def_readwrite (area_prop_list[1].c_str(),  &SecurArea::iAreaType)
		.def_readwrite (area_prop_list[2].c_str(),  &SecurArea::iActiveVar)
		.def_readwrite (area_prop_list[3].c_str(),  &SecurArea::iStateVar)
		.def_readwrite (area_prop_list[4].c_str(),  &SecurArea::active)
		.def_readwrite (area_prop_list[5].c_str(),  &SecurArea::emgcOut)
		.def_readwrite (area_prop_list[6].c_str(),  &SecurArea::xPos)
		.def_readwrite (area_prop_list[7].c_str(),  &SecurArea::yPos)
		.def_readwrite (area_prop_list[8].c_str(),  &SecurArea::zPos)
		.def_readwrite (area_prop_list[9].c_str(),  &SecurArea::aPos)
		.def_readwrite (area_prop_list[10].c_str(),  &SecurArea::bPos)
		.def_readwrite (area_prop_list[11].c_str(),  &SecurArea::cPos)
		.property (     area_prop_list[12].c_str(),  &SecurAreaShapeHelper::get<0>, &SecurAreaShapeHelper::set<0> ) 
		.property (     area_prop_list[13].c_str(),  &SecurAreaShapeHelper::get<1>, &SecurAreaShapeHelper::set<1> )
		.property (     area_prop_list[14].c_str(),  &SecurAreaShapeHelper::get<2>, &SecurAreaShapeHelper::set<2> )
 		.def ("IsRobPosInArea",(bool(SecurArea::*)()) &SecurArea::IsRobPosInArea)
		.def ("IsPosInArea",(bool(SecurArea::*)(const CartPos*)) &SecurArea::IsPosInArea)
 		.def ("Activate", &SecurArea::Activate)
		.def ("Deactivate", &SecurArea::Deactivate)
 		.def ("SetTransformation", (void(SecurArea::*)(double,double,double)) &SecurArea::SetTransformation)
		.def ("SetBoxSize", (void(SecurArea::*)(double,double,double)) &SecurArea::SetBoxSize)
		.def ("SetCylinderSize", (void(SecurArea::*)(double,double)) &SecurArea::SetCylinderSize)
		.def ("WaitRobInside", (void(SecurArea::*)(double,double)) &SecurArea::WaitRobInside)
		.def ("Connect", (void(SecurArea::*)(int)) &SecurArea::Connect)
		.def ("Disconnect", (void(SecurArea::*)()) &SecurArea::Disconnect)
		.def ("Disconnect", (bool(SecurArea::*)(const CartPos*)) &SecurArea::PosHasSpaceViolation)
		,

		registerLuaInteger(),
		registerLuaBool(),
		registerLuaReal(),
		registerLuaString(),
		registerLuaAngle(),
		registerLuaAPOS(apos_prop_list),
		registerLuaCPOS(cart_prop_list, apos_prop_list)
	];
	apos_prop_list.clear();
	cart_prop_list.clear();
	dyn_prop_list.clear();
	ovl_prop_list.clear();
	din_prop_list.clear();
	dout_prop_list.clear();
	conveyor_prop_list.clear();
	sensableobject_prop_list.clear();
	area_prop_list.clear();
	 
 }

void LuaScript::addVariableToState( const int sv, const char* vname, void* ptrValue )
{
	string tname (LVAR->get_type_name((ScriptVarT)sv));

	if ( sv == svAXISPOS || sv == svCARTPOS )
	{
		luabind::globals(m_luaState->getState())[vname] =  static_cast<PosInfo*>(ptrValue);
 	}
	else if ( sv == svINTEGER || sv == svBOOL || sv == svRDI || sv == svRDO )
	{
 		luabind::globals(m_luaState->getState())[vname] =  static_cast<int*>(ptrValue);
  	}
	else if ( sv == svREAL )
	{
		luabind::globals(m_luaState->getState())[vname] =  static_cast<Real*>(ptrValue);
 	}
	else if ( sv == svDYNAMIC )
	{
		luabind::globals(m_luaState->getState())[vname] =  static_cast<Dynamic*>(ptrValue);
 	}
	else if ( sv == svOVERLAP )
	{
		luabind::globals(m_luaState->getState())[vname] =  static_cast<Overlap*>(ptrValue);
	}
	else if ( sv == svDIGITALINPUT )
	{
		luabind::globals(m_luaState->getState())[vname] =  static_cast<DigitalInput*>(ptrValue);
	}
	else if ( sv == svDIGITALOUTPUT )
	{
		luabind::globals(m_luaState->getState())[vname] =  static_cast<DigitalOutput*>(ptrValue);
	}
	else if ( sv == svCARTREF || sv == svTOOL )
	{
		luabind::globals(m_luaState->getState())[vname] =  static_cast<CartRef*>(ptrValue);
	}
	else if ( sv == svSTRING )
	{
		luabind::globals(m_luaState->getState())[vname] =  static_cast<const char*>(ptrValue);
 	}
	else if ( sv == svLUAINTEGER )
	{
		luabind::globals(m_luaState->getState())[vname] =  static_cast<LuaInteger*>(ptrValue);
	}
	else if ( sv == svLUABOOL )
	{
		luabind::globals(m_luaState->getState())[vname] =  static_cast<LuaBool*>(ptrValue);
	}
	else if ( sv == svLUAREAL )
	{
		luabind::globals(m_luaState->getState())[vname] =  static_cast<LuaReal*>(ptrValue);
	}
	else if ( sv == svLUASTRING )
	{
		luabind::globals(m_luaState->getState())[vname] =  static_cast<LuaString*>(ptrValue);
	}
	else if ( sv == svCONVEYOR )
	{
		luabind::globals(m_luaState->getState())[vname] =  static_cast<Conveyor*>(ptrValue);
	}
	else if ( sv == svSensableObject )
	{
		luabind::globals(m_luaState->getState())[vname] =  static_cast<SensableObject*>(ptrValue);
 	}
	else if ( sv == svSecurArea )
	{
		luabind::globals(m_luaState->getState())[vname] =  static_cast<SecurArea*>(ptrValue);
 	}

}

 

void LuaScript::addFunctionToState( void )
{
	int count = 0;
 
	module(m_luaState->getState())
		[
			def( "CHook", &ScriptFunc::luaHook),
			
			def( "WaitTime", (int(*)(long)) &ScriptFunc::WaitTime ),
			def( "SPEED", (void(*)(double)) &ScriptFunc::SPEED ),
 			def( "BLEND", (void(*)(int))    &ScriptFunc::BLEND ),
			def( "PTP",  (void(*)(PosInfo))                   &ScriptFunc::MOVEP ),
			def( "PTP",  (void(*)(PosInfo,Dynamic*))          &ScriptFunc::MOVEP ),
			def( "PTP",  (void(*)(PosInfo,Dynamic*,Overlap*)) &ScriptFunc::MOVEP ),
			def( "Lin",  (void(*)(PosInfo))                   &ScriptFunc::MOVEL ),
			def( "Lin",  (void(*)(PosInfo,Dynamic*))          &ScriptFunc::MOVEL ),
			def( "Lin",  (void(*)(PosInfo,Dynamic*,Overlap*)) &ScriptFunc::MOVEL ),
			def( "Cir",  (void(*)(PosInfo,PosInfo))                   &ScriptFunc::MOVEC ),
			def( "Cir",  (void(*)(PosInfo,PosInfo,Dynamic*))          &ScriptFunc::MOVEC ),
			def( "Cir",  (void(*)(PosInfo,PosInfo,Dynamic*,Overlap*)) &ScriptFunc::MOVEC ),
			def( "PTPRel",  (void(*)(PosInfo))                   &ScriptFunc::MOVEPREL ),
			def( "PTPRel",  (void(*)(PosInfo,Dynamic*))          &ScriptFunc::MOVEPREL ),
			def( "PTPRel",  (void(*)(PosInfo,Dynamic*,Overlap*)) &ScriptFunc::MOVEPREL ),
			def( "LinRel",  (void(*)(PosInfo))                   &ScriptFunc::MOVELREL ),
			def( "LinRel",  (void(*)(PosInfo,Dynamic*))          &ScriptFunc::MOVELREL ),
			def( "LinRel",  (void(*)(PosInfo,Dynamic*,Overlap*)) &ScriptFunc::MOVELREL ),
			def( "CirRel",  (void(*)(PosInfo,PosInfo))                   &ScriptFunc::MOVECREL ),
			def( "CirRel",  (void(*)(PosInfo,PosInfo,Dynamic*))          &ScriptFunc::MOVECREL ),
			def( "CirRel",  (void(*)(PosInfo,PosInfo,Dynamic*,Overlap*)) &ScriptFunc::MOVECREL ),
			def( "GetRobPos", (void(*)(PosInfo*))                  &ScriptFunc::GetRobPos ),
			def( "DIWait", (int(*)(int, unsigned int, unsigned int, long, bool))  &ScriptFunc::WAITIO ),
			def( "DIRead",         (bool(*)(int, unsigned int, bool,bool))       &ScriptFunc::INIO ),
			def( "DOSet",          (void(*)(int,bool))  &ScriptFunc::OUTIO ),
			def( "WaitIsFinished", (bool(*)())          &ScriptFunc::WaitIsFinished ),
			def( "Info",       (void(*)(const char*))               &ScriptFunc::Info ),
			def( "OnDistance", (int(*)(int,double,DigitalOutput*,bool))  &ScriptFunc::on_distance_dout ),
			def( "OnPosition", (int(*)(double,DigitalOutput*,bool))       &ScriptFunc::on_position_dout ),
			def( "ReadWhileMotion", (bool(*)(double,int))       &ScriptFunc::on_position_din ),
			def( "Dyn",    (void(*)(Dynamic*)) &ScriptFunc::SET_DYNAMIC ),
			def( "Ovl",    (void(*)(Overlap*)) &ScriptFunc::SET_OVERLAP ),
			def( "RefSys", (void(*)(CartRef*)) &ScriptFunc::SET_REFCART ),
			def( "Tool",   (void(*)(CartRef*)) &ScriptFunc::SET_TOOL ),
			def( "TrackVis", (void(*)(Dynamic*)) &ScriptFunc::TRACKVIS ),
			def( "TrackVis", (void(*)(Dynamic*,Overlap*)) &ScriptFunc::TRACKVIS ),
 
			def( "GETINT", (int(*)(int*))        &get_int_variable ),
			def( "GETREAL", (double(*)(double*)) &get_real_variable ),
			def( "GETSTR", (string(*)(const char*)) &get_string_variable ),
			def( "LOADSUB", (void(*)(const char*)) &ScriptFunc::LOADSUB )

		];
}

int LuaScript::get_int_variable( int* data)
{
	return *data;
}

double LuaScript::get_real_variable(double* data)
{
	return *data;
}

string LuaScript::get_string_variable(const char* data)
{
	return string(data);
}

luabind::scope registerLuaInteger()
{
	return	class_<LuaInteger>("INT")
		.def(constructor<>())
		.def(constructor<int>())
		.def(constructor<const LuaInteger&>())
		.def(const_self + other<int>())  
		.def(const_self - other<int>())  
		.def(const_self * other<int>())  
		.def(const_self / other<int>()) 
		.def(const_self == other<const int>())
		.def(const_self <= other<int>())
		.def(const_self <  other<int>())
		.def(const_self == const_self)
		.def(const_self <= const_self)
		.def(const_self <  const_self)

		.def(-self)  
		.def(self())  
		.def(self(int()))

		.def(const_self   * const_self)  
		.def(other<int>() + const_self)  
		.def(other<int>() - const_self)  
		.def(other<int>() * const_self)  
		.def(other<int>() / const_self)  
		.def(other<int>() == const_self)  
		.def(other<int>() <= const_self)  
		.def(other<int>() <  const_self) 

		.def(tostring(const_self))
		;
}

luabind::scope registerLuaBool()
{
	return class_<LuaBool>("BOOL")
		.def(constructor<>())
		.def(constructor<bool>())
		.def(constructor<const LuaBool&>())
		.def(self == other<bool>())
		.def(const_self == const_self )

		.def(self())  
		.def(self(other<bool>()))

		.def(other<bool>() == const_self)  

		.def(tostring(const_self))
		;
}

luabind::scope registerLuaReal()
{
	return class_<LuaReal>("REAL")
		.def(constructor<>())
		.def(constructor<double>())
		.def(constructor<const LuaReal&>())
		.def(const_self + other<double>())  
		.def(const_self - other<double>())  
		.def(const_self * other<double>())  
		.def(const_self / other<double>()) 
		.def(const_self == other<double>())
		.def(const_self <= other<double>())
		.def(const_self <  other<double>())
		.def(const_self == const_self)
		.def(const_self <= const_self)
		.def(const_self <  const_self)

		.def(-self)  
		.def(self())  
		.def(self(double()))

		.def(const_self   * const_self)  
		.def(other<double>() + const_self)  
		.def(other<double>() - const_self)  
		.def(other<double>() * const_self)  
		.def(other<double>() / const_self)  
		.def(other<double>() == const_self)  
		.def(other<double>() <= const_self)  
		.def(other<double>() <  const_self) 

		.def(tostring(const_self))
		;
}

luabind::scope registerLuaString()
{
	return 	class_<LuaString>("STRING")
		.def(constructor<>())
		.def(constructor<std::string>())
		// 		.def(constructor<const char*>())
		.def(constructor<const LuaString&>())
		.def(self + other<std::string>())  
		.def(self == other<std::string>())
		.def(const_self == const_self )

		.def(self())  
		.def(self(other<std::string>()))

		.def(other<std::string>() + self)  
		.def(other<std::string>() == const_self)  

		.def(tostring(const_self))  
		;
}

luabind::scope registerLuaAngle()
{
	return class_<Angle>("ANGLE")
		.def(constructor<>())
		.def(constructor<double>())
		.def(constructor<const Angle&>())
		.def(const_self + other<Angle>())  
		.def(const_self - other<Angle>())  
		.def(const_self == other<Angle>())

		.def(-self)  

		.def(const_self   * other<double>())  
		.def(other<double>() * const_self)  

		.def(tostring(const_self))
		.def ("SetRad", &Angle::set_rad)
		.def ("GetRad", &Angle::get_rad)
		.def ("SetDeg", &Angle::set_deg)
		.def ("GetDeg", &Angle::get_deg)
		;
}

luabind::scope registerLuaAPOS( const std::vector<std::string>& apos_prop_list )
{
	return class_<LuaAPOS> (LVAR->get_type_name(svAXISPOS)) // APOS
		.def(constructor<>())
		.def(constructor<double,double,double,double,double,double,double,double>())
		.def(constructor<const PosInfo&>())

		.def(self())  
		.def_readwrite ("cart",     &LuaAPOS::cart )
		.def_readwrite ("axis",     &LuaAPOS::axis )
		.def_readwrite ("vtrans",   &LuaAPOS::vtrans )
		.def_readwrite ("vrot",     &LuaAPOS::vrot   )
		.def_readwrite ("auxi",     &LuaAPOS::auxi   )
		.def_readwrite ("valid",    &LuaAPOS::valid  )
		.def_readwrite ("posType",  &LuaAPOS::pos_type)
		.def ("SetAxis", &LuaAPOS::set_axis)
		.def ("SetCart", &LuaAPOS::set_cart)
		.property (  apos_prop_list[0].c_str(), &LuaAPOSHelper::get<0>, &LuaAPOSHelper::set<0> ) // a1
		.property (  apos_prop_list[1].c_str(), &LuaAPOSHelper::get<1>, &LuaAPOSHelper::set<1> ) // a2
		.property (  apos_prop_list[2].c_str(), &LuaAPOSHelper::get<2>, &LuaAPOSHelper::set<2> ) // a3
		.property (  apos_prop_list[3].c_str(), &LuaAPOSHelper::get<3>, &LuaAPOSHelper::set<3> ) // a4
		.property (  apos_prop_list[4].c_str(), &LuaAPOSHelper::get<4>, &LuaAPOSHelper::set<4> )
		.property (  apos_prop_list[5].c_str(), &LuaAPOSHelper::get<5>, &LuaAPOSHelper::set<5> )
		.property (  apos_prop_list[6].c_str(), &LuaAPOSHelper::get<7>, &LuaAPOSHelper::set<7> ) // a7
		.property (  apos_prop_list[7].c_str(), &LuaAPOSHelper::get<8>, &LuaAPOSHelper::set<8> ) // a8 
		;
}

luabind::scope registerLuaCPOS( const std::vector<std::string>& cart_prop_list, const std::vector<std::string>& apos_prop_list )
{
	return class_<LuaCPOS> (LVAR->get_type_name(svCARTPOS)) // CPOS
		.def(constructor<>())
		.def(constructor<double,double,double,double,double,double,int,double,double>())
		.def(constructor<const PosInfo&>())

		.def(self()) 
		.def_readwrite ("cart",     &LuaCPOS::cart )
		.def_readwrite ("axis",     &LuaCPOS::axis )
		.def_readwrite ("vtrans",   &LuaCPOS::vtrans )
		.def_readwrite ("vrot",     &LuaCPOS::vrot   )
		.def_readwrite ("auxi",     &LuaCPOS::auxi   )
		.def_readwrite ("valid",    &LuaCPOS::valid  )
		.def_readwrite ("posType",  &LuaCPOS::pos_type)
		.def ("SetAxis", &LuaCPOS::set_axis)
		.def ("SetCart", &LuaCPOS::set_cart)
		.property (  apos_prop_list[6].c_str(), &LuaCPOSHelper::get<7>, &LuaCPOSHelper::set<7> ) // a7
		.property (  apos_prop_list[7].c_str(), &LuaCPOSHelper::get<8>, &LuaCPOSHelper::set<8> ) // a8 

		.property (  cart_prop_list[0].c_str(), &LuaCPOSHelper::get<0>, &LuaCPOSHelper::set<0> ) // CPOS : x
		.property (  cart_prop_list[1].c_str(), &LuaCPOSHelper::get<1>, &LuaCPOSHelper::set<1> ) // y
		.property (  cart_prop_list[2].c_str(), &LuaCPOSHelper::get<2>, &LuaCPOSHelper::set<2> ) // z
		.property (  cart_prop_list[3].c_str(), &LuaCPOSHelper::get<3>, &LuaCPOSHelper::set<3> ) // A
		.property (  cart_prop_list[4].c_str(), &LuaCPOSHelper::get<4>, &LuaCPOSHelper::set<4> ) // B
		.property (  cart_prop_list[5].c_str(), &LuaCPOSHelper::get<5>, &LuaCPOSHelper::set<5> ) // C
		.property (  cart_prop_list[6].c_str(), &LuaCPOSHelper::get<6>, &LuaCPOSHelper::set<6> ) // Fig.
		;
}