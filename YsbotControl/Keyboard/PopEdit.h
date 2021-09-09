// Copyright 2016, 亚思信息科技有限责任公司
// All right reserved.
//
// Author: Liu Run Dan ( lrdcz@163.com )
//
// Yasi Robot Motion Control System - 亚思机器人运动控制系统

#pragma once
 
#include "digital_keyboard.h"
#include "full_qwerty.h"

class CpopEdit : public CEdit
{
	DECLARE_DYNAMIC(CpopEdit)
public:
	virtual ~CpopEdit();
	CpopEdit();

	BOOL Create( DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, BOOL bFullKeyboard = FALSE );
	void SetKB(DigitalKeyboard* kb) {KB_=kb;}; 
	void SetValueRange(double minVal,double maxVal,short decimals); 
	void SetText( LPCTSTR lpszString )  { CWnd::SetWindowText(lpszString); }
	void SetRulePlan(unsigned short ruleIndex) ;
	void set_rule(CString rule) ;
	void SetHighLightText(BOOL b) { m_bHighLightText = b; }
    void SetAllowPopupNested(BOOL b) { m_bAllowPopupNested = b; }

protected:
	void refreshValue();
// 	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
// 	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);

	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnUpdate();
	afx_msg BOOL OnChangeReflect();
	afx_msg void OnPaint();
	virtual afx_msg LRESULT OnKeyboardEnter(WPARAM,LPARAM); 
	virtual afx_msg LRESULT OnKeyboardCancle(WPARAM,LPARAM);

	DECLARE_MESSAGE_MAP()

private:
	void run_keyboard();
	virtual CString getValstr() { return _T(""); }
	virtual bool setValstr(CString s) { return false;}

private:
	DigitalKeyboard *KB_;
	double MinVal, MaxVal;
	short Decimals;

	bool				m_userUpdate;
	bool				m_allowEnChange;
	BOOL                m_useFullKeyboard;
	BOOL                m_bHighLightText;
	BOOL                m_bAllowPopupNested;

	CBrush      m_BrushEnable;
	CBrush      m_BrushDisable;
	CBrush      m_BrushFrame;
	CBrush      m_BrushTextBk;
	 
	int         m_id;
};

class CpopEditLong : public CpopEdit
{
public:
	CpopEditLong() : CpopEdit() { }

	void SetValue(long value);
	long GetValue()						{ return m_value; }

private:
	CString getValstr();
	bool setValstr(CString s);
	long				m_value;
};


class CpopEditDouble : public CpopEdit
{
public:
	CpopEditDouble() : CpopEdit() { iDecimalLoc = 3; }

	void SetValue(double value);
	double GetValue()						{ return m_value; }
	void SetDecimalLoc(int ipos) { iDecimalLoc = ipos; }
private:
	CString getValstr();
	bool setValstr(CString s);
	double				m_value;
	int iDecimalLoc;
};