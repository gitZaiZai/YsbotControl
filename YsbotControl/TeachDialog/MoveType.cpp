#include "stdafx.h"
#include "../TeachDialog/MoveType.h"

//CMoveType Frame
BEGIN_MESSAGE_MAP(CMoveType, CGroupFrame)
	ON_WM_CREATE()
	ON_CONTROL_RANGE( BN_CLICKED ,IDC_TEACHDLG_MOVETYPE+1, IDC_TEACHDLG_MOVETYPE+2, OnBtnClicked )
END_MESSAGE_MAP()

CMoveType::CMoveType() 
{
 ;
}

CMoveType::~CMoveType() 
{

}

BOOL CMoveType::Create(CString caption,DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	_nID=nID;
	CStatic::Create(caption, dwStyle|SS_NOTIFY, rect, pParentWnd, nID);
	Initialize(rect, pParentWnd, nID);		
	return TRUE;
}

int CMoveType::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	label_[0].Create(_T("直线"),WS_CHILD|WS_VISIBLE,CRect(80,38,130,70),this);
 	label_[1].Create(_T("点到点"),WS_CHILD|WS_VISIBLE,CRect(250,38,330,70),this);
 	for ( int i = 0; i < 2; ++i )
	{
		label_[i].SetAlignment(1);
		label_[i].Initialize(kColor[0]);
		label_[i].SetFontType(20,350,_T("宋体"));
	}
	btnMoveTpye[MovePTP].Create(_T(""),WS_CHILD|WS_VISIBLE|WS_GROUP ,CRect(220,35,250,65),this,IDC_TEACHDLG_MOVETYPE+1);
	btnMoveTpye[MoveLine].Create(_T(""),WS_CHILD|WS_VISIBLE ,CRect(50,35,80,65),this,IDC_TEACHDLG_MOVETYPE+2);
	//btnMoveTpye[MoveLine].InitImage(IDB_PNG_UNCHECK,IDB_PNG_CHECKED,IDB_PNG_DISABLECHECK);
	//btnMoveTpye[MovePTP].InitImage(IDB_PNG_UNCHECK,IDB_PNG_CHECKED,IDB_PNG_DISABLECHECK);
	btnMoveTpye[MoveLine].InitImage(_T("res/radio-empty.bmp"),_T("res/radio-slelected.bmp"),_T("res/radio-empty_disable.bmp"),_T("res/radio-slelected_disable.bmp"));
	btnMoveTpye[MovePTP].InitImage(_T("res/radio-empty.bmp"),_T("res/radio-slelected.bmp"),_T("res/radio-empty_disable.bmp"),_T("res/radio-slelected_disable.bmp"));

	SetMoveType( MovePTP );

	return 0;
}

void CMoveType::OnBtnClicked(unsigned int btnID)
{
	unsigned int nID=btnID-IDC_TEACHDLG_MOVETYPE-1;
	SetMoveType((movetype)nID);
}

void CMoveType::SetMoveType( movetype mTpye )
{
 	if ( mTpye == MovePTP )
	{
		btnMoveTpye[MovePTP].SetChecked(1);
		btnMoveTpye[MoveLine].SetChecked(0);
	}
	else if ( mTpye == MoveLine)
	{
		btnMoveTpye[MovePTP].SetChecked(0);
		btnMoveTpye[MoveLine].SetChecked(1);
 	}
  
	CWnd* pWnd = GetParent();
	if ( pWnd != NULL && ::IsWindow(pWnd->m_hWnd) )
	{
 		pWnd->SendMessage(WM_TEACHDLG_MOVETYPE,(WPARAM)mTpye);
 	}
}

void CMoveType::Enable(bool b)
{
	for ( int i = 0; i < 2; ++i)
	{
		btnMoveTpye[i].EnableButton(b);
	}
}

unsigned int CMoveType::GetMoveType()
{
	if ( btnMoveTpye[MovePTP].GetChecked() )
	{
		return (unsigned int)(MovePTP);
	}
	else if ( btnMoveTpye[MoveLine].GetChecked() )
	{
		return (unsigned int)(MoveLine);
 	}
	return (unsigned int)(MovePTP);
}