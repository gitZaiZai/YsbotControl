#pragma once
#include "StaticTransparent.h"



extern void DrawGroupLine(CWindowDC *dc, COLORREF color, int x0, int y0, int x1,
	int y1);
extern void DrawGroupFrame(CWindowDC *dc, CString caption=_T(""), int r = 220,
	int g = 220, int b = 220,unsigned int fontHeight=20,CString fontStr=_T("Arial"));
extern void GroupTextOut(CWindowDC *dc,CString caption,CRect rect, COLORREF color=RGB(0,0,0),
	unsigned int FontHeigth=20,CString FontStr=_T("Arial"));


class CGroupFrame : public CStatic
{
public:
	DECLARE_DYNAMIC(CGroupFrame)
	CGroupFrame(BOOL bEnableTitle = TRUE);
	virtual ~CGroupFrame(void);

	virtual BOOL Create(CString caption, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd=NULL, UINT nID=65535);
	void SetFont(unsigned int fontHeight=18,CString fontStr=_T("Arial"));

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void PreSubclassWindow();
	afx_msg void DrawItem(LPDRAWITEMSTRUCT ); 
	virtual void Initialize(const RECT& rect, CWnd* pParentWnd, UINT nID);
	DECLARE_MESSAGE_MAP()

	CString caption_default;

private:

	CString caption_string;
	unsigned int _fontHeight;
	CString _fontStr;
	CBrush m_brush;
	BOOL m_enableTitle;
};
