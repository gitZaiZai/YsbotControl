#pragma once

// CStatic增加类
class StaticPlus : public CStatic {
public:
	StaticPlus(void);
	~StaticPlus(void);
	// 字体设置
	// 输入：height 字体高度，width 字体宽度（为0时表示按照高度来自动设置宽度）
	//   name 字体名称，weigth 字体磅数，italic 斜体，underline 下划线。
	void SetMyFont(int height = 18, int width = 0, CString name = _T("Consolas"),
		int weight = FW_HEAVY, BYTE italic = FALSE, BYTE underline = FALSE);
	// 文字颜色设置
	inline void set_text_color(COLORREF color) {text_color_ = color;}
	// 文字背景色设置
	inline void set_backcolor(COLORREF color) {backcolor_ = color;}
	// 文字透明设置
	inline void set_transparent(BOOL t1) {transparent_ = t1;}

protected:
	afx_msg void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
  virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
  DECLARE_MESSAGE_MAP()

private:
	CFont *font_;          // 字体
  COLORREF text_color_;  // 文字颜色
	COLORREF backcolor_;   // 文字底色
	BOOL transparent_;     // 是否透明
};