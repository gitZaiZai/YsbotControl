// Copyright 2016, ��˼��Ϣ�Ƽ��������ι�˾
// All right reserved.
//
// Author: letuspray@163.com (Lu Wei)
//
// Yasi Robot Motion Control System - ��˼�������˶�����ϵͳ
//
// ���̰�ť��ʵ���ļ�

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
	// �ͷ�����
	if (nullptr != font_) {
		delete font_;
		font_ = nullptr;
	}
	return;
}

// ��������
void KeyboardButton::SetMyFont(int height, int width, CString name, int weight,
	BYTE italic, BYTE underline) {
		// ֱ���޸�CFont�����㣬����ʹ���½������滻������ķ�ʽ��ʵ��
		// ��������Ҫ�Ѿ������ͷŵ�
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

// ����ͼ��
void KeyboardButton::SetMyIcon(CString icon) {
	HICON my_icon =
		(HICON)::LoadImage(NULL, icon, IMAGE_ICON, 32, 32, LR_LOADFROMFILE);
	SetIcon(my_icon);
	Invalidate(FALSE);
	return;
}

void KeyboardButton::OnLButtonDown(UINT nFlags, CPoint point) {
	// ֻ��ִ���˸���ĺ����󣬲Ż��а��µ�Ч����ʾ���������ƽ�滯��
	CMFCButton::OnLButtonDown(nFlags, point);
	SetFaceColor(RGB(200, 200, 200));
	return;
}

void KeyboardButton::OnLButtonUp(UINT nFlags, CPoint point) {
	CMFCButton::OnLButtonUp(nFlags, point);
	SetFaceColor(RGB(239, 239, 239));
	return;
}
