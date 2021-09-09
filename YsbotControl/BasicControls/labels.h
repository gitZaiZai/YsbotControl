#pragma once
#include "../colourcode.h"

// ������CStatic���ı���
class BasicLabel : public CStatic {
	DECLARE_DYNAMIC(BasicLabel)
public:
	CFont font_;

	BasicLabel();
	virtual ~BasicLabel();
	// ��ʼ����ǩ
	void Initialize(COLORREF forecolor = kColor[0], int mode = TRANSPARENT,
		COLORREF backcolor = kColor[1], int font_height = 20,
		int font_width = 0, CString font_name = _T("Arial"));
	void SetForeColor(COLORREF color);             // ����������ɫ
	void SetBackColor(COLORREF color);             // ���ñ�����ɫ
	void SetBackMode(int mode);                    // ���ñ���Ч��
// 	void SetFontSize(int height, int width = 0);   // ��������߶�����
	//���������С������  
	void SetFontType(int fontSize,int width, CString fontType);  

	void SetFontName(CString name);                // ������������
	void SetAlignment(int align);                  // ���ö��뷽ʽ��1��2��3����
	// ʹ��˫������ʾ���֣����룺Ҫ��ʾ�����֣�
	void ShowText(CString str, COLORREF text_color = kColor[1], 
		COLORREF backcolor = kColor[2]);

protected:
	DECLARE_MESSAGE_MAP()
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	afx_msg void OnPaint();

private:
	int alignment_;
	int font_height_;
	int font_width_;
	int mode_;
	CString font_name_;
	COLORREF forecolor_;
	COLORREF backcolor_;

	CRect rect_;
	CDC *dc_;
	CDC mem_dc_;
	CBitmap bitmap_;
};
