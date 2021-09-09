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
#include "SecondMenuDlg.h"
#include "WindowButtonMenu.h"
#include "PageManager.h"
 
#define  NUM_BUTTON  (NUM_MAIN_BUTTON+1)

class CMainMenuDlg : public DialogType 
{
public:
	CMainMenuDlg( );
	virtual ~CMainMenuDlg();
	void OnClickedFromPendant (UINT id);
	virtual int OnActivateChildWin(LPARAM);

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);
	DECLARE_MESSAGE_MAP()

	afx_msg LRESULT OnActiveWindow(WPARAM,LPARAM); 
	afx_msg void OnMainButtonClicked(UINT id);
	afx_msg void OnSecondButtonClicked(UINT id);


private:
	int m_nRowMain;
	int m_nColMain;
	int m_nRowSecondMenu;
	int m_nColSecondMenu;
	int m_nmainBtnIDActive;

	CRect clientRect;
	std::vector<CButtonST*> main_button_list;
	std::vector<CButtonST*> second_button_list;
	std::vector<CRect*>     main_rect_list;
	std::vector<CRect*>     second_rect_list;

	CSecondMenuDlg* second_menu_dlg;
	CPageManager* second_page;
	MainButtonMenu*  button_menu_info;
	COLORREF maskColor;

    HICON m_hIcon[NUM_BUTTON];
	HICON m_hIconSecondBk;

	CFont m_font;

 	void creat_main_button (); 
	void creat_second_button (); 
	void UpdateSurface();

public:
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};