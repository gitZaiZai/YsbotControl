// Copyright 2016, 亚思信息科技有限责任公司
// All right reserved.
//
// Author: Liu Run Dan ( lrdcz@163.com )
//
// Yasi Robot Motion Control System - 亚思机器人运动控制系统

#include "stdafx.h"
#include "resource.h"
#include "variable_manager.h"
#include "stringconvert.h"
#include "MotionControl.h"
#include "LuaScript/ScriptVariables.h"
#include "control_id.h"
#include "WorldModel.h"
#include "Keyboard/digital_keyboard.h"
#include "Keyboard/full_qwerty.h"
#include "location_manager.h"
#include "GridCtrl_src/StringCompare.h"
#include "TeachDialog/TeachDlg.h"
#include "UIDialogs/state_dialog.h"

VariableDialog *variable_dialog = nullptr;

BEGIN_MESSAGE_MAP(VariableDialog, DialogType)
   	ON_NOTIFY( NM_DBLCLK, VARIABLE_DIALOG_TREE, &VariableDialog::OnClickTree )
	ON_NOTIFY( GVN_BEGINLABELEDIT,VARIABLE_DIALOG_TREE,   &VariableDialog::OnTreeBeginEdit)
	ON_NOTIFY( GVN_ENDLABELEDIT, VARIABLE_DIALOG_TREE,    &VariableDialog::OnTreeEndEdit)
	ON_COMMAND_RANGE( VARIABLE_RADIO_BUTTON_1, VARIABLE_RADIO_BUTTON_1+2, &VariableDialog::OnRadioButtonClicked )
	ON_NOTIFY( WM_CUSTOMCOMBOX_SELCHANGED, VARIABLE_COMBOBOX_TYPE, &VariableDialog::OnSelchangeCommbo)

END_MESSAGE_MAP()

VariableDialog::VariableDialog( HBITMAP hBmpScrollBar,
	HBITMAP hBmpPlus,
	HBITMAP hBmpMinus,
	DlgNewVariable* pNewVar ) 
	: m_hBmpScrollBar(hBmpScrollBar),m_hBmpPlus(hBmpPlus), m_hBmpMinus(hBmpMinus),pWndNewVar(pNewVar),
	edit_step(VAR_NONE),src_edit_htree(NULL),m_nRadioType(0),m_columFitTypeGrid(0),m_bResizeGrid(true),
	type_combox(12)
{
 	for ( int i = 0; i < SVAR_SET_NUM; ++i)
	{
		m_hItemVar[i] = NULL;
		m_icon[i] = -1;
 	}
    radio_list.clear();
	var_filter.clear();
}

VariableDialog::~VariableDialog() 
{
	image_list_.DeleteImageList();
	for (unsigned int i = 0; i < radio_list.size(); ++i )
	{
		delete radio_list[i];
	}
	radio_list.clear();
	var_filter.clear();
 
}

void VariableDialog::DoDataExchange(CDataExchange* pDX) {

}

LRESULT VariableDialog::OnResizeGrid(WPARAM wp,LPARAM lp)
{
// 	static UINT lastType = 0;
  	static UINT i = 0;
	UINT nType = UINT(wp);
 
	// re-size all pages
	if ( m_columFitTypeGrid != nType )
	{
		m_Grid.ExpandColumn(1,250, (nType==0) ); // 0 - fit, 1 = resize colum.
		m_columFitTypeGrid = nType;
		printf(" N %d dialog on size message %d\n",i++,nType);
	}

// 	if ( nType == 1 )
// 	{
// 		if ( m_columFitTypeGrid != nType )
// 		{
// 			m_Grid.ExpandColumn(1,250,FALSE);
//  			m_columFitTypeGrid = nType;
// 			printf(" N %d dialog on size message %d\n",i++,nType);
// 		}
// 	}
// 	else
// 	{
// 		if ( m_columFitTypeGrid != nType )
// 		{
// 			m_Grid.ExpandColumn(1,250,TRUE);
// 			m_columFitTypeGrid = nType;
// 			printf(" N %d dialog on size message %d\n",i++,nType);
// 		}
// 	}
	printf(" dialog on size message\n");

	return 1;
}

BOOL VariableDialog::OnInitDialog() {
	CDialogEx::OnInitDialog();
 
	if ( GetParent()->GetDlgItem(IDC_BOX_MAIN)->GetSafeHwnd() )
	{
		GetParent()->GetDlgItem(IDC_BOX_MAIN)->GetClientRect(&clientRect);
		SetWindowPos(NULL,clientRect.left, clientRect.top, clientRect.Width(), clientRect.Height(), SWP_NOZORDER);
	}

	SetBackgroundColor(kColor[8]);
// 	ModifyStyleEx(WS_EX_STATICEDGE, WS_EX_CLIENTEDGE, SWP_DRAWFRAME);
 
	// 创建图片列表
	image_list_.Create (22, 22, ILC_COLOR32 | ILC_MASK, 5, 1);
	m_icon[SVAR_SYSTEM]  = image_list_.Add(AfxGetApp()->LoadIcon(IDI_VARIABLE_SYSTEM));
	m_icon[SVAR_GLOBAL]  = image_list_.Add(AfxGetApp()->LoadIcon(IDI_VARIABLE_GLOBAL));
	m_icon[SVAR_PROGRAM] = image_list_.Add(AfxGetApp()->LoadIcon(IDI_VARIABLE_PROJECT));
	int overlay_icon = image_list_.Add(AfxGetApp()->LoadIcon(IDI_OVERLAY_TRANS));
    image_list_.SetOverlayImage(overlay_icon, 1);
 
	m_rcTextLabel = CRect(10, 402, 95, 434);
	create_tree_grid();
	// 标签一

	title1_.Create(_T("过滤器"), WS_CHILD | WS_VISIBLE, m_rcTextLabel, this);
	title1_.SetAlignment(1);
	title1_.Initialize(kColor[0]);
	title1_.SetFontType(20,400,_T("Arial"));

	// 类型选择下拉框
	CRect rcCombobox = CRect(400, m_rcTextLabel.top - 5, 725, 415);
	type_combox.Create(WS_CHILD | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED |CBS_HASSTRINGS,rcCombobox, this, VARIABLE_COMBOBOX_TYPE );
 	 
	type_combox.AddString(_T("BOOL"));
	type_combox.AddString(_T("INT"));
	type_combox.SetShowImage(FALSE);
// 	type_combox.SetFontType(20,_T("Arial"));
	type_combox.SetCurSel(0);
	type_combox.EnableWindow(FALSE);
    type_combox.SetAlignTop(TRUE);
	CIconComboBox::set_DropDownHight(type_combox,type_combox.GetLineNumber() );

	//type_combox.set_DropDownHight(6);
	
	CRect rcra[3];
 	rcra[0] = rcra[1] = rcra[2] = m_rcTextLabel;
 
	for ( int i = 0; i < 3; ++i )
	{
		rcra[i].OffsetRect(90*(i+1),-5);
		CPngRadio* pRadio = new CPngRadio();
		pRadio->Create(NULL,WS_CHILD|WS_VISIBLE|WS_GROUP,rcra[i],this,VARIABLE_RADIO_BUTTON_1+i);
		pRadio->SetFontType(20,_T("Arial"));
		pRadio->InitImage(NULL,_T("res/radio-slelected.bmp"),_T("res/radio-empty.bmp"),_T("res/radio-empty_disable.bmp"),_T("res/radio-slelected_disable.bmp") );
		radio_list.push_back(pRadio); 
 	}
	radio_list[0]->SetWindowText(_T("全部"));
	radio_list[1]->SetWindowText(_T("类型"));
 	radio_list[2]->SetWindowText(_T("类别"));
	CPngRadio::AddGroup(radio_list);  
 	radio_list[m_nRadioType]->SetChecked(TRUE);


 	return TRUE;
}

BOOL VariableDialog::create_tree_grid() throw (InvalidConfigurationException)
{
	BOOL ret = m_Grid.Create(
		CRect(clientRect.left, clientRect.top, clientRect.right, m_rcTextLabel.top-20 ), 
		this,
		VARIABLE_DIALOG_TREE, WS_VISIBLE | WS_CHILD,
		m_hBmpScrollBar, m_hBmpPlus, m_hBmpMinus, SKIN_WINDOW_VARIABLE, 0 );

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
		lf.lfHeight = 24;
		wcsncpy_s(lf.lfFaceName, _T("Arial"), LF_FACESIZE-1);
		CFont font;
		font.CreateFontIndirect(&lf);
		m_Grid.SendMessage(WM_SETFONT,(WPARAM)(font.GetSafeHandle()));
		font.DeleteObject();

		m_Grid.SetEditable(FALSE);
		m_Grid.SetListMode(TRUE);

		m_Grid.SetGridLines(GVL_BOTH);
		m_Grid.SetGridLineColor(RGB(60,64,60));

		m_Grid.SetFixedRowSelection(TRUE);
		m_Grid.SetSingleRowSelection(TRUE);
		m_Grid.SetFixedColumnSelection(FALSE);

		initialize_tree_grid();
  	}
 	return ret;
}

BOOL VariableDialog::initialize_tree_grid()
{
 	m_Grid.SetAutoSizeStyle();

  	m_Grid.SetFixedRowCount(1);
  
    m_Grid.SetDefCellHeight(38);
 
//   	m_Grid.SetRowHeight(0,34);
  
	m_Grid.InsertColumn(_T("变量"),DT_LEFT|DT_TOP);
	m_Grid.InsertColumn(_T("数值"),DT_LEFT|DT_TOP);

	m_Grid.GetCell(0, 0)->SetDrawSortArrow(FALSE); 
	m_Grid.GetCell(0, 1)->SetDrawSortArrow(FALSE); 
	
	m_Grid.SetColumnWidth(0, 400);
	m_Grid.SetColumnWidth(1, 250);
	 
  	m_Grid.ExpandColumnsToFit(); // liurundan
 
	m_Grid.SetTreeColumnCellTypeID(CT_DEFAULT);	
 
	// Write extra codes for setting up the treelistctrl for sorting
	m_Grid.SetHeaderSort(FALSE);

	m_Grid.SetShowSrollBar(SB_HORZ,FALSE); // liurundan.

   	m_Grid.SetCompareFunction(CGridCtrl::pfnCustomCompare);
  
 
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
GTV_HTREENODE VariableDialog::init_variable_grid(ScriptVarSetT svm)
{
	GTV_HTREENODE hItemCurr = NULL;
	GV_ITEM Item;
	Item.mask    = GVIF_TEXT | GVIF_IMAGE | GVIF_FORMAT | GVIF_STATE;
	Item.nFormat = DT_LEFT | DT_BOTTOM;
	Item.nState  = GVIS_READONLY;
	if ( svm == SVAR_SYSTEM )
	{
		Item.strText = _T("系统");
   	}
	else if ( svm == SVAR_GLOBAL )
	{
		Item.strText = _T("全局");
   	}
	else if ( svm == SVAR_PROGRAM )
	{
		Item.strText = _T("程序");
   	}
	Item.iImage = m_icon[svm];
	hItemCurr   = m_Grid.InsertItem(&Item);
 	m_Grid.SetItemState(m_Grid.FindRowIndex(hItemCurr),1,GVIS_READONLY);
	int nRow = m_Grid.FindRowIndex(hItemCurr);
 	m_Grid.SetItemData( nRow, 1, -1 );

	return hItemCurr;
		
}


void VariableDialog::insert_script_variable( const std::vector<ScriptVarT>& varTypefilter, ScriptVarSetT svm )
{
	const std::map<ScriptVarT, std::map<std::string, void*>>* var_list = LVAR->get_var_list(svm);
	bool bFilterVar = ( varTypefilter.size() == 0 ) ? false : true;

	std::string varname = "";
	for( map<ScriptVarT,  map<string, void*>>::const_iterator it = var_list->begin(); 
		it != var_list->end(); 
		it++ ) 
	{
		for ( map<string, void*>::const_iterator innerIt = it->second.begin(); 
			innerIt != it->second.end(); 
			innerIt++ ) 
		{
			varname = innerIt->first;
			if ( bFilterVar )
			{
				for (unsigned int i = 0; i < varTypefilter.size(); ++i )
				{
					if ( it->first == varTypefilter[i] )
					{
						InsertVariable( it->first, varname.c_str(), innerIt->second, svm );
						break;
					}
				}
 			}
			else
			{
				InsertVariable( it->first, varname.c_str(), innerIt->second, svm );
 			}
		}
	}
}

void VariableDialog::update_variable_grid(ScriptVarSetT svm)
{
	BOOL ret = TRUE;
	m_Grid.SetRedraw(FALSE);

	if ( m_hItemVar[svm] )
	{
		ret = m_Grid.DeleteItem( m_hItemVar[svm] );
  	}

	if ( ret )
	{
		if ( svm == SVAR_PROGRAM && pWndNewVar )
		{
 			pWndNewVar->reset_var_index(0);
 		}
 
		m_hItemVar[svm] = init_variable_grid(svm);
	    
 		insert_script_variable(var_filter, svm);
		sort_variable(svm);
 
		m_Grid.CollapseTreeNode( m_hItemVar[svm] );	
	}
	m_Grid.SetRedraw(TRUE);

}
 
// 插入变量，输入类型、名称、数值 
void VariableDialog::InsertVariable( ScriptVarT sv, const char* vname, void* ptrvalue, ScriptVarSetT svm ) 
{
      
	std::string varinfo = std::string(vname) + SPLIT_STR + std::string(LVAR->get_type_name(sv));
	 
	GTV_HTREENODE hItemcurr = m_hItemVar[svm];
 
	if ( hItemcurr == NULL )
	{
		cout << "error item!\n";
	}

	GTV_HTREENODE namehItem = m_Grid.InsertItem( CString(varinfo.c_str()), hItemcurr );
	if ( namehItem != NULL )
	{
		int nRow = m_Grid.FindRowIndex(namehItem);
		DWORD dwData = 0; 
		dwData = MAKEWORD( sv, svm );
		m_Grid.SetItemData( nRow, 1, dwData );
// 		m_Grid.SetItemData( nRow, 0, (LPARAM)(vname) );

		if ( sv == svAXISPOS || sv == svCARTPOS )
		{
 			m_Grid.SetTreeItemText(namehItem, 1, CString(VARINFO->get_pose_lable()));
		}
		else
		{
			m_Grid.SetItemFormat(nRow,1,DT_RIGHT);
		}
		m_Grid.SetItemState(nRow,0,GVIS_READONLY);
		m_Grid.SetItemState(nRow,1,GVIS_READONLY);

		if ( sv == svBOOL || sv == svLUABOOL )
		{
			if ( svm != SVAR_SYSTEM )
			{
				m_Grid.SetItemState(nRow,1,m_Grid.GetItemState(nRow, 1) & ~GVIS_READONLY);	
 			}
		 	m_Grid.SetCellType(nRow, 1, RUNTIME_CLASS(CGridCellCheck));
		}
		else
		    insert_varprop_item( namehItem, sv, svm );

		update_tree( namehItem, sv, ptrvalue );
		m_Grid.CollapseTreeNode(namehItem);
	}
 
}

void VariableDialog::insert_varprop_item(GTV_HTREENODE item, ScriptVarT sv, ScriptVarSetT svm )
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
			if ( svm != SVAR_SYSTEM )
				m_Grid.SetItemState(nRow,1,m_Grid.GetItemState(nRow, 1) & ~GVIS_READONLY);	

			m_Grid.SetCellType(nRow, 1, RUNTIME_CLASS(CGridCellCheck));
		}
		// 变量属性增加下拉框选项
  	}
}
 
void VariableDialog::update_tree(GTV_HTREENODE varitem, ScriptVarT sv, void* valueptr)
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

void VariableDialog::update_check_box(GTV_HTREENODE hitem, BOOL b)
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
 
void VariableDialog::teach_robot_position()
{
 	VariableKey vkey = getVarKeyCurr(true);
	if ( !vkey.valid || vkey.varSet == SVAR_SYSTEM )
		return;

	if ( vkey.varTyp != svAXISPOS && vkey.varTyp != svCARTPOS  )
 		return;

	PosInfo pi; 

	if ( vkey.varTyp == svAXISPOS )
	{
		pi = SMC.get_position(PosInfo::CTaxis);
	}
	else if ( vkey.varTyp == svCARTPOS )
	{
		pi = SMC.get_position(PosInfo::CTmachine);
		JogCoordT jog_type = MWM.get_robot_state().coordinate;
		if ( jog_type == jtTOOL || jog_type == jtUSER )
		{
			CartRef cref = MWM.get_jog_cart().refSys;
			CartRef tool = MWM.get_jog_tool().refSys;
			CartPos tcp;
			MWM.calc_tcp_pose( &pi.cart, &cref, &tool.cpos, &tcp );
			pi.cart = tcp;
		}
	}
	 
	update_tree( m_Grid.GetTreeNode(vkey.nRow), vkey.varTyp, &pi );
	LVAR->modify_variable_ui( vkey.varTyp, vkey.varName, &pi, vkey.varSet );
	m_Grid.Invalidate();	
 }

// 
bool VariableDialog::new_variable_ui( ScriptVarT vartype, const char* varname, ScriptVarSetT svm, bool fromEditWin )
{
	if ( svm == SVAR_SYSTEM )
 		return false;

 	ScriptVarT sv  = vartype;
	bool ret = LVAR->creat_variable( vartype, varname, NULL, svm );  
  
	if( ret )
	{
 		void* ptrvalue = LVAR->get_value_ptr(vartype,varname, svm);

 
		InsertVariable( vartype, varname, ptrvalue, svm );
		sort_variable(svm);
		
		GTV_HTREENODE treeitem;
		for( treeitem = m_Grid.GetChildNode(m_hItemVar[svm]); 
			treeitem != NULL; 
			treeitem = m_Grid.GetNextSiblingNode(treeitem) )
		{
			m_Grid.CollapseTreeNode(treeitem);
		} 

		if ( fromEditWin )
		{
			for ( int i = SVAR_SYSTEM; i < SVAR_SET_NUM; ++i )
			{
				if ( m_hItemVar[i] )
				{
					m_Grid.CollapseTreeNode( m_hItemVar[i] );
				}
			}
 		}

// 		if ( fromEditWin && m_columFitTypeGrid == 1 )
//   			m_bResizeGrid = false;
 
 
 		std::string varinfo = string(varname) + SPLIT_STR + LVAR->get_type_name(vartype);

		// add variable name to location dialog commbox.
		if ( vartype == svTOOL || vartype == svCARTREF )
 		{
			location_dialog->SendMessage( WM_UPDATE_COMMBOX, 1, LPARAM( &CString(varinfo.c_str()) ) );
 		}

		 
   	}
	
	return ret;
}

void VariableDialog::delete_variable_ui()
{
	VariableKey vkey = getVarKeyCurr(true);
	if ( !vkey.valid || vkey.varSet == SVAR_SYSTEM ||  !vkey.first_members )
		return;
 
    delete_variable(vkey);

}

void VariableDialog::delete_variable(const VariableKey& vkey)
{
 	GTV_HTREENODE htree = m_Grid.GetTreeNode(vkey.nRow);
	BOOL haschildren    = m_Grid.NodeHasChildren(htree);
	if ( vkey.varTyp == svTOOL || vkey.varTyp == svCARTREF )
	{
		CString strval = m_Grid.GetTreeItemText(htree,0); // 获得节点值
 		location_dialog->SendMessage( WM_UPDATE_COMMBOX, 0, LPARAM( &strval ) );
	}
  	m_Grid.DeleteItem(htree);
	 
	LVAR->delete_variable( vkey.varTyp, vkey.varName, vkey.varSet ); 
	cout << "del " << string(vkey.varName) << '\n';
}

void VariableDialog::rename_variable_ui()
{
	VariableKey vkey = getVarKeyCurr(true);
	if ( !vkey.valid || vkey.varSet == SVAR_SYSTEM )
		return;

 	m_CellEdit.row = vkey.nRow;
	m_CellEdit.col = 0;
	FKEY->activate_keyboard( GetSafeHwnd(), Ansi2Unicode(vkey.varName).c_str(),
		Ansi2Unicode(LVAR->get_type_name(vkey.varTyp)).c_str(),(LPARAM)&m_CellEdit);
  
}

LRESULT VariableDialog::OnMainButtonClicked( WPARAM wp, LPARAM lp )
{
	int btnid  = (int)(wp);
	int menuid = (int)(lp);

	switch( btnid )
	{
	case 0: 
		{
			switch(menuid)
			{ 
 			case 0: paste_variable(); break;
			case 1: copy_variable();  break;
			case 2: cut_variable();   break;
			case 3: rename_variable_ui(); break;
 			default: break;
			}
		}
		break;
	case 1: teach_robot_position(); break; // teach
	case 2: getVarKeyCurr(true);    break; // check
	case 3: GetParent()->SendMessage( WM_ACTIVATE_WINDOW, winNewScriptVariable, 0 ); break;
	case 4: delete_variable_ui(); break;
	case 5: update_grid_data();	  break;
	default: break;
	}
// 	printf("VariableDialog id = %d, menu_id = %d\n", btnid, menuid );
 	return 1;
}

// system variable do not allow change.
void VariableDialog::update_grid_data()
{
	GTV_HTREENODE treeitem      = NULL;
 	VariableKey vkey;
	for ( ScriptVarSetT varSet = SVAR_GLOBAL; varSet <= SVAR_PROGRAM; )
	{
 		for( treeitem = m_Grid.GetChildNode(m_hItemVar[varSet]); 
			treeitem != NULL; 
			treeitem = m_Grid.GetNextSiblingNode(treeitem) )
		{
 			vkey = getVarKeyFromItem(treeitem,true);
			if ( vkey.valid )
			{
				void* pval    = LVAR->get_value_ptr( vkey.varTyp, vkey.varName, varSet );
				update_tree( treeitem, vkey.varTyp, pval );
 			}
		} 

		varSet = (ScriptVarSetT)(int (varSet) + 1);
 	}
 
	m_Grid.Invalidate();
}


void VariableDialog::sort_variable(ScriptVarSetT svm)
{
  	m_Grid.ExpandTreeNode(m_hItemVar[svm]);
	m_Grid.SortItems(m_hItemVar[svm],NULL,0,TRUE,0,FALSE);
	 
}

void VariableDialog::OnClickTree(NMHDR* pNMHDR, LRESULT* pResult)
{
 	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*) pNMHDR;
	int nRow = pItem->iRow;
	int nCol = pItem->iColumn;
  	 
	VariableKey vkey = getVarKeyFromRow(nRow,true);
	if ( !vkey.valid || vkey.varSet == SVAR_SYSTEM )
  		return;
   
	if ( nCol == 1 )
	{
		CGridCellBase *pCell = m_Grid.GetCell(nRow, 1); 
		if ( pCell == NULL )
 			return;

 		if ( pCell->GetRuntimeClass() == RUNTIME_CLASS(CGridCellCombo) ||
			 pCell->GetRuntimeClass() == RUNTIME_CLASS(CGridCellCheck) )  // liurundan
		{
 			return;
 		}
		else
		{
 			parse_var_key(vkey);
		}
 	}
	else
 	    *pResult = 0;
 	 
}

void VariableDialog::OnTreeBeginEdit(NMHDR *pNotifyStruct, LRESULT* pResult)
{
 	//	TRACE(_T("Start Edit on row %d, col %d\n"), pItem->iRow, pItem->iColumn);
 	 
	GV_DISPINFO *pgvDispInfo = (GV_DISPINFO *)pNotifyStruct;
	GV_ITEM     *pgvItem = &pgvDispInfo->item;
	
 	int nRow = pgvItem->row;
 	VariableKey vkey = getVarKeyFromRow(nRow);
	if ( !vkey.valid )
	{
		*pResult = -1;
		return;
	}
	else
		*pResult = (vkey.varSet == SVAR_SYSTEM)? -1 : 0; // assign -1 here to reject edit changes
  
}

void VariableDialog::OnTreeEndEdit(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	//	TRACE(_T("End Edit on row %d, col %d\n"), pItem->iRow, pItem->iColumn);
	GV_DISPINFO *pgvDispInfo = (GV_DISPINFO *)pNotifyStruct;
	GV_ITEM     *pgvItem = &pgvDispInfo->item;
	int nRow = pgvItem->row;
	int nCol = pgvItem->col;
	 
	VariableKey vkey = getVarKeyFromRow(nRow,true);
	if ( !vkey.valid )
	{
		*pResult = -1;
		return;
	}
	else
		*pResult = (vkey.varSet == SVAR_SYSTEM)? -1 : 0; // assign -1 here to reject edit changes

	CGridCellBase *pCell = m_Grid.GetCell(nRow, 1); 
	if ( !pCell )
 		return;
 
	if ( pCell->GetRuntimeClass() != RUNTIME_CLASS(CGridCellCombo) &&
		 pCell->GetRuntimeClass()  != RUNTIME_CLASS(CGridCellCheck) )  // liurundan
	{
		return;
	}
   
	if ( pCell->GetRuntimeClass()  == RUNTIME_CLASS(CGridCellCheck) )
	{
		CGridCellCheck *pCheckCell = (CGridCellCheck*) m_Grid.GetCell( nRow, 1);
		BOOL bCheck = pCheckCell->GetCheck();
		VARRW->write_variable( &vkey, bCheck ? "true" : "false");
 	}
	else if ( pCell->GetRuntimeClass()  == RUNTIME_CLASS(CGridCellCombo) )
	{
		CString  str = m_Grid.GetItemText( nRow, nCol );
		//////////////////////////////////////////////////////////////////////////
 	}

}

void VariableDialog::parse_var_key(const VariableKey& vkey)
{
	ScriptVarT svDataType = vkey.varTyp;
	if( !vkey.first_members )
		svDataType = vkey.propTyp;

	if ( svDataType == svAXISPOS || svDataType == svCARTPOS )
	{
		GetParent()->SendMessage( WM_ACTIVATE_WINDOW, winTeachDLG, (LPARAM)&vkey );
 	}
	else
	{
 		m_CellEdit.row = vkey.nRow;
		m_CellEdit.col = 1;
		CString typeStr = m_Grid.GetItemText( vkey.nRow,0);
		CString dataStr = m_Grid.GetItemText( vkey.nRow,1);

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
 
LRESULT VariableDialog::OnKeyboardEnter(WPARAM wp,LPARAM lp)
{
	CString str_result = *((CString *)wp);
	CCellID cell = *(CCellID*)(lp);
	int row = cell.row;
	int col = cell.col;
  
 	bool valid = false;
 
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
		valid = VARRW->write_variable(&vkey, Unicode2Ansi(str_result.GetBuffer(0)).c_str() );

		// Is the value valid?
		if ( valid )
		{
			m_Grid.SetCellText(row, 1, str_result);
			m_Grid.Invalidate();
		}
	}
	else // edit variable name
	{

		CString newName = str_result + SPLIT_STR + LVAR->get_type_name(vkey.varTyp) ;
		m_Grid.SetCellText( row,col,newName);
		m_Grid.Invalidate();

 		LVAR->rename_variable( vkey.varTyp, vkey.varName, Unicode2Ansi(str_result.GetBuffer(0)), vkey.varSet );
 
	}
 
	return 1;
}
 
bool VariableDialog::copy_variable()
{
 	src_var_key = getVarKeyCurr(true);
	edit_step   = VAR_COPY;
	return true;
}

bool VariableDialog::cut_variable()
{
 	src_var_key = getVarKeyCurr(true);
	edit_step   = VAR_CUT;
	return true;
}

bool VariableDialog::paste_variable()
{
 	// <<<<<<<<<<
	if ( !src_var_key.valid || edit_step == VAR_NONE)
 		return false;

	if ( edit_step == VAR_CUT && src_var_key.varSet == SVAR_SYSTEM )
 		return false;
    
	if ( src_var_key.first_members == false )
 		return false;
 
 	VariableKey varSrc  = src_var_key;
	VariableKey varDest = getVarKeyCurr(true);

	if ( varDest.varSet == SVAR_SYSTEM || varSrc.varTyp != varDest.varTyp || 
	   ( varDest.varSet == varSrc.varSet && (strcmp(varSrc.varName, varDest.varName) == 0 ) ) )
	{
		return false;
	}
  	 
	void* pval = LVAR->get_value_ptr( varSrc.varTyp, varSrc.varName, varSrc.varSet );
	if ( !pval )
		return false;

	LVAR->set_value( varDest.varTyp, varDest.varName, pval, varDest.varSet );
	if ( edit_step == VAR_CUT )
	{
 		delete_variable(src_var_key);
 	}
	/*update_grid_data();*/
	GTV_HTREENODE varItem = m_Grid.GetTreeNode(varDest.nRow);
	update_tree( varItem, varDest.varTyp, pval );
	m_Grid.Invalidate();

	edit_step = VAR_NONE;
 
	return true;
}
 
void VariableDialog::OnRadioButtonClicked(UINT id)
{
	m_nRadioType = id - VARIABLE_RADIO_BUTTON_1;
	if ( m_nRadioType == 0 )
	{
		if ( type_combox.IsWindowEnabled() )
		{
			type_combox.EnableWindow(FALSE);
		}
	}
	else if ( m_nRadioType == 1 )
	{
		if ( !type_combox.IsWindowEnabled() )
		{
			type_combox.EnableWindow(TRUE);
		}
	}
	else if ( m_nRadioType == 2 )
	{
		if ( !type_combox.IsWindowEnabled() )
		{
			type_combox.EnableWindow(TRUE);
		}
	}
    update_type_combobox(m_nRadioType);
}

void VariableDialog::update_type_combobox(int radioType)
{
	std::string varTypeName = "";
    static int last_radio_type = 0;

	if ( type_combox.GetCount() > 0 )
	{
		type_combox.ResetContent();
	}

	if ( radioType == 0 && last_radio_type != 0 && var_filter.size() > 0 )
	{
		var_filter.clear();
		reload_grid_var();
	}
	else if ( radioType == 1  )
	{
		for( ScriptVarT svt = svAXISPOS; svt != svTypeNum; )
		{
			varTypeName = LVAR->get_type_name(svt);
 			type_combox.AddString( Ansi2Unicode(varTypeName).c_str(), (DWORD)(svt) );
			svt = (ScriptVarT)( (int)(svt) + 1 );
		}
 	}
	else if ( radioType == 2 )
	{
  		for( ScriptVarCategory vset = BASIC_KIND; vset != CATEGORY_NUM; )
		{
			varTypeName =VARINFO->var_category[vset]->getClassName();
			type_combox.AddString( Ansi2Unicode(varTypeName).c_str(), (DWORD)(vset) );
			vset = (ScriptVarCategory)( (int)(vset) + 1 );
		}
 	}

	last_radio_type = radioType;

	type_combox.SetCurSel(-1);
}

void VariableDialog::OnSelchangeCommbo(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	int index = type_combox.GetCurSel();
	if ( index >= 0 )
	{
		DWORD dwData = type_combox.GetItemData( index );
		var_filter.clear();
		if ( m_nRadioType == 1 )
		{
			var_filter.push_back((ScriptVarT)(dwData));
		}
		else if ( m_nRadioType == 2 )
		{
			ScriptVarCategory svc = (ScriptVarCategory)(dwData);
			for ( int i = 0; i < VARINFO->var_category[svc]->getValuesTotal(); ++i )
			{
				dwData = VARINFO->var_category[svc]->getValueKind(i+1);
				var_filter.push_back( (ScriptVarT)(dwData) );
			}
		}
		// 		cout << " LOC : " << CStringA(csValue) << curr_tool << '\n';
		reload_grid_var();
	}
}

void VariableDialog::reload_grid_var()
{
	m_Grid.SetRedraw(FALSE);

	for ( int i =0; i < SVAR_SET_NUM; ++i )
	{
		m_Grid.DeleteItem( m_hItemVar[i] );
		m_hItemVar[i] = NULL;
	}
	update_variable_grid(SVAR_SYSTEM);
	update_variable_grid(SVAR_GLOBAL);
	update_variable_grid(SVAR_PROGRAM);

	m_Grid.SetRedraw(TRUE);
}

VariableKey VariableDialog::getVarKeyFromRow( int nRow, bool bNeedName /* = false */ )
{
	VariableKey vkey;
	int dwData  = static_cast<int>( m_Grid.GetItemData(nRow,1) );
	if ( nRow < 0 || dwData < 0 )
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

	GTV_HTREENODE currItem = m_Grid.GetTreeNode(nRow);
	if ( bNeedName && currItem )
	{
 		CString stritem = m_Grid.GetTreeItemText(currItem, 0);
		sprintf( vkey.strKey, "%s", Unicode2Ansi(stritem.GetBuffer(0)).c_str() );

		CString varname = stritem.SpanExcluding(_T(":"));
		varname.Trim();
		sprintf( vkey.varName, "%s", Unicode2Ansi(varname.GetBuffer(0)).c_str() );
 	}
	// 注意：结构体中不能再有结构体的数据成员 parentItem != m_hItemVar[i]
	if ( vkey.varTyp == svINTEGER || vkey.varTyp == svBOOL || vkey.varTyp == svREAL || vkey.varTyp == svSTRING   )
	{
		vkey.first_members = false;
		vkey.propTyp = vkey.varTyp;

		GTV_HTREENODE parentItem = m_Grid.GetParentNode(currItem);
		if ( parentItem )
		{
 			dwData      = static_cast<int>( m_Grid.GetItemData( m_Grid.FindRowIndex(parentItem), 1 ) );
			vkey.varTyp = static_cast<ScriptVarT>( LOBYTE(dwData) );
			if ( vkey.varTyp < svAXISPOS || vkey.varTyp >= svTypeNum )
			{
				vkey.valid = false;
				return vkey;
			}

			if ( bNeedName )
			{
				CString stritem = m_Grid.GetTreeItemText(parentItem, 0);
				CString varname = stritem.SpanExcluding(_T(":"));
				varname.Trim();
				sprintf( vkey.propName, "%s", vkey.varName );
				sprintf( vkey.varName, "%s", Unicode2Ansi(varname.GetBuffer(0)).c_str() );
			}		
		}
		else
			vkey.valid = false;

	 
	}

//  	if ( currItem )// 注意：结构体中不能再有结构体的数据成员
//  	{
// 		GTV_HTREENODE ParentItem = m_Grid.GetParentNode(currItem); 
// 		GTV_HTREENODE TopItem    = m_Grid.GetParentNode(ParentItem);
// 		if ( ParentItem && TopItem )
// 		{
// 			for ( int i = 0; i < SVAR_SET_NUM; ++i )
// 			{
// 				if ( TopItem == m_hItemVar[i] )
// 				{
// 					vkey.is_struct = true;
// 					break;
// 				}
// 			}
//  		}
// 	}
    
// 	cout << " varKey = " << vkey.varSet << " : " << vkey.varTyp <<" : " << vkey.first_members << " : " << string(vkey.varName) 
// 		 << " : "  << vkey.valid << " : " << string(vkey.propName) << '\n';

	return vkey;
}

VariableKey VariableDialog::getVarKeyCurr( bool bNeedName /* = false */ )
{
	int row = m_Grid.GetFocusCell().row;
	return getVarKeyFromRow(row,bNeedName);
}

VariableKey VariableDialog::getVarKeyFromItem( GTV_HTREENODE  _item, bool bNeedName )
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

int VariableDialog::OnActivateChildWin( LPARAM lp )
{
	update_grid_data();
	return 1;
}
