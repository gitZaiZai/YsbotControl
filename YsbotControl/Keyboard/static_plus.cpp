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
	// �ͷ�����
	if (nullptr != font_) {
		delete font_;
		font_ = nullptr;
	}
	return;
}

// �ػ档������ʽ�ı�������ɫ����̫�ɿ��������ػ�ɿ�Щ��
void StaticPlus::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) {
	CRect rect;
	GetWindowRect(&rect);             // ��ȡ���������С
	ScreenToClient(&rect);            // ����Ļ����ת��Ϊ�ͻ�������

	CString str;
	GetWindowText(str);
	CDC *dc = GetWindowDC();         // ��õ�ǰ���ڵ��豸����
	CDC dc_mem;
	dc_mem.CreateCompatibleDC(dc);   // �����뵱ǰ�豸�������ݵ��ڴ��豸�����Ļ���
	CBitmap bmp_background;
	// �����뵱ǰ�豸������صļ��ݵ�λͼ
	bmp_background.CreateCompatibleBitmap(dc, rect.Width(), rect.Height());
	// ��λͼѡ��ָ�����豸����
	dc_mem.SelectObject(&bmp_background);
	dc_mem.StretchBlt(0, 0, rect.Width(), rect.Height(), dc, rect.left, 
		rect.top, rect.Width(), rect.Height(), SRCCOPY);
	// ������ѡ���豸������
	dc_mem.SelectObject(font_);
	if (transparent_ == TRUE) {
		dc_mem.SetBkMode(TRANSPARENT);   // �������ֱ���͸��
	} else {
		dc_mem.SetBkColor(backcolor_);   // �������ֱ�����ɫ
	}
	dc_mem.SetTextColor(text_color_);  // ����������ɫ
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

// ��������
void StaticPlus::SetMyFont(int height, int width, CString name, int weight,
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
