// Copyright 2016, 亚思信息科技有限责任公司
// All right reserved.
//
// Author: Liu Run Dan ( lrdcz@163.com )
//
// Yasi Robot Motion Control System - 亚思机器人运动控制系统

#include "stdafx.h"
#include "state_dialog.h"
#include "../WorldModel.h"
#include "../UserInterface/DialogUserInterface.h"
#include "../MotionControl.h"
#include "../stringconvert.h"
#include "InfoBox.h"
#include "../resource.h"
#include "JournalData.h"
#include "JournalDLG.h"


LabelInfo:: LabelInfo(int idLabel_, int idImage_, CRect* prect, CLabel* pLabel,bool hasImage_)
	: idLabel(idLabel_), idImage(idImage_), rect(prect), label(pLabel), hasImage(hasImage_)
{}

StateDialog*  StateDialog::main_state_dialog = NULL;

StateDialog&  StateDialog::get_main_state_dialog () 
{ 
	return * StateDialog::main_state_dialog; 
}

bool  StateDialog::is_main_state_dialog_available () throw()
{
	return main_state_dialog != NULL;
}

StateDialog::StateDialog(HWND hParent) 
{
	if( StateDialog::main_state_dialog == NULL ) 
		StateDialog::main_state_dialog = this; 

	m_hParent = hParent;
	m_Font.CreateFont(16, 8, 0, 0, FW_NORMAL,
		FALSE, FALSE, 0, 
		ANSI_CHARSET,
		OUT_DEFAULT_PRECIS, 
		CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, 
		DEFAULT_PITCH|FF_SWISS,
		_T("Arial")		);
	m_whitePen.CreatePen(PS_SOLID,2,RGB(255,255,255));

	m_crBackgroud = RGB(102, 102, 102);
	m_crText      = RGB(255, 255, 255);
	m_crDarkRed   = RGB(128, 0, 0);
	m_crWarning   = RGB(248,252,0);
	m_crError     = RGB(200, 48, 48);

	label_list.resize(LB_NUM,NULL);

	pendant_type = DialogUserInterface::TCP_SOCKET;

	strSpaceBmp[0] = _T("res//space_watch.bmp");
	strSpaceBmp[1] = _T("res//space_watch_alarm.bmp");

	m_journal_data.nCode = -1;
	m_journal_data.nState = JOURNAL_CONFIRMED; // default have checked.
	message_list.clear();

 
}

StateDialog::~StateDialog() 
{
	if( StateDialog::main_state_dialog == this )
		StateDialog::main_state_dialog = NULL;

	message_list.clear();
	m_Font.DeleteObject();
	m_whitePen.DeleteObject();

	for (unsigned int i=0; i < label_list.size(); i++) 
	{
		if ( label_list[i] )
		{
			if ( label_list[i]->rect )
			{
				delete label_list[i]->rect;
			}
			if ( label_list[i]->label )
			{
				delete label_list[i]->label;
			}
			delete label_list[i];		
		}
 	}
 
 	label_list.clear();
}

BEGIN_MESSAGE_MAP(StateDialog, DialogType)
	//ON_WM_SIZE()
	ON_BN_CLICKED(IDC_PROMPT_CONFIRM, PromptConFirm)
	ON_BN_CLICKED(IDC_MAIN_MENU_BTN, OnClickMainMenuBtn)
	ON_WM_LBUTTONDBLCLK()

// 	ON_WM_PAINT()
 	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

void StateDialog::DoDataExchange(CDataExchange* pDX)
{

}

void StateDialog::init_rect_list()
{
	//GetClientRect(&clientRect);

	if ( GetParent()->GetDlgItem(IDC_BOX_STATE)->GetSafeHwnd() )
	{
		GetParent()->GetDlgItem(IDC_BOX_STATE)->GetClientRect(&clientRect);
		SetWindowPos( NULL, clientRect.left, clientRect.top, clientRect.Width(), clientRect.Height(),  SWP_NOZORDER );
    } 
	SetBackgroundColor(kColor[2],TRUE);
 
	const int lineHeight = (int)(clientRect.Height()/3);
	const int labelSpace = -1;
	const int lineSpace  = -1;

	int iWidthMainMenu  = 80;
	int iHeightMainMenu = lineHeight*2;
	int iWidthConfirm   = 60;
	int iHeightConfirm  = lineHeight*2;

	// first line
	int iWidthValid = clientRect.Width() - (iWidthMainMenu+iWidthConfirm);
	int x1 = clientRect.left;
	int y1 = clientRect.top;
	int x2 = x1 + iWidthMainMenu;
	int y2 = clientRect.top + lineHeight;

	CRect rectMenuBtn = CRect(x1,y1,x2, y1 + iHeightMainMenu );
	rectMenuBtn.DeflateRect(2,2,2,2);
	main_menu_btn.Create( _T(""), WS_VISIBLE| WS_CHILD | WS_TABSTOP, rectMenuBtn,this,IDC_MAIN_MENU_BTN );
	main_menu_btn.SetDefaultColors();
	main_menu_btn.SetIcon(IDI_MAIN_HOME_BTN,(int)BTNST_AUTO_DARKER);//
	main_menu_btn.DrawTransparent(TRUE);
    main_menu_btn.DrawBorder(FALSE);
	main_menu_btn.SetFlat(TRUE);
 
	x1 = x2 + labelSpace;
	x2 = x1 + iWidthConfirm;
	CRect rectConfirmBtn = CRect(x1,y1,x2,y1+iHeightConfirm);
  	rectConfirmBtn.DeflateRect(2,2,2,2);

	prompt_confirm_.Create( _T(""),WS_VISIBLE | WS_CHILD | WS_TABSTOP,	rectConfirmBtn,	this, IDC_PROMPT_CONFIRM );
	prompt_confirm_.SetDefaultColors();
 	prompt_confirm_.SetIcon(IDI_PROMPT_CONFIRM,(int)BTNST_AUTO_DARKER);
	prompt_confirm_.DrawTransparent(TRUE);
	prompt_confirm_.DrawBorder(FALSE);
	prompt_confirm_.SetFlat(TRUE);

    x1 = clientRect.left + iWidthMainMenu + iWidthConfirm;
    x2 = x1 + (int)( iWidthValid/6.0 );
 
    label_list[LB_ROBOT_STATE] = new LabelInfo( LB_ROBOT_STATE,0, new CRect(x1,y1,x2,y2), new CLabel() ) ;

	x1 = x2 + labelSpace;
	x2 = x1 + (int)( iWidthValid/10.0 );
    label_list[LB_OPERATION_MODE] = new LabelInfo( LB_OPERATION_MODE,0, new CRect(x1,y1,x2,y2), new CLabel() ) ;
 
	x1 = x2 + labelSpace;
	x2 = x1 + (int)( iWidthValid/4.0 );
 	label_list[LB_COORDINATE_SYSTEM] = new LabelInfo( LB_COORDINATE_SYSTEM,0, new CRect(x1,y1,x2,y2), new CLabel() ) ;

	x1 = x2 + labelSpace;
	x2 = x1 + (int)( iWidthValid/4.0 ); // miner adjust for alignment second line
  	label_list[LB_TOOL_NAME] = new LabelInfo( LB_TOOL_NAME,0, new CRect(x1,y1,x2,y2), new CLabel() ) ;

	x1 = x2 + labelSpace;
	x2 = x1 + (int)( iWidthValid/10.0 );
	label_list[LB_ROBOT_SPEED] = new LabelInfo( LB_ROBOT_SPEED,0, new CRect(x1,y1,x2,y2), new CLabel(), false ) ;
 
	x1 = x2 + labelSpace;
	x2 = clientRect.right;
 	label_list[LB_COMPANY_LOGO] = new LabelInfo( LB_COMPANY_LOGO,0, new CRect(x1,y1,x2,y2+ lineHeight), new CLabel() ) ;


	// second line
	const int cellNum = 5;
	int cellWidth = (int)( iWidthValid/(4.0*5)+1 );
	x1 = clientRect.left+iWidthMainMenu+iWidthConfirm;
	y1 = y2 + lineSpace;
	y2 = y2 + lineHeight;
	x2 = label_list[LB_COORDINATE_SYSTEM]->rect->right;//x1 + (int)( iWidthValid/2.0 );
 	label_list[LB_PROJECT_NAME] = new LabelInfo( LB_PROJECT_NAME,0, new CRect(x1,y1,x2,y2), new CLabel() ) ;

	for ( int i = 0; i < cellNum; ++i )
	{
		x1 = x2 + labelSpace;
		if ( i == 4 )
            x2 = label_list[LB_TOOL_NAME]->rect->right;
 		else
			x2 = x1 + cellWidth;
 		label_list[LB_PROGRAM_STATE+i] = new LabelInfo( LB_PROGRAM_STATE+i,0, new CRect(x1,y1,x2,y2), new CLabel() ) ;
	}

	x1 = x2 + labelSpace;
	x2 = x1 + cellWidth;
	label_list[LB_SPACE_WATCH] = new LabelInfo( LB_SPACE_WATCH,0, new CRect(x1,y1,x2,y2), new CLabel() ) ;

	label_list[LB_SPACE_WATCH]->label->set_stretch_drawbmp(true);

 	x1 = x2 + labelSpace;
	x2 = label_list[LB_ROBOT_SPEED]->rect->right;
 	label_list[LB_SAFES_TATE] = new LabelInfo( LB_SAFES_TATE,0, new CRect(x1,y1,x2,y2), new CLabel() ) ;

	x1 = x2 + labelSpace;
	x2 = clientRect.right;
 	label_list[LB_USER_GRADE] = new LabelInfo( LB_USER_GRADE,0, new CRect(x1,y1,x2,y2), new CLabel(), false ) ;

// 	x1 = x2 + labelSpace;
// 	x2 = clientRect.right;
//     label_list[LB_SYSTEM_TIME] = new LabelInfo( LB_SYSTEM_TIME,0, new CRect(x1,y1,x2,y2), new CLabel(), false ) ;
 
	// third line
	int nButtonWidth = 80;
	x1 = clientRect.left;
	y1 = y2 + lineSpace;
	y2 = y2 + lineHeight + 2;
	x2 = clientRect.right;
 	label_list[LB_MESSAGE_BOX] = new LabelInfo( LB_MESSAGE_BOX,0, new CRect(x1,y1,x2,y2), new CLabel() ) ;
 
	for ( int i = 0; i < LB_NUM; i++ )
	{
//   		CRect rc = *label_list[i]->rect;
//  		printf("No.%d rect %0d,%0d - %0d,%0d\n",i, rc.left, rc.top, rc.right, rc.bottom);
		if ( label_list[i] )
		{
			label_list[i]->label->Create( _T(""), WS_CHILD | WS_VISIBLE | DT_LEFT | SS_CENTERIMAGE,
				*(label_list[i]->rect), this, IDS_ROBOT_STATE+i );
			if ( label_list[i]->hasImage == false )
			{
				label_list[i]->label->ModifyStyle(DT_LEFT,DT_CENTER,TRUE);
			}
			label_list[i]->label->SetBkColor(m_crBackgroud)
				.SetTextColor(m_crText)
				.SetText(_T(""))
				;
		}
 	
 	}
	label_list[LB_RESERVE_3]->label->ModifyStyle(0,SS_CENTER);
 

}

BOOL StateDialog::OnInitDialog() 
{
	CDialogEx::OnInitDialog();

    //SetWindowPos(NULL, m_RectPos.left, m_RectPos.top, m_RectPos.Width(), m_RectPos.Height(), SWP_NOZORDER);
	 
 	SetBackgroundColor(kColor[2], TRUE);

	ModifyStyle(WS_CAPTION | WS_BORDER,0,SWP_DRAWFRAME); 
 
	init_rect_list();

	//SetCompanyLogo(); 
 
	 
// 	x1 = x2;
// 	x2 = x1 + 70;
// 	y1 += 1;
// 
// 	create_all_label(&prompt_confirm_,_T(""), IDI_TICK, CRect(x1,y1,x2,y2),IDC_PROMPT_CONFIRM);
// 	prompt_confirm_.SetFlat(FALSE);
// 	prompt_confirm_.SetBtBackGroudColor(  RGB(216,220,216) );

	return FALSE;
}

void StateDialog::OnSize(UINT nType, int cx, int cy)
{
 	DialogType::OnSize(nType, cx, cy);
}
 
// 设置操作模式
void StateDialog::SetOperationMode(int mode) 
{
	int imageidx = IDI_AUTO_MODE;
	switch(mode)
	{
	case 0: imageidx = IDI_EXTERNAL_MODE; break;
	case 1: imageidx = IDI_AUTO_MODE;   break;
	case 2: imageidx = IDI_MANUAL_MODE; break;
	default: break;
	}
	if ( label_list[LB_OPERATION_MODE]->idImage != imageidx )
	{
		label_list[LB_OPERATION_MODE]->label->SetLabelIcon(imageidx,60,28);
		label_list[LB_OPERATION_MODE]->idImage = imageidx;
	}
	 
}

// 设置机器人状态
void StateDialog::SetRobotState(int state) 
{
	int imageidx = IDI_SERVO_OFF;
	switch(state)
	{
	case 0: imageidx = IDI_SERVO_OFF;    break;
	case 1: imageidx = IDI_SERVO_ON;   break;
  	default: break;
	}

	if ( MWM.get_robot_state().operate == omTEACH )
	{
		if ( state )
		{
			if ( MWM.get_robot_state().enable_press )
			{
				imageidx = IDI_SERVO_ON; 
			}
			else
			{
				imageidx = IDI_SERVO_READY; 
			}
 		}
	}

	if ( label_list[LB_ROBOT_STATE]->idImage != imageidx )
	{
		label_list[LB_ROBOT_STATE]->idImage = imageidx;
		label_list[LB_ROBOT_STATE]->label->SetLabelIcon(imageidx,28,28);
		label_list[LB_ROBOT_STATE]->label->SetText(_T("DR60X"));
	} 
}
 
// 设置坐标系名称
void StateDialog::SetCoordinateSystem(const char* coordname) 
{
	static CString lastCoord = _T("");
	CartRef procref = MWM.get_prog_cart().refSys;
	CartRef jogcref = MWM.get_jog_cart().refSys;

	const char* newCoord   = procref.selfname;
	const char* teachCoord = jogcref.selfname;
	
	if ( lastCoord != CString(newCoord) ) // TEXT
	{
		label_list[LB_COORDINATE_SYSTEM]->label->SetText(CString(newCoord));
		lastCoord = CString(newCoord);
	}

	if ( CString(newCoord) != CString(teachCoord) ) // Backgroud color
	{
		if (label_list[LB_COORDINATE_SYSTEM]->label->GetBkColor() != m_crDarkRed )
		{
		    label_list[LB_COORDINATE_SYSTEM]->label->SetBkColor(m_crDarkRed);
		}
	}
	else
	{
		if (label_list[LB_COORDINATE_SYSTEM]->label->GetBkColor() != m_crBackgroud )
		{
			label_list[LB_COORDINATE_SYSTEM]->label->SetBkColor(m_crBackgroud);
		}
 	}

 	if ( label_list[LB_COORDINATE_SYSTEM]->idImage != IDI_COORD )
	{
		label_list[LB_COORDINATE_SYSTEM]->label->SetLabelIcon(IDI_COORD,28,28);
		label_list[LB_COORDINATE_SYSTEM]->idImage = IDI_COORD;
	}  
}

// 设置工具手
void StateDialog::SetTool(const char* toolname) 
{
	static CString lastTool = _T("");
	CartRef procref = MWM.get_prog_tool().refSys;
	CartRef jogcref = MWM.get_jog_tool().refSys;

	const char* newTool   = procref.selfname;
	const char* teachTool = jogcref.selfname;

	if ( lastTool != CString(newTool) ) // TEXT
	{
		label_list[LB_TOOL_NAME]->label->SetText(CString(newTool));
		lastTool = CString(newTool);
	}

	if ( CString(newTool) != CString(teachTool) ) // Backgroud color
	{
		if (label_list[LB_TOOL_NAME]->label->GetBkColor() != m_crDarkRed )
		{
			label_list[LB_TOOL_NAME]->label->SetBkColor(m_crDarkRed);
		}
	}
	else
	{
		if (label_list[LB_TOOL_NAME]->label->GetBkColor() != m_crBackgroud )
		{
			label_list[LB_TOOL_NAME]->label->SetBkColor(m_crBackgroud);
		}
	}

 
	if ( label_list[LB_TOOL_NAME]->idImage != IDI_TOOL )
	{
		label_list[LB_TOOL_NAME]->label->SetLabelIcon(IDI_TOOL,28,28);
		label_list[LB_TOOL_NAME]->idImage = IDI_TOOL;
 	}   
}

// 设置机器人速度
void StateDialog::SetRobotSpeed(double speed) 
{
	CString str;
	int c1 = (int)speed;
	if (speed < 1) 
	{
		str.Format(_T(" %.1lf %%"), speed - c1);
	} 
	else 
	{
		str.Format(_T(" %d %%"), c1);
	}
   // label_list[LB_ROBOT_SPEED]->label->SetText(str,FALSE);
	label_list[LB_ROBOT_SPEED]->label->SetText(str, (pendant_type == DialogUserInterface::TCP_SOCKET) ? TRUE : FALSE );
 
	const COLORREF& cr = label_list[LB_ROBOT_SPEED]->label->GetBkColor();
 	const COLORREF darkYellor = RGB(160,160,22);
	const COLORREF darkRed    = RGB(200,0,0);

  	if ( speed <= 50 )
	{
	    if ( cr != m_crBackgroud )
 			label_list[LB_ROBOT_SPEED]->label->SetBkColor(m_crBackgroud);
 	}
	else if ( speed > 50 && speed <= 80)
	{
		if ( cr != darkYellor )
			label_list[LB_ROBOT_SPEED]->label->SetBkColor(darkYellor);
 	}
	else if ( speed > 80 )
	{
		if ( cr != darkRed )
			label_list[LB_ROBOT_SPEED]->label->SetBkColor(darkRed);
	}
	
}

void StateDialog::SetCompanyLogo()
{
    if ( label_list[LB_COMPANY_LOGO] )
    {
		HBITMAP hBmp = NULL;
		hBmp = (HBITMAP)::LoadImage(::AfxGetInstanceHandle(), _T("res/logo1.bmp"),
			IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		label_list[LB_COMPANY_LOGO]->label->SetBkColor(kColor[1]);
		label_list[LB_COMPANY_LOGO]->label->SetLabelBitmaps(hBmp,m_crBackgroud);
     }
	 
}

// 设置项目文件图标
void StateDialog::SetProjectName(LPCTSTR strName) 
{
 	if ( label_list[LB_PROJECT_NAME]->idImage != IDI_PROJECT )
	{
		label_list[LB_PROJECT_NAME]->label->SetLabelIcon(IDI_PROJECT,28,28);
		label_list[LB_PROJECT_NAME]->idImage = IDI_PROJECT;
	}   

	CString labeltext;
	label_list[LB_PROJECT_NAME]->label->GetWindowText(labeltext);

	CString newText = strName;
	if ( labeltext != newText )
	{
		label_list[LB_PROJECT_NAME]->label->SetText(newText);
	}
}
 
// 设置程序运行状态
void StateDialog::SetProgramState(int state) 
{
	int imageidx = IDI_PROGRAM_STOP;

	const RobotState& rs = MWM.get_robot_state();
	GroupState gs = rs.iGroupStatus;

	if ( gs == GrpMoving )
	{
		state = 1;
	}
	else if ( gs == GrpDelaying || gs == GrpWait )
	{
		state = 2;
	}
	else if ( gs == GrpHold )
	{
		state = 3;
	}
	else
		state = 0;

	switch( state )
	{
	case 0: imageidx = IDI_PROGRAM_STOP;    break;
	case 1: imageidx = IDI_PROGRAM_RUN;     break;
	case 2: imageidx = IDI_PROGRAM_WAIT;    break;
	case 3: imageidx = IDI_PROGRAM_PAUSE;   break;
	default: break;
	}

	if ( label_list[LB_PROGRAM_STATE]->idImage != imageidx )
	{
		label_list[LB_PROGRAM_STATE]->label->SetLabelIcon(imageidx,28,28);
		label_list[LB_PROGRAM_STATE]->idImage = imageidx;
 	} 

// 	cout << " iGroupStatus = " << rs.iGroupStatus << endl;

}

// 设置程序运行模式
void StateDialog::SetRunningMode(int state) 
{
	int imageidx = IDI_RUN_CONTINUE;
	switch( state )
	{
	case 0: imageidx = IDI_RUN_STEP;     break;
	case 1: imageidx = IDI_MOTION_STEP;  break;
	case 2: imageidx = IDI_RUN_CONTINUE; break;
 	default: break;
	}

	if ( label_list[LB_RUNNING_MODE]->idImage != imageidx )
	{
		label_list[LB_RUNNING_MODE]->label->SetLabelIcon(imageidx,28,28);
		label_list[LB_RUNNING_MODE]->idImage = imageidx;
	} 
}

// 设置空间监控 from 1
void StateDialog::SetSpaceWatch(int state) 
{
  	if ( label_list[LB_SPACE_WATCH]->idImage != state )
	{
		label_list[LB_SPACE_WATCH]->idImage = state;
 		label_list[LB_SPACE_WATCH]->label->SetLabelBitmaps(strSpaceBmp[state-1]);
	} 
} 

void StateDialog::SetSpaceActiveNum(unsigned int nNum)
{
	if ( label_list[LB_RESERVE_3]->idImage != nNum )
	{
		CString str;
		str.Format(_T("  %d"),nNum);
		label_list[LB_RESERVE_3]->idImage = nNum;
		label_list[LB_RESERVE_3]->label->SetText(str);
	} 
}

// 设置安全状态
void StateDialog::SetSafeState(int state) 
{
// 	safe_state_.SetBitmap(NULL);
	const RobotState& rs = MWM.get_robot_state();
	bool emgc = rs.in_emergency;
 
	int imageidx = emgc ? IDI_LOOP_DISCONNECT : IDI_LOOP_CONNECT;
 
	if ( label_list[LB_SAFES_TATE]->idImage != imageidx )
	{
		label_list[LB_SAFES_TATE]->label->SetLabelIcon(imageidx,28,28);
		label_list[LB_SAFES_TATE]->idImage = imageidx;
	}  
}

// 设置用户等级
void StateDialog::SetUserGrade(int grade) 
{
 	if ( label_list[LB_USER_GRADE]->idImage != grade )
	{
		CString str;
		str.Format(_T("%d"), grade);
		label_list[LB_USER_GRADE]->label->SetText(str);
 		label_list[LB_USER_GRADE]->idImage = grade;
	} 

  	const COLORREF darkGreen = RGB(88,148,24);
	const COLORREF& currcolor = label_list[LB_USER_GRADE]->label->GetBkColor();

	if ( grade < 16 )
	{
		if ( currcolor != m_crBackgroud )
			label_list[LB_USER_GRADE]->label->SetBkColor(m_crBackgroud);
	}
	else  
	{
		if ( currcolor != darkGreen )
			label_list[LB_USER_GRADE]->label->SetBkColor(darkGreen);
	}
}

// 更新时间
void StateDialog::SetSystemTime() 
{
	CTime time = CTime::GetCurrentTime();
	CString time_str;
	CString str;
	// 时间格式转换
	int h1 = time.GetHour();
	int m1 = time.GetMinute();
	int s1 = time.GetSecond();
	str.Format(_T("%d"), h1);
	if (h1 < 10) str = _T("0") + str;
	time_str = str + _T(":");
	str.Format(_T("%d"), m1);
	if (m1 < 10) str = _T("0") + str;
	time_str = time_str + str + _T(":");
	str.Format(_T("%d"), s1);
	if (s1 < 10) str = _T("0") + str;
	time_str = time_str + str;
 
	//label_list[LB_SYSTEM_TIME]->label->SetText(time_str,(pendant_type == DialogUserInterface::TCP_SOCKET) ? TRUE : FALSE);
}

// 设置信息提示状态
void StateDialog::SetMessagePrompt( CString str,int nType ) 
{
 	int imageidx = IDI_GRID_INFO;
	switch( nType )
	{
 	case 0: imageidx = IDI_GRID_INFO;     break;
	case 1: imageidx = IDI_GRID_WARNING;  break;
	case 2: imageidx = IDI_GRID_ERROR;    break;
	default: break;
	}

	if ( label_list[LB_MESSAGE_BOX]->idImage != imageidx )
	{
		label_list[LB_MESSAGE_BOX]->label->SetLabelIcon(imageidx,24,24);
		label_list[LB_MESSAGE_BOX]->idImage = imageidx;
	} 

	const COLORREF& currcolor = label_list[LB_MESSAGE_BOX]->label->GetBkColor();

	if ( imageidx == IDI_WARNING_PROMPT )
	{
		if ( currcolor != m_crWarning )
		{
			label_list[LB_MESSAGE_BOX]->label->SetBkColor(m_crWarning);
			label_list[LB_MESSAGE_BOX]->label->SetTextColor(RGB(0,0,0));
 		}
 	}
	else if ( imageidx == IDI_ERROR_PROMPT )
	{
		if ( currcolor != m_crError )
		{
			label_list[LB_MESSAGE_BOX]->label->SetBkColor(m_crError);
			label_list[LB_MESSAGE_BOX]->label->SetTextColor(m_crText);
 		}
 	}
	else
	{
		if ( currcolor != m_crBackgroud )
		{
			label_list[LB_MESSAGE_BOX]->label->SetBkColor(m_crBackgroud);
			label_list[LB_MESSAGE_BOX]->label->SetTextColor(m_crText);
		}
 	}

	label_list[LB_MESSAGE_BOX]->label->SetText(str,(pendant_type == DialogUserInterface::TCP_SOCKET) ? TRUE : FALSE);
 
 
}

void StateDialog::insert_journal( LPCTSTR msgText )
{
	CString journalText = msgText;
	// insert to grid and save to database. 
	if ( journalText.IsEmpty() == FALSE )
	{
 		m_journal_data.timeRecord = sql::time::now();
		m_journal_data.set_user( MWM.get_userdata().strUserName );
		m_journal_data.set_description( UnicodeToUtf8(journalText).c_str() ); // for show data in SQLlite manage for firefox normally.
		journal_dialog->insert_journal_information( m_journal_data ); 
	}
}

// 显示信息内容
void StateDialog::ShowInfo( CString str, int nType , int nMsgCode, bool bNeedConfirm )
{
	if ( bNeedConfirm ) // error or warning
	{
		if ( m_journal_data.nState == JOURNAL_CONFIRMED )
		{
			m_journal_data.nState = JOURNAL_ARISE;
		}
		else
		{
			return;
		}
	}
	else
	{
        if ( m_journal_data.nState == JOURNAL_ARISE ) // occurs waring, so don't print message on bar.
        {
			// insert to grid and save to database. 
			m_journal_data.nType = nType;
			m_journal_data.nCode = nMsgCode;
 			insert_journal( str );

			message_list.push_back(str);

 			return;
        }
	}
	// update status bar.
	SetMessagePrompt( str, nType );

	// insert to grid and save to database. 
	m_journal_data.nType = nType;
	m_journal_data.nCode = nMsgCode;

	insert_journal( str );
 
}

void StateDialog::ShowInfo(LPCTSTR infoText, LPCTSTR caption, CWnd* parent, unsigned int showTimes /* = 0 */, int nType /* = JTMESSAGE */, int nMsgCode /* = 5050 */)
{
	if ( !parent || CString(infoText).IsEmpty() )
 		return;

	INFOBOX.ShowInfoBox( parent, infoText, caption, showTimes, nType );
// 	m_journal_data.nType = nType;
// 	m_journal_data.nCode = nMsgCode;
// 	insert_journal( infoText );
	 
}

void StateDialog::update_state_bar()
{
	if ( MWM.is_main_world_model_available() )
	{
		 SetRobotSpeed( MWM.get_external_speed() );
		 SetRobotState( MWM.get_robot_state().servo_on );
 		 SetRunningMode( MWM.get_robot_state().program_run );
   		 SetCoordinateSystem( MWM.get_prog_cart().refSys.selfname );
		 SetTool( MWM.get_prog_tool().refSys.selfname ); 
		 SetOperationMode( MWM.get_robot_state().operate );
		 SetProgramState(1);
		 SetSafeState(1);
		 SetUserGrade( MWM.get_userdata().nGrade ); 
		 SetSystemTime();
// 		 static int ddd = 0;
// 		 CString sss;
// 		 sss.Format(_T("hello robot control %d"),ddd++);
// 		 SetMessagePrompt(sss,0);
	}
}

// 点击信息确认按钮
void StateDialog::PromptConFirm() 
{
	m_journal_data.nState = JOURNAL_CONFIRMED; // reset state
   	ShowInfo(_T("")); // clear

	CString strmsg =_T("");
	if ( message_list.size() > 0 )
	{
		strmsg = message_list.back();
		message_list.pop_back();
 		ShowInfo(strmsg); // clear
 	}
 
	if ( SMC.isCodeSysMotion )
	{
		CmdInfo cf;
		cf.iCommandType = CMD_RESET;
		SMC.setCmdInfo(cf);
		Sleep(500);
		ShowInfo(_T(""));
	} 
}

void StateDialog::set_coordinate_bgcolor()
{
 
}

void StateDialog::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	ScreenToClient(&point); 
	CRect rcProject = label_list[LB_PROJECT_NAME]->rect;
	CRect rcMainMenuButton = label_list[LB_COMPANY_LOGO]->rect;
	if ( rcProject.PtInRect(point) )
	{
		if ( m_hParent )
		{
			::SendMessage( m_hParent, WM_ACTIVATE_WINDOW, winProgram,0 );
		}
	}
	SetFocus();
// 	else if ( rcMainMenuButton.PtInRect(point) )
// 	{
// 		if ( m_hParent )
// 		{
// 			::SendMessage( m_hParent, WM_ACTIVATE_WINDOW, winMainMenuDlg,0 );
//  		}
// 	}

	 
	DialogType::OnLButtonDblClk(nFlags, point);
}

void StateDialog::OnClickMainMenuBtn()
{
	if ( m_hParent )
	{
		::SendMessage( m_hParent, WM_ACTIVATE_WINDOW, winMainMenuDlg,0 );
	}
}

void StateDialog::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call DialogType::OnPaint() for painting messages
 
// 	CMemDC dcMem(dc, this);
// 
// 	CDC& memdc = dcMem.GetDC();
// 	memdc.FillSolidRect(0,0,800,92,kColor[2]);
// 	//   //	memdc.SetBkMode(TRANSPARENT);
// 	// 
// 	CPen* pOldPen = memdc.SelectObject(&m_whitePen);
//  	memdc.MoveTo(0,0);
// 	memdc.LineTo(150,0);
// 	memdc.MoveTo(0,0);
// 	memdc.LineTo(0,65);
// 	memdc.SelectObject(pOldPen);
}


BOOL StateDialog::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
// 	main_menu_btn.SetBk(pDC);
// 	prompt_confirm_.SetBk(pDC);
	BOOL ret = DialogType::OnEraseBkgnd(pDC);
	CRect rect;
	GetClientRect(&rect);
	pDC->FillSolidRect(&rect, kColor[2]);

 	prompt_confirm_.SetBk(pDC);
	main_menu_btn.SetBk(pDC); 

	return ret;
}
