// Copyright 2016, ��˼��Ϣ�Ƽ��������ι�˾
// All right reserved.
#include "stdafx.h"
#include "InfoBox.h"
#include "../resource.h"
#include "JournalData.h"

BEGIN_MESSAGE_MAP(CInfoBox, DialogType)
	ON_BN_CLICKED(IDC_INFOBOX_BUTTON+1, BtnCancel)
	ON_BN_CLICKED(IDC_INFOBOX_BUTTON, BtnOK)
END_MESSAGE_MAP()

CInfoBox* CInfoBox::_theInfoBox = NULL;

CInfoBox&  CInfoBox::GetInfoBox () 
{ 
	return * CInfoBox::_theInfoBox; 
}
 
bool  CInfoBox::is_main_infobox_available ()  
{
	return _theInfoBox != NULL;
}
 
CInfoBox::CInfoBox() 
{
	if (_theInfoBox == NULL) 
		_theInfoBox = this;

	m_ElspTime.update();
	showtime_ = 0;
}

CInfoBox::~CInfoBox()
{
	if( CInfoBox::_theInfoBox ==this  ) 
		CInfoBox::_theInfoBox = NULL; 

	font_.DeleteObject();
}
 
// ��ʼ��
BOOL CInfoBox::OnInitDialog() 
{
	DialogType::OnInitDialog();
	//  LONG lfEscapement, ��3���������Ӱ���о�
	font_.CreateFont(22, 0, 5, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
		OUT_STROKE_PRECIS, CLIP_STROKE_PRECIS, DRAFT_QUALITY, FIXED_PITCH |
		FF_MODERN, _T("Arial"));

	// ���ݸ�����λ�ü������λ�� LONG lfEscapement;
 	_theHost=NULL;
	CRect rect;
	GetParent()->GetWindowRect(&rect);
// 	RECT rt = {0};
// 	::GetWindowRect( AfxGetMainWnd()->GetSafeHwnd(), &rt );
	int width0 = 300;
	int height0 = 240;
	int x0 = rect.Width()/2;
	int y0 = rect.Height()/2;
	SetWindowPos(NULL, x0, y0, width0, height0, SWP_NOZORDER);

	// ���ñ���ɫ
	// �ڲ���ϵͳ�Ĳ�ͬ��������£��Ի������ű䣬��������������һ���Է����ű�
	SetBackgroundColor(RGB(239, 239, 239));
	title_.Create(_T(""), WS_CHILD | WS_VISIBLE | SS_BITMAP | SS_NOTIFY,
		CRect(0, 0, width0, 30), this, IDC_INFOBOX_TITLE);
	title_.SetBkBmp(_T("res\\01.bmp"));
	title_.Draw(_T("INFOBOX"));
	title_.set_custom_font(TRUE);
	rect.SetRectEmpty(); // �����϶���Ϣ��ʾ�Ի���
	title_.SetArea(rect);

// 	font_.CreateFont(22, 0, 0, 0, FW_HEAVY, FALSE, FALSE, FALSE,
// 		DEFAULT_CHARSET, OUT_STROKE_PRECIS, CLIP_STROKE_PRECIS,
// 		DRAFT_QUALITY, FIXED_PITCH | FF_MODERN, _T("Arial"));
	// ������ť
	btn_OK.Create(_T("ȷ��"),WS_CHILD | WS_VISIBLE | BS_BOTTOM,
		CRect(100, 200, 180, 230), this, IDC_INFOBOX_BUTTON);
	btn_OK.SetFont(&font_);
	
	edit_info.Create(WS_CHILD|WS_VISIBLE|ES_READONLY|ES_MULTILINE,CRect(10,50,285,190),this,IDC_INFOBOX_EDIT);
	edit_info.SetFont(&font_);

	return TRUE;
}

void CInfoBox::BtnCancel()
{
	showtime_ = 0;
	_theHost=nullptr;
	this->ShowWindow(false);
}

void CInfoBox::BtnOK()
{
	this->ShowWindow(SW_HIDE);
	if( _theHost )
	{
		_theHost->ShowWindow(SW_SHOW);
	}	
 
	btn_OK.SetWindowTextW(_T("ȷ��"));

	showtime_ = 0;
	_theHost = NULL;
}

//������Ҫ���õĴ���ָ�룬��������ֵ����
void CInfoBox::ShowInfoBox(CWnd* hostPtr,CString infoTXT,CString caption, unsigned int showTime,unsigned int uTYPE)
{
	_theHost = hostPtr;
	if( caption != _T("") )
		caption_ = caption;
	else
		caption_ = _T("��ʾ");
	if ( uTYPE == JTMESSAGE )
	{
		title_.SetBkBmp(_T("res\\01.bmp"));
	}
	else
	{
		title_.SetBkBmp(_T("res\\02.bmp"));
 	}
	title_.Draw( caption_ );
	edit_info.SetWindowText( infoTXT );

	if( showTime > 0 ) 
	{
		unsigned int tsec = showTime*1000;
	 
		showtime_ = showTime;
		CString txt;
		txt.Format(_T("ȷ�� %d"),showtime_);
		btn_OK.SetWindowTextW(txt);
	}
	else
	{
		showtime_ = 0;
		btn_OK.SetWindowTextW(_T("ȷ��"));
	}
	ShowWindow(SW_SHOW);

	m_ElspTime.update();

	BringWindowToTop();
}

void CInfoBox::update_ui_status()
{
	if ( IsWindowVisible() == FALSE || showtime_ <= 0 )
 		return;
	 
 	unsigned int sec_run = m_ElspTime.elapsed_sec();
	int span_sec = showtime_ - sec_run;
	if ( span_sec < 0 )
	{
		this->ShowWindow(SW_HIDE);
		if(_theHost)
		{
			_theHost->ShowWindow(SW_SHOW);
		}	
		_theHost = NULL;
		showtime_ = 0;
	}
	else
	{
		CString txt;
 		txt.Format(_T("ȷ�� %d"),span_sec);
		btn_OK.SetWindowTextW( txt );	
	}
}

 