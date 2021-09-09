#include "StdAfx.h"
#include "SetControl.h"
#include "../Angle.h"
#include "../stringconvert.h"

BEGIN_MESSAGE_MAP(CSetControl, CGroupFrame)

END_MESSAGE_MAP()

CSetControl::CSetControl(const ConfigReader& vread):ip_address_controler("")
{
	caption_default = _T("控制参数");
	vread.get("CodeSysMotion::motion_ip_address", ip_address_controler ); 
	vread.get("TeachPendant::key_server_address", ip_address_teachpendant ); 
}


CSetControl::~CSetControl(void)
{
	font_.DeleteObject();

}

BOOL CSetControl::Create(CString caption,DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	CStatic::Create( caption.IsEmpty() ? caption_default : caption, dwStyle|SS_NOTIFY, rect, pParentWnd, nID );
	m_nID = nID;

	font_.CreateFont(22, 0, 0, 0, 0, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
		OUT_STROKE_PRECIS, CLIP_STROKE_PRECIS, DRAFT_QUALITY, FIXED_PITCH |
		FF_MODERN, _T("Arial"));

	LOGFONT lf;
	font_.GetLogFont(&lf);

	const LPCTSTR strTitle[] = 
	{
		_T("示教器IP"),
		_T("控制器IP"),
		_T("定时器周期"),
		_T("扫描周期"),
		_T("ms"),
		_T("ms"),

	};
	CRect clientRect;
	GetClientRect(&clientRect);
	int widthBtn  = 150;
	int heightBtn = 50;
	int halfWidthClient = clientRect.Width()/2-5;

	int yPosSpace = 45;
	for ( int i = 0; i < ARRAY_SIZE(titleLabel); ++i )
	{
		titleLabel[i].Create(strTitle[i],WS_CHILD|WS_VISIBLE, 
			CRect( i > 3 ? CPoint(clientRect.right-50, 35 + yPosSpace * (i-2) ) : CPoint(10, 35 + yPosSpace * i ) , CSize(120,30) ),this );
		titleLabel[i].SetFont(22,_T("宋体"));
	}
 
	for ( int i=0; i<ARRAY_SIZE(editBox); i++ )
	{
		editBox[i].Create(WS_CHILD|WS_VISIBLE|WS_BORDER|ES_RIGHT,
			CRect( CPoint(halfWidthClient, 35 + yPosSpace * i ), CSize( i > 1 ? halfWidthClient/2 : halfWidthClient-5,30) ),this, IDC_SETTING_CONTROL_EDIT+i,TRUE );
		editBox[i].SetFont(&font_);
		editBox[i].EnableWindow(FALSE);
 	}
	 
	editBox[0].SetText(_T("192.168.11.21"));
	if ( ip_address_teachpendant.length() > 0 )
	{
		editBox[0].SetText( Ansi2Unicode(ip_address_teachpendant.c_str()).c_str() );
	}	
	editBox[1].SetText(_T("192.168.11.168"));
	if ( ip_address_controler.length() > 0 )
	{
		editBox[1].SetText( Ansi2Unicode(ip_address_controler.c_str()).c_str() );
	}
	editBox[2].SetText(_T("60"));
	editBox[3].SetText(_T("5"));


	return TRUE;
}