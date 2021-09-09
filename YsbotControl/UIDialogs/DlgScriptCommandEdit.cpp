// Copyright 2016, 亚思信息科技有限责任公司
// All right reserved.
//
// Author: Liu Run Dan ( lrdcz@163.com )
//
// Yasi Robot Motion Control System - 亚思机器人运动控制系统

// DlgScriptCommandEdit.cpp : implementation file
//

#include "stdafx.h"
#include "../YsbotControl.h"
#include "DlgScriptCommandEdit.h"
#include "../colourcode.h" 
#include "InfoBox.h"
#include "../program_dialog.h"
#include "../WorldModel.h"

// CDlgScriptCommandEdit dialog

IMPLEMENT_DYNAMIC(CDlgScriptCommandEdit, DialogType)

CDlgScriptCommandEdit::CDlgScriptCommandEdit(LPRECT pRect)
{
	if ( pRect )
 		m_rcDlgPos = CRect(pRect);
 	else
		m_rcDlgPos = CRect(CPoint(19,481),CSize(729,53));
	radio_list.clear();
	m_bMaxWin = true;
}

CDlgScriptCommandEdit::~CDlgScriptCommandEdit()
{
	m_font.DeleteObject();
	for (unsigned int i = 0; i < radio_list.size(); ++i )
	{
		delete radio_list[i];
	}
	radio_list.clear();
}

void CDlgScriptCommandEdit::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgScriptCommandEdit, DialogType)
	ON_COMMAND_RANGE( IDE_DLGSCRIPTCOMMANDEDIT_BTLEFT, IDE_DLGSCRIPTCOMMANDEDIT_BTRIGHT, OnButtonClicked )
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CDlgScriptCommandEdit message handlers
BOOL CDlgScriptCommandEdit::OnInitDialog() 
{
	CDialogEx::OnInitDialog();
	SetWindowPos(NULL, m_rcDlgPos.left, m_rcDlgPos.top, m_rcDlgPos.Width(), m_rcDlgPos.Height(), SWP_NOZORDER);
	SetBackgroundColor(kColor[8]); 

	m_font.CreateFontW(18, 0, 0, 0, 680, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
		OUT_STROKE_PRECIS, CLIP_STROKE_PRECIS, DRAFT_QUALITY, FIXED_PITCH |
		FF_MODERN, _T("Arial"));

	CRect rcClient,editrect,rcButton[2];
 	calc_rect( rcClient, editrect, rcButton[0], rcButton[1] );

	CRect rcRadio[2];
	rcRadio[0] = rcRadio[1] = CRect( CPoint(40,5), CSize(110,32) );
	rcRadio[1].OffsetRect(rcClient.Width()/2,0);
	for ( int i = 0; i < 2; ++i )
	{
		CPngRadio* pRadio = new CPngRadio();
		pRadio->Create(NULL,WS_CHILD|WS_GROUP,rcRadio[i],this,IDE_DLGSCRIPTCOMMANDEDIT_RADIO1+i);
		pRadio->SetFontType(20,_T("Arial"));
		pRadio->InitImage(NULL,_T("res/radio-slelected.bmp"),_T("res/radio-empty.bmp"),_T("res/radio-empty_disable.bmp"),_T("res/radio-slelected_disable.bmp") );
		radio_list.push_back(pRadio);
 	}
	radio_list[0]->SetWindowText(_T("添加"));
	radio_list[1]->SetWindowText(_T("括号选择"));
	CPngRadio::AddGroup(radio_list);  
	radio_list[0]->SetChecked(TRUE);
  
	m_edit.Create( WS_CHILD | WS_VISIBLE | WS_BORDER, editrect, this, IDE_DLGSCRIPTCOMMANDEDIT_EDIT, TRUE ); // | WS_TABSTOP
	m_edit.SetFont(&m_font);
	m_edit.SetRulePlan(KBRule_Program);
	m_edit.SetHighLightText(TRUE);
	m_edit.SetWindowText(_T(" edit program! "));
 
	for( int i = 0; i < 2; ++i)
	{
		m_button[i].Create(_T(""),	WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE, rcButton[i], this, IDE_DLGSCRIPTCOMMANDEDIT_BTLEFT+i);
  	}

	m_button[0].SetIcon(IDI_LEFT_ARROW);
	m_button[1].SetIcon(IDI_RIGHT_ARROW);

	m_button[0].SetFocus();



	return FALSE;
}

LRESULT CDlgScriptCommandEdit::OnMainButtonClicked( WPARAM wp, LPARAM lp)
{
	int btnid  = (int)(wp);
	int menuid = (int)(lp);
 	CString cmdText = _T("");
	m_edit.GetWindowText(cmdText);
	int nEditType = 0;
 	switch( btnid )
	{
	case 0:		//	更改
	    FKEY->activate_keyboard(m_edit.GetSafeHwnd(),cmdText,0,0);
		break;
	case 1:		//	替换
  		break;
	case 2:		//	新增
		nEditType = 2; // 1: before add, 2: after add, 
 		break;
	case 3:		//	删除
		nEditType = 3;  
 		break;
	case 4:		//	取消
		break;
	case 5:		//	确定
		{
			if ( m_bMaxWin )
 				nEditType = 2;
  		}
		break;
	default: break;
	}

	if ( btnid > 0 )
	{
		if ( btnid != 4 )
		{
			program_dialog->modify_command_from_wizards( cmdText, nEditType, true, m_bMaxWin, radio_list[1]->GetChecked() ? true : false );
		}
		GetParent()->GetParent()->SendMessage(WM_ACTIVATE_WINDOW,winProgram);
 	}

	return 1;
}

void CDlgScriptCommandEdit::OnButtonClicked(UINT id)
{
	if ( MWM.is_popup_window_active() )
		return;

	unsigned int bnt_index = id - IDE_DLGSCRIPTCOMMANDEDIT_BTLEFT;
	 
	if ( bnt_index == 0 ) // left
	{
		std::cout << " left arrow clicked.\n";
	}
	else // right
	{
		std::cout << " right arrow clicked.\n";
 	}

}

void CDlgScriptCommandEdit::OnSize(UINT nType, int cx, int cy)
{
	DialogType::OnSize(nType, cx, cy);
	if ( nType == SIZE_RESTORED && ::IsWindow( m_edit.GetSafeHwnd() ) )
	{
		CRect rcClient,editrect,rcButton[2];
  		calc_rect(rcClient,editrect,rcButton[0],rcButton[1]);
 		m_edit.MoveWindow (&editrect);
		m_button[0].MoveWindow(&rcButton[0]);
		m_button[1].MoveWindow(&rcButton[1]);
	}
	// TODO: Add your message handler code here
}

void CDlgScriptCommandEdit::calc_rect( CRect& rcClient, CRect& rcEdit, CRect& rcBtLeft, CRect& rcBtRight )
{
 	GetClientRect(&rcClient); //取客户区大小
	int width  = 40;
	int height = 44;

 	if ( rcClient.Height() < m_rcDlgPos.Height()/2+11 )
	{
		for (unsigned int i = 0; i < radio_list.size(); ++i )
		{
			radio_list[i]->ShowWindow(SW_HIDE);
		}
		m_bMaxWin = false;
	}
	else
	{
		for (unsigned int i = 0; i < radio_list.size(); ++i )
		{
			radio_list[i]->ShowWindow(SW_SHOW);
		}
		m_bMaxWin = true;
 	}

	CRect rcButton[2];
	rcButton[0] = rcClient;
	rcButton[0].top    = rcClient.bottom - height - 4;
	rcButton[0].left   = rcClient.left + 2;
	rcButton[0].right  = rcButton[0].left + width;
	rcButton[0].bottom = rcClient.bottom - 4;
	rcButton[1] = rcButton[0];

	CRect editrect( CPoint(rcButton[0].right,rcButton[0].top), CSize(rcClient.Width()-width*2-6,height) );
// 	editrect.left    = rcButton[0].right;
// 	editrect.top     = rcButton[0].top;
// 	editrect.bottom  = rcButton[0].top + height;
	rcButton[1].OffsetRect(editrect.right,0);

	rcEdit    = editrect;
	rcBtLeft  = rcButton[0];
	rcBtRight = rcButton[1];

 
}
