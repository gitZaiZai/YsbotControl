// GridCellCheck.cpp : implementation file
//
// MFC Grid Control - Main grid cell class
//
// Provides the implementation for a combobox cell type of the
// grid control.
//
// Written by Chris Maunder <chris@codeproject.com>
// Copyright (c) 1998-2005. All Rights Reserved.
//
// Parts of the code contained in this file are based on the original
// CInPlaceList from http://www.codeguru.com/listview
//
// This code may be used in compiled form in any way you desire. This
// file may be redistributed unmodified by any means PROVIDING it is 
// not sold for profit without the authors written consent, and 
// providing that this notice and the authors name and all copyright 
// notices remains intact. 
//
// An email letting me know how you are using it would be nice as well. 
//
// This file is provided "as is" with no expressed or implied warranty.
// The author accepts no liability for any damage/loss of business that
// this product may cause.
//
// For use with CGridCtrl v2.22+
//
// History:
// 23 Jul 2001 - Complete rewrite
// 13 Mar 2004 - GetCellExtent and GetCheckPlacement fixed by Yogurt
//             - Read-only now honoured - Yogurt
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "../GridCtrl_src/GridCell.h"
#include "../GridCtrl_src/GridCtrl.h"

#include "GridCellCheck.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CGridCellCheck, CGridCell)

CGridCellCheck::CGridCellCheck() : CGridCell()
{
	m_bChecked = FALSE;
	//m_Rect.IsRectNull();
}

CSize CGridCellCheck::GetCellExtent(CDC* pDC)
{
	// Using SM_CXHSCROLL as a guide to the size of the checkbox
	int nWidth = GetSystemMetrics(SM_CXHSCROLL) + 2*GetMargin();
	CSize cellSize = CGridCell::GetCellExtent(pDC);
	cellSize.cx += nWidth;
	cellSize.cy = max (cellSize.cy, nWidth);
	return cellSize;
}

// i/o: i=dims of cell rect; o=dims of text rect
BOOL CGridCellCheck::GetTextRect(LPRECT pRect)
{
	BOOL bResult = CGridCell::GetTextRect(pRect);
	return bResult;
}

// Override draw so that when the cell is selected, a drop arrow is shown in the RHS.
BOOL CGridCellCheck::Draw(CDC* pDC, int nRow, int nCol, CRect rect, BOOL bEraseBkgnd /*=TRUE*/)
{
	// Store the cell's dimensions for later
	m_Rect = rect;
	CRect CheckRect = GetCheckPlacement();
	rect.left = CheckRect.right;
 
 	BOOL bResult = CGridCell::Draw(pDC, nRow, nCol, rect, bEraseBkgnd);
  
#ifndef _WIN32_WCE

	// enough room to draw?
	// if (CheckRect.Width() < rect.Width() && CheckRect.Height() < rect.Height()) {

	// Do the draw
	pDC->DrawFrameControl(CheckRect, DFC_BUTTON, m_bChecked ? DFCS_BUTTONCHECK | DFCS_CHECKED : DFCS_BUTTONCHECK); // liurundan

// 	if(CheckRect.right < rect.right && CheckRect.bottom < rect.bottom)
// 	{
// 		pDC->DrawFrameControl(GetCheckPlacement(), DFC_BUTTON, 
// 			(m_bChecked)? DFCS_BUTTONCHECK | DFCS_CHECKED : DFCS_BUTTONCHECK);
// 	}
	// }
#endif
	return bResult;
}

// JGW
BOOL CGridCellCheck::Edit(int nItem, int nSubItem, CRect rect, CPoint point, UINT nID, UINT nChar)
{
	BOOL bRet = FALSE;
	CRect rcCheck = GetCheckPlacement();
	BOOL bClick = nChar == VK_LBUTTON && point.x > -1;

	if (!bClick)
	{
		rect.left = rcCheck.right + 1;
		bRet = CGridCell::Edit(nItem, nSubItem, rect, point, nID, nChar);
	}
	else
	{
		if (IsSelected())
		{
			CPoint pt(point.x + rect.left, point.y + rect.top);
			if (rcCheck.PtInRect(pt))
			{
				SetCheck(!GetCheck());
				bRet = TRUE;
			}
			else
			{
				rect.left = rcCheck.right + 1;
				bRet = CGridCell::Edit(nItem, nSubItem, rect, point, nID, nChar);
			}
		}
	}

	return bRet;
}

void CGridCellCheck::OnClick(CRect cellRect, CPoint PointCellRelative)
{
	/*
	// PointCellRelative is relative to the topleft of the cell. Convert to client coords
	PointCellRelative += m_Rect.TopLeft();

	// Bail if cell is read-only
	CCellID cell = GetGrid()->GetCellFromPt(PointCellRelative);
	if (!GetGrid()->IsCellEditable(cell))
	return;

	// GetCheckPlacement returns the checkbox dimensions in client coords. Only check/
	// uncheck if the user clicked in the box
	if (GetCheckPlacement().PtInRect(PointCellRelative))
	{
	m_bChecked = !m_bChecked;
	GetGrid()->InvalidateRect(m_Rect);
	}
	*/

	// JGW
	if (IsSelected())
	{
		CRect rcCheck = GetCheckPlacement();
		CPoint pt(PointCellRelative.x + cellRect.left, PointCellRelative.y + cellRect.top);
		if (rcCheck.PtInRect(pt))
		{

			PointCellRelative += m_Rect.TopLeft();
			CCellID cell = GetGrid()->GetCellFromPt(PointCellRelative);
			if ( GetGrid()->GetItemState( cell.row, cell.col) & GVIS_READONLY )
			{
				return;
			}
			SetCheck(!GetCheck());

			LRESULT res = GetGrid()->SendMessageToParent( cell.row,cell.col,GVN_ENDLABELEDIT); // liurundan
		}
	}
}

//////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////

BOOL CGridCellCheck::SetCheck(BOOL bChecked /*=TRUE*/)
{
	BOOL bTemp = m_bChecked;
	m_bChecked = bChecked;
	if (!m_Rect.IsRectEmpty())
		GetGrid()->InvalidateRect(m_Rect);

	return bTemp;
}

BOOL CGridCellCheck::GetCheck()
{
	return m_bChecked;
}

//////////////////////////////////////////////////////////////////////
// Protected implementation
//////////////////////////////////////////////////////////////////////

//Returns the dimensions and placement of the checkbox in client coords.

CRect CGridCellCheck::GetCheckPlacement()
{
	int nWidth = GetSystemMetrics(SM_CXHSCROLL)+20;
	CRect place = m_Rect + CSize(GetMargin(), GetMargin());
  	place.right = place.left + nWidth;
 	place.bottom = place.top + nWidth;
 
	//JGW ¸ÄÎª¾ÓÖÐ
	//if (m_Rect.Height() < nWidth + 2 * static_cast<int> (GetMargin()))
	//{
	place.top    = m_Rect.top + (m_Rect.Height() - nWidth) / 2;
	place.bottom = place.top + nWidth;
	place.left  += 210;
	place.right += 235;
	//}

	// Draw control at RHS of cell liurundan
	CRect ScrollRect  = m_Rect;
	ScrollRect.left   = m_Rect.right - nWidth; // liurundan
	ScrollRect.bottom = m_Rect.top   + nWidth;
	 
	return ScrollRect;
}

 