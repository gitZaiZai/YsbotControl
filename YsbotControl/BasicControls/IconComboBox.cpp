// Copyright 2016, 亚思信息科技有限责任公司
// All right reserved.
//
// Author: Liu Run Dan ( lrdcz@163.com )
//
// Yasi Robot Motion Control System - 亚思机器人运动控制系统

// IconComboBox.cpp : 实现文件
//

#include "stdafx.h"
//#include "Frame.h"
#include "IconComboBox.h"
#include "../resource.h"
#include "../LuaScript/ScriptVariableType.h"
#include "../WorldModel.h"

// CIconComboBox

IMPLEMENT_DYNAMIC(CIconComboBox, CComboBox)

CIconComboBox::CIconComboBox(UINT nLines):m_LinesToDisplay(nLines)
{
	m_bIsThumpHover = FALSE;
	m_bIsThumpDown = FALSE;
	m_bIsCustomMessage = TRUE;
	m_bIsShowImage = TRUE;
	m_bAllowPopupNested = FALSE;

	for ( int i = 0; i < 3; ++i )
	{
		m_hItemIcon[i] = NULL;
	}
	m_hWndList = NULL;
	m_hWndEdit = NULL;
	m_hBitmapBoxThump = NULL;
	m_hIconBoxThump = NULL;
	m_bTopAlign = FALSE;
	m_sort = true;

	m_imgThumpWidth = 32;

	m_font.CreateFont(18, 0, 0, 0, 0, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
		OUT_STROKE_PRECIS, CLIP_STROKE_PRECIS, DRAFT_QUALITY, FIXED_PITCH |
		FF_MODERN, _T("宋体")); 
}

CIconComboBox::~CIconComboBox()
{
	m_font.DeleteObject();
	for ( int i = 0; i < 3; ++i )
	{
		DestroyIcon(m_hItemIcon[i]);
	}
	m_imgComboBox.Destroy();
    m_imgComboBoxThump.Destroy();

	if (m_hBitmapBoxThump)
	{
		DeleteObject(m_hBitmapBoxThump);
		m_hBitmapBoxThump = NULL;
	}
	if ( m_hIconBoxThump )
	{
		DestroyIcon(m_hIconBoxThump);
 	}
}


BEGIN_MESSAGE_MAP(CIconComboBox, CComboBox)
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_MOUSEHOVER, OnMouseHover)
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
	ON_CONTROL_REFLECT(CBN_SELCHANGE, &CIconComboBox::OnCbnSelchange)
	ON_WM_CTLCOLOR() 
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
 
END_MESSAGE_MAP()



// CIconComboBox 消息处理程序



void CIconComboBox::OnSize(UINT nType, int cx, int cy)
{
	CComboBox::OnSize(nType, cx, cy);

// 	COMBOBOXINFO comboBoxInfo;
// 	comboBoxInfo.cbSize = sizeof(COMBOBOXINFO);
// 	GetComboBoxInfo(&comboBoxInfo);
 
}

 void CIconComboBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
 {
 	HDC hDC = lpDrawItemStruct->hDC;
  	int nItem = lpDrawItemStruct->itemID;
 	CRect rcItem = lpDrawItemStruct->rcItem;
 	DWORD data = lpDrawItemStruct->itemData;
 	HICON hIcon = NULL;
 	hIcon = get_icon( data );
  
 	UINT CtlType = lpDrawItemStruct->CtlType;
 	if ( CtlType != ODT_COMBOBOX )
 	{
 		return;
 	}
 	int hoffset = ( rcItem.Height() - 16 )/2;
 	if ( hoffset < 0 )
  		hoffset = 0;
 	CRect rcIcon = rcItem;
 	rcIcon.OffsetRect(0,hoffset);
 
  
 	if (nItem != CB_ERR)
 	{
 		COLORREF clrBackground;
 		COLORREF clrText;
 		CRect rcText = rcItem;
 		if (lpDrawItemStruct->itemState & ODS_SELECTED)  //如果当前项被选中
 		{
 // 			clrBackground = ::GetSysColor(COLOR_HIGHLIGHT);
 // 			clrText = ~::GetSysColor(COLOR_WINDOWTEXT) & 0x00FFFFFF;
 
 			clrBackground  = RGB(88,148,24);
 			clrText  = RGB(0,0,0);  
 		}
 		else
 		{
 			clrBackground = ::GetSysColor(COLOR_WINDOW);
 			clrText = ::GetSysColor(COLOR_WINDOWTEXT);
 		}
 		::SetTextColor(hDC, clrText);
 		::SetBkColor(hDC, clrBackground);
 		::ExtTextOut(hDC, 0, 0, ETO_OPAQUE, rcItem, NULL, 0, NULL); // rcText
 		CString strItem;
 		GetLBText(nItem, strItem);
 		
 		HGDIOBJ oldFont = ::SelectObject( hDC, m_font.GetSafeHandle());
 
 		if (hIcon && m_bIsShowImage )
 		{
 			DrawIconEx(hDC, 1, rcIcon.top, hIcon, 16, 16, 0, 0, DI_NORMAL);
 			rcText.left = 16+4;
 		}
 		DrawText(hDC, strItem, strItem.GetLength(), rcText, DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_WORD_ELLIPSIS);
 		::SelectObject( hDC, oldFont);

		//printf("\n draw item combbox!\n");
 	}
 	if( lpDrawItemStruct->itemAction & ODA_DRAWENTIRE )
 	{
 		Invalidate(); 
 		RedrawWindow(); // liurundan update edit box at once.
 	}
 	 
}

void CIconComboBox::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	lpMeasureItemStruct->itemHeight = 30;
}

int CIconComboBox::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if ( !m_sort )
	{
		ModifyStyle(m_hWnd,CBS_SORT,0,0);
	}

	if (CComboBox::OnCreate(lpCreateStruct) == -1)
		return -1;
	SetItemHeight(-1, 30);
	SetMinVisibleItems(m_LinesToDisplay); 
// 	if (!m_imgComboBox.IsNull()) return 0;
// 	if (!m_imgComboBoxThump.IsNull()) return 0;

	//m_imgComboBox.LoadFromResource(AfxGetInstanceHandle(), IDB_COMBOBOX);
	HINSTANCE hInstance = AfxGetInstanceHandle( );
 	m_imgComboBoxThump.LoadFromResource(hInstance, IDB_COMBOBOX_THUMP);
	
	m_imgThumpWidth = m_imgComboBoxThump.GetWidth();

	m_hItemIcon[0] = AfxGetApp()->LoadIcon(IDI_VARIABLE_SYSTEM);
	m_hItemIcon[1] = AfxGetApp()->LoadIcon(IDI_VARIABLE_GLOBAL);
	m_hItemIcon[2] = AfxGetApp()->LoadIcon(IDI_VARIABLE_PROJECT);

	//SetFontType(20,_T("Arial"));

// 	CString strIconName = _T("./res/idb_combobox_thump.ico");
// 	m_hIconBoxThump = (HICON)LoadImage(hInstance, strIconName, IMAGE_ICON, 0, 0, LR_LOADFROMFILE);

	return 0;
}

 void CIconComboBox::OnPaint()
 {
 	 CPaintDC dc(this); // device context for painting
      
	 int slideWidth = m_imgThumpWidth;

 	 CRect rcComboBox;
	 GetClientRect(&rcComboBox);

	 CMemDC MemDC(dc, rcComboBox);
	 CDC& mdc = MemDC.GetDC();

// 	 rcShow.DeflateRect(1,1);
	 CRect rcShow = rcComboBox;
	 rcShow.OffsetRect(0,0);  
  	 rcShow.right = rcComboBox.right-slideWidth;
 	 rcShow.bottom -= 1;

 	 CRect rcComboThump = rcComboBox;
	 rcComboThump.left = rcComboBox.right - slideWidth;
     
	 mdc.SetStretchBltMode(COLORONCOLOR);
	 if ( m_imgComboBoxThump.IsNull() == false )
	 {
		 m_imgComboBoxThump.Draw( mdc, rcComboThump ); 
	 }
	// DrawIconEx(mdc, rcComboThump.left, rcComboThump.top, m_hIconBoxThump, rcComboThump.Width(), rcComboThump.Height(), 0, 0, DI_NORMAL);
	 mdc.SetBkMode(TRANSPARENT);

	 CBrush pBrush;
 
  	 if ( IsWindowEnabled() )
	 {
 		 pBrush.CreateSolidBrush(RGB(255,255,255));
	 }
	 else
	 {
		 pBrush.CreateSolidBrush(RGB(216,220,216));
 	 }
   	 mdc.FillRect( rcShow, &pBrush );
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
 
	// mdc.SelectObject(m_hBitmapBoxThump);
// 	 dc.StretchBlt(rcComboThump.left,rcComboThump.top,rcComboThump.Width(),rcComboThump.Height(),&mdc,0,0,33,28,SRCCOPY);

 }
  
void CIconComboBox::OnMouseMove(UINT nFlags, CPoint point)
{
	TRACKMOUSEEVENT tme;
	tme.cbSize = sizeof(TRACKMOUSEEVENT);
	tme.hwndTrack = m_hWnd;
	tme.dwFlags = TME_HOVER | TME_LEAVE;
	tme.dwHoverTime = 1;//HOVER_DEFAULT;
	TrackMouseEvent(&tme);

	CComboBox::OnMouseMove(nFlags, point);
}

LRESULT CIconComboBox::OnMouseHover( WPARAM wParam, LPARAM lParam )
{
	if (!m_bIsThumpHover)
	{
		COMBOBOXINFO comboBoxInfo;
		comboBoxInfo.cbSize = sizeof(COMBOBOXINFO);
		GetComboBoxInfo(&comboBoxInfo);

		CRect rcEdit;
		::GetClientRect(comboBoxInfo.hwndItem, &rcEdit);
		CRect rcComboThump = comboBoxInfo.rcButton;
		rcComboThump.left = 22 + rcEdit.Width();
		rcComboThump.top = 5;
		rcComboThump.bottom = 23;

		CPoint pt;
		pt.x = LOWORD(lParam);
		pt.y = HIWORD(lParam);

		if (rcComboThump.PtInRect(pt))
		{
			m_bIsThumpHover = TRUE;
			m_bIsThumpDown = wParam == MK_LBUTTON;
			Invalidate();
		}
	}

	return 0;
}

LRESULT CIconComboBox::OnMouseLeave( WPARAM wParam, LPARAM lParam )
{
	m_bIsThumpHover = FALSE;
	m_bIsThumpDown = FALSE;
	Invalidate();

	return 0;
}

int CIconComboBox::AddString( LPCTSTR lpszString, DWORD dwData )
{
	int nItem = CComboBox::AddString(lpszString);
	//if ( dwData != NULL )
	{
		SetItemData( nItem, dwData );
 	}
	return nItem;
}
 
void CIconComboBox::OnCbnSelchange()
{
	int nItem = GetCurSel();
	if (nItem == -1) return;
	DWORD data = GetItemData(nItem);
    
  	if (get_icon( data ))
	{
		Invalidate();
	}

	SendMessageToParent(nItem,data,WM_CUSTOMCOMBOX_SELCHANGED); 
}
 
void CIconComboBox::set_DropDownHight(CComboBox& box, UINT LinesToDisplay)  
{  
	ASSERT(IsWindow(box));  
 
	CRect cbSize;  
	int Height;  

	box.GetClientRect(cbSize);   
	Height = box.GetItemHeight(-1);   
	Height += box.GetItemHeight(0) * LinesToDisplay;  

	Height += GetSystemMetrics(SM_CYEDGE) * 2;  

	Height += GetSystemMetrics(SM_CYEDGE) * 2;  

	box.SetWindowPos(NULL,  
		0, 0,  
		cbSize.right, Height,  
		SWP_NOMOVE | SWP_NOZORDER  
		);   
     
}  

void CIconComboBox::set_DropDownWidth( CComboBox& box, UINT nWidth )  
{  
	ASSERT(IsWindow(box));  
	box.SetDroppedWidth(nWidth);  
}  

void CIconComboBox::PreSubclassWindow()
{
	// TODO: Add your specialized code here and/or call the base class
	if (::IsWindow(m_hWnd))
	  ModifyStyle(NULL, CBS_DROPDOWNLIST | CBS_SORT |  CBS_OWNERDRAWFIXED | CBS_HASSTRINGS | WS_VSCROLL );
	CComboBox::PreSubclassWindow();
}

LRESULT CIconComboBox::SendMessageToParent( int nIndex, DWORD data, int nMessage ) const
{
	if (!IsWindow(m_hWnd))
		return 0;

	NM_COMBOXVIEW nmgv;
	nmgv.currSelIndex = nIndex;
	nmgv.data         = data;
	nmgv.hdr.hwndFrom = m_hWnd;
	nmgv.hdr.idFrom   = GetDlgCtrlID();
	nmgv.hdr.code     = nMessage;
  
	CWnd* pWnd = GetParent();
	if ( pWnd != NULL && ::IsWindow(pWnd->m_hWnd) )
	{
		if ( m_bIsCustomMessage )  // 	ON_NOTIFY( WM_CUSTOMCOMBOX_SELCHANGED, IDC_TEACHDLG_TEACHINFO+1, &CCoSysInfo::OnSelchangeRef)
		{
			pWnd->SendMessage(WM_NOTIFY, nmgv.hdr.idFrom, (LPARAM)&nmgv);
 		}
		else // ON_CBN_SELCHANGE( IDC_TEACHDLG_TEACHINFO+1, &CCoSysInfo::OnSelchangeRef)
		{
			int nID = GetDlgCtrlID();
			LPARAM lParam = MAKELPARAM(m_hWnd, 0);
			WPARAM wParam = MAKEWPARAM( nID, CBN_SELCHANGE );
			pWnd->SendMessage(WM_COMMAND,  wParam, lParam );
 		}
		
	}

	return 1;

}

HICON CIconComboBox::get_icon( DWORD  dwData )
{
	int hiByte = HIBYTE(dwData);
 	if ( hiByte < 0 || hiByte > 2)
 	{
		return NULL;
 	}
	return m_hItemIcon[hiByte];
}

void CIconComboBox::SetFontType(int fontSize, CString fontType) 
{  
	m_font.DeleteObject();
	LOGFONT lf;  
	memset(&lf, 0, sizeof(LOGFONT));  
	lf.lfHeight = fontSize;  
	lf.lfWeight = 650;
	_tcsncpy_s(lf.lfFaceName, LF_FACESIZE, fontType, fontType.GetLength());  
	VERIFY(m_font.CreateFontIndirect(&lf));  
}

HBRUSH CIconComboBox::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CComboBox::OnCtlColor(pDC, pWnd, nCtlColor);
	// 	if ( nCtlColor == CTLCOLOR_EDIT && !m_hWndList)
	// 	{
	// 		pWnd->ShowWindow(SW_HIDE);
	// 		SkinWndScroll(pWnd,m_hBmpScroll);
	// 		m_hWndList=pWnd->m_hWnd;
	// 		ShowDropDown(TRUE);
	// 		ShowDropDown(FALSE);
	// 
	// 	}
	// 	else 
	// 	if (nCtlColor == CTLCOLOR_LISTBOX && !this->m_lstBox.m_hWnd  )
	// 	{
	// 		//第一次弹出下拉列表，先隐藏列表
	// 		pWnd->ShowWindow(SW_HIDE);
	// 
	// 		m_lstBox.m_hWnd=pWnd->m_hWnd;
	// 	}
  
	if ( nCtlColor == CTLCOLOR_EDIT )
 		m_hWndEdit = pWnd->GetSafeHwnd();
 
	if ( !m_bTopAlign )
 		return hbr;
 
	TCHAR clsname[MAX_PATH];
	GetClassName(pWnd->m_hWnd, clsname, MAX_PATH);

	CString strClsName;
	strClsName = clsname;
	if(strClsName.CompareNoCase(_T("ComboLBox")) == 0) //CTLCOLOR_EDIT
 	{
		CRect rc, rcThis;
		pWnd->GetWindowRect(&rc);
 		
		GetWindowRect(&rcThis);
		 
		pWnd->MoveWindow(rc.left, rcThis.top - rc.Height() - 1, rc.Width(), rc.Height(), TRUE);
 
		m_hWndList = pWnd->GetSafeHwnd();
	}

	return hbr;
}
 
void CIconComboBox::OnLButtonDown(UINT nFlags, CPoint point)
{
	TRACE(_T(" LB down\n"));
	if ( !m_bAllowPopupNested && MWM.is_popup_window_active() )
	{
		return;
	}
 	CComboBox::OnLButtonDown(nFlags, point);

//  	if ( m_hWndList )
// 	{
// 		CRect rc;
// 		GetWindowRect(&rc);
// 		::SetWindowPos(m_hWndList,HWND_TOPMOST,rc.left,rc.top - 182, rc.Width(), 182, SWP_SHOWWINDOW );//SWP_NOACTIVATE | SWP_NOMOVE
// 	} 

}

void CIconComboBox::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	TRACE(_T(" LB DBclk\n"));
	if ( !m_bAllowPopupNested && MWM.is_popup_window_active() )
	{
		return;
	}
	CComboBox::OnLButtonDblClk(nFlags, point);

}

BOOL CIconComboBox::PreTranslateMessage(MSG* pMsg)
{
	if ( pMsg->hwnd == m_hWndEdit && MWM.is_popup_window_active() )
	{
		return TRUE;
	}

	return CComboBox::PreTranslateMessage(pMsg);
}

 