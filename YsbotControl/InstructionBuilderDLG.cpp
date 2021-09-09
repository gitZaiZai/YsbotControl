#include "stdafx.h"
#include "InstructionBuilderDLG.h"
#include "InstructionEditDLG.h"
#include "LuaScript/ScriptVariables.h"
#include "colourcode.h"
#include "YsbotControl.h"
#include "stringconvert.h"

InstructionBuilderDLG *instruction_DLG;

BEGIN_MESSAGE_MAP(InstructionBuilderDLG, DialogType)
 	ON_NOTIFY( GVN_SELCHANGED, IDC_Instruction_Kinds, &InstructionBuilderDLG::OnGridEndSelChange )
 	ON_NOTIFY( GVN_SELCHANGED, IDC_Instruction_Type, &InstructionBuilderDLG::OnRightGridEndSelChange )
	ON_WM_PAINT()
END_MESSAGE_MAP()

InstructionBuilderDLG::InstructionBuilderDLG( HBITMAP hBmpScrollBar ): m_hBmpScrollBar(hBmpScrollBar)
{
	grid_left = NULL;
	grid_right = NULL;
	m_last_row = 0;
	cmd_edit_info.maxWin = true;
	
}

InstructionBuilderDLG:: ~InstructionBuilderDLG()
{
	image_list_.DeleteImageList();
	delete grid_left;
	delete grid_right;
}

void InstructionBuilderDLG::DoDataExchange(CDataExchange* pDX) 
{

}

LRESULT InstructionBuilderDLG::OnMainButtonClicked( WPARAM wp, LPARAM lp)
{
	unsigned int btnid = (unsigned int)(wp);
 	 
	switch(btnid)
	{
	case 0:  
		break;
	case 1: 
		break;
	case 2: 
		break;
	case 3: 
		break;
	case 4: // cancle
		//GetParent()->SendMessage(WM_ACTIVATE_WINDOW,winLastWindow,0);      // return last window.
		ShowWindow(SW_HIDE);
		GetParent()->SendMessage(WM_ACTIVATE_WINDOW,winProgram);
		break; 
	case 5: 
		{
 			CString str = _T("");
			int nrow        = grid_right->GetFocusCell().row;
			int commandType = static_cast<int>( grid_right->GetItemData(nrow,0) );
			//instructionEdit_DLG->SetInstruction(str);
			if ( LSCINFO->isLuaSystemFunction(ScriptCommandT(commandType)) )
			{
				cmd_edit_info.lineCmd = CString(LSCINFO->CommandParameters[commandType]->getClassName());
				cmd_edit_info.nCmdType = ScriptCommandT(commandType);
				GetParent()->SendMessage( WM_ACTIVATE_WINDOW, winEditCommand, (LPARAM)&cmd_edit_info ); // simple
 			}
			else
			{
				RegParseResult cmdRes(true);
				cmdRes.sct = ScriptCommandT(commandType);
				cmdRes.nFromID = 0;
				GetParent()->SendMessage( WM_ACTIVATE_WINDOW,winInstructionEdit, (LPARAM)&cmdRes );
  			}
		}
		break;
	default: break;
	}

	return 1;
}

BOOL InstructionBuilderDLG::OnInitDialog() 
{
	DialogType::OnInitDialog();
	/*SetWindowPos(NULL,g_x0 + 22, 95, 724, 439, SWP_NOZORDER);*/
	if ( GetParent()->GetDlgItem(IDC_BOX_MAIN)->GetSafeHwnd() )
	{
		GetParent()->GetDlgItem(IDC_BOX_MAIN)->GetWindowRect(&clientRect);
		SetWindowPos(NULL,clientRect.left, clientRect.top, clientRect.Width(), clientRect.Height(), SWP_NOZORDER);
	}
	//	SetBackgroundColor(kColor[8], TRUE);
 	// 创建图片列表
	image_list_.Create (24, 24, ILC_COLOR32 | ILC_MASK, 5, 1);
	empty_icon    = image_list_.Add(AfxGetApp()->LoadIcon(IDI_RADIO_EMPTY));
	selected_icon = image_list_.Add(AfxGetApp()->LoadIcon(IDI_RADIO_SELECTED));
	int overlay_icon = image_list_.Add(AfxGetApp()->LoadIcon(IDI_OVERLAY_TRANS));
	image_list_.SetOverlayImage(overlay_icon, 1);

	CRect clientrect;
	GetClientRect(&clientrect);

	CRect rectTitleGrid[2];
	rectTitleGrid[0] = rectTitleGrid[1] = CRect(0, 0, (int)(clientrect.Width()/2+1.5), 315 );
	OffsetRect(&rectTitleGrid[1],rectTitleGrid[0].Width(),0);

	rectTitleGrid[0].bottom = rectTitleGrid[1].bottom = clientrect.top + 34;
	rectTitleGrid[0].right -= 1;
	rectTitleGrid[0].OffsetRect(1,0);
	rectTitleGrid[1].OffsetRect(2,0);

	CRect leftrect(0, 34, (int)(clientrect.Width()/2+1.5), 370 );
	CRect rightrect = leftrect;
	OffsetRect(&rightrect,leftrect.Width()+4,0);

	creat_grid(leftrect,IDC_Instruction_Kinds);
	creat_grid(rightrect,IDC_Instruction_Type);
  
	CRect descrect( 2, leftrect.bottom + 8, rightrect.right-10, leftrect.bottom+60);
 
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
		.SetText(_T(" 指令描述"));

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
			.SetText(i==0?_T(" 分类"):_T(" 宏指令"));
	}
  
	return FALSE;
}

void InstructionBuilderDLG::init_window()
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
		int row_height = 40;

		for ( int i = 0; i < SM_MAX_SETS; i++ )
		{
			grid_left->InsertRow(_T(""),i);
			grid_left->SetRowHeight(i,row_height);
			categoryname = LSCINFO->CommandCategory[i]->getClassName();
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
		desc_label.SetText( _T(" 点到点运动至绝对位置"), TRUE);
	}
}

CGridCtrl* InstructionBuilderDLG::creat_grid( CRect rect,  unsigned int gridID )
{
	CGridCtrl* pGrid = NULL;

	pGrid = new CGridCtrl();

	BOOL ret = FALSE;
	 
	if ( gridID == IDC_Instruction_Kinds )
	{
		ret = pGrid->Create(
			rect, 
			this,
			gridID,WS_CHILD | WS_VISIBLE,
			NULL, NULL, NULL, SKIN_WINDOW_VARIABLE );
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
  
	pGrid->SetImageList(&image_list_);
	pGrid->SetColumnResize(FALSE);
	pGrid->SetRowResize(FALSE);

	pGrid->EnableDragAndDrop(FALSE);
	pGrid->SetBkColor(RGB(248, 252, 248));
	pGrid->GetDefaultCell(FALSE, FALSE)->SetBackClr(RGB(248, 252, 248)); // 0XE0

	LOGFONT lf;
	memset(&lf, 0, sizeof(LOGFONT));
	lf.lfHeight = 24;
	lf.lfWeight = 700;
	wcsncpy_s(lf.lfFaceName, _T("Arial"), LF_FACESIZE-1);
	CFont font;
	font.CreateFontIndirect(&lf);
	pGrid->SendMessage(WM_SETFONT,(WPARAM)(font.GetSafeHandle()));
	font.DeleteObject();

	pGrid->SetEditable(FALSE);
	pGrid->SetListMode(TRUE);
	pGrid->EnableDragAndDrop(FALSE);

	pGrid->SetGridLines(GVL_NONE);

	pGrid->SetSingleRowSelection(TRUE);
 
// 	pGrid->SetDefCellHeight(row_height);

	pGrid->SetColumnWidth(0, 200);

	if ( pGrid->GetRowCount() > 0 )
	{
		pGrid->DeleteNonFixedRows();
	}
 
 	pGrid->ExpandColumnsToFit(TRUE); // liurundan
    
	pGrid->SetFocusCell(0,0);

	return pGrid;
}

void InstructionBuilderDLG::update_right_grid( ScriptCommandSetT svc  )
{
	if ( svc == SM_MAX_SETS )
		return;

	if ( grid_right->GetRowCount() > 0 )
		grid_right->DeleteNonFixedRows();
	 
	int type_num = LSCINFO->CommandCategory[svc]->getValuesTotal();
	char prop[VAR_STR_LEN];
	std::deque<std::string> strlist;
	int row_height = 40;
	for ( int i = 0; i < type_num; ++i )
	{
		LSCINFO->CommandCategory[svc]->getValue(i+1,prop);
		split_string( strlist, prop, LSCINFO->getSeparatorStr() );
		if ( strlist.size() >= 2)
		{
			grid_right->InsertRow( CString( strlist[0].c_str() ), i );
			grid_right->SetItemData(i,0, LSCINFO->CommandCategory[svc]->getValueKind(i+1) );
			grid_right->SetRowHeight(i,row_height);
		}	
	}
	strlist.clear();

	grid_right->SetFocusCell( 0, 0 );
	grid_right->SetSelectedRange(0,0,0,0,FALSE,TRUE);
 
	grid_right->Invalidate(TRUE);
	 
}
 
void InstructionBuilderDLG::update_window(int nRow)
{
	update_right_grid( ScriptCommandSetT(nRow) );
	update_edit( nRow, 0);
}

void InstructionBuilderDLG::OnGridEndSelChange(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*) pNMHDR;
	int nRow = pItem->iRow;

	if ( nRow < 0 || nRow == SM_MAX_SETS )
		return;

	if ( nRow != m_last_row )
	{
		bool bChangeFocusByMouse = false;
		int idCurrIcon = grid_left->GetItemImage(nRow,0);
		if ( idCurrIcon == empty_icon )
		{
			grid_left->SetItemImage( nRow, 0, selected_icon );
			bChangeFocusByMouse = true;
		}
		if ( m_last_row >= 0 )
		{
			int idLastIcon = grid_left->GetItemImage(m_last_row,0);
			if ( idLastIcon == selected_icon )
			{
				grid_left->SetItemImage( m_last_row, 0, empty_icon );
				bChangeFocusByMouse = true;
			}
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

void InstructionBuilderDLG::OnRightGridEndSelChange(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*) pNMHDR;
	int nRow = pItem->iRow;
	int nCol = pItem->iColumn;
	int nRowLeftGrid = grid_left->GetFocusCell().row;
	if ( nRowLeftGrid < 0 || nRow < 0 || nRowLeftGrid >= (int)(SM_MAX_SETS) )
		return;

	update_edit( nRowLeftGrid, nRow );

}

void InstructionBuilderDLG::update_edit(int leftRow, int rightRow)
{
	int type_num = LSCINFO->CommandCategory[leftRow]->getValuesTotal();
	if ( rightRow > type_num )
		return;

	char prop[VAR_STR_LEN];
	std::deque<std::string> strlist;

	LSCINFO->CommandCategory[leftRow]->getValue(rightRow+1,prop);
	split_string( strlist, prop, LSCINFO->getSeparatorStr() );

	if ( strlist.size() >= 2 )
	{
 		desc_label.SetText( _T(" ") + CString(strlist[1].c_str()), FALSE);
	}		 
}

void InstructionBuilderDLG::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call DialogType::OnPaint() for painting messages
	static bool b = false;
	if ( !b )
	{
		b = true;
		init_window();
		cout << "instruction builder on paint\n";
	}
}
