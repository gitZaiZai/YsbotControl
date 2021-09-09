#pragma once

class CStaticTran :public CStatic
{
	DECLARE_DYNAMIC(CStaticTran)
public:
	void SetFont(int fontHeight=18, CString fontStr=_T("ËÎÌו"));
	CStaticTran();
	~CStaticTran();

protected:
	//afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	DECLARE_MESSAGE_MAP()
	


private:
	CDC m_memDC;
	int FontHeight;
	CString FontStr;
	CFont m_font;
	void UpdateSurface();

public:
//	afx_msg void OnStnClicked();
};

