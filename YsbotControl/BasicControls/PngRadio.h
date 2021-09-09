#pragma once

#include <vector>  
//////////////////////////////////////////////////////////////////////////  
#include <atlimage.h>

#define DEF_TEXT_FRAME_COLOR            RGB(255,255,255)                //默认颜色  
#define DEF_TEXT_COLOR                  RGB(10,10,10)                   //默认颜色  
#define TOOLTIP_ID                      100                             //提示 ID  
// CPngRadio

//////////////////////////////////////////////////////////////////////////  
class CPngRadio : public CButton  
{  
public:  
	CPngRadio();  
	virtual ~CPngRadio();  

public:  
	// 初始化背景图片，及选中图片  
	void Init( UINT nBkImg, UINT nCkImg, UINT nUnCkImg, UINT nDisableImg, UINT nDisableCkImg );  
	void InitImage(LPCTSTR BkPath, LPCTSTR CkPath, LPCTSTR UnCkPath, LPCTSTR DisablePath,  LPCTSTR CkDisablePath);
	void InitIcon(LPCTSTR CkPath, LPCTSTR UnCkPath, LPCTSTR DisablePath,  LPCTSTR CkDisablePath);

	// 获取当前选中状态  
	BOOL GetChecked() { return m_bChecked; }  
	// 设置选中状态  
	void SetChecked(BOOL bChecked = TRUE);  
	// 设置背景颜色  
	void SetBackColor(COLORREF crBack) { m_crBack = crBack; }  
	//设置字体大小及类型  
	void SetFontType(int fontSize, CString fontType);  
	//设置字体颜色  
	bool SetTextColor(COLORREF crTextColor, COLORREF crTextFrameColor = DEF_TEXT_FRAME_COLOR, bool bShowFrame = false);  
	// 添加所有的radio到一组  
	static void AddGroup(std::vector<CPngRadio*> vcRadio);  

protected:  
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);  
	virtual void PreSubclassWindow();  
 	void DrawTextString(CDC * pDC, LPCTSTR pszString, COLORREF crText, COLORREF crFrame, LPRECT lpRect);  
	void DrawTextString(CDC * pDC, LPCTSTR pszString, COLORREF crText, COLORREF crFrame, int nXPos, int nYPos);  
	void SetGroup(std::vector<CPngRadio*> vcRadio);  
	void PaintParent();  
	void GetIconSize(HICON hIcon, int* cx, int* cy);

protected:  
	BOOL                                m_bChecked;  
	CImage                              m_ImageCheck, m_ImageUncheck, m_ImageDisable, m_ImageDisableCheck;  
	CImage                              m_ImageBack;  
	COLORREF                            m_crBack;  
	COLORREF                            m_crTextColor;  
	CFont                               m_font;  
	BOOL                                m_bShowTextFrame;  
	COLORREF                            m_crTextFrameColor;  
	BOOL                                m_bSetBack;  
	std::vector<CPngRadio*>             m_vcRadio;  
	int m_imgWidth;  
	int m_imgHeight;  

	enum { CHECKED, UNCHECKED, DISABLEUNCHECK, DISABLECHECKED };
	HICON                               m_hIcon[4];

	afx_msg BOOL OnClicked();  
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);  
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);  

	DECLARE_MESSAGE_MAP()  
	DECLARE_DYNAMIC(CPngRadio)  
};  

