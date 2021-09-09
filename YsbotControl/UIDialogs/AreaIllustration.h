//////////Ã∆è™è™è™è™

#pragma once


#include "../IO Dialog/StaticTransparent.h"
#include "../IO Dialog/GroupFrame.h"
#include "../BasicControls/Label.h"

#define IDC_SecurArea_Illustration 5040

class CAreaIllustration:public CGroupFrame
{
public:
	CAreaIllustration();
	virtual ~CAreaIllustration();
	BOOL Create(CString caption, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd=NULL, UINT nID=65535);
	void update_picture(int shape,int type);

protected:
 	DECLARE_MESSAGE_MAP()

private:
	CLabel m_AreaPic;
	unsigned int _nID;


};
