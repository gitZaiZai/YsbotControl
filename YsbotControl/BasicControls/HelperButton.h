#pragma once

#include "BtnST.h"

// CHelperButton

class CHelperButton : public CButtonST
{
	DECLARE_DYNAMIC(CHelperButton)

public:
	CHelperButton();
	virtual ~CHelperButton();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(  UINT nFlags, CPoint point);
};
