#include "stdafx.h"
#include "button_class.h"

// ����������������ʽ����ɫ
NewButton::NewButton() {
	font.CreateFont(22, 10, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
		DEFAULT_CHARSET, OUT_STROKE_PRECIS, CLIP_STROKE_PRECIS, DRAFT_QUALITY,
		FIXED_PITCH | FF_MODERN, _T("����"));
	SetTextColor(RGB(0, 0, 0));
}

// �������ò�������Ϊfont������˽�еģ�����Ҫ��������һ���������������壬����
// ����ʽ�ڹ��캯��������
void NewButton::SetMyFont() {
	SetFont(&font);
}