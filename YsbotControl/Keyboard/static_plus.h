#pragma once

// CStatic������
class StaticPlus : public CStatic {
public:
	StaticPlus(void);
	~StaticPlus(void);
	// ��������
	// ���룺height ����߶ȣ�width �����ȣ�Ϊ0ʱ��ʾ���ո߶����Զ����ÿ�ȣ�
	//   name �������ƣ�weigth ���������italic б�壬underline �»��ߡ�
	void SetMyFont(int height = 18, int width = 0, CString name = _T("Consolas"),
		int weight = FW_HEAVY, BYTE italic = FALSE, BYTE underline = FALSE);
	// ������ɫ����
	inline void set_text_color(COLORREF color) {text_color_ = color;}
	// ���ֱ���ɫ����
	inline void set_backcolor(COLORREF color) {backcolor_ = color;}
	// ����͸������
	inline void set_transparent(BOOL t1) {transparent_ = t1;}

protected:
	afx_msg void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
  virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
  DECLARE_MESSAGE_MAP()

private:
	CFont *font_;          // ����
  COLORREF text_color_;  // ������ɫ
	COLORREF backcolor_;   // ���ֵ�ɫ
	BOOL transparent_;     // �Ƿ�͸��
};