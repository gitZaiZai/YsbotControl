// Copyright 2016, 亚思信息科技有限责任公司
// All right reserved.
//
// Author: Liu Run Dan ( lrdcz@163.com )
//
// Yasi Robot Motion Control System - 亚思机器人运动控制系统

#pragma once
#include <atlimage.h>
 

// CIconComboBox
typedef struct tagNM_COMBOXVIEW {
	NMHDR hdr;
	int   currSelIndex;
	DWORD data;
} NM_COMBOXVIEW;

class CIconComboBox : public CComboBox
{
	DECLARE_DYNAMIC(CIconComboBox)

public:
	CIconComboBox(UINT nLines = 6);
	virtual ~CIconComboBox();
	void set_custom_message ( BOOL b) { m_bIsCustomMessage = b; }
	void SetShowImage( BOOL bShow = TRUE ) { m_bIsShowImage = bShow; }
 	BOOL IsShownImage( ) { return m_bIsShowImage; }
	//设置字体大小及类型  
	void SetFontType(int fontSize, CString fontType);  
    void SetAlignTop(BOOL bTopAlign) { m_bTopAlign = bTopAlign; }
	UINT GetLineNumber() { return m_LinesToDisplay; }
	void SetSort(bool b) { m_sort = b; }
	void SetAllowPopupNested(BOOL b) { m_bAllowPopupNested = b; }

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg LRESULT OnMouseHover(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
	afx_msg void OnCbnSelchange();
	afx_msg LRESULT SendMessageToParent(int nIndex, DWORD dwData, int nMessage) const; // liurundan
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
  	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point); // ON_WM_LBUTTONDBLCLK

 	 
public:
	virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT /*lpMeasureItemStruct*/);
	virtual HICON get_icon ( DWORD ); // other image maybe use, so overloading this fuction to show different image. liurundan 

	static void set_DropDownHight(CComboBox& box, UINT LinesToDisplay);  
	static void set_DropDownWidth(CComboBox& box, UINT nWidth);  
 

public:
	int AddString(LPCTSTR lpszString, DWORD dwData = 0);
 
private:
	CImage m_imgComboBox;
  	CImage m_imgComboBoxThump;
	CImage m_imgMinClose;
	BOOL m_bIsThumpHover;
	BOOL m_bIsThumpDown;
	BOOL m_bTopAlign;

	CListBox* m_pListBox;
	HICON m_hItemIcon[3];
    BOOL m_bIsCustomMessage;
	BOOL m_bIsShowImage;

    CFont m_font;
    CListBox   m_lstBox;
    HWND	m_hWndList;
	HWND    m_hWndEdit;
	UINT    m_LinesToDisplay;
	bool    m_sort;
	BOOL    m_bAllowPopupNested;
	HBITMAP m_hBitmapBoxThump;
	HICON   m_hIconBoxThump;

	int     m_imgThumpWidth;

public:
	 
	virtual void PreSubclassWindow();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

};