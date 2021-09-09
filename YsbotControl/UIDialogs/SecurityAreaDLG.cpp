
//////////唐彧彧彧彧
#include "stdafx.h"
#include "SecurityAreaDLG.h"
#include "../colourcode.h"
#include "../WorldModel.h"
#include "../MotionControl.h"
#include "../LuaScript/ScriptVariables.h"
#include "../stringconvert.h"
#include "state_dialog.h"

using namespace std;

SecurityAreaDLG *SecurityArea_DLG;

BEGIN_MESSAGE_MAP(SecurityAreaDLG, DialogType)
	ON_MESSAGE(WM_SELAREASHAPE, &OnSelchangeshape)
	ON_MESSAGE(WM_SELAREATYPE,&OnSelchangetype)
	ON_MESSAGE(WM_SelChangeAreaName, &OnSelchangeAreaName )	

END_MESSAGE_MAP()

ScanAreaList::ScanAreaList(int n)
{
 	for (int i=0; i < n; i++) 
	{
		areas.push_back(new SecurArea());
		keys.push_back(new VariableKey() );
	}
}

ScanAreaList::~ScanAreaList()
{
	clear();
}

void ScanAreaList::clear()
{
	for (unsigned int i=0; i < areas.size(); i++) 
	{
		delete areas[i];
		delete keys[i];
	}
	areas.clear();
	keys.clear();
}

void ScanAreaList::active( VariableKey vkey, SecurArea area )
{
	std::vector<SecurArea*>::iterator itStartArea = areas.begin();
	int nKey = findAreaID(vkey);
	if ( nKey != -1 )
	{
		if ( (*(itStartArea+nKey))->active )
		{
			cout << "don't active again! \n";
			return;
		}
	}

	int nListID = findDeactiveAreaID();
	if ( nListID != -1 ) // 覆盖原数据
	{
 		*(*(itStartArea+nListID)) = area;

 		std::vector<VariableKey*>::iterator itStartKey = keys.begin();
		*(*(itStartKey+nListID)) = vkey;
 	}
	else  
	{
 		MSD.ShowInfo(_T("区域队列已满！"),JTMESSAGE);
 	}
}

void ScanAreaList::deactive( VariableKey vkey )
{
	std::vector<SecurArea*>::iterator itStartArea = areas.begin();
	int nListID = findAreaID(vkey);
	if ( nListID != -1 ) //
	{
		(*(itStartArea+nListID))->active = false;
		std::vector<VariableKey*>::iterator itStartKey = keys.begin();
		(*(itStartKey+nListID))->varName[0] = '\0';

	}
	else  
	{
		 cout << " no exit this area!" << endl;
	}
}

int ScanAreaList::findAreaID(const VariableKey& vkey)
{
 	std::vector<VariableKey*>::iterator itStartKey = keys.begin();
	std::vector<VariableKey*>::iterator itEndKey = keys.end();
	int nIndex = -1;
	while( itStartKey < itEndKey )
	{ 
		if ( vkey.varSet == (*itStartKey)->varSet && strcmp( vkey.varName, (*itStartKey)->varName ) == 0 )
		{
			nIndex = itStartKey - keys.begin();
 			break;
		}
		itStartKey++;
	}

	return nIndex;
}

int ScanAreaList::findDeactiveAreaID()
{
	std::vector<SecurArea*>::iterator itStartArea = areas.begin();
	std::vector<SecurArea*>::iterator itEndArea = areas.end();
	int nIndex = -1;
	while( itStartArea < itEndArea )
	{ 
		if ( (*(itStartArea))->active == false )
		{
			nIndex = itStartArea - areas.begin();
 			break;
		}
		itStartArea++;
	}

	return nIndex;
}

//////////////////////////////////////////////////////////////////////////

SecurityAreaDLG::SecurityAreaDLG()
{
	m_shape = 0;
	m_type  = 0;

	areaScanList = new ScanAreaList(MAX_AREA_NUM);

	m_bEnableScanArea = true;

	m_din.device_id = MCS_REMOTEIO;
	m_dout.device_id = MCS_REMOTEIO;
}

SecurityAreaDLG:: ~SecurityAreaDLG()
{
	delete areaScanList;
}

void SecurityAreaDLG::DoDataExchange(CDataExchange* pDX) 
{
	
}

LRESULT SecurityAreaDLG::OnMainButtonClicked( WPARAM wp, LPARAM lp)
{
	int btnid  = (int)(wp);
	int menuid = (int)(lp);

	switch(btnid)
	{
	case 0:
 		break;
	case 1:
		break;
	case 2:
		break;
	case 3: // set save edit result.
 		{
			save_area();
		}
		break;
	case 4: // active
 		{
			if ( AreaInformation.Area_name.GetCurSel() != -1 )
			{
				if ( m_area.active == false )
				{
					m_area.active = true;
					AreaInformation.update_areainfo_show(m_area);
					save_area();

					areaScanList->active( m_currAreaKey, m_area );
				}
				else
				{
					MSD.ShowInfo(_T("已经启用区域检测！"),JTMESSAGE);
				}
 			}
		}
		break;
	case 5: // deactive
 		{
			if ( AreaInformation.Area_name.GetCurSel() != -1 )
			{
				if ( m_area.active )
				{
					m_area.active = false;
					areaScanList->deactive( m_currAreaKey );
					AreaInformation.update_areainfo_show(m_area);

					save_area();
				}
				else
				{
					MSD.ShowInfo(_T("已经禁用区域检测！"),JTMESSAGE);
				}		
			}
 
		}
		break;

	default:
		break;
	}

    MSD.SetSpaceActiveNum( get_acitive_area_num() );

	return 1;
}

BOOL SecurityAreaDLG::OnInitDialog() 
{
	CDialogEx::OnInitDialog();

	if ( GetParent()->GetDlgItem(IDC_BOX_MAIN)->GetSafeHwnd() )
	{
		GetParent()->GetDlgItem(IDC_BOX_MAIN)->GetClientRect(&clientRect);
		SetWindowPos(NULL,clientRect.left, clientRect.top, clientRect.Width(), clientRect.Height(), SWP_NOZORDER);
	}

// 	SetWindowPos(NULL,g_x0 + 22, 95, 724, 439, SWP_NOZORDER);
	SetBackgroundColor(kColor[8], TRUE);
	//初始化控件
	CRect rectBox[4];
	int iBoxInfoHeight = (int)( clientRect.Height()*1/5);
	int iBoxGeneralHeight = (int)( clientRect.Height()*2/5);
	int iBoxDimensionHeight = clientRect.Height() - (iBoxInfoHeight + iBoxGeneralHeight);
	int iBoxDimeWidth = (int)( clientRect.Width()/3 );
	rectBox[0] = CRect(CPoint(clientRect.left,clientRect.top),CSize(clientRect.Width(),iBoxInfoHeight));
	rectBox[1] = CRect(CPoint(clientRect.left,rectBox[0].bottom),CSize(clientRect.Width(),iBoxGeneralHeight));
	rectBox[2] = CRect(CPoint(clientRect.left,rectBox[1].bottom),CSize(clientRect.Width()-iBoxDimeWidth,iBoxDimensionHeight));
	rectBox[3] = CRect(CPoint(clientRect.left+rectBox[2].Width(),rectBox[1].bottom),CSize(iBoxDimeWidth,iBoxDimensionHeight));
	unsigned int iFontSize = 20;

	AreaInformation.Create(_T("区域信息"),WS_CHILD|WS_VISIBLE|SS_NOTIFY,rectBox[0],this,IDC_SecurArea_Information);
	AreaInformation.SetFont(iFontSize,_T("宋体"));
 	AreaGeneral.Create(_T("常规"),WS_CHILD|WS_VISIBLE|SS_NOTIFY,rectBox[1],this,IDC_SecurArea_General);
	AreaGeneral.SetFont(iFontSize,_T("宋体"));
	AreaDimension.Create(_T("起点与尺寸"),WS_CHILD|WS_VISIBLE|SS_NOTIFY,rectBox[2],this,IDC_SecurArea_Dimension);
	AreaDimension.SetFont(iFontSize,_T("宋体"));
	AreaIllustration.Create(_T("图示"),WS_CHILD|WS_VISIBLE|SS_NOTIFY,rectBox[3],this,IDC_SecurArea_Illustration);
	AreaIllustration.SetFont(iFontSize,_T("宋体"));

	InitCombobox();

	return TRUE;
}

LRESULT SecurityAreaDLG::OnSelchangeshape(WPARAM wp, LPARAM lp)
{
	m_shape = (int)(wp);
// 	cout<<"shape = "<<m_shape<<"   type = "<<m_type<<endl;
	AreaIllustration.update_picture(m_shape,m_type);
	AreaDimension.update_text(m_shape);

	return 0;
}

LRESULT SecurityAreaDLG::OnSelchangetype(WPARAM wp,LPARAM lp)
{
	m_type = (int)(wp);
// 	cout<<"shape = "<<m_shape<<"    type = "<<m_type<<endl;
	AreaIllustration.update_picture(m_shape,m_type);
 
	return 0;
 
}

void SecurityAreaDLG::update_area_dlg( int selID )
{
	DWORD dwData = AreaInformation.Area_name.GetItemData(selID);
	ScriptVarSetT varSet =  ScriptVarSetT(HIBYTE(dwData));

	CString curRefName = _T("");
	AreaInformation.Area_name.GetLBText(selID,curRefName);
	std::string varname = Unicode2Ansi( curRefName.GetBuffer(0) );
	LVAR->get_value( svSecurArea, varname, &m_area, varSet );
	
	m_currAreaKey.varSet = varSet;
	sprintf( m_currAreaKey.varName, "%s", varname.c_str() );
 	m_currAreaKey.varTyp = svSecurArea;
	m_currAreaKey.valid = true;

	AreaInformation.update_areainfo_show(m_area);
	AreaGeneral.update_general_show(m_area);
	AreaDimension.update_dimension_show(m_area);
}

LRESULT SecurityAreaDLG::OnSelchangeAreaName(WPARAM wp, LPARAM lp)
{
	int index = (int)(wp);

	update_area_dlg( index );

	return 0;
}

void SecurityAreaDLG::InitCombobox()
{
	//Ysbot::Time ttt;
	AreaInformation.Area_name.ResetContent();
//     InitAreaActiveAndStateVar()
	std::string varname = "";
	VariableKey vkeyTmp;
	SecurArea areaTmp;
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
				if( it->first == svSecurArea )
				{
					if ( svarset != SVAR_SYSTEM )
					{
						AreaInformation.Area_name.AddString(CString(varname.c_str()),dwData);

						//////////////////////////////////////////////////////////////////////////
						vkeyTmp.varSet = svarset;
						sprintf( vkeyTmp.varName, "%s", varname.c_str() );
						vkeyTmp.varTyp = svSecurArea;
						vkeyTmp.valid = true;  
						LVAR->get_value( svSecurArea, varname, &areaTmp, svarset );
						if ( areaTmp.active )
						{
							areaScanList->active(vkeyTmp,areaTmp);
 						}
					}

 				}
			}
		}

		svarset = ScriptVarSetT((int)svarset + 1);
	}

	if ( AreaInformation.Area_name.GetCount() > 0 )
	{
		AreaInformation.Area_name.SetCurSel(0);
		update_area_dlg(0);

		MSD.SetSpaceActiveNum( get_acitive_area_num() );

	}
}

void SecurityAreaDLG::InitAreaActiveAndStateVar()
{
	//AreaGeneral.Area_cmb[3].ResetContent();

	;
}

int SecurityAreaDLG::OnActivateChildWin( LPARAM lp )
{
	InitCombobox(); 
	UpdateWindow();
	 
	return 1;
}

void SecurityAreaDLG::save_area()
{
	SecurArea area;
	AreaInformation.get_area_information(&area);
	AreaGeneral.get_general_setting(&area);
	AreaDimension.get_dimension(&area);
	m_area = area;
	LVAR->set_value( svSecurArea, m_currAreaKey.varName, &area, m_currAreaKey.varSet );
}

unsigned int SecurityAreaDLG::get_acitive_area_num()
{
	unsigned int nCnt = 0;
	for (unsigned int i = 0; i < areaScanList->areas.size(); ++i )
	{
		if (areaScanList->areas[i]->active )
			nCnt++;
	}
	return nCnt;
}

void SecurityAreaDLG::scan_security_area()
{
	if ( !m_bEnableScanArea )
 		return;
    
	// 检查启动变量的状态，以决定启用区域变量。

	std::vector<SecurArea*>::iterator itStartArea = areaScanList->areas.begin();
	std::vector<SecurArea*>::iterator itEndArea = areaScanList->areas.end();
	std::vector<VariableKey*>::iterator itKey = areaScanList->keys.begin();

    int areaType = 0;
	CartPos rpos = SMC.get_position( PosInfo::CTmachine, LDScommad ).cart;

	bool bNeedUpdateStateBmp = false;
	bool bAlarm = false;
	bool bInSide = false;
	unsigned int nCount = 0;
	CString msgOut =_T("");

	// 遍历区域变量
	while( itStartArea < itEndArea )
	{ 
   		if ( (*itStartArea)->active )
		{
			bNeedUpdateStateBmp = false;
 			if ( m_currAreaKey.varSet == (*itKey)->varSet && strcmp( m_currAreaKey.varName, (*itKey)->varName ) == 0 )
			{
				bNeedUpdateStateBmp = true;
			}

			if ((*itStartArea)->iActiveVar == 0 ) // manual active
			{
				bInSide =  (*itStartArea)->IsPosInArea(&rpos);
			}
			else // IO variable active
			{
				int nInfoActiveVar = (*itStartArea)->iActiveVar; 
				m_din.port = nInfoActiveVar;
				int ret = m_din.read();
				if ( ret == 1 )
				{
					bInSide =  (*itStartArea)->IsPosInArea(&rpos);
 				}
			}

			areaType  = (*itStartArea)->iAreaType;

			if ( areaType == 0 ) // block
			{
				if ( bInSide )
				{
					printf("Entering blocked [ %s ], must do something!\n", (*itKey)->varName );
					msgOut.Format(_T("进入禁止区域 [ %s ]，注意安全运行！"), Ansi2Unicode((*itKey)->varName).c_str() );
					 
					MSD.ShowInfo(msgOut,JTERROR,3030,true);

					bAlarm = true;
 				}
 			}
			else // work
			{
				if ( bInSide == false )
				{
					printf("Out work [%s], must do something!\n", (*itKey)->varName );
					bAlarm = true;
					msgOut.Format(_T("超出工作区域 [ %s ]，需减速运行！"), Ansi2Unicode((*itKey)->varName).c_str() );
					MSD.ShowInfo(msgOut,JTERROR,3031,true);
   				}
 			}

			if ( bNeedUpdateStateBmp )
			{
				AreaInformation.update_area_state(bInSide);
			}
		}

		//////////////////////////////////////////////////////////////////////////
		int nInfoStateVar = (*itStartArea)->iStateVar; 
		if ( nInfoStateVar != 0 ) // use remote IO show state
		{
			m_dout.port = nInfoStateVar;
			m_dout.set( areaType == 0 ? bInSide : !bInSide );
		}
		else // use global bool variable show state, may be use variable in memory.
		{
			LuaBool lbool;
			lbool.m_value =  areaType == 0 ? bInSide : !bInSide;
			LVAR->set_value( svLUABOOL, "b1", &lbool, SVAR_GLOBAL );
		}	

		nCount++;
		itStartArea++;
		itKey++;
	}
 	 
	MSD.SetSpaceWatch( bAlarm ? 2:1 );
	 
}

