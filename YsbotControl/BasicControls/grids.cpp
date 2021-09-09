#include "stdafx.h"
#include "grids.h"



BEGIN_MESSAGE_MAP( BasicGrid, CListCtrl )
    ON_WM_MEASUREITEM_REFLECT()
END_MESSAGE_MAP()

BasicGrid::BasicGrid() 
{
	font_.CreateFont(21, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
		OUT_STROKE_PRECIS, CLIP_STROKE_PRECIS, DRAFT_QUALITY, FIXED_PITCH |
		FF_MODERN, _T("Arial"));

	m_nItemHeight = 40;
}

void BasicGrid::SetMyFont() {
	SetFont(&font_);
}

void BasicGrid::SetItemHeight(UINT nHeight)
{
	m_nItemHeight = nHeight;
 
}

void BasicGrid::PreSubclassWindow()
{
// 	CListCtrl::PreSubclassWindow();

 	CRect rcwin;  
	GetWindowRect(rcwin);  

	ModifyStyleEx(0, LVS_OWNERDRAWFIXED);

	WINDOWPOS wp;  
	wp.hwnd=m_hWnd;  
	wp.cx=rcwin.Width();  
	wp.cy=rcwin.Height();  
	wp.flags=SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOOWNERZORDER|SWP_NOZORDER;  
	SendMessage(WM_WINDOWPOSCHANGED,0,(LPARAM)&wp);  

}

void BasicGrid::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	if( m_nItemHeight > 0 )
		lpMeasureItemStruct->itemHeight = m_nItemHeight;
}

BasicTree::BasicTree() {
	font_.CreateFont(22, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
		OUT_STROKE_PRECIS, CLIP_STROKE_PRECIS, DRAFT_QUALITY, FIXED_PITCH |
		FF_MODERN, _T("Arial"));

}

void BasicTree::SetMyFont() {
	SetFont(&font_);
}