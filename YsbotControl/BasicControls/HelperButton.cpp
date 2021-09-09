
// HelperButton.cpp : implementation file
//

#include "stdafx.h"

#include "HelperButton.h"


// CHelperButton

IMPLEMENT_DYNAMIC(CHelperButton, CButtonST)
	CHelperButton::CHelperButton()
{
}

CHelperButton::~CHelperButton()
{
}


BEGIN_MESSAGE_MAP(CHelperButton, CButtonST)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()



// CHelperButton message handlers


void CHelperButton::OnLButtonDown(UINT nFlags, CPoint point)
{
	HWND  hParent = ::GetParent(this->m_hWnd);

	if( hParent )
	{
		::SendMessage(
			hParent, 
			WM_COMMAND, 
			(WPARAM) MAKELONG(::GetDlgCtrlID(this->m_hWnd), WM_LBUTTONDOWN), 
			(LPARAM) this->m_hWnd);
	}

	CButtonST::OnLButtonDown(nFlags, point);
}

void CHelperButton::OnLButtonUp(UINT nFlags, CPoint point)
{
	HWND  hParent = ::GetParent(this->m_hWnd);

	if( hParent )
	{
		::SendMessage(
			hParent, 
			WM_COMMAND, 
			(WPARAM) MAKELONG(::GetDlgCtrlID(this->m_hWnd), WM_LBUTTONUP), 
			(LPARAM) this->m_hWnd);
	}

	CButtonST::OnLButtonUp(nFlags, point);
}

