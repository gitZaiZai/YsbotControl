#include "stdafx.h"
#include "../TeachDialog/TeachDlg.h"
#include "../colourcode.h"
#include "../WorldModel.h"
#include "../MotionControl.h"
#include "../LuaScript/ScriptVariables.h"
#include "../stringconvert.h"
#include "../UIDialogs/InfoBox.h"
#include "../UIDialogs/state_dialog.h"

TeachDialog *teach_Dialog = NULL;

////TeachDialog class
BEGIN_MESSAGE_MAP(TeachDialog, DialogType)
	ON_MESSAGE(WM_SelChangeVariable, &TeachDialog::OnSelchangeVariable )	
	ON_MESSAGE(WM_TEACHDLG_MOVETYPE,&TeachDialog::OnChangeMoveType)

END_MESSAGE_MAP()

TeachDialog::TeachDialog()
{
	curr_teach_cmd.dyn.JointVel = 20;
	curr_teach_cmd.dyn.JointAcc = 30;
	curr_teach_cmd.dyn.JointDec = 30;
	curr_teach_cmd.dyn.JointJerk = 100.0;

	curr_teach_cmd.dyn.PathVel = 100;
	curr_teach_cmd.dyn.PathAcc = 750;
	curr_teach_cmd.dyn.PathDec = 750;
	curr_teach_cmd.dyn.PathJerk = 1e6;

	curr_teach_cmd.ovl.iBufferMode = Overlap::mcBuffered;
	curr_teach_cmd.ovl.iTransMode = Overlap::TMNone;
}

TeachDialog:: ~TeachDialog()
{
 
}

void TeachDialog::DoDataExchange(CDataExchange* pDX) 
{

}

LRESULT TeachDialog::OnMainButtonClicked( WPARAM wp, LPARAM lp)
{
	int btnid  = (int)(wp);
	int menuid = (int)(lp);

 	PosInfo next_pos;
  
	switch(btnid)
	{
	case 0:	break;
	case 1:	teach_pos_var(); break;// 示教按钮
	case 2:	show_window_type( help_frame.IsWindowVisible() ? -1 : 2 ); break;	//帮助按钮
 	case 3:	break;
	case 4:	//允许按钮
		{
			if( !MWM.is_main_world_model_available() || MWM.get_robot_state().in_teachmoving )
				break;
			if ( MWM.get_robot_state().operate != omTEACH )
			{
				MSD.ShowInfo(_T("请切换到示教模式！"),2);
				break;
			}

			show_window_type(-1);

			SetReadOnly(TRUE);
			SMC.reset_teach_info();
			MWM.set_teach_moving(TRUE);

			curr_cart_ref = MWM.get_jog_cart().refSys; // liurundan maybe use prog ref.
			curr_tool     = MWM.get_jog_tool().refSys;

 			GetPositionData( next_pos );

			curr_teach_cmd.iCommandType = ( GetMoveType() == 0 ? CMD_PTP: CMD_LIN  );
			curr_teach_cmd.destPos = next_pos;
			curr_teach_cmd.cartref = &curr_cart_ref;
			curr_teach_cmd.tool    = &curr_tool.cpos;
			curr_teach_cmd.dyn.custom_parameter = false;
 
			MWM.set_teach_command(curr_teach_cmd);
 		}

		break;
	case 5:	//禁止按钮
		show_window_type(-1);
		MWM.set_teach_moving(FALSE);
		SetReadOnly(FALSE);
 		break;

	default:
		break;
	}
	printf("Teach Dialog id = %d, menu_id = %d\n", btnid, menuid );
	return 1;
}

BOOL TeachDialog::OnInitDialog() 
{
	CDialogEx::OnInitDialog();
	if ( GetParent()->GetDlgItem(IDC_BOX_MAIN)->GetSafeHwnd() )
	{
		GetParent()->GetDlgItem(IDC_BOX_MAIN)->GetClientRect(&clientRect);
		SetWindowPos(NULL,clientRect.left, clientRect.top, clientRect.Width(), clientRect.Height(), SWP_NOZORDER);
	}
	//SetWindowPos(NULL,g_x0 + 22, 95, 724, 439, SWP_NOZORDER);
	SetBackgroundColor(kColor[8], TRUE);

	CRect rectBox[5];
	int iBoxVariableHeight = (int)( clientRect.Height()*1/5);
	int iBoxLocationHeight = (int)( clientRect.Height()*4/5);
	int iBoxContinueHeight = (int)( iBoxLocationHeight *2/5);
	int iBoxJogCoordHeight = iBoxContinueHeight;
	int iBoxJogTypeHeight  = iBoxLocationHeight - (iBoxContinueHeight + iBoxJogCoordHeight);
	int iBoxLocationWidth = (int)( clientRect.Width()/2.0 );
	rectBox[0] = CRect(CPoint(clientRect.left,clientRect.top),CSize(clientRect.Width(),iBoxVariableHeight));
	rectBox[1] = CRect(CPoint(clientRect.left,rectBox[0].bottom),CSize(iBoxLocationWidth,iBoxLocationHeight));
	rectBox[2] = CRect(CPoint(clientRect.left+rectBox[1].Width(),rectBox[0].bottom),CSize(iBoxLocationWidth,iBoxContinueHeight));
	rectBox[3] = rectBox[2];
	rectBox[3].OffsetRect(0,rectBox[2].Height());
	rectBox[4] = CRect(CPoint(clientRect.left+rectBox[1].Width(),rectBox[3].bottom),CSize(iBoxLocationWidth,iBoxJogTypeHeight));
	//初始化控件
	unsigned int iFontSize = 20;
	variable_Select.Create(_T("位置"),WS_CHILD|WS_VISIBLE|SS_NOTIFY,rectBox[0],this,IDC_TEACHDLG_VARIABLE);	
	variable_Select.SetFont(iFontSize,_T("宋体"));
	jointData_frame.Create(_T("位置数据"),WS_CHILD|SS_NOTIFY,rectBox[1],this,IDC_TEACHDLG_JOINT);
	jointData_frame.SetFont(iFontSize,_T("宋体"));
	poseData_frame.Create(_T("位置数据"),WS_CHILD|SS_NOTIFY,rectBox[1],this,IDC_TEACHDLG_POSE);
	poseData_frame.SetFont(iFontSize,_T("宋体"));
	help_frame.Create(_T("帮助"),WS_CHILD|SS_NOTIFY,rectBox[1],this);
	help_frame.SetFont(iFontSize,_T("宋体"));
 
	teacCoSys_frame.Create(_T("连续运行信息"),WS_CHILD|WS_VISIBLE|SS_NOTIFY,rectBox[2],this,IDC_TEACHDLG_TEACHINFO);
	teacCoSys_frame.SetFont(iFontSize,_T("宋体"));
	posCoSys_frame.Create(_T("点动控制信息"),WS_CHILD|WS_VISIBLE|SS_NOTIFY,rectBox[3],this,IDC_TEACHDLG_POSINFO);
	posCoSys_frame.SetFont(iFontSize,_T("宋体"));
	moveType_frame.Create(_T("点动类型"),WS_CHILD|WS_VISIBLE|SS_NOTIFY,rectBox[4],this,IDC_TEACHDLG_MOVETYPE);
	moveType_frame.SetFont(iFontSize,_T("宋体"));

// 	variable_Select.Create(_T("位置"),WS_CHILD|WS_VISIBLE|SS_NOTIFY,CRect(5,8,725,83),this,IDC_TEACHDLG_VARIABLE);	
// 	jointData_frame.Create(_T("位置数据"),WS_CHILD|SS_NOTIFY,CRect(5,90,360,435),this,IDC_TEACHDLG_JOINT);
// 	poseData_frame.Create(_T("位置数据"),WS_CHILD|SS_NOTIFY,CRect(5,90,360,435),this,IDC_TEACHDLG_POSE);
// 	help_frame.Create(_T("帮助"),WS_CHILD|SS_NOTIFY,CRect(5,90,360,435),this);
//   
// 	teacCoSys_frame.Create(_T("连续运行信息"),WS_CHILD|WS_VISIBLE|SS_NOTIFY,CRect(365,90,725,210),this,IDC_TEACHDLG_TEACHINFO);
// 	posCoSys_frame.Create(_T("点动控制信息"),WS_CHILD|WS_VISIBLE|SS_NOTIFY,CRect(365,220,725,340),this,IDC_TEACHDLG_POSINFO);
// 	moveType_frame.Create(_T("点动类型"),WS_CHILD|WS_VISIBLE|SS_NOTIFY,CRect(365,350,725,435),this,IDC_TEACHDLG_MOVETYPE);
 	 
// 	SetReadOnly(TRUE);

	InitVariableCombo();
    
	set_variable();
    
	curr_teach_cmd.iCommandType = CMD_LIN ;
    std::string varname = "";
	ScriptVarSetT varset = SVAR_SYSTEM;
	if ( MWM.is_main_world_model_available() )
	{
 		RefSysSetlInfo jogCartInfo = MWM.get_jog_cart();
		varset = jogCartInfo.refSet;
  		varname = jogCartInfo.refSys.getSelfName();
 		set_jog_cartref( varset, CString(Ansi2Unicode(varname).c_str()) );

		RefSysSetlInfo jogToolInfo = MWM.get_jog_tool();
		varset = jogToolInfo.refSet;
		varname = jogToolInfo.refSys.getSelfName();
  		set_jog_toolref( varset, CString(Ansi2Unicode(varname).c_str()) );
 
		// Sys_ , Glo_
  
		CartRef curr_coord;
		LVAR->get_value( svCARTREF, SYSTEM_CARTREF_NAME, &curr_coord, SVAR_SYSTEM );
		RefSysSetlInfo coordInfo;
		coordInfo.refSet = SVAR_SYSTEM;
		coordInfo.refSys = curr_coord;
		MWM.set_prog_cart(coordInfo);

		CartRef curr_tool;
		LVAR->get_value( svTOOL, SYSTEM_TOOL_NAME, &curr_tool, SVAR_SYSTEM );
		RefSysSetlInfo toolInfo;
		toolInfo.refSet = SVAR_SYSTEM;
		toolInfo.refSys = curr_tool;
		MWM.set_prog_tool(toolInfo);
	}
	return TRUE;
}

LRESULT TeachDialog::OnSelchangeVariable(WPARAM Wparam,LPARAM Lparam)
{
 	set_variable();
	return 0;
}
 
// void TeachDialog::update_Teach_window()
// {
// 	if ( ::IsWindow( GetSafeHwnd() ) && IsWindowVisible() )
// 	{
// 		PosInfo newpi;
// 		curr_cart_ref = MWM.get_jog_cart().refSys;
// 		curr_tool     = MWM.get_jog_tool().refSys;
// 		ScriptVarSetT vSet;
// 		ScriptVarT    varT ;
// 		variable_Select.get_curr_set_type(&vSet,&varT);
// 
// 		switch( varT )
// 		{
// 		case svAXISPOS:	newpi = SMC.get_position( PosInfo::CTaxis,   LDScommad ); break;
// 		case svCARTPOS: newpi = SMC.get_position( PosInfo::CTmachine,LDScommad ); break;
//  		default:break;
// 		}		
// 		SetPositionData( newpi );
// 	}
// }

void TeachDialog::SetPositionData(const PosInfo& cp )
{
 	if ( cp.pos_type == PosInfo::CTaxis )
	{
 		set_axis_pos(cp);
	}
	else if ( cp.pos_type == PosInfo::CTmachine )
	{
 		set_cart_pos(cp);
	}
}

bool TeachDialog::GetPositionData( PosInfo& cp )
{
  	bool ret = true;
	if ( jointData_frame.IsWindowVisible() )
	{
		double dfpos[12] = {0};
		AxisPos apos;
		AxisPos auxipos;
		jointData_frame.GetData(dfpos);
		apos.q1 = deg2rad(dfpos[0]); apos.q2 =  deg2rad(dfpos[1]); apos.q3 =  deg2rad(dfpos[2]);
		apos.q4 = deg2rad(dfpos[3]); apos.q5 =  deg2rad(dfpos[4]); apos.q6 =  deg2rad(dfpos[5]);
		auxipos.q1=deg2rad(dfpos[6]); auxipos.q2=deg2rad(dfpos[7]);
		cp.set_axis(apos,auxipos);
		cp.cart.fig=5;
	    cp.pos_type = PosInfo::CTaxis;
	} 
	else if( poseData_frame.IsWindowVisible() )
	{
		double dfpos[12] = {0};
		CartPos tcp;
		AxisPos auxipos;
		poseData_frame.GetData(dfpos);
		tcp.pos.x = dfpos[0];          tcp.pos.y = dfpos[1];          tcp.pos.z = dfpos[2];
		tcp.ang.x = deg2rad(dfpos[3]); tcp.ang.y = deg2rad(dfpos[4]); tcp.ang.z = deg2rad(dfpos[5]);	
		auxipos.q1= deg2rad(dfpos[6]);	auxipos.q2 =deg2rad(dfpos[7]); 
		tcp.fig=poseData_frame.GetMode();
		cp.set_cart(tcp,auxipos);
	    cp.pos_type = PosInfo::CTmachine;
	}
	else
		ret = false;
	return ret;
}
 
void TeachDialog::set_variable()
{
	int nIndex = variable_Select.GetCurSel();
	if ( nIndex == CB_ERR )
 		return;

	CString varName = variable_Select.GetSeLVariableName();
	std::string strVarName = Unicode2Ansi((std::wstring)varName);
	ScriptVarSetT vset;
	ScriptVarT    varT;
	variable_Select.get_curr_set_type( &vset, &varT );
	PosInfo pos_temp;
	LVAR->get_value( varT,strVarName,(void*)&pos_temp, vset);

	SetPositionData(pos_temp);
    
	show_window_type( varT == svAXISPOS ? 0 : 1 );
}

int TeachDialog::OnActivateChildWin(LPARAM lp)
{
	if ( lp )
	{
		active_from_variable_dlg((VariableKey*)(lp));
 	}
	UpdateWindow();
	return 1;
}

void TeachDialog::active_from_variable_dlg(VariableKey* pVarKey)
{
	if ( pVarKey == NULL )
 		return;
 
 	std::string varName = pVarKey->varName;
	ScriptVarSetT vSet = pVarKey->varSet;
	ScriptVarT vTyp = pVarKey->varTyp;
	SelectVariableName(CString(varName.c_str()),vSet,vTyp);

	UpdateWindow(); // ??????????????????????????
}

// variable dialog update this dlg display use this function.
int TeachDialog::SelectVariableName( CString varName, ScriptVarSetT vSet, ScriptVarT vTyp,  BOOL bRefresh/* =TRUE */)
{
	if ( MWM.get_robot_state().in_teachmoving )
		return 0;

  	int nIndex = CB_ERR;
	ScriptVarSetT _varset = SVAR_PROGRAM;
	ScriptVarT   _vartype = svAXISPOS;
	do 
	{
		nIndex = variable_Select.FindStringExact( nIndex, varName );
		variable_Select.get_set_type(nIndex,&_varset,&_vartype);
 		if ( _varset == vSet && _vartype == vTyp && nIndex != CB_ERR )
		{
			variable_Select.select_variable_index( nIndex );
			set_variable();
			break;
		}

	} while ( nIndex != CB_ERR );

	return 1;

}
 
void TeachDialog::SetReadOnly(BOOL bReadOnly)
{
	if (jointData_frame.IsWindowVisible())
		jointData_frame.SetReadOnly(bReadOnly);
	if( poseData_frame.IsWindowVisible() )
		poseData_frame.SetReadOnly(bReadOnly);

	variable_Select.EnableWindow(!bReadOnly);
	moveType_frame.Enable(!bReadOnly);

}
  
void TeachDialog::InitVariableCombo()
{
 	variable_Select.ResetContent();
  
  	std::string varname = "";
	 
	DWORD dwData = 0;
	const std::map<ScriptVarT, std::map<std::string, void*>>* var_list = NULL;
 	ScriptVarSetT svarset = SVAR_SYSTEM;
	ScriptVarT svt = svAXISPOS;
	while ( svarset != SVAR_SET_NUM )
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
				svt = it->first;
				varname = innerIt->first;
				if ( svt == svAXISPOS || svt == svCARTPOS )
				{
					dwData = MAKEWORD( svt, svarset );
					variable_Select.AddString( CString(varname.c_str()), dwData );
				}
 			}
		}

		svarset = ScriptVarSetT((int)svarset + 1);
 	}
  
	variable_Select.select_variable_index(0);
}

void TeachDialog::set_axis_pos( const PosInfo& ap )
{
	double dfpos[12] = {0};
	AxisPos apos = ap.axis.get_deg();
	dfpos[0] = apos.q1; dfpos[1] = apos.q2; dfpos[2] = apos.q3;
	dfpos[3] = apos.q4; dfpos[4] = apos.q5; dfpos[5] = apos.q6;
	dfpos[6] = rad2deg(ap.auxi.q1); dfpos[7] = rad2deg(ap.auxi.q2); 
	jointData_frame.SetData(dfpos);
}

void TeachDialog::set_cart_pos( const PosInfo& cp )
{
	double dfpos[12] = {0};
	CartPos tcp = cp.cart;
 
	dfpos[0] = tcp.pos.x;           dfpos[1] = tcp.pos.y;          dfpos[2] = tcp.pos.z;
	dfpos[3] = rad2deg(tcp.ang.x);  dfpos[4] = rad2deg(tcp.ang.y); dfpos[5] = rad2deg(tcp.ang.z);
	dfpos[6] = rad2deg(cp.auxi.q1); dfpos[7] = rad2deg(cp.auxi.q2); 
	poseData_frame.SetData(dfpos);
	poseData_frame.SetMode(cp.cart.fig);
}

LRESULT TeachDialog::OnChangeMoveType(WPARAM Wparam,LPARAM Lparam)
{
	curr_teach_cmd.iCommandType = ( (unsigned int)(Lparam) == 0 ? CMD_PTP : CMD_LIN  );
  	return 0;
}
 
void TeachDialog::set_prog_cartref( ScriptVarSetT vt, CString strname ) 
{
	if ( ::IsWindow( teacCoSys_frame.GetSafeHwnd() ) )
	{
		teacCoSys_frame.set_cartref(vt,strname); 
 	}
}

void TeachDialog::set_prog_toolref( ScriptVarSetT vt, CString strname )
{
	if ( teacCoSys_frame.GetSafeHwnd() )
	{
		teacCoSys_frame.set_toolref(vt,strname);
 	}
}

void TeachDialog::set_jog_cartref( ScriptVarSetT vt, CString strname )
{
	if ( posCoSys_frame.GetSafeHwnd() )
	{
		posCoSys_frame.set_cartref(vt,strname);
 	}
}

void TeachDialog::set_jog_toolref( ScriptVarSetT vt, CString strname )
{
	if ( posCoSys_frame.GetSafeHwnd() )
	{
		posCoSys_frame.set_toolref(vt,strname);
 	}
}

void TeachDialog::teach_pos_var()
{
	if ( ::IsWindow( GetSafeHwnd() ) && IsWindowVisible() )
	{
 		ScriptVarSetT vSet;
		ScriptVarT    varT ;
		variable_Select.get_curr_set_type(&vSet,&varT);
	    show_window_type(varT == svAXISPOS ? 0 : 1);
 
 		CString varName = variable_Select.GetSeLVariableName();
		std::string strVarName = Unicode2Ansi((std::wstring)varName);
 
		PosInfo pos_temp;
		bool ret = GetPositionData( pos_temp );
		if ( !ret )
 			return;

 		if ( ( pos_temp.pos_type == PosInfo::CTaxis    && varT == svAXISPOS  ) ||
			 ( pos_temp.pos_type == PosInfo::CTmachine && varT == svCARTPOS  ) )
		{
			LVAR->set_value( varT, strVarName, (void*)&pos_temp, vSet );
 		}
	}
}

void TeachDialog::show_window_type( int nType )
{
	if ( nType == 2 )
	{
		jointData_frame.ShowWindow(SW_HIDE);
		poseData_frame.ShowWindow(SW_HIDE);
		help_frame.ShowWindow(SW_SHOW);
	}
	else if( nType == 0 )
	{
		help_frame.ShowWindow(SW_HIDE);
		poseData_frame.ShowWindow(SW_HIDE);
 		jointData_frame.ShowWindow(SW_SHOW);
	}
	else if ( nType == 1 )
	{
		help_frame.ShowWindow(SW_HIDE);
		jointData_frame.ShowWindow(SW_HIDE);
		poseData_frame.ShowWindow(SW_SHOW);
 	}
	else if ( nType == -1 )
	{
		ScriptVarSetT vSet;
		ScriptVarT    varT ;
		variable_Select.get_curr_set_type(&vSet,&varT);
		 
	    help_frame.ShowWindow(SW_HIDE);
		 
		if ( varT == svAXISPOS )
		{
			poseData_frame.ShowWindow(SW_HIDE);
			jointData_frame.ShowWindow(SW_SHOW);
		}
		else
		{
			jointData_frame.ShowWindow(SW_HIDE);
			poseData_frame.ShowWindow(SW_SHOW);
		}
	}

}
 