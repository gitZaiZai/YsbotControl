// Copyright 2016, 亚思信息科技有限责任公司
// All right reserved.
//
// Author: letuspray@163.com (Lu Wei)
//
// Yasi Robot Motion Control System - 亚思机器人运动控制系统
//
// 标题栏类头文件

#pragma once

class RedbudCaption : public CStatic {
public:
  RedbudCaption(void);
  ~RedbudCaption(void);
  void SetBkBmp(CString file); // 设置背景图片文件
	void Draw(CString str);      // 绘制背景图与文字
	void SetArea(CRect rect);    // 设置活动区域
	inline CString text(void) {return text_;}
	inline void set_text(CString str) {text_ = str;}
	inline void text_color(COLORREF color) {text_color_ = color;} // 设置字符颜色
	void set_custom_font(BOOL b) { m_bUseCustomFont = b; }

protected:
  afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
  DECLARE_MESSAGE_MAP()

private:
	CBrush brush_;        // 绘背景图画刷
	CString text_;        // 要显示的字符
	COLORREF text_color_; // 字符颜色
	BOOL drag_;           // 拖拽状态
	CPoint start_point_;  // 拖拽的起始点
	CRect start_site_;    // 拖拽的起始位置
	CRect area_;          // 活动区域
	BOOL mouse_tracking_; // 鼠标状态
	CFont m_font;
	BOOL m_bUseCustomFont;
};