#include "stdafx.h"
#include "YsbotControlDlg.h"
#include "left_dialog.h"
#include "LuaScript/ScriptVariables.h"
#include "resource.h"
#include "Keyboard/digital_keyboard.h"
#include "Keyboard/full_qwerty.h"
 
#include "UIDialogs/JournalDLG.h"

LeftDialog::LeftDialog( const ConfigReader& vread ) :configuration_list(vread)
{
	state_dialog_  = NULL;    // 状态栏窗口
	right_dialog_  = NULL;    // 右侧窗口
	bottom_dialog_ = NULL;  // 下方窗口
	m_exit_system  = false; // for test.
	restart_system = false;

	m_nLastSelPage = 0;
	m_nCurSelPage  = 0;
	m_nLastLPARAM  = 0;
	m_nCurLPARAM   = 0;
 	m_ppPages = new CWnd*[winNum];
	ClearAllPages();

	button_menu_info   = new MainButtonMenu();

	main_popup_menu_ = NULL;
	m_hBmpScrollBar = NULL;
	m_hBmpPlus      = NULL;				// bitmap 
	m_hBmpMinus     = NULL;	
	m_pwndDlgSettingJoint = NULL;
    m_pwndDlgSettingUser = NULL;
	m_pwndDlgLog = NULL;
	the_user_interface = NULL;

	load_all_bitmap();

	home_path = vread.getenv();
}

LeftDialog::~LeftDialog() 
{
 	delete button_menu_info;
	
	Singleton<DigitalKeyboard>::UnInstance();
	Singleton<FullQwerty>::UnInstance();

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
	destroy_child_window(log_dialog);

	destroy_child_window(theInfoBox);
    destroy_child_window(SecurityArea_DLG);
	destroy_child_window(m_pwndDlgSettingUser);
	destroy_child_window(journal_dialog);
 
	delete [] m_ppPages;

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

BEGIN_MESSAGE_MAP(LeftDialog, DialogType)
	ON_COMMAND_RANGE( LEFT_DIALOG_BUTTON1, LEFT_DIALOG_BUTTON7, Clicked )
	ON_MESSAGE(WM_MESSAGE_CONFIRM,&LeftDialog::OnStateDlgConfirmMsg)
 	ON_MESSAGE(WM_ACTIVATE_WINDOW,&LeftDialog::OnActiveWindow)
	ON_MESSAGE(WM_REBOOT_SYSTEM,&LeftDialog::OnRebootSystem)
	ON_MESSAGE(WM_SAVE_PROJECT,&LeftDialog::OnSaveProject )
	ON_MESSAGE(WM_2ND_CLICKED,&LeftDialog::On2NDToggle )
	ON_MESSAGE(WM_GET_HOMEPOS,&LeftDialog::OnGetHomePosition )
	ON_MESSAGE(WM_UPDATE_BUTTONTEXT, &LeftDialog::OnUpdateBottomButton )
	ON_WM_SIZE()
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()

void LeftDialog::DoDataExchange(CDataExchange* pDX) {

}

void LeftDialog::destroy_child_window(CWnd* pWnd)
{
 	if (pWnd->GetSafeHwnd()) 
	{
 		pWnd->DestroyWindow();
 	}

	if ( pWnd ) 
	{
		delete pWnd;
		pWnd = NULL;
	}
}

BOOL LeftDialog::OnInitDialog() 
{
	CDialogEx::OnInitDialog();
 // 	SetWindowPos( NULL, g_x0, 92, 20, 474, SWP_NOZORDER );
	::SetWindowPos(GetSafeHwnd(), HWND_TOP, g_x0, 61, 20, 475, 0);
	SetBackgroundColor(kColor[8], TRUE);
 
	main_popup_menu_ = new PopupMenu();
	main_popup_menu_->CreateDialogFromTemplate(this, FALSE, WS_POPUP | SS_OWNERDRAW);
 
	creat_fixed_window();
	creat_child_window();
	m_pwndZOrderAfter = this;

	ActiveDialog(winProgram);

	if(MSD.is_main_state_dialog_available())
		MSD.SetUserGrade(1);

	CRect rc;
	GetClientRect(&rc);
	const int totalHeight = rc.Height();
	const int cellHeight = (int)( (float)(rc.Height())/7.0+0.5);

	int space = 5;
	int height = cellHeight - space;
	int x0 = 2;
	int y0 = 2;
	int x1 = 17;
	int y1 = y0 + height;
	for (unsigned i = 0; i != 7; i++) 
	{
		left_button_[i].Create(_T(""), WS_CHILD | WS_VISIBLE,
			CRect(x0, y0, x1, y1), this, LEFT_DIALOG_BUTTON1 + i);
		left_button_[i].SetFaceColor(kColor[8]);
		y0 = y1 + space;
		y1 = y0 + height;
	}
 
	return FALSE;
}

void LeftDialog::Clicked(UINT id) 
{
 	if ( MWM.is_popup_window_active() )
 		return;
 
	unsigned int bnt_index = id - LEFT_DIALOG_BUTTON1;
	if ( bnt_index < 7 )
	{
		int menu_cnt = button_menu_info->GetMenuCount( bnt_index );
		if ( menu_cnt > 0 ) // 有子菜单
		{
			main_popup_menu_->active_menu( GetSafeHwnd(), MAIN_BUTTON_BASE_ID+bnt_index, menu_cnt, button_menu_info->get_menu_text(bnt_index) );
		}
		else // 无下一级菜单的左侧按钮
		{
			if ( main_popup_menu_->IsWindowVisible() )
			{
				main_popup_menu_->ShowWindow(SW_HIDE);
			}
			ActiveDialog(bnt_index);
		}
	}
 
}

LRESULT LeftDialog::OnStateDlgConfirmMsg(WPARAM wParam, LPARAM lParam)
{
	std::cout << "信息确认22222222222" << std::endl;
	return 1;
}

LRESULT LeftDialog::OnMainButtonClicked( WPARAM wp, LPARAM lp)
{
	int button_id = int(wp);
	int menu_id   = (int)lp;

	int button_offset = button_id - MAIN_BUTTON_BASE_ID;
	 
	bool bMainButton = (button_offset < 0) ? false : true;
    
	int nGradeUser = MWM.get_userdata().nGrade;

	if ( bMainButton )
	{
		switch(button_offset)
		{
		case 0: //user menu
			{
				switch(menu_id)
				{
				case 0: ActiveDialog(winUserDefine); break;
				case 1: //退出
					if( nGradeUser > 1 ) 
						exit_system(); 
					else
						MSD.ShowInfo(  _T("权限不足,请登录更高权限再执行此操作"), _T("提示"),this, 5 );
					break;
 				default: break;
				}
			}
			break;
		case 1: //setting menu
			{
				if( nGradeUser <= 1 )
				{
					MSD.ShowInfo(  _T("权限不足,请登录更高权限再执行此操作"), _T("提示"),this, 5 );
 					break;
				}
				switch(menu_id)
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

		case 2: //variable menu
			{
				switch(menu_id)
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



		default: break;
		}
 	}
	else
	{
		if ( button_id == 6 ) // return button.
		{
			//ActiveDialog( m_nLastSelPage );
			OnActiveWindow(winLastWindow,0);
		}
	}

	return 1;
}

void LeftDialog::exit_system()
{
	if ( program_dialog->editor_.is_running_flag )
	{
		program_dialog->on_free_script();
	}
	m_pwndDlgProject->save_project(NULL);
	MSD.ShowInfo(_T("退出系统"),JTWARNING);
	 
	m_exit_system = true;
}

LRESULT LeftDialog::OnRebootSystem(WPARAM wp, LPARAM lp)
{
	restart_system = true;
    exit_system();
	return 1;
}

LRESULT LeftDialog::OnSaveProject(WPARAM wp, LPARAM lp)
{
	m_pwndDlgProject->save_project(NULL);
	return 1;
}

LRESULT LeftDialog::On2NDToggle(WPARAM,LPARAM)
{
	the_user_interface->toggle_second_page();
	return 1;
}

LRESULT LeftDialog::OnGetHomePosition( WPARAM wp, LPARAM lp )
{
	m_pwndDlgSettingOrigin->getOrgPos();
	return 1;
}
 
void LeftDialog::update_window()
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

	if ( Inout_Dialog->GetSafeHwnd() && Inout_Dialog->IsWindowVisible() )
	{
  		Inout_Dialog->Updata_IOState();
	}

	 
    bottom_dialog_->reset_button_cation(ProgramEditor::is_running_flag);
	
	SecurityArea_DLG->scan_security_area();
	m_pwndDlgSettingUser->update_userdlg_show();
	theInfoBox->update_ui_status();
}

void LeftDialog::ActiveDialog( int dialog_id, LPARAM lParam ) 
{
	if ( ChilidWindowKind(dialog_id) < winUserDefine || ChilidWindowKind(dialog_id) >= winNum )
		return;
 
	CWnd* pwnd = SelectPage( dialog_id, lParam );
	if ( pwnd )
	{
		bottom_dialog_->SetCaptionScheme( dialog_id, GetPage(dialog_id)->GetSafeHwnd() );
	}

	DialogType* pDlg = (DialogType*)GetPage(dialog_id);
	pDlg->OnActivateChildWin(lParam);

}
 
LRESULT LeftDialog::OnActiveWindow( WPARAM wp, LPARAM lParam )
{
	if ( MWM.is_popup_window_active() )
 		return 0;

   	int winID = (int) (wp);
	if ( winID == winLastWindow )
	{
  		ActiveDialog( m_nLastSelPage, m_nLastLPARAM );
	}
	else if ( winID == winMainMenu ) // 左侧按钮及外部示教盒按键
	{
		unsigned int buttonID = (unsigned int)(lParam); // index by 0,1,2....
		Clicked( buttonID+LEFT_DIALOG_BUTTON1 );
	}
   	else
	{
	    ActiveDialog( winID, lParam );
  	}

	return 1;
}

void LeftDialog::creat_child_window()
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
 
	Inout_Dialog = new InoutDialog(configuration_list,m_hBmpScrollBar,m_hBmpPlus,m_hBmpMinus);
// 	Inout_Dialog->CreateDialogFromTemplate(this,0,WS_POPUP|WS_CHILD|WS_VSCROLL);
	Inout_Dialog->CreateModeDlg(_T(""), CRect(0, 0, 0, 0), FALSE,this);

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

	//cts addtion
	teach_Dialog=new TeachDialog();
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


	InsertPage( winUserDefine,     m_pwndDlgSettingUser    );
	InsertPage( winSetting,        m_pwndDlgSetting );
	InsertPage( winVariableData,   variable_dialog  );
	InsertPage( winProject,        m_pwndDlgProject );
 	InsertPage( winProgram,        program_dialog   );
	InsertPage( winPostionTeach,   location_dialog  );
//	InsertPage( winLogInformation, m_pwndDlgLog     );
	InsertPage( winLogInformation, journal_dialog     );
    InsertPage( winNewScriptVariable, m_pwndNewVariable );
 //	InsertPage( winInOutDeviece,      m_pwndDlgIO ); // failed, liurundan 
	InsertPage( winInOutDeviece,      Inout_Dialog );

// 	InsertPage( winSettingJoints,     m_pwndDlgSettingJoint     );
	InsertPage( winSettingMechanism,  m_pwndDlgSettingMechanism );
	InsertPage( winSettingOrigin,     m_pwndDlgSettingOrigin );

	InsertPage( winTeachDLG,				teach_Dialog );
	InsertPage( winUserCoSysDLG,			userCoSys_DLG );
	InsertPage( winToolCoSysDLG,			toolCoSys_DLG );
	InsertPage( winPalletDLG,					pallet_DLG );
	InsertPage( winInstructionDLG,      instruction_DLG );
	InsertPage( winInstructionEdit,     instructionEdit_DLG );
	InsertPage( winVariableBuilder,     variableBuilder_DLG );
	InsertPage( winEditCommand,     program_dialog->getEditWin() );
	InsertPage( winSecurityArea,     SecurityArea_DLG );

}

// 激活窗口
void LeftDialog::creat_fixed_window()
{
 	Singleton<DigitalKeyboard>::Instance()->CreateDialogFromTemplate(GetParent());
	Singleton<DigitalKeyboard>::Instance()->SetArea( CRect(22, 95, 740, 535));
	Singleton<DigitalKeyboard>::Instance()->ShowWindow(SW_HIDE);

 	Singleton<FullQwerty>::Instance()->CreateDialogFromTemplate(GetParent());
	Singleton<FullQwerty>::Instance()->SetArea( CRect(22, 95, 740, 535));
	Singleton<FullQwerty>::Instance()->ShowWindow(SW_HIDE);

	journal_dialog = new JournalDLG(configuration_list,m_hBmpScrollBar,m_hBmpPlus,m_hBmpMinus);
	journal_dialog->CreateModeDlg(_T(""), CRect(0, 0, 0, 0), FALSE,this);

 	state_dialog_ = new StateDialog(GetSafeHwnd());
	state_dialog_->CreateModeDlg(_T(""), CRect(0, 0, 0, 0), FALSE, NULL);
	state_dialog_->ShowWindow(SW_SHOW);
 
	right_dialog_ = new RightDialog();
	right_dialog_->CreateModeDlg(_T(""), CRect(0, 0, 0, 0), FALSE, this);
  	right_dialog_->ShowWindow(SW_SHOW);
 
	bottom_dialog_ = new BottomDialog(main_popup_menu_);
	bottom_dialog_->CreateModeDlg(_T(""), CRect(0, 0, 0, 0), FALSE, this);
	bottom_dialog_->ShowWindow(SW_SHOW);

	the_user_interface = new DialogUserInterface(configuration_list);
 	the_user_interface->CreateDialogFromTemplate(this);
	the_user_interface->ShowWindow(SW_HIDE);

	if ( the_user_interface )
	{
		state_dialog_->set_pendant_type( the_user_interface->get_pendant_type() );
	}

	theInfoBox = new CInfoBox();
	theInfoBox->CreateDialogFromTemplate(AfxGetMainWnd());
	theInfoBox->ShowWindow(SW_HIDE);
 }

bool LeftDialog::isExitSystem()
{
 	return m_exit_system;
}

// page manage 
void LeftDialog::ClearAllPages()
{
	m_nCurSelPage = -1;
	m_uPageNum = 0;
	::ZeroMemory( m_ppPages, sizeof(CWnd*) * winNum );
}

void LeftDialog::SizePage(CWnd* pwndPage)
{
	if( GetSafeHwnd() && pwndPage->GetSafeHwnd() )
	{
//      move & re-size the window to exactly the same with the manager's
// 		CRect  rect;
// 		GetWindowRect(&rect);

		CRect  rect(20, 95, 750, 537);
		GetParent()->ScreenToClient(&rect);
		pwndPage->MoveWindow(&rect);

	}
}

UINT LeftDialog::InsertPage(int nIndex, CWnd* pwndPage, UINT nIDTemplate)
{
	ASSERT( GetParent() );

	if( pwndPage )
	{
		if( !pwndPage->GetSafeHwnd() )
		{
			if( !pwndPage->IsKindOf(RUNTIME_CLASS(CDialog)) ||
				!((CDialog*)pwndPage)->Create(nIDTemplate, GetParent()) )
				return -1;
		}

		// store in the list
		m_ppPages[nIndex] = pwndPage;
        m_uPageNum++;
		// in case the manager has been created
	 
		SizePage(pwndPage);	 
 	 
		// firstly hide window
		pwndPage->ShowWindow(SW_HIDE);
	}

	return m_uPageNum;
}

CWnd* LeftDialog::SelectPage( int nIndex, LPARAM lParam )
{
	CWnd*  pWnd = NULL;

	if( nIndex >= 0 && nIndex < winNum )
	{
		pWnd = m_ppPages[nIndex];
		if ( pWnd == NULL )
		{
			AfxMessageBox(_T("窗口待开发！"));
			return NULL;
		}
		if(nIndex != m_nCurSelPage)
		{
 			if ( nIndex == winEditCommand )
			{
  				if ( m_nCurSelPage >= 0 && m_nCurSelPage != winProgram )
				{
					ASSERT(m_ppPages[m_nCurSelPage]);
					m_ppPages[m_nCurSelPage]->ShowWindow(SW_HIDE);

					m_ppPages[winProgram]->SetWindowPos(
						NULL,	// Z-order
						0, 0, 0, 0, // not used
						SWP_NOMOVE|SWP_NOSIZE|SWP_SHOWWINDOW // show window
						);
				}

 				program_dialog->show_edit_win( TRUE, lParam ); // lp = big or small edit win.
			}
			else
			{
				if( m_nCurSelPage >= 0 && m_ppPages[m_nCurSelPage] )
				{
					if ( m_nCurSelPage == winEditCommand )
 						program_dialog->show_edit_win( FALSE );
 					else
						m_ppPages[m_nCurSelPage]->ShowWindow(SW_HIDE);
				}

				m_ppPages[nIndex]->SetWindowPos(
					NULL,	// Z-order
					0, 0, 0, 0, // not used
					SWP_NOMOVE|SWP_NOSIZE|SWP_SHOWWINDOW // show window
					);
			}

			m_ppPages[nIndex]->SetFocus();
		 
			m_nLastSelPage = m_nCurSelPage;
			m_nLastLPARAM  = m_nCurLPARAM;

			m_nCurSelPage  = nIndex;
			m_nCurLPARAM   = lParam;
		}
	}

	return pWnd;
}

CWnd* LeftDialog::GetPage(int nIndex) const
{
	return
		(nIndex >= 0 && nIndex < winNum) ?
		m_ppPages[nIndex]:
	NULL;
}

int	LeftDialog::GetPageIndex(const CWnd* pwndPage) const
{
	for(int i = 0; i < (int) GetPageCount(); ++i)
	{
		if( m_ppPages[i] == pwndPage )
		{
			return i;
		}
	}
 	return -1;
}

CWnd* LeftDialog::GetCurSelPage() const
{
	return (m_nCurSelPage >= 0) ? m_ppPages[m_nCurSelPage]: NULL;
}

int	LeftDialog::GetCurSelPageIndex(void) const
{
	return m_nCurSelPage;
}

UINT LeftDialog::GetPageCount() const
{
	return m_uPageNum;
}

void LeftDialog::EnableAllChildWindow(int nIndex, BOOL bEnable)
{
	CWnd	*pwndFirst, *pwndLast, *pwndChild;

	if(nIndex >= 0 && nIndex < (int) GetPageCount())
	{
		if( (pwndChild = m_ppPages[nIndex]->GetWindow(GW_CHILD)) != NULL )
		{
			pwndFirst = pwndChild->GetWindow(GW_HWNDFIRST);
			pwndLast  = pwndChild->GetWindow(GW_HWNDLAST);

			for(
				pwndChild = pwndFirst;
				pwndChild != pwndLast; 
			pwndChild = pwndChild->GetWindow(GW_HWNDNEXT)
				)
			{
				pwndChild->EnableWindow(bEnable);
			}

			pwndLast->EnableWindow(bEnable);
		}
	}
}
 
/////////////////////////////////////////////////////////////////////////////
// LeftDialog message handlers

void LeftDialog::OnSize(UINT nType, int cx, int cy)
{
	DialogType::OnSize(nType, cx, cy);

	// re-size all pages
	if( GetPageCount() > 0 )
	{
		for(int i = 0; i < winNum; ++i)
		{
 			SizePage(m_ppPages[i]);
		}
	}
}

void LeftDialog::OnSetFocus(CWnd* pOldWnd)
{
	DialogType::OnSetFocus(pOldWnd);

	// refer focus to currently selected page
	if( GetPageCount() > 0 )
		GetCurSelPage()->SetFocus();
}

void LeftDialog::load_all_bitmap() throw (InvalidConfigurationException)
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
		throw InvalidConfigurationException("variable_dialog_error_skin_bar:");
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
		throw InvalidConfigurationException("variable_dialog_error_read_bmp:");
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
		throw InvalidConfigurationException("variable_dialog_error_read_bmp:");
	}
	bmpMinus.DeleteObject();
}

int LeftDialog::GetCurUserLevel()
{
	return MWM.get_userdata().nGrade;
}
 
LRESULT LeftDialog::OnUpdateBottomButton( WPARAM wParam, LPARAM lParam )
{
	int iwin = (int) wParam;
	CString* pStr = (CString*)lParam;
	for ( int i = 0; i < 6; ++i )
	{
		bottom_dialog_->update_button_text( iwin, i, pStr[i] );
 	}
	return 1;
}