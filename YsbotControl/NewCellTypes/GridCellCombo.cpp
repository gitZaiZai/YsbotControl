// GridCellCombo.cpp : implementation file
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
// 6 Aug 1998 - Added CComboEdit to subclass the edit control - code 
//              provided by Roelf Werkman <rdw@inn.nl>. Added nID to 
//              the constructor param list.
// 29 Nov 1998 - bug fix in onkeydown (Markus Irtenkauf)
// 13 Mar 2004 - GetCellExtent fixed by Yogurt
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "../GridCtrl_src/GridCell.h"
#include "../GridCtrl_src/GridCtrl.h"
#include "GridCellCombo.h"
#include "../resource.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CComboEdit

CComboEdit::CComboEdit()
{
}

CComboEdit::~CComboEdit()
{
}

// Stoopid win95 accelerator key problem workaround - Matt Weagle.
BOOL CComboEdit::PreTranslateMessage(MSG* pMsg) 
{
	// Make sure that the keystrokes continue to the appropriate handlers
	if (pMsg->message == WM_KEYDOWN || pMsg->message == WM_KEYUP)
	{
		::TranslateMessage(pMsg);
		::DispatchMessage(pMsg);
		return TRUE;
	}	

	// Catch the Alt key so we don't choke if focus is going to an owner drawn button
	if (pMsg->message == WM_SYSCHAR)
		return TRUE;

	return CEdit::PreTranslateMessage(pMsg);
}

BEGIN_MESSAGE_MAP(CComboEdit, CEdit)
	//{{AFX_MSG_MAP(CComboEdit)
	ON_WM_KILLFOCUS()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CComboEdit message handlers

void CComboEdit::OnKillFocus(CWnd* pNewWnd) 
{
	CEdit::OnKillFocus(pNewWnd);
	
    CInPlaceList* pOwner = (CInPlaceList*) GetOwner();  // This MUST be a CInPlaceList
    if (pOwner)
        pOwner->EndEdit();	
}

void CComboEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if ((nChar == VK_PRIOR || nChar == VK_NEXT ||
		 nChar == VK_DOWN  || nChar == VK_UP   ||
		 nChar == VK_RIGHT || nChar == VK_LEFT) &&
		(GetKeyState(VK_CONTROL) < 0 && GetDlgCtrlID() == IDC_COMBOEDIT))
    {
        CWnd* pOwner = GetOwner();
        if (pOwner)
            pOwner->SendMessage(WM_KEYDOWN, nChar, nRepCnt+ (((DWORD)nFlags)<<16));
        return;
    }

	CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CComboEdit::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (nChar == VK_ESCAPE) 
	{
        CWnd* pOwner = GetOwner();
        if (pOwner)
            pOwner->SendMessage(WM_KEYUP, nChar, nRepCnt + (((DWORD)nFlags)<<16));
        return;
    }

	if (nChar == VK_TAB || nChar == VK_RETURN || nChar == VK_ESCAPE)
    {
        CWnd* pOwner = GetOwner();
        if (pOwner)
            pOwner->SendMessage(WM_KEYUP, nChar, nRepCnt + (((DWORD)nFlags)<<16));
        return;
    }

	CEdit::OnKeyUp(nChar, nRepCnt, nFlags);
}


/////////////////////////////////////////////////////////////////////////////
// CInPlaceList

CInPlaceList::CInPlaceList(CWnd* pParent, CRect& rect, DWORD dwStyle, UINT nID,
                           int nRow, int nColumn, 
                           COLORREF crFore, COLORREF crBack,
						   CStringArray& Items, CString sInitText, 
						   UINT nFirstChar,
						   LPCTSTR strSeparator /*=NULL*/)
{
    m_crForeClr = crFore;
    m_crBackClr = crBack;

	m_nNumLines = 6;
	m_sInitText = sInitText;
 	m_nRow		= nRow;
 	m_nCol      = nColumn;
 	m_nLastChar = 0; 
	m_bExitOnArrows = FALSE; //(nFirstChar != VK_LBUTTON);	// If mouse click brought us here,
	SetShowImage( ((CGridCtrl*)(pParent))->IsShowImageCombobox());
	/*((CGridCtrl*)(pParent))->GetItemState()*/
	// Create the combobox
 	DWORD dwComboStyle = WS_BORDER|WS_CHILD|WS_VISIBLE|WS_VSCROLL|
 					     CBS_AUTOHSCROLL | dwStyle;
	int nHeight = rect.Height();
	rect.bottom = rect.bottom + m_nNumLines*nHeight + ::GetSystemMetrics(SM_CYHSCROLL);
	if (!Create(dwComboStyle, rect, pParent, nID)) return;

	SetMinVisibleItems(6); // liurundan
 
	// Add the strings
	CStringArray strItemArray;
	CString strItem;
 	DWORD  item_value = 0;
	for (int i = 0; i < Items.GetSize(); i++) 
	{
		strItem = Items[i];
		if ( strSeparator )
		{
			m_strSeparator = strSeparator;
			UINT numstr = DivStr( strItem, strItemArray, m_strSeparator );
			if ( numstr > 1 )
			{
				item_value = _tcstol(strItemArray[1], NULL, 10);
 				strItem = strItemArray[0];
			}
  		}
		else
		{
			SetShowImage(FALSE);
		}
 		AddString( strItem, item_value );
	}
	strItemArray.RemoveAll();

	SetFont(pParent->GetFont());
	SetItemHeight(-1, nHeight);
	SetItemHeight(0, nHeight);  // 列表项高度

    int nMaxLength = GetCorrectDropWidth();
    /*
    if (nMaxLength > rect.Width())
	    rect.right = rect.left + nMaxLength;
	// Resize the edit window and the drop down window
	MoveWindow(rect);
    */

	SetDroppedWidth(nMaxLength);

	SetHorizontalExtent(0); // no horz scrolling

	// Set the initial text to m_sInitText
    if (::IsWindow(m_hWnd) && SelectString(-1, m_sInitText) == CB_ERR) 
		SetWindowText(m_sInitText);		// No text selected, so restore what was there before

    ShowDropDown();

    // Subclass the combobox edit control if style includes CBS_DROPDOWN
    if ((dwStyle & CBS_DROPDOWNLIST) != CBS_DROPDOWNLIST)
    {
        m_edit.SubclassDlgItem(IDC_COMBOEDIT, this);
 	    SetFocus();
        switch (nFirstChar)
        {
            case VK_LBUTTON: 
            case VK_RETURN:   m_edit.SetSel((int)_tcslen(m_sInitText), -1); return;
            case VK_BACK:     m_edit.SetSel((int)_tcslen(m_sInitText), -1); break;
            case VK_DOWN: 
            case VK_UP:   
            case VK_RIGHT:
            case VK_LEFT:  
            case VK_NEXT:  
            case VK_PRIOR: 
            case VK_HOME:  
            case VK_END:      m_edit.SetSel(0,-1); return;
            default:          m_edit.SetSel(0,-1);
        }
        SendMessage(WM_CHAR, nFirstChar);
    }
    else
 	    SetFocus();
// 
// 	// 下面一段代码就是用来去掉WIN7或者WIN8旁边的透明边框
// 	HINSTANCE hInst = LoadLibrary(_T("UxTheme.dll"));
// 	if (hInst)
// 	{
// 		typedef HRESULT (WINAPI *PFUN_SetWindowTheme)(HWND, LPCTSTR, LPCTSTR);
// 		PFUN_SetWindowTheme pFun = (PFUN_SetWindowTheme)GetProcAddress(hInst, "SetWindowTheme");
// 		if (pFun)
// 		{
// 			pFun(m_hWnd, _T(""), _T("")); //去掉xp主题
// 		}
// 		FreeLibrary(hInst);
// 	}
// 
// 	hInst = LoadLibrary(_T("dwmapi.dll"));
// 	if (hInst)
// 	{
// 		typedef HRESULT (WINAPI * TmpFun)(HWND,DWORD,LPCVOID,DWORD);
// 		TmpFun DwmSetWindowAttributeEX = (TmpFun)::GetProcAddress(hInst, "DwmSetWindowAttribute");
// 		if (DwmSetWindowAttributeEX)
// 		{
// 			DWORD dwAttr = 1;
// 			DwmSetWindowAttributeEX(GetSafeHwnd(), 2, &dwAttr, 4); //去掉vista或win7特效
// 		}
// 		FreeLibrary(hInst);
// 	}

}

CInPlaceList::~CInPlaceList()
{
}

void CInPlaceList::BeginEdit()
{
	CString str;
	if (::IsWindow(m_hWnd))
		GetWindowText(str);
  
	// Send Notification to parent
	GV_DISPINFO dispinfo;

	dispinfo.hdr.hwndFrom = GetSafeHwnd();
	dispinfo.hdr.idFrom   = GetDlgCtrlID();
	dispinfo.hdr.code     = GVN_BEGINLABELEDIT;
 
	dispinfo.item.row     = m_nRow;
	dispinfo.item.col     = m_nCol;
	dispinfo.item.strText = str;
 
	CWnd* pOwner = GetOwner();
	if (IsWindow(pOwner->GetSafeHwnd()))
		pOwner->SendMessage(WM_NOTIFY, GetDlgCtrlID(), (LPARAM)&dispinfo );
}

void CInPlaceList::EndEdit()
{
    CString str;
    if (::IsWindow(m_hWnd))
        GetWindowText(str);
 
	int nIndex = GetCurSel();
	DWORD dwData = GetItemData(nIndex);
 	 
    // Send Notification to parent
    GV_DISPINFO dispinfo;

    dispinfo.hdr.hwndFrom = GetSafeHwnd();
    dispinfo.hdr.idFrom   = GetDlgCtrlID();
    dispinfo.hdr.code     = GVN_ENDLABELEDIT;
 
	if ( IsShownImage() )
 		dispinfo.item.mask    = LVIF_TEXT|LVIF_PARAM|LVIF_IMAGE;
 	else
		dispinfo.item.mask    = LVIF_TEXT|LVIF_PARAM;

    dispinfo.item.row     = m_nRow;
    dispinfo.item.col     = m_nCol;
    dispinfo.item.strText = str;
    dispinfo.item.lParam  = (LPARAM) m_nLastChar; 
    dispinfo.item.iImage  = (int)(dwData);
    CWnd* pOwner = GetOwner();
    if (IsWindow(pOwner->GetSafeHwnd()))
        pOwner->SendMessage(WM_NOTIFY, GetDlgCtrlID(), (LPARAM)&dispinfo );
 
    // Close this window (PostNcDestroy will delete this)
    if (::IsWindow(m_hWnd))
        PostMessage(WM_CLOSE, 0, 0);
}

int CInPlaceList::GetCorrectDropWidth()
{
    const int nMaxWidth = 200;  // don't let the box be bigger than this

    // Reset the dropped width
    int nNumEntries = GetCount();
    int nWidth = 0;
    CString str;

    CClientDC dc(this);
    int nSave = dc.SaveDC();
    dc.SelectObject(GetFont());

    int nScrollWidth = ::GetSystemMetrics(SM_CXVSCROLL);
    for (int i = 0; i < nNumEntries; i++)
    {
        GetLBText(i, str);
        int nLength = dc.GetTextExtent(str).cx + nScrollWidth;
        nWidth = max(nWidth, nLength);
    }
    
    // Add margin space to the calculations
    nWidth += dc.GetTextExtent(_T("0")).cx;

    dc.RestoreDC(nSave);

    nWidth = min(nWidth, nMaxWidth);

    return nWidth;
    //SetDroppedWidth(nWidth);
}

/*
// Fix by Ray (raybie@Exabyte.COM)
void CInPlaceList::OnSelendOK() 
{
    int iIndex = GetCurSel(); 
    if( iIndex != CB_ERR) 
    { 
        CString strLbText; 
        GetLBText( iIndex, strLbText); 
 
        if (!((GetStyle() & CBS_DROPDOWNLIST) == CBS_DROPDOWNLIST)) 
           m_edit.SetWindowText( strLbText); 
    } 
 
    GetParent()->SetFocus(); 	
}
*/

void CInPlaceList::PostNcDestroy() 
{
	CComboBox::PostNcDestroy();

	delete this;
}

BEGIN_MESSAGE_MAP(CInPlaceList, CIconComboBox)
	//{{AFX_MSG_MAP(CInPlaceList)
	ON_WM_KILLFOCUS()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_CONTROL_REFLECT(CBN_DROPDOWN, OnDropdown)
	ON_WM_GETDLGCODE()
	ON_WM_CTLCOLOR_REFLECT()
	//ON_WM_PAINT()
	//}}AFX_MSG_MAP
	//ON_CONTROL_REFLECT(CBN_SELENDOK, OnSelendOK)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CInPlaceList message handlers
void CInPlaceList::OnPaint()
{
	CPaintDC dc(this); // device context for painting
 
	CRect rcComboBox;
	GetClientRect(&rcComboBox);

	CBrush pBrush;
    pBrush.CreateSolidBrush(m_crBackClr);
	FillRect(dc.m_hDC, rcComboBox, pBrush);

	CString sz;
	GetWindowText(sz);
	rcComboBox.left += 5;

	dc.SetBkMode(TRANSPARENT);
	dc.SelectObject(GetFont());
	dc.DrawText(sz,&rcComboBox,DT_VCENTER | DT_SINGLELINE);
	
}

UINT CInPlaceList::OnGetDlgCode() 
{
    return DLGC_WANTALLKEYS;
}

void CInPlaceList::OnDropdown() 
{
   OnPaint();
   SetDroppedWidth(GetCorrectDropWidth());
}

void CInPlaceList::OnKillFocus(CWnd* pNewWnd) 
{
	CComboBox::OnKillFocus(pNewWnd);

	if (GetSafeHwnd() == pNewWnd->GetSafeHwnd())
        return;

    // Only end editing on change of focus if we're using the CBS_DROPDOWNLIST style
    if ((GetStyle() & CBS_DROPDOWNLIST) == CBS_DROPDOWNLIST)
        EndEdit();
}

// If an arrow key (or associated) is pressed, then exit if
//  a) The Ctrl key was down, or
//  b) m_bExitOnArrows == TRUE
void CInPlaceList::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if ((nChar == VK_PRIOR || nChar == VK_NEXT ||
		 nChar == VK_DOWN  || nChar == VK_UP   ||
		 nChar == VK_RIGHT || nChar == VK_LEFT) &&
		(m_bExitOnArrows || GetKeyState(VK_CONTROL) < 0))
	{
		m_nLastChar = nChar;
		GetParent()->SetFocus();
		return;
	}

	CComboBox::OnKeyDown(nChar, nRepCnt, nFlags);
}

// Need to keep a lookout for Tabs, Esc and Returns.
void CInPlaceList::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (nChar == VK_ESCAPE) 
		SetWindowText(m_sInitText);	// restore previous text

	if (nChar == VK_TAB || nChar == VK_RETURN || nChar == VK_ESCAPE)
	{
		m_nLastChar = nChar;
		GetParent()->SetFocus();	// This will destroy this window
		return;
	}

	CComboBox::OnKeyUp(nChar, nRepCnt, nFlags);
}

HBRUSH CInPlaceList::CtlColor(CDC* /*pDC*/, UINT /*nCtlColor*/) 
{
    /*
    static CBrush brush(m_crBackClr);
    pDC->SetTextColor(m_crForeClr);
    pDC->SetBkMode(TRANSPARENT);
    return (HBRUSH) brush.GetSafeHandle();
    */
	
	// TODO: Return a non-NULL brush if the parent's handler should not be called
	return NULL;
}

UINT CInPlaceList::DivStr(CString str,CStringArray& Arr,CString strDiv)
{
	Arr.RemoveAll();

	int nFindposi  = str.Find(strDiv);
	if( nFindposi <0 )
		return 0;

	str.Trim();
	while( nFindposi > 0)
	{
		Arr.Add(str.Left(nFindposi) );
		str = str.Right( str.GetLength() - nFindposi -1);
		str.TrimLeft(strDiv);    //warning

		nFindposi  = str.Find(strDiv);
	}

	if( !str.IsEmpty() )
		Arr.Add(str);

	return Arr.GetSize();
}
/////////////////////////////////////////////////////////////////////////////
// CGridCellCombo 
/////////////////////////////////////////////////////////////////////////////


IMPLEMENT_DYNCREATE(CGridCellCombo, CGridCell)

CGridCellCombo::CGridCellCombo() : CGridCell()
{
	m_DrawArrowAlways = FALSE;
	m_strSeparator = NULL;
    SetStyle(CBS_DROPDOWN);  // CBS_DROPDOWN, CBS_DROPDOWNLIST, CBS_SIMPLE, CBS_SORT

	if ( m_imgComboBoxThump.IsNull() ) 
		m_imgComboBoxThump.LoadFromResource(AfxGetInstanceHandle(), IDB_COMBOBOX_THUMP);

	//m_imgComboBox.LoadFromResource(AfxGetInstanceHandle(), IDB_COMBOBOX);
}

CGridCellCombo::~CGridCellCombo()
{
	m_imgComboBoxThump.Destroy();
}

// Create a control to do the editing
BOOL CGridCellCombo::Edit(int nRow, int nCol, CRect rect, CPoint /* point */, UINT nID, UINT nChar)
{
    m_bEditing = TRUE;
  
    // CInPlaceList auto-deletes itself
    m_pEditWnd = new CInPlaceList(GetGrid(), rect, GetStyle(), nID, nRow, nCol, 
                                  GetTextClr(), GetGrid()->GetBkColor(), m_Strings, GetText(), nChar, m_strSeparator);

    return TRUE;
}

CWnd* CGridCellCombo::GetEditWnd() const
{
	if (m_pEditWnd && (m_pEditWnd->GetStyle() & CBS_DROPDOWNLIST) != CBS_DROPDOWNLIST )
		return &(((CInPlaceList*)m_pEditWnd)->m_edit);

	return NULL;
}


CSize CGridCellCombo::GetCellExtent(CDC* pDC)
{    
    CSize sizeScroll (GetSystemMetrics(SM_CXVSCROLL), GetSystemMetrics(SM_CYHSCROLL));    
    CSize sizeCell (CGridCell::GetCellExtent(pDC));    
    sizeCell.cx += sizeScroll.cx;    
    sizeCell.cy = max(sizeCell.cy,sizeScroll.cy);    
    return sizeCell;
}

// Cancel the editing.
void CGridCellCombo::EndEdit()
{
    if (m_pEditWnd)
        ((CInPlaceList*)m_pEditWnd)->EndEdit();
}

// Override draw so that when the cell is selected, a drop arrow is shown in the RHS.
BOOL CGridCellCombo::Draw(CDC* pDC, int nRow, int nCol, CRect rect,  BOOL bEraseBkgnd /*=TRUE*/)
{
#ifdef _WIN32_WCE
    return CGridCell::Draw(pDC, nRow, nCol, rect,  bEraseBkgnd);
#else
    // Cell selected?
    //if ( !IsFixed() && IsFocused())
    if ( m_DrawArrowAlways || (GetGrid()->IsCellEditable(nRow, nCol) && !IsEditing() ) )
    {
        // Get the size of the scroll box
        CSize sizeScroll(GetSystemMetrics(SM_CXVSCROLL), GetSystemMetrics(SM_CYHSCROLL));

        // enough room to draw?
        if (sizeScroll.cy < rect.Width() && sizeScroll.cy < rect.Height())
        {
            // Draw control at RHS of cell
            CRect ScrollRect = rect;
            ScrollRect.left   = rect.right - sizeScroll.cx*2; // liurundan
            ScrollRect.bottom = rect.top + sizeScroll.cy*2;
			m_ScrollRect = ScrollRect;
            // Do the draw 
            //
  			if ( m_imgComboBoxThump.IsNull() )
			{
				pDC->DrawFrameControl(ScrollRect, DFC_SCROLL, DFCS_SCROLLDOWN);
			}
 			else
  			{
 				m_imgComboBoxThump.Draw( pDC->m_hDC, ScrollRect ); 
  			}

            // Adjust the remaining space in the cell
            rect.right = ScrollRect.left;
        }
    }

    CString strTempText = GetText();
    if (IsEditing())
        SetText(_T(""));

    // drop through and complete the cell drawing using the base class' method
    BOOL bResult = CGridCell::Draw(pDC, nRow, nCol, rect,  bEraseBkgnd);

    if (IsEditing())
        SetText(strTempText);

	return bResult;
#endif
}

// For setting the strings that will be displayed in the drop list
void CGridCellCombo::SetOptions(const CStringArray& ar, BOOL bDrawArrow, LPCTSTR strSeparator )
{ 
	m_Strings.RemoveAll();
	for (int i = 0; i < ar.GetSize(); i++)
		m_Strings.Add(ar[i]);
	m_DrawArrowAlways = bDrawArrow;
	m_strSeparator    = strSeparator; // liurundan
}

void  CGridCellCombo::SetTwoTypeOptions(const CStringArray& arFirstType, const CStringArray& arSecondType, BOOL bDrawArrow, LPCTSTR strSeparator )
{ 
	m_Strings.RemoveAll();
	
	for (int i = 0; i < arFirstType.GetSize(); i++)
		m_Strings.Add(arFirstType[i]);
	for (int i = 0; i < arSecondType.GetSize(); i++)
		m_Strings.Add(arSecondType[i]);

	m_DrawArrowAlways = bDrawArrow;
	m_strSeparator    = strSeparator; // liurundan
}

void CGridCellCombo::OnClick(CRect cellRect, CPoint PointCellRelative)
{
	if ( IsSelected() )
	{
		CPoint pt(PointCellRelative.x + cellRect.left, PointCellRelative.y + cellRect.top);
		if ( m_ScrollRect.PtInRect(pt) )
		{
			int nRow = GetGrid()->GetFocusCell().row;
			int nCol = GetGrid()->GetFocusCell().col;

			CCellID cell(nRow, nCol);
			if (!GetGrid()->IsValid(cell) )
				return;

			// Can we see what we are doing?
			GetGrid()->EnsureVisible(nRow, nCol);
			if (!GetGrid()->IsCellVisible(nRow, nCol))
				return;

			CRect rect;
			if (!GetGrid()->GetCellRect(cell, rect))
				return;

			GetGrid()->GetTopLeftMergedCell(nRow, nCol, rect);
 
			m_pEditWnd = new CInPlaceList(GetGrid(), rect, GetStyle() | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED |CBS_HASSTRINGS, IDC_INPLACE_CONTROL, nRow, nCol, 
				GetTextClr(), GetGrid()->GetBkColor(), m_Strings, GetText(), VK_LBUTTON, m_strSeparator);


			((CInPlaceList*)m_pEditWnd)->BeginEdit();

 			TRACE(_T("click combox r = %d c = %d!\n"),nRow,nCol);
		}
	}
}