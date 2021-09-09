#pragma once

#include "LoadPngImage.h"
//////////////////////////////////////////////////////////////////////////  

#define DEF_TEXT_FRAME_COLOR            RGB(255,255,255)                //默认颜色  
#define DEF_TEXT_COLOR                  RGB(10,10,10)                   //默认颜色  
#define TOOLTIP_ID                      100                             //提示 ID  

//////////////////////////////////////////////////////////////////////////  
// CPngCheckBox

class CPngCheckBox : public CButton
{
	DECLARE_DYNAMIC(CPngCheckBox)

public:
	CPngCheckBox();
	virtual ~CPngCheckBox();

public:  
	BOOL GetChecked() { return m_bChecked; }  
	void SetChecked(BOOL bChecked = TRUE) { m_bChecked = bChecked; PaintParent();Invalidate(FALSE); }  

public:  
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);  
  
protected:  
	virtual void PreSubclassWindow();  

public:  
	// 初始化  
	void Init(UINT nBkImg, UINT nCkImg, UINT nDisableImg, UINT nDisableUnChkImg);  
	void SetCheckBoxTextColor(COLORREF crTextColor);  
	void SetBackColor(COLORREF crBack) { m_crBack = crBack; }  
	//设置颜色  
	bool SetTextColor(COLORREF crTextColor, COLORREF crTextFrameColor = DEF_TEXT_FRAME_COLOR, bool bShowFrame = false);  
	//设置字体大小及类型  
	void SetFontType(int fontSize, CString fontType);  
	Gdiplus::Image *ImageFromResource(HINSTANCE hInstance, UINT uImgID, LPCTSTR lpType);  
	void DrawTextString(CDC * pDC, LPCTSTR pszString, COLORREF crText, COLORREF crFrame, LPRECT lpRect);  
	void DrawTextString(CDC * pDC, LPCTSTR pszString, COLORREF crText, COLORREF crFrame, int nXPos, int nYPos);  
	void PaintParent();  

protected:  
	BOOL                                m_bChecked;  
	Gdiplus::Image*                              m_ImageCheck;  
	Gdiplus::Image*                              m_ImageBack;  
	Gdiplus::Image*                              m_ImageDisableCheck;  
	Gdiplus::Image*                              m_ImageDisableUncheck;  

	COLORREF                            m_crBack;  
	COLORREF                            m_crTextColor;  
	CFont                               m_font;  
	BOOL                                m_bShowTextFrame;  
	COLORREF                            m_crTextFrameColor;  
	BOOL                                m_bSetBack;  

	afx_msg BOOL OnClicked();  
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);  
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);  

	DECLARE_MESSAGE_MAP()  
	 
public:
	afx_msg void OnBnDoubleclicked();
};


