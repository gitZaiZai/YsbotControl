#include "stdafx.h"
#include "MainMenuDlg.h"
#include "../colourcode.h"
#include "../Angle.h"
#include "../control_id.h"
#include "../resource.h"
#include "../WorldModel.h"
#include "SystemInformation.h"

CMainMenuDlg::CMainMenuDlg( )
{
	m_nRowMain = 4;
	m_nColMain = 4;

	m_nRowSecondMenu = 3;
	m_nColSecondMenu = 4;

    main_button_list.resize(NUM_BUTTON,NULL);
	main_rect_list.resize(m_nRowMain*m_nColMain,NULL);

	second_rect_list.resize(m_nRowSecondMenu*m_nColSecondMenu,NULL);
	second_button_list.resize(m_nRowSecondMenu*m_nColSecondMenu,NULL);
	for ( int i = 0; i < NUM_BUTTON; ++i )
	{
 		m_hIcon[i] = NULL;
	}
	m_hIconSecondBk = NULL;
	second_menu_dlg = NULL;
	second_page = NULL;

	button_menu_info   = new MainButtonMenu();

	m_nmainBtnIDActive = -1;
}

CMainMenuDlg::~CMainMenuDlg() 
{
	// main menu
	m_font.DeleteObject();

	for ( unsigned int i =0; i < main_button_list.size(); ++i )
	{
		SAFE_DELETE(main_button_list[i]);
	}
	main_button_list.clear();

	for ( unsigned int i = 0; i < main_rect_list.size(); ++i )
	{
 		SAFE_DELETE(main_rect_list[i]);
 	}
	main_rect_list.clear();

	// child menu
	for ( unsigned int i =0; i < second_button_list.size(); ++i )
	{
		SAFE_DELETE(second_button_list[i]);
	}
	second_button_list.clear();

	for ( unsigned int i = 0; i < second_rect_list.size(); ++i )
	{
		SAFE_DELETE(second_rect_list[i]);
	}
	second_rect_list.clear();

	for ( int i = 0; i < NUM_BUTTON; ++i )
	{
		if (m_hIcon[i])	::DestroyIcon(m_hIcon[i]);
	}
	if (m_hIconSecondBk) ::DestroyIcon(m_hIconSecondBk);

	SAFE_DELETE(button_menu_info);
// 	destroy_child_window(second_menu_dlg);
// 	destroy_child_window(second_page);
}

BEGIN_MESSAGE_MAP(CMainMenuDlg, DialogType)
	ON_COMMAND_RANGE( IDC_FIRSTMENU_BTN1, IDC_FIRSTMENU_BTN1+NUM_BUTTON, &CMainMenuDlg::OnMainButtonClicked )
	ON_COMMAND_RANGE( IDC_SECONDMENU_BTN1, IDC_SECONDMENU_BTN12, &CMainMenuDlg::OnSecondButtonClicked )

// 	ON_WM_PAINT()
 	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

void CMainMenuDlg::DoDataExchange(CDataExchange* pDX) 
{

}

BOOL CMainMenuDlg::OnInitDialog() 
{
  	CDialogEx::OnInitDialog();
 	
	if ( GetParent()->GetDlgItem(IDC_BOX_MENUDLG)->GetSafeHwnd() )
	{
		GetParent()->GetDlgItem(IDC_BOX_MENUDLG)->GetClientRect(&clientRect);
// 		SetWindowPos(&wndTopMost, clientRect.left, clientRect.right, clientRect.Width(), clientRect.Height(), SWP_NOMOVE );
		SetWindowPos(&wndTopMost, clientRect.left, clientRect.right, clientRect.Width(), clientRect.Height(), SWP_NOACTIVATE );
    } 

	m_font.CreateFont(16, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
		OUT_STROKE_PRECIS, CLIP_STROKE_PRECIS, DRAFT_QUALITY, FIXED_PITCH |
		FF_MODERN, _T("宋体"));

	creat_main_button();
	creat_second_button();

	maskColor = RGB(100,120,100);
 
 	BOOL bCanTransparentTheme = TRUE;

	TCHAR szThemeName[_MAX_PATH+10]={'\0'};
	CSystemInformation sysinfo;
	long ret = sysinfo.GetActiveWindowsTheme(szThemeName);
	if ( ret == ERROR_SUCCESS )
	{
		LPCTSTR szNoTransparentTheme[] = { _T("classic.theme"), _T("basic.theme") };

		int len = lstrlen(szThemeName);
		int iResCmp = 0;
		for (  int i = 0; i < ARRAY_SIZE(szNoTransparentTheme); ++i )
		{
			if( len >= lstrlen(szNoTransparentTheme[i]) )
			{
				iResCmp = lstrcmpi( &szThemeName[len-lstrlen(szNoTransparentTheme[i])], szNoTransparentTheme[i] );
				if( iResCmp == 0 )
				{
					bCanTransparentTheme = FALSE;
				}
			}
		}

	}

	if ( bCanTransparentTheme )
	{
		SetWindowLong( this->GetSafeHwnd(),GWL_EXSTYLE, GetWindowLong(this->GetSafeHwnd(),GWL_EXSTYLE) | WS_EX_LAYERED );
		SetLayeredWindowAttributes(maskColor,220,LWA_ALPHA);
 	}

	SetBackgroundColor( kColor[2], TRUE);
 


	return FALSE;
}

void CMainMenuDlg::creat_main_button()
{
	int iSpaceX = 30;
	int iSpaceY = 20;

	int xsize = (int)((clientRect.Width()-(m_nColMain+1)*iSpaceX)/m_nColMain);
	int ysize = (int)((clientRect.Height()-(m_nRowMain+1)*iSpaceY)/m_nRowMain);
    
	for ( int i = 0; i < m_nRowMain; ++i )
	{
		for ( int j =0; j < m_nColMain; ++j )
		{
			main_rect_list[i*m_nColMain+j] = new CRect(CPoint( iSpaceX + (xsize+iSpaceX)*j, iSpaceY + (ysize+iSpaceY)*i ),CSize(xsize,ysize));
		}
 	}
  
	CString strIconName = _T("");
	HINSTANCE hInstance = AfxGetInstanceHandle( );
	for ( int i = 0; i < NUM_BUTTON; ++i )
	{
		strIconName.Format(_T("./res/main_menu_%d.ico"),i+1);
		m_hIcon[i] = (HICON)LoadImage(hInstance, strIconName, IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
	}

    for (unsigned int i = 0; i < main_button_list.size(); ++i )
    {
 		main_button_list[i] = new CButtonST();
		main_button_list[i]->Create( _T(""), WS_VISIBLE| WS_CHILD | WS_TABSTOP, *main_rect_list[i],this, IDC_FIRSTMENU_BTN1+i );
		main_button_list[i]->SetDefaultColors();
		//main_button_list[i]->SetIcon(IDI_MAIN_HOME_BTN,(int)BTNST_AUTO_DARKER);//
		main_button_list[i]->DrawTransparent(TRUE);
		main_button_list[i]->DrawBorder(FALSE);
		main_button_list[i]->SetFlat(TRUE);
		main_button_list[i]->SetIcon( m_hIcon[i], BTNST_AUTO_DARKER );
		main_button_list[i]->DrawFlatFocus(TRUE);
    }

	main_button_list[NUM_BUTTON-1]->SetTooltipText(_T("关闭主菜单"));
}

void CMainMenuDlg::creat_second_button()
{
	int iSpaceY = 15;
	int iSpaceX = 1;
	int nMarginX = (int)(clientRect.Width()/8.0);

	int iValidWidth  = (int)( clientRect.Width() - nMarginX*2 );
	int iValidHeight = (int)(clientRect.Height() * 0.6 ); //main_rect_list[0]->Height() * 2 + 20 * 4; 

 	double yratio = 0.7;
	int xsize = (int)((iValidWidth-(m_nColSecondMenu-1)*iSpaceX)/m_nColSecondMenu);
	int ysize = (int)(yratio * (iValidHeight-(m_nRowSecondMenu-1)*iSpaceY)/m_nRowSecondMenu);

	nMarginX += 10;
	for ( int i = 0; i < m_nRowSecondMenu; ++i )
	{
		for ( int j =0; j < m_nColSecondMenu; ++j )
		{
			second_rect_list[i*m_nColSecondMenu+j] = new CRect(CPoint( nMarginX + (xsize+iSpaceX)*j, (ysize+iSpaceY)*i ),CSize(xsize,ysize));
 		}
	}

	CString strIconName = _T("./res/second_menu_bk.ico");
	HINSTANCE hInstance = AfxGetInstanceHandle( );
	m_hIconSecondBk = (HICON)LoadImage(hInstance, strIconName, IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
 
	for (unsigned int i = 0; i < second_button_list.size(); ++i )
	{
		second_button_list[i] = new CButtonST();
		second_button_list[i]->Create( _T(""), WS_VISIBLE| WS_CHILD | WS_TABSTOP, *second_rect_list[i],this, IDC_SECONDMENU_BTN1+i );
		second_button_list[i]->SetDefaultColors();
		second_button_list[i]->SetIcon(m_hIconSecondBk, BTNST_AUTO_DARKER );//
		second_button_list[i]->DrawTransparent(TRUE);
		second_button_list[i]->DrawBorder(FALSE);
		second_button_list[i]->SetFont(&m_font);
		second_button_list[i]->ShowWindow(SW_HIDE);
		CString msg;
		msg.Format(_T("child second btn _%d"),i);
		second_button_list[i]->SetWindowText(msg);
		second_button_list[i]->SetAlign(CButtonST::ST_ALIGN_OVERLAP,FALSE);
	}
}

void CMainMenuDlg::OnClickedFromPendant(UINT id)
{
	int bnt_index = id;
	if ( bnt_index < 0 || bnt_index > NUM_BUTTON - 1 || bnt_index == m_nmainBtnIDActive )
		return;

	int nLastLineClicked = (m_nmainBtnIDActive == -1) ? -1 : (int)( m_nmainBtnIDActive / m_nColMain );

	m_nmainBtnIDActive = bnt_index;
	int menu_cnt = button_menu_info->GetMenuCount( bnt_index );
	if ( menu_cnt <= 0 )
	{
		if ( ::IsWindow(GetParent()->GetSafeHwnd()) )
		{
			GetParent()->SendMessage(WM_MENU_CLICK,bnt_index,-1);
			return;
		}
	}

	for ( int i = 0; i < menu_cnt; ++i )
	{
		second_button_list[i]->SetWindowTextW( button_menu_info->get_menu_text(bnt_index)[i] );
	}

	int nLine = (int)(bnt_index/m_nColMain);


	// calculate second menu position.
	CRect rcTmp;
	rcTmp.SetRectEmpty();
	int nYOffset = main_rect_list[bnt_index]->bottom + 30;

	for (unsigned int i = 0; i < second_button_list.size(); ++i )
	{
		rcTmp = *second_rect_list[i];
		rcTmp.OffsetRect(0,nYOffset);
		second_button_list[i]->MoveWindow(rcTmp);
		second_button_list[i]->ShowWindow( (int)i < menu_cnt ? SW_SHOW : SW_HIDE );
	}

	// reset main button position exclude don't move to new pos.
	nYOffset = second_rect_list[menu_cnt-1]->bottom + 40;
	int nIndexBtn = ( nLine + 1 ) * m_nColMain;
	for ( int i = m_nColMain; i < nIndexBtn; ++i ) 
	{
		main_button_list[i]->MoveWindow(*main_rect_list[i]) ;
	}

	// move main button for display second menu.
	for (unsigned int i = nIndexBtn; i < main_button_list.size(); ++i )
	{
		rcTmp = *main_rect_list[i];
		rcTmp.OffsetRect(0,nYOffset);
		main_button_list[i]->MoveWindow(rcTmp) ;
	}


	printf("\n main btn %d clicked ! \n",bnt_index);


	UpdateSurface();
}

void CMainMenuDlg::OnMainButtonClicked(UINT id) 
{
	if ( MWM.is_popup_window_active() )
		return;

	int bnt_index = id - IDC_FIRSTMENU_BTN1;
 
	OnClickedFromPendant( UINT(bnt_index) );
 	 
}

void CMainMenuDlg::UpdateSurface()
{
	CRect (rc);
	GetWindowRect(rc);
	RedrawWindow();

	GetParent()->ScreenToClient(rc);
	GetParent()->InvalidateRect(rc,TRUE);
	GetParent()->UpdateWindow();
}


void CMainMenuDlg::OnSecondButtonClicked(UINT id) 
{
	if ( MWM.is_popup_window_active() )
		return;

	int bnt_index = id - IDC_SECONDMENU_BTN1;
 
	if ( ::IsWindow(GetParent()->GetSafeHwnd()) )
	{
		GetParent()->SendMessage(WM_MENU_CLICK,m_nmainBtnIDActive,bnt_index);
 	}

	printf("\n second btn %d clicked ! \n",bnt_index);

}

void CMainMenuDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call DialogType::OnPaint() for painting messages
//     CDC *pDC = GetDC();
// 	for ( unsigned int i =0; i < main_button_list.size(); ++i )
// 	{
// 		main_button_list[i]->SetBk(pDC);
// 	}
// 	ReleaseDC(pDC);	

	CRect rect;
	GetClientRect(rect);
	dc.FillSolidRect(rect,maskColor);

}


BOOL CMainMenuDlg::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
    BOOL ret = DialogType::OnEraseBkgnd(pDC);
	CRect rect;
	GetClientRect(&rect);
	pDC->FillSolidRect(&rect, maskColor);

	for ( unsigned int i =0; i < main_button_list.size(); ++i )
	{
		 main_button_list[i]->SetBk(pDC);
	}

 	return ret;
}

int CMainMenuDlg::OnActivateChildWin(LPARAM lp)
{
	m_nmainBtnIDActive = -1;
	for (unsigned int i = 0; i < second_button_list.size(); ++i )
	{
		second_button_list[i]->ShowWindow(SW_HIDE);
	}

	for (unsigned int i = m_nColMain; i < main_button_list.size(); ++i )
	{
		main_button_list[i]->MoveWindow(*main_rect_list[i]) ;
	}

	Invalidate();
 
	return 1;
}