#include "stdafx.h"
#include "button_class.h"

// 在这里设置字体样式、颜色
NewButton::NewButton() {
	font.CreateFont(22, 10, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
		DEFAULT_CHARSET, OUT_STROKE_PRECIS, CLIP_STROKE_PRECIS, DRAFT_QUALITY,
		FIXED_PITCH | FF_MODERN, _T("黑体"));
	SetTextColor(RGB(0, 0, 0));
}

// 字体设置操作，因为font属性是私有的，所以要设置这样一个函数来设置字体，字体
// 的样式在构造函数中设置
void NewButton::SetMyFont() {
	SetFont(&font);
}