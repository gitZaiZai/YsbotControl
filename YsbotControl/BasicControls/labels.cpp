#include "stdafx.h"
#include "labels.h"

IMPLEMENT_DYNAMIC(BasicLabel, CStatic)

BEGIN_MESSAGE_MAP(BasicLabel, CStatic)
	ON_WM_PAINT()
END_MESSAGE_MAP()

BasicLabel::BasicLabel() {
	font_height_ = 18;
	font_width_ = 0;
	font_.CreateFont(font_height_, font_width_, 0, 0, FW_BOLD, FALSE, FALSE,
		FALSE, DEFAULT_CHARSET, OUT_STROKE_PRECIS, CLIP_STROKE_PRECIS,
		DRAFT_QUALITY, FIXED_PITCH | FF_MODERN, font_name_);
	alignment_ = DT_CENTER;
}

BasicLabel::~BasicLabel() 
{
	//ReleaseDC(dc_);
	//mem_dc_.DeleteDC();
	font_.DeleteObject();

}

LRESULT BasicLabel::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) {
	if (WM_SETTEXT == message) {
		CRect rect;
		GetWindowRect(&rect);
		CWnd *pParentWnd = GetParent();
		if (pParentWnd) {
			pParentWnd->ScreenToClient(&rect);
			pParentWnd->InvalidateRect(&rect);
		}
	}
	return CStatic::DefWindowProc(message, wParam, lParam);
}

void BasicLabel::OnPaint() {
	if (IsIconic())	{
		CPaintDC dc(this); // device context for painting
		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);
	} else {
		CPaintDC dc(this);

		GetClientRect(&rect_);

		dc.SetBkColor(backcolor_);
		dc.SetBkMode(mode_);		
		dc.SelectObject(&font_);
		dc.SetTextColor(forecolor_);
		CString str;
		GetWindowText(str);
		dc.DrawText(str, &rect_, DT_SINGLELINE | alignment_);
	}	
}

void BasicLabel::Initialize(COLORREF forecolor, int mode, COLORREF backcolor,
	int font_height, int font_width, CString font_name) {
	forecolor_ = forecolor;
	mode_ = mode;
	backcolor_ = backcolor;
	font_height_ = font_height;
	font_width_ = font_width;
	font_name_ = font_name;
}

void BasicLabel::SetForeColor(COLORREF color) {
	forecolor_ = color;
}

void BasicLabel::SetBackColor(COLORREF color) {
	backcolor_ = color;
}

// 设置背景效果，1是透明，其他数值都是不透明
void BasicLabel::SetBackMode(int mode) {
	if (mode == 1) {
		mode_ = TRANSPARENT;
	} else {
		mode_ = OPAQUE;
	}
}

void BasicLabel::SetFontName(CString name) {
	font_name_ = name;
}

void BasicLabel::SetAlignment(int align) {
	if (align == 1) {
		alignment_ = DT_LEFT;
	} else if (align == 2) {
		alignment_ = DT_RIGHT;
	} else {
		alignment_ = DT_CENTER;
	}
}

// 使用双缓存显示文字
void BasicLabel::ShowText(CString str, COLORREF text_color, COLORREF backcolor)
{
	dc_ = this->GetDC();
	GetClientRect(&rect_);
	if (mem_dc_.CreateCompatibleDC(dc_)) {
		mem_dc_.FillSolidRect(rect_.left, rect_.top, rect_.Width(), rect_.Height(),
			backcolor);
		mem_dc_.SelectObject(&font_);
		mem_dc_.SetTextColor(text_color);
		CBrush brush1;
		brush1.CreateSolidBrush(backcolor);
		if (bitmap_.CreateCompatibleBitmap(dc_, rect_.right, rect_.bottom)) {
			mem_dc_.SelectObject(&bitmap_);
			mem_dc_.FillRect(&rect_, &brush1);
			mem_dc_.TextOut(rect_.left, rect_.top, str);
			// 把内存绘图拷贝到屏幕
			dc_->BitBlt(rect_.left, rect_.top, rect_.right, rect_.bottom, &mem_dc_,
				rect_.left, rect_.top, SRCCOPY);
			// 清理释放
			bitmap_.DeleteObject();
		}
		mem_dc_.DeleteDC();
	}
	ReleaseDC(dc_);

}

void BasicLabel::SetFontType(int fontSize, int width, CString fontType) 
{  
	font_.DeleteObject();

	LOGFONT lf;  
	memset(&lf, 0, sizeof(LOGFONT));  
	lf.lfHeight = fontSize; 
	lf.lfWeight = width;
	_tcsncpy_s(lf.lfFaceName, LF_FACESIZE, fontType, fontType.GetLength());  
	VERIFY(font_.CreateFontIndirect(&lf));  
}  