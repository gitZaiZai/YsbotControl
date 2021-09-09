#pragma once

// ������CStatic��ͼ����
class StaticIcon : public CStatic {
	DECLARE_DYNAMIC(StaticIcon)
public:
	StaticIcon();
	virtual ~StaticIcon();
	void SetBmp(int image);  // ����ͼ��ID��װ��ͼ��
	void ShowBmp( int image); // ʹ��˫������ʾͼƬ
	void SetLabelBkColor(COLORREF col) { bgcolor = col; }

private:
	CBitmap bitmap_;
	CRect rect_;
	CDC *dc_;
	CDC mem_dc_;
	COLORREF bgcolor;
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	DECLARE_MESSAGE_MAP()
};