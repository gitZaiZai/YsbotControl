// Copyright 2016, 亚思信息科技有限责任公司
// All right reserved.
//
// Author: letuspray@163.com (Lu Wei)
//
// Yasi Robot Motion Control System - 亚思机器人运动控制系统
//
// 标题栏类实现文件

#include "stdafx.h"
#include "redbud_caption.h"

BEGIN_MESSAGE_MAP(RedbudCaption, CStatic)
  ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
END_MESSAGE_MAP()

RedbudCaption::RedbudCaption() 
{
	text_ = _T("");
	text_color_ = RGB(0, 0, 0);
	drag_ = FALSE;
	// 设置默认活动范围，实际使用中用户应自行设置
	area_ = CRect(0, 0, 500, 300);

	m_bUseCustomFont = FALSE;
	m_font.CreateFont(22, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
		OUT_STROKE_PRECIS, CLIP_STROKE_PRECIS, DRAFT_QUALITY, FIXED_PITCH |
		FF_MODERN, _T("Arial"));
}

RedbudCaption::~RedbudCaption() 
{
	brush_.DeleteObject();
	m_font.DeleteObject();
};

void RedbudCaption::SetBkBmp(CString file) 
{
	brush_.DeleteObject();
	CRect rect;
	GetClientRect(&rect);
  // CBitmap类型不能直接加载图片文件，因此使用HBITMAP来间接实现
  HBITMAP bmp = (HBITMAP)::LoadImage(NULL, file, IMAGE_BITMAP, rect.Width(),
		rect.Height(), LR_LOADFROMFILE | LR_CREATEDIBSECTION);
	CBitmap bitmap;
  bitmap.Attach(bmp);
	brush_.CreatePatternBrush(&bitmap);  // 把背景图作为图样来初始化画刷
	bitmap.DeleteObject();               // 释放位图
	return;
}

void RedbudCaption::OnPaint() {
	if (IsIconic()) {

	} else {
		CStatic::OnPaint();
		Draw(text_);
	}
	return;
}

void RedbudCaption::Draw(CString str) 
{
	text_ = str;
	CDC *dc = GetDC();
	CRect rect;
	GetClientRect(&rect);
	dc->FillRect(rect, &brush_);         // 绘制背景图
	dc->SetBkMode(TRANSPARENT);          // 设置背景透明
	dc->SetTextColor(text_color_);       // 设置文字颜色
	rect.left = 10;
	CFont* pOldFont = NULL;
	if ( m_bUseCustomFont  )
	{
		pOldFont = dc->SelectObject(&m_font);
 	}
	dc->DrawText(str, rect, DT_VCENTER | DT_SINGLELINE);// 绘制单行垂直居中字符
	if(pOldFont)
		dc->SelectObject (pOldFont);
	return;
}

void RedbudCaption::OnLButtonDown(UINT nFlags, CPoint point) {
	drag_ = TRUE;                // 拖拽开始
	start_point_ = point;        // 设置原始鼠标所在位置（相对于本控件）
	GetWindowRect(&start_site_); // 读取原始位置（本控件在父对话框中的位置）
	return;
}

// 鼠标移动
void RedbudCaption::OnMouseMove(UINT nFlags, CPoint point) {
	if (drag_ == FALSE) return;
	// 通过新点与原点计算偏移量
	int offset_x = point.x - start_point_.x;
	int offset_y = point.y - start_point_.y;
	// 通过偏移量计算新的左上角坐标
	int new_x = start_site_.left + offset_x;
	int new_y = start_site_.top + offset_y;
	// 根据设定的活动区域限制对话框的活动范围
	new_x = (new_x <= area_.left) ? area_.left : new_x;
	new_y = (new_y <= area_.top) ? area_.top : new_y;
	new_x = (new_x >= area_.right) ? area_.right : new_x;
	new_y = (new_y >= area_.bottom) ? area_.bottom : new_y;
	// 根据偏移量修改父对话框位置
	if ( !area_.IsRectEmpty() )
	{
		GetParent()->SetWindowPos(NULL, new_x, new_y,	0, 0, SWP_NOSIZE);
	}
	// 刷新原始位置
	GetWindowRect(&start_site_);

	// 通过以上内容我们就可以知道拖拽一个窗口，特别是拖拽时还要显示窗口内容时，
	// 系统需要做的事情是比较多的，因此对CPU的占用要远超静态显示。
	// 在Windows系列的操作系统中的性能设置中就有一个选项“拖动时显示窗口内容”

	// 当鼠标以极快的速动移动到本控件之外时，控件不能即时做出反应，
	// 当鼠标再回到本控件上时会产生跳动感，不影响使用，只是有些不爽
	// MFC虽然有WM_MOUSELEAVE，但并不会自动响应，需要用户设置，代码如下
	TRACKMOUSEEVENT tme;
	tme.cbSize = sizeof(TRACKMOUSEEVENT);
	tme.dwFlags = TME_LEAVE;
	tme.hwndTrack = this->m_hWnd;

	if (::_TrackMouseEvent(&tme)) {
		mouse_tracking_ = TRUE;
	}
	return;
}

void RedbudCaption::OnLButtonUp(UINT nFlags, CPoint point) {
	drag_ = FALSE;   // 放开鼠标左键后拖拽结束
	return;
}

// 当鼠标离开时结束拖拽状态
LRESULT RedbudCaption::OnMouseLeave(WPARAM wParam, LPARAM lParam) {
	mouse_tracking_ = FALSE;
	drag_ = FALSE;
	return TRUE;
}

// 活动区域设置
void RedbudCaption::SetArea(CRect rect) {
	area_= rect;
	// 在这里设置活动区域，以减少鼠标移动时的计算量
	// 1、获取父对话框的位置大小
	GetParent()->GetWindowRect(&rect);
	// 2、右下角横坐标减去宽度获得左上角横坐标的可活动范围
	area_.right = area_.right - rect.Width();
	// 3、右下角纵坐标减去高度获得左上角纵坐标的可活动范围
	area_.bottom = area_.bottom - rect.Height();
	return;
}