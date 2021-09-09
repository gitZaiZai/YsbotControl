#include "stdafx.h"
#include "../colourcode.h"
#include "../Angle.h"
#include "../control_id.h"
#include "../Keyboard/digital_keyboard.h"
#include "../Keyboard/full_qwerty.h"
#include "../WorldModel.h"
#include "../resource.h"
#include "../program_dialog.h"
#include "../variable_manager.h"
#include "../location_manager.h"
#include "../IO Dialog/InOutDialog.h"
#include "../IO Dialog/SettingIODlg.h"
#include "../TeachDialog/TeachDlg.h"
#include "../PalletDLG.h"
#include "../InstructionBuilderDLG.h"
#include "../InstructionEditDLG.h"
#include "../VariableBuilderDLG.h"
#include "SecurityAreaDLG.h"
#include "UserCoSysDLG.h"
#include "ToolCoSysDLG.h"
#include "MainDlg.h"
 

CMainDlg::CMainDlg( const ConfigReader& vread ) :configuration_list(vread),m_bIsUbuntuWineRuntime(false)
{
	state_dialog_    = NULL;     
	right_dialog_    = NULL;     
	bottom_dialog_   = NULL;   
	main_popup_menu_ = NULL;
	m_hBmpScrollBar  = NULL;
	m_hBmpPlus       = NULL;				// bitmap 
	m_hBmpMinus      = NULL;	
	the_user_interface = NULL;
	m_pwndDlgSettingJoint = NULL;
	m_pwndDlgSettingUser  = NULL;
    m_pwndDlgIO = NULL;
	m_pwndDlgTeachPanel = NULL;

	m_exit_system  = false; // for test.
	restart_system = false;

	button_menu_info  = new MainButtonMenu();

	for (int i = 0; i < ARRAY_SIZE(m_PageManager); ++i )
	{
 		m_PageManager[i] = NULL;
 	}

	load_all_bitmap();
	home_path = vread.getenv();

	std::string strTmp = "";
	if ( vread.get("runtime_system", strTmp) > 0 )
	{
		if ( strTmp.compare("wine") == 0 )
 			m_bIsUbuntuWineRuntime = true;
 	}
	
}

void CMainDlg::destroy_child_window(CWnd* pWnd)
{
	if ( pWnd && ::IsWindow( pWnd->GetSafeHwnd() ) ) 
	{
		pWnd->DestroyWindow();
	}
	SAFE_DELETE(pWnd);

}

CMainDlg::~CMainDlg() 
{
	boxRect.clear();

	Singleton<DigitalKeyboard>::UnInstance();
	Singleton<FullQwerty>::UnInstance();
  
	delete button_menu_info;

 	for (int i = 0; i < ARRAY_SIZE(m_PageManager); ++i )
	{
		destroy_child_window(m_PageManager[i]);
	}

	destroy_child_window(state_dialog_);
	destroy_child_window(right_dialog_);
	destroy_child_window(bottom_dialog_);
	destroy_child_window(main_popup_menu_);
	destroy_child_window(the_user_interface);

	destroy_child_window(location_dialog);
	destroy_child_window(variable_dialog);
	destroy_child_window(program_dialog);
	destroy_child_window(m_pwndNewVariable);
	
	destroy_child_window(m_pwndDlgSetting);
	destroy_child_window(m_pwndDlgProject);
	// 	destroy_child_window(m_pwndDlgLog);
	destroy_child_window(Inout_Dialog);
 	destroy_child_window(m_pwndDlgIO);
	destroy_child_window(m_pwndDlgSettingJoint);
	destroy_child_window(m_pwndDlgSettingMechanism);
	destroy_child_window(m_pwndDlgSettingOrigin);

	destroy_child_window(teach_Dialog);
	destroy_child_window(userCoSys_DLG);
	destroy_child_window(toolCoSys_DLG);
	destroy_child_window(pallet_DLG);
	destroy_child_window(instruction_DLG);
	destroy_child_window(instructionEdit_DLG);
	destroy_child_window(variableBuilder_DLG);
// 	destroy_child_window(log_dialog);

	destroy_child_window(theInfoBox);
	destroy_child_window(SecurityArea_DLG);
	destroy_child_window(m_pwndDlgSettingUser);
	destroy_child_window(journal_dialog);

	destroy_child_window(main_menu_dlg);
	destroy_child_window(m_pwndDlgTeachPanel);


	if (m_hBmpPlus)
	{
		DeleteObject(m_hBmpPlus);
		m_hBmpPlus = NULL;
	}

	if (m_hBmpMinus)
	{
		DeleteObject(m_hBmpMinus);
		m_hBmpMinus = NULL;
	}

	if (m_hBmpScrollBar)
	{
		DeleteObject(m_hBmpScrollBar);
		m_hBmpScrollBar = NULL;
	}
}

BEGIN_MESSAGE_MAP(CMainDlg, DialogType)
	ON_MESSAGE(WM_MENU_CLICK,&CMainDlg::OnMainMenuClicked)
	ON_MESSAGE(WM_MESSAGE_CONFIRM,&CMainDlg::OnStateDlgConfirmMsg)
	ON_MESSAGE(WM_ACTIVATE_WINDOW,&CMainDlg::OnActiveWindow)
	ON_MESSAGE(WM_REBOOT_SYSTEM,&CMainDlg::OnRebootSystem)
	ON_MESSAGE(WM_SAVE_PROJECT,&CMainDlg::OnSaveProject )
	ON_MESSAGE(WM_2ND_CLICKED,&CMainDlg::On2NDToggle )
	ON_MESSAGE(WM_GET_HOMEPOS,&CMainDlg::OnGetHomePosition )
	ON_MESSAGE(WM_UPDATE_BUTTONTEXT, &CMainDlg::OnUpdateBottomButton )
	ON_MESSAGE(WM_LEFT_KEY_PRESS, &CMainDlg::OnLeftKeyPressFromPendant )
	ON_MESSAGE(WM_SET_PCKEY_DATA, &CMainDlg::OnSetPCKeyData )

END_MESSAGE_MAP()

void CMainDlg::DoDataExchange(CDataExchange* pDX) 
{

}
 
BOOL CMainDlg::OnInitDialog() 
{
	CDialogEx::OnInitDialog();

	CRect parentRect;
	GetParent()->GetClientRect(&parentRect);
	int iWinLeft = parentRect.left;
	int iWinTop  = parentRect.top;
	SetWindowPos( NULL, iWinLeft, iWinTop, parentRect.Width(), parentRect.Height(), SWP_SHOWWINDOW | SWP_NOZORDER );
	
	SetBackgroundColor( kColor[1], TRUE);

// 	CRect clientRect;
// 	GetClientRect(&clientRect);
	m_iTopBarOffset = 0;
	if ( m_bIsUbuntuWineRuntime )
 		m_iTopBarOffset = 24;
 
	int iStateBoxHeight  = 92;
	int iJogBoxWidth     = 50;
	int iBottonBoxHeight = 63;
	int iMainBoxHeight   = parentRect.Height()-(iStateBoxHeight+iBottonBoxHeight);
	int iLineHeight = (int)( iStateBoxHeight/3.0);
	boxRect.resize(ARRAY_SIZE(m_PageManager),CRect(0,0,0,0));
 	boxRect[BOX_STATE] = CRect(CPoint(iWinLeft,iWinTop),CSize(parentRect.Width(),iStateBoxHeight));

 	boxRect[BOX_MAIN]  = CRect(CPoint(iWinLeft,boxRect[BOX_STATE].bottom+m_iTopBarOffset),CSize(parentRect.Width()-iJogBoxWidth, iMainBoxHeight ) );
	boxRect[BOX_RIGHT] = CRect(CPoint(boxRect[BOX_MAIN].right,boxRect[BOX_STATE].bottom+m_iTopBarOffset), CSize( iJogBoxWidth, iMainBoxHeight ) );
	boxRect[BOX_BUTTONS] = CRect(CPoint(iWinLeft,boxRect[BOX_RIGHT].bottom), CSize( parentRect.Width(), iBottonBoxHeight ) );
 //	boxRect[BOX_MENUDLG]  = CRect(CPoint(boxRect[BOX_MAIN].left,iLineHeight*2), CSize( parentRect.Width(), parentRect.Height()-iLineHeight*2 ) );
//	boxRect[BOX_MENUDLG]  = CRect(CPoint(iWinLeft,iWinTop), CSize( parentRect.Width(), parentRect.Height() ) );
	boxRect[BOX_MENUDLG]  = CRect(CPoint(iWinLeft,boxRect[BOX_STATE].bottom+m_iTopBarOffset), CSize( parentRect.Width(), parentRect.Height()-boxRect[BOX_STATE].bottom ) );
	 
	for ( int i = 0; i < ARRAY_SIZE(m_PageManager); ++i )
	{
		m_PageManager[i] = new CPageManager();
		m_PageManager[i]->Create(_T("ddd"),WS_CHILD|WS_BORDER, boxRect[i], this, IDC_BOX_STATE+i );//WS_VISIBLE| 
	}
 
	creat_fixed_window();
	creat_child_window();

	ActiveDialog(winProgram);

	return FALSE;
}

void CMainDlg::creat_fixed_window()
{
	Singleton<DigitalKeyboard>::Instance()->CreateDialogFromTemplate(GetParent());
	Singleton<DigitalKeyboard>::Instance()->SetArea( CRect(22, 95, 740, 535));
	Singleton<DigitalKeyboard>::Instance()->ShowWindow(SW_HIDE);

	Singleton<FullQwerty>::Instance()->CreateDialogFromTemplate(this);
	Singleton<FullQwerty>::Instance()->SetArea( CRect(22, 95, 740, 535));
	Singleton<FullQwerty>::Instance()->ShowWindow(SW_HIDE);

	the_user_interface = new DialogUserInterface(configuration_list);
	the_user_interface->CreateDialogFromTemplate(this);
	the_user_interface->ShowWindow(SW_HIDE);

 	state_dialog_ = new StateDialog( GetSafeHwnd() );
  	state_dialog_->CreateModeDlg(_T(""), CRect(0, 0, 0, 0), FALSE, this);
 
	right_dialog_ = new RightDialog();
	right_dialog_->CreateModeDlg(_T(""), CRect(0, 0, 0, 0), FALSE, this);

	main_popup_menu_ = new PopupMenu();
	main_popup_menu_->CreateDialogFromTemplate(this, FALSE, WS_POPUP | SS_OWNERDRAW);

	bottom_dialog_ = new BottomDialog(main_popup_menu_);
	bottom_dialog_->CreateModeDlg(_T(""), CRect(0, 0, 0, 0), FALSE, this);


	if ( the_user_interface )
	{
		state_dialog_->set_pendant_type( the_user_interface->get_pendant_type() );
	}

	main_menu_dlg = new CMainMenuDlg();
	main_menu_dlg->CreateModeDlg(_T(""), CRect(0, 0, 0, 0), FALSE, this, WS_POPUP | WS_BORDER);

	m_PageManager[BOX_STATE]->InsertPage(state_dialog_);
	m_PageManager[BOX_STATE]->SelectPage(0);

	m_PageManager[BOX_RIGHT]->InsertPage(right_dialog_);
	m_PageManager[BOX_RIGHT]->SelectPage(0);

	m_PageManager[BOX_BUTTONS]->InsertPage(bottom_dialog_);
	m_PageManager[BOX_BUTTONS]->SelectPage(0);

	m_PageManager[BOX_MENUDLG]->InsertPage(main_menu_dlg);

	theInfoBox = new CInfoBox();
	theInfoBox->CreateDialogFromTemplate(AfxGetMainWnd());
	theInfoBox->ShowWindow(SW_HIDE);
}

void CMainDlg::creat_child_window()
{
	journal_dialog = new JournalDLG(configuration_list,m_hBmpScrollBar,m_hBmpPlus,m_hBmpMinus);
	journal_dialog->CreateModeDlg(_T(""), CRect(0, 0, 0, 0), FALSE,this);

	program_dialog = new ProgramDialog(home_path,m_hBmpScrollBar);
	program_dialog->CreateModeDlg( _T(""), CRect(0, 0, 0, 0), FALSE, this );

	m_pwndNewVariable = new DlgNewVariable(m_hBmpScrollBar);
	m_pwndNewVariable->CreateModeDlg(_T(""), CRect(0, 0, 0, 0), FALSE,this);

	variable_dialog = new VariableDialog(m_hBmpScrollBar,m_hBmpPlus,m_hBmpMinus,m_pwndNewVariable);
	variable_dialog->CreateModeDlg(_T(""), CRect(0, 0, 0, 0), FALSE,this);

	// project dialog must be created before location dialog, because load program file and variable data from disk.
	m_pwndDlgProject = new ProjectDialog(home_path.c_str());
	m_pwndDlgProject->CreateModeDlg(_T(""), CRect(0, 0, 0, 0), FALSE,this);

	location_dialog = new LocationDialog();
	location_dialog->CreateModeDlg(_T(""), CRect(0, 0, 0, 0), FALSE,this);
	location_dialog->set_curr_tool();
	if ( the_user_interface )
	{
		location_dialog->set_redraw_lable( the_user_interface->get_pendant_type() == DialogUserInterface::TCP_SOCKET );
	}

	Inout_Dialog = new InoutDialog(configuration_list,m_hBmpScrollBar,m_hBmpPlus,m_hBmpMinus);
	Inout_Dialog->CreateModeDlg(_T(""), CRect(0, 0, 0, 0), FALSE,this);
// 
	m_pwndDlgIO = new CSettingIODlg(configuration_list,m_hBmpScrollBar,m_hBmpPlus,m_hBmpMinus);
	m_pwndDlgIO->CreateModeDlg(_T(""), CRect(0, 0, 0, 0), FALSE,this);

	m_pwndDlgSettingMechanism = new Setting_MechanismDLG(configuration_list);
	m_pwndDlgSettingMechanism->CreateModeDlg(_T(""), CRect(0, 0, 0, 0), FALSE,this);
	// 	m_pwndDlgSettingJoint = new Setting_JointsDLG(configuration_list);
	// 	m_pwndDlgSettingJoint->CreateModeDlg(_T(""), CRect(0, 0, 0, 0), FALSE,this);

	m_pwndDlgSettingOrigin = new Setting_OriginDLG(configuration_list);
	m_pwndDlgSettingOrigin->CreateModeDlg(_T(""), CRect(0, 0, 0, 0), FALSE,this);

	m_pwndDlgSetting = new DlgParameterSetting(configuration_list,m_hBmpScrollBar,m_hBmpPlus,m_hBmpMinus);
	m_pwndDlgSetting->CreateModeDlg(_T(""), CRect(0, 0, 0, 0), FALSE,this);

 	teach_Dialog = new TeachDialog();
	teach_Dialog->CreateModeDlg(_T(""), CRect(0, 0, 0, 0), FALSE,this);

	userCoSys_DLG=new UserCoSysDLG(winUserCoSysDLG);
	userCoSys_DLG->CreateModeDlg(_T(""), CRect(0, 0, 0, 0), FALSE,this);

	toolCoSys_DLG=new ToolCoSysDLG(winToolCoSysDLG);
	toolCoSys_DLG->CreateModeDlg(_T(""), CRect(0, 0, 0, 0), FALSE,this);

	pallet_DLG=new PalletDLG();
	pallet_DLG->CreateModeDlg(_T(""), CRect(0, 0, 0, 0), FALSE,this);

	instruction_DLG=new InstructionBuilderDLG(m_hBmpScrollBar);
	instruction_DLG->CreateModeDlg(_T(""), CRect(0, 0, 0, 0), FALSE,this);

	instructionEdit_DLG=new InstructionEditDLG(m_hBmpScrollBar,m_hBmpPlus,m_hBmpMinus);
	instructionEdit_DLG->CreateModeDlg(_T(""), CRect(0, 0, 0, 0), FALSE,this);

	variableBuilder_DLG = new VariableBuilderDLG(m_hBmpScrollBar,m_hBmpPlus,m_hBmpMinus);
	variableBuilder_DLG->CreateModeDlg(_T(""), CRect(0, 0, 0, 0), FALSE,this);

	SecurityArea_DLG = new SecurityAreaDLG();
	SecurityArea_DLG->CreateModeDlg(_T(""), CRect(0, 0, 0, 0), FALSE,this);

	m_pwndDlgSettingUser = new CSettingUserDLG(configuration_list,m_hBmpScrollBar,m_hBmpPlus,m_hBmpMinus);
	m_pwndDlgSettingUser->CreateModeDlg(_T(""), CRect(0, 0, 0, 0), FALSE,this);

	m_pwndDlgTeachPanel = new CTeachPanelDlg(configuration_list);
	m_pwndDlgTeachPanel->CreateModeDlg(_T(""), CRect(0, 0, 0, 0), FALSE,this);


	m_PageManager[BOX_MAIN]->InsertPage( winUserDefine,     m_pwndDlgSettingUser    );
	m_PageManager[BOX_MAIN]->InsertPage( winSetting,        m_pwndDlgSetting );
	m_PageManager[BOX_MAIN]->InsertPage( winVariableData,   variable_dialog  );
	m_PageManager[BOX_MAIN]->InsertPage( winProject,        m_pwndDlgProject );
	m_PageManager[BOX_MAIN]->InsertPage( winProgram,        program_dialog   );
	m_PageManager[BOX_MAIN]->InsertPage( winPostionTeach,   location_dialog  );
 
	m_PageManager[BOX_MAIN]->InsertPage( winLogInformation,    journal_dialog     );
	m_PageManager[BOX_MAIN]->InsertPage( winNewScriptVariable, m_pwndNewVariable );
		m_PageManager[BOX_MAIN]->InsertPage( winInOutDeviece,      m_pwndDlgIO ); // failed, liurundan 
	//m_PageManager[BOX_MAIN]->InsertPage( winInOutDeviece,      Inout_Dialog );

 	m_PageManager[BOX_MAIN]->InsertPage( winSettingMechanism,  m_pwndDlgSettingMechanism );
	m_PageManager[BOX_MAIN]->InsertPage( winSettingOrigin,     m_pwndDlgSettingOrigin );

	m_PageManager[BOX_MAIN]->InsertPage( winTeachDLG,			 teach_Dialog );
	m_PageManager[BOX_MAIN]->InsertPage( winUserCoSysDLG,		 userCoSys_DLG );
	m_PageManager[BOX_MAIN]->InsertPage( winToolCoSysDLG,		 toolCoSys_DLG );
	m_PageManager[BOX_MAIN]->InsertPage( winPalletDLG,			 pallet_DLG );
	m_PageManager[BOX_MAIN]->InsertPage( winInstructionDLG,      instruction_DLG );
	m_PageManager[BOX_MAIN]->InsertPage( winInstructionEdit,     instructionEdit_DLG );
	m_PageManager[BOX_MAIN]->InsertPage( winVariableBuilder,     variableBuilder_DLG );
	m_PageManager[BOX_MAIN]->InsertPage( winEditCommand,         program_dialog->getEditWin() );
	m_PageManager[BOX_MAIN]->InsertPage( winSecurityArea,        SecurityArea_DLG );
	m_PageManager[BOX_MAIN]->InsertPage( winTeachPanelDlg,       m_pwndDlgTeachPanel );
	 
}

// main button home
LRESULT CMainDlg::OnActiveWindow( WPARAM wp, LPARAM lParam )
{
	if ( MWM.is_popup_window_active() )
		return 0;
 
	int winID = (int) (wp);
	if ( winID == winMainMenuDlg )
	{
		DialogType* pDlg = (DialogType*)m_PageManager[BOX_MENUDLG]->GetPage(0);
		if ( pDlg->IsWindowVisible() )
		{
 			m_PageManager[BOX_MENUDLG]->SelectPage(WINNONE);
// 			::SetWindowPos( main_menu_dlg->GetSafeHwnd(),HWND_TOPMOST,	// Z-order
// 				0, 0, 0, 0, // not used
// 				SWP_NOMOVE|SWP_NOSIZE|SWP_HIDEWINDOW  
// 				);
  		}
		else
		{
 			m_PageManager[BOX_MENUDLG]->SelectPage(0);
// 			::SetWindowPos( main_menu_dlg->GetSafeHwnd(),HWND_TOPMOST,	// Z-order
// 				   				0, 0, 0, 0, // not used
// 				   				SWP_NOMOVE|SWP_NOSIZE|SWP_SHOWWINDOW // show window |SWP_SHOWWINDOW
// 				   				);
 			pDlg->BringWindowToTop();
			pDlg->OnActivateChildWin(lParam);
			pDlg->SetFocus();
			CRect (rc);
			GetWindowRect(rc);
			RedrawWindow();

			GetParent()->ScreenToClient(rc);
			GetParent()->InvalidateRect(rc,TRUE);
			GetParent()->UpdateWindow();
   		}
    }
	else if ( winID == winLastWindow )
	{
		ActiveDialog( m_nLastSelPage, m_nLastLPARAM );
  	}
	else
		ActiveDialog( winID, lParam );
	

	return 1;
}

LRESULT CMainDlg::OnMainMenuClicked( WPARAM wParam, LPARAM lParam )
{
 	int mainID   = (int) (wParam);
	int secondID = (int) (lParam);
	m_PageManager[BOX_MENUDLG]->SelectPage(WINNONE);
	//OnActiveWindow(winMainMenuDlg,NULL);
	int nGradeUser = MWM.get_userdata().nGrade;

	switch ( mainID )
	{
	case 0:  
		{
			switch( secondID )
			{
			case 0: ActiveDialog(winUserDefine); break;
			case 1: 
				{
					if( nGradeUser > 1 ) 
						exit_system(); 
					else
						MSD.ShowInfo(  _T("权限不足,请登录更高权限再执行此操作"), _T("提示"),this, 5 );
				}
				break;
			default:break;
			}
		}
		break;
	case 1:
		{
			if( nGradeUser <= 1 )
			{
				MSD.ShowInfo(  _T("权限不足,请登录更高权限再执行此操作"), _T("提示"),this, 5 );
				break;
			}
			switch(secondID)
			{
			case 0:	ActiveDialog(winSettingMechanism); break;//机械参数
			case 1: ActiveDialog(winSetting);    break;
			case 2: ActiveDialog(winSettingOrigin); break;
			case 3: ActiveDialog(winInOutDeviece);	break;
			case 4: ActiveDialog(winSecurityArea);  break;
			case 5: break;
			case 6: break;
 			default: break;
			}
		}
		break;
	case 2: 
		{
			switch(secondID)
			{
			case 0: ActiveDialog(winVariableData);	break;
			case 1: ActiveDialog(winTeachDLG);		break;
			case 2: ActiveDialog(winUserCoSysDLG);	break;
			case 3: ActiveDialog(winToolCoSysDLG);  break;
			case 4: ActiveDialog(winPalletDLG);		break;
			case 5:  break;
			case 6: ;break;

			default: break;
			}
		}
		break;
	case 3: ActiveDialog(winProject);		break;
	case 4: ActiveDialog(winProgram);		break;
	case 5: ActiveDialog(winPostionTeach);	break;
	case 6: ActiveDialog(winLogInformation); break;
	case 7: 	break;
	default:break;
	}


	printf("\nmain menu = %d, second menu = %d \n",mainID,secondID);


	return 1;
}

void CMainDlg::ActiveDialog( int dialog_id, LPARAM lParam ) 
{
	if ( ChilidWindowKind(dialog_id) < winUserDefine || ChilidWindowKind(dialog_id) >= winNum )
		return;

	m_nLastSelPage = m_PageManager[BOX_MAIN]->GetCurSelPageIndex();
	m_nLastLPARAM  = m_PageManager[BOX_MAIN]->GetCurSelPageLParam();

	DialogType* pDlg = (DialogType*)m_PageManager[BOX_MAIN]->SelectPage( dialog_id, lParam );
	 
	if ( pDlg )
	{
		bottom_dialog_->SetCaptionScheme( dialog_id, pDlg->GetSafeHwnd() );
 		pDlg->OnActivateChildWin(lParam);
		pDlg->SetFocus();
	}
 
}

void CMainDlg::load_all_bitmap()  
{
	CBitmap bmp;
	//	bmp.LoadBitmap(IDB_SKIN_SCROLLBAR);
	BOOL bstate = bmp.LoadBitmap(IDB_SKIN_SCROLLBAR);
	if ( bstate )
	{
		BITMAP bm;
		bmp.GetBitmap(&bm);
		m_hBmpScrollBar=(HBITMAP)bmp.Detach();
	}
	else
	{
		;
	}
	bmp.DeleteObject();

	CBitmap bmpPlus;
	bstate = bmpPlus.LoadBitmap( IDB_PLUS );
	if ( bstate )
	{
		BITMAP bmPlus;
		bmpPlus.GetBitmap(&bmPlus);
		m_hBmpPlus=(HBITMAP)bmpPlus.Detach();
	}
	else
	{
		;
	}
	bmpPlus.DeleteObject();

	CBitmap bmpMinus;
	bstate = bmpMinus.LoadBitmap( IDB_MINUS );
	if ( bstate )
	{
		BITMAP bmMinus;
		bmpMinus.GetBitmap(&bmMinus);
		m_hBmpMinus = (HBITMAP)bmpMinus.Detach();
	}
	else
	{
		;
	}
	bmpMinus.DeleteObject();
}

void CMainDlg::exit_system()
{
	if ( program_dialog->editor_.is_running_flag )
	{
		program_dialog->on_free_script();
	}
	m_pwndDlgProject->save_project(NULL);
	MSD.ShowInfo(_T("退出系统"),JTWARNING);

	m_exit_system = true;

// 	::SendMessage(GetParent()->GetSafeHwnd(), WM_CLOSE, 0, 0);
// 	CDialogEx::OnClose();
}

bool CMainDlg::isExitSystem()
{
	return m_exit_system;
}

int CMainDlg::GetCurUserLevel()
{
	return MWM.get_userdata().nGrade;
}

LRESULT CMainDlg::OnUpdateBottomButton( WPARAM wParam, LPARAM lParam )
{
	int iwin = (int) wParam;
	CString* pStr = (CString*)lParam;
	for ( int i = 0; i < 6; ++i )
	{
		bottom_dialog_->update_button_text( iwin, i, pStr[i] );
	}
	return 1;
}

LRESULT CMainDlg::OnRebootSystem(WPARAM wp, LPARAM lp)
{
	restart_system = true;
	exit_system();
	return 1;
}

LRESULT CMainDlg::OnSaveProject(WPARAM wp, LPARAM lp)
{
	m_pwndDlgProject->save_project(NULL);
	return 1;
}

LRESULT CMainDlg::On2NDToggle(WPARAM,LPARAM)
{
	the_user_interface->toggle_second_page();
	return 1;
}

LRESULT CMainDlg::OnGetHomePosition( WPARAM wp, LPARAM lp )
{
	m_pwndDlgSettingOrigin->getOrgPos();
	return 1;
}

LRESULT CMainDlg::OnStateDlgConfirmMsg(WPARAM wParam, LPARAM lParam)
{
	std::cout << "信息确认22222222222" << std::endl;
	return 1;
}

LRESULT CMainDlg::OnLeftKeyPressFromPendant(WPARAM wParam, LPARAM lParam)
{
	unsigned int keyID = (unsigned int) ( wParam );
 
	DialogType* pDlg = (DialogType*)m_PageManager[BOX_MENUDLG]->GetPage(0);
	if ( pDlg == NULL )
 		return 0;

 	if ( pDlg->IsWindowVisible() == FALSE )
	{
		m_PageManager[BOX_MENUDLG]->SelectPage(0);
 		pDlg->BringWindowToTop();
		pDlg->OnActivateChildWin(lParam);
 	}
 	 
	main_menu_dlg->OnClickedFromPendant(keyID);
 	 

	return 1;
}

LRESULT CMainDlg::OnSetPCKeyData(WPARAM wParam, LPARAM lParam)
{
	the_user_interface->set_pc_keydata(wParam,lParam);
	return 1;
}

void CMainDlg::update_window()
{
	// 	if ( MWM.is_main_world_model_available() )
	// 	{
	// 		ActiveDialog( MWM.get_window_index() );
	// 	}

	if ( the_user_interface )
		the_user_interface->process_messages();

	location_dialog->update_location_window();
	program_dialog->update_program_window();
	right_dialog_->update_window();
	state_dialog_->update_state_bar();

// 	if ( Inout_Dialog->GetSafeHwnd() && Inout_Dialog->IsWindowVisible() )
// 	{
// 		Inout_Dialog->Updata_IOState();
// 	}

	if ( m_pwndDlgIO && m_pwndDlgIO->GetSafeHwnd() && m_pwndDlgIO->IsWindowVisible() )
	{
		m_pwndDlgIO->update_io_status();
	}
 
	bottom_dialog_->reset_button_cation(ProgramEditor::is_running_flag);

	SecurityArea_DLG->scan_security_area();
	m_pwndDlgSettingUser->update_userdlg_show();
	theInfoBox->update_ui_status();
}