#pragma once

#include "dialog_template.h"
#include "WindowButtonMenu.h"
// DialogType dialog
 
#ifndef SAFE_DELETE
#define SAFE_DELETE( x ) \
	if ( NULL != x ) \
{ \
	delete x; \
	x = NULL; \
}
#endif

class DialogType : public CDialogEx
{
	DECLARE_DYNAMIC(DialogType)

public:
	DialogType();   // standard constructor
	DialogType(UINT uResourceID, CWnd* pParent);
	DialogType(LPCTSTR pszResourceID, CWnd* pParent);

	virtual ~DialogType();

	virtual BOOL CreateModeDlg(CString caption, CRect rect, BOOL mode = TRUE,
		CWnd *parent = NULL, DWORD style = WS_POPUP | WS_CHILD | WS_BORDER );

	virtual BOOL CreateDialogFromTemplate(  CWnd *parent = NULL,                   // 父窗口
		BOOL mode = FALSE,                     // 模态
		DWORD style = WS_POPUP | WS_DLGFRAME,  // 风格
		LPCSTR caption = "",                   // 标题
		DWORD style_ex = 0                     // 扩展风格
		); 
	 DLGTEMPLATE* CreateTemplate(DWORD dwStyle,CRect& rect,CString strCaption,DWORD dwStyleEx=0);
	virtual afx_msg LRESULT OnMainButtonClicked(WPARAM,LPARAM) { return 0; } 
	virtual afx_msg LRESULT OnKeyboardEnter(WPARAM,LPARAM=NULL) { return 0; } 
	virtual afx_msg LRESULT OnKeyboardCancle(WPARAM,LPARAM) { return 0; } 
	virtual afx_msg LRESULT OnResizeGrid(WPARAM,LPARAM) { return 0; } 
	CDlgTemplate dlg_temp;
	virtual int OnActivateChildWin(LPARAM) { return 0; } 
	 
	CRect clientRect;

// Dialog Data
// 	enum { IDD = IDD_DIALOGTYPE };
     
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support


	DECLARE_MESSAGE_MAP()

 

};
