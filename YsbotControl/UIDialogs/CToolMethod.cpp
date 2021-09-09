// Copyright 2016, 亚思信息科技有限责任公司
// All right reserved.
//
// Author: Liu Run Dan ( lrdcz@163.com )
//
// Yasi Robot Motion Control System - 亚思机器人运动控制系统

#include "stdafx.h"
#include "CToolMethod.h"
#include "../MotionControl.h"
#include "../PosInfo.h"

BEGIN_MESSAGE_MAP(CToolMethod, CGroupFrame)
 	ON_COMMAND_RANGE( TOOLMETHOD_RADIO_BUTTON_1, TOOLMETHOD_RADIO_BUTTON_1+2, &CToolMethod::OnRadioButtonClicked )

END_MESSAGE_MAP()

CToolMethod::CToolMethod():m_nRadioType(0)
{
	radio_method_list.clear();
}

CToolMethod::~CToolMethod() 
{
	for (unsigned int i = 0; i < radio_method_list.size(); ++i )
	{
		delete radio_method_list[i];
	}
	radio_method_list.clear();

}

BOOL CToolMethod::Create(CString caption,DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	CGroupFrame::Create(caption, dwStyle|SS_NOTIFY, rect, pParentWnd, nID);
	//Initialize(rect, pParentWnd, nID);		
	_nID=nID;

	font_.CreateFont(20, 0, 0, 0, 0, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
		OUT_STROKE_PRECIS, CLIP_STROKE_PRECIS, DRAFT_QUALITY, FIXED_PITCH |
		FF_MODERN, _T("宋体"));

	toolTitle[0].Create(_T("保持姿态"), WS_CHILD | WS_VISIBLE,
		CRect(CPoint(20,50),CSize(140,30)), this);
	toolTitle[0].SetAlignment(1);
	toolTitle[0].Initialize(kColor[0]);
	toolTitle[0].SetFontType(20,350,_T("Arial"));

	toolTitle[1].Create(_T("改变姿态"), WS_CHILD | WS_VISIBLE,
		CRect(CPoint(20,170),CSize(140,30)), this);
	toolTitle[1].SetAlignment(1);
	toolTitle[1].Initialize(kColor[0]);
	toolTitle[1].SetFontType(20,350,_T("Arial"));

	COLORREF m_crBackgroud = RGB(220, 220, 220);
	COLORREF m_crText      = RGB(96, 100, 96);
 
	int x0=10,y0=143,w0=100,h0=25,Hspace=30,Vspace=180;
	int x1=37,y1=140,w1=95,h1=25;


	CRect rcra[3];
	rcra[0] = rcra[1] = rcra[2] = CRect(30,80,300,112);

	for ( int i = 0; i < 3; ++i )
	{
		if ( i == 2 )
  			rcra[i].OffsetRect(0,60*i);
 		else
			rcra[i].OffsetRect(0,40*i);

		CPngRadio* pRadio = new CPngRadio();
		pRadio->Create(NULL,WS_CHILD|WS_VISIBLE|WS_GROUP,rcra[i],this,TOOLMETHOD_RADIO_BUTTON_1+i);
		pRadio->SetFontType(20,_T("Arial"));
		pRadio->InitImage(NULL,_T("res/radio-slelected.bmp"),_T("res/radio-empty.bmp"),_T("res/radio-empty_disable.bmp"),_T("res/radio-slelected_disable.bmp") );
		radio_method_list.push_back(pRadio); 
	}
	radio_method_list[0]->SetWindowText(_T("一点法XYZ"));
	radio_method_list[1]->SetWindowText(_T("四点法XYZ"));
	radio_method_list[2]->SetWindowText(_T("一点法6D"));
	CPngRadio::AddGroup(radio_method_list);  
	radio_method_list[0]->SetChecked(TRUE);

	return TRUE;
}
  
void CToolMethod::OnRadioButtonClicked(UINT id)
{
	m_nRadioType = id - TOOLMETHOD_RADIO_BUTTON_1;
	GetParent()->SendMessage(WM_CHANGE_TOOLMETHOD,m_nRadioType);

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

void CToolMethod::reset()
{
	m_nRadioType = 0;
	radio_method_list[0]->SetChecked(TRUE);
}