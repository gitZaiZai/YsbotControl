#include "stdafx.h"
#include "popup_menu.h"
#include "../BasicControls/draw_frame.h"

BEGIN_MESSAGE_MAP(PopupMenu, CDialogEx)
	ON_WM_ERASEBKGND()
	//ON_COMMAND_RANGE(IDC_POPUP_MENU_ITEM, IDC_POPUP_MENU_ITEM + MAX_MENUITEM_NUMBER, ItemClick)
	ON_CONTROL_RANGE(BN_CLICKED,IDC_POPUP_MENU_ITEM, IDC_POPUP_MENU_ITEM + MAX_MENUITEM_NUMBER, ItemClick)
	ON_WM_KILLFOCUS()
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()

PopupMenu::PopupMenu( ) 
{
// 	font_.CreateFont(18, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
// 		DEFAULT_CHARSET, OUT_STROKE_PRECIS, CLIP_STROKE_PRECIS,
// 		DRAFT_QUALITY, FIXED_PITCH | FF_MODERN, _T("����"));
	font_.CreateFont(18, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
		OUT_STROKE_PRECIS, CLIP_STROKE_PRECIS, DRAFT_QUALITY, FIXED_PITCH |
		FF_MODERN, _T("����"));

	for (unsigned i = 0; i != MAX_MENUITEM_NUMBER; i++) {
		item_name_[i] = _T("");
	}
	item_number_ = 0;
	parent_menu_id_    = -1;
	draw_begin_ = FALSE;
	m_hRespondWin = NULL;

	m_rcButton.SetRectEmpty();
}

PopupMenu::~PopupMenu()
{
 	//DestroyWindow();
	font_.DeleteObject();
}

void PopupMenu::DoDataExchange(CDataExchange* pDX) {

}

// void PopupMenu::OnCancel() {
// 	DestroyWindow();
// }
// 
// void PopupMenu::PostNcDestroy() {
// 	CDialogEx::PostNcDestroy();
// 	if (NULL != this) 
// 	{
// 		delete this;
// 	}
// }

BOOL PopupMenu::OnInitDialog()
{
	DialogType::OnInitDialog();
	// ���ݸ�����λ�ü������λ��
	CRect rect;
	GetParent()->GetWindowRect(&rect);
	SetWindowPos(NULL, 22, 195, 182, 0, SWP_NOZORDER);
	// �ȴ������յĲ˵���
	int y0 = 4;
	for (unsigned i = 0; i != MAX_MENUITEM_NUMBER; i++) 
	{
		item_[i].Create(_T(""), WS_CHILD | WS_VISIBLE | SS_NOTIFY |
			SS_OWNERDRAW | SS_CENTER | SS_CENTERIMAGE, CRect(1, y0, 182, y0 + 40),
			this, IDC_POPUP_MENU_ITEM + i);
		item_[i].SetFont(&font_);
		y0 += 44;
	}

	return TRUE;
}

BOOL PopupMenu::OnEraseBkgnd(CDC* pDC) {
	CDialogEx::OnEraseBkgnd(pDC);
	CWindowDC dc(this);
	if (draw_begin_ == FALSE)
		return TRUE;
	DrawMyFrame(&dc, item_number_, 220, 220, 220);

	return TRUE;
}

void PopupMenu::OnKillFocus(CWnd* pNewWnd) {
	
	ShowWindow(SW_HIDE);
	return;
}

void PopupMenu::OnSetFocus(CWnd* pOldWnd) 
{
	return;
	//////////////////////////////////////////////////////////////////////////
/*
 	if ( parent_menu_id_ >= MAIN_BUTTON_BASE_ID && parent_menu_id_ < MAIN_BUTTON_BASE_ID+7) 
	{
		int coord_y[] ={95, 160, 225, 290, 355, 420, 485};
		SetCursorPos(10, coord_y[parent_menu_id_ - MAIN_BUTTON_BASE_ID]);
	}

	// ��ȡ���λ��
	POINT point;
	GetCursorPos(&point);
	printf("popup menu clicked %d,%d\n", point.x, point.y);

	CRect point_ctrl;
	// ��ȡ������ڿؼ��ľ��
	CWnd *cwnd = WindowFromPoint(point);
	cwnd->GetWindowRect(&point_ctrl);
	// ��ȡ�˵���С
	CRect rect;
	GetWindowRect(&rect);
	// λ���ж�
	int x0 = point.x + 5;
	if (point.x + rect.Width() >= 800) {
		x0 = point.x - rect.Width();
	}
	int y0 = point.y;
	if (point.y + rect.Height() >= 600) 
	{
		y0 = point.y - rect.Height();
	}
	// ��������������˵�ʱ�Ժ����������
	if (x0 <= 20) {
		x0 = 23;
		y0 = point_ctrl.top;
	}
	// ��������²��˵�ʱ�Ժ����������
// 	if (y0 <= 538) 
// 	{
// 		if (x0 >= 131) 
// 		{
// 			x0 = point_ctrl.left;
// 		} 
// 		else 
// 		{
// 			x0 = 23;
// 		}
// 	}

	if ( y0 <= 538 ) 
	{
 		x0 = point_ctrl.left;
 	}

	y0 = y0 + (rect.Height() >= 530) ? (540 - rect.Height()) : y0;
	SetWindowPos(0, x0, y0, 0, 0, SWP_NOSIZE);
	
	return;
	*/
}

void PopupMenu::CreateMenuItem(int id, CString str) {
	// ���ĩβ�Ƿ�Ϊ���ţ�������������
	CString str0 =
		str.Find(_T(","), str.GetLength() - 1) == -1 ? _T(",") : _T("");
	str += str0;
	item_number_ = 0;
	// ��ȡ�˵������ֲ������˵������
	for (unsigned i = 0; i != MAX_MENUITEM_NUMBER; i++) {
		// ��ȡ�˵�������
		int site = str.Find(_T(","));
		// ���ò˵���ߴ�
		item_[i].SetWindowPos(NULL, 0, 0, MENUITEM_WIDTH, 40, SWP_NOMOVE);
		if (site == -1) {
			// ������Ч�˵������˵��Ի���ߴ�
			SetWindowPos(NULL, 0, 0, MENUITEM_WIDTH, 4 + i * 40 + (i - 1) * 4 + 5,
				SWP_NOMOVE);
			break;
		}
		item_number_++;
		item_name_[i] = str.SpanExcluding(_T(","));  // ��ȡ��һ������ǰ���ַ���
		str.Delete(0, site + 1);                     // ɾ����һ������ǰ���ַ���
		// ��д�˵�������
		item_[i].SetWindowText(item_name_[i]);
	}
	parent_menu_id_ = id;
	CWindowDC dc(this);
	DrawMyFrame(&dc, item_number_, 220, 220, 220);
	draw_begin_ = TRUE;
	return;
}

void PopupMenu::ItemClick(UINT number) 
{
	int bntindex = number - IDC_POPUP_MENU_ITEM;
 	if ( bntindex >= 0 )
	{
		ShowWindow(SW_HIDE);
 		if ( m_hRespondWin )
		{
			::SendMessage( m_hRespondWin, WM_BUTTON_PRESS, parent_menu_id_, bntindex ); // send message to current window.
		}
		//Sleep(100);

// 		if ( m_hRespondWin )
// 		{
// 			CWnd* pWnd = CWnd::FromHandle(m_hRespondWin); 
// 			pWnd->RedrawWindow();
// 		}
	}
	return;
}

void PopupMenu::active_menu(HWND hRespondWin, int primary_btn_id )
{
 	ShowWindow(SW_SHOW);
 	parent_menu_id_  = primary_btn_id;
	m_hRespondWin = hRespondWin;
}

void PopupMenu::set_rect_button(LPRECT lrct)
{
	m_rcButton = CRect(lrct);
}

void PopupMenu::active_menu(HWND hRespondWin, int primary_btn_id, int menu_count, const LPCTSTR* menu_text )
{
	if ( menu_count > MAX_MENUITEM_NUMBER )
 		return;

 	parent_menu_id_ = primary_btn_id;
	m_hRespondWin = hRespondWin;
    
	item_number_ = 0;
	// ��ȡ�˵������ֲ������˵������
	for (unsigned i = 0; i != menu_count; i++) 
	{
 		item_[i].SetWindowPos(NULL, 0, 0, MENUITEM_WIDTH, 40, SWP_NOMOVE);
 		item_number_++;
		item_name_[i] = menu_text[i];  
 		item_[i].SetWindowText(item_name_[i]);
		item_[i].SendMessage(WM_LBUTTONUP); // liurundan
	}

	int nWidth = MENUITEM_WIDTH;
	int nHeight = 4 + menu_count * 40 + (menu_count - 1) * 4 + 5;

	if ( m_rcButton.IsRectEmpty() == FALSE )
	{
 		SetWindowPos(NULL, m_rcButton.left, m_rcButton.top - nHeight, MENUITEM_WIDTH, 4 + menu_count * 40 + (menu_count - 1) * 4 + 5,SWP_SHOWWINDOW);
	}
	
	CWindowDC dc(this);
	DrawMyFrame(&dc, item_number_, 220, 220, 220);
	draw_begin_ = TRUE;
// 	SetFocus();
// 	OnSetFocus(this);

	//ShowWindow(SW_SHOW);
	 
}