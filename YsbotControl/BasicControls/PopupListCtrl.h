#pragma once


// CPopupListCtrl
class CPopupListCtrl : public CListCtrl
{
	DECLARE_DYNCREATE(CPopupListCtrl)

public:
	CPopupListCtrl();
	virtual ~CPopupListCtrl();
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

public:
	bool		CreatePopupCtrl(CWnd* pOwner);
	int			GetCount(void) const;
	int			GetCurrentSelection(void) const;
	int			SetCurrentSelection(int nSelect);
	void		GetItemText(int nIndex,CString& rString) const;
	int			DeleteItem(UINT nIndex);
	int			FindString(int nStartAfter,LPCTSTR lpszString) const;
	int			FindStringExact(int nStartAfter,LPCTSTR lpszString) const;
	void		RemoveAllItems();

	DECLARE_MESSAGE_MAP()
};


