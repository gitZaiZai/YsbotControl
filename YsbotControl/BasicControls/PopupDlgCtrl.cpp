// PopupDlgCtrl.cpp : implementation file
//

#include "stdafx.h"

#include "PopupDlgCtrl.h"
#include "CstmComboBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CPopupDlgCtrl dialog

IMPLEMENT_DYNCREATE(CPopupDlgCtrl, CDialog)
CPopupDlgCtrl::CPopupDlgCtrl(CWnd* pParent /*=NULL*/)
	: CDialog(CPopupDlgCtrl::IDD, pParent)
{
}

CPopupDlgCtrl::~CPopupDlgCtrl()
{
}

void CPopupDlgCtrl::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//DDX_Control(pDX, IDC_SLIDER1, m_slder);
}


BEGIN_MESSAGE_MAP(CPopupDlgCtrl, CDialog)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CPopupDlgCtrl message handlers

BOOL CPopupDlgCtrl::OnInitDialog()
{
	CDialog::OnInitDialog();
	CRect rcSlder( 10, 10, 200, 30 );
	m_slder.Create( WS_CHILD | WS_VISIBLE,rcSlder,this, 65536 );
	m_slder.SetRange(0,100);
	return TRUE;
}

LRESULT CPopupDlgCtrl::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if(message==WM_PRINT)
		message=WM_PAINT;

	return CDialog::WindowProc(message, wParam, lParam);
}

bool CPopupDlgCtrl::CreatePopupCtrl(CWnd* pOwner)
{
	if(Create(CPopupDlgCtrl::IDD))
	{
		ModifyStyle(WS_VISIBLE,0);
		ModifyStyle(WS_OVERLAPPED ,WS_CLIPSIBLINGS );
		ModifyStyleEx(WS_EX_CLIENTEDGE|WS_EX_STATICEDGE,WS_EX_TOOLWINDOW);

		if(pOwner)
			SetOwner(pOwner);
		return true;
	}
	

	return false;
}
void CPopupDlgCtrl::OnCancel()
{
	if(((CCstmComboBox*)GetOwner())->GetDroppedState())
		((CCstmComboBox*)GetOwner())->ShowDropDown(FALSE);
}

void CPopupDlgCtrl::OnOK()
{
	if(((CCstmComboBox*)GetOwner())->GetDroppedState())
		((CCstmComboBox*)GetOwner())->ShowDropDown(FALSE);

	WPARAM wParam= MAKEWPARAM( GetOwner()->GetDlgCtrlID(),CBN_SELCHANGE);
	LPARAM lParam=(LPARAM)GetOwner()->m_hWnd;
	GetOwner()->PostMessage(WM_COMMAND,wParam,lParam);
}
int CPopupDlgCtrl::GetCount(void) const
{
	return -1;
}
int CPopupDlgCtrl::GetCurrentSelection( ) const
{
	return m_slder.GetRangeMax() ;
}
int CPopupDlgCtrl::SetCurrentSelection(int nSelect)
{
	m_slder.SetPos(nSelect) ;
	return 1;
}
void CPopupDlgCtrl::GetItemText(int nIndex,CString& rString) const
{
	rString.Format(_T("%d"),m_slder.GetPos());
}
int CPopupDlgCtrl::DeleteItem(UINT nIndex)
{
	return CB_ERR ;
}
int CPopupDlgCtrl::FindString(int nStartAfter,LPCTSTR lpszString) const
{
	return CB_ERR ;
}
int CPopupDlgCtrl::FindStringExact(int nStartAfter,LPCTSTR lpszString) const
{
	return CB_ERR ;
}
void CPopupDlgCtrl::RemoveAllItems()
{

}

