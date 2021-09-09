// Copyright 2016, 亚思信息科技有限责任公司
// All right reserved.
//
// Author: Liu Run Dan ( lrdcz@163.com )
//
// Yasi Robot Motion Control System - 亚思机器人运动控制系统

#ifndef _server_data_
#define _server_data_

#include <AclAPI.h>
#include <ostream>
#include <fstream>
#include "IecBasicTypes.h"

const unsigned long NETRGUSER_CFM_CODE = 0x1211DBFF; // 校驗碼, 用於命令數據
const unsigned long NETRGUSER_CMD_NONE = 0;          // 初始化指令碼, 無指令


// 以下為錯誤碼定義
#define ERROR_LEN             256         // 錯誤描述長度
#define ERROR_INVALID_CMDCODE 0xE00001FF  // 已經存在完全一樣的共享內存
#define ERROR_NO_MAPFILE      0xE00002FF  // 未分配共享內存文件
#define ERROR_INVALID_CFMCODE 0xE00003FF  // 校驗碼不匹配

namespace Ysbot {

#pragma pack(push) //保存对齐状态
// #if USE_MACHINE_X86
// 	#pragma pack(4)//设定为4字节对齐
// #endif
#ifdef USE_MACHINE_X86
	#pragma pack(4) //设定为4字节对齐
#endif // USE_MACHINE_X86
	typedef struct _tagIECMotionInfo
	{
		SMC_AXIS_GROUP_STATE group_state;
		ULINT                nCommandIndex;
		BOOL                 InSync; 
		BOOL                 TrackingDynamicCS;
	}IECMotionInfo, *LPIECMotionInfo;
 
	typedef struct _tagIECServerData
	{
		short            iMotionState;

		ULINT            iCommandRet;
		unsigned int     iCommandIndex;

		SMC_AXIS_GROUP_STATE group_state;
		ULINT                iCurrMotionIndex;
		bool                 InSync; 
		bool                 TrackingDynamicCS;

		short            coordSystem;

		SMC_POS_REF      acceleration;
		SMC_POS_REF      velocity;
		SMC_POS_REF      axisPos;
		SMC_POS_REF      cartPos;
		SMC_POS_REF      cartPosBaseMCS;

		SMC_POS_REF      auxAxisPos;

		short            errType; // 2 bytes
		ULINT            errID; // 8bytes

        short int        iAxisState[12];
		short int		 iServoAxisState[12];
        short int        iAxisNum ;

		WORD             wInputValue[8];
		short int        iAuxGroupState;

		double           dfMemData[10];
		int              iMemdata[10];

		_tagIECServerData()
		{
			iMotionState        = 0;
			iCommandRet   = 0;
			iCommandIndex = 0;
			errID         = 0;
			for ( int i = 0; i < 6; ++i )
			{
				acceleration.valArray[i] = 0;
				velocity.valArray[i] = 0;
				axisPos.valArray[i] = 0;
				cartPos.valArray[i] = 0;
				cartPosBaseMCS.valArray[i] = 0;
				auxAxisPos.valArray[i] = 0;
 			}
			for ( int i =0; i < 10; ++i )
			{
				dfMemData[i] = 0.0;
				iMemdata[i] = 0;
			}
		}
	}IECServerData, *LPIECServerData;
  
	// 用於存儲命令數據的內存文件格式
	typedef struct _tagDATA_HEADER
	{
		DWORD dwConfirmCode; // 校驗碼
		DWORD nCommandCode;  // 指令識別碼
		DWORD dwDataSize;    // 數據的大小
		BYTE  dwReserved[19]; // 保留
		BYTE  bInfo[1];       // 數據起始地址
		_tagDATA_HEADER()
		{
			dwConfirmCode = NETRGUSER_CFM_CODE;
			nCommandCode  = NETRGUSER_CMD_NONE;
			dwDataSize = 0;
			ZeroMemory(dwReserved, 19);
			ZeroMemory(bInfo, 1);
		}
	}DATA_HEADER, *LPDATA_HEADER;

#pragma pack(pop) //恢复对齐状态

}
  

std::ostream& operator<< ( std::ostream& os, const Ysbot::IECServerData& sdata );

#endif