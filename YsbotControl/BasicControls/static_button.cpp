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

// �ػ档������ʽ�ı�������ɫ����̫�ɿ��������ػ�ɿ�Щ��
void StaticButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) {
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
	dc_mem.SetBkMode(TRANSPARENT);      // �������ֱ���͸��
	dc_mem.SetTextColor(RGB(0, 0, 0));  // ����������ɫ
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