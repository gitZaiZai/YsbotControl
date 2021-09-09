#if !defined(AFX_GUIGROUPBOX_H__C7AAD4CA_1B37_42E8_8F08_AEF18BA77DFD__INCLUDED_)
#define AFX_GUIGROUPBOX_H__C7AAD4CA_1B37_42E8_8F08_AEF18BA77DFD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
 
class CGroupBox : public CButton
{
	// Construction
public:
	CGroupBox();
	enum Aling{ALING_LEFT=0,ALING_RIGHT=1,ALING_DOWN=2, ALING_UP=3};
	// Attributes
public:
	void SetStyle(Aling AlingStyle);
	void SetCaption(CString Caption);
	Aling m_style;
	COLORREF  m_clrface;
	COLORREF  m_clrShadow;
	CString   m_Caption;
	// Operations
public:
	virtual void PreSubclassWindow();
public:
	virtual ~CGroupBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(CGuiGroupBox)
	afx_msg void OnPaint();
	afx_msg void OnSysColorChange();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GUIGROUPBOX_H__C7AAD4CA_1B37_42E8_8F08_AEF18BA77DFD__INCLUDED_)