// Copyright 2016, 亚思信息科技有限责任公司
// All right reserved.
//
// Author: Liu Run Dan ( lrdcz@163.com )
//
// Yasi Robot Motion Control System - 亚思机器人运动控制系统

#include "stdafx.h"
//#include "resource.h"
#include "UserCoSysDLG.h"
#include "../colourcode.h"
#include "../WorldModel.h"
#include "../MotionControl.h"
#include "../LuaScript/ScriptVariables.h"
#include "../stringconvert.h"

UserCoSysDLG *userCoSys_DLG;

const CString UserCoSysDLG::strSetupBtn[6]   = { _T(""),     _T(""), _T(""), _T(""), _T(""),         _T("设置")     };
const CString UserCoSysDLG::strProcessBtn[6] = { _T("取消"), _T(""), _T(""), _T(""), _T("< 上一步"), _T("下一步 >") };
const CString UserCoSysDLG::strConfirmBtn[6] = { _T("取消"), _T(""), _T(""), _T(""), _T("< 上一步"), _T("确认")     };

 
BEGIN_MESSAGE_MAP(UserCoSysDLG, DialogType)
	ON_MESSAGE(WM_SelChangeUserRef, &OnSelchangeUserRef )	
 	ON_MESSAGE(WM_SelChangeBaseRef, &OnSelchangeBaseRef )
	ON_MESSAGE(WM_SelChangeRefStep, &OnSelchangeRefStep )
	ON_MESSAGE(WM_CHANGE_CALIBRATIONUSER, &OnMethodChange )
	//ON_WM_SETFOCUS()
END_MESSAGE_MAP()

UserCoSysDLG::UserCoSysDLG(int iWinID) : m_iWindowID(iWinID), m_iMethodType(0)
{
 	curRefName = _T("");
	m_iStep    = 0;
}

UserCoSysDLG:: ~UserCoSysDLG()
{

}

void UserCoSysDLG::DoDataExchange(CDataExchange* pDX) 
{

}

void UserCoSysDLG::update_show_ui( int iStep )
{
	bool saveDataStep = false;
	if ( iStep == 0 )
	{
		refCoSysSelect.enable_window(TRUE);
		refCoSysSetting.ShowWindow(SW_SHOW);
		toolCoSysVal.ShowWindow(SW_SHOW);
		refChooseMethod.ShowWindow(SW_HIDE);
		refGuide.ShowWindow(SW_HIDE);
		refPicture.ShowWindow(SW_HIDE);

		GetParent()->SendMessage( WM_UPDATE_BUTTONTEXT, m_iWindowID, (LPARAM)&strSetupBtn[0] );

	}
	else
	{
		refCoSysSelect.enable_window(FALSE);
		refChooseMethod.ShowWindow( iStep == 1 ? SW_SHOW : SW_HIDE );
		toolCoSysVal.ShowWindow(SW_HIDE);
		refPicture.ShowWindow(SW_SHOW);
		GetParent()->SendMessage( WM_UPDATE_BUTTONTEXT, m_iWindowID, (LPARAM)&strProcessBtn[0] );

		if ( m_iMethodType == 2 ) // one point
		{
			refCoSysSetting.ShowWindow( iStep == 3 ? SW_SHOW : SW_HIDE);
 			refGuide.ShowWindow(  ( iStep < 3 && iStep > 1 ) ? SW_SHOW : SW_HIDE );
 
			if ( iStep == 3 )
			{
				GetParent()->SendMessage( WM_UPDATE_BUTTONTEXT, m_iWindowID, (LPARAM)&strConfirmBtn[0] );
				saveDataStep = true;
 			}
		}
		else
		{
 			refCoSysSetting.ShowWindow( iStep == 5 ? SW_SHOW : SW_HIDE);
 			refGuide.ShowWindow(  ( iStep < 5 && iStep > 1 ) ? SW_SHOW : SW_HIDE );

			GetParent()->SendMessage( WM_UPDATE_BUTTONTEXT, m_iWindowID, (LPARAM)&strProcessBtn[0] );

			if ( iStep == 5 )
			{
				GetParent()->SendMessage( WM_UPDATE_BUTTONTEXT, m_iWindowID, (LPARAM)&strConfirmBtn[0] );
				saveDataStep = true;
 			}
		}

 	}

	refPicture.update_picture_region(m_iMethodType,m_iStep);
	refGuide.update_region_show(m_iMethodType,m_iStep);

	if ( saveDataStep )
	{
 		const CartRef& refsys = refGuide.get_refsys();
		SetUserCoSysData(&refsys);
 	}

}

LRESULT UserCoSysDLG::OnMainButtonClicked( WPARAM wp, LPARAM lp)
{
	int btnid  = (int)(wp);
	int menuid = (int)(lp);

	switch(btnid)
	{
	case 0: // cancle
        reset();
		break;
	case 1: break;
	case 2:	break;
	case 3:	break;
	case 4:	// back
		if ( m_iStep > 0 )
		{
			m_iStep -= 1;
 		}
 		break;
	case 5: // next
		{
			if ( m_iMethodType == 2 )
			{
				if ( m_iStep < 3 )
				{
 					m_iStep +=1;
 				}
				else
				{
					m_iStep = 0; // save refuser data.
					cout << ("save data of cartref variable\n");
					save_calc_result();
   				}
			}
			else
			{
				if ( m_iStep < 5 )
				{
 					m_iStep += 1;
				}
				else
				{
					m_iStep = 0;
					cout << ("save data of cartref variable\n");
					save_calc_result();
   				}
			}

		}
		break;
 	default: break;
	}
	cout << "calibration step = " << m_iStep << endl;

	update_show_ui(m_iStep);

	return 1;
}

BOOL UserCoSysDLG::OnInitDialog() 
{
	CDialogEx::OnInitDialog();
	if ( GetParent()->GetDlgItem(IDC_BOX_MAIN)->GetSafeHwnd() )
	{
		GetParent()->GetDlgItem(IDC_BOX_MAIN)->GetClientRect(&clientRect);
		SetWindowPos(NULL,clientRect.left, clientRect.top, clientRect.Width(), clientRect.Height(), SWP_NOZORDER);
	}
 	SetBackgroundColor(kColor[8], TRUE);

	int iBoxVarHeight = (int)( clientRect.Height() / 5.0);
	int iBoxWorkHeight = clientRect.Height() - iBoxVarHeight;
	int iHalfWidth = (int) ( clientRect.Width() / 2.0 );

	CRect rcVar = CRect( CPoint( clientRect.left,clientRect.top), CSize( clientRect.Width(),iBoxVarHeight));
	CRect rcWorkLeft  = CRect( CPoint( clientRect.left,rcVar.bottom), CSize(iHalfWidth,iBoxWorkHeight));
	CRect rcWorkRight = rcWorkLeft;
	rcWorkRight.OffsetRect(iHalfWidth,0);
	int nFontSize = 20;
	//初始化控件
	refCoSysSelect.Create(_T("选择变量"),WS_CHILD|WS_VISIBLE|SS_NOTIFY,rcVar,this,IDC_UserCoSys_RefSelect);
	refCoSysSelect.SetFont(nFontSize,_T("宋体"));
	refCoSysSetting.Create(_T("用户坐标值"),WS_CHILD|WS_VISIBLE|SS_NOTIFY,rcWorkLeft,this,IDC_UserCoSys_RefSetting);
	refCoSysSetting.SetFont(nFontSize,_T("宋体"));
	refChooseMethod.Create(_T("第1步: 选择标定方法"),WS_CHILD|SS_NOTIFY,rcWorkLeft,this,IDC_UserCoSys_ChooseMethod);
	refChooseMethod.SetFont(nFontSize,_T("宋体"));
	toolCoSysVal.Create(_T("工具TCP值"),WS_CHILD|WS_VISIBLE|SS_NOTIFY,rcWorkRight,this,IDC_UserCoSys_ToolVal);
	toolCoSysVal.SetFont(nFontSize,_T("宋体"));
	toolCoSysVal.SetReadOnly(TRUE);
	refGuide.Create(_T("设置向导"),WS_CHILD|WS_VISIBLE,rcWorkLeft,this,IDC_UserCoSys_SettingGuid);
	refGuide.SetFont(nFontSize,_T("宋体"));
	refGuide.ShowWindow(SW_HIDE);
	refPicture.Create(_T("图示"),WS_CHILD|WS_VISIBLE,rcWorkRight,this,IDC_UserCoSys_PosPicture);
	refPicture.SetFont(nFontSize,_T("宋体"));
	refPicture.ShowWindow(SW_HIDE);
  
 	//InitCombobox();

	return TRUE;
}
 

void UserCoSysDLG::update_refData( int selID )
{
	DWORD dwData = refCoSysSelect.ref_.GetItemData(selID);
	ScriptVarSetT varSet =  ScriptVarSetT(HIBYTE(dwData));
	refCoSysSelect.ref_.GetLBText(selID,curRefName);
	std::string varname = Unicode2Ansi( curRefName.GetBuffer(0) );
	CartRef ref_temp;
	LVAR->get_value( svCARTREF, varname, &ref_temp, varSet );
	SetUserCoSysData(&ref_temp);

	m_currRef.varSet = varSet;
 	sprintf( m_currRef.varName, "%s", varname.c_str() );

	m_currRef.varTyp = svCARTREF;
	m_currRef.valid = true;

	update_toolData(&ref_temp);
}

void UserCoSysDLG::update_toolData(const CartRef* refsys)
{
 	PosInfo rpos  = SMC.get_position( PosInfo::CTmachine,LDScommad );
 	CartRef curr_jog_tool = MWM.get_jog_tool().refSys;

	double dfpos[12] = {0};
	CartPos tcp;
	MWM.calc_tcp_pose( &rpos.cart, refsys, &curr_jog_tool.cpos, &tcp, AM_EULER ); 
	CartRef rtool;
	rtool.cpos = tcp;
	SetToolCoSysData(&rtool);
}

LRESULT UserCoSysDLG::OnSelchangeUserRef(WPARAM wp, LPARAM lp)
{
 	int index = (int)(wp);

 	update_refData(index);
 
	return 0;
}

LRESULT UserCoSysDLG::OnSelchangeBaseRef(WPARAM wp, LPARAM lp)
{
	return 0;
}

LRESULT UserCoSysDLG::OnSelchangeRefStep(WPARAM Wparam,LPARAM Lparam)
{
	int nID=(int)Lparam;
	//refPicture.SelectPicture(nID);
	return 0;
}

int UserCoSysDLG::SelectRefName(CString RefName,BOOL bRefresh)
{
	int ret=refCoSysSelect.SelectRefName(RefName);
	if (ret>=0 && bRefresh)
	{
		curRefName=RefName;
		std::string varname=Unicode2Ansi((std::wstring)curRefName);
		ScriptVarT varT=svCARTREF;
		CartRef ref_temp;
		LVAR->get_value( varT,varname,(void*)&ref_temp);
		SetUserCoSysData(&ref_temp);
	}
	return ret;
}

void UserCoSysDLG::InitCombobox()
{
	//Ysbot::Time ttt;
	refCoSysSelect.ResetContent();
	refCoSysSetting.ResetContent();
	///cout << ttt.elapsed_msec() << "----> ";
 
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
			    if( it->first == svCARTREF )
				{
					if (svarset != SVAR_SYSTEM )
					{
 						refCoSysSelect.AddString(CString(varname.c_str()),dwData);
					}

					refCoSysSetting.AddString(CString(varname.c_str()),dwData); // dialog setting
				}
			}
		}

		svarset = ScriptVarSetT((int)svarset + 1);
	}

	if ( refCoSysSelect.ref_.GetCount() > 0 )
	{
		refCoSysSelect.ref_.SetCurSel(0);
		update_refData(0);
	}

	refCoSysSetting.base_ref.SetCurSel(0);

}

// void UserCoSysDLG::update_UserCoSys_window()
// {
// 	if ( ::IsWindow( GetSafeHwnd() ) && IsWindowVisible())
// 	{
// 		curr_cart_ref = MWM.get_jog_cart().refSys;
// 		curr_tool     = MWM.get_jog_tool().refSys;
// 		SetUserCoSysData(&curr_cart_ref);
// 		SetToolCoSysData(&curr_tool);
// 	}
// }

void UserCoSysDLG::SetUserCoSysData(const CartRef* cref )
{
	double dfpos[12] = {0};
	CartPos apos=cref->cpos;
	dfpos[0] = apos.pos.x;			dfpos[1] = apos.pos.y;			dfpos[2] = apos.pos.z;
	dfpos[3] = rad2deg(apos.ang.x); dfpos[4] = rad2deg(apos.ang.y); dfpos[5] = rad2deg(apos.ang.z);
	refCoSysSetting.SetData(dfpos);
}

void UserCoSysDLG::GetUserCoSysData(CartRef& cref )
{
	double dfpos[12] = {0};
	refCoSysSetting.GetData(dfpos);
	CartPos apos=cref.cpos;
	apos.pos.x = dfpos[0];			apos.pos.y = dfpos[1];		   apos.pos.z = dfpos[2];
	apos.ang.x = deg2rad(dfpos[3]); apos.ang.y =deg2rad(dfpos[4]); apos.ang.z = deg2rad(dfpos[5]);
	cref.cpos=apos;
}

void UserCoSysDLG::SetToolCoSysData(const CartRef* cref )
{
	double dfpos[12] = {0};
	CartPos apos=cref->cpos;
	dfpos[0] = apos.pos.x;			dfpos[1] = apos.pos.y;			dfpos[2] = apos.pos.z;
	dfpos[3] = rad2deg(apos.ang.x); dfpos[4] = rad2deg(apos.ang.y); dfpos[5] = rad2deg(apos.ang.z);
	toolCoSysVal.SetData(dfpos);
}

void UserCoSysDLG::GetToolCoSysData(CartRef& cref )
{
	double dfpos[12] = {0};
	toolCoSysVal.GetData(dfpos);
	CartPos apos=cref.cpos;
	apos.pos.x = dfpos[0];			apos.pos.y = dfpos[1];		   apos.pos.z = dfpos[2];
	apos.ang.x = deg2rad(dfpos[3]); apos.ang.y =deg2rad(dfpos[4]); apos.ang.z = deg2rad(dfpos[5]);
	cref.cpos=apos;
}

void UserCoSysDLG::SaveCurRefData()
{
	CString Rname=curRefName;
	if (Rname!=_T(""))
	{
		//curRefName=Rname;
		std::string varname=Unicode2Ansi((std::wstring)curRefName);
		ScriptVarT varT=svCARTREF;
		CartRef ref_temp;
		GetUserCoSysData(ref_temp);
		LVAR->set_value( varT,varname,(void*)&ref_temp);
	}
}
 
LRESULT UserCoSysDLG::OnMethodChange(WPARAM wp, LPARAM lp)
{
    m_iMethodType = (int)wp;
	 
	refPicture.update_picture_region(m_iMethodType,m_iStep);
	refGuide.reset_all();
	refCoSysSetting.clear_show();

	return 0;
}

void UserCoSysDLG::reset()
{
	m_iStep = 0;
	m_iMethodType = 0;
	refChooseMethod.reset();
	refGuide.reset_all();

	refCoSysSetting.clear_show();
}

void UserCoSysDLG::save_calc_result()
{
	CartRef ref_temp;
	GetUserCoSysData(ref_temp);
	LVAR->set_value( svCARTREF, m_currRef.varName, &ref_temp, m_currRef.varSet );

}

int UserCoSysDLG::OnActivateChildWin( LPARAM lp )
{
	InitCombobox(); 
	UpdateWindow();
	SetFocus();
// 	refCoSysSetting.Invalidate();
// 	Invalidate();
	return 1;
}