//////////Ã∆è™è™è™è™

#include "stdafx.h"
#include "AreaInformation.h"
#include "../resource.h"

BEGIN_MESSAGE_MAP(CAreaInformation, CGroupFrame)
	ON_CBN_SELCHANGE(IDC_SecurArea_Information+1, &CAreaInformation::OnSelchangeArea)
 
END_MESSAGE_MAP()


CAreaInformation::CAreaInformation()
{
	nStateBmp = 0;
}
CAreaInformation::~CAreaInformation()
{

}

BOOL CAreaInformation::Create(CString caption,DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	CStatic::Create(caption, dwStyle|SS_NOTIFY, rect, pParentWnd, nID);
	_nID = nID;
	for (int i=0;i<3;i++)
	{
		AreaLabel[i].SetFont(22);
 	}
 	AreaLabel[0].Create(_T("«¯”Ú√˚≥∆"),WS_CHILD|WS_VISIBLE,CRect(10,35,100,70),this);

	// combbox list.
	Area_name.Create(WS_CHILD | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED |CBS_HASSTRINGS,CRect(100,28,300,260),this,_nID+1);
	Area_name.set_custom_message(FALSE);

	AreaLabel[1].Create(_T("∆Ù”√"),WS_CHILD|WS_VISIBLE,CRect(370,35,470,70),this);
	ActivePic.Create( _T(""), WS_CHILD | WS_VISIBLE | DT_LEFT | SS_CENTERIMAGE,CRect(430,30,462,62), this );
	//ActivePic.SetBkColor( RGB(0,0,0));
	ActivePic.SetLabelBitmaps(_T("res//disableUncheck.bmp"));
	ActivePic.set_stretch_drawbmp(true);

 	ActivePic.ShowWindow(SW_HIDE);

	AreaLabel[2].Create(_T("◊¥Ã¨"),WS_CHILD|WS_VISIBLE,CRect(550,35,650,70),this);
	
	stateBmpName[0] = _T("res//State.bmp");
	stateBmpName[1] = _T("res//StateIN.bmp");
	stateBmpName[2] = _T("res//StateOUT.bmp");
 	
	StatePic.Create( _T(""), WS_CHILD | WS_VISIBLE | DT_LEFT | SS_CENTERIMAGE,CRect(620,30,678,59), this ); // CRect(CPoint(620,30),CSize(58,29))
	//StatePic.SetBkColor(RGB(0,0,0));
	StatePic.SetLabelBitmaps(stateBmpName[nStateBmp]);
	StatePic.set_stretch_drawbmp(true);

	//m_AreaPic.Create( _T(""), WS_CHILD | WS_VISIBLE | DT_LEFT | SS_CENTERIMAGE,CRect( CPoint(5,22), CSize(220,143) ), this );
	//m_AreaPic.SetBkColor( RGB(0,0,0));

//	active_check.Create(_T(""),WS_CHILD|WS_VISIBLE|WS_GROUP ,CRect(CPoint(460,30),CSize(30,32)),this); 
// 	active_check.loadBmp(_T("res/unchecked.bmp"),_T("res/checked.bmp"),_T("res/disablecheck.bmp"),_T("res/disableUncheck.bmp"));

	active_check.Create(_T(""),WS_CHILD|WS_VISIBLE|BS_AUTOCHECKBOX,	CRect(CPoint(460,30),CSize(30,32)),this,IDC_AreaInfo_CHECK);
	active_check.Init(IDB_PNG_UNCHECK, IDB_PNG_CHECKED, IDB_PNG_DISABLECHECK, IDB_PNG_DISABLEUNCHECK ); 
	active_check.EnableWindow(FALSE);

	return TRUE;
}

void CAreaInformation::update_areainfo_show(const SecurArea& area)
{
	active_check.SetChecked(area.active);
	if ( area.active == false )
	{
		if ( nStateBmp != 0 )
		{
			nStateBmp = 0;
			StatePic.SetLabelBitmaps(stateBmpName[nStateBmp]);
 		}
 	}
	CRect (rc);
	GetWindowRect(rc);
	RedrawWindow();

	GetParent()->ScreenToClient(rc);
	GetParent()->InvalidateRect(rc,TRUE);
	GetParent()->UpdateWindow();
}

void CAreaInformation::update_area_state( bool b )
{
	if ( active_check.GetChecked() )
	{
		if ( b && nStateBmp != 1 )
		{
			nStateBmp = 1;
			StatePic.SetLabelBitmaps(stateBmpName[nStateBmp]);
		}
		else if ( !b && nStateBmp != 2 )
		{
			nStateBmp = 2;
			StatePic.SetLabelBitmaps(stateBmpName[nStateBmp]);
 		}
	}
}

void CAreaInformation::OnSelchangeArea()
{
	int index = Area_name.GetCurSel();
	//CString* str;
	//variable_Combobox.GetLBText(index,*str);
	GetParent()->SendMessage( WM_SelChangeAreaName, (WPARAM)index );
}

void CAreaInformation::get_area_information(SecurArea* pArea)
{
	pArea->active = active_check.GetChecked()? true : false;
}