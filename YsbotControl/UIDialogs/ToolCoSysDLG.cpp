// Copyright 2016, 亚思信息科技有限责任公司
// All right reserved.
//
// Author: Liu Run Dan ( lrdcz@163.com )
//
// Yasi Robot Motion Control System - 亚思机器人运动控制系统

#include "stdafx.h"
#include "ToolCoSysDLG.h"
#include "../colourcode.h"
#include "../resource.h"
#include "../WorldModel.h"
#include "../WorldModel.h"
#include "../MotionControl.h"
#include "../LuaScript/ScriptVariables.h"
#include "../stringconvert.h" 

ToolCoSysDLG *toolCoSys_DLG;

const CString ToolCoSysDLG::strToolSetupBtn[6]   = { _T(""),     _T(""), _T(""), _T(""), _T(""),         _T("设置")     };
const CString ToolCoSysDLG::strToolProcessBtn[6] = { _T("取消"), _T(""), _T(""), _T(""), _T("< 上一步"), _T("下一步 >") };
const CString ToolCoSysDLG::strToolConfirmBtn[6] = { _T("取消"), _T(""), _T(""), _T(""), _T("< 上一步"), _T("确认")     };
 
BEGIN_MESSAGE_MAP(ToolCoSysDLG, DialogType)
	//ON_WM_SETFOCUS()
	ON_MESSAGE(WM_SelChangeUserRef, &OnSelchangeTool )	
	ON_MESSAGE(WM_CHANGE_TOOLMETHOD, &OnMethodChange )

END_MESSAGE_MAP()

ToolCoSysDLG::ToolCoSysDLG(int iWinID) : m_iWindowID(iWinID), toolSelect(true)
{
 	 
	m_iMethodType = 0;
	m_iStep       = 0;

}

ToolCoSysDLG:: ~ToolCoSysDLG()
{

}

void ToolCoSysDLG::DoDataExchange(CDataExchange* pDX) 
{

}
 
BOOL ToolCoSysDLG::OnInitDialog() 
{
	CDialogEx::OnInitDialog();
	if ( GetParent()->GetDlgItem(IDC_BOX_MAIN)->GetSafeHwnd() )
	{
		GetParent()->GetDlgItem(IDC_BOX_MAIN)->GetClientRect(&clientRect);
		SetWindowPos(NULL,clientRect.left, clientRect.top, clientRect.Width(), clientRect.Height(), SWP_NOZORDER);
	}
	int iBoxVarHeight = (int)( clientRect.Height() / 5.0);
	int iBoxWorkHeight = clientRect.Height() - iBoxVarHeight;
	int iHalfWidth = (int) ( clientRect.Width() / 2.0 );

	CRect rcVar = CRect( CPoint( clientRect.left,clientRect.top), CSize( clientRect.Width(),iBoxVarHeight));
	CRect rcWorkLeft  = CRect( CPoint( clientRect.left,rcVar.bottom), CSize(iHalfWidth,iBoxWorkHeight));
	CRect rcWorkRight = rcWorkLeft;
	rcWorkRight.OffsetRect(iHalfWidth,0);
	int nFontSize = 20;
	SetBackgroundColor(kColor[8], TRUE);

	//初始化控件
	toolSelect.Create(_T("变量选择"),WS_CHILD|WS_VISIBLE|SS_NOTIFY,rcVar,this,IDC_UserCoSys_RefSelect);
	toolSelect.SetFont(20,_T("宋体"));
	toolSetting.Create(_T("工具坐标设置"),WS_CHILD|WS_VISIBLE|SS_NOTIFY,rcWorkLeft,this,IDC_ToolCoSys_RefSetting);
	toolSetting.SetFont(20,_T("宋体"));
	toolMethod.Create(_T("第1步: 选择标定方法"),WS_CHILD|SS_NOTIFY,rcWorkLeft,this,IDC_ToolCoSys_ChooseMethod);
	toolMethod.SetFont(20,_T("宋体"));
  	toolGuide.Create(_T("工具设置向导"),WS_CHILD,rcWorkLeft,this,IDC_ToolCoSys_SettingGuid);
	toolGuide.SetFont(20,_T("宋体"));
	toolPicture.Create(_T("图示"),WS_CHILD|WS_VISIBLE,rcWorkRight,this,IDC_TOOL_PIC_ILLUSTRATION );
	toolPicture.SetFont(20,_T("宋体"));
  
	return TRUE;
}


LRESULT ToolCoSysDLG::OnMainButtonClicked( WPARAM wp, LPARAM lp)
{
	int btnid  = (int)(wp);
	int menuid = (int)(lp);

	switch(btnid)
	{
	case 0: reset();	break;
	case 1:	break;
	case 2:	break;
	case 3:	break;
	case 4:
		if ( m_iStep > 0 )
		{
			m_iStep -= 1;
		}
		break;
	case 5:  //设置
		{
			if ( m_iMethodType == 0 )
			{
				if ( m_iStep < 4 )
				{
					m_iStep +=1;
				}
				else
				{
					m_iStep = 0; // save refuser data.
					save_calc_result();
 					cout << ("save data of cartref variable\n");
 				}
			}
			else if( m_iMethodType == 1 )
			{
				if ( m_iStep < 6 )
				{
					m_iStep += 1;
				}
				else
				{
					m_iStep = 0;
					save_calc_result();
 					cout << ("save data of cartref variable\n");
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
					save_calc_result();
 					cout << ("save data of cartref variable\n");
				}
			}

		}
		break;

	default:
		break;
	}
 
	update_show_ui( m_iStep );

	return 1;
}

void ToolCoSysDLG::update_show_ui( int iStep )
{
 	printf(" method = %d step = %d \n",m_iMethodType,m_iStep);

	bool saveDataStep = false;
	if ( iStep == 0 )
	{
		toolSelect.enable_window(TRUE);
		toolSetting.ShowWindow(SW_SHOW);
 
		toolMethod.ShowWindow(SW_HIDE);
		toolGuide.ShowWindow(SW_HIDE);

  		GetParent()->SendMessage( WM_UPDATE_BUTTONTEXT, m_iWindowID, (LPARAM)&strToolSetupBtn[0] );

		reset();
		int selToolName = toolSelect.ref_.GetCurSel();
		update_tool_data( selToolName );
 	}
	else
	{
		toolSelect.enable_window(FALSE);

		toolMethod.ShowWindow( iStep == 1 ? SW_SHOW : SW_HIDE );
  
 		GetParent()->SendMessage( WM_UPDATE_BUTTONTEXT, m_iWindowID, (LPARAM)&strToolProcessBtn[0] );

		if ( m_iMethodType == 0 )
		{
			toolGuide.ShowWindow(  ( iStep < 4 && iStep > 1 ) ? SW_SHOW : SW_HIDE );
			toolSetting.ShowWindow( ( iStep == 4 ) ? SW_SHOW : SW_HIDE);

			if ( iStep == 4 )
			{
				GetParent()->SendMessage( WM_UPDATE_BUTTONTEXT, m_iWindowID, (LPARAM)&strToolConfirmBtn[0] );
				saveDataStep = true;
 			}
		}
		else if ( m_iMethodType == 1 )
		{
			toolGuide.ShowWindow(  ( iStep < 6 && iStep > 1 ) ? SW_SHOW : SW_HIDE );
			toolSetting.ShowWindow( ( iStep == 6 ) ? SW_SHOW : SW_HIDE);

			if ( iStep == 6 )
			{
				GetParent()->SendMessage( WM_UPDATE_BUTTONTEXT, m_iWindowID, (LPARAM)&strToolConfirmBtn[0] );
				saveDataStep = true;
			}
		}
 		else
		{
			toolGuide.ShowWindow(  ( iStep < 5 && iStep > 1 ) ? SW_SHOW : SW_HIDE );
			toolSetting.ShowWindow( ( iStep == 5 ) ? SW_SHOW : SW_HIDE);

			if ( iStep == 5 )
			{
				GetParent()->SendMessage( WM_UPDATE_BUTTONTEXT, m_iWindowID, (LPARAM)&strToolConfirmBtn[0] );
				saveDataStep = true;
			}
		}
	}

	toolPicture.update_illustration( m_iMethodType+1, m_iStep );
	toolGuide.update_guide( m_iMethodType, m_iStep );

// 	if ( toolGuide.IsWindowVisible() )
// 	{
// 		toolGuide.RedrawWindow();
// 	}

	if ( saveDataStep )
	{
		const CartRef& reftool = toolGuide.get_refsys();
		SetToolCoSysData(&reftool);
	}
}
 
int ToolCoSysDLG::OnActivateChildWin( LPARAM lp )
{
	InitCombobox(); 
	UpdateWindow();
	return 1;
}

void ToolCoSysDLG::OnSetFocus(CWnd* pOldWnd)
{
// 	if (controlsReady)
// 	{
// 		InitCombobox();
// 		SelectToolName(curToolName,FALSE);
// 		//cout <<"get focus ; " ;
// 	}
}

LRESULT ToolCoSysDLG::OnSelchangeTool(WPARAM Wparam,LPARAM Lparam)
{
	int index = (int)(Wparam);

	update_tool_data(index);

	return 0;
}

void ToolCoSysDLG::update_tool_data( int selID )
{
	DWORD dwData = toolSelect.ref_.GetItemData(selID);
	ScriptVarSetT varSet =  ScriptVarSetT(HIBYTE(dwData));
	toolSelect.ref_.GetLBText(selID,curRefName);
	std::string varname = Unicode2Ansi( curRefName.GetBuffer(0) );
	CartRef ref_temp;
	LVAR->get_value( svTOOL, varname, &ref_temp, varSet );
	SetToolCoSysData(&ref_temp);

	m_currRef.varSet = varSet;
	sprintf( m_currRef.varName, "%s", varname.c_str() );

	m_currRef.varTyp = svTOOL;
	m_currRef.valid = true;

}

int ToolCoSysDLG::SelectToolName(CString RefName,BOOL bRefresh)
{
// 	int ret=toolSelect.SelectRefName(RefName);
//  
// 	if (ret>=0 && bRefresh)
// 	{
// 		curToolName=RefName;
// 		std::string varname=Unicode2Ansi((std::wstring)curToolName);
// 		ScriptVarT varT=svTOOL;
// 		CartRef ref_temp;
// 		LVAR->get_value( varT,varname,(void*)&ref_temp);
// 		SetToolCoSysData(&ref_temp);
// 	}
// 	return ret;
	return 0;
}

void ToolCoSysDLG::InitCombobox()
{
	toolSelect.ResetContent();

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
				if( it->first == svTOOL )
				{
					if (svarset != SVAR_SYSTEM )
					{
						toolSelect.AddString(CString(varname.c_str()),dwData);
					}
  				}
			}
		}

		svarset = ScriptVarSetT((int)svarset + 1);
	}

	if ( toolSelect.ref_.GetCount() > 0 )
	{
		toolSelect.ref_.SetCurSel(0);
		update_tool_data(0);
	}

}
  
void ToolCoSysDLG::SetToolCoSysData( const CartRef* cref )
{
	double dfpos[12] = {0};
	CartPos apos=cref->cpos;
	dfpos[0] = apos.pos.x;			dfpos[1] = apos.pos.y;			dfpos[2] = apos.pos.z;
	dfpos[3] = rad2deg(apos.ang.x); dfpos[4] = rad2deg(apos.ang.y); dfpos[5] = rad2deg(apos.ang.z);
	toolSetting.SetData(dfpos);
}

void ToolCoSysDLG::GetToolCoSysData(CartRef& cref )
{
	double dfpos[12] = {0};
	toolSetting.GetData(dfpos);
	CartPos apos=cref.cpos;
	apos.pos.x = dfpos[0];			apos.pos.y = dfpos[1];		   apos.pos.z = dfpos[2];
	apos.ang.x = deg2rad(dfpos[3]); apos.ang.y =deg2rad(dfpos[4]); apos.ang.z = deg2rad(dfpos[5]);
	cref.cpos=apos;
}

void ToolCoSysDLG::SaveCurToolData()
{
	CString Tname=curToolName;
	if (Tname!=_T(""))
	{
		//curToolName=Tname;
		std::string varname=Unicode2Ansi((std::wstring)curToolName);
		ScriptVarT varT=svTOOL;
		CartRef ref_temp;
		GetToolCoSysData(ref_temp);
		LVAR->set_value( varT,varname,(void*)&ref_temp);
		/*LVAR.write_var_at()*/
	}
}


void ToolCoSysDLG::reset()
{
	m_iStep = 0;
	m_iMethodType = 0;
	toolGuide.reset_all();
	toolMethod.reset();

	toolSetting.clear_show();

}

LRESULT ToolCoSysDLG::OnMethodChange(WPARAM wp, LPARAM lp)
{
	m_iMethodType = (int)wp;

	toolPicture.update_illustration( m_iMethodType+1, m_iStep );
	toolGuide.reset_all();
	toolSetting.clear_show();

	return 0;
}

void ToolCoSysDLG::save_calc_result()
{
	CartRef ref_temp;
	GetToolCoSysData( ref_temp );
	LVAR->set_value( svTOOL, m_currRef.varName, &ref_temp, m_currRef.varSet );

}
