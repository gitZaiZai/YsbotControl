#include "StdAfx.h"
#include "DlgParameterSetting.h"
#include "../colourcode.h"
#include "../resource.h"
#include "../WorldModel.h"
#include "../stringconvert.h"
#include "../Keyboard/digital_keyboard.h"
#include "../Keyboard/full_qwerty.h"
 
#include "state_dialog.h"

DlgParameterSetting::DlgParameterSetting( const ConfigReader& vr,
	HBITMAP hBmpScrollBar, 
	HBITMAP hBmpPlus, 
	HBITMAP hBmpMinus ) : m_hBmpScrollBar(hBmpScrollBar),
	m_hBmpPlus(hBmpPlus), m_hBmpMinus(hBmpMinus)
{
	hItem_list.clear();
	joint_icon  = -1;
	motion_icon = -1;
 	group_read_writer = new GroupPropertiesReadWriter(vr);
}


DlgParameterSetting::~DlgParameterSetting(void)
{
	hItem_list.clear();
	image_list_.DeleteImageList();
	delete group_read_writer;
}

BEGIN_MESSAGE_MAP(DlgParameterSetting, DialogType)
	ON_NOTIFY( NM_DBLCLK, PARAMETER_DIALOG_TREE, OnClickTree )
	ON_NOTIFY(GVN_BEGINLABELEDIT,PARAMETER_DIALOG_TREE,   &DlgParameterSetting::OnTreeBeginEdit)
	ON_NOTIFY(GVN_ENDLABELEDIT, PARAMETER_DIALOG_TREE,    &DlgParameterSetting::OnTreeEndEdit)
END_MESSAGE_MAP()

void DlgParameterSetting::DoDataExchange(CDataExchange* pDX) 
{
	;
}

LRESULT DlgParameterSetting::OnResizeGrid(WPARAM wp,LPARAM lp)
{
	static UINT lastType = 0;
	static UINT i = 0;
	UINT nType = UINT(wp);
	// re-size all pages
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
 
BOOL DlgParameterSetting::OnInitDialog() 
{
	CDialogEx::OnInitDialog();
	if ( GetParent()->GetDlgItem(IDC_BOX_MAIN)->GetSafeHwnd() )
	{
		GetParent()->GetDlgItem(IDC_BOX_MAIN)->GetClientRect(&clientRect);
 		SetWindowPos(NULL,clientRect.left, clientRect.top, clientRect.Width(), clientRect.Height(), SWP_NOZORDER);
	}
	//SetWindowPos(NULL, g_x0 + 20, 95, 730, 439, SWP_NOZORDER);
	SetBackgroundColor(kColor[8]);
	// 	ModifyStyleEx(WS_EX_STATICEDGE, WS_EX_CLIENTEDGE, SWP_DRAWFRAME);

	// 创建图片列表
	image_list_.Create (22, 22, ILC_COLOR32 | ILC_MASK, 5, 1);
	joint_icon = image_list_.Add(AfxGetApp()->LoadIcon(IDI_AXIS_MOTOR));
	motion_icon = image_list_.Add(AfxGetApp()->LoadIcon(IDI_MOTION_SPEED));
 	int overlay_icon = image_list_.Add(AfxGetApp()->LoadIcon(IDI_OVERLAY_TRANS));
	image_list_.SetOverlayImage(overlay_icon, 1);

	group_prop = MWM.get_group_properties();

	create_tree_grid();
 
	return TRUE;
}

BOOL DlgParameterSetting::create_tree_grid() throw (InvalidConfigurationException)
{
	BOOL ret = m_Grid.Create(
		CRect(0, 0, clientRect.Width(), clientRect.Height() ), 
		this,
		PARAMETER_DIALOG_TREE, WS_VISIBLE | WS_CHILD,
		m_hBmpScrollBar, m_hBmpPlus, m_hBmpMinus, SKIN_WINDOW_VARIABLE, 0 );

	if ( ret )
	{
		m_Grid.SetImageList(&image_list_);
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
		m_Grid.SetShowImageCombobox(FALSE);

		initialize_tree_grid();
		m_Grid.GetCell(0, 0)->SetDrawSortArrow(FALSE); 
		m_Grid.GetCell(0, 1)->SetDrawSortArrow(FALSE); 
	}
	return ret;
}

BOOL DlgParameterSetting::initialize_tree_grid()
{
	UpdateData();

	m_Grid.SetAutoSizeStyle();

	int m_nFixCols = 0;
	int m_nFixRows = 1;
 	m_Grid.SetFixedRowCount(m_nFixRows);
	m_Grid.SetFixedColumnCount(m_nFixCols);
	 
	m_Grid.SetDefCellHeight(38);
	m_Grid.SetRedraw(FALSE);
 
	m_Grid.InsertColumn(_T("参数"),DT_LEFT|DT_TOP);
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

	CString strlabel;
	CString paratitle = CString(group_read_writer->group_properties_descriptor[PARAMETER_AXIS]->getClassName());
	GTV_HTREENODE hItem;
	if ( MWM.is_main_world_model_available() )
	{
		const GroupProperties& group_prop = MWM.get_group_properties();
		int aixsNum = group_prop.axis_num;
		for ( int i = 0; i < aixsNum; ++i )
		{
			strlabel.Format(_T(" %d"),i+1);
			strlabel = paratitle + strlabel;
			Item.strText = strlabel;
			Item.iImage  = joint_icon;
			hItem        = m_Grid.InsertItem(&Item);
			m_Grid.SetItemState(m_Grid.FindRowIndex(hItem),1,GVIS_READONLY);
			hItem_list.push_back(hItem);
		}
	}
 
	Item.strText = CString(group_read_writer->group_properties_descriptor[PARAMETER_MOTION]->getClassName());
	Item.iImage  = motion_icon;
	hItem  = m_Grid.InsertItem(&Item);
	m_Grid.SetItemState(m_Grid.FindRowIndex(hItem),1,GVIS_READONLY);
	hItem_list.push_back(hItem);
 
	// Write extra codes for setting up the treelistctrl for sorting
	m_Grid.SetHeaderSort(FALSE);

	// Set the default compare function for all rows
	m_Grid.SetCompareFunction(CGridCtrl::pfnCellTextCompare);

    insert_axis_parameter();
    insert_motion_parameter();

	update_tree_parameter();

	UpdateData(FALSE);

	return TRUE;
}

void DlgParameterSetting::insert_axis_parameter()
{
	GTV_HTREENODE hItemAxis = NULL;
	GV_ITEM Item;
	Item.mask = GVIF_TEXT|GVIF_FORMAT;
	Item.row = 0;
	Item.col = 0;
	Item.nFormat = DT_LEFT | DT_VCENTER;
	
	CString strtmp;
	int nRow = -1;

	CStringArray options;
    options.Add(_T("Dummy"));
	options.Add(_T("Nidec"));
	options.Add(_T("Maxsine"));
	options.Add(_T("DeltaA2"));
	options.Add(_T("Yaskawa"));
	options.Add(_T("SanyoRS2"));
	options.Add(_T("Tronix"));
 
	char prop[VAR_STR_LEN];
	for ( unsigned int i = 0; i < hItem_list.size()-1; ++i )
	{
		hItemAxis = hItem_list[i];

		for ( int j = 0; j < group_read_writer->group_properties_descriptor[PARAMETER_AXIS]->getValuesTotal(); j++ )
		{
			group_read_writer->group_properties_descriptor[PARAMETER_AXIS]->getValue(j+1,prop);
			Item.strText = CString(prop);
 			nRow = m_Grid.FindRowIndex( m_Grid.InsertItem(&Item, hItemAxis) );
// 			m_Grid.SetItemText( nRow, 1, _T("0.123456789") );
			m_Grid.SetItemFormat(nRow,1,DT_RIGHT);
			m_Grid.SetItemState(nRow,0,GVIS_READONLY);
			m_Grid.SetItemState(nRow,1,GVIS_READONLY);	

			if ( strstr(prop,"servo_type") ) // servo name
			{
				m_Grid.SetItemState(nRow,1,m_Grid.GetItemState(nRow, 1) & ~GVIS_READONLY);	
				if (!m_Grid.SetCellType(nRow, 1, RUNTIME_CLASS(CGridCellCombo)))
				{
					cout << "error set grid type\n";
					continue;
				}
 
				CGridCellCombo *pCell = (CGridCellCombo*) m_Grid.GetCell(nRow, 1);
				pCell->SetOptions(options,TRUE);
				pCell->SetStyle(CBS_DROPDOWNLIST);  
 			}
			else if ( strstr(prop,"BOOL") ) // reverse
			{
				m_Grid.SetItemState(nRow,1,m_Grid.GetItemState(nRow, 1) & ~GVIS_READONLY);	
				m_Grid.SetCellType(nRow, 1, RUNTIME_CLASS(CGridCellCheck));
 			}
			
		}
		m_Grid.CollapseTreeNode(hItemAxis);
	}
	 
}

void DlgParameterSetting::insert_motion_parameter()
{
	GTV_HTREENODE hItemAxis = hItem_list[hItem_list.size()-1];
	GV_ITEM Item;
	Item.mask = GVIF_TEXT|GVIF_FORMAT;
	Item.row = 0;
	Item.col = 0;
	Item.nFormat = DT_LEFT | DT_VCENTER;

	CString strtmp;
	int nRow = -1;
	char prop[VAR_STR_LEN];

	for ( int j = 0; j < group_read_writer->group_properties_descriptor[PARAMETER_MOTION]->getValuesTotal(); ++j )
	{
		group_read_writer->group_properties_descriptor[PARAMETER_MOTION]->getValue(j+1,prop);
  		Item.strText =  CString(prop);
		nRow = m_Grid.FindRowIndex( m_Grid.InsertItem(&Item, hItemAxis) );
		m_Grid.SetItemText( nRow, 1, _T("0.123456789") );
		m_Grid.SetItemFormat(nRow,1,DT_RIGHT);
		m_Grid.SetItemState(nRow,0,GVIS_READONLY);
		m_Grid.SetItemState(nRow,1,GVIS_READONLY);	
	}
	m_Grid.CollapseTreeNode(hItemAxis);
}

void DlgParameterSetting::update_tree_parameter()
{
	GTV_HTREENODE hItemAxis = NULL;
	CString paraKind;
	CString paraName;

	char strValue[VAR_STR_LEN];

	for (unsigned int i = 0; i < hItem_list.size(); ++i )
	{
		hItemAxis = hItem_list[i];
		if ( !m_Grid.NodeHasChildren(hItemAxis) )
 			continue;
 
		paraKind = m_Grid.GetTreeItemText(hItemAxis,0);

 		for( GTV_HTREENODE treeitem = m_Grid.GetChildNode(hItemAxis); 
			treeitem != NULL; 
			treeitem = m_Grid.GetNextSiblingNode(treeitem) )
		{
			if ( treeitem != NULL )
			{
				paraName = m_Grid.GetTreeItemText(treeitem,0);
 
				group_read_writer->read_group_parameter( MWM.get_group_properties(),
					Unicode2Ansi(paraKind.GetBuffer(0)).c_str(),
					Unicode2Ansi(paraName.GetBuffer(0)).c_str(),
					strValue );
			    if ( strValue[0] != '\0' )
			    {
// 					int nRow = m_Grid.FindRowIndex(treeitem);
// 					CGridCellCheck *pCell = (CGridCellCheck*) m_Grid.GetCell(nRow, 1);
// 					if ( strcmp(strValue,"true") == 0 )
// 					{
// 						if ( pCell->GetCheck() == FALSE )
// 						{
//  							pCell->SetCheck(TRUE);
// 						}
// 					}
// 					else if ( strcmp(strValue,"false") == 0 )
// 					{
// 						pCell->SetCheck(FALSE);
// 					}
 					if ( strcmp(strValue,"true") == 0 || strcmp(strValue,"false") == 0 )
					{
						int nRow = m_Grid.FindRowIndex(treeitem);
						CGridCellCheck* pCell = (CGridCellCheck*) m_Grid.GetCell(nRow, 1);
						BOOL bCheck = ( strcmp(strValue,"true") == 0 ) ? TRUE : FALSE;
						if ( pCell->GetCheck() == FALSE && bCheck )
						{
							pCell->SetCheck(TRUE);
						}
						else if ( pCell->GetCheck() == TRUE && !bCheck )
						{
							pCell->SetCheck(FALSE);
						}
					}
 					else
					    m_Grid.SetTreeItemText(treeitem,1,CString(strValue));
			    }
			} 
  		}
   	}

	m_Grid.Invalidate();
}

void DlgParameterSetting::OnTreeBeginEdit(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*) pNotifyStruct;
	printf(("dan Start Edit on row %d, col %d\n"), pItem->iRow, pItem->iColumn);
	*pResult = 0;  
}

void DlgParameterSetting::OnTreeEndEdit(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*) pNotifyStruct;
	printf(("dan End Edit on row %d, col %d\n"), pItem->iRow, pItem->iColumn);
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
 
		if ( pCell->GetRuntimeClass() == RUNTIME_CLASS(CGridCellCombo)  )  // liurundan
		{
			CString  str = m_Grid.GetItemText( pItem->iRow, pItem->iColumn );
 			group_read_writer->write_group_parameter( group_prop,
				Unicode2Ansi(strparent.GetBuffer(0)).c_str(),
				Unicode2Ansi(strval.GetBuffer(0)).c_str(),
				Unicode2Ansi(str.GetBuffer(0)).c_str() );
 		}
		else if ( pCell->GetRuntimeClass() == RUNTIME_CLASS(CGridCellCheck) )
		{
 			CGridCellCheck *pCell = (CGridCellCheck*) m_Grid.GetCell(pItem->iRow, 1);
 			group_read_writer->write_group_parameter( group_prop,
				Unicode2Ansi(strparent.GetBuffer(0)).c_str(),
				Unicode2Ansi(strval.GetBuffer(0)).c_str(),
				pCell->GetCheck() ? "true" : "false" );
		}	
	}
}

LRESULT DlgParameterSetting::OnKeyboardEnter( WPARAM wp, LPARAM lp )
{
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
  
 	valid = group_read_writer->write_group_parameter( group_prop,
		Unicode2Ansi(strparent.GetBuffer(0)).c_str(),
		Unicode2Ansi(strval.GetBuffer(0)).c_str(),
		Unicode2Ansi(str_result.GetBuffer(0)).c_str() );
	// Is the value valid?
	if ( valid )
	{
		m_Grid.SetTreeItemText(currItem, 1, str_result);
		m_Grid.Invalidate();
 	}

 	return 1;
}

void DlgParameterSetting::OnClickTree(NMHDR* pNMHDR, LRESULT* pResult)
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

void DlgParameterSetting::parse_item_string( int row )
{
	// modify variable directly.
	CString datastr  = m_Grid.GetItemText(row,1);
	CString datatype = m_Grid.GetItemText(row,0);
	 
	Singleton<DigitalKeyboard>* digital_keyboard_ = Singleton<DigitalKeyboard>::Instance();
	Singleton<FullQwerty>*      full_keyboard_    = Singleton<FullQwerty>::Instance();
	 
	m_CellEdit.row = row;
	m_CellEdit.col =  m_Grid.GetFocusCell().col;
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

LRESULT DlgParameterSetting::OnMainButtonClicked( WPARAM wp, LPARAM lp)
{
	int btnid  = (int)(wp);
	int menuid = (int)(lp);
	int curr_row = -1;
	switch(btnid)
	{
	case 0:
		printf("Reboot soft control!\n");
		GetParent()->SendMessage(WM_REBOOT_SYSTEM,0,0);
		break;
	case 1:
		curr_row = m_Grid.GetFocusCell().row;
		parse_item_string(curr_row);
 		break;
	case 2:
		group_prop = MWM.get_group_properties();
		update_tree_parameter();
   		break;
	case 3:		
		MWM.set_group_properties(group_prop);
		group_read_writer->save_parameter_to_file(group_prop);
		MSD.ShowInfo(_T("修改系统参数"),JTMESSAGE);
 		break;
	case 4: break;
	case 5:
		ShowWindow(SW_HIDE);
		GetParent()->SendMessage(WM_ACTIVATE_WINDOW, winLastWindow);
 		break;
	default:break;
	}
	return 1;
}