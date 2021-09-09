#if !defined(AFX_SKINSCROLLWND_H__891A5F44_7A55_46DF_8BD7_9C454283CE40__INCLUDED_)
#define AFX_SKINSCROLLWND_H__891A5F44_7A55_46DF_8BD7_9C454283CE40__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SkinScrollWnd.h : header file
//
 
#include "SkinScrollBar.h"
#define UM_DESTMOVE	(WM_USER+100)

enum SkinWindowKind
{
	SKIN_WINDOW_VARIABLE  = 0,
	SKIN_WINDOW_PROGRAM   = 1,
	SKIN_WINDOW_USERLIST  = 2,
};
/////////////////////////////////////////////////////////////////////////////
// CSkinScrollWnd window

class CSkinScrollWnd : public CWnd
{
// Construction
public:
	CSkinScrollWnd();

// Attributes
public:
	int			m_nScrollWid;
	CSkinScrollBar	m_sbHorz,m_sbVert;
	CWnd		m_wndLimit;
	HBITMAP		m_hBmpScroll;
	BOOL		m_bOp;
	int			m_nAngleType;

	int         m_bar_top_offset;
	CWnd*       m_parent;
	CWnd*       m_selfcwnd;
	bool        m_bRedrawWnd;
	SkinWindowKind m_nWndKind;

	BOOL        m_bVisibleSB;
	BOOL        m_bVisibleSBLast;

// Operations
public:
	WNDPROC		m_funOldProc;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSkinScrollWnd)
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	BOOL SkinWindow(CWnd *pWnd,HBITMAP hBmpScroll,int bar_top_offset = 30, SkinWindowKind nWndKind = SKIN_WINDOW_VARIABLE);
	BOOL SetSkinScrollInfo(int nBar,LPSCROLLINFO lpScrollInfo, BOOL bRedraw = TRUE, BOOL bIsVisible = TRUE );
	BOOL GetSkinScrollInfo(int nBar,LPSCROLLINFO lpScrollInfo, UINT nMask = 23 );
	void SetWindowStyle(UINT);

	virtual ~CSkinScrollWnd();

	// Generated message map functions
protected:
	afx_msg LRESULT OnDestMove(WPARAM wParam,LPARAM lParam);
	//{{AFX_MSG(CSkinScrollWnd)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT nIDEvent);

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

extern "C" BOOL UnskinWndScroll(CWnd *pWnd);

extern "C" CSkinScrollWnd* SkinWndScroll(CWnd *pWnd,HBITMAP hBmpScroll,int bar_top_offset = 30, SkinWindowKind nWndKind = SKIN_WINDOW_VARIABLE);
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SKINSCROLLWND_H__891A5F44_7A55_46DF_8BD7_9C454283CE40__INCLUDED_)
