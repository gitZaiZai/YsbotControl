// Copyright 2016, 亚思信息科技有限责任公司
// All right reserved.
//
// Author: letuspray@163.com (Lu Wei)
//
// Yasi Robot Motion Control System - 亚思机器人运动控制系统
//
// 键盘按钮类实现文件

#include "stdafx.h"
#include "keyboard_button.h"

BEGIN_MESSAGE_MAP(KeyboardButton, CMFCButton)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

KeyboardButton::KeyboardButton() {
	font_ = nullptr;
	return;
}

KeyboardButton::~KeyboardButton() {
	// 释放字体
	if (nullptr != font_) {
		delete font_;
		font_ = nullptr;
	}
	return;
}

// 字体设置
void KeyboardButton::SetMyFont(int height, int width, CString name, int weight,
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

// 设置图标
void KeyboardButton::SetMyIcon(CString icon) {
	HICON my_icon =
		(HICON)::LoadImage(NULL, icon, IMAGE_ICON, 32, 32, LR_LOADFROMFILE);
	SetIcon(my_icon);
	Invalidate(FALSE);
	return;
}

void KeyboardButton::OnLButtonDown(UINT nFlags, CPoint point) {
	// 只有执行了父类的函数后，才会有按下的效果显示，否则就是平面化的
	CMFCButton::OnLButtonDown(nFlags, point);
	SetFaceColor(RGB(200, 200, 200));
	return;
}

void KeyboardButton::OnLButtonUp(UINT nFlags, CPoint point) {
	CMFCButton::OnLButtonUp(nFlags, point);
	SetFaceColor(RGB(239, 239, 239));
	return;
}
