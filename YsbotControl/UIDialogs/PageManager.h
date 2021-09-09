#if !defined(AFX_PAGEMANAGER_H__46313796_4A01_4EA8_A726_8F895FF14541__INCLUDED_)
#define AFX_PAGEMANAGER_H__46313796_4A01_4EA8_A726_8F895FF14541__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PageManager.h : header file
//
#define  WINNONE    (-1)
/////////////////////////////////////////////////////////////////////////////
// CPageManager window

class CPageManager : public CStatic
{
	enum { MAX_PAGE_NUM = 64 };

	// Construction
public:
	CPageManager();
	int		m_nCurSelPage;

	// Attributes
protected:
	CWnd*	m_pwndZOrderAfter;
	UINT	m_uPageNum;
	CWnd**	m_ppPages;
	LPARAM  m_lParam;

	// Operations
public:
	void	ClearAllPages(void);
	UINT	InsertPage(CWnd* pwndPage, UINT nIDTemplate = 0);
	UINT    InsertPage(int nIndex, CWnd* pwndPage, UINT nIDTemplate = 0);

	CWnd*	SelectPage(int nIndex, LPARAM lp = 0 ); // zero-based
	CWnd*	GetPage(int nIndex) const;
	int		GetPageIndex(const CWnd* pwndPage) const;
	CWnd*	GetCurSelPage(void) const;
	int		GetCurSelPageIndex(void) const;
	LPARAM  GetCurSelPageLParam(void) const;
	UINT	GetPageCount(void) const;
	void	EnableAllChildWindow(int nIndex, BOOL bEnable);

	void    BringToTopShow(int nIndex);

private:
	void	SizePage(CWnd* pwndPage);

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPageManager)
protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

	// Implementation
public:
	virtual ~CPageManager();

	// Generated message map functions
protected:
	//{{AFX_MSG(CPageManager)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
};

 

#endif 