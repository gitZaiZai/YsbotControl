#pragma once

#include "../IO Dialog/GroupFrame.h"
#include "../BasicControls/IconComboBox.h"
#include "../BasicControls/Label.h"
#include "../BasicControls/buttons.h"
#include "../IO Dialog/StaticTransparent.h"
#include "../ConfigReader.h"

#define IDC_SETTING_DISPLAY                 6040 
#define IDC_SETTING_DISPLAY_COMBBOX         6041 // 6044 

class CSetDisplay :	public CGroupFrame
{
public:
	CSetDisplay(const ConfigReader&);
	virtual ~CSetDisplay(void);
	BOOL Create(CString caption, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd=NULL, UINT nID=65535);

protected:

	DECLARE_MESSAGE_MAP()
	afx_msg void OnCbnSelControlType();
	afx_msg void OnCbnSelTeachType();
	afx_msg void OnCbnSelComPort();

private:
	CIconComboBox m_combbox[4];
	CStaticTran titleLabel[4];
	 
	unsigned int m_nID;
	CFont font_;
	std::string strControlType;
	std::string strTeachPendantType;
	unsigned int nPortTeachPendant;

	ConfigReader& cfg;
};

