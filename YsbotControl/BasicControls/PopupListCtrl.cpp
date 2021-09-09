// PopupListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "cstmComboBox.h"
#include "PopupListCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
// CPopupListCtrl

IMPLEMENT_DYNCREATE(CPopupListCtrl, CListCtrl)
CPopupListCtrl::CPopupListCtrl()
{
}

CPopupListCtrl::~CPopupListCtrl()
{
}

BEGIN_MESSAGE_MAP(CPopupListCtrl, CListCtrl)
	ON_WM_LBUTTONDOWN()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

bool CPopupListCtrl::CreatePopupCtrl(CWnd* pOwner)
{
	if(Create(WS_CHILD | WS_BORDER | WS_TABSTOP ,CRect(0,0,0,0),GetDesktopWindow(),0x123))
	{
		ModifyStyle(0,WS_CLIPSIBLINGS | LVS_SHOWSELALWAYS| WS_OVERLAPPED |
			LVS_REPORT | LVS_SINGLESEL | LVS_SHOWSELALWAYS 
			/*| LVS_NOCOLUMNHEADER*/
			);
		ListView_SetExtendedListViewStyle(m_hWnd, LVS_EX_GRIDLINES |LVS_EX_TRACKSELECT| LVS_EX_FULLROWSELECT);
		ModifyStyleEx(0, WS_EX_TOOLWINDOW);
		if(pOwner)
			SetOwner(pOwner);
		return true;
	}
	return false;
}
// CPopupListCtrl message handlers

LRESULT CPopupListCtrl::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch( message )
	{
		case WM_PRINT:
			{
				message=WM_PAINT;
				break;
			}
	}
	return CListCtrl::WindowProc(message, wParam, lParam);
}

void CPopupListCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	CListCtrl::OnLButtonDown(nFlags, point);
	
	if(((CCstmComboBox*)GetOwner())->GetDroppedState())
		((CCstmComboBox*)GetOwner())->ShowDropDown(FALSE);

	WPARAM wParam= MAKEWPARAM( GetOwner()->GetDlgCtrlID(),CBN_SELCHANGE);
	LPARAM lParam=(LPARAM)GetOwner()->m_hWnd;
	GetOwner()->PostMessage(WM_COMMAND,wParam,lParam);

}
void CPopupListCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{	
	CListCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
	if(nChar==VK_UP || nChar|VK_DOWN)
	{
			WPARAM wParam= MAKEWPARAM( GetOwner()->GetDlgCtrlID(),CBN_SELCHANGE);
			LPARAM lParam=(LPARAM)GetOwner()->m_hWnd;
			GetOwner()->PostMessage(WM_COMMAND,wParam,lParam);
	}

}

int CPopupListCtrl::GetCount(void)const
{
	return GetItemCount();
}
int CPopupListCtrl::GetCurrentSelection( ) const
{
	return GetNextItem(-1, LVIS_SELECTED);
}
int CPopupListCtrl::SetCurrentSelection(int nSelect)
{
	if(nSelect==-1)
		SetItemState(GetNextItem(-1, LVIS_SELECTED ), ~LVIS_SELECTED , LVIS_SELECTED );
	return SetItemState(nSelect, LVIS_SELECTED , LVIS_SELECTED);
}
void CPopupListCtrl::GetItemText(int nIndex,CString& rString) const
{
	rString= ((CListCtrl*)this)->GetItemText(nIndex,0);
}
int CPopupListCtrl::DeleteItem(UINT nIndex)
{	
	return ((CListCtrl*)this)->DeleteItem((int)nIndex);
}
int CPopupListCtrl::FindString(int nStartAfter,LPCTSTR lpszString) const
{
	LVFINDINFO info;
	info.flags = LVFI_PARTIAL|LVFI_STRING;
	info.psz = lpszString;
	return ((CListCtrl*)this)->FindItem(&info);
}
int CPopupListCtrl::FindStringExact(int nStartAfter,LPCTSTR lpszString) const
{
	LVFINDINFO info;
	info.flags = LVFI_STRING;
	info.psz = lpszString;
	return ((CListCtrl*)this)->FindItem(&info);
}
void CPopupListCtrl::RemoveAllItems()
{
	VERIFY(DeleteAllItems());
}