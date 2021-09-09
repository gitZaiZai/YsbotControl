#include "stdafx.h"
#include "JournalDLG.h"
#include "../WorldModel.h"
#include "../MotionControl.h"
#include "../LuaScript/ScriptVariables.h"
#include "../stringconvert.h"
#include "../colourcode.h"
#include "../resource.h"
#include "SqlDatabase.h"
#include <locale.h>
#include "state_dialog.h"
#include "../DirectoryOperate.h"

#define MAX_GRID_ROWS     (15)

#define DEBUG_JOURNAL_DATABASE  (0)

#define  MAX_JOURNAL_DIR_SIZE ( 5*1024*1024)   // 所有log文件的总大小限制 5M

JournalDLG *journal_dialog = nullptr;


JournalDLG::JournalDLG(const ConfigReader& vr, HBITMAP hBmpScrollBar, 
	HBITMAP hBmpPlus, 
	HBITMAP hBmpMinus ) : m_hBmpScrollBar(hBmpScrollBar),
	m_hBmpPlus(hBmpPlus), m_hBmpMinus(hBmpMinus)
{
    m_columFitTypeGrid = 0;
	m_ShowJournalType = -1;

	nMessageCount = 0;
	nWarningCount = 0;
	nErrorCount   = 0;
}

JournalDLG::~JournalDLG() 
{

}

BEGIN_MESSAGE_MAP(JournalDLG, DialogType)
	ON_CBN_SELCHANGE(JOURNAL_DIALOG_COMBBOX, &JournalDLG::OnCbnSelGroupType)
	ON_NOTIFY( NM_CLICK, JOURNAL_DIALOG_GRID, &JournalDLG::OnClickGrid )

END_MESSAGE_MAP()
  
// have error when insert data from self dialog and the other dialog.
LRESULT JournalDLG::OnResizeGrid(WPARAM wp,LPARAM lp)
{
// 	static UINT lastType = 0;
// 	static UINT i = 0;
// 	UINT nType = UINT(wp);
// 
// 	// re-size all pages
// 	if ( m_columFitTypeGrid != nType )
// 	{
// 		m_columFitTypeGrid = nType;
// 		m_Grid.ExpandColumn(3,400, (nType==0) ); // 0 - fit, 1 = resize colum.
// 		printf("jour N %d dialog on size message %d\n",i++,nType);
// 	}
// 
// 	printf("jour dialog on size message\n");

	return 1;
}

BOOL JournalDLG::OnInitDialog() 
{
	CDialogEx::OnInitDialog();
	/*SetWindowPos(NULL, g_x0 + 20, 95, 730, 439, SWP_NOZORDER);*/
	if ( GetParent()->GetDlgItem(IDC_BOX_MAIN)->GetSafeHwnd() )
	{
		GetParent()->GetDlgItem(IDC_BOX_MAIN)->GetClientRect(&clientRect);
		SetWindowPos(NULL,clientRect.left, clientRect.top, clientRect.Width(), clientRect.Height(), SWP_NOZORDER);
	}
	SetBackgroundColor(kColor[8]);
	// 	ModifyStyleEx(WS_EX_STATICEDGE, WS_EX_CLIENTEDGE, SWP_DRAWFRAME);

	font_.CreateFont(22, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
		OUT_STROKE_PRECIS, CLIP_STROKE_PRECIS, DRAFT_QUALITY, FIXED_PITCH |
		FF_MODERN, _T("Arial"));

	LOGFONT lf;
	font_.GetLogFont(&lf);

	// 创建图片列表
	image_list_.Create (22, 22, ILC_COLOR32 | ILC_MASK, 5, 1);
	none_icon    = image_list_.Add(AfxGetApp()->LoadIcon(IDI_NONE));
	message_icon = image_list_.Add(AfxGetApp()->LoadIcon(IDI_GRID_INFO));
	warning_icon = image_list_.Add(AfxGetApp()->LoadIcon(IDI_GRID_WARNING));
	error_icon   = image_list_.Add(AfxGetApp()->LoadIcon(IDI_GRID_ERROR));
	overlay_icon = image_list_.Add(AfxGetApp()->LoadIcon(IDI_OVERLAY_TRANS));
	image_list_.SetOverlayImage(overlay_icon, 1);
	
	CRect clientRect;
	GetClientRect(&clientRect);
	int heightLastLine = 35;
 	int halfWidthClient =(int)( clientRect.Width()/2.0);
	int thirdWidth = (int)( clientRect.Width()/3);
	m_title.Create(_T("分组"),WS_CHILD|WS_VISIBLE,CRect( CPoint(10,clientRect.bottom-heightLastLine+5), CSize(50,30) ),this);
	m_title.SetFont(22);
 
	CRect rectPos( CPoint(halfWidthClient-thirdWidth-10,clientRect.bottom-heightLastLine), CSize(thirdWidth,170) );
	group_combbox.SetSort(false);
	group_combbox.SetShowImage(FALSE);
	group_combbox.Create(WS_CHILD | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED |CBS_HASSTRINGS,rectPos,this, JOURNAL_DIALOG_COMBBOX );
	group_combbox.set_custom_message(FALSE);
	group_combbox.SetAlignTop(TRUE);
	group_combbox.AddString(_T("全部信息"));
	group_combbox.AddString(_T("错误"));
	group_combbox.AddString(_T("警告"));
	group_combbox.AddString(_T("提示"));
	group_combbox.AddString(_T("历史信息"));
	group_combbox.SetCurSel(0);
  	
	m_tipEdit.Create( WS_BORDER|WS_CHILD|WS_VISIBLE|WS_TABSTOP|ES_MULTILINE,
		CRect( CPoint(0,(int)( clientRect.Height() * 0.78) ), CSize(clientRect.Width(),60) ),this,JOURNAL_DIALOG_TIPS);
	m_tipEdit.SetWindowText(_T(""));
	m_tipEdit.EnableWindow(FALSE);
 
	const LPCTSTR strTitle[] = 
	{
		_T("警告"),
		_T("错误"),
		_T("提示"),
		_T("异常"),

	};

 
	lf.lfHeight = 18;

	int xPosSpace = 45;
	int widthLable = (int) clientRect.Width()/8;
	for ( int i = 0; i < ARRAY_SIZE(m_labelCount); ++i )
	{
		m_labelCount[i].Create(strTitle[i],WS_CHILD|WS_VISIBLE|SS_CENTERIMAGE, 
			CRect( CPoint(halfWidthClient+i*widthLable, clientRect.bottom-heightLastLine+5 ) , CSize(widthLable,30) ),this );
		m_labelCount[i].SetFont(lf)
			.SetBorder(TRUE)
			.SetBkColor(kColor[8]);
	}
	m_labelCount[0].SetLabelIcon(IDI_GRID_WARNING,16,16);
	m_labelCount[1].SetLabelIcon(IDI_GRID_ERROR,16,16);
	m_labelCount[2].SetLabelIcon(IDI_GRID_INFO,16,16);
	m_labelCount[3].ShowWindow(SW_HIDE);

	m_Item.mask = GVIF_TEXT | GVIF_FORMAT | GVIF_STATE | GVIF_IMAGE;
	m_Item.nFormat = DT_LEFT | DT_BOTTOM;
	m_Item.nState  = GVIS_READONLY;
	m_Item.strText = _T("");
	m_Item.iImage = message_icon;
	m_Item.row = 1;

	create_tree_grid();
 
	//list loaded records

 
	// for test
#if  DEBUG_JOURNAL_DATABASE
	SQL.getTable(TBLLogger)->open();
	for (int index = 0; index < SQL.getTable(TBLLogger)->recordCount(); index++)
		if (sql::Record* record = SQL.getTable(TBLLogger)->getRecord(index))
			sql::log(record->toString());

#endif 
 	
	timeSystemBoot = sql::time::now();
	return TRUE;
}
 
LRESULT JournalDLG::OnMainButtonClicked( WPARAM wp, LPARAM lp)
{
	int btnid  = (int)(wp);
	int menuid = (int)(lp);

	cout << "JournalDLG id = " << btnid << '\n';
	 
 	switch(btnid)
	{
	case 0:
		{
			for ( int i = 1; i < m_Grid.GetRowCount(); ++i )
			{
			     m_Grid.SetItemText( i, 2, _T("√") );
			}
			m_Grid.Invalidate();
   		}
 		break;
	case 1: 
		{
 			DirectoryOperate directory_operate;
			const std::wstring dirLogFile = Ansi2Unicode(SQL_LOG_DIR).c_str();
			BOOL direxist = directory_operate.CheckDirectoryExist(dirLogFile);
			if ( direxist )
			{
				DWORD dirSize = directory_operate.GetDirectorySize(dirLogFile);
				cout << " dirSize = " << dirSize << "\n";

				if ( dirSize > MAX_JOURNAL_DIR_SIZE )
				{
					directory_operate.DeleteDirectory(dirLogFile);
					MSD.ShowInfo(_T("日志文件清理成功！"),_T(""),this,3);
 				}
				else
				{
					MSD.ShowInfo(_T("日志文件不需清理！"),_T(""),this,3);
				}
			}
			else
				MSD.ShowInfo(_T("暂无需清理的日志！"),_T(""),this,3);

 		}
		break;
	case 2: 
		{
		    show_journal_history(-1);
		}
		break;
	case 3: 
		{
			show_journal_history(1);
 		}
		break;
	case 4:  // for test
		{
			//MSD.ShowInfo(_T("dd"),_T("ka"),this,5,JTERROR,5050); // TEST
			//m_Grid.DeleteNonFixedRows();

			JournalData jdata;
			sql::string rdetail = "";

			jdata.nType = 1;
			jdata.nCode = 22;
			rdetail = "dadadadfad";
			jdata.set_description(rdetail.c_str());
			jdata.timeRecord = sql::time::now();
			for ( int i = 0; i < 1; ++i )
			{
				insert_journal_information( jdata, false );
			}

			m_Grid.invalidate_sel_bottom();
			int nRowCount = m_Grid.GetRowCount();
			int nColCount = m_Grid.GetColumnCount();
			m_Grid.SetFocusCell(nRowCount-1,0);
			m_Grid.SetSelectedRange(nRowCount-1,0,nRowCount-1,nColCount-1,FALSE,TRUE);

		}
		break;
	case 5:
 		break;
	default:break;
	}

	return 1;
}

BOOL JournalDLG::create_tree_grid()
{
	CRect clientRect;
	GetClientRect(&clientRect);
	int heightGrid = (int)( clientRect.Height()* 0.78  );//

	BOOL ret = m_Grid.Create(
		CRect(0, 0, clientRect.Width(), heightGrid ), // 370
		this,
		JOURNAL_DIALOG_GRID, WS_VISIBLE | WS_CHILD,
		m_hBmpScrollBar, NULL, NULL, SKIN_WINDOW_USERLIST, 0 );

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

		m_Grid.SetEditable(FALSE); // read only set
		m_Grid.SetListMode(TRUE);

		m_Grid.SetGridLines(GVL_BOTH);
		m_Grid.SetGridLineColor(RGB(60,64,60));

		m_Grid.SetFixedRowSelection(TRUE);
		m_Grid.SetSingleRowSelection(TRUE);
		m_Grid.SetFixedColumnSelection(FALSE);

		//
		m_Grid.SetShowImageCombobox(FALSE);
		m_Grid.SetAutoSizeStyle();

		//////////////////////////////////////////////////////////////////////////

		int m_nFixCols = 0;
		int m_nFixRows = 1;
		m_Grid.SetFixedRowCount(m_nFixRows);
		m_Grid.SetFixedColumnCount(m_nFixCols);

		m_Grid.SetDefCellHeight(35);
		m_Grid.SetRedraw(FALSE);

		m_Grid.InsertColumn(_T("类别"),DT_CENTER|DT_TOP);
		m_Grid.InsertColumn(_T("时间"),DT_CENTER|DT_TOP);
		m_Grid.InsertColumn(_T("状态"),DT_CENTER|DT_TOP);
		m_Grid.InsertColumn(_T("描述"),DT_CENTER|DT_TOP);

		m_Grid.SetColumnWidth(0, 65);
		m_Grid.SetColumnWidth(1, 220);
		m_Grid.SetColumnWidth(2, 65);
		m_Grid.SetColumnWidth(3, 400);

		m_Grid.ExpandColumnsToFit(); 

		m_Grid.SetRedraw(TRUE);

		m_Grid.SetHeaderSort(FALSE);

		m_Grid.GetCell(0, 0)->SetDrawSortArrow(FALSE); 
		m_Grid.GetCell(0, 1)->SetDrawSortArrow(FALSE); 
		m_Grid.GetCell(0, 2)->SetDrawSortArrow(FALSE); 
		m_Grid.GetCell(0, 3)->SetDrawSortArrow(FALSE); 
		m_Grid.SetShowSrollBar(SB_HORZ, FALSE);  


	}
	return ret;
}

void JournalDLG::OnCbnSelGroupType()
{
	int index = group_combbox.GetCurSel();
 	show_journal_according_type( index );
 	//cout << "index = " << index << endl;
}
 
void JournalDLG::OnClickGrid(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*) pNMHDR;
	int nRow = pItem->iRow;
    if ( nRow < 1 )
 		return;

	CString desc = m_Grid.GetItemText( nRow, 3 );
	m_tipEdit.SetWindowText(desc);
 
}

int JournalDLG::OnActivateChildWin( LPARAM lp )
{
	m_Grid.invalidate_sel_bottom();
	int nRowCount = m_Grid.GetRowCount();
	int nColCount = m_Grid.GetColumnCount();
	m_Grid.SetFocusCell(nRowCount-1,0);
	m_Grid.SetSelectedRange(nRowCount-1,0,nRowCount-1,nColCount-1,FALSE,TRUE);

	if ( nRowCount > 1 )
	{
		CString desc = m_Grid.GetItemText( nRowCount-1, nColCount-1 );
 		m_tipEdit.SetWindowText(desc);
	}
	else
	{
		m_tipEdit.SetWindowText(_T(""));
 	}

	Invalidate();

	return 1;
}

void JournalDLG::insert_journal_information( const JournalData& jdata, bool insertDatabase )
{
	if ( !journal_dialog || !::IsWindow( m_Grid.GetSafeHwnd() ) )
 		return;
 
	if ( insertDatabase )
	{
		if ( m_ShowJournalType != SHOW_ALL ) 			// retrieve data;
 		{
			group_combbox.SetCurSel(0);
			show_journal_according_type(SHOW_ALL,0);
 		}
	}

	int nRowCount = m_Grid.GetRowCount();
	if ( nRowCount > MAX_GRID_ROWS-1 )
	{
		DWORD dw = m_Grid.GetItemData(1, 0);
		switch(dw)
		{
		case 0: nMessageCount--; break;
		case 1: nWarningCount--; break;
		case 2: nErrorCount--; break;
		default: break;
		}
 		m_Grid.DeleteRow(1);
	}

    int nImage = none_icon;
	switch(jdata.nType)
	{
  	case 0: nImage = message_icon; nMessageCount++; break;
	case 1: nImage = warning_icon; nWarningCount++; break;
	case 2: nImage = error_icon;   nErrorCount++;   break;
	default: nImage = none_icon; break;
	}

 	JournalData jour = jdata;
 
	timeCurrentLastRecord = jdata.timeRecord;
	m_Item.iImage = nImage;

	//m_Grid.SetRedraw(FALSE); // have error liurundan.
 	m_Item.row = m_Grid.InsertRow(m_Item.strText);	
	int nRow = m_Item.row;
  	m_Grid.SetItemFormat(m_Item.row,0,SS_CENTERIMAGE);
	m_Grid.SetItemData(nRow, 0, jdata.nType);
 	m_Grid.SetItemImage(nRow, 0, nImage );
	m_Grid.SetItemText( nRow, 1, Ansi2Unicode( jour.timeRecord.asTimsSecString() ).c_str() );
	m_Grid.SetItemData( nRow, 1, (LPARAM)jour.timeRecord.asInteger() );
 	m_Grid.SetItemFormat(nRow,2,DT_CENTER);
 
	m_Grid.SetItemText( nRow, 2, jour.nState == 0 ? _T("☆"): _T("√"));
	m_Grid.SetItemFormat(nRow,3,DT_LEFT|DT_TOP);
	// for show data in SQLlite manage for firefox normally.
    m_Grid.SetItemText( nRow, 3, Utf8ToUnicode(jdata.strDesc) ); 

	//m_Grid.SetItem(&m_Item);
 	//m_Grid.SetRedraw(TRUE);
 	m_Grid.invalidate_sel_bottom();

 	show_journal_count(nWarningCount,nErrorCount,nMessageCount);
 
	// insert to database
	if ( insertDatabase )
	{
		int nret = SQL.insert_journal(jour);
 	}

	OnActivateChildWin( 0 );

}

void JournalDLG::UpdateSurface()
{
	CRect (rc);
	GetWindowRect(rc);
	RedrawWindow();

	GetParent()->ScreenToClient(rc);
	GetParent()->InvalidateRect(rc,TRUE);
	GetParent()->UpdateWindow();
}
 
void JournalDLG::insert_journal_record( sql::Record* record, bool insertDatabase )
{
	JournalData jdata;
	sql::string rdetail = "";

   	jdata.nType = (int)record->getValue(SQL.getFieldSet(TBLLogger)->getByIndex(1)->getName())->asInteger();
	jdata.nCode = (int)record->getValue(SQL.getFieldSet(TBLLogger)->getByIndex(2)->getName())->asInteger();
	rdetail = record->getValue(SQL.getFieldSet(TBLLogger)->getByIndex(7)->getName())->asString();
	jdata.set_description(rdetail.c_str());
	jdata.timeRecord = record->getValue(SQL.getFieldSet(TBLLogger)->getByIndex(8)->getName())->asTime();

	insert_journal_information( jdata, insertDatabase );
}

void JournalDLG::show_journal_according_type(int nType, int nFindDir)
{
	if ( nType == m_ShowJournalType && nFindDir == 0 )
 		return;

	m_ShowJournalType = nType;

	char whereCondition[100] = {'\0'};
	char sortCondition[100] = {'\0'};

    make_where_condition(whereCondition,sortCondition,nType,nFindDir);

	m_Grid.DeleteNonFixedRows();
	nMessageCount = 0;
	nWarningCount = 0;
	nErrorCount   = 0;
	show_journal_count(nWarningCount,nErrorCount,nMessageCount);
 
	SQL.getTable(TBLLogger)->open( whereCondition, sortCondition );

 	sql::Record* record = NULL;
 	int nCountRecord = SQL.getTable(TBLLogger)->recordCount();

	if ( nFindDir == 1 ) // asc
	{
		for (int index = 0; index < nCountRecord;  index++ )
		{
			if ( record = SQL.getTable(TBLLogger)->getRecord(index) )
			{
				insert_journal_record(record, false);
			}
		}
	}
	else // desc
	{
		for (int index = nCountRecord - 1; index >= 0;  index-- )
		{
			if ( record = SQL.getTable(TBLLogger)->getRecord(index) )
			{
				insert_journal_record(record, false);
 			}
		}
 	}

	m_Grid.invalidate_sel_bottom();
	int nRowCount = m_Grid.GetRowCount();
	int nColCount = m_Grid.GetColumnCount();
	m_Grid.SetFocusCell(nRowCount-1,0);
	m_Grid.SetSelectedRange(nRowCount-1,0,nRowCount-1,nColCount-1,FALSE,TRUE);

}

void JournalDLG::show_journal_count( int nWarning, int nError, int nMessage )
{
	CString strWarningCount,strErrorCount,strMessageCount;
	strWarningCount.Format(_T("%d 警告"),nWarning);
	strErrorCount.Format(_T("%d 错误"),nError);
	strMessageCount.Format(_T("%d 提示"),nMessage);
	m_labelCount[0].SetText(strWarningCount);
	m_labelCount[1].SetText(strErrorCount);
	m_labelCount[2].SetText(strMessageCount);
}

void JournalDLG::show_journal_history(int dir)
{
	int index = group_combbox.GetCurSel();
	show_journal_according_type( index, dir );
}

void JournalDLG::make_where_condition( char* whereCondition, char* sortCondition, int nType, int nFindDir )
{
	if ( nFindDir != 1 )
	{
		sprintf(sortCondition," time desc limit %d ", MAX_GRID_ROWS-1); // 按时间降序限制总数为200条记录
	}
	else
	{
		sprintf(sortCondition," time asc limit %d ", MAX_GRID_ROWS-1); // 按时间升序限制总数为200条记录
	}

	sql::integer timeIntFirstRecord = timeSystemBoot.asInteger();
	sql::integer timeIntLastRecord = timeSystemBoot.asInteger();
	int nRowCount = m_Grid.GetRowCount();
	if ( nRowCount > 1 )
	{
		timeIntFirstRecord = (sql::integer)m_Grid.GetItemData(1, 1);
		timeIntLastRecord = (sql::integer)m_Grid.GetItemData( nRowCount-1, 1);
	}
 
	switch( nFindDir )
	{
	case -1 : 
		{
			switch(nType)
			{
			case SHOW_ALL: sprintf(whereCondition," time <= %d ", timeIntFirstRecord ); break;
			case SHOW_ERROR: sprintf(whereCondition," type = %d AND time <= %d ", 2, timeIntFirstRecord ); break;
			case SHOW_WARNING: sprintf(whereCondition," type = %d AND time <= %d ", 1, timeIntFirstRecord ); break;
			case SHOW_MESSAGE: sprintf(whereCondition," type = %d AND time <= %d ", 0, timeIntFirstRecord );break;
			default: break;
			}
		}
		break;
	case 0: // combbox
		{
			switch(nType)
			{
			case SHOW_ALL: sprintf(whereCondition," time >= %d ", timeSystemBoot.asInteger() ); break;
			case SHOW_ERROR: sprintf(whereCondition," type = %d AND time >= %d ", 2, timeSystemBoot.asInteger() ); break;
			case SHOW_WARNING:sprintf(whereCondition," type = %d AND time >= %d  ", 1, timeSystemBoot.asInteger() ); break;
			case SHOW_MESSAGE: sprintf(whereCondition," type = %d AND time >= %d  ", 0, timeSystemBoot.asInteger() ); break;
			default: break;
			}
		}
		break;
	case 1:
		{
			switch(nType)
			{
			case SHOW_ALL: sprintf(whereCondition," time >= %d ", timeIntLastRecord ); break;
			case SHOW_ERROR: sprintf(whereCondition," type = %d AND time >= %d ", 2, timeIntLastRecord ); break;
			case SHOW_WARNING: sprintf(whereCondition," type = %d AND time >= %d ", 1, timeIntLastRecord ); break;
			case SHOW_MESSAGE: sprintf(whereCondition," type = %d AND time >= %d ", 0, timeIntLastRecord );break;
			default: break;
			}
		}
		break;
	default: break;
	}
}