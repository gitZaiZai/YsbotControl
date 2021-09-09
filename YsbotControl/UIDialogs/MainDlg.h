// Copyright 2016, 亚思信息科技有限责任公司
// All right reserved.
//
// Author: Liu Run Dan ( lrdcz@163.com )
//
// Yasi Robot Motion Control System - 亚思机器人运动控制系统

#pragma once

#include "DialogType.h"
#include "../ConfigReader.h" 
#include "../IO Dialog/GroupFrame.h"
#include "state_dialog.h"
#include "right_dialog.h"
#include "bottom_dialog.h"
#include "InfoBox.h"
#include "PageManager.h"
#include "MainMenuDlg.h" 

#include "JournalDLG.h"
#include "ProjectManager.h"
#include "SettingUserDLG.h"
#include "DlgParameterSetting.h"
#include "../SettingDialog/SettingDlg_Joints.h"
#include "../SettingDialog/SettingDlg_Mechanism.h"
#include "../SettingDialog/SettingDlg_Origin.h"
#include "../UserInterface/DialogUserInterface.h"
#include "TeachPanelDlg.h"

class CMainDlg : public DialogType 
{
public:
	CMainDlg( const ConfigReader& );
	virtual ~CMainDlg();
	bool isExitSystem();
	bool isRebootSystem() { return restart_system; }

	void update_window();


protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);
	DECLARE_MESSAGE_MAP()

	afx_msg LRESULT OnMainMenuClicked(WPARAM,LPARAM); 
 	afx_msg LRESULT OnStateDlgConfirmMsg(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnActiveWindow(WPARAM,LPARAM); 
	afx_msg LRESULT OnRebootSystem(WPARAM,LPARAM); 
	afx_msg LRESULT OnSaveProject(WPARAM,LPARAM); 
	afx_msg LRESULT On2NDToggle(WPARAM,LPARAM); 
	afx_msg LRESULT OnGetHomePosition(WPARAM,LPARAM); 
	afx_msg LRESULT OnUpdateBottomButton(WPARAM,LPARAM); 
	afx_msg LRESULT OnLeftKeyPressFromPendant(WPARAM,LPARAM); 
	afx_msg LRESULT OnSetPCKeyData(WPARAM,LPARAM); 
	void destroy_child_window(CWnd* pWnd);

private:
	const ConfigReader& configuration_list;
	enum { BOX_STATE, BOX_MAIN, BOX_RIGHT, BOX_BUTTONS, BOX_MENUDLG }; 	 

 	std::vector<CRect> boxRect;

	CPageManager*    m_PageManager[5];

 	StateDialog*          state_dialog_;    // 状态栏窗口
	RightDialog*          right_dialog_;    // 右侧窗口
	BottomDialog*         bottom_dialog_;  // 下方窗口
	DlgNewVariable*       m_pwndNewVariable;
	DlgParameterSetting*  m_pwndDlgSetting;

	PopupMenu*            main_popup_menu_;
	MainButtonMenu*       button_menu_info;
	CMainMenuDlg*         main_menu_dlg;
	Setting_JointsDLG*    m_pwndDlgSettingJoint;
	Setting_MechanismDLG* m_pwndDlgSettingMechanism;
	Setting_OriginDLG*    m_pwndDlgSettingOrigin;
	DialogUserInterface*  the_user_interface;
	CInfoBox*		      theInfoBox;
	CTeachPanelDlg*       m_pwndDlgTeachPanel;

	HBITMAP	m_hBmpScrollBar;
	HBITMAP	m_hBmpPlus;				// bitmap 
	HBITMAP	m_hBmpMinus;		
	std::string home_path;

	bool    m_exit_system;
	bool    restart_system;

	int     m_nLastSelPage;
	LPARAM  m_nLastLPARAM;
	LPARAM  m_nCurLPARAM;

	bool    m_bIsUbuntuWineRuntime;
	int     m_iTopBarOffset;

	void creat_fixed_window();
	void creat_child_window();

	void ActiveDialog( int dialog_id, LPARAM lParam = 0 ); 
	void load_all_bitmap ();
	void exit_system ();
	int  GetCurUserLevel ();

};