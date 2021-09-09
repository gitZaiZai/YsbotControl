#pragma once
#include "PopupListCtrl.h"
#include "PopupMFCGrid.h"
#include "PopupDlgCtrl.h"

#include <atlimage.h>
#include <afxcontrolbars.h>     // MFC support for ribbons and control bars

// CCstmEditCombo
#define  WM_CUSTOMCOMBOX_SELCHANGED  (WM_USER+101)

class CCstmEditCombo : public CEdit
{
	DECLARE_DYNAMIC(CCstmEditCombo)

public:
	CCstmEditCombo();
	virtual ~CCstmEditCombo();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
};


// CCstmComboBox

class CCstmComboBox		:public CComboBox
{
	DECLARE_DYNAMIC(CCstmComboBox)

public:
	CCstmComboBox();
	virtual ~CCstmComboBox();
	//设置字体大小及类型  
	void SetFontType(int fontSize, CString fontType);  
	void SetShowImage( BOOL bShow = TRUE ) { m_bIsShowImage = bShow; }
	BOOL IsShownImage( ) { return m_bIsShowImage; }
	void SetAlignTop(BOOL bTopAlign) { m_bTopAlign = bTopAlign; }

	enum e_popup_type
	{
		__CCB_STANDARD	= 0,
		__CCB_LIST_CTRL	= 1,
		__CCB_MFC_GRID  = 2,
		__CCB_DIALOG  = 4,
	};
	
protected:
	virtual int			CompareItem(LPCOMPAREITEMSTRUCT lpCompareItemStruct);
	virtual void		DeleteItem(LPDELETEITEMSTRUCT lpDeleteItemStruct);
	virtual void		DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void		MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	virtual void		PreSubclassWindow();
	DECLARE_MESSAGE_MAP()
protected:
	UINT				m_nHeight;
	PCOMBOBOXINFO		m_pcbInfo;
	CWnd*				m_lstPopup;
	CCstmEditCombo		m_wndEditCombo;
	bool				m_bAutoComplete;

 	CImage m_imgComboBoxThump;
	HICON  m_hItemIcon[3];
	CFont  m_font;
	BOOL   m_bIsShowImage;
	BOOL   m_bTopAlign;
protected:
	bool InitCombo(void);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnPaint();
	afx_msg void OnCbnCloseup();
	afx_msg void OnCbnDropdown();

//Implementation
public:
	bool SetCustomPopupCtrl(e_popup_type _type  );
	CWnd* GetPopupCtrl(void);
	int SetDroppedHeight(UINT nHeight);
	void EnsureVisible(int nItem);
	virtual void Draw(CDC *pDC);
 
	int GetCount( ) const;
	int GetCurSel( ) const;
	int SetCurSel(int nSelect);
	void GetDroppedControlRect(LPRECT lprect) const;
	int GetLBText(int nIndex,LPTSTR lpszText) const;
	void GetLBText(int nIndex,CString& rString) const;
	int GetLBTextLen(int nIndex) const;
	int AddString(LPCTSTR lpszString);
	int DeleteString(UINT nIndex);
	int Dir(UINT attr,LPCTSTR lpszWildCard);
	int FindString(int nStartAfter,LPCTSTR lpszString) const;
	int FindStringExact(int nIndexStart,LPCTSTR lpszFind) const;
	int InsertString(int nIndex,LPCTSTR lpszString);
	int SelectString(int nStartAfter,LPCTSTR lpszString);
	void ResetContent( );
	int SetItemData(int nIndex, DWORD_PTR dwItemData) ;
	DWORD_PTR GetItemData(int nIndex) const;
	virtual HICON get_icon ( DWORD ); // other image maybe use, so overloading this fuction to show different image. liurundan 


public:
	virtual BOOL OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};


