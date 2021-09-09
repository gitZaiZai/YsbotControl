// Copyright 2016, ��˼��Ϣ�Ƽ��������ι�˾
// All right reserved.
//
// Author: Liu Run Dan ( lrdcz@163.com )
//
// Yasi Robot Motion Control System - ��˼�������˶�����ϵͳ

#ifndef _server_data_
#define _server_data_

#include <AclAPI.h>
#include <ostream>
#include <fstream>
#include "IecBasicTypes.h"

const unsigned long NETRGUSER_CFM_CODE = 0x1211DBFF; // У�a, ��������
const unsigned long NETRGUSER_CMD_NONE = 0;          // ��ʼ��ָ��a, �oָ��


// �����e�`�a���x
#define ERROR_LEN             256         // �e�`�����L��
#define ERROR_INVALID_CMDCODE 0xE00001FF  // �ѽ�������ȫһ�ӵĹ���ȴ�
#define ERROR_NO_MAPFILE      0xE00002FF  // δ���乲��ȴ��ļ�
#define ERROR_INVALID_CFMCODE 0xE00003FF  // У�a��ƥ��

namespace Ysbot {

#pragma pack(push) //�������״̬
// #if USE_MACHINE_X86
// 	#pragma pack(4)//�趨Ϊ4�ֽڶ���
// #endif
#ifdef USE_MACHINE_X86
	#pragma pack(4) //�趨Ϊ4�ֽڶ���
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
  
	// ��춴惦������ăȴ��ļ���ʽ
	typedef struct _tagDATA_HEADER
	{
		DWORD dwConfirmCode; // У�a
		DWORD nCommandCode;  // ָ���R�e�a
		DWORD dwDataSize;    // �����Ĵ�С
		BYTE  dwReserved[19]; // ����
		BYTE  bInfo[1];       // ������ʼ��ַ
		_tagDATA_HEADER()
		{
			dwConfirmCode = NETRGUSER_CFM_CODE;
			nCommandCode  = NETRGUSER_CMD_NONE;
			dwDataSize = 0;
			ZeroMemory(dwReserved, 19);
			ZeroMemory(bInfo, 1);
		}
	}DATA_HEADER, *LPDATA_HEADER;

#pragma pack(pop) //�ָ�����״̬

}
  

std::ostream& operator<< ( std::ostream& os, const Ysbot::IECServerData& sdata );

#endif