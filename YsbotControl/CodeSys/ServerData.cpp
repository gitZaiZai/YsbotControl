// Copyright 2016, 亚思信息科技有限责任公司
// All right reserved.
//
// Author: Liu Run Dan ( lrdcz@163.com )
//
// Yasi Robot Motion Control System - 亚思机器人运动控制系统

#include "stdafx.h"
#include "ServerData.h"

using namespace Ysbot;

std::ostream& operator<< (  std::ostream& os, const IECServerData& sdata )
{
	std::string strgs;
	bool bStandstill = true;
	for ( int i = 0; i < sdata.iAxisNum; ++i )
	{
		if ( sdata.iAxisState[i] != standstill )
		{
			bStandstill = false;
			break;
		}
	}

 	os << group_state_names[sdata.group_state] 
//	os << (bStandstill ? "Standstill" : "Not Stand") 
	   << ' ' << sdata.iCommandIndex << ' ' << sdata.iCurrMotionIndex  << ' ' 
	   << sdata.iCommandRet << ' ' << sdata.errID << "| "
	   << sdata.axisPos.apos.a0 << ' '<< sdata.axisPos.apos.a1 << ' '<< sdata.axisPos.apos.a2 << ' ' 
	   << sdata.axisPos.apos.a3 << ' '<< sdata.axisPos.apos.a4 << ' '<< sdata.axisPos.apos.a5 <<" | "
	   << sdata.cartPosBaseMCS.coordRef.X << ' '<< sdata.cartPosBaseMCS.coordRef.Y << ' '<< sdata.cartPosBaseMCS.coordRef.Z << ' '
	   << sdata.cartPosBaseMCS.coordRef.A << ' '<< sdata.cartPosBaseMCS.coordRef.B << ' '<< sdata.cartPosBaseMCS.coordRef.C << " | ";
	return os;
}

