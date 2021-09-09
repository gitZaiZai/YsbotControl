#include "stdafx.h"
#include "logdialog.h"
#include "WorldModel.h"
#include "MotionControl.h"
#include "LuaScript/ScriptVariables.h"
#include "stringconvert.h"

LogDialog *log_dialog = nullptr;


LogDialog::LogDialog() 
{
	ctlReady = FALSE;
	filter_ = 0;
}

LogDialog::~LogDialog() 
{

}

BEGIN_MESSAGE_MAP(LogDialog, DialogType)
	ON_NOTIFY_REFLECT(LVN_ITEMCHANGED,OnItemchangedList)
	ON_NOTIFY(LVN_ITEMCHANGED,LOG_DIALOG_GRID,OnItemchangedList)
	ON_WM_TIMER()
END_MESSAGE_MAP()

void LogDialog::DoDataExchange(CDataExchange* pDX) {

}

BOOL LogDialog::OnInitDialog() 
{
	CDialogEx::OnInitDialog();
	SetWindowPos(NULL, g_x0 + 20, 95, 730, 439, SWP_NOZORDER);
	SetBackgroundColor(kColor[8]);
	// 	ModifyStyleEx(WS_EX_STATICEDGE, WS_EX_CLIENTEDGE, SWP_DRAWFRAME);
	// 创建表格
	grid_.Create(WS_BORDER | WS_VISIBLE | WS_CHILD | LVS_REPORT | LVCFMT_RIGHT,
		CRect(0, 0, 725, 436), this, LOG_DIALOG_GRID);
	grid_.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER | LVS_OWNERDRAWFIXED |LVS_EX_FULLROWSELECT );
	//grid_.SetItemState()
	grid_.ShowWindow(SW_SHOW);
	//grid_.SetMyFont();
	font_.CreateFont(21, 0, 0, 0, 0, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
		OUT_STROKE_PRECIS, CLIP_STROKE_PRECIS, DRAFT_QUALITY, FIXED_PITCH |
		FF_MODERN, _T("Arial"));
	grid_.SetFont(&font_);
	grid_.InsertColumn(0, _T("序号"), LVCFMT_CENTER, 50);
	grid_.InsertColumn(1, _T("类别"), LVCFMT_CENTER, 50);
	grid_.InsertColumn(2, _T("描述"), LVCFMT_CENTER, 260);
	grid_.InsertColumn(3, _T("状态"), LVCFMT_CENTER, 50);
	grid_.InsertColumn(4, _T("时间"), LVCFMT_CENTER, 195);
	grid_.InsertColumn(5, _T("备注"), LVCFMT_CENTER, 100);	

	//grid_.SetItemHeight(40);

	updata_grid_fromDB();
	ctlReady = TRUE;
	//SetTimer(3,10000,NULL);
	return TRUE;
}

void LogDialog::updata_grid_fromDB(unsigned int logType)
{
	if(MDB.is_main_database_available()==false)
		return;
	MDB.RefreshLogRecords(logType);

	std::vector<LogInfo> li;
	li = MDB.GetLogRecords();
	grid_.DeleteAllItems();
	for (unsigned int i=0; i < li.size(); i++)
	{
		CString	str;
		grid_.InsertItem(i,li[i].LogID);
		grid_.SetItemText(i,1,li[i].LogLevel);
		grid_.SetItemText(i,2,li[i].LogDescription);
		if (li[i].LogState==_T("1") || li[i].LogState==_T("ON") || li[i].LogState==_T("TRUE"))
		{
			grid_.SetItemText(i,3,_T("ON"));
		} 
		else
		{
			grid_.SetItemText(i,3,_T("OFF"));
		}

		grid_.SetItemText(i,4,li[i].LogTime);
		grid_.SetItemText(i,5,li[i].LogPrgName);
	}
}

LRESULT LogDialog::OnMainButtonClicked( WPARAM wp, LPARAM lp)
{
	int btnid  = (int)(wp);
	int menuid = (int)(lp);

	cout << "LogDialog id = " << btnid << '\n';
	LogInfo linfo;//test insert log
	std::string err;//test

	switch(btnid)
	{
	case 0:
		switch(menuid)
		{
		case 0: //全部记录
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:	
			updata_grid_fromDB(menuid);
			filter_ = menuid;
			break;
		case 6:
 			cout << err <<'\n';
			break;
		default:
			updata_grid_fromDB(0);
		}

		break;
	case 1: 

		break;
	case 2: 

		break;
	case 3: 

		break;
	case 4: 

		break;
	case 5:
		updata_grid_fromDB(filter_);
		break;
	default:break;
	}

	return 1;
}

void LogDialog::OnTimer(UINT_PTR nIDEvent)
{
	if(ctlReady && IsWindowVisible())
		updata_grid_fromDB();
	KillTimer(3);
}

void LogDialog::OnItemchangedList(NMHDR* pNMHDR,LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	if(pNMListView->uChanged==LVIF_STATE)
	{
		if(pNMListView->uNewState & LVIS_SELECTED)
		{
			//处理内容
			int nItem=pNMListView->iItem;			
			cout << "grid_.item changed,select item " << nItem << '\n';
		}
	}
	*pResult = 0;

}
