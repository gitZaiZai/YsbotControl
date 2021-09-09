#pragma once

class StaticButton : public CStatic {
public:
	StaticButton();
	~StaticButton();

protected:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

private:
	CFont font_;
	COLORREF backcolor_;
};