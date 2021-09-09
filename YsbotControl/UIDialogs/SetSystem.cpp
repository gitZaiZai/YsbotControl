#include "StdAfx.h"
#include "SetSystem.h"
#include "../Angle.h"
#include "../colourcode.h"
#include "../stringconvert.h"
#include "SqlDatabase.h"
#include "state_dialog.h"

BEGIN_MESSAGE_MAP(CSetSystem, CGroupFrame)

END_MESSAGE_MAP()

CSetSystem::CSetSystem(void):m_strMachineID(_T(""))
{
	caption_default = _T("系统设置");
}


CSetSystem::~CSetSystem(void)
{
	font_.DeleteObject();
}
 

BOOL CSetSystem::Create(CString caption,DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
  	CStatic::Create( caption.IsEmpty() ? caption_default : caption, dwStyle|SS_NOTIFY, rect, pParentWnd, nID );
 
	font_.CreateFont(20, 0, 0, 0, 0, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
		OUT_STROKE_PRECIS, CLIP_STROKE_PRECIS, DRAFT_QUALITY, FIXED_PITCH |
		FF_MODERN, _T("Arial"));

	LOGFONT lf;
	font_.GetLogFont(&lf);

	const LPCTSTR strTitle[] = 
	{
		_T("语言"),
		_T("日期"),
		_T("时间"),
		_T("运行时间"),
 
	};
	CRect clientRect;
	GetClientRect(&clientRect);
	int widthBtn  = 150;
	int heightBtn = 50;
	int halfWidthClient = clientRect.Width()/2-5;

	int yPosSpace = 45;
	for ( int i = 0; i < ARRAY_SIZE(titleLabel); ++i )
	{
		titleLabel[i].Create(strTitle[i],WS_CHILD|WS_VISIBLE, CRect( CPoint(10, 35 + yPosSpace * i ) , CSize(100,30) ),this );
		titleLabel[i].SetFont(22,_T("宋体"));
 	}

	CRect rectPos( CPoint(halfWidthClient,30), CSize(halfWidthClient,170) );
	language_combbox.SetSort(false);
	language_combbox.SetShowImage(FALSE);
	language_combbox.Create( WS_CHILD | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED |CBS_HASSTRINGS,rectPos,this, IDC_SETTING_SYSTEM_COMBBOX );
	language_combbox.set_custom_message(FALSE);
	language_combbox.AddString(_T("中文"),0);
	language_combbox.EnableWindow(FALSE);
	language_combbox.SetCurSel(0);
 
	for ( int i = 0; i < ARRAY_SIZE(contentLabel); ++i )
	{
		contentLabel[i].Create( _T(""), WS_CHILD | WS_VISIBLE | DT_LEFT | SS_CENTERIMAGE, 
			CRect( CPoint(halfWidthClient, 35 + yPosSpace * (i+1)), CSize(halfWidthClient,30) ), this );
		contentLabel[i].SetFont(lf);
// 		contentLabel[i].SetBkColor(kColor[8]);
	}

	char szCurrentDate[32] = {'\0'};     
	char szCurrentTime[32] = {'\0'};     
	SYSTEMTIME systm;     
	GetLocalTime(&systm);     
	sprintf(szCurrentDate, " %.2d-%.2d-%.2d", systm.wYear, systm.wMonth, systm.wDay );   
	sprintf(szCurrentTime, " %.2d:%.2d:%.2d", systm.wHour, systm.wMinute, systm.wSecond );   

    contentLabel[0].SetText( Ansi2Unicode(szCurrentDate).c_str() );
	contentLabel[1].SetText( Ansi2Unicode(szCurrentTime).c_str() );
 
	timeBegin = CTime::GetCurrentTime();

	CalcMachineID(m_strMachineID);

	BOOL ret = CheckLegalityMachine(_T("6E30015020")); // save hardwareID data to SQL disk.
// 	if ( !ret )
// 	{
// 		exit(0);
// 	}
	return TRUE;
}

void CSetSystem::update_ui_status()
{
	if (GetParent()->IsWindowVisible() && this->IsWindowVisible())
	{
		CTimeSpan span;
		CTime timeNow = CTime::GetCurrentTime();

		span = timeNow - timeBegin;

		CString strDays;
		CString strTime;
		strDays.Format(_T(" %2d 天 "),span.GetDays() );
		strTime.Format(_T("%02d:%02d:%02d"),span.GetHours(),span.GetMinutes(),span.GetSeconds());
		strDays += strTime;
		contentLabel[2].SetText( strDays, FALSE );	
		//可采用总运行时间来限制
		static bool valid = false;
		if ( !valid && checktime.elapsed_sec() > 7*24*3600 ) // liurundan 持续运行一周，到期自动关闭
		{
			valid = true;
			BOOL res = CheckLegalityMachine(_T("6C30083630")); 
			if ( !res )	
			{
 				//exit(0);
			}
		}
	}
	

}


BOOL CSetSystem::CheckLegalityMachine(const CString& RUN_ID)
{
	BOOL OverPass_Flag = TRUE;    
	if( m_strMachineID.Compare( RUN_ID ) == 0 )   
		OverPass_Flag = TRUE;     
	else 
		OverPass_Flag = FALSE;    

	return OverPass_Flag;
}

void CSetSystem::CalcMachineID(CString& hardwareID)
{
	unsigned long s1,s2;     
	char sel;   
	sel='1';   
	CString MyCpuID,CPUID1,CPUID2;   
	__asm{   
		mov eax,01h   
			xor edx,edx   
			cpuid   
			mov s1,edx   
			mov s2,eax   
	}   
	CPUID1.Format(_T("%08X%08X"),s1,s2);   
	__asm{   
		mov eax,03h   
			xor ecx,ecx   
			xor edx,edx   
			cpuid   
			mov s1,edx   
			mov s2,ecx   
	}   
	CPUID2.Format(_T("%08X%08X"),s1,s2);   

	MyCpuID=CPUID1+CPUID2;

	DWORD ser;
	TCHAR namebuf[128];
	TCHAR filebuf[128];
	::GetVolumeInformation(_T("c:\\"),namebuf,128,&ser,0,0,filebuf,128);
	CString strdisk;
	strdisk.Format(_T("%d"),ser);
	CString strmachine;
	strmachine=MyCpuID.Mid(13,5);  
	strmachine+=strdisk.Mid(1,5); 

	hardwareID = strmachine;

	///// save calculate result to database /////////////////////////////////////////
 	SQL.set("MachineID", Unicode2Ansi(strmachine.GetBuffer(0)).c_str() );
 	//////////////////////////////////////////////////////////////////////////
}