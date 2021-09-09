// Copyright 2016, 亚思信息科技有限责任公司
// All right reserved.
//
// Author: Liu Run Dan ( lrdcz@163.com )
//
// Yasi Robot Motion Control System - 亚思机器人运动控制系统

#pragma once

#include "DialogType.h"
#include "../ConfigReader.h" 
#include "../BasicControls/BtnST.h"
 

class CSecondMenuDlg : public DialogType 
{
public:
	CSecondMenuDlg( );
	virtual ~CSecondMenuDlg();

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);
	DECLARE_MESSAGE_MAP()

 	afx_msg void OnSecondButtonClicked(UINT id);


private:
	CRect clientRect;
	std::vector<CButtonST*> second_button_list;

	void creat_second_button (); 

};