// Copyright 2016, 亚思信息科技有限公司
// All rights reserved.
//
// Author: letuspray@163.com (Lu Wei)
//
// Yasi Robot Motion Control System - 亚思机器人运动控制系统
//
// 这是标签形式的按钮类实现文件

#include "stdafx.h"
#include "yrmcs_label_button.h"
#include "../colourcode.h"

LabelButton::LabelButton() {
	SetTextColor(kColor[1]);                     // 设置文字颜色为白色
	SetFaceColor(kColor[2]);                     // 设置背景色
	m_nAlignStyle = CMFCButton::ALIGN_LEFT;      // 文本左对齐
	m_nFlatStyle = CMFCButton::BUTTONSTYLE_FLAT; // 设置成平面按钮
	states_ = -1;     // 初始化状态值
	font_ = nullptr;  // 必须初始化，否则设置字体时会出错
	// 初始化图标文件名与文字
	for (unsigned i = 0; i <= ICON_NUMBER - 1; i++) {
		icons_[i] = NULL;
		caption_[i] = _T("");
	}
}

LabelButton::~LabelButton() {
	// 释放字体
	if (nullptr != font_) {
		delete font_;
		font_ = nullptr;
	}
	return;
}

// 字体设置
void LabelButton::SetMyFont(int height, int width, CString name, int weight,
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

// 设置标题
void LabelButton::SetCaption(short id1, CString caption) {
	if (id1 <= -1 || id1 >= ICON_NUMBER) return;
	caption_[id1] = caption;
	return;
}

// 获取标题
CString LabelButton::GetCaption(short id1) {
	if (id1 <= -1 || id1 >= ICON_NUMBER) return _T("");
	return caption_[id1];
}


// 设置图标序列
void LabelButton::SetIcons(short number, CString file, int width, int height) {
	if (number <= -1 || number >= ICON_NUMBER) return;
	icons_[number] = (HICON)LoadImage(NULL, file, IMAGE_ICON, width, height,
		LR_LOADFROMFILE);
}

// 通过序号设置当前显示内容
void LabelButton::SetLable(short number) {
	if (number <= -1 || number >= ICON_NUMBER) return;
	SetIcon(icons_[number]);
	SetWindowText(caption_[number]);
}