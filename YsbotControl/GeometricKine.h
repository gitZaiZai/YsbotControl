// Copyright 2016, 亚思信息科技有限责任公司
// All right reserved.
//
// Author: Liu Run Dan ( lrdcz@163.com )
//
// Yasi Robot Motion Control System - 亚思机器人运动控制系统

#ifndef geometric_kinematic_h
#define geometric_kinematic_h

#include "ConfigReader.h"
#include "KinematicsBase.h"

namespace Ysbot
{

int init_kine_geometric( const ConfigReader& ) throw (InvalidConfigurationException);
void set_kine_axis_num(int);

BOOL __stdcall Geometric_Fwk(int nGroup,
	int nSynAxisNum,
	const double* pdfFrom,
	double* pdfTo,
	DWORD& dwPosture,
	void* pvExtension);

BOOL __stdcall Geometric_Ink(int nGroup,
	int nSynAxisNum,
	const double* pdfFrom,
	const double* pdfPrevResult,
	double* pdfTo,
	DWORD& dwPosture,
	void* pvExtension);

// for HMI change dh parameters.
void   Set6DofDHParam(const DH6dofParam& dhp);
void   Get6DofDHParam(DH6dofParam* pdhp);

void get_extend_dhpara(DHExParam* exdh);

//EXTENSION extension;

}

#endif