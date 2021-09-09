#include "stdafx.h"
#include "draw_frame.h"

void DrawMyFrame(CWindowDC *dc, int item_number, int r, int g, int b) {
	CRect rect;
	dc->GetWindow()->GetWindowRect(&rect);
	// ����߶ȣ�����ǹ̶��ģ��߶ȸ�����Ŀ����������
	int height = 4 + 40 * item_number + 4 * (item_number - 1) + 5;
	int width = rect.Width();
	// �����߿�ɫ��
	COLORREF color_top[] = {
		RGB(r + 2, g + 2, b + 2), RGB(r + 17, g + 17, b + 17),
		RGB(r + 27, g + 27, b + 27), RGB(r + 13, g + 13, b + 13)
	};
	// �ײ��߿�ɫ��
	COLORREF color_bottom[] = {
		RGB(r - 56, g - 56, b - 56), RGB(r - 114, g - 114, b - 114),
		RGB(r - 91, g - 91, b - 91), RGB(r - 58, g - 58, b - 58),
		RGB(r - 27, g - 27, b - 27)
	};
	// ���߿�ɫ��
	COLORREF color_left = RGB(r + 25, g + 25, b + 25);
	// �Ҳ�߿�ɫ��
	COLORREF color_right[] = {
		RGB(r - 6, g - 6, b - 6), RGB(r - 87, g - 87, b - 87)
	};
	// �ָ���ɫ��
	COLORREF color_split[] = {
		RGB(r - 38, g - 38, b - 38), RGB(r + 6, g + 6, b + 6),
		RGB(r + 35, g + 35, b + 35), RGB(r - 66, g - 66, b - 66)
	};
	// �������߿�
	DrawMyLine(dc, color_left, 0, 0, 0, height);
	// �����Ҳ�߿�
	DrawMyLine(dc, color_right[0], width - 2, 0, width - 2, height);
	DrawMyLine(dc, color_right[1], width - 1, 0, width - 1, height);
	// ���ƶ����߿��������»���
	for (unsigned i = 0; i != 4; i++) {
		DrawMyLine(dc, color_top[i], 0, i, width, i);
	}
	// ���Ƶײ��߿��������ϻ���
	for (unsigned i = 0; i != 5; i++) {
		DrawMyLine(dc, color_bottom[i], 0, height - i - 1, width, height - i - 1);
	}
	
	// �����м�ָ���
	for (unsigned i = 0; i != item_number - 1; i++) {
		int x0 = 6;
		int y0 = 44 * (i + 1);
		// 1����������ɫ
		CBrush brush(RGB(r, g, b));
		dc->FillRect(CRect(1, y0, 5, y0 + 4), &brush);
		// 2���������߿�
		dc->SetPixel(x0, y0, RGB(r - 4, g - 4, b - 4));
		dc->SetPixel(x0, y0 + 1, RGB(r - 45, g - 45, b - 45));
		dc->SetPixel(x0, y0 + 2, RGB(r - 59, g - 59, b - 59));
		dc->SetPixel(x0, y0 + 3, RGB(r - 7, g - 7, b - 7));
		// 3�������м����
		DrawMyLine(dc, color_split[0], x0 + 1, y0, x0 - 10 + width, y0);
		DrawMyLine(dc, color_split[1], x0 + 1, y0 + 1, x0 - 10 + width, y0 + 1);
		DrawMyLine(dc, color_split[2], x0 + 1, y0 + 2, x0 - 10 + width, y0 + 2);
		DrawMyLine(dc, color_split[3], x0 + 1, y0 + 3, x0 - 10 + width, y0 + 3); // 
 		// 4�������Ҳ�߿�
		x0 = width - 7;
		dc->SetPixel(x0, y0, RGB(r - 1, g - 1, b - 1));
		dc->SetPixel(x0, y0 + 1, RGB(r - 49, g - 49, b - 49));
		dc->SetPixel(x0, y0 + 2, RGB(r - 67, g - 67, b - 67));
		dc->SetPixel(x0, y0 + 3, RGB(r - 1, g - 1, b - 1));
		// 5�������Ҳ��ɫ
  		dc->FillRect(CRect(width - 6, y0, width, y0 + 4), &brush); // liurundan
	}

	return;
}

void DrawMyLine(
	CWindowDC *dc, COLORREF color, int x0, int y0, int x1, int y1) {
		CPen pen(PS_SOLID, 1, color);
		CPen *old_pen = dc->SelectObject(&pen);
		dc->SelectObject(pen);
		dc->MoveTo(x0, y0);
		dc->LineTo(x1, y1);
		dc->SelectObject(old_pen);
		pen.DeleteObject();
		return;
}