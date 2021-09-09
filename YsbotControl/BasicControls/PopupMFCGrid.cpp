// PopupListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "cstmComboBox.h"
#include "PopupMFCGrid.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CPopupMFCGid

IMPLEMENT_DYNCREATE(CPopupMFCGid, CGridCtrl)
CPopupMFCGid::CPopupMFCGid()
{
	m_nKeyColumn=0;
}

CPopupMFCGid::~CPopupMFCGid()
{
}


BEGIN_MESSAGE_MAP(CPopupMFCGid, CGridCtrl)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()



// CPopupMFCGid message handlers
LRESULT CPopupMFCGid::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch( message )
	{
	case WM_PRINT:
		{
			message=WM_PAINT;
			break;
		}

	}
	return CGridCtrl::WindowProc(message, wParam, lParam);
}
void CPopupMFCGid::OnLButtonDown(UINT nFlags, CPoint point)
{
	CGridCtrl::OnLButtonDown(nFlags, point);	
}

bool CPopupMFCGid::CreatePopupCtrl(CWnd* pOwner)
{
	
	if(Create(CRect(0,0,0,0),GetDesktopWindow(),0x123,WS_CHILD | WS_BORDER | WS_TABSTOP)) // GetDesktopWindow()
	{
		ModifyStyle(WS_OVERLAPPED ,WS_CLIPSIBLINGS );
		ModifyStyleEx(WS_EX_CLIENTEDGE|WS_EX_STATICEDGE,WS_EX_TOOLWINDOW);

		SetEditable(FALSE);
		SetSingleRowSelection(TRUE);
		SetFixedRowSelection(FALSE);
		SetFixedColumnSelection(FALSE);
		SetHeaderSort(); 
		SetListMode();

		if(pOwner)
			SetOwner(pOwner);
 
	    return true;
	}
	return false;
}

void CPopupMFCGid::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if(!m_idCurrentCell.IsValid())
		m_idCurrentCell.col=0,m_idCurrentCell.row=0;
	CGridCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
	if(nChar==VK_UP || nChar|VK_DOWN)
	{
		WPARAM wParam= MAKEWPARAM( GetOwner()->GetDlgCtrlID(),CBN_SELCHANGE);
		LPARAM lParam=(LPARAM)GetOwner()->m_hWnd;
		GetOwner()->PostMessage(WM_COMMAND,wParam,lParam);
	}
}

void CPopupMFCGid::OnLButtonUp(UINT nFlags, CPoint point)
{
	CGridCtrl::OnLButtonUp(nFlags, point);
	//If Mouse in Cell but Not in Fixed Row/Col Area
	CRect rect;
	GetClientRect(rect);
	rect.top+=GetFixedRowHeight();
	rect.left+=GetFixedColumnWidth();

	if(rect.PtInRect(point))
	{
		if(((CCstmComboBox*)GetOwner())->GetDroppedState())
			((CCstmComboBox*)GetOwner())->ShowDropDown(FALSE);

		WPARAM wParam= MAKEWPARAM( GetOwner()->GetDlgCtrlID(),CBN_SELCHANGE);
		LPARAM lParam=(LPARAM)GetOwner()->m_hWnd;
		GetOwner()->PostMessage(WM_COMMAND,wParam,lParam);

	}
}

int CPopupMFCGid::GetCount(void) const
{
	return GetRowCount();
}
int CPopupMFCGid::GetCurrentSelection( ) const
{
	return GetFocusCell().row;
}

int CPopupMFCGid::SetCurrentSelection(int nSelect)
{
	CCellID cell(-1,-1);
	if(nSelect==-1)
	{
		cell=GetFocusCell();
		if(cell.IsValid())
		{
			SetItemState(cell.row, cell.col,
				GetItemState(cell.row, cell.col) & ~GVIS_FOCUSED);
			for(int i=0;i<GetColumnCount();i++)
			SetItemState(cell.row, i,
				GetItemState(cell.row, i) & ~GVIS_SELECTED);
			Invalidate();
		}
		
	}
	else
	{
		cell.row=nSelect;
		cell.col=m_nKeyColumn;
		SelectRows(cell);
		cell=SetFocusCell(nSelect,m_nKeyColumn);
	}
	
	return cell.row;
}

void CPopupMFCGid::GetItemText(int nIndex,CString& rString) const
{
	rString= ((CGridCtrl*)this)->GetItemText(nIndex,m_nKeyColumn);
}

int CPopupMFCGid::DeleteItem(UINT nIndex)
{
	return ((CGridCtrl*)this)->DeleteRow(nIndex);
}

int CPopupMFCGid::FindString(int nStartAfter,LPCTSTR lpszString) const
{
	if(nStartAfter==-1)
		nStartAfter=0;
	CString strText;
	CString strdefText=lpszString;
	for(int i=nStartAfter;i<GetRowCount();i++)
	{
		strText=((CGridCtrl*)this)->GetItemText(i,m_nKeyColumn);
		if(strText.Find(lpszString)==0)
		{
			return i;
		}
	}

	return CB_ERR ;
}

int CPopupMFCGid::FindStringExact(int nStartAfter,LPCTSTR lpszString) const
{
	if(nStartAfter==-1)
		nStartAfter=0;
	CString strText;
	CString strdefText=lpszString;
	for(int i=nStartAfter;i<GetRowCount();i++)
	{
		strText=((CGridCtrl*)this)->GetItemText(i,m_nKeyColumn);
		if(strText==strdefText)
		{
			return i;
		}
	}

	return CB_ERR ;
}

void CPopupMFCGid::RemoveAllItems()
{
	((CGridCtrl*)this)->DeleteAllItems();
}
