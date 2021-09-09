#include "stdafx.h"
#include "../TeachDialog/CoSysInfo.h"
#include "../stringconvert.h"

//CCoSysInfo Frame
BEGIN_MESSAGE_MAP(CCoSysInfo, CGroupFrame)
	ON_WM_CREATE()
END_MESSAGE_MAP()

CCoSysInfo::CCoSysInfo() 
{
	widthIcon  = 16;
	heightIcon = 16;
}

CCoSysInfo::~CCoSysInfo() 
{

}

BOOL CCoSysInfo::Create(CString caption,DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	_nID=nID;
	CStatic::Create(caption, dwStyle|SS_NOTIFY, rect, pParentWnd, nID);
	Initialize(rect, pParentWnd, nID);

	label_[0].Create(_T("参考系"),WS_CHILD|WS_VISIBLE,CRect(30,38,100,68),this);
	label_[0].SetFont(23);
	label_[1].Create(_T("工具坐标"),WS_CHILD|WS_VISIBLE,CRect(30,80,130,110),this);
	label_[1].SetFont(23);

  
	COLORREF m_crBackgroud = RGB(216, 220, 216);
	COLORREF m_crText      = RGB(96, 100, 96);
// 	m_hIconSystemVar= AfxGetApp()->LoadIcon(IDI_VARIABLE_SYSTEM);
// 	m_hIconGlobalVar=AfxGetApp()->LoadIcon(IDI_VARIABLE_GLOBAL);
// 	m_hIconProgramVar = AfxGetApp()->LoadIcon(IDI_VARIABLE_PROJECT);
	int labelPosStartX = 180;
	int labelHeight = 32;
	CRect rcCartRef = CRect(labelPosStartX,33,340,33+labelHeight);
	CRect rcTool = rcCartRef;
	rcTool.OffsetRect(0,labelHeight+10);

	ref_cosys_label.Create( _T(""), WS_CHILD | WS_VISIBLE | DT_LEFT | SS_CENTERIMAGE,
		rcCartRef, this, _nID+1 );
	ref_cosys_label.SetBkColor(m_crBackgroud)
		.SetTextColor(m_crText)
        .SetBorder(TRUE)
		.SetFontBold(TRUE)
		.SetText(_T("Base"))
        .SetLabelIcon(IDI_VARIABLE_SYSTEM,widthIcon,heightIcon);

	tool_cosys_label.Create( _T(""), WS_CHILD | WS_VISIBLE | DT_LEFT | SS_CENTERIMAGE,
		rcTool, this, _nID+2 );
	tool_cosys_label.SetBkColor(m_crBackgroud)
		.SetTextColor(m_crText)
		.SetBorder(TRUE)
		.SetFontBold(TRUE)
		.SetText(_T("Tool_1"))
		.SetLabelIcon(IDI_VARIABLE_GLOBAL,16,16);
 
	return TRUE;
}

int CCoSysInfo::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	return 0;
}

void CCoSysInfo::set_cartref(ScriptVarSetT vt,CString crefStr)
{
	if ( ::IsWindow( ref_cosys_label.GetSafeHwnd() ) )
	{
		CString refName = crefStr;
		UINT nID = get_icon_id(vt);
		if ( vt == SVAR_SYSTEM || vt == SVAR_GLOBAL )
		{
			if ( refName.Find( CString(Ansi2Unicode(STR_LABEL_SYSTEM).c_str()) ) >= 0
				|| refName.Find( CString(Ansi2Unicode(STR_LABEL_GLOBAL).c_str()) ) >= 0 )
			{
				refName = crefStr.Right( crefStr.GetLength()-4 );
			}
 		}
		ref_cosys_label.SetText( refName );
		ref_cosys_label.SetLabelIcon(nID,widthIcon,heightIcon);	
	}
}

void CCoSysInfo::set_toolref(ScriptVarSetT vt,CString toolStr)
{
	if ( ::IsWindow( tool_cosys_label.GetSafeHwnd() ) )
	{
		CString refName = toolStr;
		 
		if ( vt == SVAR_SYSTEM || vt == SVAR_GLOBAL )
		{
			if ( refName.Find( CString(Ansi2Unicode(STR_LABEL_SYSTEM).c_str()) ) >= 0
				|| refName.Find( CString(Ansi2Unicode(STR_LABEL_GLOBAL).c_str()) ) >= 0 )
			{
 				refName = toolStr.Right( toolStr.GetLength()-4 );
			}
		}	
		tool_cosys_label.SetText(refName).SetLabelIcon(get_icon_id(vt),widthIcon,heightIcon);
	}
}

void CCoSysInfo::get_cartref(int* pIcon,CString* pCartRef)
{
	CString strref;
	int nIDIcon = ref_cosys_label.GetIconID();
	ref_cosys_label.GetWindowText(strref);
	*pCartRef = strref;
	*pIcon = nIDIcon;
}

void CCoSysInfo::get_toolref(int* pIcon,CString* pCartRef)
{
	CString strref;
	int nIDIcon = tool_cosys_label.GetIconID();
	tool_cosys_label.GetWindowText(strref);
	*pCartRef = strref;
	*pIcon = nIDIcon;
}

UINT CCoSysInfo::get_icon_id( ScriptVarSetT vt )
{
	UINT nIcon = NULL;

	switch(vt)
	{
	case SVAR_SYSTEM: nIcon = IDI_VARIABLE_SYSTEM; break;
	case SVAR_GLOBAL: nIcon = IDI_VARIABLE_GLOBAL; break;
	case SVAR_PROGRAM: nIcon = IDI_VARIABLE_PROJECT; break;
	default: break;
	}
	return nIcon;
}