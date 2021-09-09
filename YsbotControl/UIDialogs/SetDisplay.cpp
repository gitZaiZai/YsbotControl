#include "StdAfx.h"
#include "SetDisplay.h"
#include "../Angle.h"
#include "../stringconvert.h"

BEGIN_MESSAGE_MAP(CSetDisplay, CGroupFrame)
	ON_CBN_SELCHANGE(IDC_SETTING_DISPLAY_COMBBOX+0,&CSetDisplay::OnCbnSelControlType)
	ON_CBN_SELCHANGE(IDC_SETTING_DISPLAY_COMBBOX+1,&CSetDisplay::OnCbnSelTeachType)
	ON_CBN_SELCHANGE(IDC_SETTING_DISPLAY_COMBBOX+2,&CSetDisplay::OnCbnSelComPort)
END_MESSAGE_MAP()

CSetDisplay::CSetDisplay(const ConfigReader& vread) : strControlType(""),cfg(const_cast<ConfigReader&>(vread))
{
	caption_default = _T("其他参数");
	vread.get("motion_control_type", strControlType ); 
	vread.get("TeachPendant::teach_pendat_type", strTeachPendantType );
	vread.get("TeachPendant::com_port", nPortTeachPendant );
}
 
CSetDisplay::~CSetDisplay(void)
{
	font_.DeleteObject();
}

BOOL CSetDisplay::Create(CString caption,DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	CStatic::Create( caption.IsEmpty() ? caption_default : caption, dwStyle|SS_NOTIFY, rect, pParentWnd, nID );

	m_nID = nID;

	font_.CreateFont(20, 0, 0, 0, 0, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
		OUT_STROKE_PRECIS, CLIP_STROKE_PRECIS, DRAFT_QUALITY, FIXED_PITCH |
		FF_MODERN, _T("Arial"));

	LOGFONT lf;
	font_.GetLogFont(&lf);

	const LPCTSTR strTitle[] = 
	{
		_T("控制器类型"),
		_T("示教器类型"),
		_T("串口号"),
		_T("波特率"),
 
	};
	CRect clientRect;
	GetClientRect(&clientRect);
	int widthBtn  = 150;
	int heightBtn = 50;
	int halfWidthClient = clientRect.Width()/2-5;

	int yPosSpace = 45;
	for ( int i = 0; i < ARRAY_SIZE(titleLabel); ++i )
	{
		titleLabel[i].Create(strTitle[i],WS_CHILD|WS_VISIBLE, CRect( CPoint(10, 35 + yPosSpace * i ) , CSize(120,30) ),this );
		titleLabel[i].SetFont(22,_T("宋体"));
	}

	//CRect rectPos( CPoint(halfWidthClient,30), CSize(halfWidthClient,170) );
	for ( int i = 0; i < ARRAY_SIZE(titleLabel); ++i )
	{
		m_combbox[i].SetSort(false);
		m_combbox[i].SetShowImage(FALSE);
		m_combbox[i].Create( WS_CHILD | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED |CBS_HASSTRINGS,
			CRect( CPoint(halfWidthClient, 35 + yPosSpace * i), CSize(halfWidthClient,170) ),this, IDC_SETTING_DISPLAY_COMBBOX+i );
		m_combbox[i].set_custom_message(FALSE);
		//m_combbox[i].EnableWindow(FALSE);
		m_combbox[i].SetCurSel(0);
	}

	m_combbox[0].AddString(_T("CodeSysPLC"),0);
	m_combbox[0].AddString(_T("YWT_PMC"),0);

	m_combbox[1].AddString(_T("pc_keyboard"),0);
	m_combbox[1].AddString(_T("com_serial"),0);
	m_combbox[1].AddString(_T("tcp_socket"),0);

	CString strPort;

	for ( int i = 0; i < 10; ++i )
	{
		strPort.Format(_T("%d"),i+1);
		m_combbox[2].AddString(strPort,i+1);
	}

	const LPCTSTR strBaud[] = 
	{
		_T("9600"),
		_T("19200"),
		_T("38400"),
		_T("57600"),
		_T("115200"),

	};
	for ( int i = 0; i < ARRAY_SIZE(strBaud); ++i )
	{
 		m_combbox[3].AddString(strBaud[i],0);
 	}
 
	for ( int i = 0; i < ARRAY_SIZE(m_combbox); ++i )
	{
 		m_combbox[i].SetCurSel(0);
	}

	CString textControl;
	for ( int i = 0; i < m_combbox[0].GetCount(); ++i )
	{
		m_combbox[0].GetLBText(i,textControl);
		if ( strstr( Unicode2Ansi(textControl.GetBuffer(0)).c_str(), strControlType.c_str() ) != NULL )
		{
			m_combbox[0].SetCurSel(i);
			break;
		}
	}

 	for ( int i = 0; i < m_combbox[1].GetCount(); ++i )
	{
		m_combbox[1].GetLBText(i,textControl);
		if ( strstr( Unicode2Ansi(textControl.GetBuffer(0)).c_str(), strTeachPendantType.c_str() ) != NULL )
		{
			m_combbox[1].SetCurSel(i);
			break;
		}
	}

	DWORD nport = 0;
	for ( int i = 0; i < m_combbox[2].GetCount(); ++i )
	{
		nport = m_combbox[2].GetItemData(i);
		if ( nport == nPortTeachPendant )
		{
			m_combbox[2].SetCurSel(i);
			break;
		}
	}

	m_combbox[2].EnableWindow( m_combbox[1].GetCurSel() == 1 );

	m_combbox[3].EnableWindow(FALSE);
	 

	return TRUE;
}

void CSetDisplay::OnCbnSelControlType()
{
	int index = m_combbox[0].GetCurSel();
	cout<<"index = "<<index<<endl;
	cfg.set( "motion_control_type", index == 0 ? "CodeSys" : "PMC" ); 
	cfg.write_all();
}

void CSetDisplay::OnCbnSelTeachType()
{
	int index = m_combbox[1].GetCurSel();

	m_combbox[2].EnableWindow( index == 1 );
 
	CString textControl;
 	m_combbox[1].GetLBText(index,textControl);

 	cfg.set( "TeachPendant::teach_pendat_type", Unicode2Ansi(textControl.GetBuffer(0)).c_str() ); 
	cfg.write_all();
}

void CSetDisplay::OnCbnSelComPort()
{
	int index = m_combbox[2].GetCurSel();
	DWORD nport = m_combbox[2].GetItemData(index);

 	cfg.set( "TeachPendant::com_port", nport ); 
	cfg.write_all();
}
