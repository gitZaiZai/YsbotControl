#include "StdAfx.h"
#include "SetUserManage.h"
#include "../colourcode.h"

#include "state_dialog.h"
#include "../WorldModel.h"
#include "../stringconvert.h"
#include "SettingUserPopup.h"
#include "SqlDatabase.h"
#include "../resource.h"

BEGIN_MESSAGE_MAP(CSetUserManage, CGroupFrame)
 	ON_COMMAND_RANGE( IDC_SETTING_USERMANAGE_BTN_EDIT, IDC_SETTING_USERMANAGE_BIN_DEL, OnButtonClicked )
	ON_MESSAGE( WM_POPUPBTN_YESNO, &OnButtonPopup )	
	//ON_MESSAGE( WM_RESIZE_GRID,     &OnResizeGrid        )

END_MESSAGE_MAP()

CSetUserManage::CSetUserManage(HBITMAP hBmpScrollBar, 
	HBITMAP hBmpPlus, 
	HBITMAP hBmpMinus ) : m_hBmpScrollBar(hBmpScrollBar),
	m_hBmpPlus(hBmpPlus), m_hBmpMinus(hBmpMinus)
{
	caption_default = _T("用户");
}

CSetUserManage::~CSetUserManage(void)
{
	font_.DeleteObject();
	image_list_.DeleteImageList();
}

BOOL CSetUserManage::Create(CString caption,DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	CStatic::Create( caption.IsEmpty() ? caption_default : caption, dwStyle|SS_NOTIFY, rect, pParentWnd, nID );

	m_nID = nID;

	font_.CreateFont(22, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
		OUT_STROKE_PRECIS, CLIP_STROKE_PRECIS, DRAFT_QUALITY, FIXED_PITCH |
		FF_MODERN, _T("Arial"));

	LOGFONT lf;
	font_.GetLogFont(&lf);
 
	// 创建图片列表
	image_list_.Create (22, 22, ILC_COLOR32 | ILC_MASK, 5, 1);
	admin_icon = image_list_.Add(AfxGetApp()->LoadIcon(IDI_AXIS_MOTOR));
	normal_icon = image_list_.Add(AfxGetApp()->LoadIcon(IDI_MOTION_SPEED));
	int overlay_icon = image_list_.Add(AfxGetApp()->LoadIcon(IDI_OVERLAY_TRANS));
	image_list_.SetOverlayImage(overlay_icon, 1);
// 	overlay_icon = image_list_.Add((HICON)::LoadImageW(AfxGetInstanceHandle(),
// 		_T(".\\res\\OverlayTrans.ico"),IMAGE_ICON,0,0,LR_DEFAULTSIZE|LR_LOADFROMFILE));

// 	btn_login.Create(_T("登录"), WS_CHILD | WS_VISIBLE,CRect( CPoint(160,150), CSize(190,46) ), this, IDC_SETTING_LOGIN_BTN );
// 	btn_login.SetFont(&font_);
// 	btn_login.SetFaceColor(kColor[8]);
	clientRect = rect;
	//GetClientRect(&clientRect);
 	int heightBtn = 50;
	int widthBtn = clientRect.Width()/5;
	const LPCTSTR strButtonTitle[] = 
	{
		_T("编辑"),
		_T("新建"),
		_T("删除"),
 
	};
	for ( int i=0; i<ARRAY_SIZE(m_button); i++ )
	{
		m_RectBtn = CRect( CPoint(widthBtn*(i+1),clientRect.bottom - heightBtn-10), CSize(widthBtn,heightBtn) );
		m_button[i].Create(strButtonTitle[i], WS_CHILD | WS_VISIBLE, m_RectBtn, 
			this, 
			IDC_SETTING_USERMANAGE_BTN_EDIT+i );

		m_button[i].SetFont(&font_);
		m_button[i].SetFaceColor(kColor[8]);
	}
	
	create_tree_grid();
//     load_user_data();

	return TRUE;
}

void CSetUserManage::OnButtonClicked(UINT id)
{
	unsigned int bnt_index = id - IDC_SETTING_USERMANAGE_BTN_EDIT; 
	const UserData& userData = MWM.get_userdata();
	if ( !userData.administrator || userData.nGrade < 5 )
	{
		MSD.ShowInfo(_T("当前用户无权限"));;
 		return;
 	}

	int nRow = m_Grid.GetFocusCell().row;
	if ( bnt_index == 0 ) // edit
	{
		if ( nRow <= 1 )
		{
			MSD.ShowInfo(_T("请选择可编辑的用户"));
			return;
		}
        int data = (int) m_Grid.GetItemData(nRow,0);
		GetParent()->SendMessage( WM_BTN_USER_MANAGE, data, bnt_index+1 );
 
	}
	else if ( bnt_index == 1 ) // new
	{
		GetParent()->SendMessage( WM_BTN_USER_MANAGE, 0, bnt_index+1 );
	}
	else // del
	{
 		if ( nRow > 1 )
		{
			int nKeyPrimary = m_Grid.GetItemData(nRow,0);
			char condition[200] = {'\0'};
			sprintf(condition,"_ID = %d",nKeyPrimary);
			bool ret = SQL.getTable(TBLUser)->deleteRecords(condition);
			 
			//CString username = m_Grid.GetItemText(nRow,0);
			if ( ret )
			{
				m_Grid.DeleteRow(nRow);
				m_Grid.Invalidate();
 				MSD.ShowInfo(_T("删除用户成功"));

				GetParent()->SendMessage( WM_BTN_USER_MANAGE, nKeyPrimary, bnt_index+1 );

			}
			else
			{
				MSD.ShowInfo(_T("删除用户失败"));
			}

		}
	}
	 
	cout << bnt_index << " clicked user manage\n";
}

BOOL CSetUserManage::create_tree_grid()
{
	int iTitleBarHeight = 25;
	BOOL ret = m_Grid.Create(
		CRect(0, iTitleBarHeight, clientRect.Width(), m_RectBtn.top ), 
		this,
		IDC_SETTING_USERMANAGE_GRID, WS_VISIBLE | WS_CHILD,
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

		m_Grid.SetDefCellHeight(38);
		m_Grid.SetRedraw(FALSE);

		m_Grid.InsertColumn(_T("用户"),DT_CENTER|DT_TOP);
		m_Grid.InsertColumn(_T("级别"),DT_CENTER|DT_TOP);
		m_Grid.InsertColumn(_T("语言"),DT_CENTER|DT_TOP);

		m_Grid.SetColumnWidth(0, 300);
		m_Grid.SetColumnWidth(1, 150);
		m_Grid.SetColumnWidth(2, 150);

		m_Grid.ExpandColumnsToFit(); 

		m_Grid.SetRedraw(TRUE);
   
 		m_Grid.SetHeaderSort(FALSE);
    
		m_Grid.GetCell(0, 0)->SetDrawSortArrow(FALSE); 
		m_Grid.GetCell(0, 1)->SetDrawSortArrow(FALSE); 
		m_Grid.GetCell(0, 2)->SetDrawSortArrow(FALSE); 
		m_Grid.SetShowSrollBar(SB_HORZ, FALSE);  
 
		 
	}
	return ret;
}

BOOL CSetUserManage::load_user_data()
{
	m_Grid.DeleteNonFixedRows();

	GV_ITEM Item;
 	Item.mask = GVIF_TEXT | GVIF_FORMAT | GVIF_STATE;
	Item.nFormat = DT_LEFT | DT_BOTTOM;
	Item.nState  = GVIS_READONLY;

	sql::string strValName = "";
	sql::integer gradeVal = 0;
	CString levelStr;
	
	Item.strText = _T("Administrator");
	Item.row = m_Grid.InsertRow(Item.strText);
	 	
	levelStr.Format(_T("%d"), 16 );
	m_Grid.SetItemText( Item.row, 1, levelStr );
 	m_Grid.SetItemText( Item.row, 2, _T("中文") );
 	m_Grid.SetItemData( Item.row, 0, 0 );
	//m_Grid.SetItem(&Item);
	m_Grid.SetItemFormat(Item.row,1,DT_RIGHT | DT_BOTTOM);

 	int nKeyID = 0;
	SQL.getTable(TBLUser)->open();
	for (int index = 0; index < SQL.getTable(TBLUser)->recordCount(); index++)
	{
		if ( sql::Record* record = SQL.getTable(TBLUser)->getRecord(index) )
		{
			nKeyID = (int)record->getValue(SQL.getFieldSet(TBLUser)->getByIndex(0)->getName())->asInteger();
			strValName = record->getValue(SQL.getFieldSet(TBLUser)->getByIndex(1)->getName())->asString();
			gradeVal = record->getValue(SQL.getFieldSet(TBLUser)->getByIndex(3)->getName())->asInteger();
			Item.strText = Ansi2Unicode(strValName).c_str();
			Item.row = m_Grid.InsertRow(Item.strText);	
			 	
			levelStr.Format(_T("%d"),(int)gradeVal);
			m_Grid.SetItemText( Item.row, 1, levelStr );
			m_Grid.SetItemText( Item.row, 2, _T("中文") );
			m_Grid.SetItemData( Item.row, 0, nKeyID );
  			m_Grid.SetItemFormat(Item.row,1,DT_RIGHT | DT_BOTTOM);
		}
	}

	return TRUE;
}

int CSetUserManage::insert_user_data(const UserData& user)
{
	GV_ITEM Item;
	Item.mask = GVIF_TEXT | GVIF_FORMAT | GVIF_STATE;
	Item.nFormat = DT_LEFT | DT_BOTTOM;
	Item.nState  = GVIS_READONLY;

	sql::string strValName = "";
	sql::integer gradeVal = 0;
	CString levelStr;

	Item.strText = Ansi2Unicode(user.strUserName).c_str();
	 
	Item.row = m_Grid.InsertRow(Item.strText);		
	levelStr.Format(_T("%d"), user.nGrade );
	m_Grid.SetItemText( Item.row, 1, levelStr );
	m_Grid.SetItemText( Item.row, 2, _T("中文") );
	m_Grid.SetItemData( Item.row, 0, user.nUserID );
// 	m_Grid.SetItem( &Item );
	m_Grid.SetItemFormat(Item.row,1,DT_RIGHT | DT_BOTTOM);
  
	m_Grid.invalidate_sel_bottom(); // solve vert scroll bar disable

	int nCount = m_Grid.GetRowCount();
	int nColCount = m_Grid.GetColumnCount();

	m_Grid.SetFocusCell(nCount-1,0);
	m_Grid.SetSelectedRange(nCount-1,0,nCount-1,nColCount-1,FALSE,TRUE);
 
	return 0;
}

LRESULT CSetUserManage::OnButtonPopup(WPARAM wp, LPARAM lp)
{
 	UserData newUser  = *(UserData*)(wp);
	int nret = insert_user_data(newUser);

	return nret;
}

LRESULT CSetUserManage::OnResizeGrid(WPARAM wp,LPARAM lp)
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