// Copyright 2016, 亚思信息科技有限责任公司
// All right reserved.
//
// Author: Liu Run Dan ( lrdcz@163.com )
//
// Yasi Robot Motion Control System - 亚思机器人运动控制系统

// YsbotControlDlg.cpp : implementation file
//

#include "stdafx.h"
#include "YsbotControl.h"
#include "YsbotControlDlg.h"
#include "afxdialogex.h"
#include "Journal.h"
#include "stringconvert.h"
#include "random.h"
#include "LuaScript/ScriptVariables.h"
#include "LuaScript/LuaScript.h"
#include "LuaScript/ScriptFunc.h"
#include "PoseTransform.h"
#include "VisionCoordTransform.h"
#include "LuaScript/ScriptVariableType.h"
#include "variable_manager.h"
#include "hotkey_id.h"
#include "MiniDump.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CYsbotControlDlg dialog

CYsbotControlDlg::CYsbotControlDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CYsbotControlDlg::IDD, pParent)
{
 	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_nVkFirst = 0;
	m_nFocusAxis = 0;
	autoRunFlag = false;
	the_teach_pendant = NULL;
	the_simulator = NULL;
	the_vision_processing = NULL;
	left_dialog_ = NULL;
	time1 = 0;

	iMainWinWidth  = 800;
	iMainWinHeight = 600;
	m_pwndMainDlg = NULL;
}

CYsbotControlDlg::~CYsbotControlDlg() 
{
	;
}

void CYsbotControlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
//	DDX_Control(pDX, IDC_EDIT_SCRIPT, m_edit_script);
}

BEGIN_MESSAGE_MAP(CYsbotControlDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_SHOWWINDOW()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
 	ON_WM_TIMER()
  	ON_MESSAGE(WM_COMM_RXCHAR,  &CYsbotControlDlg::OnMsgTeachPendant ) 
 	ON_MESSAGE(WM_ACQUIRE_KEY_INPUT, &CYsbotControlDlg::OnAcquireKeyInput)
	ON_MESSAGE(WM_RELEASE_KEY_INPUT, &CYsbotControlDlg::OnReleaseKeyInput)
	ON_MESSAGE(WM_EXIT_SYSTEM, &CYsbotControlDlg::OnExitSystem)
	ON_REGISTERED_MESSAGE(WM_ACTIVE_MSG, &CYsbotControlDlg::OnActiveMsg)

END_MESSAGE_MAP()

LRESULT CYsbotControlDlg::OnActiveMsg(WPARAM wParam, LPARAM lParam)
{
	::ShowWindow(m_hWnd, SW_SHOWNORMAL);
	::SetForegroundWindow(m_hWnd);
	return TRUE;
}

void CYsbotControlDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	SetWindowPos(this, 0, 0, iMainWinWidth, iMainWinHeight, SWP_NOZORDER);
}
 
void test_posetrans()
{
	cout << "******************************" << '\n';

 	PoseTransform ptrans;
	CartPos rob( 200, 100, 300, deg2rad(90), deg2rad(-30), deg2rad(10), 5 );
	CartPos tool( 10, 20, 30, 0, 0.68, 1.57 );
	CartPos work( 200, 100, 300, 0, 0, 0 );

	CartRef workref;
	workref.cpos = work;

	std::vector<CartPos> list = ptrans.get_rotate_tcp_path(ROTATE_Z_AXIS,deg2rad(30),&rob,&workref,&tool);
	for (unsigned int i = 0; i < list.size(); ++i )
	{
// 		cout << list[i] << '\n';
	}
	cout << "******************************" << '\n';

 
  	cout << "******************************" << '\n';

	double dfvec[] = { -1, 0, 1 };
	ColumnVector vecmove(3);
 
  	 
}

void test_vision_trans()
{
	Matrix rot(3,1);

	rot(1,1) = 3; rot(2,1) = 4; rot(3,1) = 5;

	Matrix rnormal = rot / rot.NormFrobenius();

	cout << rot << '\n';

	cout << rnormal << endl ;
 
	VisionCoordTransform vtrans;

	vector<Vec> vis;
	vector<CartPos> rob;
	 
	vis.push_back(Vec(122,140));
	vis.push_back(Vec(366,142));
	vis.push_back(Vec(374,358));
	rob.push_back(CartPos(290,400,300,deg2rad(180),0,deg2rad(180),5));	
	rob.push_back(CartPos(400,400,300,0,0,0));	
	rob.push_back(CartPos(400,200,300,0,0,0));	
 
	vtrans.vision_calibration(vis,rob);
	vtrans.save("coord_transform.cfg");

	vtrans.load("coord_transform.cfg");
	CartPos ddd = vtrans.vision_mapto_world(Vec(366,142));
	cout << ddd << endl;
	vis.clear();
	rob.clear();
}

long   __stdcall  CrashReportEx(_EXCEPTION_POINTERS*   excp)     
{    
	Create_Dump(excp,TRUE,TRUE);
 	return   EXCEPTION_EXECUTE_HANDLER;     
}  

BOOL CYsbotControlDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

 	//SetWindowPos( NULL,0, 0, iMainWinWidth, iMainWinHeight, SWP_NOZORDER );
	OnShowWindow(TRUE,0);

	SetUnhandledExceptionFilter(CrashReportEx);
	HMODULE        hKernel32;

	// Try to get MiniDumpWriteDump() address.
	hDbgHelp = LoadLibrary(_T("DBGHELP.DLL"));
	MiniDumpWriteDump_ = (MINIDUMP_WRITE_DUMP)GetProcAddress(hDbgHelp,  ("MiniDumpWriteDump") );
	//        d("hDbgHelp=%X, MiniDumpWriteDump_=%X", hDbgHelp, MiniDumpWriteDump_);

	// Try to get Tool Help library functions.
	hKernel32 = GetModuleHandle(_T("KERNEL32"));
	CreateToolhelp32Snapshot_ = (CREATE_TOOL_HELP32_SNAPSHOT)GetProcAddress(hKernel32, ("CreateToolhelp32Snapshot"));
	Module32First_ = (MODULE32_FIRST)GetProcAddress(hKernel32, ("Module32First"));
	Module32Next_ = (MODULE32_NEST)GetProcAddress(hKernel32, ("Module32Next"));

	// TODO: Add extra initialization here
	SetBackgroundColor(kColor[1], TRUE);
    
 	std::string configfile = ("config_files/armcfg.cfg");
	try
	{
		bool success = vread.append_from_file (configfile.c_str());
		if( !success )
		{
			std::string err = "未能打开配置文件: " + configfile;
			AfxMessageBox( Ansi2Unicode(err).c_str() );
		}
	 
		Journal::the_journal.set_mode(vread);

// 		LuaScript::getInstance()->registerClassToLua();		

// 		LVAR->read_var_from("UserPrograms/test.var");
	}
	catch (InvalidConfigurationException& e)
	{
		CString smsg;
		smsg = e.what();
		AfxMessageBox(smsg);
		JERROR(e.what());
	}
	catch ( YsbotsException& e )
	{
		CString smsg;
		smsg = e.what();
		AfxMessageBox(smsg);
	}

	string tt = string(vread.getenv());

	try
	{
		the_world_model      = new WorldModel ( vread );
		the_robot_kinematics = new RobotKinematics ( vread );
		the_servo            = new Servo ( vread ); 

		the_motion_control   = new MotionControl ( vread );
 	    the_fieldbus_device  = new FieldBusDevice ( vread );

// 		the_teach_pendant    = new AddComUserInterface(vread,&teach_pendant);
 		the_vision_processing = new VisionProcessing(vread);
  
		Ysbot::Time ti;
 		the_sql_database = new SqlDatabase();
 		cout << "load database use " << ti.elapsed_msec() << "ms" <<'\n';
 
	}
	catch ( YsbotsException& e )
	{
		CString smsg;
		smsg = e.what();
		AfxMessageBox(smsg);
		JERROR(e.what());
	}

	int      iMinLogLevel = 0;
	int      iMaxLogLevel = 50;  

	logout.addLogRange(iMinLogLevel,iMaxLogLevel);
	bSuppliedLogFile = false;
	if ( bSuppliedLogFile )
	{
		std::string filename = SQL_LOG_DIR + string("\\") + string("logfile.log");
		fileout.open( filename.c_str() );
        logout.setOutputStream( fileout ); 
	}
	else
	{
	    logout.setOutputStream( std::cout ); 
	}
	logout.restartTimer();
	
 
	LVAR->getInstance(); // can not delete, because have two single instances, one is var, the other is script.
 
// 	left_dialog_ = new LeftDialog(vread);
// 	left_dialog_->CreateModeDlg(_T(""), CRect(0, 0, 0, 0), FALSE, this);
// 	left_dialog_->ShowWindow(SW_SHOW);
  
	if ( m_pwndMainDlg == NULL )
	{
		m_pwndMainDlg = new CMainDlg(vread);
		m_pwndMainDlg->CreateModeDlg(_T(""), CRect(0, 0, 0, 0), FALSE,this);
		//m_pwndMainDlg->ShowWindow(SW_SHOW);
		//m_pwndMainDlg->CreateDialogFromTemplate(this, FALSE, WS_POPUP | WS_VISIBLE | SS_OWNERDRAW);
 	}

	unsigned int ui;
	if (vread.get ("random_seed", ui)>0)
		random_seed (ui);

//   	test_vision_trans();
 
	PosInfo rloc;
	rloc.axis.set_rad(0,0,0,0,0,0);
 
	int nret = the_motion_control->init_system();
	if ( nret != 0 )
	{
		if ( MSD.is_main_state_dialog_available() )
		{
			MSD.ShowInfo(_T("运动控制模块初始化失败！"),JTERROR,3020,true);
 		}
	}
	else
	{
 		if ( SMC.isCodeSysMotion )
		{
			SMC.mc_dout( 0, 1, MCS_REMOTEIO ); // output ready signal. 
		}
		if ( MSD.is_main_state_dialog_available() )
		{
			MSD.ShowInfo(_T("系统启动成功"));
 		}
	}

	ScriptFunc::init_script_function(); // ?????????

	SetTimer(0,60,NULL); // 10
 

//////////////////////////////////////////////////////////////////////////
	CSerialPort::receive_message = true;
  
	 
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CYsbotControlDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CYsbotControlDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}
 
void CYsbotControlDlg::OnDestroy()
{
	CDialogEx::OnDestroy();
	if ( bSuppliedLogFile )
 		fileout.close();
 
   	SAFE_DELETE(the_robot_kinematics);
	SAFE_DELETE(the_servo);
	SAFE_DELETE(the_fieldbus_device);
 	SAFE_DELETE(the_motion_control);
	SAFE_DELETE(the_world_model);
	SAFE_DELETE(the_teach_pendant);
 	SAFE_DELETE(the_vision_processing);
  	SAFE_DELETE(the_sql_database);
	
	bool isReboot = false;
	if ( left_dialog_ )
	{
		bool isReboot = left_dialog_->isRebootSystem();
 	}
	SAFE_DELETE(left_dialog_);

	SAFE_DELETE(m_pwndMainDlg);
 
	if ( isReboot )
	{
		TCHAR pBuf[MAX_PATH];

		//获取应用程序完全路径 
		GetModuleFileName(NULL,pBuf,MAX_PATH);

		STARTUPINFO startupinfo;
		PROCESS_INFORMATION proc_info;

		memset(&startupinfo,0,sizeof(STARTUPINFO));
		startupinfo.cb=sizeof(STARTUPINFO);

 		::CreateProcess( pBuf,NULL,NULL,NULL,FALSE,
			NORMAL_PRIORITY_CLASS,NULL,NULL,&startupinfo,&proc_info);
	}
	 
/*	program_dialog->on_free_script();*/
 	TRACE(_T("Program normal over.\n"));
}

 
void CYsbotControlDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
 	Ysbot::Time timeloop;
  
  	CString msg;
	msg.Format(_T("motion status = %d\n"), static_cast<int>( MWM.get_robot_state().mc_info.iMotionStatus ) );
// 	TRACE(msg);
	
	long inval = 0;
    Ysbot::Time t;	

 	the_fieldbus_device->process_io_signals(t);
	the_vision_processing->process_vision();
	the_motion_control->process_motion_information(t);
	the_servo->process_servo_message();
 
	if ( left_dialog_ )
	{
		left_dialog_->update_window();
 	}
  	
	if ( left_dialog_ && left_dialog_->isExitSystem() )
	{
		CDialogEx::OnOK();
	}
    
	if ( m_pwndMainDlg  )
	{
		if ( m_pwndMainDlg->isExitSystem() )
		{
			OnCancel();
		}
		else
		{
			m_pwndMainDlg->update_window();
		}

	}

 	CDialogEx::OnTimer(nIDEvent);
}
 
BOOL CYsbotControlDlg::PreTranslateMessage(MSG* pMsg)
{
  	if( pMsg->message == WM_KEYDOWN    || pMsg->message == WM_KEYUP    || 
		pMsg->message == WM_SYSKEYDOWN || pMsg->message == WM_SYSKEYUP )
	{
		cout << "key press \n";
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}


void CYsbotControlDlg::OnKeyboardInput(BOOL bUpAndDown, int nVirtualKey)
{
	cmd.iCommandType  = CMD_JOG;

	if( bUpAndDown ) // Key up
	{
		if( nVirtualKey == m_nVkFirst )
		{
			switch( nVirtualKey )
			{
			case VK_RIGHT:
			case VK_LEFT:
				cmd.iCommandType = CMD_ABORT;
				the_motion_control->setCmdInfo(cmd);
				break;
			}
			m_nVkFirst = 0;
		}
	}
	else if( m_nVkFirst == 0 || m_nVkFirst == nVirtualKey ) // Key down
	{
		switch( nVirtualKey )
		{
		case VK_UP:		m_nFocusAxis = (m_nFocusAxis + 5) % 6; break;
		case VK_DOWN:	m_nFocusAxis = (m_nFocusAxis + 1) % 6; break;
		case VK_ADD:  break;    // increase speed rate
		case VK_SUBTRACT: break;// decrease speed rate
		default:
			if( m_nVkFirst == 0 )
			{
				switch( nVirtualKey )
				{
				case '1': case '2': case '3':
				case '4': case '5': case '6':
				case '7': case '8': case '9':
					m_nFocusAxis = nVirtualKey - '1';

					break;
				case 'J':  cmd.iJogCoordType = jtJOINT;  cout << " joint mode axis!!" << '\n';	break;
				case 'C':  cmd.iJogCoordType = jtUSER;   cout << " carti mode User!!" << '\n';	break;
				case 'T':  cmd.iJogCoordType = jtTOOL;   cout << " carti mode Tool!!" << '\n';	break;
				case 'W': // World coordinate
					break;
				case 'G': autoRunFlag = !autoRunFlag; cout << " auto run!!" << '\n';	break;
				case VK_RIGHT:
				case VK_LEFT:
					if( the_motion_control->get_group_status() == 1 )// GMS_STOP
					{
						dir = (nVirtualKey == VK_RIGHT) ? 1: -1;
						nAxis = m_nFocusAxis;

						cmd.chJogAxisMark = 1 << nAxis;
						cmd.chJogDirMark  = (nVirtualKey == VK_RIGHT) ? 0: (1 << nAxis);

						cmd.dyn.custom_parameter = false;
						cmd.dyn.JointVel  = MWM.get_external_speed() > 10 ? 10 : MWM.get_external_speed();

						the_motion_control->setCmdInfo(cmd);

						// 						TRACE(_T(" jog dir = %d axis = %d\n"),cmd.chJogDirMark, cmd.chJogAxisMark );

						double pos[MAX_JOINT_NUM] = {0};

						// 						the_motion_control->get_position(pos,mcACS,LDScommad,true);
						// 						cout <<"******************************************************************\n";
						// 						cout << pos[0] << " " <<pos[1] << " "<< pos[2] << " "<< pos[3] << " "<< pos[4]<< " " << pos[5] << '\n'<< '\n';

						PosInfo pi = the_motion_control->get_position(PosInfo::CTmachine);

						CString str;
						str.Format(_T(" pos = %.2f %.2f %.2f  %.2f %.2f %.2f\n"), 
							pi.cart.pos.x,pi.cart.pos.y, pi.cart.pos.z, 
							pi.cart.ang.x,pi.cart.ang.y, pi.cart.ang.z );
						TRACE(str);
						//OutputDebugString(str);
						cout << pi.cart.pos << " "<< pi.cart.ang << " "<< '\n'<< '\n';

						// 						CString str;
						// 						str.Format(_T(" %.2f %.2f  %.2f %.2f  %.2f %.2f \n"),
						// 							pos[0], pos[1],pos[2], pos[3], pos[4], pos[5] );
						// 						TRACE(str);
					}
					break;
				default:break;
				}

				MWM.update_robot_state(cmd);
			}
			break;
		}

		m_nVkFirst = nVirtualKey;
	}
}

void CYsbotControlDlg::auto_run_pro()
{
	MotionCoreInfo gi = the_motion_control->get_motion_core_information();
  
	if ( gi.iMotionStatus == 1 ) // stop
	{
		cmd.iCommandType = CMD_PTP;
		cmd.destPos.pos_type = PosInfo::CTaxis;
		cmd.destPos.set_axis_deg(0,45,90,0,45,0);
  		the_motion_control->setCmdInfo(cmd);

		cmd.iCommandType = CMD_DELAYING;
		cmd.iDelayTime   = 2000;
		the_motion_control->setCmdInfo(cmd);

		cmd.iCommandType = CMD_PTP;
 		cmd.destPos.pos_type = PosInfo::CTaxis;
		cmd.destPos.set_axis_deg(-10,45,90,0,45,0);
 		the_motion_control->setCmdInfo(cmd);

		cmd.iCommandType = CMD_DELAYING;
		cmd.iDelayTime   = 2000;
		the_motion_control->setCmdInfo(cmd);

		cmd.iCommandType = CMD_PTP;
		cmd.destPos.pos_type = PosInfo::CTaxis;
		cmd.destPos.set_axis_deg(10,45,90,0,45,0);
		the_motion_control->setCmdInfo(cmd);

		cmd.iCommandType  = CMD_LIN;
 		cmd.destPos.pos_type = PosInfo::CTaxis;
		cmd.destPos.set_axis_deg(7.52, 45.85,  88.53, 0.00,  45.62, 7.49);
   		the_motion_control->setCmdInfo(cmd);
//   
// 		dest_pos.set_deg(-10, 45,  90.0, 0.00,  45.0, 0.0);
// 		cmd.destAxisPos.axis = dest_pos;
// 
//  		the_motion_control->setCmdInfo(cmd);
// 
		cmd.iCommandType  = CMD_LIN;
		cmd.destPos.pos_type = PosInfo::CTaxis;
 		cmd.destPos.set_cart_pos( 1097.81, -100.00, 397,  3.14, 0.00, -3.14, 5 );
		the_motion_control->setCmdInfo(cmd);

		autoRunFlag = false;
 	}
}

/*
void CYsbotControlDlg::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class
	m_edit_script.SaveFile("UserPrograms/4.lua");
	m_edit_script.FreeContext();

	CDialogEx::OnOK();
}

void CYsbotControlDlg::OnCancel()
{
	m_edit_script.FreeContext();
 	CDialogEx::OnCancel();
}
*/

// LRESULT CYsbotControlDlg::OnHotKey(WPARAM wp, LPARAM lp)
// {
// 	UINT modifiers = (UINT)LOWORD(lp);
// 	UINT virtual_key = (UINT)HIWORD(lp);
// 
//  	// 调用程序界面
// 	if ( MOD_CONTROL == modifiers && 'D' == virtual_key) {
// 		return 0;
// 	}
// 	if ( MOD_SHIFT == modifiers && '8' == virtual_key ) 
// 	{
//  		the_simulator->startup_simulator();
//  	}
// 	if ( MOD_SHIFT == modifiers && '9' == virtual_key )
// 	{
// 		the_simulator->close_simulator();
// 	}
// 
//   	left_dialog_->set_pc_keydata(modifiers,virtual_key);
// //     the_teach_pendant->set_pc_keydata(modifiers,virtual_key);
//  	 
// 	return 0;
// }
 
LRESULT CYsbotControlDlg::OnMsgTeachPendant( WPARAM RecByte, LPARAM portNB )
{
	if ( left_dialog_ && !left_dialog_->isExitSystem() && the_teach_pendant)
	{
		the_teach_pendant->ProcessReceiveChar( (unsigned char*)(RecByte) ) ;
	}
	return true;
}
 
LRESULT CYsbotControlDlg::OnAcquireKeyInput(WPARAM wParam, LPARAM lParam)
{
	CYsbotControlApp  *pApp = (CYsbotControlApp*) AfxGetApp();
	return pApp->AcquireKeyInput((HWND) wParam, (BYTE*) lParam) ? 1: 0;
}

LRESULT CYsbotControlDlg::OnReleaseKeyInput(WPARAM wParam, LPARAM lParam)
{
	((CYsbotControlApp*) AfxGetApp())->ReleaseKeyInput();
	return 1;
}

LRESULT CYsbotControlDlg::OnExitSystem(WPARAM wParam, LPARAM lParam)
{
	 OnCancel();
	 return 1;
}