// Copyright 2016, ��˼��Ϣ�Ƽ��������ι�˾
// All right reserved.
//
// Author: letuspray@163.com (Lu Wei)
//
// Yasi Robot Motion Control System - ��˼�������˶�����ϵͳ
//
// ��������ʵ���ļ�

#include "stdafx.h"
#include "redbud_caption.h"

BEGIN_MESSAGE_MAP(RedbudCaption, CStatic)
  ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
END_MESSAGE_MAP()

RedbudCaption::RedbudCaption() 
{
	text_ = _T("");
	text_color_ = RGB(0, 0, 0);
	drag_ = FALSE;
	// ����Ĭ�ϻ��Χ��ʵ��ʹ�����û�Ӧ��������
	area_ = CRect(0, 0, 500, 300);

	m_bUseCustomFont = FALSE;
	m_font.CreateFont(22, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
		OUT_STROKE_PRECIS, CLIP_STROKE_PRECIS, DRAFT_QUALITY, FIXED_PITCH |
		FF_MODERN, _T("Arial"));
}

RedbudCaption::~RedbudCaption() 
{
	brush_.DeleteObject();
	m_font.DeleteObject();
};

void RedbudCaption::SetBkBmp(CString file) 
{
	brush_.DeleteObject();
	CRect rect;
	GetClientRect(&rect);
  // CBitmap���Ͳ���ֱ�Ӽ���ͼƬ�ļ������ʹ��HBITMAP�����ʵ��
  HBITMAP bmp = (HBITMAP)::LoadImage(NULL, file, IMAGE_BITMAP, rect.Width(),
		rect.Height(), LR_LOADFROMFILE | LR_CREATEDIBSECTION);
	CBitmap bitmap;
  bitmap.Attach(bmp);
	brush_.CreatePatternBrush(&bitmap);  // �ѱ���ͼ��Ϊͼ������ʼ����ˢ
	bitmap.DeleteObject();               // �ͷ�λͼ
	return;
}

void RedbudCaption::OnPaint() {
	if (IsIconic()) {

	} else {
		CStatic::OnPaint();
		Draw(text_);
	}
	return;
}

void RedbudCaption::Draw(CString str) 
{
	text_ = str;
	CDC *dc = GetDC();
	CRect rect;
	GetClientRect(&rect);
	dc->FillRect(rect, &brush_);         // ���Ʊ���ͼ
	dc->SetBkMode(TRANSPARENT);          // ���ñ���͸��
	dc->SetTextColor(text_color_);       // ����������ɫ
	rect.left = 10;
	CFont* pOldFont = NULL;
	if ( m_bUseCustomFont  )
	{
		pOldFont = dc->SelectObject(&m_font);
 	}
	dc->DrawText(str, rect, DT_VCENTER | DT_SINGLELINE);// ���Ƶ��д�ֱ�����ַ�
	if(pOldFont)
		dc->SelectObject (pOldFont);
	return;
}

void RedbudCaption::OnLButtonDown(UINT nFlags, CPoint point) {
	drag_ = TRUE;                // ��ק��ʼ
	start_point_ = point;        // ����ԭʼ�������λ�ã�����ڱ��ؼ���
	GetWindowRect(&start_site_); // ��ȡԭʼλ�ã����ؼ��ڸ��Ի����е�λ�ã�
	return;
}

// ����ƶ�
void RedbudCaption::OnMouseMove(UINT nFlags, CPoint point) {
	if (drag_ == FALSE) return;
	// ͨ���µ���ԭ�����ƫ����
	int offset_x = point.x - start_point_.x;
	int offset_y = point.y - start_point_.y;
	// ͨ��ƫ���������µ����Ͻ�����
	int new_x = start_site_.left + offset_x;
	int new_y = start_site_.top + offset_y;
	// �����趨�Ļ�������ƶԻ���Ļ��Χ
	new_x = (new_x <= area_.left) ? area_.left : new_x;
	new_y = (new_y <= area_.top) ? area_.top : new_y;
	new_x = (new_x >= area_.right) ? area_.right : new_x;
	new_y = (new_y >= area_.bottom) ? area_.bottom : new_y;
	// ����ƫ�����޸ĸ��Ի���λ��
	if ( !area_.IsRectEmpty() )
	{
		GetParent()->SetWindowPos(NULL, new_x, new_y,	0, 0, SWP_NOSIZE);
	}
	// ˢ��ԭʼλ��
	GetWindowRect(&start_site_);

	// ͨ�������������ǾͿ���֪����קһ�����ڣ��ر�����קʱ��Ҫ��ʾ��������ʱ��
	// ϵͳ��Ҫ���������ǱȽ϶�ģ���˶�CPU��ռ��ҪԶ����̬��ʾ��
	// ��Windowsϵ�еĲ���ϵͳ�е����������о���һ��ѡ��϶�ʱ��ʾ�������ݡ�

	// ������Լ�����ٶ��ƶ������ؼ�֮��ʱ���ؼ����ܼ�ʱ������Ӧ��
	// ������ٻص����ؼ���ʱ����������У���Ӱ��ʹ�ã�ֻ����Щ��ˬ
	// MFC��Ȼ��WM_MOUSELEAVE�����������Զ���Ӧ����Ҫ�û����ã���������
	TRACKMOUSEEVENT tme;
	tme.cbSize = sizeof(TRACKMOUSEEVENT);
	tme.dwFlags = TME_LEAVE;
	tme.hwndTrack = this->m_hWnd;

	if (::_TrackMouseEvent(&tme)) {
		mouse_tracking_ = TRUE;
	}
	return;
}

void RedbudCaption::OnLButtonUp(UINT nFlags, CPoint point) {
	drag_ = FALSE;   // �ſ�����������ק����
	return;
}

// ������뿪ʱ������ק״̬
LRESULT RedbudCaption::OnMouseLeave(WPARAM wParam, LPARAM lParam) {
	mouse_tracking_ = FALSE;
	drag_ = FALSE;
	return TRUE;
}

// ���������
void RedbudCaption::SetArea(CRect rect) {
	area_= rect;
	// ���������û�����Լ�������ƶ�ʱ�ļ�����
	// 1����ȡ���Ի����λ�ô�С
	GetParent()->GetWindowRect(&rect);
	// 2�����½Ǻ������ȥ��Ȼ�����ϽǺ�����Ŀɻ��Χ
	area_.right = area_.right - rect.Width();
	// 3�����½��������ȥ�߶Ȼ�����Ͻ�������Ŀɻ��Χ
	area_.bottom = area_.bottom - rect.Height();
	return;
}