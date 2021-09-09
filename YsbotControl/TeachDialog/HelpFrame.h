#pragma once


#include "../IO Dialog/GroupFrame.h"
#include "../Keyboard/PopEdit.h" 

class CHelpFrame: public CGroupFrame
{
public:
	CHelpFrame();
	virtual ~CHelpFrame();

	BOOL Create(CString caption, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd=NULL, UINT nID=65535);

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()

private:
	CStatic picture_[3];
	CEdit text_;
	unsigned int _nID;

};
