#pragma once

#include "../IO Dialog/GroupFrame.h"
#include "../BasicControls/buttons.h"
#include "../GridCtrl_src/TreeGridCtrl.h" 
#include "UserData.h"

#define IDC_SETTING_USERMANAGE             8010
#define IDC_SETTING_USERMANAGE_BTN_EDIT    8011
#define IDC_SETTING_USERMANAGE_BIN_NEW     8012
#define IDC_SETTING_USERMANAGE_BIN_DEL     8013
#define IDC_SETTING_USERMANAGE_GRID        8014
#define WM_BTN_USER_MANAGE                 8015

class CSetUserManage :	public CGroupFrame
{
public:
	CSetUserManage(HBITMAP hBmpScrollBar = NULL, 
		HBITMAP hBmpPlus  = NULL, 
		HBITMAP hBmpMinus = NULL );
	virtual ~CSetUserManage(void);
	BOOL Create(CString caption, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd=NULL, UINT nID=65535);
	int insert_user_data (const UserData&);
	int delete_user_data ();
	BOOL load_user_data();

protected:

	DECLARE_MESSAGE_MAP()
	afx_msg void OnButtonClicked(UINT id);
	afx_msg LRESULT OnButtonPopup(WPARAM Wparam,LPARAM Lparam);	
	afx_msg LRESULT OnResizeGrid(WPARAM wp,LPARAM lp);

private:
 
	BasicButton m_button[3];
	unsigned int m_nID;
	CFont font_;
	
	CGridCtrl m_Grid;

	HBITMAP m_hBmpScrollBar;
	HBITMAP m_hBmpPlus;
	HBITMAP m_hBmpMinus;

	CImageList image_list_;
	int overlay_icon ;
	int     admin_icon;
	int     normal_icon;
	CRect   clientRect;
	CRect   m_RectBtn;

	BOOL create_tree_grid();

};
