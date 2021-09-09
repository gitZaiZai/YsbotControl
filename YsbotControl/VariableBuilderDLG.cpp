
#include "stdafx.h"
#include "YsbotControl.h"
#include "VariableBuilderDLG.h"
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
#include "variable_manager.h"
#include "UIDialogs/InfoBox.h"
#include <regex>

VariableBuilderDLG* variableBuilder_DLG;
// VariableBuilderDLG dialog
VariableBuilderDLG::VariableBuilderDLG(
	HBITMAP hBmpScrollBar, 
	HBITMAP hBmpPlus, 
	HBITMAP hBmpMinus ) : m_hBmpScrollBar(hBmpScrollBar),
	m_hBmpPlus(hBmpPlus), m_hBmpMinus(hBmpMinus)
{
	hItem_list.clear();
	joint_icon  = -1;
	motion_icon = -1;
	writeOnCtl = WriteNULL;
}

VariableBuilderDLG::~VariableBuilderDLG()
{
	hItem_list.clear();
	image_list_.DeleteImageList();

}

void VariableBuilderDLG::DoDataExchange(CDataExchange* pDX)
{
	;
}

BEGIN_MESSAGE_MAP(VariableBuilderDLG, DialogType)
	ON_NOTIFY( NM_DBLCLK, IDC_VariableBuilder_Grid, OnClickTree )
	ON_NOTIFY(GVN_BEGINLABELEDIT,IDC_VariableBuilder_Grid,  OnTreeBeginEdit)
	ON_NOTIFY(GVN_ENDLABELEDIT, IDC_VariableBuilder_Grid,   OnTreeEndEdit)
	ON_BN_CLICKED(IDC_VariableBuilder_BNOK,OnClickBtn)
	ON_EN_SETFOCUS(IDC_VariableBuilder_EDIT,OnClickEdit)
END_MESSAGE_MAP()


// VariableBuilderDLG message handlers
BOOL VariableBuilderDLG::OnInitDialog() 
{
	CDialogEx::OnInitDialog();
	SetWindowPos(NULL, g_x0 + 20, 95, 730, 439, SWP_NOZORDER);
	SetBackgroundColor(kColor[8]);
	// 	ModifyStyleEx(WS_EX_STATICEDGE, WS_EX_CLIENTEDGE, SWP_DRAWFRAME);
	// 创建图片列表
	image_list_.Create (22, 22, ILC_COLOR32 | ILC_MASK, 5, 1);
	joint_icon = image_list_.Add(AfxGetApp()->LoadIcon(IDI_AXIS_MOTOR));
	motion_icon = image_list_.Add(AfxGetApp()->LoadIcon(IDI_MOTION_SPEED));
	int overlay_icon = image_list_.Add(AfxGetApp()->LoadIcon(IDI_OVERLAY_TRANS));
	image_list_.SetOverlayImage(overlay_icon, 1);

	create_tree_grid();
	font_.CreateFontW(32, 0, 0, 0, 0, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
		OUT_STROKE_PRECIS, CLIP_STROKE_PRECIS, DRAFT_QUALITY, FIXED_PITCH |
		FF_MODERN, _T("Arial"));
	
	label_.Create(_T("变量名："),WS_CHILD|WS_VISIBLE,CRect(10,395,100,430),this);
	label_.SetFont(28);
	CRect editrect( 110, 390, 650, 430 );
	variableEdit_.Create( WS_CHILD | WS_VISIBLE | WS_BORDER, editrect, this, 
		IDC_VariableBuilder_EDIT, TRUE ); // | WS_TABSTOP
	variableEdit_.SetFont(&font_);
	variableEdit_.SetRulePlan(KBRule_Program);
	btn_ok.Create(_T("创建"),WS_CHILD|WS_VISIBLE|BS_BOTTOM|BS_CENTER|BS_VCENTER,
		CRect(660,390,720,430),this,IDC_VariableBuilder_BNOK);
	btn_ok.SetFont(&font_);
	return TRUE;
}

LRESULT VariableBuilderDLG::OnResizeGrid(WPARAM wp,LPARAM lp)
{
	static UINT lastType = 0;
	static UINT i = 0;
	UINT nType = UINT(wp);
	// re-size all pages
// 	if ( nType == 1 )
// 	{
// 		if ( lastType != nType )
// 		{
// 			m_Grid.SetColumnWidth(1,250);
// 			m_Grid.ExpandColumnsToFit(FALSE);
// 			m_Grid.Invalidate();
// 			lastType = nType;
// 			printf(" N %d dialog on size message %d\n",i++,nType);
// 		}
// 	}
// 	else
// 	{
// 		if ( lastType != nType )
// 		{
// 			m_Grid.ExpandColumnsToFit(TRUE);
// 			lastType = nType;
// 			printf(" N %d dialog on size message %d\n",i++,nType);
// 		}
// 	}
	if ( nType == 1 )
	{
		if ( lastType != nType )
		{
			m_Grid.ExpandColumn(1,250,FALSE);
			lastType = nType;
			printf(" N %d dialog on size message %d\n",i++,nType);
		}
	}
	else
	{
		if ( lastType != nType )
		{
			m_Grid.ExpandColumn(1,250,TRUE);
			lastType = nType;
			printf(" N %d dialog on size message %d\n",i++,nType);
		}
	}
	return 1;
}

LRESULT VariableBuilderDLG::OnMainButtonClicked( WPARAM wp, LPARAM lp)
{
	int btnid  = (int)(wp);
	int menuid = (int)(lp);
	CString text;
	CString name;
	PosInfo pi;
	switch( btnid )
	{
	case 0:		//	
		break;
	case 1:		//	
		break;
	case 2:		//	
		break;
	case 3:		//
		break;
	case 4:		//	取消
		ShowWindow(SW_HIDE);
		GetParent()->SendMessage(WM_ACTIVATE_WINDOW,winInstructionEdit);
		break;
	case 5:		//	确定
		if(host_)
			host_->SendMessage(WM_VariableBuilder_NewName,WPARAM(&variable_text),varRow_);
		ShowWindow(SW_HIDE);
		GetParent()->SendMessage(WM_ACTIVATE_WINDOW,winInstructionEdit);
		break;
	default: break;
	}

	return 1;
}

void VariableBuilderDLG::OnClickBtn()
{
	variableEdit_.GetWindowTextW(variable_text);
	if (variable_text ==_T("")) return;
	curVarName = Unicode2Ansi(variable_text.GetBuffer(0)).c_str();
	std::string re = "|([a-z]|[A-Z])\\w{0,19}";
	if (!CheckRule(curVarName,re))
	{
		MSD.ShowInfo(_T("变量名称不合法"),_T("提示"),this,5);
		return;
	}
	BOOL isRedefined = FALSE;
	const std::map<ScriptVarT, std::map<std::string, void*>>* var_list = LVAR->get_var_list();
	for( map<ScriptVarT,  map<string, void*>>::const_iterator it = var_list->begin(); 
		it != var_list->end(); 
		it++ ) 
	{
		for ( map<string, void*>::const_iterator innerIt = it->second.begin(); 
			innerIt != it->second.end(); 
			innerIt++ ) 
		{
			if (strcmp(innerIt->first.c_str(),curVarName.c_str()) == 0)
			{ 
				isRedefined=TRUE;
				break;
			}
		}
		if(isRedefined) break;
	}
	if (isRedefined)
	{
		MSD.ShowInfo(_T("变量名称已存在，请重新输入"),_T("提示"),this,5);
		return;
	}
	std::string svt = LVAR->get_type_name(curVarT);
	variable_dialog->new_variable_ui(curVarT,curVarName.c_str());
	CString str = m_Grid.GetItemText(1,0);
	str = str.Right(str.GetLength() - str.Find(_T(":")));
	str = variable_text + str;
	m_Grid.SetCellText(1,0,str);
	while(m_Grid.GetItemCount()>1)
	{
		m_Grid.DeleteItem(m_Grid.GetItemCount()-1);
	}
	void* varv = LVAR->get_value_ptr(curVarT,curVarName);
	InsertVariable(VARINFO->vardesc[curVarT]->getClassName(),curVarName.c_str(),varv);

	m_Grid.Invalidate();
}

BOOL VariableBuilderDLG::create_tree_grid() throw (InvalidConfigurationException)
{
	BOOL ret = m_Grid.Create(
		CRect(0, 0, 730, 380), 
		this,
		IDC_VariableBuilder_Grid, WS_VISIBLE | WS_CHILD,
		m_hBmpScrollBar, m_hBmpPlus, m_hBmpMinus, SKIN_WINDOW_VARIABLE, 35 );

	if ( ret )
	{
		//m_Grid.SetImageList(&image_list_);
		m_Grid.SetColumnResize(FALSE);
		m_Grid.SetRowResize(FALSE);
		// 		m_Grid.EnableSmoothScroll(TRUE);

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

		m_Grid.SetEditable(FALSE); // read only set
		m_Grid.SetListMode(TRUE);

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

BOOL VariableBuilderDLG::initialize_tree_grid()
{
	UpdateData();

	m_Grid.SetAutoSizeStyle();

	int m_nFixCols = 0;
	int m_nFixRows = 1;
	m_Grid.SetFixedRowCount(m_nFixRows);
	m_Grid.SetFixedColumnCount(m_nFixCols);
	m_Grid.SetDefCellHeight(38);
	m_Grid.SetRedraw(FALSE);

	m_Grid.InsertColumn(_T("变量名"),DT_LEFT|DT_TOP);
	m_Grid.InsertColumn(_T("变量值"),DT_LEFT|DT_TOP);

	m_Grid.SetColumnWidth(0, 400);
	m_Grid.SetColumnWidth(1, 250);
	m_Grid.ExpandColumnsToFit(); // liurundan
	m_Grid.SetRedraw(TRUE);
	m_Grid.SetTreeColumnCellTypeID(CT_DEFAULT);	

	GV_ITEM Item;
	Item.mask = GVIF_TEXT | GVIF_IMAGE | GVIF_FORMAT | GVIF_STATE;
	Item.nFormat = DT_LEFT | DT_BOTTOM;
	Item.nState  = GVIS_READONLY;

	// Write extra codes for setting up the treelistctrl for sorting
	m_Grid.SetHeaderSort(FALSE);
	// Set the default compare function for all rows
	m_Grid.SetCompareFunction(CGridCtrl::pfnCellTextCompare);

	UpdateData(FALSE);

	return TRUE;
}

void VariableBuilderDLG::OnTreeBeginEdit(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*) pNotifyStruct;
	printf(("dan Start Edit on row %d, col %d\n"), pItem->iRow, pItem->iColumn);
	*pResult = 0;  
}

void VariableBuilderDLG::OnTreeEndEdit(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*) pNotifyStruct;
	//printf(("dan End Edit on row %d, col %d\n"), pItem->iRow, pItem->iColumn);
	*pResult = 0; // assign -1 here to reject edit changes

	// retrieve text of the edited cell
	if ( pItem->iColumn == 1 )
	{
		GTV_HTREENODE currItem   = m_Grid.GetTreeNode(pItem->iRow);
		GTV_HTREENODE ParentItem = m_Grid.GetParentNode(currItem);
		if ( ParentItem == NULL || currItem == NULL )
			return;

		CString strparent = m_Grid.GetTreeItemText(ParentItem,0);
		CString strval    = m_Grid.GetTreeItemText(currItem,0); // 获得节点值

		CGridCellBase *pCell = m_Grid.GetCell(pItem->iRow, pItem->iColumn); 
		if ( !pCell)
			return;
		CString val;
		if ( pCell->GetRuntimeClass() == RUNTIME_CLASS(CGridCellCombo)  )  // liurundan
		{
			val = m_Grid.GetItemText( pItem->iRow, pItem->iColumn );

		}
		else if ( pCell->GetRuntimeClass() == RUNTIME_CLASS(CGridCellCheck) )
		{
			CGridCellCheck *pCell = (CGridCellCheck*) m_Grid.GetCell(pItem->iRow, 1);
			val = pCell->GetCheck() ? "true" : "false";
			var_read_writer->write_variable( 
				Unicode2Ansi(strparent.GetBuffer(0)).c_str(), 
				Unicode2Ansi(strval.GetBuffer(0)).c_str(),
				val ? "true" : "false",false );	
		}
	}
}

LRESULT VariableBuilderDLG::OnKeyboardEnter( WPARAM wp, LPARAM lp )
{
	if(writeOnCtl != WriteOnTree) return 1;
	CString str_result = *((CString *)wp);
	CCellID cell = *(CCellID*)(lp);
	int row = cell.row;
	int col = cell.col;
	GTV_HTREENODE currItem   = m_Grid.GetTreeNode(row);
	GTV_HTREENODE ParentItem = m_Grid.GetParentNode(currItem);
	if ( ParentItem == NULL || currItem == NULL || m_Grid.NodeHasChildren(currItem) == TRUE )
		return 0;

	CRect				screenRect;
	CString				text;

	m_Grid.GetCellRect(row,1,&screenRect);
	OffsetRect(&screenRect,-550,0);
	m_Grid.ClientToScreen(&screenRect);

	text = m_Grid.GetTreeItemText(currItem,1);
	bool valid = true;

	CString strparent = m_Grid.GetTreeItemText(ParentItem,0);
	CString strval    = m_Grid.GetTreeItemText(currItem,0); // 获得节点值
	valid = var_read_writer->write_variable( 
		Unicode2Ansi(strparent.GetBuffer(0)).c_str(), 
		Unicode2Ansi(strval.GetBuffer(0)).c_str(),
		Unicode2Ansi(str_result.GetBuffer(0)).c_str(),
		false );
	// Is the value valid?
	if ( valid )
	{
		m_Grid.SetTreeItemText(currItem, 1, str_result);
		m_Grid.Invalidate();

	}
	writeOnCtl = WriteNULL;
	return 1;
}

void VariableBuilderDLG::OnClickTree(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*) pNMHDR;
	int nRow = pItem->iRow;
	int nCol = pItem->iColumn;
	TRACE(_T("dan Right button click on row %d, col %d\n"), pItem->iRow, pItem->iColumn);

	if ( nCol == 1 )
	{
		CGridCellBase *pCell = m_Grid.GetCell(nRow, 1); 
		if ( !pCell )
		{
			return;
		}

		if ( pCell->GetRuntimeClass() == RUNTIME_CLASS(CGridCellCombo) ||
			pCell->GetRuntimeClass() == RUNTIME_CLASS(CGridCellCheck) )  // liurundan
		{
			return;
		}
		parse_item_string(nRow);
	}
	else
		*pResult = 0;

}

void VariableBuilderDLG::parse_item_string( int row )
{
	// modify variable directly.
	writeOnCtl = WriteOnTree;
	CString datastr  = m_Grid.GetItemText(row,1);
	CString datatype = m_Grid.GetItemText(row,0);

	Singleton<DigitalKeyboard>* digital_keyboard_ = Singleton<DigitalKeyboard>::Instance();
	Singleton<FullQwerty>*      full_keyboard_    = Singleton<FullQwerty>::Instance();
	 
	m_CellEdit.row = row;
	m_CellEdit.col = m_Grid.GetFocusCell().col;
	if ( datatype.Find(_T("INT")) != -1 )
	{
		digital_keyboard_->activate_keyboard(GetSafeHwnd(),datastr,datatype,(LPARAM)&m_CellEdit);
		digital_keyboard_->SetValueRange(-100000000, 100000000, 0);
	}
	else if ( datatype.Find(_T("REAL")) != -1 )
	{
		digital_keyboard_->activate_keyboard(GetSafeHwnd(),datastr,datatype,(LPARAM)&m_CellEdit);
		digital_keyboard_->SetValueRange(-1000000, 1000000, 3);
	}
	else if ( datatype.Find(_T("BOOL")) != -1 )
	{
		full_keyboard_->activate_keyboard(GetSafeHwnd(),datastr,datatype,(LPARAM)&m_CellEdit);
	}
	else if ( datatype.Find(_T("STRING")) != -1 )
	{
		full_keyboard_->activate_keyboard(GetSafeHwnd(),datastr,datatype,(LPARAM)&m_CellEdit);
	}

}

void VariableBuilderDLG::OnClickEdit()
{
	writeOnCtl = WriteOnEdit;
}

void VariableBuilderDLG::SetVariable(ScriptVarT sv, std::string varName, void* pValue, CWnd* hostPtr, int nRow)
{
	//m_Grid.DeleteNonFixedRows();
	host_ = hostPtr;
	varRow_ = nRow;
	curVarT = sv;
	curVarName = varName;
	curValuePtr = pValue;
	if(varName=="" || pValue==NULL)
	{
		variableEdit_.EnableWindow(TRUE);
		variableEdit_.SetWindowTextW(_T(""));
		btn_ok.ShowWindow(TRUE);
	}
	else
	{
		variableEdit_.EnableWindow(FALSE);
		variableEdit_.SetWindowTextW(CString(varName.c_str()));
		btn_ok.ShowWindow(FALSE);		
	}
	while(m_Grid.GetItemCount()>1)
	{
		m_Grid.DeleteItem(m_Grid.GetItemCount()-1);
	}
	InsertVariable(VARINFO->vardesc[sv]->getClassName(),varName.c_str(),pValue);
}

void VariableBuilderDLG::InsertVariable( const char* vartype, const char* vname, void* ptrvalue ) 
{
	ScriptVarT sv = LVAR->get_variable_type(vartype);
	std::string varinfo = std::string(vname) + ": " + std::string(vartype);

	GTV_HTREENODE namehItem = m_Grid.InsertItem( CString(varinfo.c_str()) );
	if ( namehItem != NULL )
	{
		int nRow = m_Grid.FindRowIndex(namehItem);
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

		if ( sv == svBOOL )
		{
			m_Grid.SetItemState(nRow,1,m_Grid.GetItemState(nRow, 1) & ~GVIS_READONLY);	
			m_Grid.SetCellType(nRow, 1, RUNTIME_CLASS(CGridCellCheck));
		}
		else
			insert_varprop_item(namehItem, sv);
		update_tree( namehItem, sv, ptrvalue );
		//m_Grid.CollapseTreeNode(namehItem);
	}

}

void VariableBuilderDLG::insert_varprop_item(GTV_HTREENODE item, ScriptVarT sv)
{
	char prop[VAR_STR_LEN];

	GV_ITEM propItem;
	propItem.mask = GVIF_TEXT|GVIF_FORMAT;
	propItem.nFormat = DT_LEFT | DT_VCENTER;

	GTV_HTREENODE childItem = NULL;
	int nRow = -1;
	for ( int i = 0; i < VARINFO->vardesc[sv]->getValuesTotal(); i++ )
	{
		VARINFO->vardesc[sv]->getValue(i+1,prop);
		propItem.strText = CString(prop);

		childItem = m_Grid.InsertItem( &propItem, item );
		nRow = m_Grid.FindRowIndex(childItem);
		m_Grid.SetItemFormat(nRow,1,DT_RIGHT);
		m_Grid.SetItemState(nRow,0,GVIS_READONLY);
		m_Grid.SetItemState(nRow,1,GVIS_READONLY);

		if ( strstr(prop,": BOOL") ) // 
		{
			m_Grid.SetItemState(nRow,1,m_Grid.GetItemState(nRow, 1) & ~GVIS_READONLY);	
			m_Grid.SetCellType(nRow, 1, RUNTIME_CLASS(CGridCellCheck));
		}
	}

}

void VariableBuilderDLG::update_tree(GTV_HTREENODE varitem, ScriptVarT sv, void* valueptr)
{
	if ( valueptr == NULL )
		return;

	CString strtemp;
	int nRow = -1;
	std::vector<std::string> valuelist;
	var_read_writer->read_variable(sv,valueptr,valuelist);

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
void VariableBuilderDLG::update_check_box(GTV_HTREENODE hitem, BOOL b)
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

BOOL VariableBuilderDLG::CheckRule(CString str,std::string rule)
{
	std::string s;
	s = Unicode2Ansi(std::wstring(str)).c_str();
	return CheckRule(s,rule); 
}
BOOL VariableBuilderDLG::CheckRule(std::string str,std::string rule)
{
	std::regex re(rule);
	return regex_match(str,re);
}