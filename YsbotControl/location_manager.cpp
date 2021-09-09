
// Copyright 2016, 亚思信息科技有限责任公司
// All right reserved.
//
// Author: Liu Run Dan ( lrdcz@163.com )
//
// Yasi Robot Motion Control System - 亚思机器人运动控制系统

#include "stdafx.h"
#include "location_manager.h"
#include "WorldModel.h"
#include "MotionControl.h"
#include "LuaScript/ScriptVariables.h"
#include "stringconvert.h"
#include "TeachDialog/TeachDlg.h"
#include "program_dialog.h" 
#include "UIDialogs/state_dialog.h"

LocationDialog *location_dialog = nullptr;

LPCTSTR LocationDialog::cart_label[8][2] = { 
	{ _T("X"), _T(" 毫米") }, 
	{ _T("Y"), _T(" 毫米") }, 
	{ _T("Z"), _T(" 毫米") },
    { _T("A"), _T(" 度")   }, 
	{ _T("B"), _T(" 度")   }, 
	{ _T("C"), _T(" 度")   },
    { _T("A7"), _T(" 度")   },
	{ _T("A8"), _T(" 度")   } } ;


LocationDialog::LocationDialog() :show_coordinate_type(SHOW_JOINT)
{
	grid_row_count = 8;
	m_reDrawLable = FALSE;
}

LocationDialog::~LocationDialog() 
{
	for ( int i = 0; i < ARRAY_SIZE(m_IconPower); ++i )
	{
		DestroyIcon( m_IconPower[i] );
		m_IconPower[i] = NULL;
	}
}

BEGIN_MESSAGE_MAP(LocationDialog, DialogType)
	ON_MESSAGE(WM_UPDATE_COMMBOX,		&LocationDialog::OnUpdateLocationCommbox )
 	ON_NOTIFY( WM_CUSTOMCOMBOX_SELCHANGED, LOCATION_DIALOG_COORDINATE, &LocationDialog::OnSelchangeCommboUserCoord)
	ON_NOTIFY( WM_CUSTOMCOMBOX_SELCHANGED, LOCATION_DIALOG_TOOL, &LocationDialog::OnSelchangeCommboTool)
	ON_COMMAND_RANGE( IDC_POWER_OFF_BTN, IDC_POWER_ON_BTN, &LocationDialog::OnPowerButtonClicked )
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

void LocationDialog::DoDataExchange(CDataExchange* pDX) {

}
  
BOOL LocationDialog::OnInitDialog() 
{
	CDialogEx::OnInitDialog();
	//SetWindowPos(NULL, g_x0 + 20, 95, 730, 439, SWP_NOZORDER);
	if ( GetParent()->GetDlgItem(IDC_BOX_MAIN)->GetSafeHwnd() )
	{
		GetParent()->GetDlgItem(IDC_BOX_MAIN)->GetClientRect(&clientRect);
		SetWindowPos(NULL,clientRect.left, clientRect.top, clientRect.Width(), clientRect.Height(), SWP_NOZORDER);
	}
	SetBackgroundColor(kColor[8]);
	m_iWidthGrid  = (int)(clientRect.Width() * 5/8);
	m_iHeightGrid = (int)(clientRect.Height()*2/3);


// 	ModifyStyleEx(WS_EX_STATICEDGE, WS_EX_CLIENTEDGE, SWP_DRAWFRAME);
	// 创建表格

	// 创建标题一
	title1_.Create(_T("机器人名称"), WS_CHILD | WS_VISIBLE,
		CRect(470, 20, 580, 40), this);
	title1_.SetAlignment(1);
	title1_.Initialize(kColor[0]);
	// 创建机器人名称下拉框
	robot_name_.Create(WS_CHILD | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED |CBS_HASSTRINGS,
		CRect(470, 50, 710, 70), this, LOCATION_DIALOG_ROBOT_NAME);
	robot_name_.AddString(_T("Dear"));
	robot_name_.SetCurSel(0);
	 
	robot_name_.EnableWindow(FALSE);
	robot_name_.ShowWindow(SW_HIDE);

	COLORREF m_crBackgroud = RGB(220, 220, 220);
	COLORREF m_crText      = RGB(96, 100, 96);
 
 	robot_name_label.Create( _T(""), WS_CHILD | WS_VISIBLE | DT_LEFT | SS_CENTERIMAGE,
		CRect(470, 50, 710, 85), this, LOCATION_DIALOG_ROBOT_NAME );
	robot_name_label.SetBkColor(m_crBackgroud)
		.SetTextColor(m_crText)
		.SetBorder(TRUE)
		.SetFontBold(TRUE)
		.SetText(_T("Dear60X"))
		.SetLabelIcon(IDI_VARIABLE_SYSTEM,16,16);

 
	// 创建标题二
	title2_.Create(_T("参考坐标"), WS_CHILD | WS_VISIBLE,
		CRect(470, 100, 560, 120), this);
	title2_.SetAlignment(1);
	title2_.Initialize(kColor[0]);
	// 创建机器人名称下拉框
	coordinate_.Create(WS_CHILD | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED |CBS_HASSTRINGS,
		CRect(470, 130, 710, 150), this, LOCATION_DIALOG_COORDINATE);
// 	coordinate_.AddString(_T("基坐标系"));
//  	coordinate_.SetCurSel(0);
	 
	// 创建标题三
	title3_.Create(_T("工具坐标"), WS_CHILD | WS_VISIBLE,
		CRect(470, 190, 560, 210), this);
	title3_.SetAlignment(1);
	title3_.Initialize(kColor[0]);
	// 创建 下拉框
	tool_.Create(WS_CHILD | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED |CBS_HASSTRINGS,
		CRect(470, 220, 710, 220), this, LOCATION_DIALOG_TOOL);
	tool_.SetAlignTop(TRUE);
// 	tool_.AddString(_T("Flange"));
// 	tool_.SetCurSel(0);
	 
	// 创建标题四
	title4_.Create(_T("速度：            毫米/秒"), WS_CHILD |	WS_VISIBLE, CRect(470, 280, 720, 300), this);
	title4_.SetAlignment(1);
	title4_.Initialize(kColor[0]);
	// 创建机器人速度
 
	LOGFONT font;
	memset(&font,0,sizeof(LOGFONT));
	font.lfHeight = 22;
	_tcsnccpy_s(font.lfFaceName,LF_FACESIZE,_T("Arial"),sizeof(LOGFONT));
 
	m_act_speed.Create( _T(""), WS_CHILD | WS_VISIBLE | DT_LEFT | SS_CENTERIMAGE,
		CRect(530, 280, 600, 300), this );
	m_act_speed.SetBkColor(m_crBackgroud)
		.SetBorder(FALSE)
        .SetFont(font)
		.SetFontBold(TRUE);
	SetSpeed(0.00);
 
	// 创建标题五
	title5_.Create(_T("模式："), WS_CHILD | WS_VISIBLE,	CRect(470, 320, 710, 340), this);
	title5_.SetAlignment(1);
	title5_.Initialize(kColor[0]);
	// 创建模式
 	m_robot_mode.Create( _T(""), WS_CHILD | WS_VISIBLE | DT_LEFT | SS_CENTERIMAGE,
		CRect(530, 320, 560, 340), this );
	m_robot_mode.SetBkColor(m_crBackgroud)
		.SetBorder(FALSE)
		.SetFont(font)
		.SetFontBold(TRUE);
	SetMode(5);

	// 创建标题六
	title6_.Create(_T("点动速度："), WS_CHILD | WS_VISIBLE,	CRect(470, 360, 710, 380), this);
	title6_.SetAlignment(1);
	title6_.Initialize(kColor[0]);
	// 创建点动速度
 
	m_jog_speed.Create( _T(""), WS_CHILD | WS_VISIBLE | DT_LEFT | SS_CENTERIMAGE,
		CRect(570, 360, 660, 380), this );
	m_jog_speed.SetBkColor(m_crBackgroud)
 		.SetBorder(FALSE)
		.SetFont(font)
  		.SetFontBold(TRUE);
	SetJogSpeed(10);
   
	CRect rectBox = CRect( CPoint(5,m_iHeightGrid), CSize(m_iWidthGrid/3,clientRect.Height()-m_iHeightGrid-5) );
	if( !m_GroupBoxMotor.GetSafeHwnd() )
	{
		m_GroupBoxMotor.Create(_T("电机"),BS_GROUPBOX|WS_CHILD, rectBox,this,8888);//|WS_VISIBLE
	} 

 	create_tree_grid();

	init_commbox();
	CIconComboBox::set_DropDownHight(coordinate_,6);
	CIconComboBox::set_DropDownHight(tool_,6);

	m_IconPower[POWER_OFF] = (HICON)::LoadImageW(NULL, _T("res/power_off.ico"), IMAGE_ICON, 120, 60, LR_LOADFROMFILE);
	m_IconPower[POWER_ON] = (HICON)::LoadImageW(NULL, _T("res/power_on.ico"), IMAGE_ICON, 120, 60, LR_LOADFROMFILE);
 	CRect rectPowerBtn[2];
	rectPowerBtn[POWER_OFF] = CRect(CPoint(rectBox.left+10,rectBox.top + 10),CSize( 125,(int)(rectBox.Height()/2.0)-10 ));
    rectPowerBtn[POWER_ON] = rectPowerBtn[POWER_OFF];
	rectPowerBtn[POWER_ON].OffsetRect(0, rectPowerBtn[POWER_OFF].Height()+5 );
	for ( int i =0; i < ARRAY_SIZE(m_BtnServoPower); ++i )
	{
		m_BtnServoPower[i].Create( _T(""), WS_VISIBLE| WS_CHILD | WS_TABSTOP, rectPowerBtn[i],this, IDC_POWER_OFF_BTN+i );
		m_BtnServoPower[i].SetDefaultColors();
		m_BtnServoPower[i].SetIcon(m_IconPower[i],BTNST_AUTO_DARKER);//
		m_BtnServoPower[i].DrawTransparent(TRUE);
		m_BtnServoPower[i].DrawBorder(FALSE);
		m_BtnServoPower[i].SetFlat(TRUE);
		m_BtnServoPower[i].SetAlign(CButtonST::ST_ALIGN_OVERLAP,FALSE);
	}
	m_BtnServoPower[POWER_OFF].SetWindowText(_T("关闭伺服"));
	m_BtnServoPower[POWER_ON].SetWindowText(_T("接通伺服"));

	 
	return TRUE;
}

void LocationDialog::set_curr_tool()
{
	//1.16 mm, -82.20 mm, 101.79 mm, 90.00°, -60.00°, 0.00°
// 	ScriptVariables::getInstance()->get_value(svTOOL,"tool1",&curr_jog_tool);
}

LRESULT LocationDialog::OnUpdateLocationCommbox( WPARAM wp, LPARAM lp )
{
	bool bAddVar = ((int)wp == 1) ? true : false;
	CString strval = *(CString*)lp;
	CString varname = strval.SpanExcluding(_T(":"));
	CString vartype = strval.TrimLeft(varname+':');
	std::string tooltype = LVAR->get_type_name(svTOOL);
	std::string carttype = LVAR->get_type_name(svCARTREF);
	vartype.Trim();
	varname.Trim();
	DWORD dwData = MAKEWORD(svTOOL,SVAR_PROGRAM);
	if ( CString(tooltype.c_str()) == vartype )
	{
		if ( bAddVar )
			add_tool(CStringA(varname),dwData);
		else
			delete_tool(CStringA(varname));
	}
	else if ( CString(carttype.c_str() ) == vartype )
	{
		dwData = MAKEWORD(svCARTREF,SVAR_PROGRAM);
		if ( bAddVar )
			add_user_coordinate(CStringA(varname),dwData);
		else
			delete_user_coordinate(CStringA(varname));
 	}
 
	return 1;
}

void LocationDialog::add_user_coordinate(const char* usercoord,DWORD dwData)
{
	coordinate_.AddString(CString(usercoord),dwData);
}

void LocationDialog::delete_user_coordinate(const char* usercoord)
{
	int  index = coordinate_.FindStringExact(0,CString(usercoord));
	if ( index >= 0 )
	{
		coordinate_.DeleteString(index);
 	}
	if ( coordinate_.GetCount() > 0 )
	{
		coordinate_.SetCurSel(0);
	}
}


void LocationDialog::add_tool(const char* toolname,DWORD dwData)
{
	tool_.AddString(CString(toolname),dwData);
}

void LocationDialog::delete_tool(const char* toolname)
{
	int  index = tool_.FindStringExact(0,CString(toolname));
	if ( index >= 0 )
	{
		tool_.DeleteString(index);
	}
	if ( tool_.GetCount() > 0 )
	{
		tool_.SetCurSel(0);
	}
}

void LocationDialog::init_commbox()
{
 	std::string varname = "";
	 
	DWORD dwData = 0;
 	ScriptVarSetT svarset = SVAR_SYSTEM;
    const std::map<ScriptVarT, std::map<std::string, void*>>* var_list = NULL;
	while (svarset != SVAR_SET_NUM)
	{
		var_list = LVAR->get_var_list(svarset);

		for( map<ScriptVarT,  map<string, void*>>::const_iterator it = var_list->begin(); 
			it != var_list->end(); 
			it++ ) 
		{
			for ( map<string, void*>::const_iterator innerIt = it->second.begin(); 
				innerIt != it->second.end(); 
				innerIt++ ) 
			{
				varname = innerIt->first;
				dwData = MAKEWORD( it->first, svarset );
				if ( it->first == svTOOL )
				{
					add_tool(varname.c_str(),dwData);
				}
				else if( it->first == svCARTREF )
				{
					add_user_coordinate(varname.c_str(),dwData);
				}
 			}
		}

	    svarset = ScriptVarSetT((int)svarset + 1);
	}

 	ScriptVarSetT varset = SVAR_SYSTEM;
	CString csValue;
	RefSysSetlInfo refInfo;

	if ( coordinate_.GetCount() > 0 )
	{
		coordinate_.SetCurSel(0);

		get_cartref_data(0,&varset);

		coordinate_.GetLBText(0,csValue);
		CartRef curr_coord;
		std::string varname( Unicode2Ansi( csValue.GetBuffer(0) ).c_str() );
		LVAR->get_value(svCARTREF,varname,&curr_coord,varset);

		refInfo.refSet = varset;
		refInfo.refSys = curr_coord;
		MWM.set_jog_cart(refInfo);
	}

	if ( tool_.GetCount() > 0 )
	{
		tool_.SetCurSel(0);

		get_tool_data(0,&varset);

		tool_.GetLBText(0,csValue);
		CartRef curr_tool;
		std::string varname( Unicode2Ansi( csValue.GetBuffer(0) ).c_str() );
		LVAR->get_value(svTOOL,varname,&curr_tool,varset);

		refInfo.refSet = varset;
		refInfo.refSys = curr_tool;
		
		MWM.set_jog_tool(refInfo);
	}
}
 
// 表格上显示数据
void LocationDialog::SetType() 
{
 	update_pos_grid_text();
}

void LocationDialog::SetData(double *data) {
 	update_pos_grid_data(data);
}

void LocationDialog::SetSpeed(double speed) {
	CString str;
	str.Format(_T("%.2lf"), speed);
	//robot_speed_.ShowText(str,RGB(0,0,0),RGB(220,220,220));
 	//robot_speed_.SetWindowTextW(str);
	m_act_speed.SetText(str,m_reDrawLable);
}

void LocationDialog::SetMode(int mode) 
{
	CString str;
	str.Format(_T("%d"), mode);
// 	mode_.ShowText(str,RGB(0,0,0),RGB(220,220,220));
	//mode_.SetWindowTextW(str);
 	m_robot_mode.SetText(str, m_reDrawLable );

}

void LocationDialog::SetJogSpeed(double speed) {
	CString str;
	str.Format(_T("%.1f"), speed);
	str = str + _T("%");
// 	jog_speed_.ShowText(str,RGB(0,0,0),RGB(220,220,220));
	//jog_speed_.SetWindowTextW(str);
	 
	m_jog_speed.SetText(str, m_reDrawLable );

}

void LocationDialog::update_location_window()
{
	if ( ::IsWindow( GetSafeHwnd() ) && IsWindowVisible() )
	{
		PosInfo joint = SMC.get_position( PosInfo::CTaxis,LDScommad );
		PosInfo cart  = SMC.get_position( PosInfo::CTmachine,LDScommad );
		curr_jog_cart_ref = MWM.get_jog_cart().refSys;
		curr_jog_tool = MWM.get_jog_tool().refSys;

		switch(show_coordinate_type)
		{
		case SHOW_SERVO: show_servo_pos(); break;
		case SHOW_JOINT: show_axis_pos(joint);	break;
		case SHOW_WORLD: show_cart_pos(cart, NULL, NULL );	break;
 		case SHOW_WORK:  show_cart_pos(cart, &curr_jog_cart_ref, &curr_jog_tool.cpos );	break;
		default:      break;
		}

		SetJogSpeed(MWM.get_external_speed());
		const RobotState& rs = MWM.get_robot_state();
		double speed = rs.mc_info.dfCurFeedSpeed;
		SetSpeed(speed*100.0);
 	}
}

void LocationDialog::show_axis_pos( const PosInfo& ap )
{
	double dfpos[12] = {0};
	AxisPos apos = ap.axis.get_deg();
	dfpos[0] = apos.q1; dfpos[1] = apos.q2; dfpos[2] = apos.q3;
	dfpos[3] = apos.q4; dfpos[4] = apos.q5; dfpos[5] = apos.q6;
	dfpos[6] = rad2deg(ap.auxi.q1); dfpos[7] = rad2deg(ap.auxi.q2); 
	SetData(dfpos);
	SetMode(ap.cart.fig);
}

void LocationDialog::show_cart_pos( const PosInfo& cp, const CartRef* cref, const CartPos* tool )
{
	double dfpos[12] = {0};
	CartPos tcp;
	MWM.calc_tcp_pose( &cp.cart, cref, tool, &tcp );

	dfpos[0] = tcp.pos.x;           dfpos[1] = tcp.pos.y;          dfpos[2] = tcp.pos.z;
	dfpos[3] = rad2deg(tcp.ang.x);  dfpos[4] = rad2deg(tcp.ang.y); dfpos[5] = rad2deg(tcp.ang.z);
	dfpos[6] = rad2deg(cp.auxi.q1); dfpos[7] = rad2deg(cp.auxi.q2); 
	SetData(dfpos);
	SetMode(cp.cart.fig);
}

// future update.
void LocationDialog::show_servo_pos()
{
	double dfpos[12] = {0};
	const PosInfo& spos = MWM.get_servo_position();
	AxisPos apos = spos.axis.get_deg();
	dfpos[0] = apos.q1; dfpos[1] = apos.q2; dfpos[2] = apos.q3;
	dfpos[3] = apos.q4; dfpos[4] = apos.q5; dfpos[5] = apos.q6;
	dfpos[6] =  rad2deg(spos.auxi.q1); dfpos[7] =  rad2deg(spos.auxi.q2); 

	SetData(dfpos);
}

LRESULT LocationDialog::OnMainButtonClicked( WPARAM wp, LPARAM lp)
{
	int btnid  = (int)(wp);
	int menuid = (int)(lp);

	cout << "LocationDialog id = " << btnid << '\n';
	CmdInfo cmd;
	PosInfo startpi;
	AxisPos ap;
	ap.set_deg(0,30,110,0,30,0);
 	startpi.axis = ap;

	double speed_percent = 0.01;
	switch(btnid)
	{
	case 0: 
		{
			switch(menuid)
			{ 
			case 0: show_coordinate_type = SHOW_SERVO; break;
			case 1: show_coordinate_type = SHOW_JOINT;  break;
			case 2:  show_coordinate_type = SHOW_WORLD;  break;
			case 3: show_coordinate_type = SHOW_WORK;  break;
 			default: break;
			}
		}
		break;
	case 1: 
		{
			OperateMachineT omt;
			switch(menuid)
			{ 
			case 0: omt = omTEACH; break;
			case 1: omt = omPLAY;  break;
			case 2:  omt = omREMOTE;  break;
 			default: break;
			}
 
			if (MWM.is_main_world_model_available())
			{
				OperateMachineT old = MWM.get_robot_state().operate;
				if ( old != omt )
				{
					if ( ProgramEditor::is_running_flag )
					{
						program_dialog->on_free_script();
// 						if ( MWM.get_robot_state().run_light == lsON )
// 						{
// 							//set_command( false, RUNSIG );
// 							MWM.set_run_light(lsOFF);
// 						}
					}
				}
				MWM.set_operation_mode(omt);
			}
		}
		break;
	case 2: 
		{
			GetParent()->SendMessage(WM_ACTIVATE_WINDOW,winTeachPanelDlg,0);
  		}
		break;
	case 3:   break;
	case 4: 
		{
			switch(menuid)
			{ 
			case 0: set_speed_percent(100); break;
			case 1: set_speed_percent(50);  break;
			case 2: set_speed_percent(25);  break;
			case 3: set_speed_percent(10);  break;
			case 4: set_speed_percent(1);   break;
			case 5: set_speed_percent(0.1); break;
			default: break;
			}
		}
		break;
	case 5:
		{
			switch(menuid)
			{ 
			case 0: MWM.set_jog_coordinate(jtJOINT); break;
			case 1: MWM.set_jog_coordinate(jtWORLD); break;
			case 2: MWM.set_jog_coordinate(jtTOOL);  break;
			case 3: MWM.set_jog_coordinate(jtUSER);  break;
			case 4: // 回零
				{
					cmd.iCommandType = CMD_SETPOSITION;
					cmd.customStartPos = true;
					cmd.startPos = startpi; 
					SMC.setCmdInfo(cmd); 
				}
				break;
 			default: break;
			}
		}
		break;
	default:break;
	}
	SetType(); 
  
	return 1;
}

void LocationDialog::set_speed_percent( double speed )
{
	SetJogSpeed(speed);
// 	CString strspeed;
// 	strspeed.Format(_T("%.f %%"),speed);
// 	if ( speed < 1)
//  		strspeed.Format(_T("%.1f %%"),speed);
// 
//   	jog_speed_.SetWindowText(strspeed);
	MWM.set_external_speed(speed);
}

void LocationDialog::OnSelchangeCommboUserCoord(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	int index = coordinate_.GetCurSel();
	if ( index >= 0 )
	{
 		ScriptVarSetT varset = SVAR_SYSTEM;
		get_cartref_data(index, &varset);

		CString csValue;
		coordinate_.GetLBText(index,csValue);
		CartRef curr_coord;
		std::string varname( Unicode2Ansi( csValue.GetBuffer(0) ).c_str() );
		LVAR->get_value(svCARTREF,varname,&curr_coord,varset);
 
		RefSysSetlInfo refcart;
		refcart.refSet = varset;
		refcart.refSys = curr_coord;
		MWM.set_jog_cart(refcart);
// 		cout << " LOC : " << CStringA(csValue) << curr_coord << '\n';
 		 
		teach_Dialog->set_jog_cartref(varset,csValue);
		 
	}
}

void LocationDialog::OnSelchangeCommboTool(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	int index = tool_.GetCurSel();
	if ( index >= 0 )
	{
 		ScriptVarSetT varset = SVAR_SYSTEM;
		get_tool_data( index,&varset );

		CString csValue;
		tool_.GetLBText(index,csValue);
		CartRef curr_tool;
		std::string varname( Unicode2Ansi( csValue.GetBuffer(0) ).c_str() );
		LVAR->get_value(svTOOL,varname,&curr_tool,varset);
 
		RefSysSetlInfo reftool;
		reftool.refSet = varset;
		reftool.refSys = curr_tool;
		MWM.set_jog_tool(reftool);
// 		cout << " LOC : " << CStringA(csValue) << curr_tool << '\n';
 	 
		teach_Dialog->set_jog_toolref(varset,csValue);
		 
	}
}

void LocationDialog::get_cartref_data( int nIndex, ScriptVarSetT* varset  )
{
	if ( nIndex < 0 || nIndex > coordinate_.GetCount() )
		return;

  	DWORD dwData = coordinate_.GetItemData(nIndex);
 	*varset   = ScriptVarSetT(HIBYTE(dwData));
 
}

void LocationDialog::get_tool_data( int nIndex, ScriptVarSetT* varset  )
{
	if ( nIndex < 0 || nIndex > tool_.GetCount() )
		return;

 	DWORD dwData = tool_.GetItemData(nIndex);
	*varset  = ScriptVarSetT(HIBYTE(dwData));
}

BOOL LocationDialog::create_tree_grid() throw (InvalidConfigurationException)
{
 	BOOL ret = m_Grid.Create(
		CRect(0, 0, m_iWidthGrid, m_iHeightGrid), 
		this,
		LOCATION_DIALOG_GRID, WS_VISIBLE | WS_CHILD,
		NULL, NULL, NULL, SKIN_WINDOW_VARIABLE, 0 );
 	if ( ret )
	{
 		m_Grid.SetColumnResize(FALSE);
		m_Grid.SetRowResize(FALSE);
 
		m_Grid.EnableDragAndDrop(FALSE);
		m_Grid.SetBkColor(RGB(248, 252, 248));
		m_Grid.GetDefaultCell(FALSE, FALSE)->SetBackClr(RGB(248, 252, 248)); // 0XE0

		LOGFONT lf;
		memset(&lf, 0, sizeof(LOGFONT));
		lf.lfHeight = 20;
		lf.lfWeight = 700;
		wcsncpy_s(lf.lfFaceName, _T("Arial"), LF_FACESIZE-1);
		CFont font;
		font.CreateFontIndirect(&lf);
 		m_Grid.SendMessage(WM_SETFONT,(WPARAM)(font.GetSafeHandle()));
		font.DeleteObject();

		m_Grid.SetEditable(FALSE); // read only set
		m_Grid.SetListMode(TRUE);
		m_Grid.EnableSelection(FALSE);
		m_Grid.SetFrameFocusCell(FALSE);

		m_Grid.SetGridLines(GVL_BOTH);
		m_Grid.SetGridLineColor(RGB(60,64,60));

		m_Grid.SetFixedRowSelection(TRUE);
		m_Grid.SetSingleRowSelection(TRUE);
		m_Grid.SetFixedColumnSelection(FALSE);

		m_Grid.SetShowSrollBar(SB_HORZ,FALSE); // liurundan.

		initialize_tree_grid();
		m_Grid.GetCell(0, 0)->SetDrawSortArrow(FALSE); 
		m_Grid.GetCell(0, 1)->SetDrawSortArrow(FALSE); 
	}
	return ret;
}

BOOL LocationDialog::initialize_tree_grid()
{
	UpdateData();

	m_Grid.SetAutoSizeStyle();

	int m_nFixCols = 0;
	int m_nFixRows = 1;
	m_Grid.SetFixedRowCount(m_nFixRows);
	m_Grid.SetFixedColumnCount(m_nFixCols);

	m_Grid.SetDefCellHeight(30);
 
	m_Grid.SetRedraw(FALSE);
 
	m_Grid.InsertColumn(_T(""), DT_CENTER|DT_TOP);
	m_Grid.InsertColumn(_T("名称"), DT_CENTER|DT_TOP);
	m_Grid.InsertColumn(_T("数值"), DT_CENTER|DT_TOP);
	m_Grid.InsertColumn(_T("单位"), DT_CENTER|DT_TOP);
	m_Grid.InsertColumn(_T("备注"), DT_CENTER|DT_TOP);
 
	m_Grid.SetColumnWidth(0, 0);
	m_Grid.SetColumnWidth(1, 100);
	m_Grid.SetColumnWidth(2, 150);
	m_Grid.SetColumnWidth(3, 80);
	m_Grid.SetColumnWidth(4, 130);

	m_Grid.ExpandColumnsToFit(); // liurundan

	m_Grid.SetRedraw(TRUE);
  
	GV_ITEM Item;
	Item.mask = GVIF_TEXT | GVIF_FORMAT | GVIF_STATE;
	Item.col = 1;
	Item.nFormat = DT_LEFT | DT_BOTTOM;
	Item.nState  = GVIS_READONLY;
 	GTV_HTREENODE hItem;
 	CString strlabel;
	for ( int i = 0; i < grid_row_count; ++i )
	{
		strlabel.Format(_T("A%d"),i+1);
 		Item.strText = strlabel;
 		hItem        = m_Grid.InsertItem(&Item);
		m_Grid.ShowTreeNodeButton( hItem, FALSE );
		m_Grid.SetItemFormat(i+1,2,DT_RIGHT); 
		 
  	}
	// Write extra codes for setting up the treelistctrl for sorting
	m_Grid.SetHeaderSort(FALSE);
 	// Set the default compare function for all rows
	m_Grid.SetCompareFunction(CGridCtrl::pfnCellTextCompare);
  
 	update_pos_grid_text();
 	UpdateData(FALSE);

	return TRUE;
}

// 表格上显示数据
void LocationDialog::update_pos_grid_text() 
{
	CString strlab;
 
	const GroupProperties& gp = MWM.get_group_properties();

 	for( int i = 1; i <= grid_row_count; ++i )  
	{
		if ( show_coordinate_type == SHOW_SERVO || show_coordinate_type == SHOW_JOINT )
		{
			if ( show_coordinate_type == SHOW_SERVO )
			{
			    strlab.Format(_T("D%d"),i);
				m_Grid.SetItemText(i, 4, CString(gp.servoName[i-1]) == _T("Dummy") ? _T("仿真"): CString(gp.servoName[i-1]) );
			}
			else
			{
				strlab.Format(_T("A%d"),i);
				m_Grid.SetItemText(i, 4,  _T("") );
			}
			m_Grid.SetItemText(i, 1, strlab );
			m_Grid.SetItemText(i, 3, _T(" 度") );
		}
		else
		{
			m_Grid.SetItemText(i, 1, cart_label[i-1][0] );
			m_Grid.SetItemText(i, 3, cart_label[i-1][1] );
			m_Grid.SetItemText(i, 4,  show_coordinate_type == SHOW_WORLD ? _T("世界坐标"): _T("用户坐标") );
			if ( i > 6 )
			{
				m_Grid.SetItemText(i, 4, _T("") );
			}
		} 
 	} 

	m_Grid.Invalidate();
}

void LocationDialog::update_pos_grid_data(double *data)
{
 	CString str;
	for( int i = 0; i < grid_row_count; i++ ) 
	{
		str.Format(_T("%.2lf"), data[i]);
		m_Grid.SetItemText(i+1, 2, str);
	}
	m_Grid.Invalidate(TRUE);

}


int LocationDialog::OnActivateChildWin(LPARAM lp)
{
	//UpdateWindow();
	return 1;
}

void LocationDialog::OnPowerButtonClicked(UINT id)
{
	CmdInfo cmd;
	cmd.iCommandType  = CMD_POWER;

 	if ( id == IDC_POWER_OFF_BTN )
	{
	    printf("power off\n");
		m_BtnServoPower[POWER_OFF].EnableWindow(FALSE);
		m_BtnServoPower[POWER_ON].EnableWindow(TRUE);
		if ( ProgramEditor::is_running_flag )
		{
			MSD.ShowInfo(_T("请先停止程序运行！"),2);
			return;
		}
		if ( MWM.get_robot_state().servo_on == true )
			cmd.bOnOff = false; 
		else
			return;
 	}
	else
	{
		printf("power on\n");
		m_BtnServoPower[POWER_OFF].EnableWindow(TRUE);
		m_BtnServoPower[POWER_ON].EnableWindow(FALSE);

		if ( MWM.get_robot_state().servo_on == false )
 			cmd.bOnOff = true;  
 		else
			return;
	}

	if ( SMC.isCodeSysMotion )
	{
		;
	}
	else
	{
		MWM.set_servo_on(cmd.bOnOff);
		MWM.set_enable(cmd.bOnOff);
	}
	SMC.setCmdInfo( cmd );

}


BOOL LocationDialog::OnEraseBkgnd(CDC* pDC)
{
	BOOL ret = DialogType::OnEraseBkgnd(pDC);
	CRect rect;
	GetClientRect(&rect);
	pDC->FillSolidRect(&rect, kColor[8]);

	for ( int i = 0; i < ARRAY_SIZE(m_BtnServoPower); ++i )
	{
		m_BtnServoPower[i].SetBk(pDC);
	}
 

	return ret;
}
