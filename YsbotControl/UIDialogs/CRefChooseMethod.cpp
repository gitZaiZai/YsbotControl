// Copyright 2016, 亚思信息科技有限责任公司
// All right reserved.
//
// Author: Liu Run Dan ( lrdcz@163.com )
//
// Yasi Robot Motion Control System - 亚思机器人运动控制系统

#include "stdafx.h"
#include "CRefChooseMethod.h"
#include "../MotionControl.h"
#include "../PosInfo.h"
 
BEGIN_MESSAGE_MAP(CRefChooseMethod, CGroupFrame)
 	ON_COMMAND_RANGE( REFMETHOD_RADIO_BUTTON_1, REFMETHOD_RADIO_BUTTON_1+2, &CRefChooseMethod::OnRadioButtonClicked )

END_MESSAGE_MAP()

CRefChooseMethod::CRefChooseMethod():m_nRadioType(0)
{
 	radio_method_list.clear();
}

CRefChooseMethod::~CRefChooseMethod() 
{
	for (unsigned int i = 0; i < radio_method_list.size(); ++i )
	{
		delete radio_method_list[i];
	}
	radio_method_list.clear();

}

BOOL CRefChooseMethod::Create(CString caption,DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	CGroupFrame::Create(caption, dwStyle|SS_NOTIFY, rect, pParentWnd, nID);
	//Initialize(rect, pParentWnd, nID);		
	_nID=nID;

	font_.CreateFont(20, 0, 0, 0, 0, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
		OUT_STROKE_PRECIS, CLIP_STROKE_PRECIS, DRAFT_QUALITY, FIXED_PITCH |
		FF_MODERN, _T("宋体"));

	toolTitle.Create(_T("示教使用的工具"), WS_CHILD | WS_VISIBLE,
		CRect(CPoint(10,255),CSize(140,30)), this);
	toolTitle.SetAlignment(1);
	toolTitle.Initialize(kColor[0]);
	toolTitle.SetFontType(20,350,_T("Arial"));

	COLORREF m_crBackgroud = RGB(220, 220, 220);
	COLORREF m_crText      = RGB(96, 100, 96);

	toolLabel.Create( _T(""), WS_CHILD | WS_VISIBLE | DT_LEFT | SS_CENTERIMAGE,
		CRect(CPoint(160,250),CSize(175,30)), this, 65535 );
	toolLabel.SetBkColor(m_crBackgroud)
		.SetTextColor(m_crText)
		.SetBorder(TRUE)
		.SetFontBold(TRUE)
		.SetText(_T("Flange"))
		.SetLabelIcon(IDI_VARIABLE_SYSTEM,16,16);

	if( !m_GroupBox.GetSafeHwnd() )
	{
		m_GroupBox.Create(_T(""),BS_GROUPBOX|WS_VISIBLE|WS_CHILD,CRect(CPoint(5,220),CSize(340,80)),this,8888);
	} 

	if ( !m_GroupBoxRadio.GetSafeHwnd())
	{
		m_GroupBoxRadio.Create(_T(""),BS_GROUPBOX|WS_VISIBLE|WS_CHILD,CRect(CPoint(5,20),CSize(340,200)),this,8888);
 	}

	int x0=10,y0=143,w0=100,h0=25,Hspace=30,Vspace=180;
	int x1=37,y1=140,w1=95,h1=25;
 

	CRect rcra[3];
	rcra[0] = rcra[1] = rcra[2] = CRect(10,60,300,92);

	for ( int i = 0; i < 3; ++i )
	{
		rcra[i].OffsetRect(0,40*i);
		CPngRadio* pRadio = new CPngRadio();
		pRadio->Create(NULL,WS_CHILD|WS_VISIBLE|WS_GROUP,rcra[i],this,REFMETHOD_RADIO_BUTTON_1+i);
		pRadio->SetFontType(20,_T("Arial"));
// 		pRadio->InitIcon(_T("res/radio-slelected.ico"),_T("res/radio-empty.ico"),_T("res/radio-empty_disable.ico"),_T("res/radio-slelected_disable.ico") );
		pRadio->InitImage(NULL,_T("res/radio-slelected.bmp"),_T("res/radio-empty.bmp"),_T("res/radio-empty_disable.bmp"),_T("res/radio-slelected_disable.bmp") );
		radio_method_list.push_back(pRadio); 
	}
	radio_method_list[0]->SetWindowText(_T("三点法（有原点）"));
	radio_method_list[1]->SetWindowText(_T("三点法（无原点）"));
	radio_method_list[2]->SetWindowText(_T("一点法（姿态与基坐标系相同）"));
	CPngRadio::AddGroup(radio_method_list);  
	radio_method_list[0]->SetChecked(TRUE);

	return TRUE;
}

 
 
void CRefChooseMethod::OnRadioButtonClicked(UINT id)
{
	m_nRadioType = id - REFMETHOD_RADIO_BUTTON_1;
	GetParent()->SendMessage(WM_CHANGE_CALIBRATIONUSER,m_nRadioType);

	if ( m_nRadioType == 0 )
	{
		 
	}
	else if ( m_nRadioType == 1 )
	{
		 
	}
	else if ( m_nRadioType == 2 )
	{
		 
	}
	cout << "method type = " << m_nRadioType << '\n';
}

void CRefChooseMethod::reset()
{
	m_nRadioType = 0;
	radio_method_list[0]->SetChecked(TRUE);
}