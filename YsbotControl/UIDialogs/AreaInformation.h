//////////Ã∆è™è™è™è™

#pragma once


#include "../IO Dialog/StaticTransparent.h"
#include "../IO Dialog/GroupFrame.h"
#include "../BasicControls/IconComboBox.h"
#include "../BasicControls/Label.h"
#include "../BasicControls/PngButton.h"
#include "../BasicControls/PngCheckBox.h"
#include "SecurArea.h"

#define IDC_SecurArea_Information 5010
#define IDC_AreaInfo_COMBBOX      5011
#define WM_SelChangeAreaName      5012
#define IDC_AreaInfo_CHECK        5013

class CAreaInformation:public CGroupFrame
{
public:
	CAreaInformation();
	virtual ~CAreaInformation();
	BOOL Create(CString caption, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd=NULL, UINT nID=65535);
	void get_area_information(SecurArea*);
	void update_areainfo_show(const SecurArea&);
	void update_area_state(bool);

	CIconComboBox Area_name;

protected:
 	afx_msg void OnSelchangeArea();

	DECLARE_MESSAGE_MAP()

private:
	unsigned int _nID;
	CStaticTran AreaLabel[3];
	CLabel ActivePic;
	CLabel StatePic;
	CPngCheckBox active_check;
	CString stateBmpName[3];
	unsigned int nStateBmp;
};