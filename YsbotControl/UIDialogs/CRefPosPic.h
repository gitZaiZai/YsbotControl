
#pragma once
 
#include "../IO Dialog/StaticTransparent.h"
#include "../IO Dialog/GroupFrame.h"
#include "../BasicControls/Label.h"
#include "../BasicControls/CGroupBox.h"

 
#define IDC_UserCoSys_PosPicture 4632
#define  MAX_TEACHUSER_METHOD (3)
#define  MAX_TEACHUSER_STEP   (5)

class CRefPosPic : public CGroupFrame
{
public:
	CRefPosPic();
	virtual ~CRefPosPic();
	BOOL Create(CString caption, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd=NULL, UINT nID=65535);
	void update_picture_region(int iMethod, int iStep);

protected:
 	DECLARE_MESSAGE_MAP()

private:
 	CLabel m_RegionPic;
 	CFont font_;
	unsigned int _nID;
 
	CLabel      _StepText;
 	CGroupBox   m_GroupBoxStep;
	CString m_strBmpName[MAX_TEACHUSER_METHOD][MAX_TEACHUSER_STEP];
	CString m_strStepNum[MAX_TEACHUSER_METHOD][MAX_TEACHUSER_STEP];
};