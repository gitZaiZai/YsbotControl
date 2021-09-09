// Copyright 2016, 亚思信息科技有限责任公司
// All right reserved.
//
// Author: Liu Run Dan ( lrdcz@163.com )
//
// Yasi Robot Motion Control System - 亚思机器人运动控制系统

#ifndef numeric_kinematic_h
#define numeric_kinematic_h
#include "ConfigReader.h"
#include "KinematicsBase.h"

namespace Ysbot
{
	int init_kine_numeric( const ConfigReader& ) throw (InvalidConfigurationException);
	void release_kine_numeric();
	extern Robot*  peter_fwk;

	BOOL __stdcall Numeric_Fwk(int nGroup,
		int nSynAxisNum,
		const double* pdfFrom,
		double* pdfTo,
		DWORD& dwPosture,
		void* pvExtension);

	BOOL __stdcall Numeric_Ink(int nGroup,
		int nSynAxisNum,
		const double* pdfFrom,
		const double* pdfPrevResult,
		double* pdfTo,
		DWORD& dwPosture,
		void* pvExtension);

	void   GetDHParam( int, DHParaBase* );
	void   set_start_pos( const AxisPos& );
}


#endif