#include "stdafx.h"
#include "TeachPanelDlg.h"
#include "../colourcode.h"
#include "../WorldModel.h"
#include "../MotionControl.h"
#include "../LuaScript/ScriptVariables.h"
#include "../stringconvert.h" 
#include "state_dialog.h"
#include "../InOutObject.h"

using namespace std;

 
 
BEGIN_MESSAGE_MAP(CTeachPanelDlg, CBkDialogST)
 
END_MESSAGE_MAP()

CTeachPanelDlg::CTeachPanelDlg( const ConfigReader& vr ) 
{
	m_bmpBackGroud = NULL;
	m_bmpCartTeachBk = NULL;
 	for (int i = 0; i < ARRAY_SIZE(m_iconDirection); ++i )
	{
		m_iconDirection[i] = NULL;
	}
}

CTeachPanelDlg:: ~CTeachPanelDlg()
{
	if ( m_bmpBackGroud )
	{
		::DeleteObject(m_bmpBackGroud);
	}
	if ( m_bmpCartTeachBk )
	{
		::DeleteObject(m_bmpCartTeachBk);
	}
	 

	for (int i = 0; i < ARRAY_SIZE(m_iconDirection); ++i )
	{
 		if ( m_iconDirection[i] )
		{
			::DestroyIcon(m_iconDirection[i]);
		}
	}
}
 
LRESULT CTeachPanelDlg::OnMainButtonClicked( WPARAM wp, LPARAM lp)
{
	int btnid  = (int)(wp);
	int menuid = (int)(lp);

	UserData currUserData;
 
	switch(btnid)
	{
	case 0: 	break;
	case 1:		break;
	case 2:		break;
	case 3:     break;
	case 4: 	break;
	case 5:		GetParent()->SendMessage(WM_ACTIVATE_WINDOW,winLastWindow,0); break;
	default:
		break;
	}
 
	return 1;
}

BOOL CTeachPanelDlg::OnInitDialog() 
{
	CBkDialogST::OnInitDialog();
 
	if ( GetParent()->GetDlgItem(IDC_BOX_MAIN)->GetSafeHwnd() )
	{
		GetParent()->GetDlgItem(IDC_BOX_MAIN)->GetClientRect(&clientRect);
		SetWindowPos(NULL,clientRect.left, clientRect.top, clientRect.Width(), clientRect.Height(), SWP_NOZORDER);
	}

	SetBackgroundColor(kColor[8], TRUE);
 
	SetMode(BKDLGST_MODE_TOPLEFT);
 
   
 	for ( int i =0; i < ARRAY_SIZE(m_ButtonJogLeft); ++i )
	{
		m_ButtonJogLeft[i].Create( _T(""), WS_VISIBLE| WS_CHILD | WS_TABSTOP, CRect(0,0,0,0),this, IDC_TEACH_JOG_LEFT1+i );
		m_ButtonJogLeft[i].SetDefaultColors();
 		m_ButtonJogLeft[i].DrawTransparent(TRUE);
		m_ButtonJogLeft[i].DrawBorder(FALSE);
		m_ButtonJogLeft[i].SetFlat(TRUE);
 		m_ButtonJogLeft[i].SetAlign(CButtonST::ST_ALIGN_OVERLAP,FALSE);

		m_ButtonJogRight[i].Create( _T(""), WS_VISIBLE| WS_CHILD | WS_TABSTOP, CRect(0,0,0,0),this, IDC_TEACH_JOG_RIGHT1+i );
		m_ButtonJogRight[i].SetDefaultColors();
 
		m_ButtonJogRight[i].DrawTransparent(TRUE);
		m_ButtonJogRight[i].DrawBorder(FALSE);
		m_ButtonJogRight[i].SetFlat(TRUE);
 		m_ButtonJogRight[i].SetAlign(CButtonST::ST_ALIGN_OVERLAP,FALSE);

	}

	return FALSE;
}

int CTeachPanelDlg::OnActivateChildWin( LPARAM lp )
{
	CString strBmpName = _T("res/robot_teach.bmp");
	
	for (int i = 0; i < ARRAY_SIZE(m_iconDirection); ++i )
	{
		if ( m_iconDirection[i] )
		{
			::DestroyIcon(m_iconDirection[i]);
		}
	}

	m_iconDirection[0] = (HICON)::LoadImageW(NULL, _T("res/jog_left.ico"), IMAGE_ICON, 48, 48, LR_LOADFROMFILE);
	m_iconDirection[1] = (HICON)::LoadImageW(NULL, _T("res/jog_right.ico"), IMAGE_ICON, 48, 48, LR_LOADFROMFILE);
	m_iconDirection[2] = (HICON)::LoadImageW(NULL, _T("res/jog_up.ico"), IMAGE_ICON, 48, 48, LR_LOADFROMFILE);
	m_iconDirection[3] = (HICON)::LoadImageW(NULL, _T("res/jog_down.ico"), IMAGE_ICON, 48, 48, LR_LOADFROMFILE);
	m_iconDirection[4] = (HICON)::LoadImageW(NULL, _T("res/jog_rot_left.ico"), IMAGE_ICON, 48, 48, LR_LOADFROMFILE);
	m_iconDirection[5] = (HICON)::LoadImageW(NULL, _T("res/jog_rot_right.ico"), IMAGE_ICON, 48, 48, LR_LOADFROMFILE);

	if ( MWM.get_robot_state().coordinate != jtJOINT )
	{
		strBmpName = _T("res/robot_teach_cart.bmp");
		if ( m_bmpCartTeachBk )
		{
			::DeleteObject(m_bmpCartTeachBk);
		}
		m_bmpCartTeachBk = (HBITMAP)::LoadImage(::AfxGetInstanceHandle(), strBmpName,	IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
 		SetBitmap(m_bmpCartTeachBk);

		teach_cart_coord();
    }
	else
	{
		if ( m_bmpBackGroud )
		{
			::DeleteObject(m_bmpBackGroud);
		}
		m_bmpBackGroud = (HBITMAP)::LoadImage(::AfxGetInstanceHandle(), strBmpName,	IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		SetBitmap(m_bmpBackGroud);
		teach_axis_coord();
 	}

	Invalidate();

	return 1;
}

void CTeachPanelDlg::OnPostEraseBkgnd(CDC* pDC)
{
	for ( int i = 0; i < ARRAY_SIZE(m_ButtonJogLeft); ++i )
	{
		m_ButtonJogLeft[i].SetBk(pDC);
	}
	for ( int i = 0; i < ARRAY_SIZE(m_ButtonJogRight); ++i )
	{
		m_ButtonJogRight[i].SetBk(pDC);
	}

}

BOOL CTeachPanelDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	int  axis = -1, dir;
	int  control = (int) LOWORD(wParam);
	int  message = (int) HIWORD(wParam);
	CmdInfo cmd;

	if( message == WM_LBUTTONDOWN || message == WM_LBUTTONUP )
	{
		if( control >= IDC_TEACH_JOG_LEFT1 && control <= IDC_TEACH_JOG_LEFT6 )
		{
			axis = control - IDC_TEACH_JOG_LEFT1;
			dir  = -1;
		}
		else if( control >= IDC_TEACH_JOG_RIGHT1 && control <= IDC_TEACH_JOG_RIGHT6 )
		{
			axis = control - IDC_TEACH_JOG_RIGHT1;
			dir  = 1;
		}


		if( axis >= 0 )
		{
			if( message == WM_LBUTTONDOWN )
			{
				// perform jog only when motion is stopped
				// 					if( MCC_GetMotionStatus(g_nGroup) == GMS_STOP )
				// 						Jog(m_ledJogMode.GetCheck(), dir, axis);



				if ( MWM.get_robot_state().mc_info.iMotionStatus == MotionCoreInfo::msSTOP )
				{
					int dirmark = 0;
					int axismark = 0;
					if ( dir == -1 )
					{
						SETBIT(dirmark, axis);
					}
					SETBIT(axismark,axis);

					CartRef cartref = MWM.get_jog_cart().refSys;
					CartRef tool    = MWM.get_jog_tool().refSys;
					cmd.iCommandType  = CMD_JOG;
					cmd.iJogCoordType = MWM.get_robot_state().coordinate; // jtJOINT; 
					cmd.chJogAxisMark = axismark;
					cmd.chJogDirMark  = dirmark;
					cmd.cartref = &cartref;
					cmd.tool    = &tool.cpos;
					// speed parameters
					cmd.dyn.custom_parameter = false;
					//  			cmd.dyn.JointVel  = MWM.get_external_speed();
 					if ( SMC.isCodeSysMotion )
					{
						; 
					}
					else
					{
						cmd.cartref = NULL;
						cmd.tool    = NULL;

						cmd.dyn.JointVel  = MWM.get_external_speed(); 
					}
					SMC.setCmdInfo( cmd );
				}
			}
			else // if( message == WM_LBUTTONUP )
			{
				if ( MWM.get_robot_state().mc_info.iMotionStatus != MotionCoreInfo::msSTOP )
				{
					if ( SMC.isCodeSysMotion )
					{
						SMC.setCmdInfo(CMD_ABORT); 

						SMC.setCmdInfo(CMD_ABORTJOG); 
					}
					else
						SMC.setCmdInfo(CMD_ABORT); 
				}
			}
		}  

		printf("\nJOG: axis = %d dir = %d\n",axis,dir);
	}


	return CDialog::OnCommand(wParam, lParam);
}

void CTeachPanelDlg::teach_axis_coord()
{
	int nWidthBmp  = 440;
	int nHeightBmp = 440;

	int iWidthBtn  = 70;
	int iHeightBtn = 50;
	CRect rectBtnLeft[6];
	rectBtnLeft[0] = CRect( CPoint(14,318), CSize( iWidthBtn, iHeightBtn ) );
	rectBtnLeft[1] = CRect( CPoint(14,196), CSize( iWidthBtn, iHeightBtn ) );
	rectBtnLeft[2] = CRect( CPoint(190,6 ), CSize( iWidthBtn, iHeightBtn ) );
	rectBtnLeft[3] = CRect( CPoint(287,29 ), CSize( iWidthBtn, iHeightBtn ) );
	rectBtnLeft[4] = CRect( CPoint(355,66), CSize( iWidthBtn, iHeightBtn ) );
	rectBtnLeft[5] = CRect( CPoint(425,126), CSize( iWidthBtn, iHeightBtn ) );
	CRect rectBtnRight[6];
	rectBtnRight[0] = CRect( CPoint(269,370), CSize( iWidthBtn, iHeightBtn ) );
	rectBtnRight[1] = CRect( CPoint(269,283 ), CSize( iWidthBtn, iHeightBtn ) );
	rectBtnRight[2] = CRect( CPoint(190,164), CSize( iWidthBtn, iHeightBtn ) );
	rectBtnRight[3] = CRect( CPoint(287,164), CSize( iWidthBtn, iHeightBtn ) );
	rectBtnRight[4] = CRect( CPoint(355,201), CSize( iWidthBtn, iHeightBtn ) );
	rectBtnRight[5] = CRect( CPoint(425,201), CSize( iWidthBtn, iHeightBtn ) );

	CString title;
	for ( int i =0; i < ARRAY_SIZE(m_ButtonJogLeft); ++i )
	{
		m_ButtonJogLeft[i].MoveWindow(&rectBtnLeft[i]);
		if ( i == 0 || i == 3 || i == 5 )
		{
			m_ButtonJogLeft[i].SetIcon(m_iconDirection[0],BTNST_AUTO_DARKER);
		}
		else
		{
			m_ButtonJogLeft[i].SetIcon(m_iconDirection[2],BTNST_AUTO_DARKER);
		}
 
		title.Format(_T("%d-"),i+1);
		m_ButtonJogLeft[i].SetWindowTextW(title);
		m_ButtonJogLeft[i].SetAlign(CButtonST::ST_ALIGN_OVERLAP,FALSE);

		m_ButtonJogRight[i].MoveWindow(&rectBtnRight[i]);
 
		if ( i == 0 || i == 3 || i == 5 )
		{
			m_ButtonJogRight[i].SetIcon(m_iconDirection[1],BTNST_AUTO_DARKER);
		}
		else
		{
			m_ButtonJogRight[i].SetIcon(m_iconDirection[3],BTNST_AUTO_DARKER);
		}
 
		title.Format(_T("%d+"),i+1);
		m_ButtonJogRight[i].SetWindowTextW(title);
		m_ButtonJogRight[i].SetAlign(CButtonST::ST_ALIGN_OVERLAP,FALSE);

	}
}

void CTeachPanelDlg::teach_cart_coord()
{
	int nWidthBmp  = 440;
	int nHeightBmp = 440;

	int iWidthBtn  = 75;
	int iHeightBtn = 50;
	LPCTSTR strLeft[] = { _T("后X-"), _T("右Y-"), _T("下Z-"), _T("A-"), _T("B-"), _T("C-")};
	LPCTSTR strRight[] = { _T("前X+"), _T("左Y+"), _T("上Z+"), _T("A+"), _T("B+"), _T("C+")};
	CRect rectBtnLeft[6];
	rectBtnLeft[0] = CRect( CPoint(12,265), CSize( iWidthBtn, iHeightBtn ) );
	rectBtnLeft[1] = CRect( CPoint(12,390), CSize( iWidthBtn, iHeightBtn ) );
	rectBtnLeft[2] = CRect( CPoint(148,390 ), CSize( iWidthBtn, iHeightBtn ) );
	rectBtnLeft[3] = CRect( CPoint(345,18 ), CSize( iWidthBtn, iHeightBtn ) );
	rectBtnLeft[4] = CRect( CPoint(345,118), CSize( iWidthBtn, iHeightBtn ) );
	rectBtnLeft[5] = CRect( CPoint(345,170), CSize( iWidthBtn, iHeightBtn ) );
	CRect rectBtnRight[6];
	rectBtnRight[0] = CRect( CPoint(317,390), CSize( iWidthBtn, iHeightBtn ) );
	rectBtnRight[1] = CRect( CPoint(317,265 ), CSize( iWidthBtn, iHeightBtn ) );
	rectBtnRight[2] = CRect( CPoint(148,193), CSize( iWidthBtn, iHeightBtn ) );
	rectBtnRight[3] = CRect( CPoint(424,18), CSize( iWidthBtn, iHeightBtn ) );
	rectBtnRight[4] = CRect( CPoint(424,118), CSize( iWidthBtn, iHeightBtn ) );
	rectBtnRight[5] = CRect( CPoint(424,170), CSize( iWidthBtn, iHeightBtn ) );

 	for ( int i =0; i < ARRAY_SIZE(m_ButtonJogLeft); ++i )
	{
		m_ButtonJogLeft[i].MoveWindow(&rectBtnLeft[i]);
		if ( i == 2 )
		{
			m_ButtonJogLeft[i].SetIcon(m_iconDirection[3],BTNST_AUTO_DARKER);
		}
		else if ( i == 3 || i == 4 || i == 5 )
		{
			m_ButtonJogLeft[i].SetIcon(m_iconDirection[4],BTNST_AUTO_DARKER);
 		}
		else
		{
			m_ButtonJogLeft[i].SetIcon(m_iconDirection[0],BTNST_AUTO_DARKER);
		}

 		m_ButtonJogLeft[i].SetWindowTextW(strLeft[i]);
		m_ButtonJogLeft[i].SetAlign(CButtonST::ST_ALIGN_HORIZ,FALSE);

		m_ButtonJogRight[i].MoveWindow(&rectBtnRight[i]);

		if ( i == 2 )
		{
			m_ButtonJogRight[i].SetIcon(m_iconDirection[2],BTNST_AUTO_DARKER);
		}
		else if ( i == 3 || i == 4 || i == 5 )
		{
			m_ButtonJogRight[i].SetIcon(m_iconDirection[5],BTNST_AUTO_DARKER);
		}
 		else
		{
			m_ButtonJogRight[i].SetIcon(m_iconDirection[1],BTNST_AUTO_DARKER);
		}

 		m_ButtonJogRight[i].SetWindowTextW(strRight[i]);
		m_ButtonJogRight[i].SetAlign(CButtonST::ST_ALIGN_HORIZ,FALSE);

 	}

}