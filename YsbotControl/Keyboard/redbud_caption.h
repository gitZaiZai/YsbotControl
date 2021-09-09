// Copyright 2016, ��˼��Ϣ�Ƽ��������ι�˾
// All right reserved.
//
// Author: letuspray@163.com (Lu Wei)
//
// Yasi Robot Motion Control System - ��˼�������˶�����ϵͳ
//
// ��������ͷ�ļ�

#pragma once

class RedbudCaption : public CStatic {
public:
  RedbudCaption(void);
  ~RedbudCaption(void);
  void SetBkBmp(CString file); // ���ñ���ͼƬ�ļ�
	void Draw(CString str);      // ���Ʊ���ͼ������
	void SetArea(CRect rect);    // ���û����
	inline CString text(void) {return text_;}
	inline void set_text(CString str) {text_ = str;}
	inline void text_color(COLORREF color) {text_color_ = color;} // �����ַ���ɫ
	void set_custom_font(BOOL b) { m_bUseCustomFont = b; }

protected:
  afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
  DECLARE_MESSAGE_MAP()

private:
	CBrush brush_;        // �汳��ͼ��ˢ
	CString text_;        // Ҫ��ʾ���ַ�
	COLORREF text_color_; // �ַ���ɫ
	BOOL drag_;           // ��ק״̬
	CPoint start_point_;  // ��ק����ʼ��
	CRect start_site_;    // ��ק����ʼλ��
	CRect area_;          // �����
	BOOL mouse_tracking_; // ���״̬
	CFont m_font;
	BOOL m_bUseCustomFont;
};