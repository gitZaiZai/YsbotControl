// DlgDummy.cpp : implementation file
//

#include "stdafx.h"
#include "YsbotControl.h"
#include "DlgDummy.h"
#include "colourcode.h"
#include "stringconvert.h"

// CDlgDummy dialog

CDlgDummy::CDlgDummy()
{

}

CDlgDummy::~CDlgDummy()
{
}

void CDlgDummy::DoDataExchange(CDataExchange* pDX)
{
	;
}


BEGIN_MESSAGE_MAP(CDlgDummy, DialogType)
END_MESSAGE_MAP()


// CDlgDummy message handlers
BOOL CDlgDummy::OnInitDialog() 
{
	CDialogEx::OnInitDialog();
	SetWindowPos(NULL, g_x0 + 20, 95, 730, 439, SWP_NOZORDER);
	SetBackgroundColor(kColor[8]);
	// 	ModifyStyleEx(WS_EX_STATICEDGE, WS_EX_CLIENTEDGE, SWP_DRAWFRAME);

	CString caption;
    GetWindowText(caption);
	CRect rect;
	GetClientRect(&rect);
	// ±Í«©“ª
	title1_.Create(caption, WS_CHILD | WS_VISIBLE,
		CRect(10+rect.Width()/2, 10+rect.Height()/2, 100+rect.Width()/2, 430+rect.Height()/2), this);
	title1_.SetAlignment(1);
	title1_.Initialize(kColor[0]);

	return TRUE;
}

LRESULT CDlgDummy::OnMainButtonClicked( WPARAM wp, LPARAM lp)
{
	int btnid  = (int)(wp);
	int menuid = (int)(lp);

	CString textwin;
	title1_.GetWindowText(textwin);

	std::string winname = Unicode2Ansi( textwin.GetBuffer(0) );

	switch( btnid )
	{
	case 0: break;
	case 1: ; break;
	case 2: ; break;
	case 3: ; break;
	case 4: ; break;
 	default: break;
	}
	std::cout << winname << " button id = " << btnid << '\n';
	return 1;
}