// CustomComboBoxDlg.h : header file
//

#pragma once
#include "afxwin.h"
 
#include "afxcmn.h"

// CCustomComboBoxDlg dialog
class CPopupDlgCtrl : public CDialog
{
	DECLARE_DYNCREATE(CPopupDlgCtrl)
	// Construction
public:
	CPopupDlgCtrl(CWnd* pParent = NULL);	// standard constructor
	virtual ~CPopupDlgCtrl();
	// Dialog Data
	enum { IDD = IDD_CUSTOM_POPUP_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	// Implementation
protected:
	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
	bool CreatePopupCtrl(CWnd* pOwner);
protected:
	virtual void OnCancel();
	virtual void OnOK();
public:
	int GetCount(void)const;
	int GetCurrentSelection(void) const;
	int SetCurrentSelection(int nSelect);
	void GetItemText(int nIndex,CString& rString) const;
	int DeleteItem(UINT nIndex);
	int FindString(int nStartAfter,LPCTSTR lpszString) const;
	int FindStringExact(int nStartAfter,LPCTSTR lpszString) const;
	void RemoveAllItems();
	
	CSliderCtrl m_slder;
};