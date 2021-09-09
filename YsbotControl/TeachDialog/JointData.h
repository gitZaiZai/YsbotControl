#pragma once

 
#include "../IO Dialog/GroupFrame.h"
#include "../Keyboard/PopEdit.h" 
#include "../BasicControls/labels.h" 

//----------内部子框类-----------
//关节位置数据框
class CJointData : public CGroupFrame
{
public:
	CJointData();
	virtual ~CJointData();

	BOOL Create(CString caption, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd=NULL, UINT nID=65535);
	void SetData(double* JArray,unsigned int JNum=8);
	void GetData(double* JArray,unsigned int JNum=8);

	void SetReadOnly(BOOL bReadOnly);
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	DECLARE_MESSAGE_MAP()

private:
	CStaticTran label_joint[8];
	CpopEdit edit_joint[8];
	BasicLabel     strUnit[8];

	CFont font_;
	unsigned int _nID;
};

 