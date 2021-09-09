// CstmComboBox.cpp : implementation file
//

#include "stdafx.h"
#include "CstmComboBox.h"
#include "../resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CCstmEditCombo

IMPLEMENT_DYNAMIC(CCstmEditCombo, CEdit)
CCstmEditCombo::CCstmEditCombo()
{
	
}

CCstmEditCombo::~CCstmEditCombo()
{
	
}


BEGIN_MESSAGE_MAP(CCstmEditCombo, CEdit)
	ON_WM_LBUTTONDOWN()
	ON_WM_KEYDOWN()
	 
END_MESSAGE_MAP()

void CCstmEditCombo::OnLButtonDown(UINT nFlags, CPoint point)
{
	CEdit::OnLButtonDown(nFlags, point);
	if(!((CCstmComboBox*)GetOwner())->GetDroppedState())
		return;
	WPARAM wParam= MAKEWPARAM( (WORD) nFlags,0);
	LPARAM lParam=MAKELPARAM(point.x,point.y);
	GetOwner()->SendMessage(WM_LBUTTONDOWN,wParam,lParam);
}

void CCstmEditCombo::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch(nChar) {
	case VK_UP:
	case VK_DOWN:
		{
			GetOwner()->SendMessage(WM_KEYDOWN,(WPARAM)nChar,(LPARAM)nRepCnt);
			return;
		}
	case VK_F4:
		{
			GetOwner()->SendMessage(WM_KEYDOWN,(WPARAM)nChar,(LPARAM)nRepCnt);
			break;
		}
	default:
		break;
	}
	CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
}

//CCstmEditCombo implementation
IMPLEMENT_DYNAMIC(CCstmComboBox, CComboBox)
// CCstmEditCombo message handlers
CCstmComboBox::CCstmComboBox()
{
	m_pcbInfo=NULL;
	m_lstPopup=NULL;
	m_nHeight=150;
	m_bAutoComplete=true;
	for ( int i = 0; i < 3; ++i )
	{
		m_hItemIcon[i] = NULL;
	}
	m_bTopAlign    = FALSE;
	m_bIsShowImage = FALSE;
}

CCstmComboBox::~CCstmComboBox()
{
	m_font.DeleteObject();
	for ( int i = 0; i < 3; ++i )
	{
		DestroyIcon(m_hItemIcon[i]);
	}
	m_imgComboBoxThump.Destroy();
	if(m_pcbInfo!=NULL)
		delete m_pcbInfo;
	if(m_lstPopup)
		delete m_lstPopup;
}


BEGIN_MESSAGE_MAP(CCstmComboBox, CComboBox)
	ON_WM_CREATE()
  	ON_WM_LBUTTONDOWN()
	ON_WM_KEYDOWN()
	ON_WM_CTLCOLOR()
    ON_WM_PAINT()
	ON_CONTROL_REFLECT(CBN_DROPDOWN, OnCbnDropdown)
	ON_CONTROL_REFLECT(CBN_CLOSEUP, OnCbnCloseup)

END_MESSAGE_MAP()

int CCstmComboBox::SetDroppedHeight(UINT nHeight)
{
	if(nHeight>1)
		m_nHeight=nHeight;
	else
		return CB_ERR;
	return (int)m_nHeight;
}

bool CCstmComboBox::SetCustomPopupCtrl(e_popup_type _type )
{
	
	//Not Allow resetting custom Popup
	ASSERT(m_lstPopup==NULL);
	if(m_lstPopup!=NULL)
	return false;

	switch(_type) 
	{
	case __CCB_STANDARD:
		{
			return true;
		}
		
	case __CCB_LIST_CTRL:
		{
			m_lstPopup=new CPopupListCtrl();
			break;
		}
	case __CCB_MFC_GRID:
		{
			m_lstPopup=new CPopupMFCGid();
			break;
		}	
	case __CCB_DIALOG:
		{
			m_lstPopup=new CPopupDlgCtrl();
			break;
		}
	default:
		return true;
	}

 
	if(m_pcbInfo==NULL)
	{
		m_pcbInfo=new COMBOBOXINFO;
		m_pcbInfo->cbSize=sizeof(COMBOBOXINFO);
	}

	if(GetComboBoxInfo(m_pcbInfo))
	{
		//Destroy Original ListBox and crate new one
		VERIFY(::DestroyWindow(m_pcbInfo->hwndList));
		if(m_lstPopup->IsKindOf(RUNTIME_CLASS(CPopupListCtrl)))
            VERIFY(static_cast<CPopupListCtrl*>(m_lstPopup)->CreatePopupCtrl(this));
		else if(m_lstPopup->IsKindOf(RUNTIME_CLASS(CPopupMFCGid)))
			VERIFY(static_cast<CPopupMFCGid*>(m_lstPopup)->CreatePopupCtrl(this));
		else if(m_lstPopup->IsKindOf(RUNTIME_CLASS(CPopupDlgCtrl)))
			VERIFY(static_cast<CPopupDlgCtrl*>(m_lstPopup)->CreatePopupCtrl(this));
		else
			ASSERT(FALSE);
  		 
		HWND hWndEdit = ::GetWindow( GetSafeHwnd(), GW_CHILD );
		if( hWndEdit != NULL && ::IsWindow(hWndEdit) )
		{
			VERIFY( m_wndEditCombo.SubclassWindow(hWndEdit) );
			m_wndEditCombo.SetOwner(this);
		} 
		return true;
	}
	return false;
}

void CCstmComboBox::PreSubclassWindow()
{
	if (::IsWindow(m_hWnd))
		ModifyStyle(NULL, CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | CBS_HASSTRINGS |WS_VSCROLL );
	CComboBox::PreSubclassWindow();
}

// Derived class is responsible for implementing these handlers
//   for owner/self draw controls (except for the optional DeleteItem)
int CCstmComboBox::CompareItem(LPCOMPAREITEMSTRUCT /*lpCompareItemStruct*/)
{
	if(m_lstPopup)
		return 0;
	ASSERT(FALSE); 
	return 0;
}
void CCstmComboBox::DeleteItem(LPDELETEITEMSTRUCT lpDeleteItemStruct)
{
	/* default to nothing */
}

void CCstmComboBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	if(m_lstPopup)
		return;
 
	CComboBox::DrawItem(lpDrawItemStruct);
}

void CCstmComboBox::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	lpMeasureItemStruct->itemHeight = 30;
// 	if(m_lstPopup)
// 		return;
// 	CComboBox::MeasureItem(lpMeasureItemStruct);
}

void CCstmComboBox::OnLButtonDown(UINT nFlags, CPoint point)
{
	TRACE(_T(" LB down\n"));
	CComboBox::OnLButtonDown(nFlags, point);
	if(!m_lstPopup)
		return;

	UINT nHeight = m_nHeight; // m_nHeight
	if ( m_bTopAlign )
	{
		CRect rc;
		GetWindowRect(&rc);
		ShowDropDown( TRUE);
		m_lstPopup->SetWindowPos( &wndTopMost,rc.left,rc.top - nHeight, rc.Width(), nHeight, SWP_SHOWWINDOW );//SWP_NOACTIVATE | SWP_NOMOVE
 	}
	 

	//  	CRect rect;
	//  	//If point in ComboEdit ctrl
	// //  	HWND hWndEdit = ::GetWindow( GetSafeHwnd(), GW_CHILD );
	// //  	::GetWindowRect(hWndEdit,rect);
	//  	rect=m_pcbInfo->rcItem;
	// 	rect.left = rect.right;
	//  	if(rect.PtInRect(point))
	// 	{
	// 		ShowDropDown();
	// 		TRACE(_T("dadf\n"));
	// 		//PostMessage(CBN_CLOSEUP);
	// 	}
}


CWnd* CCstmComboBox::GetPopupCtrl(void)
{
	if(!m_lstPopup)
		return NULL;

	if(::IsWindow(m_lstPopup->m_hWnd))
		return m_lstPopup;
	else
		return NULL;
  
}
 
//Implementation
int CCstmComboBox::GetCount( ) const 
{
	if(m_lstPopup==NULL)
		return ((CComboBox*)this)->GetCount();

	if(m_lstPopup->IsKindOf(RUNTIME_CLASS(CPopupListCtrl)))
		return static_cast<CPopupListCtrl*>(m_lstPopup)->GetCount();
	else if(m_lstPopup->IsKindOf(RUNTIME_CLASS(CPopupMFCGid)))
		return static_cast<CPopupMFCGid*>(m_lstPopup)->GetCount();
	else if(m_lstPopup->IsKindOf(RUNTIME_CLASS(CPopupDlgCtrl)))
		return static_cast<CPopupDlgCtrl*>(m_lstPopup)->GetCount();
	else
		return ((CComboBox*)this)->GetCount();
}
int CCstmComboBox::GetCurSel( ) const
{
	if(m_lstPopup==NULL)
		return ((CComboBox*)this)->GetCurSel();

	if(m_lstPopup->IsKindOf(RUNTIME_CLASS(CPopupListCtrl)))
		return static_cast<CPopupListCtrl*>(m_lstPopup)->GetCurrentSelection();
	else if(m_lstPopup->IsKindOf(RUNTIME_CLASS(CPopupMFCGid)))
		return static_cast<CPopupMFCGid*>(m_lstPopup)->GetCurrentSelection();
	else if(m_lstPopup->IsKindOf(RUNTIME_CLASS(CPopupDlgCtrl)))
		return static_cast<CPopupDlgCtrl*>(m_lstPopup)->GetCurrentSelection();
	else
		return ((CComboBox*)this)->GetCurSel();
}
int CCstmComboBox::SetCurSel(int nSelect)
{
	if(m_lstPopup==NULL)
		return ((CComboBox*)this)->SetCurSel(nSelect);

	if(m_lstPopup->IsKindOf(RUNTIME_CLASS(CPopupListCtrl)))
		return static_cast<CPopupListCtrl*>(m_lstPopup)->SetCurrentSelection(nSelect);
	else if(m_lstPopup->IsKindOf(RUNTIME_CLASS(CPopupMFCGid)))
		return static_cast<CPopupMFCGid*>(m_lstPopup)->SetCurrentSelection(nSelect);
	else if(m_lstPopup->IsKindOf(RUNTIME_CLASS(CPopupDlgCtrl)))
		return static_cast<CPopupDlgCtrl*>(m_lstPopup)->SetCurrentSelection(nSelect);
	else
		return ((CComboBox*)this)->SetCurSel(nSelect);
}
void CCstmComboBox::GetDroppedControlRect(LPRECT lprect) const
{
	if(m_lstPopup==NULL)
		((CComboBox*)this)->GetDroppedControlRect(lprect);
	else
	{
		WINDOWPLACEMENT wndPl;
		wndPl.length=sizeof(WINDOWPLACEMENT);
		VERIFY(::GetWindowPlacement(m_lstPopup->m_hWnd,&wndPl));
		lprect->top=wndPl.rcNormalPosition.top;
		lprect->left=wndPl.rcNormalPosition.left;
		lprect->right=wndPl.rcNormalPosition.right;
		lprect->bottom=wndPl.rcNormalPosition.bottom;
		ClientToScreen(lprect);
	}
		

}
int CCstmComboBox::GetLBText(int nIndex,LPTSTR lpszText) const
{
	CString rString;
	GetLBText(nIndex,rString);
	lpszText=(LPTSTR)rString.GetString();
	return rString.GetLength();
}
void CCstmComboBox::GetLBText(int nIndex,CString& rString) const
{
	if(m_lstPopup==NULL)
	{
		((CComboBox*)this)->GetLBText(nIndex,rString);
		return;
	}

	if(m_lstPopup->IsKindOf(RUNTIME_CLASS(CPopupListCtrl)))
		static_cast<CPopupListCtrl*>(m_lstPopup)->GetItemText(nIndex,rString);
	else if(m_lstPopup->IsKindOf(RUNTIME_CLASS(CPopupMFCGid)))
		static_cast<CPopupMFCGid*>(m_lstPopup)->GetItemText(nIndex,rString);
	else if(m_lstPopup->IsKindOf(RUNTIME_CLASS(CPopupDlgCtrl)))
		static_cast<CPopupDlgCtrl*>(m_lstPopup)->GetItemText(nIndex,rString);
	else
		((CComboBox*)this)->GetLBText(nIndex,rString);
}
int CCstmComboBox::GetLBTextLen(int nIndex) const
{
	CString rString;
	GetLBText(nIndex,rString);
	return rString.GetLength();
}
int CCstmComboBox::AddString(LPCTSTR lpszString)
{
	if(m_lstPopup)
	{
		//Use custom popup specific method to add item
		ASSERT(FALSE);
		return -1;
	}
	else
		return ((CComboBox*)this)->AddString(lpszString);
}
int CCstmComboBox::DeleteString(UINT nIndex)
{
	if(m_lstPopup==NULL)
		return ((CComboBox*)this)->DeleteString(nIndex);

	if(m_lstPopup->IsKindOf(RUNTIME_CLASS(CPopupListCtrl)))
		return static_cast<CPopupListCtrl*>(m_lstPopup)->DeleteItem(nIndex);
	else if(m_lstPopup->IsKindOf(RUNTIME_CLASS(CPopupMFCGid)))
		return static_cast<CPopupMFCGid*>(m_lstPopup)->DeleteItem(nIndex);
	else if(m_lstPopup->IsKindOf(RUNTIME_CLASS(CPopupDlgCtrl)))
		return static_cast<CPopupDlgCtrl*>(m_lstPopup)->DeleteItem(nIndex);
	else
		return ((CComboBox*)this)->DeleteString(nIndex);
}
int CCstmComboBox::Dir(UINT attr,LPCTSTR lpszWildCard)
{
	if(m_lstPopup)
	{
		//Not Aviable for Custom Popup
		ASSERT(FALSE);
		return -1;
	}
	else
		return ((CComboBox*)this)->Dir(attr,lpszWildCard);
}
int CCstmComboBox::FindString(int nStartAfter,LPCTSTR lpszString) const
{
	if(m_lstPopup==NULL)
		return ((CComboBox*)this)->FindString(nStartAfter,lpszString);

		if(m_lstPopup->IsKindOf(RUNTIME_CLASS(CPopupListCtrl)))
			return static_cast<CPopupListCtrl*>(m_lstPopup)->FindString(nStartAfter,lpszString);
		else if(m_lstPopup->IsKindOf(RUNTIME_CLASS(CPopupMFCGid)))
			return static_cast<CPopupMFCGid*>(m_lstPopup)->FindString(nStartAfter,lpszString);
		else if(m_lstPopup->IsKindOf(RUNTIME_CLASS(CPopupDlgCtrl)))
			return static_cast<CPopupDlgCtrl*>(m_lstPopup)->FindString(nStartAfter,lpszString);
		else
			return ((CComboBox*)this)->FindString(nStartAfter,lpszString);
}
int CCstmComboBox::FindStringExact(int nIndexStart,LPCTSTR lpszFind) const
{
	if(m_lstPopup==NULL)
		return ((CComboBox*)this)->FindStringExact(nIndexStart,lpszFind);

		if(m_lstPopup->IsKindOf(RUNTIME_CLASS(CPopupListCtrl)))
			return static_cast<CPopupListCtrl*>(m_lstPopup)->FindStringExact(nIndexStart,lpszFind);
		else if(m_lstPopup->IsKindOf(RUNTIME_CLASS(CPopupMFCGid)))
			return static_cast<CPopupMFCGid*>(m_lstPopup)->FindStringExact(nIndexStart,lpszFind);
		else if(m_lstPopup->IsKindOf(RUNTIME_CLASS(CPopupDlgCtrl)))
			return static_cast<CPopupDlgCtrl*>(m_lstPopup)->FindStringExact(nIndexStart,lpszFind);
		else
			return ((CComboBox*)this)->FindStringExact(nIndexStart,lpszFind);
}
int CCstmComboBox::InsertString(int nIndex,LPCTSTR lpszString)
{
	if(m_lstPopup)
	{
		//Use custom popup specific method to insert item
		ASSERT(FALSE);
		return -1;
	}
	else
		return ((CComboBox*)this)->AddString(lpszString);
}
int CCstmComboBox::SelectString(int nStartAfter,LPCTSTR lpszString)
{
	if(m_lstPopup==NULL)
		return ((CComboBox*)this)->SelectString(nStartAfter,lpszString);

	int nCurSel=FindString(nStartAfter,lpszString);

	if(nCurSel!=CB_ERR)
		SetCurSel(nCurSel);
	return nCurSel;
}
void CCstmComboBox::ResetContent( )
{
	if(m_lstPopup==NULL)
	{
		((CComboBox*)this)->ResetContent();
		return;
	}

	if(m_lstPopup->IsKindOf(RUNTIME_CLASS(CPopupListCtrl)))
		static_cast<CPopupListCtrl*>(m_lstPopup)->RemoveAllItems();
	else if(m_lstPopup->IsKindOf(RUNTIME_CLASS(CPopupMFCGid)))
		static_cast<CPopupMFCGid*>(m_lstPopup)->RemoveAllItems();
	else if(m_lstPopup->IsKindOf(RUNTIME_CLASS(CPopupDlgCtrl)))
		static_cast<CPopupDlgCtrl*>(m_lstPopup)->RemoveAllItems();
	else
		((CComboBox*)this)->ResetContent();

}

int CCstmComboBox::SetItemData(int nIndex, DWORD_PTR dwItemData)
{
	if(m_lstPopup==NULL)
	{
		return ((CComboBox*)this)->SetItemData(nIndex,dwItemData);
 	}

	if(m_lstPopup->IsKindOf(RUNTIME_CLASS(CPopupListCtrl)))
		return static_cast<CPopupListCtrl*>(m_lstPopup)->SetItemData(nIndex,dwItemData);
	else if(m_lstPopup->IsKindOf(RUNTIME_CLASS(CPopupMFCGid)))
		return static_cast<CPopupMFCGid*>(m_lstPopup)->SetItemData(nIndex,0,dwItemData);

	return 0;
}

DWORD_PTR CCstmComboBox::GetItemData(int nIndex) const
{
	if(m_lstPopup==NULL)
	{
		return ((CComboBox*)this)->GetItemData(nIndex);
 	}

	if(m_lstPopup->IsKindOf(RUNTIME_CLASS(CPopupListCtrl)))
		return static_cast<CPopupListCtrl*>(m_lstPopup)->GetItemData(nIndex);
	else if(m_lstPopup->IsKindOf(RUNTIME_CLASS(CPopupMFCGid)))
		return static_cast<CPopupMFCGid*>(m_lstPopup)->GetItemData(nIndex,0);

	return 0;
}

void CCstmComboBox::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch(nChar) {
	case VK_UP:
	case VK_DOWN:
		{
			if(m_lstPopup!=NULL)
			{
				m_lstPopup->PostMessage(WM_KEYDOWN,(WPARAM)nChar,(LPARAM)nRepCnt);
				return;
			}
		}
	case VK_RETURN:
		{
			WPARAM wParam= MAKEWPARAM(GetDlgCtrlID(),CBN_SELCHANGE);
			LPARAM lParam=(LPARAM)m_hWnd;
			PostMessage(WM_COMMAND,wParam,lParam);
		}
	case VK_F4:
		{
			ShowDropDown(!GetDroppedState());
			return;
		}
	default:
		break;
	}

	CComboBox::OnKeyDown(nChar, nRepCnt, nFlags);
}

BOOL CCstmComboBox::OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult)
{
 	if(message==WM_COMMAND && m_lstPopup)
	{		
		switch(HIWORD(wParam)) 
		{
		case CBN_SELENDCANCEL:
			TRACE("CBN_SELENDCANCEL\n");
			 
			break;
		case CBN_SELENDOK:
			TRACE("CBN_SELENDOK\n");
 			break;
		case CBN_CLOSEUP:
			TRACE("CBN_CLOSEUP\n");
			if ( m_lstPopup )
			{
 				m_lstPopup->ShowWindow(SW_HIDE);
  			}
			break;
		case CBN_DROPDOWN:
			TRACE("CBN_DROPDOWN\n");
			if(m_lstPopup)
			{
				WINDOWPLACEMENT wndPl;
				wndPl.length=sizeof(WINDOWPLACEMENT);
				VERIFY(::GetWindowPlacement(m_lstPopup->m_hWnd,&wndPl));

				if((wndPl.rcNormalPosition.bottom-wndPl.rcNormalPosition.top)!=m_nHeight)
				{
					wndPl.rcNormalPosition.bottom=wndPl.rcNormalPosition.top+m_nHeight;
					VERIFY(::SetWindowPlacement(m_lstPopup->m_hWnd,&wndPl));
				}

				Invalidate(); 
 
// 				CRect rc;
// 				GetWindowRect(&rc);
// 				m_lstPopup->SetWindowPos( &wndTopMost,rc.left,rc.top - m_nHeight, rc.Width(),m_nHeight, 0 );//SWP_NOACTIVATE | SWP_NOMOVE
// 				m_lstPopup->ShowWindow(SW_SHOW);
			 

				if(!m_lstPopup->IsKindOf(RUNTIME_CLASS(CPopupDlgCtrl)))
					m_lstPopup->ShowWindow(SW_SHOW);
  
				CString strText;
				//If combo box has drop list box style there is no edit
				if(::IsWindow(m_wndEditCombo.m_hWnd))
				{
					m_wndEditCombo.GetWindowText(strText);
					if(!strText.IsEmpty())
						SetCurSel(FindStringExact(-1,strText));
				}
				

			}
			break;
		case CBN_EDITUPDATE:
			{
				break;
			}
		case CBN_EDITCHANGE:
			{
				if(m_bAutoComplete)
				{
					CString strEditText,strSearchedText;
					m_wndEditCombo.GetWindowText(strEditText);
					if(strEditText.IsEmpty())
					{
						SetCurSel(-1);

						break;
					}
					int nCurSel=FindString(-1,strEditText);
					if(nCurSel>-1)
					{
						int nSel=strEditText.GetLength();
						GetLBText(nCurSel,strSearchedText);
						SetWindowText(strSearchedText);
						SetEditSel(nSel,-1);
					}
					if(GetDroppedState())
						SetCurSel(nCurSel);

					nCurSel=FindStringExact(-1,strEditText);
					if(nCurSel!=CB_ERR)
						if(!GetDroppedState())
						{
							ShowDropDown();
							SetCurSel(nCurSel);
							EnsureVisible(nCurSel);
						}

				}
				else
					SetCurSel(-1);
			
			}
			
		case CBN_KILLFOCUS:
			TRACE("CBN_KILLFOCUS\n");
			break;
		case CBN_SETFOCUS:
			TRACE("CBN_SETFOCUS\n");
			break;
		case CBN_DBLCLK:
			TRACE("CBN_DBLCLK\n");
 			break;
		case CBN_SELCHANGE:
			TRACE("CBN_SELCHANGE\n");
			CString strText;
			if(GetCurSel()>-1)
			{
				GetLBText(GetCurSel(),strText);
				SetWindowText(strText);
				SetEditSel(0,-1);
			}

			int nItem = GetCurSel();
 			DWORD data = GetItemData(nItem);
			 
			int nID = GetDlgCtrlID();
			LPARAM lParam = MAKELPARAM(m_hWnd, 0);
			WPARAM wParam = MAKEWPARAM( nID, CBN_SELCHANGE );
			CWnd* pWnd = GetParent();
			pWnd->SendMessage(WM_COMMAND,  wParam, lParam );
			 
			break;
		}
	}

	return CComboBox::OnChildNotify(message, wParam, lParam, pLResult);
}

BOOL CCstmComboBox::PreTranslateMessage(MSG* pMsg)
{
	// Need to check for backspace/delete. These will modify the text in
	// the edit box, causing the auto complete to just add back the text
	// the user has just tried to delete. 

	if (pMsg->message == WM_KEYDOWN)
	{
		m_bAutoComplete = TRUE;

		int nVirtKey = (int) pMsg->wParam;
		if (nVirtKey == VK_DELETE || nVirtKey == VK_BACK)
			m_bAutoComplete = FALSE;
	}

	return CComboBox::PreTranslateMessage(pMsg);
}

void CCstmComboBox::EnsureVisible(int nItem)
{
	if(!m_lstPopup)
		return;
	if(m_lstPopup->IsKindOf(RUNTIME_CLASS(CPopupListCtrl)))
		static_cast<CPopupListCtrl*>(m_lstPopup)->EnsureVisible(nItem,TRUE);
	else if(m_lstPopup->IsKindOf(RUNTIME_CLASS(CPopupMFCGid)))
		static_cast<CPopupMFCGid*>(m_lstPopup)->EnsureVisible(nItem,0);
//	else if(m_lstPopup->IsKindOf(RUNTIME_CLASS(CPopupDlgCtrl)))
//		static_cast<CPopupDlgCtrl*>(m_lstPopup)->RemoveAllItems();
//	else
//		((CComboBox*)this)->ResetContent(); 

}

int CCstmComboBox::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if ( CComboBox::OnCreate(lpCreateStruct) == -1 )
		return -1;

	SetFontType(20,_T("Arial"));
	SetItemHeight(-1, 32);
    SetMinVisibleItems(6);

	m_imgComboBoxThump.LoadFromResource(AfxGetInstanceHandle(), IDB_COMBOBOX_THUMP);
	m_hItemIcon[0] = AfxGetApp()->LoadIcon(IDI_VARIABLE_SYSTEM);
	m_hItemIcon[1] = AfxGetApp()->LoadIcon(IDI_VARIABLE_GLOBAL);
	m_hItemIcon[2] = AfxGetApp()->LoadIcon(IDI_VARIABLE_PROJECT);

	return 0;
}

HBRUSH CCstmComboBox::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CComboBox::OnCtlColor(pDC, pWnd, nCtlColor);
 
	TCHAR clsname[MAX_PATH];
	GetClassName(pWnd->m_hWnd, clsname, MAX_PATH);

	CString strClsName;
	strClsName = clsname;
	if( nCtlColor == CTLCOLOR_LISTBOX )
	{
  		TRACE(_T("ctlcolor \n"));

	}
 	return hbr;
}

 
void CCstmComboBox::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	if (m_imgComboBoxThump.IsNull()) return;

	int slideWidth = 42;

	CRect rcComboBox;
	GetClientRect(&rcComboBox);

	CMemDC MemDC(dc, rcComboBox);
	CDC& mdc = MemDC.GetDC();
	MemDC.GetDC().SetBkMode(TRANSPARENT);

	// 	 rcShow.DeflateRect(1,1);
	CRect rcShow = rcComboBox;
	rcShow.OffsetRect(0,0);  
	rcShow.right = rcComboBox.right-slideWidth;
	rcShow.bottom -= 1;

	CRect rcComboThump = rcComboBox;
	rcComboThump.left = rcComboBox.right - slideWidth;

	m_imgComboBoxThump.Draw( MemDC.GetDC(), rcComboThump,Gdiplus::InterpolationModeHighQuality ); 

	CBrush pBrush;

	if ( IsWindowEnabled() )
	{
		pBrush.CreateSolidBrush(RGB(255,255,255));
	}
	else
	{
		pBrush.CreateSolidBrush(RGB(216,220,216));
	}
	MemDC.GetDC().FillRect( rcShow, &pBrush );
	pBrush.DeleteObject();

	//	 CBrush br(RGB(49,106,197));
	CBrush br(RGB(0,0,0));
	MemDC.GetDC().FrameRect(rcShow,&br);
	MemDC.GetDC().FrameRect(rcComboBox,&br);

	int nItem = GetCurSel();
	if (nItem == -1) return;
	DWORD data = GetItemData(nItem);
	HICON m_Icon = get_icon( data );

	CRect rcIcon = rcComboBox;
	int hoffset = ( rcComboBox.Height() - 16 )/2;
	if ( hoffset < 0 )
		hoffset = 0;
	rcIcon.OffsetRect(0,hoffset);

	int textOffset = 4;
	if ( m_Icon && m_bIsShowImage )
	{
		DrawIconEx(MemDC.GetDC(), 5, rcIcon.top, m_Icon, 16, 16, 0, 0, DI_NORMAL);
		textOffset = 24;
	}

	CFont* oldFont = MemDC.GetDC().SelectObject(m_font.GetSafeHandle() ? &m_font : GetFont());

	CString strItem; 
     
	CRect rcText = rcComboBox;
	rcText.left += textOffset;
	GetLBText(nItem, strItem);
	DrawText(MemDC.GetDC(), strItem, strItem.GetLength(), rcText, DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_WORD_ELLIPSIS);
	MemDC.GetDC().SelectObject(oldFont);
	br.DeleteObject();
}

void CCstmComboBox::Draw(CDC *pDC)
{
	CDC dc;
	dc.Attach(pDC->m_hDC);

	CRect rc;
	GetClientRect(&rc);
	dc.FillSolidRect(rc,GetSysColor(COLOR_WINDOW));

	CString sz;
	GetWindowText(sz);
	rc.left += 4;

	dc.SetBkMode(TRANSPARENT);

	RECT rcRepaint;
	GetClientRect(&rcRepaint);

 	dc.Detach();
}

void CCstmComboBox::SetFontType(int fontSize, CString fontType) 
{  
	m_font.DeleteObject();
	LOGFONT lf;  
	memset(&lf, 0, sizeof(LOGFONT));  
	lf.lfHeight = fontSize;  
	lf.lfWeight = 650;
	_tcsncpy_s(lf.lfFaceName, LF_FACESIZE, fontType, fontType.GetLength());  
	VERIFY(m_font.CreateFontIndirect(&lf));  
}

HICON CCstmComboBox::get_icon( DWORD  dwData )
{
	int hiByte = HIBYTE(dwData);
	if ( hiByte < 0 || hiByte > 2)
	{
		return NULL;
	}
	return m_hItemIcon[hiByte];
 
}

void CCstmComboBox::OnCbnCloseup()
{
	// TODO: 在此添加控件通知处理程序代码
	TRACE("CBN_DROPDOWN 0\n");

}

void CCstmComboBox::OnCbnDropdown()
{
	// TODO: 在此添加控件通知处理程序代码
	TRACE("CBN_DROPDOWN 1\n");

}