
#include "stdafx.h"
#include "YsbotControl.h"
#include "InstructionEditDLG.h"
#include "colourcode.h"
#include "resource.h"
#include "WorldModel.h"
#include "MotionControl.h"
#include "LuaScript/ScriptVariables.h"
#include "stringconvert.h"
#include "Keyboard/digital_keyboard.h"
#include "Keyboard/full_qwerty.h"
#include "program_dialog.h"
#include "UIDialogs/state_dialog.h"
#include <regex>
#include "VariableBuilderDLG.h"
#include "UIDialogs/InfoBox.h"

#define  STR_SEPARATOR _T("=:")

InstructionEditDLG* instructionEdit_DLG;

InstructionEditDLG::InstructionEditDLG(
	HBITMAP hBmpScrollBar, 
	HBITMAP hBmpPlus, 
	HBITMAP hBmpMinus ) : m_hBmpScrollBar(hBmpScrollBar),
	m_hBmpPlus(hBmpPlus), m_hBmpMinus(hBmpMinus)
{
	hItemCommand = NULL;
	m_bShowAllRegion = true;
	item_para_list.clear();
	isCreatCommand = true;
	last_winID = 0;
	
}

InstructionEditDLG::~InstructionEditDLG()
{
	image_list_.DeleteImageList();
	font_.DeleteObject();

	for ( int i=0; i < svTypeNum; i++ )
		varNameArray[i].RemoveAll();
	item_para_list.clear();
}

void InstructionEditDLG::DoDataExchange(CDataExchange* pDX)
{
}

BEGIN_MESSAGE_MAP(InstructionEditDLG, DialogType)
	ON_NOTIFY(GVN_BEGINLABELEDIT,IDC_InstructionEDIT_Grid,  OnTreeBeginEdit)
	ON_NOTIFY(GVN_ENDLABELEDIT, IDC_InstructionEDIT_Grid,   OnTreeEndEdit)
	ON_NOTIFY( NM_DBLCLK, IDC_InstructionEDIT_Grid, OnClickTree )
END_MESSAGE_MAP()


// InstructionEditDLG message handlers
BOOL InstructionEditDLG::OnInitDialog() 
{
	CDialogEx::OnInitDialog();
	//SetWindowPos(NULL, g_x0 + 20, 95, 730, 439, SWP_NOZORDER);
	if ( GetParent()->GetDlgItem(IDC_BOX_MAIN)->GetSafeHwnd() )
	{
		GetParent()->GetDlgItem(IDC_BOX_MAIN)->GetWindowRect(&clientRect);
		SetWindowPos(NULL,clientRect.left, clientRect.top, clientRect.Width(), clientRect.Height(), SWP_NOZORDER);
	}

	SetBackgroundColor(kColor[8]);
	// 	ModifyStyleEx(WS_EX_STATICEDGE, WS_EX_CLIENTEDGE, SWP_DRAWFRAME);
	// 创建图片列表
	image_list_.Create (22, 22, ILC_COLOR32 | ILC_MASK, 5, 1);
	image_list_.Add(AfxGetApp()->LoadIcon(IDI_VARIABLE_SYSTEM));
	image_list_.Add(AfxGetApp()->LoadIcon(IDI_VARIABLE_GLOBAL));
	image_list_.Add(AfxGetApp()->LoadIcon(IDI_VARIABLE_PROJECT)); 
	int overlay_icon = image_list_.Add(AfxGetApp()->LoadIcon(IDI_OVERLAY_TRANS));
	image_list_.SetOverlayImage(overlay_icon, 1);

	m_iGridHeight = 390;
	create_tree_grid();

	font_.CreateFontW(18, 0, 0, 0, 680, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
		OUT_STROKE_PRECIS, CLIP_STROKE_PRECIS, DRAFT_QUALITY, FIXED_PITCH |
		FF_MODERN, _T("Arial"));

	//CRect editrect( 5, 390, 722, 430 );
	
	CRect editrect( CPoint(5,m_iGridHeight), CSize(clientRect.Width()-10,40));
	instructionEdit_.Create( WS_CHILD | WS_VISIBLE | WS_BORDER, editrect, this, IDC_InstructionEDIT_EDIT, TRUE ); // | WS_TABSTOP
	instructionEdit_.SetFont(&font_);
	instructionEdit_.SetRulePlan(KBRule_Program);
	instructionEdit_.SetHighLightText(TRUE);

	// 	RECT rect ;
	// 	::GetClientRect(instructionEdit_.GetSafeHwnd(),&rect);
	// 	::OffsetRect(&rect,0,5);
	// 	::InflateRect(&rect,-5,-4);
	// 	::SendMessage(instructionEdit_.GetSafeHwnd(),EM_SETRECT,0,(LPARAM)&rect);

	// 	UpdateData(FALSE);
	// 	instructionEdit_.SetFocus();
	// 	instructionEdit_.SetSel(0,-1);

	GetWindowRect(&m_rectAll);

	instructionEdit_.GetWindowRect(&m_rectEdit);

	return TRUE;
}

void InstructionEditDLG::set_show_region( bool bShowAll )
{
	if ( (m_bShowAllRegion^bShowAll) == false )
		return;

	m_bShowAllRegion = bShowAll;
	// 	CRect rtSmall = m_rectEdit;
	// 	rtSmall.SetRectEmpty();
	// 	if (rtSmall.IsRectEmpty())
	// 	{
	// 		rtSmall.left   = m_rectAll.left;
	// 		rtSmall.top    = m_rectAll.top;
	// 		rtSmall.right  = m_rectAll.right+20;
	// 		rtSmall.bottom = m_rectAll.bottom;
	// 	}

	if ( m_bShowAllRegion )
	{
		SetWindowPos(NULL, 0, 0, m_rectAll.Width(), m_rectAll.Height(), SWP_NOMOVE | SWP_NOZORDER);
	}
	else
	{
		SetWindowPos(NULL, m_rectEdit.left, m_rectEdit.top, m_rectEdit.Width(), m_rectEdit.Height(), SWP_NOMOVE | SWP_NOZORDER);
	}
}

LRESULT InstructionEditDLG::OnResizeGrid(WPARAM wp,LPARAM lp)
{
	static UINT lastType = 0;
	static UINT i = 0;
	UINT nType = UINT(wp);
	// re-size all pages
	if ( lastType != nType )
	{
		m_Grid.ExpandColumn(1,250, (nType==0) ); // 0 - fit, 1 = resize colum.
		lastType = nType;
		printf(" N %d dialog on size message %d\n",i++,nType);
	}
	return 1;
}

LRESULT InstructionEditDLG::OnMainButtonClicked( WPARAM wp, LPARAM lp)
{
	int btnid  = (int)(wp);
	int menuid = (int)(lp);

	void* varv=NULL;
	switch( btnid )
	{
	case 0:		//	变量菜单
		switch(menuid)
		{
		case 0:		//	选择变量
			break;
		case 1:		//	新建变量
			{
				m_vkeyMsg = getVarKeyCurr(); // use global variable, otherwise occurs error.
				if ( m_vkeyMsg.valid && m_vkeyMsg.first_members )
				{
					if ( VARINFO->isBaseVariableType(m_vkeyMsg.varTyp) == false )
						GetParent()->SendMessage( WM_ACTIVATE_WINDOW, winNewScriptVariable, (LPARAM)&m_vkeyMsg ); 
				}
			}
			break;
		case 2:		//	键盘
			{
				NM_GRIDVIEW pNotifyStruct;
				int iRet = 0;
				pNotifyStruct.iRow = m_Grid.GetFocusCell().row;
				pNotifyStruct.iColumn = 1;
				OnClickTree( (NMHDR*)&pNotifyStruct,(LRESULT*) &iRet );
			} 
			break;
		case 3:		//	删除：值
			{
				VariableKey vkey = getVarKeyCurr();
				if ( vkey.first_members && vkey.varTyp != svBOOL )
				{
					NM_GRIDVIEW pNotifyStruct;
					pNotifyStruct.iRow = vkey.nRow;
					pNotifyStruct.iColumn = 1;
					int iRet = 0;
 					m_Grid.SetItemText(  vkey.nRow, 1, _T("nil") );

					int dwData = MAKEWORD( vkey.varTyp, SVAR_PROGRAM );
					m_Grid.SetItemData(  vkey.nRow, 1, dwData );
					m_Grid.SetItemImage(  vkey.nRow, 1, SVAR_PROGRAM );

				    OnTreeEndEdit((NMHDR*)&pNotifyStruct,(LRESULT*)&iRet);
 				}

			} 
			break;
		default: break;
		}
		break;
	case 1:		//	示教按钮
		{
			VariableKey vkey = getVarKeyCurr();
			if ( vkey.first_members && ( vkey.varTyp == svAXISPOS || vkey.varTyp == svCARTPOS ) )
			{
				PosInfo pi;
				if( vkey.varTyp == svAXISPOS )
					pi = SMC.get_position( PosInfo::CTaxis,LDScommad );
				if( vkey.varTyp == svCARTPOS)
					pi = SMC.get_position( PosInfo::CTmachine,LDScommad );			
				LVAR->set_value(vkey.varTyp, vkey.varName, (void*)&pi );
				m_Grid.SetRedraw(FALSE);
				update_tree( m_Grid.GetTreeNode( vkey.nRow ), vkey.varTyp, (void*)&pi );
				m_Grid.SetRedraw(TRUE);

				CString strTip = CString( Ansi2Unicode(vkey.varName).c_str() );
				strTip = _T("示教点: ")+strTip;
 				MSD.ShowInfo(strTip,_T("示教"),this,6);
 
			}
		}
		break;
	case 2:		//	添加
		getVarKeyCurr(); 
		break;
	case 3:		//	修改值

		break;
	case 4:		//	取消
		if ( isCreatCommand )
		{
			GetParent()->SendMessage(WM_ACTIVATE_WINDOW,winInstructionDLG); //  winLastWindow
		}
		else
		{
			GetParent()->SendMessage(WM_ACTIVATE_WINDOW,winLastWindow); //  winLastWindow
 		}
		break;
	case 5:		//	确定
		{
			CString strCmd;
			instructionEdit_.GetWindowText(strCmd);
 	 
			program_dialog->modify_command_from_wizards( strCmd, isCreatCommand ? 2 : 0, false, isCreatCommand );
			
			GetParent()->SendMessage(WM_ACTIVATE_WINDOW,winProgram);

		}
		break;
	default: break;
	}

	return 1;
}

BOOL InstructionEditDLG::create_tree_grid() throw (InvalidConfigurationException)
{
	BOOL ret = m_Grid.Create(
		CRect(0, 0, clientRect.Width(), m_iGridHeight), 
		this,
		IDC_InstructionEDIT_Grid, WS_VISIBLE | WS_CHILD,
		m_hBmpScrollBar, m_hBmpPlus, m_hBmpMinus, SKIN_WINDOW_VARIABLE, 35 );

	if ( ret )
	{
		m_Grid.SetImageList(&image_list_);
		m_Grid.SetColumnResize(FALSE);
		m_Grid.SetRowResize(FALSE);

		m_Grid.EnableDragAndDrop(FALSE);
		m_Grid.SetBkColor(RGB(248, 252, 248));
		m_Grid.GetDefaultCell(FALSE, FALSE)->SetBackClr(RGB(248, 252, 248)); // 0XE0

		LOGFONT lf;
		memset(&lf, 0, sizeof(LOGFONT));
		lf.lfHeight = 20;
		lf.lfWeight = 600;
		wcsncpy_s(lf.lfFaceName, _T("Arial"), LF_FACESIZE-1);
		CFont font;
		font.CreateFontIndirect(&lf);
		m_Grid.SendMessage(WM_SETFONT,(WPARAM)(font.GetSafeHandle()));
		font.DeleteObject();

		m_Grid.SetEditable(FALSE); // read only set
		m_Grid.SetListMode(TRUE);

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

BOOL InstructionEditDLG::initialize_tree_grid()
{
	int m_nFixCols = 0;
	int m_nFixRows = 1;
	m_Grid.SetFixedRowCount(m_nFixRows);
	m_Grid.SetFixedColumnCount(m_nFixCols);

	m_Grid.SetDefCellHeight(38);
	m_Grid.SetRedraw(FALSE);

	m_Grid.InsertColumn(_T("名字"),DT_LEFT|DT_TOP);
	m_Grid.InsertColumn(_T("数值"),DT_LEFT|DT_TOP);

	m_Grid.SetColumnWidth(0, 400);
	m_Grid.SetColumnWidth(1, 250);

	m_Grid.ExpandColumnsToFit(); // liurundan

	m_Grid.SetRedraw(TRUE);

	m_Grid.SetTreeColumnCellTypeID(CT_DEFAULT);	

	GV_ITEM Item;
	Item.mask = GVIF_TEXT | GVIF_IMAGE | GVIF_FORMAT | GVIF_STATE;
	Item.nFormat = DT_LEFT | DT_BOTTOM;
	Item.nState  = GVIS_READONLY;

	m_Grid.SetHeaderSort(FALSE);
	m_Grid.SetCompareFunction(CGridCtrl::pfnCellTextCompare);

	return TRUE;
}


void InstructionEditDLG::OnTreeBeginEdit(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*) pNotifyStruct; // click com button
	*pResult = 0;  
	int nRow = pItem->iRow;
	vkey_before_edit = getVarKeyFromRow( nRow );
}

void InstructionEditDLG::OnTreeEndEdit(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*) pNotifyStruct;
	//printf(("dan End Edit on row %d, col %d\n"), pItem->iRow, pItem->iColumn);
	*pResult = 0; // assign -1 here to reject edit changes

	// retrieve text of the edited cell
	int nRow = pItem->iRow;
	int nCol = pItem->iColumn;
	if ( nCol == 1 )
	{
		CGridCellBase *pCell = m_Grid.GetCell(nRow, nCol);
		VariableKey curr_vkey = getVarKeyFromRow( nRow );
		if ( pCell->GetRuntimeClass() == RUNTIME_CLASS(CGridCellCombo) )
		{
			if ( curr_vkey.varTyp != svBOOL )
			{
				void* pvar = LVAR->get_value_ptr( curr_vkey.varTyp, curr_vkey.varName, curr_vkey.varSet ); 
				GTV_HTREENODE hItemCurr = m_Grid.GetTreeNode(nRow);
				if ( pvar )
				{
					if ( ( curr_vkey.varTyp != vkey_before_edit.varTyp ||curr_vkey.varSet != vkey_before_edit.varSet )
						&& hItemCurr )
					{
						while( GTV_HTREENODE hItemChild = m_Grid.GetChildNode(hItemCurr) )
							m_Grid.DeleteItem(hItemChild);

						insert_varprop_item( hItemCurr, curr_vkey.varTyp, curr_vkey.varSet );
					}

					update_tree( m_Grid.GetTreeNode(nRow), curr_vkey.varTyp, pvar );
				}
				else
				{
					CString itemText = m_Grid.GetItemText(nRow,1);
					if ( strcmp( Unicode2Ansi(itemText.GetBuffer(0)).c_str(), STR_LUA_NIL ) == 0 )
					{
						while( GTV_HTREENODE hItemChild = m_Grid.GetChildNode(hItemCurr) )
							m_Grid.DeleteItem(hItemChild);
					}
					//MSD.ShowInfo(_T("varibale not exist!"));
				}
			}
		}
		else if ( pCell->GetRuntimeClass()  == RUNTIME_CLASS(CGridCellCheck) )
		{
			CGridCellCheck *pCheckCell = (CGridCellCheck*) m_Grid.GetCell( nRow, 1);
			BOOL bCheck = pCheckCell->GetCheck();
			VARRW->write_variable( &curr_vkey, bCheck ? "true" : "false");
		}

		update_edit();

	}
}

void InstructionEditDLG::clear_all()
{
	while( m_Grid.GetItemCount() > 1 ) 
		m_Grid.DeleteItem(1);

	item_para_list.clear();
	hItemCommand = NULL;
}

void InstructionEditDLG::insert_script_command( ScriptCommandT sct, bool bNewCreate )
{
	clear_all();

	GV_ITEM Item;
	Item.mask = GVIF_TEXT | GVIF_FORMAT | GVIF_STATE;
	Item.nFormat = DT_LEFT | DT_VCENTER | DT_SINGLELINE;
	Item.nState  = GVIS_READONLY;
	Item.strText = CString(LSCINFO->CommandParameters[sct]->getClassName());
	hItemCommand = m_Grid.InsertItem(&Item);
	m_Grid.ShowTreeNodeButton(hItemCommand,FALSE);
	m_Grid.SetItemData(m_Grid.FindRowIndex(hItemCommand),0, (int)sct );

	// insert command parameters to grid.
	GTV_HTREENODE hItemParam = NULL;
	int numPrameters = LSCINFO->CommandParameters[sct]->getValuesTotal();
	int nRow = -1;
	DWORD dwData = 0; 

	if ( numPrameters > 0 )
	{
		// reset variable name array for display in combobox.
		update_var_name_array();
		CStringArray arrayBoolStr;
		arrayBoolStr.Add(_T("false"));
		arrayBoolStr.Add(_T("true"));

		CString      strName;
		CStringArray strlist;
		CString      varStrInfo;
		ScriptVarSetT varSet = SVAR_SYSTEM;
		CGridCellCombo *pCell = NULL;
		char prop[VAR_STR_LEN];
		int paramKind;
		ScriptVarT svParam;
		for ( int i = 0; i < numPrameters; ++i )
		{
			LSCINFO->CommandParameters[sct]->getValue(i+1,prop);
			Item.strText = CString(prop);
			if ( i == 0 && bNewCreate )
			{
				Item.strText += _T("(新建)");
			}
			hItemParam = m_Grid.InsertItem( &Item ); // insert grid row.
			item_para_list.push_back(hItemParam);
			nRow = m_Grid.FindRowIndex(hItemParam);

			paramKind = static_cast<int>( LSCINFO->CommandParameters[sct]->getValueKind(i+1) );
			svParam   = static_cast<ScriptVarT>( paramKind );

			dwData = MAKEWORD( svParam, SVAR_PROGRAM );
			m_Grid.SetItemData( nRow, 1, dwData );
			m_Grid.SetItemFormat( nRow, 1, DT_RIGHT );

			switch ( svParam )
			{
			case svAXISPOS:
			case svDYNAMIC:
			case svOVERLAP:
			case svCARTREF:
			case svTOOL:
			case svDIGITALINPUT:
			case svDIGITALOUTPUT:
				{
					m_Grid.SetItemState(nRow,1,m_Grid.GetItemState(nRow, 1) & ~GVIS_READONLY);	
					m_Grid.SetItemFormat( nRow, 1, DT_LEFT );
					if (!m_Grid.SetCellType(nRow, 1, RUNTIME_CLASS(CGridCellCombo)))
					{
						cout << "error set grid type\n";
						continue;
					}

					int varNum = varNameArray[svParam].GetSize();
					pCell = (CGridCellCombo*) m_Grid.GetCell(nRow, 1);
					if ( svParam == svAXISPOS )
					{
						pCell->SetTwoTypeOptions( varNameArray[svAXISPOS], varNameArray[svCARTPOS], TRUE, STR_SEPARATOR );
						if ( varNum == 0 )
						{
							varNum = varNameArray[svCARTPOS].GetSize();
							svParam = svCARTPOS;
						}
					}
					else
						pCell->SetOptions( varNameArray[paramKind], TRUE, STR_SEPARATOR );

					pCell->SetStyle(CBS_DROPDOWNLIST);  

					if ( varNum > 0  )
					{
						varStrInfo = varNameArray[svParam].GetAt(0);
						UINT sum = DivStr( varStrInfo, strlist, STR_SEPARATOR );
						if ( sum > 1 )
						{
							strName = strlist[0];
							varSet  = static_cast<ScriptVarSetT>( HIBYTE(_tcstol(strlist[1], NULL, 10)) );
						}

						dwData = MAKEWORD( svParam, varSet );
						m_Grid.SetItemData( nRow, 1, dwData );

						insert_varprop_item( hItemParam, svParam, varSet );  // insert parameters subitems.
						void* pvar = LVAR->get_value_ptr( svParam, Unicode2Ansi( strName.GetBuffer(0)  ), varSet ); 
						if ( pvar )
						{
							update_tree( hItemParam, svParam, pvar );
							m_Grid.SetItemText( nRow, 1, strName );
							m_Grid.SetItemImage( nRow, 1, varSet );
						}
						else
						{
							m_Grid.SetItemText(nRow,1,strName);
						}
						m_Grid.CollapseTreeNode( hItemParam );

					}
					else
					{
						m_Grid.SetItemText(nRow,1,_T("无数值"));
					}
				}
				break;
			case svBOOL:
				{
					// 					m_Grid.SetItemState(nRow,1,m_Grid.GetItemState(nRow, 1) & ~GVIS_READONLY);	
					// 					m_Grid.SetCellType(nRow, 1, RUNTIME_CLASS(CGridCellCheck));
					m_Grid.SetCellType(nRow, 1, RUNTIME_CLASS(CGridCellCombo));
					pCell = (CGridCellCombo*) m_Grid.GetCell(nRow, 1);
					m_Grid.SetItemFormat( nRow, 1, DT_LEFT );
					pCell->SetOptions( arrayBoolStr, TRUE );
					pCell->SetStyle( CBS_DROPDOWNLIST );  
					m_Grid.SetItemText(nRow,1,arrayBoolStr[0]);
				}
				break;
			case svINTEGER: m_Grid.SetItemText(nRow,1,_T("10"));break;
			case svREAL:    m_Grid.SetItemText(nRow,1,_T("10.0"));break;
			case svSTRING:
				if (sct == SM_SYS_PRINT || sct == SM_INFOTIP || sct == SM_LOADSUB )
				{
					m_Grid.SetItemFormat( nRow, 1, DT_LEFT );
					m_Grid.SetItemText(nRow,1,_T("\"string demo!\"")); 
				}
				else
					m_Grid.SetItemText(nRow,1,_T("undefined"));
				break;

			default: break;
			} // end_switch
		} // end_for

		strlist.RemoveAll();
		arrayBoolStr.RemoveAll();
	}
	update_edit();
}

void InstructionEditDLG::update_var_name_array()
{
	//获取变量名并分组
	for ( int i=0; i < svTypeNum; i++ )
		varNameArray[i].RemoveAll();

	std::string varname = "";
	map<ScriptVarT,  map<string, void*>>::const_iterator it;
	map<ScriptVarT,  map<string, void*>>::const_iterator itEnd;
	ScriptVarT sv;
	CString strVarData;
	DWORD dwData = 0; 


	for ( ScriptVarSetT svr = SVAR_SYSTEM; svr != SVAR_SET_NUM; )
	{
		it    = LVAR->get_var_list(svr)->begin();
		itEnd = LVAR->get_var_list(svr)->end();

		while(it != itEnd)
		{
			sv = it->first;
			dwData = MAKEWORD( sv, static_cast<int>(svr) );
			strVarData.Format( _T("%d"), dwData );

			for ( map<string, void*>::const_iterator innerIt = it->second.begin(); 
				innerIt != it->second.end(); 
				innerIt++ ) 
			{
				varname = innerIt->first;
				varNameArray[sv].Add( CString(varname.c_str())+STR_SEPARATOR+strVarData ); // use for GridCombobox display
			}
			it++;
		}

		svr = static_cast<ScriptVarSetT>( (unsigned int)(svr) + 1 );
	}
	dwData = MAKEWORD( svSTRING, SVAR_PROGRAM );
	strVarData.Format( _T("%d"), dwData ); // nil = program set

	varNameArray[svDYNAMIC].Add( CString(STR_LUA_NIL)+STR_SEPARATOR+strVarData ); // use for GridCombobox display
	varNameArray[svOVERLAP].Add( CString(STR_LUA_NIL)+STR_SEPARATOR+strVarData ); // use for GridCombobox display

}

void InstructionEditDLG::insert_varprop_item(GTV_HTREENODE item, ScriptVarT sv, ScriptVarSetT svm )
{
	char prop[VAR_STR_LEN];

	GV_ITEM propItem;
	propItem.mask = GVIF_TEXT|GVIF_FORMAT;
	propItem.nFormat = DT_LEFT | DT_VCENTER;

	GTV_HTREENODE childItem = NULL;
	int nRow = -1;
	int propType = 0;
	DWORD dwData = 0; 

	for ( int i = 0; i < VARINFO->vardesc[sv]->getValuesTotal(); i++ )
	{
		VARINFO->vardesc[sv]->getValue(i+1,prop);
		propType = VARINFO->vardesc[sv]->getValueKind(i+1);
		propItem.strText = CString(prop);

		childItem = m_Grid.InsertItem( &propItem, item );
		nRow = m_Grid.FindRowIndex(childItem);

		dwData = MAKEWORD( static_cast<ScriptVarT>(propType), svm );
		m_Grid.SetItemData( nRow, 1, dwData );	

		m_Grid.SetItemFormat(nRow,1,DT_RIGHT);
		m_Grid.SetItemState(nRow,0,GVIS_READONLY);
		m_Grid.SetItemState(nRow,1,GVIS_READONLY);

		if ( propType == svBOOL ) // 
		{
			if ( svm != SVAR_SYSTEM ) // ??
				m_Grid.SetItemState(nRow,1,m_Grid.GetItemState(nRow, 1) & ~GVIS_READONLY);	

			m_Grid.SetCellType(nRow, 1, RUNTIME_CLASS(CGridCellCheck));
		}
		// 变量属性增加下拉框选项
	}
}

void InstructionEditDLG::update_tree(GTV_HTREENODE varitem, ScriptVarT sv, void* valueptr)
{
	if ( valueptr == NULL )
		return;

	CString strtemp;
	int nRow = -1;
	std::vector<std::string> valuelist;
	VARRW->read_variable(sv,valueptr,valuelist);

	GTV_HTREENODE treeitem = m_Grid.GetChildNode(varitem);
	CGridCellCheck *pCell = NULL;
	for( unsigned int i = 0; i < valuelist.size(); ++i)
	{

		if ( valuelist.size() == 1 )
		{
			if ( strcmp(valuelist[i].c_str(),"true") == 0 || strcmp(valuelist[i].c_str(),"false") == 0 )
			{
				update_check_box( varitem, ( strcmp(valuelist[i].c_str(),"true") == 0 ) ? TRUE : FALSE );
			}
			else
				m_Grid.SetTreeItemText(varitem, 1, CString( valuelist[i].c_str() ) );
		}
		else
		{
			if ( treeitem != NULL )
			{
				if ( strcmp(valuelist[i].c_str(),"true") == 0 || strcmp(valuelist[i].c_str(),"false") == 0 )
				{
					update_check_box( treeitem, ( strcmp(valuelist[i].c_str(),"true") == 0 ) ? TRUE : FALSE );
				}
				else
					m_Grid.SetTreeItemText(treeitem, 1, CString( valuelist[i].c_str() ) );
				treeitem = m_Grid.GetNextSiblingNode(treeitem);
			}
		}

	}

	valuelist.clear();
}

void InstructionEditDLG::update_check_box(GTV_HTREENODE hitem, BOOL b)
{
	if ( hitem == NULL)
		return;

	int nRow = m_Grid.FindRowIndex(hitem);
	CGridCellCheck* pCell = (CGridCellCheck*) m_Grid.GetCell(nRow, 1);
	BOOL bCheck = b;
	if ( pCell->GetCheck() == FALSE && bCheck )
	{
		pCell->SetCheck(TRUE);
	}
	else if ( pCell->GetCheck() == TRUE && !bCheck )
	{
		pCell->SetCheck(FALSE);
	}
}

UINT InstructionEditDLG::DivStr( CString str, CStringArray& Arr,CString strDiv )
{
	Arr.RemoveAll();

	int nFindposi  = 0;       
	CString strTmp = str.Tokenize( strDiv, nFindposi );        
	strTmp.Trim();   
	while( !strTmp.IsEmpty() )     
	{  
		Arr.Add( strTmp );
		strTmp = str.Tokenize(strDiv,nFindposi);
		strTmp.Trim();
	} 
	return Arr.GetSize();
}

void InstructionEditDLG::update_grid_command( ScriptCommandT sct )
{
	int numPrameters = LSCINFO->CommandParameters[sct]->getValuesTotal();
	int nFirstRowParam = 2;
	; 

}

void InstructionEditDLG::update_edit( )
{
	if ( !hItemCommand )
		return;
	int data = m_Grid.GetItemData(m_Grid.FindRowIndex(hItemCommand),0);
	bool isClassFunc = LSCINFO->isClassMemberFunction((ScriptCommandT)(data)); 
	bool isDeclare   = LSCINFO->isVariableDeclaration((ScriptCommandT)(data));
	CString strCmdName = CString(LSCINFO->CommandParameters[(ScriptCommandT)(data)]->getClassName());
	//CString strCmdName = m_Grid.GetTreeItemText(hItemCommand,0);
	CString instLine = strCmdName + _T("( ");
	CString instDeclar = _T("";)
	CString strParaName;
	VariableKey vkey;
	for ( unsigned int i=0; i< item_para_list.size(); i++)
	{
		vkey = getVarKeyFromItem(item_para_list[i]);
		if ( vkey.valid )
		{
			if ( vkey.varSet == SVAR_SYSTEM )
				strParaName = STR_LABEL_SYSTEM;
			else if( vkey.varSet == SVAR_GLOBAL )
				strParaName = STR_LABEL_GLOBAL;
			else
				strParaName = _T("");

			strParaName += CString(vkey.varName);
		}
		else
			strParaName = m_Grid.GetTreeItemText( item_para_list[i], 1 );

		if ( isClassFunc && i == 0 )
		{
			instLine = strParaName + instLine.Right( instLine.GetLength() - instLine.ReverseFind(':') );
		}
		else if ( isDeclare )
		{
			if ( i == 0 )
			{
				if ( strParaName == _T("true") )
					strParaName = _T("local");
				else
					strParaName = _T("");
				instDeclar = strParaName;
			}
			else
			{
				if ( instDeclar.IsEmpty() )
				{
					instLine = strParaName + _T(" = ") + instLine;
 				}
				else
				{
					instLine = instDeclar + _T(" ") + strParaName + _T(" = ") + instLine;
 				}
 			}
		}
		else
		{
		 
			instLine = instLine + strParaName;
			if ( i < item_para_list.size() -1 ) 
				instLine += _T(", ") ;
		}

	}

	instLine.TrimRight(CString(STR_LUA_NIL));
	instLine.TrimRight();
	instLine.TrimRight(_T(","));
	instLine.TrimRight(CString(STR_LUA_NIL));
	instLine.TrimRight();
	instLine.TrimRight(_T(","));

	instLine += _T(" )") ;

	instructionEdit_.SetWindowText( instLine );
	//UpdateData(FALSE);
	//instructionEdit_.SetFocus();
	//instructionEdit_.SetSel(0,-1);

	m_Grid.SetTreeItemText(hItemCommand,0,instLine); // update the first row of grid.
	m_Grid.Invalidate();
}

VariableKey InstructionEditDLG::getVarKeyFromRow( int nRow, bool bNeedName /* = false */ )
{
	VariableKey vkey;
	int dwData  = static_cast<int>( m_Grid.GetItemData(nRow,1) );
	if ( nRow < 2 || dwData < 0 ) // parameters from 2 row
	{
		vkey.valid = false;
		return vkey;
	}

	vkey.varSet = static_cast<ScriptVarSetT>( HIBYTE(dwData) );
	vkey.varTyp = static_cast<ScriptVarT>   ( LOBYTE(dwData) );
	if ( vkey.varTyp < svAXISPOS || vkey.varTyp >= svTypeNum || vkey.varSet >= SVAR_SET_NUM || vkey.varSet < SVAR_SYSTEM )
	{
		vkey.valid = false;
		return vkey;
	}

	vkey.nRow = nRow;

	GTV_HTREENODE currItem   = m_Grid.GetTreeNode(nRow);
	GTV_HTREENODE parentItem = m_Grid.GetParentNode(currItem);
	GTV_HTREENODE rootItem   = m_Grid.GetRootNode();

	CString stritem = m_Grid.GetTreeItemText(currItem, 0);
	sprintf( vkey.strKey, "%s", Unicode2Ansi(stritem.GetBuffer(0)).c_str() );

	if ( !m_Grid.NodeHasChildren(currItem) )
	{
		if ( parentItem != NULL && parentItem == rootItem )
		{
			cout << "the cell is first members, not struct data \n";
			CString stritem = m_Grid.GetItemText(nRow, 1);
			stritem.Trim();
			sprintf( vkey.varName, "%s", Unicode2Ansi(stritem.GetBuffer(0)).c_str() );
			vkey.propName[0] = '\0';
		}
		else
		{
			cout << "the cell is prop value \n";

			vkey.first_members = false;
			vkey.propTyp = vkey.varTyp;

			CString strParentItem = m_Grid.GetTreeItemText( parentItem, 1 );
			sprintf( vkey.varName, "%s", Unicode2Ansi(strParentItem.GetBuffer(0)).c_str() );
			int nRowParent = m_Grid.FindRowIndex(parentItem);
			dwData  = static_cast<int>( m_Grid.GetItemData(nRowParent,1) );
			vkey.varTyp = static_cast<ScriptVarT> ( LOBYTE(dwData) );

			CString strPropName = m_Grid.GetTreeItemText(currItem,0);
			CString strName = strPropName.SpanExcluding(_T(":"));
			strName.Trim();
			sprintf( vkey.propName, "%s", Unicode2Ansi(strName.GetBuffer(0)).c_str() );
		}
	}
	else
	{
		if ( parentItem != NULL && parentItem == rootItem )
		{
			cout << "is struct data name \n";
			CString stritem = m_Grid.GetItemText(nRow, 1);
			stritem.Trim();
			sprintf( vkey.varName, "%s", Unicode2Ansi(stritem.GetBuffer(0)).c_str() );
			vkey.propName[0] = '\0';
		}
		else
		{
			ASSERT(FALSE);// 
			vkey.valid = false;
		}
	}

	if ( parentItem == NULL )
		vkey.valid = false;

	// 	cout << " varKey = " << vkey.varSet << " : " << vkey.varTyp <<" : " << vkey.first_members << " : " << string(vkey.varName) 
	// 	 		<< " : "  << vkey.valid << " : " << string(vkey.propName) << '\n';


	return vkey;
}

VariableKey InstructionEditDLG::getVarKeyCurr( bool bNeedName /* = false */ )
{
	int row = m_Grid.GetFocusCell().row;
	return getVarKeyFromRow(row,bNeedName);
}

VariableKey InstructionEditDLG::getVarKeyFromItem( GTV_HTREENODE  _item, bool bNeedName )
{
	VariableKey _key;
	if ( _item == NULL )
	{
		_key.valid = false;
		return _key;
	}
	int row = m_Grid.FindRowIndex(_item);
	return getVarKeyFromRow(row, bNeedName);
}

void InstructionEditDLG::update_parameter( GTV_HTREENODE hItem, ScriptVarT vtyp, LPCTSTR strVarName, ScriptVarSetT vset )
{

}

void InstructionEditDLG::OnClickTree(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*) pNMHDR;
	int nRow = pItem->iRow;
	int nCol = pItem->iColumn;
	TRACE(_T("dan Right button click on row %d, col %d\n"), pItem->iRow, pItem->iColumn);

	VariableKey vkey = getVarKeyFromRow(nRow,true);
	if ( !vkey.valid || vkey.varSet == SVAR_SYSTEM )
		return;

	if ( nCol == 1 )
	{
		CGridCellBase *pCell = m_Grid.GetCell(nRow, 1); 
		if ( !pCell )
			return;

		if ( pCell->GetRuntimeClass() == RUNTIME_CLASS(CGridCellCombo) ||
			pCell->GetRuntimeClass() == RUNTIME_CLASS(CGridCellCheck) )  // liurundan
		{
			return;
		}
		parse_var_key( vkey );
	}
	else
		*pResult = 0;

}

void InstructionEditDLG::parse_var_key(const VariableKey& vkey)
{
	ScriptVarT svDataType = vkey.varTyp;
	if( !vkey.first_members )
		svDataType = vkey.propTyp;

	if ( svDataType == svAXISPOS || svDataType == svCARTPOS )
	{
		return;
	}
	else
	{
 		m_CellEdit.row = vkey.nRow;
		m_CellEdit.col = 1;
 
		CString typeStr = m_Grid.GetItemText( vkey.nRow, 0 );
		CString dataStr = m_Grid.GetItemText( vkey.nRow, 1 );

		if ( svDataType == svINTEGER || svDataType == svLUAINTEGER || svDataType == svRDI || svDataType == svRDO )
		{
			DKEY->activate_keyboard(GetSafeHwnd(),dataStr,typeStr,(LPARAM)&m_CellEdit);
			DKEY->SetValueRange(-1000000, 1000000, 0);
		}
		else if ( svDataType == svREAL || svDataType == svLUAREAL )
		{
			DKEY->activate_keyboard(GetSafeHwnd(),dataStr,typeStr,(LPARAM)&m_CellEdit);
			DKEY->SetValueRange(-1000000, 1000000, 3);
		}
		else if ( svDataType == svBOOL || svDataType == svLUABOOL )
		{
			FKEY->activate_keyboard(GetSafeHwnd(),dataStr,typeStr,(LPARAM)&m_CellEdit);
		}
		else if ( svDataType == svSTRING || svDataType == svLUASTRING )
		{
			FKEY->activate_keyboard(GetSafeHwnd(),dataStr,typeStr,(LPARAM)&m_CellEdit);
		}
	}
}

LRESULT InstructionEditDLG::OnKeyboardEnter( WPARAM wp, LPARAM lp )
{
	if ( wp == NULL )
 		return 0;
 
 	CString str_result = *((CString *)wp);
	if ( lp == NULL ) // edit
	{
        instructionEdit_.SetWindowText( str_result );
 		return 0;
	}

	CCellID cell = *(CCellID*)(lp); // grid
	int row = cell.row;
	int col = cell.col;
	if ( row < 0 || col < 0 )
 		return 0;
 
	bool valid = true;

	VariableKey vkey = getVarKeyFromRow( row, true );
	if ( !vkey.valid || vkey.varSet == SVAR_SYSTEM )
		return 0;

	if ( str_result != _T("0") && str_result.GetLength() > 1 )
	{
		str_result.TrimLeft('0');
		str_result.Trim();
	}

	if ( col == 1 ) // edit value
	{
		if ( VARINFO->isBaseVariableType(vkey.varTyp) == false )
		{
			valid = VARRW->write_variable(&vkey, Unicode2Ansi(str_result.GetBuffer(0)).c_str() );
		}
	    // Is the value valid?
		if ( valid )
		{
			m_Grid.SetCellText(row, 1, str_result);
			m_Grid.Invalidate();

			if ( vkey.first_members )
			{
				update_edit();
			}		
 		}
		
 
	
	}

	return 1;
}

// 新建变量后激活窗口，主要是返回变量的名字，并将其存入单元格的列表控件中。
int InstructionEditDLG::OnActivateChildWin( LPARAM lp )
{
	if ( lp )
	{
		cout << " update cmd param \n";
		RegParseResult*  result =  (RegParseResult*)(lp);
		NM_GRIDVIEW pNotifyStruct;
		pNotifyStruct.iColumn = 1;
		int iRet = 0;
		VariableKey vkey;

		if ( result->nFromID == 0 ) // builder command
		{
			isCreatCommand = true;
			insert_script_command( result->sct, true );
		}
		else if ( result->nFromID == 1 ) // new variable
		{
			vkey = result->params.front();
			int dwData = MAKEWORD( vkey.varTyp, vkey.varSet );
			pNotifyStruct.iRow = vkey.nRow;
			OnTreeBeginEdit( (NMHDR*)&pNotifyStruct,(LRESULT*) &iRet );

			m_Grid.SetItemData( vkey.nRow, 1, dwData );
			m_Grid.SetItemText(  vkey.nRow, 1, CString(vkey.varName) );
			m_Grid.SetItemImage(  vkey.nRow, 1,  vkey.varSet );

			CString strVarData;
			dwData = MAKEWORD( vkey.varTyp, static_cast<int>(vkey.varSet) );
			strVarData.Format( _T("%d"), dwData );

			varNameArray[vkey.varTyp].Add( CString(vkey.varName)+STR_SEPARATOR+strVarData );

			CGridCellCombo* pCell = (CGridCellCombo*) m_Grid.GetCell(vkey.nRow, 1);
			if ( vkey.varTyp == svAXISPOS || vkey.varTyp == svCARTPOS  )
			{
				pCell->SetTwoTypeOptions( varNameArray[svAXISPOS], varNameArray[svCARTPOS], TRUE, STR_SEPARATOR );
			}
			else
				pCell->SetOptions( varNameArray[vkey.varTyp], TRUE, STR_SEPARATOR );


			OnTreeEndEdit((NMHDR*)&pNotifyStruct,(LRESULT*)&iRet);

		}
		else if ( result->nFromID == 2 ) // modify srcript command.
		{
			isCreatCommand = false;
			insert_script_command( result->sct, false );
			int nRowPara = 0; 
			for (unsigned int i=0; i< item_para_list.size(); i++)
			{
				 nRowPara = m_Grid.FindRowIndex(item_para_list[i]);
				 pNotifyStruct.iRow = nRowPara;
				 vkey = result->params[i];
				 OnTreeBeginEdit( (NMHDR*)&pNotifyStruct,(LRESULT*)&iRet );
 				 
                 if ( vkey.varTyp == svAXISPOS )
                 {
					 void* pVar = LVAR->get_value_ptr(svAXISPOS,vkey.varName,vkey.varSet);
					 if ( pVar == NULL )
 						 vkey.varTyp = svCARTPOS;
                 }
				 int dwData = MAKEWORD( vkey.varTyp, vkey.varSet );

				 if ( VARINFO->isBaseVariableType(vkey.varTyp) == false || strstr( vkey.varName, STR_LUA_NIL ) )
				 {
					 m_Grid.SetItemImage(nRowPara , 1,  vkey.varSet );
				 }
				 m_Grid.SetItemData( nRowPara, 1, dwData );
				 m_Grid.SetItemText(nRowPara, 1, CString(vkey.varName) );

				 OnTreeEndEdit((NMHDR*)&pNotifyStruct,(LRESULT*)&iRet);
			}
  
		}
 	
	}
	return 1;
}
