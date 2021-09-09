#include "stdafx.h"
#include "static_plus.h"

BEGIN_MESSAGE_MAP(StaticPlus, CStatic)
END_MESSAGE_MAP()

StaticPlus::StaticPlus() {
	text_color_ = RGB(0, 0, 0);
	backcolor_ = RGB(255, 255, 255);
	transparent_ = TRUE;
	font_ = nullptr;
	return;
}

StaticPlus::~StaticPlus() {
	// 释放字体
	if (nullptr != font_) {
		delete font_;
		font_ = nullptr;
	}
	return;
}

// 重绘。其他方式改变字体颜色都不太可靠，还是重绘可靠些。
void StaticPlus::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) {
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
	dc_mem.SelectObject(font_);
	if (transparent_ == TRUE) {
		dc_mem.SetBkMode(TRANSPARENT);   // 设置文字背景透明
	} else {
		dc_mem.SetBkColor(backcolor_);   // 设置文字背景颜色
	}
	dc_mem.SetTextColor(text_color_);  // 设置文字颜色
	dc_mem.DrawText(str, &rect, DT_CENTER);
	dc->BitBlt(0, 0, rect.Width(), rect.Height(), &dc_mem, 0, 0, SRCCOPY);
	ReleaseDC(dc);

	return;
}

LRESULT StaticPlus::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) {
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

// 字体设置
void StaticPlus::SetMyFont(int height, int width, CString name, int weight,
	BYTE italic, BYTE underline) {
		// 直接修改CFont不方便，这里使用新建字体替换旧字体的方式来实现
		// 这样就需要把旧字体释放掉
		if (nullptr != font_) {
			delete font_;
			font_ = nullptr;
		}
		CFont *font = new CFont();
		font->CreateFont(height, width, 0, 0, weight, italic, underline, FALSE,
			DEFAULT_CHARSET, OUT_STROKE_PRECIS, CLIP_STROKE_PRECIS,
			DRAFT_QUALITY, FIXED_PITCH | FF_MODERN, name);
		font_ = font;
		SetFont(font_);
		return;
}
