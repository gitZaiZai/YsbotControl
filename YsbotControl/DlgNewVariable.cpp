#include "StdAfx.h"
#include "DlgNewVariable.h"
#include "LuaScript/ScriptVariables.h"
#include "colourcode.h"
#include "YsbotControl.h"
#include "variable_manager.h"
#include "stringconvert.h"
#include "UIDialogs/state_dialog.h"

#define IDC_EDIT					10001

BEGIN_MESSAGE_MAP(DlgNewVariable, DialogType)
	ON_NOTIFY( GVN_SELCHANGED, IDL_VARIABLE_KIND, &DlgNewVariable::OnGridEndSelChange )
	ON_NOTIFY( GVN_SELCHANGED, IDL_VARIABLE_TYPE, &DlgNewVariable::OnRightGridEndSelChange )
	ON_WM_PAINT()
	ON_CBN_SELCHANGE(NEW_VARIABLE_COMBOBOX_TYPE, &DlgNewVariable::OnSelChangeCombo)
 
END_MESSAGE_MAP()

DlgNewVariable::DlgNewVariable(HBITMAP hBmpScrollBar) : m_hBmpScrollBar(hBmpScrollBar),m_bInitWindow(false),
	m_nWinCaller(winVariableData)
{
	grid_left  = NULL;
	grid_right = NULL;
	m_last_row = -1;
 	m_variable_index = 0;
	m_var_abbreviate = _T("");
	m_vkeyResult = new RegParseResult(true);
	m_vkeyResult->nFromID = 1;
}


DlgNewVariable::~DlgNewVariable(void)
{
	image_list_.DeleteImageList();
	delete grid_left;
	delete grid_right;

	m_vkeyResult->clear();
 	delete m_vkeyResult;
}

void DlgNewVariable::DoDataExchange(CDataExchange* pDX) 
{
	CDialog::DoDataExchange(pDX);
}

BOOL DlgNewVariable::OnInitDialog() 
{
	DialogType::OnInitDialog();
	SetWindowPos(NULL, g_x0 + 20, 95, 730, 439, SWP_NOZORDER);
	SetBackgroundColor(kColor[8]);
	// 	ModifyStyleEx(WS_EX_STATICEDGE, WS_EX_CLIENTEDGE, SWP_DRAWFRAME);

	// 创建图片列表
	image_list_.Create (24, 24, ILC_COLOR32 | ILC_MASK, 5, 1);
	empty_icon    = image_list_.Add(AfxGetApp()->LoadIcon(IDI_RADIO_EMPTY));
	selected_icon = image_list_.Add(AfxGetApp()->LoadIcon(IDI_RADIO_SELECTED));
 	int overlay_icon = image_list_.Add(AfxGetApp()->LoadIcon(IDI_OVERLAY_TRANS));
	image_list_.SetOverlayImage(overlay_icon, 1);

	CRect clientrect;
	GetClientRect(&clientrect);

	CRect rectTitleGrid[2];
	rectTitleGrid[0] = rectTitleGrid[1] = CRect(0, 0, clientrect.Width()/2, 315 );
	OffsetRect(&rectTitleGrid[1],rectTitleGrid[0].Width(),0);
	 
	rectTitleGrid[0].bottom = rectTitleGrid[1].bottom = clientrect.top + 34;
	rectTitleGrid[0].right -= 1;
	rectTitleGrid[0].OffsetRect(1,0);
	rectTitleGrid[1].OffsetRect(2,0);

	CRect leftrect(0, 34, (int)(clientrect.Width()/2+0.5), 330 );
	CRect rightrect = leftrect;
	OffsetRect(&rightrect,leftrect.Width()+4,0);

 	m_bkCol  = RGB(248, 252, 248);
	m_HighLightColDisable = RGB(216, 220, 216);
	m_HighLightColEnable  = RGB(88,148,24);

  	creat_grid(leftrect,IDL_VARIABLE_KIND);
	creat_grid(rightrect,IDL_VARIABLE_TYPE);
 
  	m_last_row = 0;

	CRect descrect( 2, leftrect.bottom + 10, (int)(clientrect.Width()*0.55),leftrect.bottom+55);
	CRect namescopeRect( descrect.right + 5, descrect.top + 2, descrect.right + 5 + 80, descrect.bottom - 2 );
	CRect descscopeRect( namescopeRect.right + 5, namescopeRect.top, rightrect.right-5, namescopeRect.bottom );
	CRect namerect( 0, descrect.bottom + 5, 40, descrect.bottom + 50 );
	CRect editrect( namerect.right+10, namerect.top+8, descrect.right, namerect.bottom-8 );

	m_edit.Create( WS_CHILD | WS_VISIBLE | WS_BORDER, editrect, this, IDC_EDIT, TRUE ); // | WS_TABSTOP
  	update_edit(0,0);
	
	COLORREF m_crBackgroud = RGB(220, 220, 220);
	COLORREF m_crText      = RGB(88, 92, 88);

	LOGFONT lf;
	memset(&lf, 0, sizeof(LOGFONT));
	lf.lfHeight = 20;
	lf.lfWeight = 700;
	lf.lfWidth  = 8;
	wcsncpy_s(lf.lfFaceName, _T("Arial"), LF_FACESIZE-1);

	desc_label.Create( _T(""), WS_CHILD | WS_VISIBLE | DT_LEFT | DT_TOP,
		descrect, this );
	desc_label.SetBkColor(m_crBackgroud)
		.SetTextColor(m_crText).SetFont(lf)
		.SetBorder(TRUE)
		.SetFontBold(TRUE)
		.SetText(_T(" 变量描述"));
  
	name_label.Create( _T(""), WS_CHILD | WS_VISIBLE | DT_LEFT | SS_CENTERIMAGE,
		namerect, this );
	name_label.SetBkColor(m_crBackgroud)
		.SetTextColor(RGB(0,0,0)).SetFont(lf)
 		.SetText(_T(" 名称:"));

	name_scope.Create( _T(""), WS_CHILD | WS_VISIBLE | DT_LEFT | SS_CENTERIMAGE,
		namescopeRect, this );
	name_scope.SetBkColor(m_crBackgroud)
		.SetTextColor(RGB(0,0,0)).SetFont(lf) 
		.SetText(_T(" 作用范围:"));
	
// 	desc_scope.Create( _T(""), WS_CHILD | WS_VISIBLE | DT_LEFT | SS_CENTERIMAGE,
// 		descscopeRect, this );
// 	desc_scope.SetBkColor(m_crBackgroud)
// 		.SetTextColor(m_crText).SetFont(lf)
// 		.SetBorder(TRUE)
// 		.SetFontBold(TRUE)
// 		.SetText(_T(" 程序"));
  
	init_box_grid(descscopeRect);

 	lf.lfWidth = 0;
  
	COLORREF m_crTitleBackgroud = RGB(224, 228, 224);

	for ( int i = 0; i < 2; ++i )
	{
		title_grid[i].Create( _T(""), WS_CHILD | WS_VISIBLE | DT_LEFT | SS_CENTERIMAGE,
			rectTitleGrid[i], this );
		title_grid[i].SetBkColor(m_crTitleBackgroud )
			.SetTextColor(m_crText)
			.SetBorder(TRUE)
			.SetFontBold(FALSE).SetFont(lf)
			.SetText(i==0?_T(" 分类"):_T(" 类型"));
	}
 
	return FALSE;
}

void DlgNewVariable::init_window()
{
 	if ( grid_left && grid_left->GetRowCount() == 0 )
	{
		GV_ITEM Item;
		Item.mask    = GVIF_TEXT | GVIF_IMAGE | GVIF_FORMAT | GVIF_STATE;
		Item.nFormat = DT_LEFT | DT_BOTTOM;
		Item.nState  = GVIS_READONLY;
		Item.row = 0;
		Item.col = 0;
		std::string categoryname;
		int row_height = 45;

		for ( int i = 0; i < CATEGORY_NUM; i++ )
		{
			grid_left->InsertRow(_T(""),i);
			grid_left->SetRowHeight(i,row_height);
			categoryname = VARINFO->var_category[i]->getClassName();
			Item.strText = CString(categoryname.c_str());
			Item.row     = i;
			Item.col     = 0;
			Item.iImage  = empty_icon;
			grid_left->SetItem( &Item );
		}
		grid_left->SetSelectedRange(0,0,0,0,TRUE,TRUE);
		grid_left->SetItemImage(0,0,selected_icon);
		grid_left->SetFocusCell(0,0);

		update_window(0);
		desc_label.SetText( _T(" 整型"), TRUE);
	}
}

CGridCtrl* DlgNewVariable::creat_grid( CRect rect,  unsigned int gridID )
{
	CGridCtrl* pGrid = NULL;

	pGrid = new CGridCtrl();

	BOOL ret = FALSE;
	int row_height = 38;
	if ( gridID == IDL_VARIABLE_KIND )
	{
		ret = pGrid->Create(
			rect, 
			this,
			gridID,WS_CHILD | WS_VISIBLE,
			NULL, NULL, NULL, SKIN_WINDOW_VARIABLE );
		row_height = 45;
		grid_left = pGrid;
    }
	else
	{
		ret = pGrid->Create(
			rect, 
			this,
			gridID, WS_CHILD | WS_VISIBLE,
			m_hBmpScrollBar, NULL, NULL, SKIN_WINDOW_VARIABLE,
			0 );
 		    pGrid->SetShowSrollBar(SB_HORZ,FALSE);
		grid_right = pGrid;
  	}

	if ( !ret )
		return NULL;
	pGrid->InsertColumn(_T(""),DT_LEFT|DT_TOP,0);

	GV_ITEM Item;
	Item.mask    = GVIF_TEXT | GVIF_IMAGE | GVIF_FORMAT | GVIF_STATE;
	Item.nFormat = DT_LEFT | DT_BOTTOM;
	Item.nState  = GVIS_READONLY;
	Item.row = 0;
	Item.col = 0;

	if ( pGrid->GetRowCount() > 0 )
	{
		pGrid->DeleteNonFixedRows();
	}
 
	pGrid->SetImageList(&image_list_);
	pGrid->SetColumnResize(FALSE);
	pGrid->SetRowResize(FALSE);
 
	pGrid->EnableDragAndDrop(FALSE);
	pGrid->SetBkColor(m_bkCol);
 	pGrid->GetDefaultCell(FALSE, FALSE)->SetBackClr(m_bkCol); // 0XE0

	LOGFONT lf;
	memset(&lf, 0, sizeof(LOGFONT));
	lf.lfHeight = 24;
	wcsncpy_s(lf.lfFaceName, _T("Arial"), LF_FACESIZE-1);
	CFont font;
	font.CreateFontIndirect(&lf);
	pGrid->SendMessage(WM_SETFONT,(WPARAM)(font.GetSafeHandle()));
	font.DeleteObject();

	pGrid->SetEditable(FALSE);
	pGrid->SetListMode(TRUE);

  	pGrid->SetGridLines(GVL_NONE);

 	pGrid->SetSingleRowSelection(TRUE);
  	pGrid->SetAutoSizeStyle();
  
	pGrid->SetDefCellHeight(row_height);
	pGrid->SetRedraw(FALSE);
  
	pGrid->SetColumnWidth(0, 200);
 
 	pGrid->ExpandColumnsToFit(); // liurundan
 
	pGrid->SetRedraw(TRUE);
	pGrid->EnableDragAndDrop(FALSE);
    pGrid->SetSelectedRange(0,0,0,0,TRUE,TRUE);

	pGrid->SetFocusCell(0,0);
 
	return pGrid;
}

void DlgNewVariable::update_right_grid( ScriptVarCategory svc  )
{
	if ( svc == CATEGORY_NUM  || svc < BASIC_KIND )
 		return;

	if ( grid_right->GetRowCount() > 0 )
 		grid_right->DeleteNonFixedRows();
     
 	int type_num = VARINFO->var_category[svc]->getValuesTotal();
	char prop[VAR_STR_LEN];
	std::deque<std::string> strlist;

	for ( int i = 0; i < type_num; ++i )
	{
		VARINFO->var_category[svc]->getValue(i+1,prop);
		split_string( strlist, prop, VARINFO->getSeparatorStr() );
		int varKind = VARINFO->var_category[svc]->getValueKind(i+1);

		if ( strlist.size() == 3)
		{
			grid_right->InsertRow( CString( strlist[0].c_str() ), i );
			grid_right->SetRowHeight(i,38);
 		}	
		grid_right->SetItemData(i,0,varKind);
	}
	strlist.clear();

	grid_right->SetFocusCell( 0, 0 );
	grid_right->SetSelectedRange(0,0,0,0,FALSE,TRUE);
	grid_right->Invalidate();

}

LRESULT DlgNewVariable::OnMainButtonClicked( WPARAM wp, LPARAM lp)
{
	unsigned int btnid = (unsigned int)(wp);
	int nrow        = grid_right->GetFocusCell().row;
	int varType     = static_cast<int>( grid_right->GetItemData(nrow,0) );
	 
	CString strname;
	m_edit.GetWindowText(strname);
	int currid = extract_int(strname);
    bool bSuccess = false;
	switch(btnid)
	{
	case 0: break;
	case 1: break;
	case 2: break;
	case 3: break;
	case 4:  
		{
// 			GetParent()->SendMessage(WM_ACTIVATE_WINDOW, m_nWinCaller, 0 );      // return last window.
			GetParent()->SendMessage(WM_ACTIVATE_WINDOW, m_nWinCaller, 0 );      // return last window.

 		}
        break; // cancle
	case 5: 
		{
			if ( varType == -1 )
			{
				return 0;
			}	

			int nrow = m_ComboVarSet.GetCurSel();
 			DWORD data = (DWORD)m_ComboVarSet.GetItemData(nrow);
 
			bSuccess = variable_dialog->new_variable_ui( static_cast<ScriptVarT>(varType),
														 Unicode2Ansi(strname.GetBuffer(0)).c_str(),
														 (ScriptVarSetT)(HIBYTE(data)),
														 (m_nWinCaller == winInstructionEdit) ); 
 
			if ( bSuccess )
			{
 				vector<VariableKey>::iterator vit = m_vkeyResult->params.begin();
				if ( vit != m_vkeyResult->params.end() )
				{
					sprintf( vit->varName, "%s", Unicode2Ansi(strname.GetBuffer(0)).c_str() ); 
					vit->varSet =  (ScriptVarSetT)(HIBYTE(data));
 					vit->varTyp =  static_cast<ScriptVarT>(varType);
				}
   
				m_variable_index = ++currid;
				CString strNextID;
				strNextID.Format(_T("%d"),m_variable_index);
				m_edit.SetText(m_var_abbreviate+strNextID);
 				 
  				GetParent()->SendMessage(WM_ACTIVATE_WINDOW, m_nWinCaller, m_nWinCaller == winInstructionEdit ? (LPARAM)m_vkeyResult : 0 );      // return last window.

  			}
			else
			{
			    MSD.ShowInfo(_T("变量名已存在，请指定其他变量名"),2);
			}
 		}
		break;
 	default: break;
	}
	cout << "New Variable = " << varType << '\n';

	return 1;
}

void DlgNewVariable::reset_var_index(int id)
{
	 m_variable_index = id;
	 CString strNextID;
	 strNextID.Format(_T("%d"),id);
	 m_edit.SetText(m_var_abbreviate+strNextID);
}

//将string类对象中的数字字符提取出来
int DlgNewVariable::extract_int(CString str)
{
	int length;
	int i;
	TCHAR temp2[VAR_STR_LEN];
	lstrcpy(temp2,str);    //copies a string to a buffer

	TCHAR temp[VAR_STR_LEN];
	int count;
	count=0;
	length=_tcslen(temp2);    //length保存了字符数组temp2的长度
	for(i=0;i<length;i++)
	{
		if ((temp2[i]<='9')&&(temp2[i]>='0')) 
		{
			temp[count]=temp2[i];
			count++;
		}
	}
	return (::atoi( Unicode2Ansi(temp).c_str() ) );
}

void DlgNewVariable::update_window(int nRow)
{
 	update_right_grid( ScriptVarCategory(nRow) );
 
	update_edit( nRow, 0);

}
 
void DlgNewVariable::OnGridEndSelChange(NMHDR* pNMHDR, LRESULT* pResult)
{
  	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*) pNMHDR;
	int nRow = pItem->iRow;
 
	if ( nRow < 0 || nRow == CATEGORY_NUM )
		return;
 
	if ( nRow != m_last_row )
	{
		bool bChangeFocusByMouse = false;
		int idCurrIcon = grid_left->GetItemImage(nRow,0);
		int idLastIcon = grid_left->GetItemImage(m_last_row,0);
		if ( idCurrIcon == empty_icon )
		{
			grid_left->SetItemImage( nRow, 0, selected_icon );
			bChangeFocusByMouse = true;
		}
		if ( idLastIcon == selected_icon )
		{
			grid_left->SetItemImage( m_last_row, 0, empty_icon );
			bChangeFocusByMouse = true;
		}

		if ( bChangeFocusByMouse )
		{
			grid_left->Invalidate();
		}		
		update_window( nRow );
		m_last_row = nRow;
	}	
// 	cout << "curr row = " << nRow << '\n';  
}
  
void DlgNewVariable::OnRightGridEndSelChange(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*) pNMHDR;
	int nRow = pItem->iRow;
	int nCol = pItem->iColumn;
	int nRowLeftGrid = grid_left->GetFocusCell().row;
	if ( nRowLeftGrid < 0 || nRow < 0 || nRowLeftGrid >= (int)(CATEGORY_NUM) )
		return;
  
	update_edit( nRowLeftGrid, nRow );

}

void DlgNewVariable::update_edit(int leftRow, int rightRow)
{
	int type_num = VARINFO->var_category[leftRow]->getValuesTotal();
	if ( rightRow > type_num )
		return;

	char prop[VAR_STR_LEN];
	std::deque<std::string> strlist;

	VARINFO->var_category[leftRow]->getValue(rightRow+1,prop);
	split_string( strlist, prop, ":" );
 	 
	if ( strlist.size() == 3 )
	{
		if ( m_edit.GetSafeHwnd() )
		{
			CString strId;
			strId.Format(_T("%d"),m_variable_index);
			m_edit.SetText( CString(strlist[2].c_str())+strId );
 			m_edit.SetSel(-1);
			m_var_abbreviate = CString(strlist[2].c_str());
		}

		// variable description in edit control.
		desc_label.SetText( _T(" ") + CString(strlist[1].c_str()), FALSE);
	}		 
}
 
void DlgNewVariable::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call DialogType::OnPaint() for painting messages
 	if ( !m_bInitWindow )
	{
		m_bInitWindow = true;
		init_window();
		cout << "DlgNewVariable on paint\n";
	}
}

void DlgNewVariable::init_box_grid(CRect& rcPos)
{
	CRect rcCombobox = rcPos;
	m_ComboVarSet.Create( WS_CHILD | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED |CBS_HASSTRINGS,rcPos, this, NEW_VARIABLE_COMBOBOX_TYPE);
	m_ComboVarSet.SetAlignTop(TRUE);
	m_ComboVarSet.set_custom_message(FALSE); 
	m_ComboVarSet.AddString(_T("全局"),MAKEWORD(0,1)); // CBS_SORT
	m_ComboVarSet.AddString(_T("程序"),MAKEWORD(0,2));
	m_ComboVarSet.SetCurSel(0);
}

void DlgNewVariable::OnSelChangeCombo()
{
	CString msg;
	int nrow = m_ComboVarSet.GetCurSel();
	
	CString fmsg;
	fmsg.Format(_T(" sel = %d, data = %d \n"), nrow, (DWORD)m_ComboVarSet.GetItemData(nrow) );
	TRACE(fmsg);
}

int DlgNewVariable::OnActivateChildWin( LPARAM lp )
{
	if ( !m_bInitWindow )
	{
		m_bInitWindow = true;
		init_window();
 	}	

	if ( lp == NULL )
	{
		grid_left->EnableWindow(TRUE);
		grid_right->EnableWindow(TRUE);
		grid_left->SetHighLightBackColour(m_HighLightColEnable);
		grid_right->SetHighLightBackColour(m_HighLightColEnable);

 		m_nWinCaller = winVariableData;
		return 0;
	}
 

	cout << " new variable... \n";
	VariableKey vkeyFromEdit = *(VariableKey*)(lp);
	
	ScriptVarT varTyp = vkeyFromEdit.varTyp;
	ScriptVarT tmpTyp;
	int i, j;
	bool bFind = false;
	for ( i=0; i < CATEGORY_NUM; ++i)
	{
 		for ( j = 0; j < VARINFO->var_category[i]->getValuesTotal(); ++j )
		{
			tmpTyp = (ScriptVarT) VARINFO->var_category[i]->getValueKind(j+1);
			
			if ( varTyp == tmpTyp )
			{
				bFind = true;
				break;
			}
		}

		if ( bFind )
 			break;
 	}

	ScriptVarCategory svc = (ScriptVarCategory)(i);
 
	if ( bFind )
	{
		NM_GRIDVIEW pNotifyStruct;
		pNotifyStruct.iRow = i;
		pNotifyStruct.iColumn = 0;
		OnGridEndSelChange( (NMHDR*)&pNotifyStruct, NULL );

		grid_left->SetFocusCell(i,0); 
		grid_left->SetSelectedRange(i, grid_left->GetFixedColumnCount(), i, grid_left->GetColumnCount()-1, TRUE);

		grid_right->SetFocusCell(j,0); 
		grid_right->SetSelectedRange(j, grid_left->GetFixedColumnCount(), j, grid_left->GetColumnCount()-1, TRUE);
		update_edit(i, j);
	    
		m_nWinCaller = winInstructionEdit;
		grid_left->EnableWindow(FALSE);
		grid_right->EnableWindow(FALSE);
		grid_left->SetHighLightBackColour(m_HighLightColDisable);
		grid_right->SetHighLightBackColour(m_HighLightColDisable);
		grid_right->Invalidate();
		grid_left->Invalidate();

		if ( m_vkeyResult )
		{
			m_vkeyResult->clear();
			m_vkeyResult->params.push_back(vkeyFromEdit);
		}
	}

	

	return 1;
}