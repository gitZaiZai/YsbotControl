#pragma once

#include "../GridCtrl_src/GridCtrl.h"

// CPopupMFCGid

class CPopupMFCGid : public CGridCtrl
{
	DECLARE_DYNCREATE(CPopupMFCGid)

public:
	CPopupMFCGid();
	virtual ~CPopupMFCGid();
protected:
	int m_nKeyColumn;
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
public:
	void SetKeyColumn(int nKeyColumn)
	{
		ASSERT(nKeyColumn>=0 && nKeyColumn<GetColumnCount());
		m_nKeyColumn=nKeyColumn;
	}
	int GetKeyColumn(){return m_nKeyColumn;}
	bool CreatePopupCtrl(CWnd* pOwner);
	int GetCount(void) const;
	int GetCurrentSelection(void) const;
	int SetCurrentSelection(int nSelect);
	void GetItemText(int nIndex,CString& rString) const;
	int DeleteItem(UINT nIndex);
	int FindString(int nStartAfter,LPCTSTR lpszString) const;
	int FindStringExact(int nStartAfter,LPCTSTR lpszString) const;
	void RemoveAllItems();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
};


