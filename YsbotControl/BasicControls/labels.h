#pragma once
#include "../colourcode.h"

// 派生自CStatic的文本类
class BasicLabel : public CStatic {
	DECLARE_DYNAMIC(BasicLabel)
public:
	CFont font_;

	BasicLabel();
	virtual ~BasicLabel();
	// 初始化标签
	void Initialize(COLORREF forecolor = kColor[0], int mode = TRANSPARENT,
		COLORREF backcolor = kColor[1], int font_height = 20,
		int font_width = 0, CString font_name = _T("Arial"));
	void SetForeColor(COLORREF color);             // 设置文字颜色
	void SetBackColor(COLORREF color);             // 设置背景颜色
	void SetBackMode(int mode);                    // 设置背景效果
// 	void SetFontSize(int height, int width = 0);   // 设置字体高度与宽度
	//设置字体大小及类型  
	void SetFontType(int fontSize,int width, CString fontType);  

	void SetFontName(CString name);                // 设置字体名称
	void SetAlignment(int align);                  // 设置对齐方式，1左2右3居中
	// 使用双缓存显示文字（输入：要显示的文字）
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
