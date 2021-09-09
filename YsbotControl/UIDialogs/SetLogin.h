#pragma once

#include "../IO Dialog/GroupFrame.h"
#include "../BasicControls/IconComboBox.h"
#include "../BasicControls/Label.h"
#include "../BasicControls/buttons.h"
#include "../IO Dialog/StaticTransparent.h"
#include "UserData.h"

#define IDC_SETTING_LOGIN         6010
#define IDC_SETTING_LOGIN_COMBBOX 6011
#define IDC_SETTING_LOGIN_BTN     6012
#define WM_BTN_USER_LOGIN         6013

class CSetLogin :	public CGroupFrame
{
public:
	CSetLogin(void);
	virtual ~CSetLogin(void);
	BOOL Create(CString caption, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd=NULL, UINT nID=65535);
	void add_user_combbox(const UserData&);
	void del_user_combbox(int nKeyPri);

protected:

	DECLARE_MESSAGE_MAP()
	afx_msg void OnButtonClicked();
	afx_msg LRESULT OnButtonPopup(WPARAM Wparam,LPARAM Lparam);	

private:
	CIconComboBox user_combbox;
	CStaticTran labels[2];
	CLabel userGradeNum;
	BasicButton btn_login;
	unsigned int m_nID;
	CFont font_;
	void set_grade(int);
};

