// Copyright 2016, 亚思信息科技有限责任公司
// All right reserved.
//
// Author: Liu Run Dan ( lrdcz@163.com )
//
// Yasi Robot Motion Control System - 亚思机器人运动控制系统

#pragma once

#include "UIDialogs/DialogType.h"
#include "UIDialogs/state_dialog.h"
#include "UIDialogs/right_dialog.h"
#include "UIDialogs/bottom_dialog.h"
#include "program_dialog.h"
#include "location_manager.h"
#include "variable_manager.h"
#include "UIDialogs/WindowButtonMenu.h"
#include "DlgNewVariable.h"
#include "DlgDummy.h"
#include "UIDialogs/popup_menu.h"
#include "UIDialogs/WindowButtonMenu.h"
#include "IO Dialog/InOutDialog.h"
#include "IO Dialog/SettingIODlg.h"
#include "UIDialogs/ProjectManager.h"
#include "ConfigReader.h"
#include "SettingDialog/SettingDlg_Joints.h"
#include "SettingDialog/SettingDlg_Mechanism.h"
#include "SettingDialog/SettingDlg_Origin.h"
#include "UIDialogs/DlgParameterSetting.h"
#include "UserInterface/DialogUserInterface.h"

#include "TeachDialog/TeachDlg.h"
#include "UIDialogs/UserCoSysDLG.h"
#include "UIDialogs/ToolCoSysDLG.h"
#include "UIDialogs/SecurityAreaDLG.h"
#include "UIDialogs/SettingUserDLG.h"
#include "PalletDLG.h"
#include "InstructionBuilderDLG.h"
#include "InstructionEditDLG.h"
#include "logDialog.h"
#include "VariableBuilderDLG.h"
#include "UIDialogs/InfoBox.h"
#include "UIDialogs/DlgScriptCommandEdit.h"
 
class LeftDialog : public DialogType {
public:
	LeftDialog( const ConfigReader& );
	virtual ~LeftDialog();
	 
	void update_window();
	bool isExitSystem();
	bool isRebootSystem() { return restart_system; }

 	ProgramDialog* get_program_window() { return program_dialog; }
	int GetCurUserLevel();
	//void set_pc_keydata (unsigned int modifiers, unsigned int virtual_key) { the_user_interface->set_pc_keydata(modifiers,virtual_key); }
	//std::string GetHomePath() { return home_path;};

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);
	DECLARE_MESSAGE_MAP()
	afx_msg void Clicked(UINT id);
	afx_msg LRESULT OnStateDlgConfirmMsg(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMainButtonClicked(WPARAM,LPARAM); 
 	afx_msg LRESULT OnActiveWindow(WPARAM,LPARAM); 
	afx_msg LRESULT OnRebootSystem(WPARAM,LPARAM); 
	afx_msg LRESULT OnSaveProject(WPARAM,LPARAM); 
	afx_msg LRESULT On2NDToggle(WPARAM,LPARAM); 
	afx_msg LRESULT OnGetHomePosition(WPARAM,LPARAM); 
	afx_msg LRESULT OnUpdateBottomButton(WPARAM,LPARAM); 

	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSetFocus(CWnd* pOldWnd);

	void ActiveDialog( int dialog_id, LPARAM lParam = 0 ); 
	void creat_fixed_window();
	void creat_child_window();

	void load_all_bitmap() throw (InvalidConfigurationException);
	void destroy_child_window(CWnd*);

private:
 	CMFCButton left_button_[7];

	StateDialog*     state_dialog_;    // 状态栏窗口
 	RightDialog*     right_dialog_;    // 右侧窗口
	BottomDialog*    bottom_dialog_;  // 下方窗口
	DlgNewVariable*  m_pwndNewVariable;
 	DlgParameterSetting*       m_pwndDlgSetting;
 	LogDialog*       m_pwndDlgLog;
// 	InoutDialog*     m_pwndDlgInOut;
	PopupMenu*       main_popup_menu_;
	MainButtonMenu*  button_menu_info;
	Setting_JointsDLG*    m_pwndDlgSettingJoint;
	Setting_MechanismDLG* m_pwndDlgSettingMechanism;
	Setting_OriginDLG*    m_pwndDlgSettingOrigin;
	DialogUserInterface*  the_user_interface;
	CInfoBox*			  theInfoBox;

	void	ClearAllPages(void);
	UINT	InsertPage(int nIndex, CWnd* pwndPage, UINT nIDTemplate = 0);
	CWnd*	SelectPage(int nIndex, LPARAM lParam  = 0 ); // zero-based
	CWnd*	GetPage(int nIndex) const;
	int		GetPageIndex(const CWnd* pwndPage) const;
	CWnd*	GetCurSelPage(void) const;
	int		GetCurSelPageIndex(void) const;
	UINT	GetPageCount(void) const;
	void	EnableAllChildWindow(int nIndex, BOOL bEnable);
	void	SizePage(CWnd* pwndPage);

	void    exit_system();

	CWnd*	m_pwndZOrderAfter;
	int		m_nCurSelPage;
	UINT	m_uPageNum;
	CWnd**	m_ppPages;
	int     m_nLastSelPage;
	LPARAM  m_nLastLPARAM;
	LPARAM  m_nCurLPARAM;
	bool    m_exit_system;
	bool    restart_system;

	HBITMAP	m_hBmpScrollBar;
	HBITMAP	m_hBmpPlus;				// bitmap 
	HBITMAP	m_hBmpMinus;		

	std::string home_path;
	const ConfigReader& configuration_list;
 };