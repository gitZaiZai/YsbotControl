#pragma once
#include "DialogType.h"
#include "../IO Dialog/StaticTransparent.h"
#include "../IO Dialog/GroupFrame.h"
#include "../ConfigReader.h"
#include "SetLogin.h"
#include "SetSystem.h"
#include "SetControl.h"
#include "SetDisplay.h"
#include "SettingUserPopup.h"
#include "SetUserManage.h"

class CSettingUserDLG :public DialogType
{
public:
	CSettingUserDLG(const ConfigReader&, HBITMAP hBmpScrollBar = NULL, 
		HBITMAP hBmpPlus  = NULL, 
		HBITMAP hBmpMinus = NULL );
	virtual ~CSettingUserDLG();
	CSettingUserPopup* pSetUserPopup;
    void update_userdlg_show ();

protected:
	virtual BOOL OnInitDialog();
 
	DECLARE_MESSAGE_MAP()

	afx_msg LRESULT OnMainButtonClicked(WPARAM,LPARAM); 
	afx_msg LRESULT OnButtonLogin(WPARAM Wparam,LPARAM Lparam);	
	afx_msg LRESULT OnButtonManage(WPARAM Wparam,LPARAM Lparam);	
	afx_msg LRESULT OnButtonPopupYesNo(WPARAM Wparam,LPARAM Lparam);	

	virtual int OnActivateChildWin(LPARAM);

private:
    CGroupFrame* pSetBoard[5];

	HBITMAP	m_hBmpScrollBar;
	HBITMAP	m_hBmpPlus;				 
	HBITMAP	m_hBmpMinus;

	enum { winPageSetting, winPageUserManage };
	void update_ui_status( int );
	void UpdateSurface();

};

extern CSettingUserDLG*	  m_pwndDlgSettingUser;