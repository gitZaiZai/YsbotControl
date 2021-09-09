#pragma once

// 派生自CStatic的图标类
class StaticIcon : public CStatic {
	DECLARE_DYNAMIC(StaticIcon)
public:
	StaticIcon();
	virtual ~StaticIcon();
	void SetBmp(int image);  // 设置图像ID与装载图像
	void ShowBmp( int image); // 使用双缓存显示图片
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