#pragma once

 
#include "../IO Dialog/GroupFrame.h"
#include "../Keyboard/PopEdit.h" 
#include "../BasicControls/labels.h"

//位姿位置数据框
class CPoseData : public CGroupFrame
{
public:
	CPoseData();
	virtual ~CPoseData();

	BOOL Create(CString caption, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd=NULL, UINT nID=65535);
	void SetData(double* PArray,unsigned int JNum=8);
	void GetData(double* PArray,unsigned int JNum=8);
	void SetMode(int pmode);
	int GetMode();
	void SetReadOnly(BOOL bReadOnly);
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	DECLARE_MESSAGE_MAP()

private:
	CStaticTran    label_joint[9];
	CpopEditDouble edit_joint[9];
  	CFont font_;
	unsigned int _nID;
	BasicLabel     strUnit[9];

};