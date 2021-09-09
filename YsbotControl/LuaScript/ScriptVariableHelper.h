// Copyright 2016, 亚思信息科技有限责任公司
// All right reserved.
//
// Author: Liu Run Dan ( lrdcz@163.com )
//
// Yasi Robot Motion Control System - 亚思机器人运动控制系统

#ifndef _script_variable_helper
#define _script_variable_helper

#include "../PosInfo.h"
#include "../CmdInfo.h"


struct AxisPosHelper
{
	template <unsigned index>
	static float get (AxisPos const* vec)
	{
		switch(index)
		{
		case 0: return vec->q1*CONST180PI; 
		case 1: return vec->q2*CONST180PI; 
		case 2: return vec->q3*CONST180PI; 
		case 3: return vec->q4*CONST180PI; 
		case 4: return vec->q5*CONST180PI; 
		case 5: return vec->q6*CONST180PI; 
		default:break;
		}
	}

	template <unsigned index>
	static void set (AxisPos* vec, double value)
	{
		switch(index)
		{
		case 0: vec->q1 = value*CONSTPI180; break;
		case 1: vec->q2 = value*CONSTPI180; break;
		case 2: vec->q3 = value*CONSTPI180; break;
		case 3: vec->q4 = value*CONSTPI180; break;
		case 4: vec->q5 = value*CONSTPI180; break;
		case 5: vec->q6 = value*CONSTPI180; break;
		default:break;
		}
	}
};

// for tool variable operation.
struct CartPosHelper
{
	template <unsigned int index>
	static double get (CartPos const* cart)
	{
		switch(index)
		{
		case 0: return cart->pos.x; 
		case 1: return cart->pos.y; 
		case 2: return cart->pos.z; 
		case 3: return cart->ang.x*CONST180PI; 
		case 4: return cart->ang.y*CONST180PI; 
		case 5: return cart->ang.z*CONST180PI; 
		default:return 0.0;
		}
	}

	template <unsigned int index>
	static void set (CartPos* cart, double value)
	{
		switch(index)
		{
		case 0: cart->pos.x = value; break;
		case 1: cart->pos.y = value; break;
		case 2: cart->pos.z = value; break;
		case 3: cart->ang.x = value*CONSTPI180; break;
		case 4: cart->ang.y = value*CONSTPI180; break;
		case 5: cart->ang.z = value*CONSTPI180; break;
		default:break;
		}
	}
};

// for work coordinate variable operation.
struct CartRefHelper
{
	template <unsigned int index>
	static double get (CartRef const* cref)
	{
		switch(index)
		{
		case 0: return cref->cpos.pos.x; 
		case 1: return cref->cpos.pos.y; 
		case 2: return cref->cpos.pos.z; 
		case 3: return cref->cpos.ang.x*CONST180PI; 
		case 4: return cref->cpos.ang.y*CONST180PI; 
		case 5: return cref->cpos.ang.z*CONST180PI; 
		default:return 0.0;
		}
	}

	template <unsigned int index>
	static void set (CartRef* cref, double value)
	{
		switch(index)
		{
		case 0:  cref->cpos.pos.x = value; break;
		case 1:  cref->cpos.pos.y = value; break;
		case 2:  cref->cpos.pos.z = value; break;
		case 3:  cref->cpos.ang.x = value*CONSTPI180; break;
		case 4:  cref->cpos.ang.y = value*CONSTPI180; break;
		case 5:  cref->cpos.ang.z = value*CONSTPI180; break;
		default:break;
		}
	}
};

// for work coordinate variable operation.
struct ToolHelper
{
	template <unsigned int index>
	static double get (Tool const* tool)
	{
		switch(index)
		{
		case 0: return tool->cpos.pos.x; 
		case 1: return tool->cpos.pos.y; 
		case 2: return tool->cpos.pos.z; 
		case 3: return tool->cpos.ang.x*CONST180PI; 
		case 4: return tool->cpos.ang.y*CONST180PI; 
		case 5: return tool->cpos.ang.z*CONST180PI; 
		default:return 0.0;
		}
	}

	template <unsigned int index>
	static void set (Tool* tool, double value)
	{
		switch(index)
		{
		case 0:  tool->cpos.pos.x = value; break;
		case 1:  tool->cpos.pos.y = value; break;
		case 2:  tool->cpos.pos.z = value; break;
		case 3:  tool->cpos.ang.x = value*CONSTPI180; break;
		case 4:  tool->cpos.ang.y = value*CONSTPI180; break;
		case 5:  tool->cpos.ang.z = value*CONSTPI180; break;
		default:break;
		}
	}
};

struct PosInfoHelper
{
	template <unsigned int index>
	static double get (PosInfo const* pinfo)
	{
		if ( pinfo->pos_type == PosInfo::CTaxis )
		{
			switch(index)
			{
			case 0: return pinfo->axis.q1*CONST180PI; 
			case 1: return pinfo->axis.q2*CONST180PI; 
			case 2: return pinfo->axis.q3*CONST180PI; 
			case 3: return pinfo->axis.q4*CONST180PI; 
			case 4: return pinfo->axis.q5*CONST180PI; 
			case 5: return pinfo->axis.q6*CONST180PI; 
			case 6: return 0.0f; 
			case 7: return pinfo->auxi.q1*CONST180PI; 
			case 8: return pinfo->auxi.q2*CONST180PI; 
			default:return 0.0f;
			}
		}
		else if ( pinfo->pos_type == PosInfo::CTmachine )
		{
			switch(index)
			{
			case 0: return pinfo->cart.pos.x; 
			case 1: return pinfo->cart.pos.y; 
			case 2: return pinfo->cart.pos.z; 
			case 3: return pinfo->cart.ang.x*CONST180PI; 
			case 4: return pinfo->cart.ang.y*CONST180PI; 
			case 5: return pinfo->cart.ang.z*CONST180PI; 
			case 6: return pinfo->cart.fig;  
			case 7: return pinfo->auxi.q1*CONST180PI; 
			case 8: return pinfo->auxi.q2*CONST180PI;
			default:return 0.0f;
			}
		}
		else
			return 0.0f;

	}

	template <unsigned int index>
	static void set (PosInfo* pinfo, double value)
	{
		if ( pinfo->pos_type == PosInfo::CTaxis )
		{
			switch(index)
			{
			case 0: pinfo->axis.q1 = value*CONSTPI180; break;
			case 1: pinfo->axis.q2 = value*CONSTPI180; break;
			case 2: pinfo->axis.q3 = value*CONSTPI180; break;
			case 3: pinfo->axis.q4 = value*CONSTPI180; break;
			case 4: pinfo->axis.q5 = value*CONSTPI180; break;
			case 5: pinfo->axis.q6 = value*CONSTPI180; break;
			case 6: break;
			case 7: pinfo->auxi.q1 = value*CONSTPI180; break;
			case 8: pinfo->auxi.q2 = value*CONSTPI180; break;
			default:break;
			}
		}
		else if ( pinfo->pos_type == PosInfo::CTmachine )
		{
			switch(index)
			{
			case 0: pinfo->cart.pos.x = value; break;
			case 1: pinfo->cart.pos.y = value; break;
			case 2: pinfo->cart.pos.z = value; break;
			case 3: pinfo->cart.ang.x = value*CONSTPI180; break;
			case 4: pinfo->cart.ang.y = value*CONSTPI180; break;
			case 5: pinfo->cart.ang.z = value*CONSTPI180; break;
			case 6: pinfo->cart.fig   = (int)value; break;
			case 7: pinfo->auxi.q1    = value*CONSTPI180; break;
			case 8: pinfo->auxi.q2    = value*CONSTPI180; break;
			default:break;
			}
		}
	}
};

struct OverlapHelper
{
	template <unsigned index>
	static double get (Overlap const* vec)
	{
		return vec->TransitionParameter[index]; 
	}

	template <unsigned index>
	static void set (Overlap* vec, double value)
	{
		cout <<"index = " << index << '\n';
		vec->TransitionParameter[index] = value;
	}
};

struct LuaAPOSHelper
{
	template <unsigned int index>
	static double get (LuaAPOS const* pinfo)
	{
		switch(index)
		{
		case 0: return pinfo->axis.q1*CONST180PI; 
		case 1: return pinfo->axis.q2*CONST180PI; 
		case 2: return pinfo->axis.q3*CONST180PI; 
		case 3: return pinfo->axis.q4*CONST180PI; 
		case 4: return pinfo->axis.q5*CONST180PI; 
		case 5: return pinfo->axis.q6*CONST180PI; 
		case 6: return 0.0f; 
		case 7: return pinfo->auxi.q1*CONST180PI; 
		case 8: return pinfo->auxi.q2*CONST180PI; 
		default:return 0.0f;
		}
	}

	template <unsigned int index>
	static void set (LuaAPOS* pinfo, double value)
	{
		switch(index)
		{
		case 0: pinfo->axis.q1 = value*CONSTPI180; break;
		case 1: pinfo->axis.q2 = value*CONSTPI180; break;
		case 2: pinfo->axis.q3 = value*CONSTPI180; break;
		case 3: pinfo->axis.q4 = value*CONSTPI180; break;
		case 4: pinfo->axis.q5 = value*CONSTPI180; break;
		case 5: pinfo->axis.q6 = value*CONSTPI180; break;
		case 6: break;
		case 7: pinfo->auxi.q1 = value*CONSTPI180; break;
		case 8: pinfo->auxi.q2 = value*CONSTPI180; break;
		default:break;
		}

	}
};

struct LuaCPOSHelper
{
	template <unsigned int index>
	static double get (LuaCPOS const* pinfo)
	{
		switch(index)
		{
		case 0: return pinfo->cart.pos.x; 
		case 1: return pinfo->cart.pos.y; 
		case 2: return pinfo->cart.pos.z; 
		case 3: return pinfo->cart.ang.x*CONST180PI; 
		case 4: return pinfo->cart.ang.y*CONST180PI; 
		case 5: return pinfo->cart.ang.z*CONST180PI; 
		case 6: return pinfo->cart.fig;  
		case 7: return pinfo->auxi.q1*CONST180PI; 
		case 8: return pinfo->auxi.q2*CONST180PI;
		default:return 0.0f;
		}
	}

	template <unsigned int index>
	static void set (LuaCPOS* pinfo, double value)
	{
		switch(index)
		{
		case 0: pinfo->cart.pos.x = value; break;
		case 1: pinfo->cart.pos.y = value; break;
		case 2: pinfo->cart.pos.z = value; break;
		case 3: pinfo->cart.ang.x = value*CONSTPI180; break;
		case 4: pinfo->cart.ang.y = value*CONSTPI180; break;
		case 5: pinfo->cart.ang.z = value*CONSTPI180; break;
		case 6: pinfo->cart.fig   = (int)value; break;
		case 7: pinfo->auxi.q1    = value*CONSTPI180; break;
		case 8: pinfo->auxi.q2    = value*CONSTPI180; break;
		default:break;
		}
	}

};

struct SecurAreaShapeHelper
{
	template <unsigned index>
	static double get (SecurArea const* area)
	{
		return area->shapeParam[index]; 
	}

	template <unsigned index>
	static void set (SecurArea* area, double value)
	{
 		area->shapeParam[index] = value;
	}
};

#endif