#pragma once
 
#include "../IO Dialog/StaticTransparent.h"
#include "../IO Dialog/GroupFrame.h"
#include "../Keyboard/PopEdit.h"
#include "../BasicControls/Label.h"
#include "../BasicControls/labels.h"  

#define IDC_UserCoSys_ToolVal 4611		//4611-4619
 
//位姿位置数据框
class CToolCoSysVal : public CGroupFrame
{
public:
	CToolCoSysVal();
	virtual ~CToolCoSysVal();

	BOOL Create(CString caption, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd=NULL, UINT nID=65535);
	void SetData(double* JArray,unsigned int JNum=6);
	void GetData(double* JArray,unsigned int JNum=6);
	void SetReadOnly(BOOL bReadOnly);
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	DECLARE_MESSAGE_MAP()

private:
	CStaticTran label_joint[7];
	CpopEditDouble edit_joint[6];
	CFont font_;
	unsigned int _nID;
	BasicLabel     strUnit[6];
};
