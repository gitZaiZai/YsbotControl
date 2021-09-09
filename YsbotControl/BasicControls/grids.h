#pragma once

class BasicGrid : public CListCtrl {
private:
	CFont font_;
	UINT m_nItemHeight;
public:
	BasicGrid();
	void SetMyFont();
	void SetItemHeight(UINT nHeight);
	DECLARE_MESSAGE_MAP()
	virtual void PreSubclassWindow();

	afx_msg void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
};

class BasicTree : public CTreeCtrl {
private:
	

public:
	CFont font_;
	BasicTree();
	void SetMyFont();

protected:

};
