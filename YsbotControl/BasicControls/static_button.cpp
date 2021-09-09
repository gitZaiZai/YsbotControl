#include "stdafx.h"
#include "static_button.h"

BEGIN_MESSAGE_MAP(StaticButton, CStatic)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

StaticButton::StaticButton() {
	backcolor_ = RGB(220, 220, 220);
}

StaticButton::~StaticButton() {

}

// 重绘。其他方式改变字体颜色都不太可靠，还是重绘可靠些。
void StaticButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) {
	CRect rect;
	GetWindowRect(&rect);             // 获取窗口坐标大小
	ScreenToClient(&rect);            // 把屏幕坐标转换为客户区坐标

	CString str;
	GetWindowText(str);
	CDC *dc = GetWindowDC();         // 获得当前窗口的设备环境
	CDC dc_mem;
	dc_mem.CreateCompatibleDC(dc);   // 创建与当前设备环境兼容的内存设备上下文环境
	CBitmap bmp_background;
	// 创建与当前设备环境相关的兼容的位图
	bmp_background.CreateCompatibleBitmap(dc, rect.Width(), rect.Height());
	// 把位图选入指定的设备环境
	dc_mem.SelectObject(&bmp_background);
	dc_mem.StretchBlt(0, 0, rect.Width(), rect.Height(), dc, rect.left, 
		rect.top, rect.Width(), rect.Height(), SRCCOPY);
	// 把字体选入设备环境中
	dc_mem.SetBkMode(TRANSPARENT);      // 设置文字背景透明
	dc_mem.SetTextColor(RGB(0, 0, 0));  // 设置文字颜色
	CBrush brush(backcolor_);
	dc_mem.FillRect(CRect(0, 0, 180, 40), &brush);
	rect.top = rect.top + 11;
	dc_mem.DrawText(str, &rect, DT_CENTER);
	dc->BitBlt(0, 0, rect.Width(), 40, &dc_mem, 0, 0, SRCCOPY);
	ReleaseDC(dc);

	return;
}

LRESULT StaticButton::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) {
	if (WM_SETTEXT == message) {
		CRect rect;
		GetWindowRect(&rect);
		CWnd *parent_wnd = GetParent();
		if (parent_wnd) {
			parent_wnd->ScreenToClient(&rect);
			parent_wnd->InvalidateRect(&rect);
		}
	}
	return CStatic::DefWindowProc(message, wParam, lParam);
}

void StaticButton::OnLButtonDown(UINT nFlags, CPoint point) {
	CStatic::OnLButtonDown(nFlags, point);
	backcolor_ = RGB(152, 152, 152);
	Invalidate(TRUE);
	return;
}

void StaticButton::OnLButtonUp(UINT nFlags, CPoint point) {
	CStatic::OnLButtonUp(nFlags, point);
	backcolor_ = RGB(220, 220, 220);
	Invalidate(TRUE);
	return;
}