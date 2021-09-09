
#pragma once

#include "../IO Dialog/StaticTransparent.h"
#include "../IO Dialog/GroupFrame.h"
#include "../BasicControls/Label.h"
#include "../BasicControls/CGroupBox.h"

#define IDC_TOOL_PIC_ILLUSTRATION  4690
#define  MAX_TOOL_METHOD      (4)
#define  MAX_TOOL_STEP        (6)

class CToolPicture : public CGroupFrame
{
public:
	CToolPicture();
	virtual ~CToolPicture();
	BOOL Create(CString caption, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd=NULL, UINT nID=65535);
	void update_illustration(int iMethod, int iStep);

protected:
 	DECLARE_MESSAGE_MAP()

private:
	CLabel m_RegionPic;
	CFont font_;
	unsigned int _nID;

	CLabel _StepText;
 	CGroupBox   m_GroupBoxStep;

	CString m_strBmpName[MAX_TOOL_METHOD][MAX_TOOL_STEP];
	CString m_strStepNum[MAX_TOOL_METHOD][MAX_TOOL_STEP];
 
};