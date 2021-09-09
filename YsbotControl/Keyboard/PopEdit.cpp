#include "stdafx.h"
#include "PopEdit.H" 

#include "digital_keyboard.h"
#include "full_qwerty.h"
#include "../WorldModel.h"

//CpopEdit Class
CpopEdit::CpopEdit()
{
	KB_=nullptr;
	MaxVal=10000;
	MinVal=-10000;
	Decimals=4;

	m_userUpdate      = false;
	m_allowEnChange   = false;
	m_useFullKeyboard = FALSE;
	m_bHighLightText  = FALSE;
	m_bAllowPopupNested = FALSE;

	m_BrushEnable.CreateSolidBrush(RGB(255,255,255));
 	m_BrushDisable.CreateSolidBrush(RGB(216,220,216)); 
    m_BrushFrame.CreateSolidBrush(RGB(0,0,0));
	m_BrushTextBk.CreateSolidBrush(RGB(88,148,24)); 
 	 
}

CpopEdit::~CpopEdit()
{
	m_BrushEnable.DeleteObject();
	m_BrushDisable.DeleteObject();
	m_BrushFrame.DeleteObject();
	m_BrushTextBk.DeleteObject();
}

IMPLEMENT_DYNAMIC(CpopEdit, CEdit)

BEGIN_MESSAGE_MAP(CpopEdit, CEdit)
// 	ON_WM_CREATE()
// 	ON_WM_CTLCOLOR()
 	ON_WM_LBUTTONDBLCLK()
	ON_WM_PAINT()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_CONTROL_REFLECT(EN_UPDATE, OnUpdate)
	ON_CONTROL_REFLECT_EX(EN_CHANGE, OnChangeReflect)
	ON_MESSAGE( WM_KEYBOARD_ENTER, &CpopEdit::OnKeyboardEnter )
	ON_MESSAGE( WM_KEYBOARD_CANCLE, &CpopEdit::OnKeyboardCancle    )
END_MESSAGE_MAP()

BOOL CpopEdit::Create( DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, BOOL bFullKeyboard )
{
	m_useFullKeyboard = bFullKeyboard;
 	BOOL ret = CEdit::Create(dwStyle, rect,pParentWnd, nID);
 	return ret;
}

// int CpopEdit::OnCreate(LPCREATESTRUCT lpCreateStruct) 
// {
//  	return 0;
// }

// HBRUSH CpopEdit::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
// {
// 	HBRUSH hbr =CEdit::OnCtlColor(pDC, pWnd, nCtlColor);
//  
// 	return hbr;
// }

void CpopEdit::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CString str;
	GetWindowTextW(str);
	
	if ( KB_ )
	{
//   		KB_->SetArea( CRect(22, 95, 740, 535) );
// 		KB_->set_original_value(str,1,this);
// 		KB_->SetValueRange(MinVal,MaxVal,Decimals);
// 		KB_->SetWindowPos(&wndTop,0,0,0,0, SWP_NOMOVE | SWP_NOSIZE);
	}
	
}

void CpopEdit::SetValueRange(double minVal,double maxVal,short decimals)
{
	MaxVal=maxVal;
	MinVal=minVal;
	Decimals=decimals;
}

void CpopEdit::SetRulePlan(unsigned short ruleIndex) 
{
	if ( m_useFullKeyboard && Singleton<FullQwerty>::IsAvaliable() )
	{
		FKEY->SetRulePlan(ruleIndex);
	}
}

void CpopEdit::set_rule(CString rule) 
{
	if ( m_useFullKeyboard)
	{
		FKEY->set_rule(rule);
	}
}
//////////////////////////////////////////////////////////////////////////
void CpopEdit::run_keyboard()
{
 	m_userUpdate = true;

	GetParent()->RedrawWindow(); // releas different from debug. liurundan

	CString str;
	GetWindowTextW(str);

	if ( m_useFullKeyboard )
	{
		FKEY->activate_keyboard( GetSafeHwnd(), str, _T(""), 0 );
	}
	else
	{
		DKEY->activate_keyboard( GetSafeHwnd(),str,_T("") );
		DKEY->SetArea( CRect(22, 95, 740, 535) );
 		DKEY->SetValueRange(MinVal,MaxVal,Decimals);
		DKEY->SetWindowPos(&wndTop,0,0,0,0, SWP_NOMOVE | SWP_NOSIZE);
	}
}

void CpopEdit::OnLButtonUp(UINT nFlags, CPoint point) 
{
	CEdit::OnLButtonUp(nFlags, point);
 
// 	if ( !(GetStyle() & ES_READONLY))
// 		run_keyboard();
}

void CpopEdit::OnLButtonDown(UINT nFlags, CPoint point) 
{
 	if ( m_bAllowPopupNested )
	{
		if ( FKEY->IsWindowVisible() || DKEY->IsWindowVisible() )
 			return;
 	}
	else
	{
		if ( MWM.is_popup_window_active() )
 			return;
 	}

	CEdit::OnLButtonDown(nFlags, point);
	SetFocus();
	if ( !(GetStyle() & ES_READONLY))
	{
		run_keyboard();
	}

}

void CpopEdit::OnUpdate()
{
// 	if ( m_userUpdate )
// 		run_keyboard();
}
 
BOOL CpopEdit::OnChangeReflect()
{
	// Return TRUE to block message to parent
	BOOL result = m_allowEnChange ? FALSE : TRUE;

	m_allowEnChange = false;

	return result;
}

LRESULT CpopEdit::OnKeyboardEnter(WPARAM wp,LPARAM lp)
{
	CString str_result = *((CString *)wp);
 
 	m_userUpdate = false;	// No dialog when we SetWindowText()
	SetWindowText(str_result);
	//m_id = GetDlgCtrlID();
	//m_userUpdate = true; 
	//GetParent()->SendMessage(WM_KEYBOARD_ENTER, wp, m_id );
	setValstr(str_result);
	GetParent()->SendMessage(WM_KEYBOARD_ENTER, wp, lp );
	return 1;
}

LRESULT CpopEdit::OnKeyboardCancle(WPARAM wp,LPARAM lp)
{
 	m_userUpdate = false;
	Invalidate(TRUE);
  	return 1;
} 

void CpopEdit::OnPaint()
{
	CPaintDC dcPaint(this); // device context for painting

	int begin, end;
	GetSel(begin, end);

	CRect rcEdit;
	GetClientRect(&rcEdit);
	
	CMemDC dcMem(dcPaint, rcEdit);
	CDC& dc = dcMem.GetDC();
    DWORD dstylte = 0;
	dc.SetBkMode(TRANSPARENT);
    
	BOOL bReadOnly = FALSE;
	bReadOnly = GetStyle() & ES_READONLY;

 	if ( ::IsWindowEnabled( GetSafeHwnd() ) )
	{
 		if ( bReadOnly )
		{
			dc.FillRect( rcEdit, &m_BrushDisable );
		}
		else
		{
			if ( m_userUpdate && !m_bHighLightText )
			{
				dc.FillRect( rcEdit, &m_BrushTextBk );
 			}
			else
				dc.FillRect( rcEdit, &m_BrushEnable);
 		}
 	}
	else
	{
		dc.FillRect( rcEdit, &m_BrushDisable );
 	}
 	 
  	dc.FrameRect(rcEdit,&m_BrushFrame);
  
	CFont* oldFont = dc.SelectObject( GetFont() );

	CString strItem ;
	GetWindowText(strItem);
  
	dstylte = GetStyle() & ES_RIGHT;
	if ( dstylte )
	{
 		rcEdit.OffsetRect(0,0);
		rcEdit.right -= 4;
	}
 	else
		rcEdit.OffsetRect(4,0);
  
	BOOL bPassword = GetStyle() & ES_PASSWORD;
	if ( bPassword )
	{
 		int npwLen = strItem.GetLength();
		strItem = _T("");
		for ( int i = 0; i < npwLen; ++i )
 			strItem += _T("*");
  	}

	if ( !bReadOnly && m_bHighLightText && !strItem.IsEmpty())
	{
		CRect rcText;
  		dc.DrawText( strItem, rcText, DT_CALCRECT); // 计算文字矩形大小
	 
		rcText.OffsetRect(2, (int)( ( rcEdit.Height()- rcText.Height() )/2 ) );
		rcText.InflateRect(0,2,3,3);
		dc.FillRect( rcText, &m_BrushTextBk );
 		dc.FrameRect(rcText, &m_BrushFrame );
	}
  
	if ( dstylte )
	{
		dc.DrawText(strItem,strItem.GetLength(),rcEdit,DT_RIGHT|DT_VCENTER|DT_SINGLELINE|DT_WORD_ELLIPSIS);
 	}
	else
		dc.DrawText(strItem,strItem.GetLength(),rcEdit,DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_WORD_ELLIPSIS);


	dc.SelectObject(oldFont);
}
 
void CpopEdit::refreshValue()
{
	CString s = getValstr();

	m_userUpdate = false;	// No dialog when we SetWindowText()
	SetWindowText(s);
	//m_userUpdate = true;
}

//-----	CEditPopup derived classes --------------------------------------------

void CpopEditLong::SetValue(long value)
{
	m_value = value;
	refreshValue();
}


CString CpopEditLong::getValstr()
{
	CString				s;

	s.Format(_T("%d"), m_value);

	return s;
}


bool CpopEditLong::setValstr(CString s)
{
	return (_stscanf(s, _T("%d"), &m_value) == 1);
}


void CpopEditDouble::SetValue(double value)
{
	m_value = value;
	refreshValue();
}


CString CpopEditDouble::getValstr()
{
	CString				s;
	s.Format(_T("%.*f"), iDecimalLoc , m_value);
	//s.Format(_T("%.3f"), m_value);

	return s;
}


bool CpopEditDouble::setValstr(CString s)
{
	return (_stscanf(s, _T("%lf"), &m_value) == 1);
}
