#pragma once

#include <vector>  
//////////////////////////////////////////////////////////////////////////  
#include <atlimage.h>

#define DEF_TEXT_FRAME_COLOR            RGB(255,255,255)                //Ĭ����ɫ  
#define DEF_TEXT_COLOR                  RGB(10,10,10)                   //Ĭ����ɫ  
#define TOOLTIP_ID                      100                             //��ʾ ID  
// CPngRadio

//////////////////////////////////////////////////////////////////////////  
class CPngRadio : public CButton  
{  
public:  
	CPngRadio();  
	virtual ~CPngRadio();  

public:  
	// ��ʼ������ͼƬ����ѡ��ͼƬ  
	void Init( UINT nBkImg, UINT nCkImg, UINT nUnCkImg, UINT nDisableImg, UINT nDisableCkImg );  
	void InitImage(LPCTSTR BkPath, LPCTSTR CkPath, LPCTSTR UnCkPath, LPCTSTR DisablePath,  LPCTSTR CkDisablePath);
	void InitIcon(LPCTSTR CkPath, LPCTSTR UnCkPath, LPCTSTR DisablePath,  LPCTSTR CkDisablePath);

	// ��ȡ��ǰѡ��״̬  
	BOOL GetChecked() { return m_bChecked; }  
	// ����ѡ��״̬  
	void SetChecked(BOOL bChecked = TRUE);  
	// ���ñ�����ɫ  
	void SetBackColor(COLORREF crBack) { m_crBack = crBack; }  
	//���������С������  
	void SetFontType(int fontSize, CString fontType);  
	//����������ɫ  
	bool SetTextColor(COLORREF crTextColor, COLORREF crTextFrameColor = DEF_TEXT_FRAME_COLOR, bool bShowFrame = false);  
	// ������е�radio��һ��  
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

